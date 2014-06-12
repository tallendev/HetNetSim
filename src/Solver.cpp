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
 * Implementation of classic simplex method to solving linear programs. 
 */
LPSolution& Solver::SimplexSolve(LinearProgram& lp)
{
    //TODO: implement me
    static LPSolution sol; //created to make compile...
    return sol;
}


