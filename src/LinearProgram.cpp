/**
 * Concrete implementation of methods for a Linear Program.
 *
 * Version: 06/13/2014
 *
 * Author: Tyler Allen
 * Author: Matthew Leeds
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
 * getEquation method. Returns the equation as a string.
 */
std::string LinearProgram::getEquation()
{
    return equation;
}

/**
 * getLeqConstraints method. returns the linked list of <= constraint equations.
 */
LinkedList<std::string>* LinearProgram:: getLeqConstraints()
{
    return &leqConstraints;
}

/**
 * getEqConstraints method. returns the linked list of = constraint equations.
 */
LinkedList<std::string>* LinearProgram:: getEqConstraints()
{
    return &eqConstraints;
}
