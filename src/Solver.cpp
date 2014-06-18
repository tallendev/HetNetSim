/**
 * This class represents a Linear Program Solver (may later be extended 
 * to solve other types of problems?). This is a Singleton class that receives
 * LinearProgram objects and returns LinearProgramSolutions. 
 *
 * Version: 06/17/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 */

#include "Solver.h"
#include "float.h"
#include <string>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <limits>

unsigned long long choose(unsigned long long n, unsigned long long k);

/**
 * Implementation of classic simplex method to solving linear programs.
 * Currently assumes:
 * -the problem is in the correct form
 * -the objective function has no constant argument (just coefficients)
 * -all constraints are <=
 * -we are trying to maximize
 * -the problem can be solved
 * -all vars must be nonnegative 
 */
LPSolution Solver::SimplexSolve(LinearProgram* lp)
{
    // TODO: check for infeasibility
    // TODO: check for cycling

    LPSolution sol;
    // declare constants for the solution's error code field
    const int SOLVED = 0;
    // 100 is default for LPSolution.errorCode
    const int UNBOUNDED = 200;
    const int EXCEEDED_MAX_ITERATIONS = 300; 
    // const int INFEASIBLE = 400;

    int numDecisionVars = 0;
    int numConstraints = lp->GetConstraints()->GetSize();
    float** tableau = lpToTableau(lp, &numDecisionVars, &numConstraints);
    float* optimalValues;
    optimalValues = new float[numDecisionVars];
    memset(optimalValues, 0, sizeof(*optimalValues) * numDecisionVars);
    unsigned long long maxIter = choose((unsigned long long) numConstraints + (unsigned long long) numDecisionVars, 
                                        (unsigned long long) numConstraints);
    unsigned long long numIter = 0; // number of iterations completed.
    while (numIter < maxIter)
    {
        // Determine if the solution is optimal or a pivot is needed.
        float maxCoeff = 0;
        int pivotCol = -1;
	for (int var = 0; var < numDecisionVars + numConstraints; var++)
	{
	    if (tableau[numConstraints][var] > maxCoeff)
	    {
		maxCoeff = tableau[numConstraints][var];
		pivotCol = var;
	    }
	}
	if (maxCoeff == 0)
	{
	    sol.SetErrorCode(SOLVED);

            // display the matrix 
            for (int i = 0; i < numConstraints + 1; i++) {
                for (int j = 0; j < numConstraints + numDecisionVars + 1; j++) 
		    std::cout << tableau[i][j] << "  ";
                std::cout << std::endl;
	    }

            // evaluate the final matrix for the values of each decision variable
            bool foundOne = false; // set to true if you find a one
            bool foundNonZero = false; // set to true if you find something other than a zero or one
            int solutionRow = -1;
            for (int col = 0; col < numDecisionVars; col++)
            {
                for (int row = 0; row < numConstraints + 1; row++)
                {
                    if (tableau[row][col] != 0)
                    {
                        if (tableau[row][col] != 1 || foundOne)
                        {
                            foundNonZero = true;
                        }
                        else if (tableau[row][col] == 1)
                        {
                            foundOne = true;
                            solutionRow = row;
                        }
                    }
                }
                if (foundOne && !foundNonZero)
                {
                   std::cout << "writing " << tableau[solutionRow][numConstraints + numDecisionVars] <<
                                " to column " << col << " in optimalValues." << std::endl;
                   optimalValues[col] = tableau[solutionRow][numConstraints + numDecisionVars];
                }
            }
            sol.SetOptimalValues(optimalValues); 
            numIter = maxIter; // break out of the loop to return
	}
	else
	{
	    // Check if all entries in pivotCol are <= 0
	    float maxVar = 0;
	    for (int i = 0; i < numConstraints; i++)
	    {
		if (tableau[i][pivotCol] > maxVar)
		    maxVar = tableau[i][pivotCol];
	    }
	    if (maxVar == 0)
	    {
		sol.SetErrorCode(UNBOUNDED);
                numIter = maxIter; // break out of the loop to return
	    }
	    else
	    {
		// Determine pivot row.
		int pivotRow = -1;
		double minRatio = DBL_MAX;
		for (int i = 0; i < numConstraints; i++)
		{
		    if (tableau[i][pivotCol] > 0)
		    {
			if ((tableau[i][numConstraints + numDecisionVars] / tableau[i][pivotCol]) < minRatio)
			{
			    minRatio = tableau[i][numConstraints + numDecisionVars] / tableau[i][pivotCol];
			    pivotRow = i;
			}
		    }
		}
		// Pivot the table to (hopefully) increase z.
                std::cout << "Pivot on row " << pivotRow << " and column " << pivotCol << std::endl;
		Pivot(tableau, &pivotRow, &pivotCol, &numDecisionVars, &numConstraints);
                numIter++;
                for (int i = 0; i < numConstraints + 1; i++) {
                    for (int j = 0; j < numConstraints + numDecisionVars + 1; j++) {
		        std::cout << tableau[i][j] << "  ";
                    }
                std::cout << std::endl;
	        }
                std::cout << std::endl;
            }
	}
    }

    if (maxIter - numIter < 2)
        sol.SetErrorCode(EXCEEDED_MAX_ITERATIONS);

    // free memory
    delete [] optimalValues;
    for (int i = 0; i < numConstraints + 1; i++)
    {
        delete [] tableau[i];
    }
    delete [] tableau;

    return sol;
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
    unsigned long long r = 1;
    if (k > n)
        // invalid parameters
        return r;
    for (unsigned long long d = 1; d <= k; ++d, --n)
    {
        unsigned long long g = gcd(r, d);
        r /= g;
        unsigned long long t = n / (d / g);
        if (r > std::numeric_limits<unsigned long long>::max() / t)
            // overflow error
            return r;
        r *= t;
    }
    return r;
}

float** Solver::lpToTableau(LinearProgram* lp, int* numDecisionVars, int* numConstraints)
{
    std::istringstream countVars(lp->GetEquation());
    std::string token;
    while (std::getline(countVars, token, ' '))
    {
        (*numDecisionVars)++;
    }

    float** tableau = new float*[*numConstraints + 1]();  //implement safe calloc
    for (int i = 0; i < *numConstraints + 1; i++)
    {
        tableau[i] = new float[(*numDecisionVars + *numConstraints + 1) * *numConstraints]();
    }
    LinkedList<std::string>* listOfConstraints = lp->GetConstraints();
    LinkedList<std::string>::ListIterator constraintsIter = listOfConstraints->Iterator();
    for (int i = 0; i < *numConstraints; i++)
    {
        int j = 0;
        std::istringstream split(constraintsIter.Next());
        while (std::getline(split, token, ' '))
        {
            std::istringstream(token) >> tableau[i][j++];
        }
        j--;
        tableau[i][*numDecisionVars + *numConstraints] = tableau[i][j];
        tableau[i][j] = 0;
        tableau[i][*numDecisionVars + i] = 1;
    }
    
    std::istringstream split(lp->GetEquation());
    for (int i = 0; std::getline(split, token, ' '); i++)
    {
        std::istringstream(token) >> tableau[*numConstraints][i];
    }
    return tableau;
}

void Solver::Pivot(float** tableau, int* pivotRow, int* pivotCol,
                   int* numDecisionVars, int* numConstraints)
{
    float pivotNumber = tableau[*pivotRow][*pivotCol];
    for (int col = 0; col < *numConstraints + *numDecisionVars + 1; col++)
    {
        tableau[*pivotRow][col] = tableau[*pivotRow][col] / pivotNumber;
    }
    for (int row = 0; row < *numConstraints + 1; row++)
    {
        if (tableau[row][*pivotCol] != 0 && row != *pivotRow)
        {
            float multiple = tableau[row][*pivotCol] / tableau[*pivotRow][*pivotCol];
            for (int col = 0; col < *numConstraints + *numDecisionVars + 1; col++)
            {
                tableau[row][col] = tableau[row][col] - (multiple * tableau[*pivotRow][col]);
            }
        }
    }
}
