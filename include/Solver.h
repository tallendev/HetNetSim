    /**
     * This class represents a Linear Program solver (may later be extended
     * to solve other types of problems?). This is a Singleton class that receives
     * LinearProgram objects and returns LPSolution references. This solver
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
#include <vector>

class Solver {
    private:
        void solve(double** table, LPSolution* sol, bool twoPhasek);
        void lpToTable (LinearProgram* lp, double** table);
        void pivot (double** table, unsigned long long pivotrow,
                    unsigned long long pivotcol);
        bool checkFeasibility (double** table);
        void displayMatrix(double** matrix);
        unsigned long long choose(unsigned long long n, unsigned long long k);
        unsigned long long gcd(unsigned long long x, unsigned long long y);

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
    LPSolution solveLP(LinearProgram* lp);

};

#endif

