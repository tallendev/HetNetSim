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

class Simplex : public Solve 
{
    private:
        unsigned int numDecisionVars;
        unsigned int numConstraints;
        unsigned int numLeqConstraints;
        unsigned int numEqConstraints;
        double** table;
        int tableX;
        int tableY;
   protected: 
        unsigned long long choose(int n, int k);
        int gcd(int x, int y);
        void Simplex::tokenizeToMatrix(LinkedList<std::string>* list, int start)
        virtual void displayMatrix(double** matrix, int x, int y);
        void lpToTable (LinearProgram* lp, double** table);
        void pivot (double** table, unsigned long long pivotrow,
               unsigned long long pivotcol);

    public:
       static constexpr double ZERO_TOLERANCE = 0.0001;
       static Simplex* simplexBuilder(LinearProgram*);

       virtual void solve() = 0;
        
}

#endif
