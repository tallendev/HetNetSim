/**
 * This class represents a Linear Program Solver (may later be extended 
 * to solve other types of problems?). This is a Singleton class that receives
 * LinearProgram objects and returns LinearProgramSolutions. 
 *
 * Version: 06/18/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 */

#include "Solver.h"
#include "float.h"
#include <string>
#include <string.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <limits>

unsigned long long choose(unsigned long long n, unsigned long long k);

/** 
 * Constants-
 * A zero tolerance in case of rounding errors, and 
 * errorCode values for LPSolution
 */
const double ZERO_TOLERANCE = 0.0001;
const int SOLVED = 0;
// 100 is the default value
const int UNBOUNDED = 200;
const int EXCEEDED_MAX_ITERATIONS = 300; 
const int INFEASIBLE = 400;

/**
 * Implementation of classic simplex method to solving linear programs.
 * Currently assumes:
 * -the problem is in the correct form
 * -the objective function has no constant argument (just coefficients)
 * -all constraints are <=
 * -we are trying to maximize
 * -the problem can be solved
 * -all decision variables must be nonnegative 
 */
LPSolution Solver::SimplexSolve(LinearProgram* lp)
{
    // TODO: check for cycling

    LPSolution sol;
    
    numLeqConstraints = lp->GetLeqConstraints()->GetSize();
    numEqConstraints = lp->GetEqConstraints()->GetSize();
    numConstraints = numLeqConstraints + numEqConstraints;
    std::istringstream countVars(lp->GetEquation());
    std::string token;
    numDecisionVars = 0;
    while (std::getline(countVars, token, ' '))
    {
        numDecisionVars++;
    }
    dblmatrix* tableau = new dblmatrix;
    tableau->resize(numConstraints + 1, 
                    std::vector<double>(numDecisionVars + numConstraints + 1, 0));

    lpToTableau (lp, tableau);

    /* display the matrix 
    std::cout << "original matrix" << std::endl;
    for (int i = 0; i < numConstraints + 1; i++) {
        for (int j = 0; j < numConstraints + numDecisionVars + 1; j++)
            std::cout << (*tableau)[i][j] << "  ";
    std::cout << std::endl;
    } */
    
    if (CheckFeasibility (tableau))
        Solve(tableau, &sol);
    else
        sol.SetErrorCode(INFEASIBLE);

    delete tableau;

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

bool Solver::CheckFeasibility(dblmatrix* tableau)
{
    // formulate a related solvable problem based on the tableau and pass it to the solver.
    dblmatrix* relatedTableau = new dblmatrix;
    relatedTableau->resize(numConstraints + 1, 
                           std::vector<double>(numDecisionVars + numConstraints + 1, 0));
    for (int i = 0; i < numConstraints; i++)
    {
        (*relatedTableau)[numConstraints][numDecisionVars + i] = -1;
        (*relatedTableau)[i][numDecisionVars + numConstraints] = 
               (*tableau)[i][numDecisionVars + numConstraints];
        if ((*tableau)[i][numDecisionVars + numConstraints] >= 0)
            (*relatedTableau)[i][numDecisionVars + i] = 1;
        else
            (*relatedTableau)[i][numDecisionVars + i] = -1;
        for (int j = 0; j < numDecisionVars; j++)
            (*relatedTableau)[i][j] = (*tableau)[i][j];
    }

    /* display the matrix 
    std::cout << "related matrix" << std::endl;
    for (int i = 0; i < numConstraints + 1; i++) {
        for (int j = 0; j < numConstraints + numDecisionVars + 1; j++) 
            std::cout << (*relatedTableau)[i][j] << "    ";
    std::cout << std::endl;
    }*/

    LPSolution relatedSol;
    Solve(relatedTableau, &relatedSol); 
    // std::cout << relatedSol.GetZValue() << std::endl;
    delete relatedTableau;
    if (std::abs(relatedSol.GetZValue()) < ZERO_TOLERANCE || 
                 relatedSol.GetZValue() > ZERO_TOLERANCE)
        return true; // the original problem is solvable
    else
        return false; // the original problem is infeasible 
}

void Solver::lpToTableau(LinearProgram* lp, dblmatrix* tableau)
{
    // populate the tableau with data from the inequality constraints
    LinkedList<std::string>* listOfLeqConstraints = lp->GetLeqConstraints();
    LinkedList<std::string>::ListIterator leqConstraintsIter = listOfLeqConstraints->Iterator();
    for (int i = 0; i < numLeqConstraints; i++)
    {
        int j = 0;
        std::istringstream split(leqConstraintsIter.Next());
        std::string token;
        while (std::getline(split, token, ' '))
        {
            std::istringstream(token) >> (*tableau)[i][j++];
        }
        j--;
        (*tableau)[i][numDecisionVars + numConstraints] = (*tableau)[i][j];
        (*tableau)[i][j] = 0;
        (*tableau)[i][numDecisionVars + i] = 1;
    }
    
    // populate the tableau with data from the equality constraints
    LinkedList<std::string>* listOfEqConstraints = lp->GetEqConstraints();
    LinkedList<std::string>::ListIterator EqConstraintsIter = listOfEqConstraints->Iterator();
    for (int i = numLeqConstraints; i < numConstraints; i++)
    {
        int j = 0;
        std::istringstream split(EqConstraintsIter.Next());
        std::string token;
        while (std::getline(split, token, ' '))
        {
            std::istringstream(token) >> (*tableau)[i][j++];
        }
        j--;
        (*tableau)[i][numDecisionVars + numConstraints] = (*tableau)[i][j];
        (*tableau)[i][j] = 0;
    }
     
    std::istringstream split(lp->GetEquation());
    std::string token;
    for (int i = 0; std::getline(split, token, ' '); i++)
    {
        std::istringstream(token) >> (*tableau)[numConstraints][i];
    }
    return;
}

void Solver::Solve(dblmatrix* tableau, LPSolution* sol)
{
    std::vector<double> optimalValues ((unsigned long) numDecisionVars, 0);

    unsigned long long maxIter = choose((unsigned long long) numConstraints + 
                                        (unsigned long long) numDecisionVars, 
                                        (unsigned long long) numConstraints);
    unsigned long long numIter = 0; // number of iterations completed.
    bool stay = true;
    while (numIter < maxIter && stay)
    {
        // Determine if the solution is optimal or a pivot is needed.
        double maxCoeff = ZERO_TOLERANCE;
        int pivotCol = -1;
	for (int col = 0; col < numDecisionVars + numConstraints; col++)
	{
	    if ((*tableau)[numConstraints][col] > maxCoeff)
	    {
		maxCoeff = (*tableau)[numConstraints][col];
		pivotCol = col;
	    }
	}
	if (maxCoeff == ZERO_TOLERANCE)
	{
	    sol->SetErrorCode(SOLVED);

            /* display the matrix 
            for (int i = 0; i < numConstraints + 1; i++) {
                for (int j = 0; j < numConstraints + numDecisionVars + 1; j++) 
		    std::cout << (*tableau)[i][j] << "  ";
                std::cout << std::endl;
	    }*/
            
            // evaluate the final matrix for the values of each decision variable
            for (int col = 0; col < numDecisionVars; col++)
            {
                bool foundOne = false; // found a 1 in this column
                bool foundNonZero = false; // found a nonzero number (except 1st 1)
                int solutionRow = -1; // theorectically redundant
                for (int row = 0; row < numConstraints + 1; row++)
                {
                    if (std::abs((*tableau)[row][col]) > ZERO_TOLERANCE)
                    {
                        if ((*tableau)[row][col] != 1 || foundOne)
                        {
                            foundNonZero = true;
                        }
                        else if ((*tableau)[row][col] == 1)
                        {
                            foundOne = true;
                            solutionRow = row;
                        }
                    }
                }
                if (foundOne && !foundNonZero)
                {
                   optimalValues[(unsigned long) col] = 
                   (*tableau)[solutionRow][numConstraints + numDecisionVars];
                }
            }
            sol->SetOptimalValues(optimalValues); 
            sol->SetZValue(-1 * (*tableau)[numConstraints][numDecisionVars + numConstraints]);
            stay = false; // break out of the loop to return
	}
	else 
	{
	    // Check if all entries in pivotCol are <= 0
	    double maxVar = ZERO_TOLERANCE;
	    for (int row = 0; row < numConstraints; row++)
	    {
		if ((*tableau)[row][pivotCol] > maxVar)
		    maxVar = (*tableau)[row][pivotCol];
	    }
	    if (maxVar == ZERO_TOLERANCE)
	    {
		sol->SetErrorCode(UNBOUNDED);
                stay = false; // break out of the loop to return
	    }
	    else
	    {
		// Determine pivot row.
		int pivotRow = -1;
		double minRatio = DBL_MAX;
		for (int row = 0; row < numConstraints; row++)
		{
		    if ((*tableau)[row][pivotCol] > ZERO_TOLERANCE)
		    {
			if (((*tableau)[row][numConstraints + numDecisionVars] / 
                             (*tableau)[row][pivotCol]) < minRatio)
			{
			    minRatio = (*tableau)[row][numConstraints + numDecisionVars] / 
                                       (*tableau)[row][pivotCol];
			    pivotRow = row;
			}
		    }
		}
		// Pivot the table to (hopefully) increase z.
		Pivot(tableau, &pivotRow, &pivotCol);
                numIter++;
	    }
        }
    } // end while loop

    if (maxIter - numIter < 2)
        sol->SetErrorCode(EXCEEDED_MAX_ITERATIONS);

    return;
}

void Solver::Pivot(dblmatrix* tableau, int* pivotRow, int* pivotCol)
{
    double pivotNumber = (*tableau)[*pivotRow][*pivotCol];
    for (int col = 0; col < numConstraints + numDecisionVars + 1; col++)
    {
        (*tableau)[*pivotRow][col] = (*tableau)[*pivotRow][col] / pivotNumber;
    }
    for (int row = 0; row < numConstraints + 1; row++)
    {
        if (std::abs((*tableau)[row][*pivotCol]) > ZERO_TOLERANCE && 
            row != *pivotRow)
        {
            double multiple = (*tableau)[row][*pivotCol] / 
                              (*tableau)[*pivotRow][*pivotCol];
            for (int col = 0; col < numConstraints + numDecisionVars + 1; col++)
            {
                (*tableau)[row][col] = (*tableau)[row][col] - 
                                       (multiple * (*tableau)[*pivotRow][col]);
            }
        }
    }
}

