/**
 * Concrete implementation of LP solution methods.
 *
 * Version: 06/12/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 */

#include "LPSolution.h"

/**
 * Constructor for LPSolution class.
 * errorCode 50 means the solution was never found.
 * allocate memory for the answer.
 */
LPSolution::LPSolution() : errorCode(100)
{
    std::vector<float> optimalValues;
}

/**
 * Destructor for LPSolution
 * frees memory from the optimalValues array.
 */
LPSolution::~LPSolution()
{
}

/**
 * GetErrorCode method.
 * This should be called whenever you recieve a solution to check 
 * for errors (unbounded, infeasible, etc)
 */
int LPSolution::GetErrorCode()
{
    return errorCode;
}

/**
 * SetErrorCode method.
 * This is used in the Solver to indicate to the caller if the solution is valid.
 * A value of 0 means success.
 */
void LPSolution::SetErrorCode(int code)
{
    errorCode = code;
}

/**
 * SetOptimalValues method.
 * This is used by the Solver to save an array of floats
 * that are the optimal values for the decision variables.
 */
void LPSolution::SetOptimalValues( std::vector<float> inOptimalValues)
{
    optimalValues = inOptimalValues;
}

/**
 * GetOptimalValues method.
 * You should call this if the error code is 0 and you want the 
 * optimal values for the decision variables.
 */
std::vector<float> LPSolution::GetOptimalValues()
{
    return optimalValues;
}
