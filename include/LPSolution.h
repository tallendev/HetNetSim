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
class LPSolution
{
    private:
        int errorCode;
        std::vector<double> optimalValues; // the optimal values for the decision variables
    public:
        LPSolution();
        ~LPSolution();
        int GetErrorCode();
        void SetErrorCode(int code);
        void SetOptimalValues(std::vector<double> optimalValues);
        std::vector<double> GetOptimalValues();

};

#endif

