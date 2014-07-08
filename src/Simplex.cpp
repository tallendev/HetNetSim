/**
 * This is the implementation of the Simplex class. It solves linear 
 * programming problems using the two-phase simplex method. Users of the class 
 * should be able to instantiate a Simplex obect with a LinearProgram object
 * and call solve() on it, which returns a pointer to an LPSolution object.
 * For details on the algorithm, see the documentation.
 *  
 * Version: 07/03/2014
 * Author: Matthew Leeds
 * Author: Tyler Allen
 *
 */

#include "Simplex.h"

/**
 * Constructor for Simplex objects.
 * Takes in a Linear Program, determines the number of constraints,
 * rows, and columns, and calls lpToTable on it to format it as a matrix.
 */
Simplex::Simplex(LinearProgram* lp)
{
    numLeqConstraints = lp->getLeqConstraints()->getSize();
    numEqConstraints = lp->getEqConstraints()->getSize();
    numConstraints = numLeqConstraints + numEqConstraints;

    std::istringstream countVars(lp->getEquation());
    std::string token;
    numDecisionVars = 0;
    while (std::getline(countVars, token, ' '))
    {
        numDecisionVars++;
    }

    numRows = numConstraints + 1;
    numCols = numDecisionVars + numConstraints + 1;
    table = arrayInit2d(numRows, numCols);
    lpToTable (lp); // converts the LP into matrix format 
                    // for more efficient solving

    #ifdef SERVER_DEBUG
        std::cerr << "original matrix" << std::endl;
        displayMatrix(table, numRows, numCols);
    #endif

}

// prints a matrix to stdout for debugging
void Simplex::displayMatrix(double** matrix, int x, int y)
{
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            fprintf(stderr, "%7.2f", matrix[i][j]);
        }

        fprintf(stderr, "\n");
    }

    fprintf(stderr, "\n");
}

/**
 * Finds the greatest common denominator given two numbers.
 * Used in the choose method.
 */
unsigned long long Simplex::gcd(unsigned long long x, unsigned long long y)
{
    while (y != 0)
    {
        unsigned long long t = x % y;
        x = y;
        y = t;
    }

    return x;
}

/**
 * Calculates the combination (n choose k), which is used as
 * an upper limit on the number of iterations the solver is 
 * allowed to complete.
 */
unsigned long long Simplex::choose(int n, int k)
{
    static const unsigned long long MAX_LONG =
        std::numeric_limits<unsigned long long>::max();
    unsigned long long r = 1;

    // check for invalid parameters
    if (k <= n)
    {
        bool overflow = false;

        for (int d = 1; d <= k && !overflow; ++d, --n)
        {
            unsigned long long g = gcd(r, (unsigned long long) d);
            r /= g;
            unsigned long long t = n / (d / g);

            if (r > MAX_LONG / t)
                // overflow error
            {
                overflow = true;
            }
            else
            {
                r *= t;
            }
        }
    }

    return r;
}

/**
 * Converts a LinearProgram into matrix (table) format
 * by splitting it with spaces as the delimiter.
 *
 * Assumes that there are only <= and = constraints, that the objective 
 * equation has no constant argument (just coefficients), and that each 
 * constraint has n + 1 numbers, one for each decision variable and a constant.
 */ 
void Simplex::lpToTable(LinearProgram* lp)
{
    tokenizeToMatrix(lp->getLeqConstraints(), 0);
    tokenizeToMatrix(lp->getEqConstraints(), numLeqConstraints);
    std::istringstream split(lp->getEquation());
    std::string token;

    for (int i = 0; std::getline(split, token, ' '); i++)
    {
        std::istringstream(token) >> table[numConstraints][i];
    }
}

/**
 * A helper method for lpToTable.
 * Takes in a list and starting value and puts each number in the list
 * into the table on the row indicated by the starting value.
 */
void Simplex::tokenizeToMatrix(LinkedList<std::string>* list, int start)
{
    LinkedList<std::string>::ListIterator iterator = list->iterator();
    for (int i = start; i < list->getSize(); i++)
    {
        int j = 0;
        std::istringstream split(iterator.next());
        std::string token;

        while (std::getline(split, token, ' '))
        {
            std::istringstream(token) >> table[i][j++];
        }

        j--;
        table[i][numDecisionVars + numConstraints] = table[i][j];
        table[i][j] = 0;
        table[i][numDecisionVars + i] = 1;
    }

}

/**
 * Pivots a table given a pivot row, pivot column, and the length and 
 * width of the table. This increases the value of the variable indicated by
 * the pivot column (by having it enter the basis). The pivot row indicates
 * the variable that leaves the basis. The pivot is accomplished by dividing
 * every entry in the pivot row by the value at table[pivotRow][pivotCol],
 * and subtracting from every other row a multiple of this new pivot row, 
 * such that they have the value 0 in the pivot column.   
 */
void Simplex::pivot(double** table, int pivotRow, int pivotCol, 
                                    int numRows, int numCols)
{   
    #ifdef SERVER_DEBUG
        std::cerr << "pivoting on row " << pivotRow << " col " << pivotCol << std::endl;
    #endif
    double pivotNumber = table[pivotRow][pivotCol];

    for (int col = 0; col < numCols; col++)
    {
        if (table[pivotRow][col] != 0) // so it doesn't produce -0
        {
            table[pivotRow][col] = table[pivotRow][col] / pivotNumber;
        }
    }

    for (int row = 0; row < numRows; row++)
    {
        if (std::abs(table[row][pivotCol]) > ZERO_TOLERANCE &&
            row != pivotRow)
        {
            double multiple = table[row][pivotCol] /
                              table[pivotRow][pivotCol];

            for (int col = 0; col < numCols; col++)
            {
                table[row][col] = table[row][col] -
                                    (multiple * table[pivotRow][col]);
            }
        }
    }
}

/**
 * Checks if the solution indicated by the table is already a Basic Feasible 
 * Solution, which would mean Phase I (the feasibility check) can be skipped.
 * You can tell this because if there are no equality constraints, 
 * only inequality ones, and all the b-values (the constants) are 
 * positive, just set all decision variables equal to zero and you have 
 * a BFS.
 *
 * When true is returned from this, go through both phases of the simplex
 * method. Otherwise, just use the standard simplex method (Phase II).
 */
bool Simplex::isTwoPhase()
{
    bool twoPhase = true;
    int negBValues = 0; // number of inequalities with b < 0

    if (numEqConstraints == 0)
    {
        for (int i = 0; i < numLeqConstraints; i++)
        {
            if (table[i][numDecisionVars + numConstraints] < 0)
            {
                negBValues++;
            }
        }

        if (negBValues == 0)
        {
            // the problem is trivially feasible
            twoPhase = false;
        }
    }
    return twoPhase;
}


/**
 * Implementation of standard simplex method.
 * This determines if Phase I is necessary using isTwoPhase(), 
 * calls checkFeasibility() if it is. optimize() is then called in either 
 * case to (hopefully) arrive at a final optimal solution, and an LPSolution
 * object is returned based on that.
 */
LPSolution* Simplex::solve()
{
    LPSolution* sol = new LPSolution();
    if (isTwoPhase())
    {
        if (checkFeasibility())
        {
            #ifdef SERVER_DEBUG
                std::cerr << "new matrix" << std::endl;
                displayMatrix(table, numRows, numCols);
            #endif
            optimize(table, sol, numRows, numCols, numRows - 1);
        }
        else
        {
            sol->setErrorCode(LPSolution::INFEASIBLE);
        }
    }
    else
    {
        optimize(table, sol, numRows, numCols, numRows - 1);
    }

    arrayDel2d(table, numConstraints + 1);

    return sol;
}

/**
 * CheckFeasiblity() is Phase I of the Two-Phase Simplex Method. It forms
 * an auxiliary problem to the original by adding "artificial" extra slack
 * variables that ensure it has a Basic Feasible Solution even if the original
 * doesn't. This auxilary problem, which trys to minimize all the artificial
 * variables to zero, is solved using the optimize() method. If the optimal 
 * value is 0, it means the original problem can be solved using the BFS 
 * indicated by the final table. In that case, the appropriate rows and 
 * columns are copied over to the original table. Otherwise, 
 * false is returned.
 */
bool Simplex::checkFeasibility()
{
    // Instantiate a table for the related (auxilary) problem.
    int curRows = numConstraints + numEqConstraints + 2;
    int curColumns = numDecisionVars + (2 * numConstraints) +
                     (2 * numEqConstraints) + 1;
    double** relatedTable = arrayInit2d(curRows, curColumns);

    // Copy the  constraints from the original problem into the 
    // first m rows and n+m columns of relatedTable.
    for (int i = 0; i < numConstraints; i++)
    {
        for (int j = 0; j < numDecisionVars + numConstraints; j++)
        {
            relatedTable[i][j] = table[i][j];
        }

        relatedTable[i][curColumns - 1] = table[i][numDecisionVars + numConstraints];
    }

    // Copy the objective equation over. 
    for (int i = 0; i < numConstraints + numDecisionVars; i++)
    {
        relatedTable[numConstraints + numEqConstraints][i] =
            table[numConstraints][i];
    }

    // Used to keep track of which column needs the next artifical variable.
    int colCounter = numDecisionVars + numConstraints + numEqConstraints;

    // Add an artificial slack variable for each inequality constraint,
    // -1 when b is negative, 1 otherwise.
    for (int i = 0; i < numLeqConstraints; i++)
    {
        if (table[i][numDecisionVars + numConstraints] < 0)
        {
            relatedTable[i][colCounter++] = -1;
        }
        else
        {
            relatedTable[i][colCounter++] = 1;
        }
    }

    int rowCounter = 0;
    // Add artificial variables for each equality constraint, and make a second
    // copy of each with an opposite-valued artificial variable. 
    for (int i = numLeqConstraints; i < numConstraints; i++)
    {
        if (table[i][numDecisionVars + numConstraints] < 0)
        {
            relatedTable[i][colCounter++] = -1;
        }
        else
        {
            relatedTable[i][colCounter++] = 1;
        }

        for (int j = 0; j < curColumns; j++)
        {
            relatedTable[numConstraints + rowCounter][j] = relatedTable[i][j];
        }

        relatedTable[numConstraints + rowCounter]
        [numDecisionVars + numLeqConstraints + rowCounter] = 0;

        relatedTable[numConstraints + rowCounter]
        [numDecisionVars + numConstraints + rowCounter] = -1;

        relatedTable[numConstraints + rowCounter][colCounter + rowCounter] =
        relatedTable[numConstraints + rowCounter][colCounter + rowCounter - 1];

        relatedTable[numConstraints + rowCounter]
                    [colCounter + rowCounter - 1] = 0;

        rowCounter++;
        colCounter++;
    }
    
    // Set the objective equation of the auxiliary problem to have a 1 in 
    // each artificial variable column. We're trying to minimize their sum.
    for (int i = numDecisionVars + numConstraints + numEqConstraints;
             i < curColumns - 1; i++)
    {
        relatedTable[numConstraints + numEqConstraints + 1][i] = 1;
    }
    
    // Multiply rows with negative b-values by -1 to simplify the solving.
    for (int i = 0; i < curRows - 1; i++)
    {
        if (relatedTable[i][curColumns - 1] < 0)
        {
            for (int j = 0; j < curColumns; j++)
            {
                if (relatedTable[i][j] != 0)
                    relatedTable[i][j] *= -1;
            }
        }
    }
    
    // Sum each column and subtract that from the entry in the last row, 
    // the objective equation.
    for (int i = 0; i < curColumns; i++)
    {
        double columnSum = 0;

        for (int j = 0; j < curRows - 2; j++)
        {
            columnSum += relatedTable[j][i];
        }

        relatedTable[curRows - 1][i] -= columnSum;
    }

    #ifdef SERVER_DEBUG
        std::cerr << "related matrix" << std::endl;
        displayMatrix(relatedTable, curRows, curColumns);
    #endif
    // Multiply the objective equation row by -1 so it can be solved as a 
    // maximization.
    for (int i = 0; i < curColumns; i++)
    {
        if (!(std::abs(relatedTable[curRows - 1][i]) < ZERO_TOLERANCE))
            relatedTable[curRows - 1][i] *= -1;
    }

    // Attempt to solve the related problem to find a BFS for the original.
    LPSolution relatedSol;
    optimize(relatedTable, &relatedSol, curRows, curColumns, curRows - 2);
    
    // Multiply the original problem's objective equation by -1. 
    for (int i = 0; i < curColumns - 1; i++)
    {
        if (!(std::abs(relatedTable[curRows - 2][i]) < ZERO_TOLERANCE))
            relatedTable[curRows - 2][i] *= -1;
    }

    // Check if the auxiliary problem's optimal value is 0, which means we
    // found a BFS for the original.
    bool solvable;
    if ((solvable = (relatedSol.getErrorCode() == 0 &&
        std::abs(relatedSol.getZValue()) < ZERO_TOLERANCE)))
    {
        //TODO: check for artifical variables in the basis
        // Transfer the BFS we found to the original table for solving later.
        for (int i = 0; i < numConstraints; i++)
        {
            for (int j = 0; j < numConstraints + numDecisionVars; j++)
            {
                table[i][j] = relatedTable[i][j];
            }

            table[i][numConstraints + numDecisionVars] =
                relatedTable[i][curColumns - 1];
        }
        for (int i = 0; i < numConstraints + numDecisionVars; i++)
        {
            table[numConstraints][i] =
                relatedTable[numConstraints + numEqConstraints][i];
        }

        table[numConstraints][numConstraints + numDecisionVars] =
            relatedTable[numConstraints + numEqConstraints][curColumns - 1];

    }
    arrayDel2d(relatedTable, curRows);
    return solvable;
}

/**
 * The core of the Simplex method. This takes in a table, solution object,
 * the size of the table, and how many of the rows are for constraints
 * (rather than objective equations). It then tries to pivot the table
 * as many times as necessary (under a limit) to increase the z-value (the
 * value of the objective equation). See the documentation for details on 
 * the algorithm used.
 */
void Simplex::optimize(double** table, LPSolution* sol, int curRows, 
                      int curCols, int constraintRows)
{
    double* optimalValues = new double[numDecisionVars]();

    unsigned long long maxIter = choose(curCols, curRows);
    unsigned long long numIter = 0; // number of iterations completed.
    bool stay = true;

    while (numIter < maxIter && stay)
    {
        int pivotCol;
        double maxCoeff = ZERO_TOLERANCE;
		// Determine if the solution is optimal or a pivot is needed.
		for (int col = 0; col < curCols - 1; col++)
		{
			if (table[curRows - 1][col] > maxCoeff)
			{
				maxCoeff = table[curRows - 1][col];
				pivotCol = col;
			}
		}
        if (maxCoeff == ZERO_TOLERANCE)
        {
            sol->setErrorCode(LPSolution::SOLVED);

            #ifdef SERVER_DEBUG
                std::cerr << "solved" << std::endl;
                displayMatrix(table, curRows, curCols);
            #endif
            // evaluate the final matrix for the values of each decision variable
            double bvals[curRows];
            for (int i = 0; i < curRows; i++)
            {
                bvals[i] = table[i][curCols - 1];
            }
            for (int col = 0; col < numDecisionVars; col++)
            {
                bool foundOne = false; // found a 1 in this column
                bool foundNonZero = false; // found a nonzero number (except 1st 1)
                int solutionRow;

                for (int row = 0; row < constraintRows; row++)
                {
                    if (std::abs(table[row][col]) > ZERO_TOLERANCE)
                    {
                        if (table[row][col] != 1 || foundOne)
                        {
                            foundNonZero = true;
                        }
                        else if (table[row][col] == 1)
                        {
                            foundOne = true;
                            solutionRow = row;
                        }
                    }
                }

                if (foundOne && !foundNonZero)
                {
                    optimalValues[col] = bvals[solutionRow];  // table[solutionRow][curCols - 1];
                    //fixes reusing B-values... maybe
                    bvals[solutionRow] = 0; 
                }
            }

            sol->setOptimalValues(optimalValues);

            if (std::abs(table[curRows - 1][curCols - 1]) < ZERO_TOLERANCE)
            {
                sol->setZValue(0);
            }
            else
            {
                sol->setZValue(-1 * table[curRows - 1][curCols - 1]);
            }

            stay = false; // break out of the loop to return
        }
        else
        {
            // Check if all entries in pivotCol are <= 0
            double maxVar = ZERO_TOLERANCE;

            for (int row = 0; row < constraintRows; row++)
            {
                if (table[row][pivotCol] > maxVar)
                {
                    maxVar = table[row][pivotCol];
                }
            }

            if (maxVar == ZERO_TOLERANCE)
            {
                sol->setErrorCode(LPSolution::UNBOUNDED);
                stay = false; // break out of the loop to return
            }
            else
            {
                // Determine pivot row.
                int pivotRow;
                double minRatio = DBL_MAX;

                for (int row = 0; row < constraintRows; row++)
                {
                    if (table[row][pivotCol] > ZERO_TOLERANCE)
                    {
                        if ((table[row][curCols - 1] /
                             table[row][pivotCol]) < minRatio)
                        {
                            minRatio = table[row][curCols - 1] /
                                       table[row][pivotCol];
                            pivotRow = row;
                        }
                    }
                }

                // pivot the table to (hopefully) increase z.
                pivot(table, pivotRow, pivotCol, curRows, curCols);
                numIter++;
                #ifdef SERVER_DEBUG
                    std::cerr << "iter " << numIter << std::endl;
                    displayMatrix(table, curRows, curCols);
                #endif 
            }
        }
    } // end while loop

    if (maxIter - numIter < 2)
    {
        sol->setErrorCode(LPSolution::EXCEEDED_MAX_ITERATIONS);
    }
}



