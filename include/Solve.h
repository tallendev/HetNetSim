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

#include "LPSolution.h"
#include "LinearProgram.h"

class Solve
{
    public:
        /** This function is intended to be the entry method into our Solve 
         * type.
         */
        virtual LPSolution* solve()=0;
};

#endif
