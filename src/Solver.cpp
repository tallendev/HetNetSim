/**
 * This class represents a Linear Program Solver (may later be extended 
 * to solve other types of problems?). This is a Singleton class that receives
 * LinearProgram objects and returns LinearProgramSolutions. 
 *
 * Version: 06/22/2014
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
    
    numLeqConstraints = (unsigned long) lp->GetLeqConstraints()->GetSize();
    numEqConstraints = (unsigned long) lp->GetEqConstraints()->GetSize();
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

    std::cout << "original matrix" << std::endl;
    DisplayMatrix(tableau);

    bool twoPhase;
    // If there are only inequality constraints ond no b values < 0, we
    // can use the origin as a BFS instead of using the Two Phase method.
    int negBValues = 0; // number of inequalities with b < 0
    if (numEqConstraints == 0)
    {
        for (dblmatrix::size_type i = 0; i < numLeqConstraints; i++)
        {
            if ((*tableau)[i][numDecisionVars + numConstraints] < 0)
                negBValues++;
        }
        if (negBValues == 0)
        {
            // the problem is trivially feasible                
            twoPhase = false;
            Solve(tableau, &sol, twoPhase);
        }
        else 
            twoPhase = true;
    }
    else
        twoPhase = true; 

    if (twoPhase)
    {
        if (CheckFeasibility(tableau))
        {
            std::cout << "new matrix" << std::endl;
            DisplayMatrix(tableau);
            Solve(tableau, &sol, twoPhase);
        }
        else
            sol.SetErrorCode(INFEASIBLE);
    }

    delete tableau;

    return sol;
}

void Solver::DisplayMatrix(dblmatrix* matrix)
{
    char outputLine[matrix->size()][95];
    for (dblmatrix::size_type i = 0; i < matrix->size(); i++)
    {
        strncpy(outputLine[i],
     "  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  **** \n",
    //          1         2         3         4         5         6         7         8         9
    //012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012
        93);
        memset(outputLine[i], ' ', 90);
    }
    char temp[5];
    for (dblmatrix::size_type i = 0; i < matrix->size(); i++) 
    {
       for (dblmatrix::size_type j = 0; j < ((*matrix)[0]).size(); j++)
       {
           memset(&temp, ' ', 4);
           sprintf(temp, "%.2f", (*matrix)[i][j]); 
           strncpy(outputLine[i]+(j*6)+2, temp, 4);
       }
    }
    for (dblmatrix::size_type i = 0; i < matrix->size(); i++)
    {
        std::cout << outputLine[i];
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
    //dblmatrix::size_type numOriginalColumns = tableau->size();
    //dblmatrix::size_type numOriginalRows = ((*tableau)[0]).size();

    // formulate a related solvable problem based on the tableau and solve it.
    dblmatrix::size_type numRows = numConstraints + numEqConstraints + 2;
    dblmatrix::size_type numColumns = numDecisionVars + (2 * numConstraints) + 
                                      (2 * numEqConstraints) + 1;
    dblmatrix* relatedTableau = new dblmatrix;
    relatedTableau->resize(numRows, std::vector<double>(numColumns, 0));
    for (dblmatrix::size_type i = 0; i < numConstraints; i++)
    {
        for (dblmatrix::size_type j = 0; j < numDecisionVars + numConstraints; j++)
        {
            (*relatedTableau)[i][j] = (*tableau)[i][j];
        }
        (*relatedTableau)[i][numColumns - 1] = 
               (*tableau)[i][numDecisionVars + numConstraints];
    }
    for (dblmatrix::size_type i = 0; i < numConstraints + numDecisionVars; i++)
    {
        (*relatedTableau)[numConstraints + numEqConstraints][i] = 
        (*tableau)[numConstraints][i];
    }
    dblmatrix::size_type colCounter = numDecisionVars + numConstraints + numEqConstraints;
    for (dblmatrix::size_type i = 0; i < numLeqConstraints; i++)
    {
        if ((*tableau)[i][numDecisionVars + numConstraints] < 0)
            (*relatedTableau)[i][colCounter++] = -1;
        else
            (*relatedTableau)[i][colCounter++] = 1;
    }
    dblmatrix::size_type rowCounter = 0;
    for (dblmatrix::size_type i = numLeqConstraints; i < numConstraints; i++)
    {
        if ((*tableau)[i][numDecisionVars + numConstraints] < 0)
            (*relatedTableau)[i][colCounter++] = -1;
        else
            (*relatedTableau)[i][colCounter++] = 1;
        for (dblmatrix::size_type j = 0; j < numColumns; j++)
        {
            (*relatedTableau)[numConstraints + rowCounter][j] = (*relatedTableau)[i][j];
        }
        (*relatedTableau)[numConstraints + rowCounter]
                         [numDecisionVars + numLeqConstraints + rowCounter] = 0;
        (*relatedTableau)[numConstraints + rowCounter]
                         [numDecisionVars + numConstraints + rowCounter] = -1;
        (*relatedTableau)[numConstraints + rowCounter][colCounter + rowCounter] = 
        (*relatedTableau)[numConstraints + rowCounter][colCounter + rowCounter - 1];
        (*relatedTableau)[numConstraints + rowCounter][colCounter + rowCounter - 1] = 0;
        rowCounter++;
        colCounter++;
    }
    for (dblmatrix::size_type i = numDecisionVars + numConstraints + numEqConstraints; 
                              i < numColumns - 1; i++)
    {
        (*relatedTableau)[numConstraints + numEqConstraints + 1][i] = 1;
    }
    for (dblmatrix::size_type i = 0; i < numRows - 1; i++)
    {
        if ((*relatedTableau)[i][numColumns - 1] < 0)
            for (dblmatrix::size_type j = 0; j < numColumns; j++)
            {
                if ((*relatedTableau)[i][j] != 0)
                    (*relatedTableau)[i][j] *= -1;
            }
    }
    for (dblmatrix::size_type i = 0; i < numColumns; i++)
    {
        double columnSum = 0;
        for (dblmatrix::size_type j = 0; j < numRows - 2; j++)
        {
            columnSum += (*relatedTableau)[j][i];
        }
        (*relatedTableau)[numRows - 1][i] -= columnSum;
    }

    std::cout << "related matrix" << std::endl;
    DisplayMatrix(relatedTableau);

    // Attempt to solve the related problem to find a BFS for the original.
    LPSolution relatedSol;
    bool stay = true;
    unsigned long long numIter = 0;
    unsigned long long maxIter = choose((unsigned long long) numColumns - 1, 
                                        (unsigned long long) numRows - 1);
    while (stay && numIter < maxIter)
    {
        double minCoeff = DBL_MAX;
	dblmatrix::size_type pivotCol;
	for (dblmatrix::size_type i = 0; i < numColumns - 1; i++)
	{
	    if ((*relatedTableau)[numRows - 1][i] < minCoeff)
	    {
		minCoeff = (*relatedTableau)[numRows - 1][i];
		pivotCol = i;
	    }
	}
	if (minCoeff > -1 * ZERO_TOLERANCE)
	{
	    relatedSol.SetErrorCode(SOLVED);
            std::cout << "related problem solved" << std::endl;
            DisplayMatrix(relatedTableau);
            relatedSol.SetZValue(-1 * (*relatedTableau)[numRows - 1][numColumns - 1]);
            stay = false;
	}
	else
	{
	    double maxCoeff = -DBL_MAX;
	    for (dblmatrix::size_type i = 0; i < numRows - 2; i++)
	    {
		if ((*relatedTableau)[i][pivotCol] > maxCoeff)
		{
		    maxCoeff = (*relatedTableau)[i][pivotCol];
		}
	    }
	    if (maxCoeff < -1 * ZERO_TOLERANCE) // theoretically impossible I think
	    {
                relatedSol.SetErrorCode(UNBOUNDED);
                stay = false;
	    }
	    else
	    {
		double minRatio = DBL_MAX;
		dblmatrix::size_type pivotRow;
		for (dblmatrix::size_type i = 0; i < numRows - 2; i++)
		{
		    if ((*relatedTableau)[i][pivotCol] > 0)
		    {
			if ((*relatedTableau)[i][numColumns - 1] / 
			    (*relatedTableau)[i][pivotCol] < minRatio)
			{
			    minRatio = (*relatedTableau)[i][numColumns - 1] /
				       (*relatedTableau)[i][pivotCol];
			    pivotRow = i;
			}
		    }
		} 
		  
		numIter++;
		std::cout << "Phase I iter " << numIter << std::endl;
		Pivot(relatedTableau, pivotRow, pivotCol);
		DisplayMatrix(relatedTableau);
            }
        }
    } // end while
    
    if (numIter == maxIter)
        relatedSol.SetErrorCode(EXCEEDED_MAX_ITERATIONS);

    if (relatedSol.GetErrorCode() == 0 && 
        std::abs(relatedSol.GetZValue()) < ZERO_TOLERANCE)
    {
        //TODO: check for artifical variables in the basis
        // Transfer the BFS we found to the original tableau for solving later.
        for (dblmatrix::size_type i = 0; i < numConstraints; i++)
        {
            for (dblmatrix::size_type j = 0; j < numConstraints + numDecisionVars; j++)
            {
                (*tableau)[i][j] = (*relatedTableau)[i][j];
            }
            (*tableau)[i][numConstraints + numDecisionVars] = 
            (*relatedTableau)[i][numColumns - 1];
        }
        for (dblmatrix::size_type i = 0; i < numConstraints + numDecisionVars; i++)
        {
            (*tableau)[numConstraints][i] = 
            (*relatedTableau)[numConstraints + numEqConstraints][i];
        }
        (*tableau)[numConstraints][numConstraints + numDecisionVars] = 
        (*relatedTableau)[numConstraints + numEqConstraints][numColumns - 1];
             
        return true; // the original problem is solvable
    }
    else
        return false; 
}

void Solver::lpToTableau(LinearProgram* lp, dblmatrix* tableau)
{
    // populate the tableau with data from the inequality constraints
    LinkedList<std::string>* listOfLeqConstraints = lp->GetLeqConstraints();
    LinkedList<std::string>::ListIterator leqConstraintsIter = listOfLeqConstraints->Iterator();
    for (dblmatrix::size_type i = 0; i < numLeqConstraints; i++)
    {
        dblmatrix::size_type j = 0;
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
    for (dblmatrix::size_type i = numLeqConstraints; i < numConstraints; i++)
    {
        dblmatrix::size_type j = 0;
        std::istringstream split(EqConstraintsIter.Next());
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
     
    std::istringstream split(lp->GetEquation());
    std::string token;
    for (dblmatrix::size_type i = 0; std::getline(split, token, ' '); i++)
    {
        std::istringstream(token) >> (*tableau)[numConstraints][i];
    }
    return;
}

void Solver::Solve(dblmatrix* tableau, LPSolution* sol, bool twoPhase)
{
    dblmatrix::size_type numRows = tableau->size(); 
    // generally numConstraints + 1
    dblmatrix::size_type numCols = ((*tableau)[0]).size(); 
    // generally numDecisionVars + numLeqConstraints + 1
    std::vector<double> optimalValues ((unsigned long) numDecisionVars, 0);

    unsigned long long maxIter = choose((unsigned long long) numConstraints + 
                                        (unsigned long long) numDecisionVars, 
                                        (unsigned long long) numConstraints);
    unsigned long long numIter = 0; // number of iterations completed.
    bool stay = true;
    while (numIter < maxIter && stay)
    {
        dblmatrix::size_type pivotCol;
        double minCoeff = DBL_MAX;
        double maxCoeff = ZERO_TOLERANCE;
        if (twoPhase)
        {
	    // Determine if the solution is optimal or a pivot is needed.
	    for (dblmatrix::size_type col = 0; col < numCols - 1; col++)
	    {
		if ((*tableau)[numRows - 1][col] < minCoeff)
		{
		    minCoeff = (*tableau)[numRows - 1][col];
		    pivotCol = col;
		}
	    }
        }
        else
        {
	    // Determine if the solution is optimal or a pivot is needed.
	    for (dblmatrix::size_type col = 0; col < numCols - 1; col++)
	    {
		if ((*tableau)[numRows - 1][col] > maxCoeff)
		{
		    maxCoeff = (*tableau)[numRows - 1][col];
		    pivotCol = col;
		}
	    }
        }
	if ( (twoPhase && minCoeff >= -1 * ZERO_TOLERANCE) || 
             (!twoPhase && maxCoeff == ZERO_TOLERANCE) )
	{
	    sol->SetErrorCode(SOLVED);

            std::cout << "solved" << std::endl;
            DisplayMatrix(tableau);
            
            // evaluate the final matrix for the values of each decision variable
            for (dblmatrix::size_type col = 0; col < numDecisionVars; col++)
            {
                bool foundOne = false; // found a 1 in this column
                bool foundNonZero = false; // found a nonzero number (except 1st 1)
                dblmatrix::size_type solutionRow; 
                for (dblmatrix::size_type row = 0; row < numRows; row++)
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
            if ((*tableau)[numRows - 1][numCols - 1] == 0)
                sol->SetZValue(0);
            else
                sol->SetZValue(-1 * (*tableau)[numRows - 1][numCols - 1]);
            stay = false; // break out of the loop to return
	}
	else 
	{
	    // Check if all entries in pivotCol are <= 0
	    double maxVar = ZERO_TOLERANCE;
	    for (dblmatrix::size_type row = 0; row < numConstraints; row++)
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
		dblmatrix::size_type pivotRow;
		double minRatio = DBL_MAX;
		for (dblmatrix::size_type row = 0; row < numConstraints; row++)
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

void Solver::Pivot(dblmatrix* tableau, dblmatrix::size_type pivotRow, dblmatrix::size_type pivotCol)
{
    std::cout << "Pivoting on row " << pivotRow << " col " << pivotCol << std::endl;
    dblmatrix::size_type numRows = tableau->size();
    dblmatrix::size_type numCols = ((*tableau)[0]).size();
    double pivotNumber = (*tableau)[pivotRow][pivotCol];
    for (dblmatrix::size_type col = 0; col < numCols; col++)
    {
        if ((*tableau)[pivotRow][col] != 0) // so it doesn't produce -0
            (*tableau)[pivotRow][col] = (*tableau)[pivotRow][col] / pivotNumber;
    }
    for (dblmatrix::size_type row = 0; row < numRows; row++)
    {
        if (std::abs((*tableau)[row][pivotCol]) > ZERO_TOLERANCE && 
            row != pivotRow)
        {
            double multiple = (*tableau)[row][pivotCol] / 
                              (*tableau)[pivotRow][pivotCol];
            for (dblmatrix::size_type col = 0; col < numCols; col++)
            {
                (*tableau)[row][col] = (*tableau)[row][col] - 
                                       (multiple * (*tableau)[pivotRow][col]);
            }
        }
    }
}

