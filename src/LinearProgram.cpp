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
 * GetEquation method. Returns the equation as a string.
 */
std::string LinearProgram::GetEquation()
{
    return equation;
}

/**
 * GetConstraints method. returns the linked list of constraint equations.
 */
LinkedList<std::string>* LinearProgram:: GetConstraints()
{
    return &constraints;
}
