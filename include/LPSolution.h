/**
 * This class represents a solution generated for a Linear Program by a Solver.
 *
 * Version: 06/12/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 */

#ifndef LPSOLUTION_H
#define LPSOLUTION_H

/**
 * This class will represent a linear program solution. 
 */
class LPSolution
{
    private:
        int errorCode;
        float* optimalValues; // an array of the optimal values for the decision variables
    public:
        LPSolution();
        ~LPSolution();
        int GetErrorCode();
        void SetErrorCode(int code);
        void SetOptimalValues(float optimalValues[]);
        float GetOptimalValues();

};

#endif

