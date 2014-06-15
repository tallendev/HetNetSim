/**
 * Concrete implementation of methods for a Linear Program.
 *
 * Version: 06/13/2014
 *
 * Author: Tyler Allen
 * Author: Matthew Leads
 */

#include "LinearProgram.h"

/**
 * Constructor for linear program.
 *
 * Param: equation - the equation we are solving.
 */
LinearProgram::LinearProgram(std::string equation) : equation (equation)
{
}


/**
 * setObjective method
 *
 * Param: maxOrMin - if true, we try to maximize the equation.
 * Otherwise, try to minimize it.
 */
void LinearProgram::setObjective(bool maxOrMin)
{
    objective = maxOrMin;
}

/**
 * getObjective method. Returns the boolean value of objective.
 */
bool LinearProgram::getObjective() 
{
    return objective;
}

/**
 * getEquation method. Returns the equation as a string.
 */
std::string LinearProgram::getEquation()
{
    return equation;
}
