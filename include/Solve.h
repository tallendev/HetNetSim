/**
 * This is an abstract class representing a worker object that does the work
 * of solving a problem.
 *
 * Version: 06/27/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 */

#ifndef SOLVE_H
#define SOLVE_H

class Solve
{
    public:
        virtual LPSolution* solve(LinearProgram*)=0;
}

#endif
