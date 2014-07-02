/**
 * This class represents a Linear Program solver (may later be extended
 * to solve other types of problems?). This is a Singleton class that receives
 * LinearProgram objects and returns LPSolution references. This solver
 * will be following the Singleton design pattern, as it should not have
 * state of its own.
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
        unsigned int numDecisionVars;
        unsigned int numConstraints;
        unsigned int numLeqConstraints;
        unsigned int numEqConstraints;
        double** table;
        int numRows;
        int numCols;
   protected: 
        unsigned long long choose(int n, int k);
        int gcd(int x, int y);
        void tokenizeToMatrix(LinkedList<std::string>* list, int start);
        virtual void displayMatrix(double** matrix, int x, int y);
        void lpToTable (LinearProgram* lp);
        void pivot(double** table, int pivotRow, int pivotCol, int numRows, int numCols);
        bool checkFeasibility ();
        void optimize(double** table, LPSolution* sol, int curRows, int curCols, int); 
        bool isTwoPhase();
        
    public:
        static constexpr double ZERO_TOLERANCE = 0.0001;
        
        Simplex(LinearProgram* lp);
        virtual LPSolution* solve();
};

#endif
