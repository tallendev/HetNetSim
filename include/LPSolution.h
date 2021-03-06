/**
 * This class represents a solution generated for a Linear Program by a Solver.
 *
 * Version: 06/12/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 */

#ifndef LPSOLUTION_H
#define LPSOLUTION_H

#include <vector>

/**
 * This class will represent a linear program solution.
 */
class LPSolution {
    private:
        /**
         * Constants-
         * A zero tolerance in case of rounding errors, and
         * errorCode values for LPSolution
         */
        int errorCode;
        double* optimalValues; // the optimal values for the decision variables
        int numOptimalValues; // the size of optimalValues
        double zValue; // optimal value of the objective equation
    public:
        /* These constants represent appropriate error codes. Any but 0 indicate
         * an error. */
        
        /* Solved. */
        static const int SOLVED = 0;
        /* There is a bug in the solver if this value is set after a call to
         * a solver. Report to the devs!*/
        static const int DEFAULT = 100; //FIXME: necessary?
        /** The solution is unbounded. */
        static const int UNBOUNDED = 200;
        /** We have exceeded the theoretical maximum number of iterations - 
         * the problem cycles.*/
        static const int EXCEEDED_MAX_ITERATIONS = 300;
        /** The Linear Program is not feasible.*/
        static const int INFEASIBLE = 400;

        LPSolution();
        ~LPSolution();
        int getErrorCode();
        void setErrorCode(int code);
        void setOptimalValues(double* optimalValues);
        double* getOptimalValues();
        int getNumOptimalValues();
        void setNumOptimalValues(int numOptimalValues);
        void setZValue(double zValue);
        double getZValue();

};

#endif

