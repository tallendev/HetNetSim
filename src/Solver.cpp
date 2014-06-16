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
    int** tableau = lpToTableau(lp);
    /**
     * The tableau (matrix) should now be filled.
     * For debugging, here's code for displaying the matrix.
     */
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 7; j++) {
            std::cout << tableau[i][j] << " ";
        }
        std::cout << std::endl;
    }

    static LPSolution sol; //created to make compile...
    return sol;
}

int** Solver::lpToTableau(LinearProgram* lp)
{
    LinkedList<std::string> listOfConstraints = lp->GetConstraints();
    int numConstraints = listOfConstraints.GetSize();
    int numDecisionVars = 0;
    std::istringstream countVars(lp->GetEquation());
    std::string token;
    while (std::getline(countVars, token, ' '))
    {
        numDecisionVars++;
    }

    int** tableau = new int*[numConstraints + 1]();  //implement safe calloc
    for (int i = 0; i < numConstraints + 1; i++)
    {
        tableau[i] = new int[(numDecisionVars + numConstraints + 1) * numConstraints]();
    }
    LinkedList<std::string>::ListIterator constraintsIter(&listOfConstraints);
    for (int i = 0; i < numConstraints; i++)
    {
        int j = 0;
        std::istringstream split(constraintsIter.Next());
        while (std::getline(split, token, ' '))
        {
            std::istringstream(token) >> tableau[i][j++];
        }
        j--;
        tableau[i][numDecisionVars + numConstraints] = tableau[i][j];
        tableau[i][j] = 0;
        tableau[i][numDecisionVars + i] = 1;
    }
    
    std::istringstream split(lp->GetEquation());
    for (int i = 0; std::getline(split, token, ' '); i++)
    {
        std::istringstream(token) >> tableau[numConstraints][i];
    }
    return tableau;
}


