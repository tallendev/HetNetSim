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
 */
LPSolution::LPSolution() : errorCode(DEFAULT), optimalValues(0), 
                           numOptimalValues(0), zValue(0)
{
}

/**
 * Destructor for LPSolution
 * frees memory from the optimalValues array.
 */
LPSolution::~LPSolution()
{
    delete [] optimalValues;
}

/**
 * getErrorCode method.
 * This should be called whenever you recieve a solution to check
 * for errors (unbounded, infeasible, etc)
 */
int LPSolution::getErrorCode()
{
    return errorCode;
}

/**
 * setErrorCode method.
 * This is used in the Solver to indicate to the caller if the solution is valid.
 * A value of 0 means success.
 */
void LPSolution::setErrorCode(int code)
{
    errorCode = code;
}

/**
 * setOptimalValues method.
 * This is used by the Solver to save an array of floats
 * that are the optimal values for the decision variables.
 */
void LPSolution::setOptimalValues(double* inOptimalValues)
{
    optimalValues = inOptimalValues;
}

/**
 * getOptimalValues method.
 * You should call this if the error code is 0 and you want the
 * optimal values for the decision variables.
 */
double* LPSolution::getOptimalValues()
{
    return optimalValues;
}

/**
 * getNumOptimalValues method.
 * Returns the size of the optimalValues array, which is
 * the number of decision variables.
 */
int LPSolution::getNumOptimalValues()
{
    return numOptimalValues;
}

/**
 * setNumOptimalValues method.
 * This sets the numOptimalValues variable, which is used to 
 * keep track of the size of the optimalValues array.
 */
void LPSolution::setNumOptimalValues(int inNumOpt)
{
    numOptimalValues = inNumOpt;
}

/**
 * SetZValue method.
 * This sets the optimal value of the objective equation.
 */
void LPSolution::setZValue(double inZValue)
{
    zValue = inZValue;
}

/**
 * getZValue method.
 * This returns the optimal value of the objective equation.
 */
double LPSolution::getZValue()
{
    return zValue;
}
