/**
 *
 */

#include "Solver.h"
#include "utils.h"
#include "float.h"
#include <cmath>
#include <iostream>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string.h>
#include <cstdio>


Simplex::Simplex(LinearProgram* lp)
{
    numLeqConstraints = lp->getLeqConstraints()->getSize();
    numEqConstraints = lp->getEqConstraints()->getSize();
    numConstraints = numLeqConstraints + numEqConstraints;
    numRows = numConstrains + 1;
    numCols = numDecisionVars + numConstrains + 1;
    std::istringstream countVars(lp->getEquation());
    std::string token;
    numDecisionVars = 0;

    while (std::getline(countVars, token, ' '))
    {
        numDecisionVars++;
    }

    double** table = arrayInit2d(numRows, numCols);
    lpToTable (lp, table, &);

    std::cout << "original matrix" << std::endl;
    //displayMatrix(table, &);

}

void Simplex::displayMatrix(double** matrix, int x, int y)
{
    for (unsigned long long i = 0; i < x; i++)
    {
        for (unsigned long long j = 0; j < y; j++)
        {
            printf("%7.2f", matrix[i][j]);
        }

        printf("\n");
    }

    printf("\n");
}

int Simplex::gcd(int x, int y)
{
    while (y != 0)
    {
        unsigned long long t = x % y;
        x = y;
        y = t;
    }

    return x;
}

int Simplex::choose(unsigned long long n, unsigned long long k)
{
    static const unsigned long long MAX_LONG =
        std::numeric_limits<unsigned long long>::max();
    unsigned long long r = 1;

    // check for invalid parameters
    if (k <= n)
    {
        bool overflow = false;

        for (unsigned long long d = 1; d <= k && !overflow; ++d, --n)
        {
            unsigned long long g = gcd(r, d);
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

void Simplex::lpToTable(LinearProgram* lp)
{
    tokenizeToMatrix(lp->getLeqConstraints(), 0);
    tokenizeToMatrix(lp->getEqConstraints(), numLeqConstraints);
    std::istringstream split(lp->getEquation());
    std::string token;

    for (unsigned long long i = 0; std::getline(split, token, ' '); i++)
    {
        std::istringstream(token) >> table[numConstraints][i];
    }
}

void Simplex::tokenizeToMatrix(LinkedList<std::string>* list, int start)
{
    LinkedList<std::string>::ListIterator iterator = list->iterator();
    for (int i = start; i < numLeqConstraints; i++)
    {
        int j = 0;
        std::istringstream split(leqConstraintsIter.next());
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

void Simplex::pivot(int pivotRow, int pivotCol, int numRows, int numCols)
{
    std::cout << "pivoting on row " << pivotRow << " col " << pivotCol << std::endl;
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

bool Simplex::isTwoPhase()
{
    // TODO: check for cycling?
    bool twoPhase = true;
    // If there are only inequality constraints ond no b values < 0, we
    // can use the origin as a BFS instead of using the Two Phase method.
    int negBValues = 0; // number of inequalities with b < 0

    if (numEqConstraints == 0)
    {
        for (unsigned long long i = 0; i < numLeqConstraints; i++)
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
 * Implementation of classic simplex method to solving linear programs.
 * Currently assumes:
 * -the problem is in the correct form
 * -the objective function has no constant argument (just coefficients)
 * -all constraints are <= or =
 * -we are trying to maximize
 */
LPSolution* Simplex::solve(LinearProgram* lp)
{
    LPSolution* sol = new LinearProgram;
    if (isTwoPhase())
    {
        if (checkFeasibility(table))
        {
            std::cout << "new matrix" << std::endl;
            displayMatrix(table);
            optimize(table, sol, false);
        }
        else
        {
            sol.setErrorCode(LPSolution::INFEASIBLE);
        }
    }
    else
    {
        optimize(table, sol, false);
    }

    arrayDel2d(table, numConstraints + 1);

    return sol;
}


bool Solver::checkFeasibility()
{
    // formulate a related solvable problem based on the table and solve it.
    int numRows = numConstraints + numEqConstraints + 2;
    int numColumns = numDecisionVars + (2 * numConstraints) +
                     (2 * numEqConstraints) + 1;
    double** relatedTable = arrayInit2d(numRows, numColumns);

    for (int i = 0; i < numConstraints; i++)
    {
        for (int j = 0; j < numDecisionVars + numConstraints; j++)
        {
            relatedTable[i][j] = table[i][j];
        }

        relatedTable[i][numColumns - 1] = table[i][numDecisionVars + numConstraints];
    }

    for (int i = 0; i < numConstraints + numDecisionVars; i++)
    {
        relatedTable[numConstraints + numEqConstraints][i] =
            table[numConstraints][i];
    }

    int colCounter = numDecisionVars + numConstraints +
                     numEqConstraints;

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

        for (int j = 0; j < numColumns; j++)
        {
            relatedTable[numConstraints + rowCounter][j] = relatedTable[i][j];
        }

        relatedTable[numConstraints + rowCounter]
        [numDecisionVars + numLeqConstraints + rowCounter] = 0;
        relatedTable[numConstraints + rowCounter]
        [numDecisionVars + numConstraints + rowCounter] = -1;
        relatedTable[numConstraints + rowCounter][colCounter + rowCounter] =
            relatedTable[numConstraints + rowCounter][colCounter + rowCounter - 1];
        relatedTable[numConstraints + rowCounter][colCounter + rowCounter - 1] = 0;
        rowCounter++;
        colCounter++;
    }

    for (int i = numDecisionVars + numConstraints + numEqConstraints;
             i < numColumns - 1; i++)
    {
        relatedTable[numConstraints + numEqConstraints + 1][i] = 1;
    }

    for (int i = 0; i < numRows - 1; i++)
    {
        if (relatedTable[i][numColumns - 1] < 0)
        {
            for (int j = 0; j < numColumns; j++)
            {
                if (relatedTable[i][j] != 0)
                    relatedTable[i][j] *= -1;
            }
        }
    }

    for (int i = 0; i < numColumns; i++)
    {
        double columnSum = 0;

        for (int j = 0; j < numRows - 2; j++)
        {
            columnSum += relatedTable[j][i];
        }

        relatedTable[numRows - 1][i] -= columnSum;
    }

    std::cout << "related matrix" << std::endl;
    displayMatrix(relatedTable);
    for (int i = 0; i < numColumns; i++)
    {
        if (!(std::abs(relatedTable[numRows - 1][i]) < ZERO_TOLERANCE))
            relatedTable[numRows - 1][i] *= -1;
    }
    // Attempt to solve the related problem to find a BFS for the original.
    LPSolution relatedSol;
    optimize(relatedTable, &relatedSol, true);
    for (int i = 0; i < numColumns; i++)
    {
        if (!(std::abs(relatedTable[numRows - 2][i]) < ZERO_TOLERANCE))
            relatedTable[numRows - 2][i] *= -1;
    }
    bool solvable;
    if (solvable = relatedSol.getErrorCode() == 0 &&
        std::abs(relatedSol.getZValue()) < ZERO_TOLERANCE)
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
                relatedTable[i][numColumns - 1];
        }
        for (int i = 0; i < numConstraints + numDecisionVars; i++)
        {
            table[numConstraints][i] =
                relatedTable[numConstraints + numEqConstraints][i];
        }

        table[numConstraints][numConstraints + numDecisionVars] =
            relatedTable[numConstraints + numEqConstraints][numColumns - 1];

    }
    return solvable;
}

void Solver::optimize(double** table, LPSolution* sol, bool twoPhase)
{
    int numRows, numCols, constraintRows;
    if (twoPhase)
    {
        numRows = numConstraints + numEqConstraints + 2;
        numCols = numDecisionVars + (2 * numConstraints) + 
                  (2 * numEqConstraints) + 1; 
        constraintRows = numRows - 2;
    }
    else
    {
        numRows = numConstraints + 1;
        numCols = numDecisionVars + numConstraints + 1;
        constraintRows = numRows - 1;
    }
    double* optimalValues = new double[int numDecisionVars];

    int maxIter = choose(numCols, numRows);
    unsigned long long numIter = 0; // number of iterations completed.
    bool stay = true;

    while (numIter < maxIter && stay)
    {
        int pivotCol;
        double maxCoeff = ZERO_TOLERANCE;
		// Determine if the solution is optimal or a pivot is needed.
		for (int col = 0; col < numCols - 1; col++)
		{
			if (table[numRows - 1][col] > maxCoeff)
			{
				maxCoeff = table[numRows - 1][col];
				pivotCol = col;
			}
		}
        if (maxCoeff == ZERO_TOLERANCE)
        {
            sol->setErrorCode(LPSolution::SOLVED);

            std::cout << "solved" << std::endl;
            displayMatrix(table);

            // evaluate the final matrix for the values of each decision variable
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
                    optimalValues[col] = table[solutionRow][numCols - 1];
                }
            }

            sol->setOptimalValues(optimalValues);

            if (table[numRows - 1][numCols - 1] == 0)
            {
                sol->setZValue(0);
            }
            else
            {
                sol->setZValue(-1 * table[numRows - 1][numCols - 1]);
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
                        if ((table[row][numCols - 1] /
                             table[row][pivotCol]) < minRatio)
                        {
                            minRatio = table[row][numCols - 1] /
                                       table[row][pivotCol];
                            pivotRow = row;
                        }
                    }
                }

                // pivot the table to (hopefully) increase z.
                pivot(table, pivotRow, pivotCol);
                numIter++;
                std::cout << "iter " << numIter << std::endl;
                displayMatrix(table);
            }
        }
    } // end while loop

    if (maxIter - numIter < 2)
    {
        sol->setErrorCode(LPSolution::EXCEEDED_MAX_ITERATIONS);
    }
}



