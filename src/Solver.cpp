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
    // TODO: check for cycling?
    // TODO: implement a struct

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
                    std::vector<double>(numDecisionVars + numLeqConstraints + 1, 0));

    lpToTableau (lp, tableau);

    std::cout << "original matrix" << std::endl;
    DisplayMatrix(tableau);

    if (CheckFeasibility(tableau))
        Solve(tableau, &sol);
    else
        sol.SetErrorCode(INFEASIBLE);

    delete tableau;

    return sol;
}

void Solver::DisplayMatrix(dblmatrix* matrix)
{
    for (int i = 0; i < matrix->size(); i++) {
       for (int j = 0; j < ((*matrix)[0]).size(); j++)
           std::cout << (*matrix)[i][j] << "  ";
       std::cout << std::endl;
    }
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
    // If there are only inequality constraints ond no b values < 0, it's solvable.
    int negBValues = 0; // number of inequalities with b < 0
    if (numEqConstraints == 0)
    {
        for (int i = 0; i < numLeqConstraints; i++)
        {
            if ((*tableau)[i][numDecisionVars + numLeqConstraints] < 0)
                negBValues++;
        }
        if (negBValues == 0)
            return true;
    } 
    // formulate a related solvable problem based on the tableau and pass it to the solver.
    int numColumns = numDecisionVars + numConstraints + negBValues + 1;
    dblmatrix* relatedTableau = new dblmatrix;
    relatedTableau->resize(numConstraints + 1, std::vector<double>(numColumns, 0));
    for (int i = 0; i < numConstraints; i++)
    {
        for (int j = 0; j < numDecisionVars + numLeqConstraints; j++)
            (*relatedTableau)[i][j] = (*tableau)[i][j];
        (*relatedTableau)[i][numColumns - 1] = 
               (*tableau)[i][numDecisionVars + numLeqConstraints];
    }
    int colCounter = 0;
    for (int i = 0; i < numLeqConstraints; i++)
    {
        if ((*tableau)[i][numDecisionVars + numLeqConstraints] < 0)
            (*relatedTableau)[i][numDecisionVars + numLeqConstraints + colCounter++] = -1;
    }
    for (int i = numLeqConstraints; i < numConstraints; i++)
    {
        if ((*tableau)[i][numDecisionVars + numLeqConstraints] < 0)
            (*relatedTableau)[i][numDecisionVars + numLeqConstraints + colCounter++] = -1;
        else
            (*relatedTableau)[i][numDecisionVars + numLeqConstraints + colCounter++] = 1;
    }
    for (int i = numDecisionVars + numLeqConstraints; i < numColumns - 1; i++)
    {
        (*relatedTableau)[numConstraints][i] = -1;
    }

    std::cout << "related matrix" << std::endl;
    DisplayMatrix(relatedTableau);
     
    for (int i = numDecisionVars + numLeqConstraints; i < numColumns - 1; i++)
    {
        for (int j = 0; j < numConstraints; j++)
        {
            if ((*relatedTableau)[j][i] == -1)
                Pivot(relatedTableau, j, i);
        }
    }        
    bool positiveValue = false;
    for (int i = 0; i < numColumns - 1; i++)
    {
        if ((*relatedTableau)[numConstraints][i] > ZERO_TOLERANCE)
            positiveValue = true;
    }
    if (!positiveValue)
    {
        for (int i = numDecisionVars + numLeqConstraints; i < numColumns - 1; i++)
            if ((*relatedTableau)[numConstraints][i] == -1)
                for (int j = 0; j < numConstraints; j++)
                    if ((*relatedTableau)[j][i] == 1)
                        Pivot(relatedTableau, j, i);
    }

    std::cout << "related matrix post-pivoting" << std::endl;
    DisplayMatrix(relatedTableau);

    LPSolution relatedSol;
    Solve(relatedTableau, &relatedSol); 
    std::cout << "relatedSol z = " << relatedSol.GetZValue() << std::endl;
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
        (*tableau)[i][numDecisionVars + numLeqConstraints] = (*tableau)[i][j];
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
        (*tableau)[i][numDecisionVars + numLeqConstraints] = (*tableau)[i][j];
        if (numLeqConstraints > 0)
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
    int numRows = tableau->size(); 
    // generally numConstraints + 1
    int numCols = ((*tableau)[0]).size(); 
    // generally numDecisionVars + numLeqConstraints + 1
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
	for (int col = 0; col < numCols; col++)
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

            std::cout << "solved" << std::endl;
            DisplayMatrix(tableau);
            
            // evaluate the final matrix for the values of each decision variable
            for (int col = 0; col < numDecisionVars; col++)
            {
                bool foundOne = false; // found a 1 in this column
                bool foundNonZero = false; // found a nonzero number (except 1st 1)
                int solutionRow = -1; // theorectically redundant
                for (int row = 0; row < numRows; row++)
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
                   (*tableau)[solutionRow][numCols - 1];
                }
            }
            sol->SetOptimalValues(optimalValues); 
            sol->SetZValue(-1 * (*tableau)[numRows - 1][numCols - 1]);
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
			if (((*tableau)[row][numCols - 1] / 
                             (*tableau)[row][pivotCol]) < minRatio)
			{
			    minRatio = (*tableau)[row][numCols - 1] / 
                                       (*tableau)[row][pivotCol];
			    pivotRow = row;
			}
		    }
		}
		// Pivot the table to (hopefully) increase z.
		Pivot(tableau, pivotRow, pivotCol);
                numIter++;
		std::cout << "iter " << numIter << std::endl;
                DisplayMatrix(tableau);
	    }
        }
    } // end while loop

    if (maxIter - numIter < 2)
        sol->SetErrorCode(EXCEEDED_MAX_ITERATIONS);

    return;
}

void Solver::Pivot(dblmatrix* tableau, int pivotRow, int pivotCol)
{
    int numRows = tableau->size();
    int numCols = ((*tableau)[0]).size();
    double pivotNumber = (*tableau)[pivotRow][pivotCol];
    for (int col = 0; col < numCols; col++)
    {
        (*tableau)[pivotRow][col] = (*tableau)[pivotRow][col] / pivotNumber;
    }
    for (int row = 0; row < numRows; row++)
    {
        if (std::abs((*tableau)[row][pivotCol]) > ZERO_TOLERANCE && 
            row != pivotRow)
        {
            double multiple = (*tableau)[row][pivotCol] / 
                              (*tableau)[pivotRow][pivotCol];
            for (int col = 0; col < numCols; col++)
            {
                (*tableau)[row][col] = (*tableau)[row][col] - 
                                       (multiple * (*tableau)[pivotRow][col]);
            }
        }
    }
}

