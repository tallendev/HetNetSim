/**
 * This class is represents the entry point for a type of problem to be 
 * solved. It is intended to provide a black-box approach to solving these 
 * problems so that the user need not know specific details to have their
 * problem solved. This class uses the Singleton Design Pattern, and with the 
 * addition of more solving methods would be an implementation of the 
 * builder pattern as well.
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
#include <vector>

class Solver {
    private:

        // Don't implement these, they prevent copies from being made.
        Solver() {};
        Solver(Solver const &copy);
        Solver &operator=(Solver const &copy);

public:
    /**
     * Returns a static instance of solver object. This allows the solver to be
     * created only when needed (lazy), automatically allocated/deallocated, and
     * is key in the implementation of the Singleton Pattern.
     *
     * Return: Returns the sole instance of the solver class.
     */
    static Solver &getInstance() {
        static Solver solver;
        return solver;
    }
    LPSolution* solve(std::string& problem);

};

#endif

