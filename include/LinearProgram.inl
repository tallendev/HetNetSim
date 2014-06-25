/**
 * Definition of inline functions for using the Linked List template.
 *
 * Version: 06/12/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 */

#ifndef LINEARPROGRAM_INL
#define LINEARPROGRAM_INL

/**
 * Add an inequality constraint to the list of constrains.
 *
 * Param: leqConstraint - The <= constraint to be added.
 */
inline void LinearProgram::addLeqConstraint(std::string leqConstraint)
{
    leqConstraints.add(leqConstraint);
}

/**
 * Add an equality constraint to the list of constrains.
 *
 * Param: eqConstraint - The = constraint to be added.
 */
inline void LinearProgram::addEqConstraint(std::string eqConstraint)
{
    eqConstraints.add(eqConstraint);
}

/**
 * Destructor for linear program. Currently does nothing.
 */
inline LinearProgram::~LinearProgram()
{
}

#endif
