/**
 * This class represents a Linear Program Solver (may later be extended 
 * to solve other types of problems?). This is a Singleton class that receives
 * LinearProgram objects and returns LinearProgramSolutions. This Solver
 * will be following the Singleton design pattern, as it should not have
 * state of its own.
 *
 * Version: 06/11/2014
 * Author: Tyler Allen
 * Author: Matthew Leads
 */

#ifndef SOLVER_H
#define SOLVER_H
#include "LPSolution.h"


class Solver
{
    private:
        Solver() {};

        // Don't implement these, they prevent copies from being made.
        Solver(Solver const& copy);
        Solver& operator=(Solver const& copy);

    public:
        static Solver& getInstance()
        {
            static Solver solver;
            return solver;
        }

        LPSolution& SimplexSolve();
};

#endif

