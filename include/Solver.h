/**
 * This class represents a Linear Program Solver (may later be extended 
 * to solve other types of problems?). This is a Singleton class that receives
 * LinearProgram objects and returns LPSolution references. This Solver
 * will be following the Singleton design pattern, as it should not have
 * state of its own.
 *
 * Version: 06/11/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 */

#ifndef SOLVER_H
#define SOLVER_H
#include "LinearProgram.h"
#include "LPSolution.h"
#include <sstream>

class Solver
{
    private:
        Solver() {};
        double** lpToTableau(LinearProgram* lp, int* numDecisionVars, int* numConstraints);
        void Pivot(double** tableau, int* pivotRow, int* pivotCol, int* numDecisionVars, int* numConstraints);
        // Don't implement these, they prevent copies from being made.
        Solver(Solver const& copy);
        Solver& operator=(Solver const& copy);

    public:
    /**
     * Returns a static instance of Solver object. This allows the Solver to be
     * created only when needed (lazy), automatically allocated/deallocated, and 
     * is key in the implementation of the Singleton Pattern.
     *
     * Return: Returns the sole instance of the Solver class.
     */ 
        static Solver& getInstance()
        {
            static Solver solver;
            return solver;
        }
        LPSolution SimplexSolve(LinearProgram* lp);
        
};

#endif

