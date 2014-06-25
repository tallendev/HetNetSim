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
        int errorCode;
        double* optimalValues; // the optimal values for the decision variables
        double zValue; // optimal value of the objective equation
    public:
        LPSolution();
        ~LPSolution();
        int getErrorCode();
        void setErrorCode(int code);
        void setOptimalValues(double* optimalValues);
        double* getOptimalValues();
        void setZValue(double zValue);
        double getZValue();

};

#endif

