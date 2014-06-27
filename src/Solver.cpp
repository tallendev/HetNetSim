/**
 * This class represents a Linear Program solver (may later be extended
 * to solve other types of problems?). This is a Singleton class that receives
 * LinearProgram objects and returns LinearProgramSolutions.
 *
 * Version: 06/24/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
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


/**
 * Implementation of classic simplex method to solving linear programs.
 * Currently assumes:
 * -the problem is in the correct form
 * -the objective function has no constant argument (just coefficients)
 * -all constraints are <= or =
 * -we are trying to maximize
 */
LPSolution Solver::simplexSolve(LinearProgram* lp)
{
    // TODO: check for cycling?

    simplex_t info;    
    LPSolution sol;
    info.numLeqConstraints = (unsigned long) lp->getLeqConstraints()->getSize();
    info.numEqConstraints = (unsigned long) lp->getEqConstraints()->getSize();
    info.numConstraints = info.numLeqConstraints + info.numEqConstraints;
    std::istringstream countVars(lp->getEquation());
    std::string token;
    info.numDecisionVars = 0;

    while (std::getline(countVars, token, ' '))
    {
        info.numDecisionVars++;
    }

    double** table = arrayInit2d(info.numConstraints + 1, 
                                info.numDecisionVars + info.numConstraints + 1);
    lpToTable (lp, table, &info);

    std::cout << "original matrix" << std::endl;
    //displayMatrix(table, &info);

    bool twoPhase;
    // If there are only inequality constraints ond no b values < 0, we
    // can use the origin as a BFS instead of using the Two Phase method.
    int negBValues = 0; // number of inequalities with b < 0

    if (info.numEqConstraints == 0)
    {
        for (unsigned long long i = 0; i < info.numLeqConstraints; i++)
        {
            if (table[i][info.numDecisionVars + info.numConstraints] < 0)
            {
                negBValues++;
            }
        }

        if (negBValues == 0)
        {
            // the problem is trivially feasible
            twoPhase = false;
            solve(table, &sol, twoPhase, &info);
        }
        else
        {
            twoPhase = true;
        }
    }
    else
    {
        twoPhase = true;
    }

    if (twoPhase)
    {
        if (checkFeasibility(table, &info))
        {
            std::cout << "new matrix" << std::endl;
            displayMatrix(table, &info);
            solve(table, &sol, twoPhase, &info);
        }
        else
        {
            sol.setErrorCode(LPSolution::INFEASIBLE);
        }
    }

    arrayDel2d(table, info.numConstraints + 1);

    return sol;
}

void Solver::displayMatrix(double** matrix, simplex_t* info)
{
    for (unsigned long long i = 0; i < 6; i++)
    {
        for (unsigned long long j = 0; j < 11; j++)
        {
            printf("%7.2f", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    //.char outputLine[6][95];

   /* for (unsigned long long i = 0; i < 6; i++)
    {
        strncpy(outputLine[i],
                "  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  **** \n",
                //          1         2         3         4         5         6         7         8         9
                //012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012
                93);
        memset(outputLine[i], ' ', 90);
    }*/
    /*std::cout << "\n\n";
    for (unsigned long long i = 0; i < 6; i++)
    {
        for (unsigned long long j = 0; j < 11; j++)
        {
            std::cout << " " << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
*/
    /*char temp[5];

    for (unsigned long long i = 0; i < 6; i++)
    {
        for (unsigned long long j = 0; j < 11; j++)
        {
            memset(&temp, ' ', 4);
            sprintf(temp, "%.2f", matrix[i][j]);
            strncpy(outputLine[i] + (j * 6) + 2, temp, 4);
        }
    }

    for (unsigned long long i = 0; i < 6; i++)
    {
        std::cout << outputLine[i];
    }*/

    return;
}

unsigned long long gcd(unsigned long long x, unsigned long long y)
{
    while (y != 0)
    {
        unsigned long long t = x % y;
        x = y;
        y = t;
    }

    return x;
}

unsigned long long choose(unsigned long long n, unsigned long long k)
{
    static const unsigned long long MAX_LONG = std::numeric_limits<unsigned long long>::max();
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

bool Solver::checkFeasibility(double** table, simplex_t* info)
{
    // formulate a related solvable problem based on the table and solve it.
    unsigned long long numRows = info->numConstraints + info->numEqConstraints + 2;
    unsigned long long numColumns = info->numDecisionVars + (2 * info->numConstraints) +
                                      (2 * info->numEqConstraints) + 1;
    double** relatedTable = arrayInit2d(numRows, numColumns);

    for (unsigned long long i = 0; i < info->numConstraints; i++)
    {
        for (unsigned long long j = 0; j < info->numDecisionVars + info->numConstraints; j++)
        {
            relatedTable[i][j] = table[i][j];
        }

        relatedTable[i][numColumns - 1] = table[i][info->numDecisionVars + info->numConstraints];
    }

    for (unsigned long long i = 0; i < info->numConstraints + info->numDecisionVars; i++)
    {
        relatedTable[info->numConstraints + info->numEqConstraints][i] =
            table[info->numConstraints][i];
    }

    unsigned long long colCounter = info->numDecisionVars + info->numConstraints +
                                    info->numEqConstraints;

    for (unsigned long long i = 0; i < info->numLeqConstraints; i++)
    {
        if (table[i][info->numDecisionVars + info->numConstraints] < 0)
        {
            relatedTable[i][colCounter++] = -1;
        }
        else
        {
            relatedTable[i][colCounter++] = 1;
        }
    }

    unsigned long long rowCounter = 0;

    for (unsigned long long i = info->numLeqConstraints; i < info->numConstraints; i++)
    {
        if (table[i][info->numDecisionVars + info->numConstraints] < 0)
        {
            relatedTable[i][colCounter++] = -1;
        }
        else
        {
            relatedTable[i][colCounter++] = 1;
        }

        for (unsigned long long j = 0; j < numColumns; j++)
        {
            relatedTable[info->numConstraints + rowCounter][j] = (relatedTable)[i][j];
        }

        relatedTable[info->numConstraints + rowCounter]
        [info->numDecisionVars + info->numLeqConstraints + rowCounter] = 0;
        relatedTable[info->numConstraints + rowCounter]
        [info->numDecisionVars + info->numConstraints + rowCounter] = -1;
        relatedTable[info->numConstraints + rowCounter][colCounter + rowCounter] =
            relatedTable[info->numConstraints + rowCounter][colCounter + rowCounter - 1];
        relatedTable[info->numConstraints + rowCounter][colCounter + rowCounter - 1] = 0;
        rowCounter++;
        colCounter++;
    }

    for (unsigned long long i = info->numDecisionVars + info->numConstraints +
                                  info->numEqConstraints;
         i < numColumns - 1; i++)
    {
        (relatedTable)[info->numConstraints + info->numEqConstraints + 1][i] = 1;
    }

    for (unsigned long long i = 0; i < numRows - 1; i++)
    {
        if (relatedTable[i][numColumns - 1] < 0)
        {
            for (unsigned long long j = 0; j < numColumns; j++)
            {
                if (relatedTable[i][j] != 0)
                {
                    relatedTable[i][j] *= -1;
                }
            }
        }
    }

    for (unsigned long long i = 0; i < numColumns; i++)
    {
        double columnSum = 0;

        for (unsigned long long j = 0; j < numRows - 2; j++)
        {
            columnSum += (relatedTable)[j][i];
        }

        relatedTable[numRows - 1][i] -= columnSum;
    }

    std::cout << "related matrix" << std::endl;
    displayMatrix(relatedTable, info);

    // Attempt to solve the related problem to find a BFS for the original.
    LPSolution relatedSol;
    bool stay = true;
    unsigned long long numIter = 0;
    unsigned long long maxIter = choose((unsigned long long) numColumns - 1,
                                        (unsigned long long) numRows - 1);

    while (stay && numIter < maxIter)
    {
        double minCoeff = DBL_MAX;
        unsigned long long pivotCol;

        for (unsigned long long i = 0; i < numColumns - 1; i++)
        {
            if (relatedTable[numRows - 1][i] < minCoeff)
            {
                minCoeff = relatedTable[numRows - 1][i];
                pivotCol = i;
            }
        }

        if (minCoeff > -1 * ZERO_TOLERANCE)
        {
            relatedSol.setErrorCode(LPSolution::SOLVED);
            std::cout << "related problem solved" << std::endl;
            displayMatrix(relatedTable, info);
            relatedSol.setZValue(-1 * (relatedTable)[numRows - 1][numColumns - 1]);
            stay = false;
        }
        else
        {
            double maxCoeff = -DBL_MAX;

            for (unsigned long long i = 0; i < numRows - 2; i++)
            {
                if (relatedTable[i][pivotCol] > maxCoeff)
                {
                    maxCoeff = relatedTable[i][pivotCol];
                }
            }

            if (maxCoeff < -1 * ZERO_TOLERANCE) // theoretically impossible
            {
                relatedSol.setErrorCode(LPSolution::UNBOUNDED);
                stay = false;
            }
            else
            {
                double minRatio = DBL_MAX;
                unsigned long long pivotRow;

                for (unsigned long long i = 0; i < numRows - 2; i++)
                {
                    if (relatedTable[i][pivotCol] > 0)
                    {
                        if (relatedTable[i][numColumns - 1] /
                            relatedTable[i][pivotCol] < minRatio)
                        {
                            minRatio = relatedTable[i][numColumns - 1] /
                                       relatedTable[i][pivotCol];
                            pivotRow = i;
                        }
                    }
                }

                numIter++;
                std::cout << "Phase I iter " << numIter << std::endl;
                pivot(relatedTable, pivotRow, pivotCol, info);
                displayMatrix(relatedTable, info);
            }
        }
    } // end while

    if (numIter == maxIter)
    {
        relatedSol.setErrorCode(LPSolution::EXCEEDED_MAX_ITERATIONS);
    }

    if (relatedSol.getErrorCode() == 0 &&
        std::abs(relatedSol.getZValue()) < ZERO_TOLERANCE)
    {
        //TODO: check for artifical variables in the basis
        // Transfer the BFS we found to the original table for solving later.
        for (unsigned long long i = 0; i < info->numConstraints; i++)
        {
            for (unsigned long long j = 0; j < info->numConstraints + info->numDecisionVars; j++)
            {
                table[i][j] = relatedTable[i][j];
            }

            (table)[i][info->numConstraints + info->numDecisionVars] =
                relatedTable[i][numColumns - 1];
        }

        for (unsigned long long i = 0; i < info->numConstraints + info->numDecisionVars; i++)
        {
            table[info->numConstraints][i] =
                relatedTable[info->numConstraints + info->numEqConstraints][i];
        }

        table[info->numConstraints][info->numConstraints + info->numDecisionVars] =
            relatedTable[info->numConstraints + info->numEqConstraints][numColumns - 1];

        return true; // the original problem is solvable
    }
    else
    {
        return false;
    }
}

void Solver::lpToTable(LinearProgram* lp, double** table, simplex_t* info)
{
    // populate the table with data from the inequality constraints
    LinkedList<std::string>* listOfLeqConstraints = lp->getLeqConstraints();
    LinkedList<std::string>::ListIterator leqConstraintsIter =
        listOfLeqConstraints->iterator();

    for (unsigned long long i = 0; i < info->numLeqConstraints; i++)
    {
        unsigned long long j = 0;
        std::istringstream split(leqConstraintsIter.next());
        std::string token;

        while (std::getline(split, token, ' '))
        {
            std::istringstream(token) >> table[i][j++];
        }

        j--;
        table[i][info->numDecisionVars + info->numConstraints] = table[i][j];
        table[i][j] = 0;
        table[i][info->numDecisionVars + i] = 1;
    }

    // populate the table with data from the equality constraints
    LinkedList<std::string>* listOfEqConstraints = lp->getEqConstraints();
    LinkedList<std::string>::ListIterator eqConstraintsIter =
        listOfEqConstraints->iterator();

    for (unsigned long long i = info->numLeqConstraints; i < info->numConstraints; i++)
    {
        unsigned long long j = 0;
        std::istringstream split(eqConstraintsIter.next());
        std::string token;

        while (std::getline(split, token, ' '))
        {
            std::istringstream(token) >> (table)[i][j++];
        }

        j--;
        (table)[i][info->numDecisionVars + info->numConstraints] = (table)[i][j];
        (table)[i][j] = 0;
        (table)[i][info->numDecisionVars + i] = 1;
    }

    std::istringstream split(lp->getEquation());
    std::string token;

    for (unsigned long long i = 0; std::getline(split, token, ' '); i++)
    {
        std::istringstream(token) >> (table)[info->numConstraints][i];
    }

    return;
}

void Solver::solve(double** table, LPSolution* sol, bool twoPhase, simplex_t* info)
{
    unsigned long long numRows = info->numConstraints + 1;
    // generally numConstraints + 1
    unsigned long long numCols = info->numDecisionVars + info->numConstraints + 1;
    // generally numDecisionVars + numLeqConstraints + 1
    double* optimalValues = new double[(unsigned long) info->numDecisionVars];

    unsigned long long maxIter = choose((unsigned long long) info->numConstraints +
                                        (unsigned long long) info->numDecisionVars,
                                        (unsigned long long) info->numConstraints);
    unsigned long long numIter = 0; // number of iterations completed.
    bool stay = true;

    while (numIter < maxIter && stay)
    {
        unsigned long long pivotCol;
        double minCoeff = DBL_MAX;
        double maxCoeff = ZERO_TOLERANCE;

        if (twoPhase)
        {
            // Determine if the solution is optimal or a pivot is needed.
            for (unsigned long long col = 0; col < numCols - 1; col++)
            {
                if (table[numRows - 1][col] < minCoeff)
                {
                    minCoeff = (table)[numRows - 1][col];
                    pivotCol = col;
                }
            }
        }
        else
        {
            // Determine if the solution is optimal or a pivot is needed.
            for (unsigned long long col = 0; col < numCols - 1; col++)
            {
                if (table[numRows - 1][col] > maxCoeff)
                {
                    maxCoeff = (table)[numRows - 1][col];
                    pivotCol = col;
                }
            }
        }

        if ( (twoPhase && minCoeff >= -1 * ZERO_TOLERANCE) ||
             (!twoPhase && maxCoeff == ZERO_TOLERANCE) )
        {
            sol->setErrorCode(LPSolution::SOLVED);

            std::cout << "solved" << std::endl;
            displayMatrix(table, info);

            // evaluate the final matrix for the values of each decision variable
            for (unsigned long long col = 0; col < info->numDecisionVars; col++)
            {
                bool foundOne = false; // found a 1 in this column
                bool foundNonZero = false; // found a nonzero number (except 1st 1)
                unsigned long long solutionRow;

                for (unsigned long long row = 0; row < numRows; row++)
                {
                    if (std::abs((table)[row][col]) > ZERO_TOLERANCE)
                    {
                        if ((table)[row][col] != 1 || foundOne)
                        {
                            foundNonZero = true;
                        }
                        else if ((table)[row][col] == 1)
                        {
                            foundOne = true;
                            solutionRow = row;
                        }
                    }
                }

                if (foundOne && !foundNonZero)
                {
                    optimalValues[(unsigned long) col] =
                        (table)[solutionRow][numCols - 1];
                }
            }

            sol->setOptimalValues(optimalValues);

            if ((table)[numRows - 1][numCols - 1] == 0)
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

            for (unsigned long long row = 0; row < info->numConstraints; row++)
            {
                if (table[row][pivotCol] > maxVar)
                {
                    maxVar = (table)[row][pivotCol];
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
                unsigned long long pivotRow;
                double minRatio = DBL_MAX;

                for (unsigned long long row = 0; row < info->numConstraints; row++)
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
                pivot(table, pivotRow, pivotCol, info);
                numIter++;
                std::cout << "iter " << numIter << std::endl;
                displayMatrix(table, info);
            }
        }
    } // end while loop

    if (maxIter - numIter < 2)
    {
        sol->setErrorCode(LPSolution::EXCEEDED_MAX_ITERATIONS);
    }
}

void Solver::pivot(double** table, unsigned long long pivotRow,
                   unsigned long long pivotCol, simplex_t* info)
{
    std::cout << "pivoting on row " << pivotRow << " col " << pivotCol << std::endl;
    unsigned long long numRows = info->numConstraints + 1;
    unsigned long long numCols = info->numDecisionVars + info->numConstraints + 1;
    double pivotNumber = (table)[pivotRow][pivotCol];

    for (unsigned long long col = 0; col < numCols; col++)
    {
        if ((table)[pivotRow][col] != 0) // so it doesn't produce -0
        {
            (table)[pivotRow][col] = (table)[pivotRow][col] / pivotNumber;
        }
    }

    for (unsigned long long row = 0; row < numRows; row++)
    {
        if (std::abs((table)[row][pivotCol]) > ZERO_TOLERANCE &&
            row != pivotRow)
        {
            double multiple = (table)[row][pivotCol] /
                              (table)[pivotRow][pivotCol];

            for (unsigned long long col = 0; col < numCols; col++)
            {
                (table)[row][col] = (table)[row][col] -
                                       (multiple * (table)[pivotRow][col]);
            }
        }
    }
}

