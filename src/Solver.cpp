/**
 * This class represents a Linear Program solver (may later be extended
 * to solve other types of problems?). This is a Singleton class that receives
 * LinearProgram objects and returns LinearProgramSolutions.
 *
 * Version: 07/01/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 */

#include "Solver.h"
#include "Simplex.h"

LPSolution* Solver::solve(LinearProgram* lp)
{
    return Simplex(lp).solve();
}
