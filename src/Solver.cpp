/**
 * This class represents a Linear Program Solver (may later be extended 
 * to solve other types of problems?). This is a Singleton class that receives
 * LinearProgram objects and returns LinearProgramSolutions. 
 *
 * Version: 06/15/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 */

#include "Solver.h"
#include "float.h"
#include <string>
#include <cstdlib>
#include <iostream>

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
    //LinkedList<std::string>* listOfConstraints = lp->GetConstraints();
    //int numConstraints = listOfConstraints->GetSize();
    int numConstraints = lp->GetConstraints()->GetSize();
    float** tableau = lpToTableau(lp, &numDecisionVars, &numConstraints);
    // TODO: check for infeasibility
    // TODO: Limit iterations to at most (n+m choose m)

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
        // populate LPSolution and return
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
            // Pivot(tableau, pivotRow, pivotCol, &numDecisionVars, &numConstraints);
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


/*
void Solver::Pivot(int** tableau, int pivotRow, int pivotCol, int* numDecisionVars, int* numConstraints)
{
    int pivotNumber = (*tableau)[pivotRow][pivotCol];
    // this entire matrix needs to be re-implemented as double
*/
