/**
 * This class represents a Linear Program Solver (may later be extended 
 * to solve other types of problems?). This is a Singleton class that receives
 * LinearProgram objects and returns LinearProgramSolutions. 
 *
 * Version: 06/16/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 */

#include "Solver.h"
#include <string>
#include <cstdlib>
#include <iostream>

// a function for counting the number of spaces in a string
int count_spaces(std::string s)
{
    int count = 0;
    for (unsigned int i = 0; i < s.size(); i++)
        if (s[i] == ' ')
            count++;
    return count;
}

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

    // calculate the # of decision vars and # of constraints
    int numDecisionVars = count_spaces(lp->GetEquation()) + 1;
    LinkedList<std::string> listOfConstraints = lp->GetConstraints();
    int numConstraints = listOfConstraints.GetSize();

    // initialize a (n+1) x (n+m+1) matrix and zero-fill it
    int tableau[numConstraints + 1][numDecisionVars + numConstraints + 1];
    for (int i = 0; i < numConstraints + 1; i++)
    {
        for (int j = 0; j < numDecisionVars + numConstraints + 1; j++)
            tableau[i][j] = 0;
    }
    // get an iterator for the constraints
    LinkedList<std::string>::ListIterator constraintsIter(&listOfConstraints);
    // for each constraint equation, parse it and fill in the array
    std::string constraintEqn = "";
    for (int row = 0; row < numConstraints; row++)
    {
        if (constraintsIter.HasNext())
            constraintEqn = constraintsIter.Next();
        long unsigned int lastSpace = -1;
        for (int var = 0; var < numDecisionVars; var++)
        {
            long unsigned int nextSpace = constraintEqn.find(' ', lastSpace + 1);
            std::string substring = constraintEqn.substr(lastSpace + 1, nextSpace - lastSpace - 1);
            tableau[row][var] = atoi(substring.c_str());
            lastSpace = nextSpace;
        }
        int bValue = atoi(constraintEqn.substr(lastSpace + 1, constraintEqn.length() - lastSpace - 1).c_str());
        tableau[row][numDecisionVars + numConstraints] = bValue;
        tableau[row][numDecisionVars + row] = 1;
    }
    // fill in the last row according to the objective equation coefficients
    std::string objEqn = lp->GetEquation();
    long unsigned int lastSpace = -1;
    for (int var = 0; var < numDecisionVars - 1; var++)
    {
        long unsigned int nextSpace = objEqn.find(' ', lastSpace + 1);
        std::string substring = objEqn.substr(lastSpace + 1, nextSpace - lastSpace - 1);
        tableau[numConstraints][var] = atoi(substring.c_str());
        lastSpace = nextSpace;
    }
    int lastCoeff = atoi(objEqn.substr(lastSpace + 1, objEqn.length() - lastSpace - 1).c_str());
    tableau[numConstraints][numDecisionVars - 1] = lastCoeff;
    // The tableau (matrix) should now be filled.
    // Display the matrix (for debugging).
    for (int i = 0; i < numConstraints + 1; i++)
    {
        for (int j = 0; j < numDecisionVars + numConstraints + 1; j++)
            std::cout << tableau[i][j] << " ";
        std::cout << std::endl;
    }

    // determine if we are at an optimal solution
    int maxCoeff = 0;
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
    }
    else
    {
        // pivot on pivotCol
    }

    static LPSolution sol; //created to make compile...
    return sol;
}


