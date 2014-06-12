/**
 * This class represents a Linear Program Solver (may later be extended 
 * to solve other types of problems?). This is a Singleton class that receives
 * LinearProgram objects and returns LinearProgramSolutions. 
 *
 * Version: 06/11/2014
 * Author: Tyler Allen
 * Author: Matthew Leads
 */

#include "Solver.h"


/**
 * Returns a static instance of Solver object. This allows the Solver to be
 * created only when needed (lazy), automatically allocated/deallocated, and 
 * is key in the implementation of the Singleton Pattern.
 *
 * Return: Returns the sole instance of the Solver class.
 */
LPSolution& Solver::SimplexSolve()
{
    //TODO: implement me
    static LPSolution sol; //created to make compile...
    return sol;
}


