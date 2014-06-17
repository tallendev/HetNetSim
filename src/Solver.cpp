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
    static LPSolution sol;
    int numDecisionVars = 0;
    int numConstraints = lp->GetConstraints()->GetSize();
    float** tableau = lpToTableau(lp, &numDecisionVars, &numConstraints);
    // TODO: check for infeasibility
    unsigned long long maxIter = choose((unsigned long long) numConstraints + numDecisionVars, 
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
	    // this is an optimal solution
	    sol.SetErrorCode(0);
            // TODO: poplulate LPSolution
            for (int i = 0; i < numConstraints + 1; i++)
            {
                delete [] tableau[i];
            }
            delete [] tableau;
            return sol;
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
		// The problem is unbounded.
		sol.SetErrorCode(100);
                for (int i = 0; i < numConstraints + 1; i++)
                {
                    delete [] tableau[i];
                }
                delete [] tableau;
		return sol;
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
		Pivot(tableau, &pivotRow, &pivotCol, &numDecisionVars, &numConstraints);
                numIter++;
            }
	}
    }

    /**
     * For debugging, here's code for displaying the matrix.
     */
    for (int i = 0; i < numConstraints + 1; i++) {
        for (int j = 0; j < numConstraints + numDecisionVars + 1; j++) {
            std::cout << tableau[i][j] << " ";
        }
        std::cout << std::endl;
    }

    /* May need to be moved later if tableau exists in LPSolution. */
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
        return r = -1;
    for (unsigned long long d = 1; d <= k; ++d, --n)
    {
        unsigned long long g = gcd(r, d);
        r /= g;
        unsigned long long t = n / (d / g);
        if (r > std::numeric_limits<unsigned long long>::max() / t)
            // overflow error
            return r = -2;
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
