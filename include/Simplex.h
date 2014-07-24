/**
 * This class represents an implementation of the Two-phase simplex method. 
 * It stores the necessary state for solving the problem and contains methods
 * for solving the problem.
 *
 * Version: 06/27/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 */

#ifndef SIMPLEX_H
#define SIMPLEX_H
#include "Solve.h"
#include "utils.h"
#include "float.h"
#include <cmath>
#include <iostream>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string.h>
#include <cstdio>
#include <sstream>

class Simplex : public Solve 
{
    private:
        /** The counted number of decision variables in the LP. */
        int numDecisionVars;
        /** The number of total constraints. */
        int numConstraints;
        /** The number of less than or equal constraints. */
        int numLeqConstraints;
        /** The number of equality constraints. */
        int numEqConstraints;
        /** The table we are storing our representation of the problem into. */
        double** table;
        /** Number of rows in the table. */
        int numRows;
        /** Number of columns. */ 
        int numCols;
   protected: 
        unsigned long long choose(int n, int k);
        unsigned long long gcd(unsigned long long x, unsigned long long y);
        void tokenizeToMatrix(LinkedList<std::string>* list, int start);
        virtual void displayMatrix(double** matrix, int x, int y);
        void lpToTable (LinearProgram* lp);
        void pivot(double** table, int pivotRow, int pivotCol, int numRows, int numCols);
        bool checkFeasibility ();
        void optimize(double** table, LPSolution* sol, int curRows, int curCols, int); 
        bool isTwoPhase();
        
    public:
        /**
         * Attempt to prevent rounding error for zero. May need to be reduced
         * in the future to improve precision.
         */
        static constexpr double ZERO_TOLERANCE = 0.0001;
        
        Simplex(LinearProgram* lp);
        virtual LPSolution* solve();
};

#endif
