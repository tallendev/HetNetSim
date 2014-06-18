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
inline void LinearProgram::AddLeqConstraint(std::string leqConstraint)
{
    leqConstraints.Add(leqConstraint);
}

/**
 * Add an equality constraint to the list of constrains.
 *
 * Param: eqConstraint - The = constraint to be added.
 */
inline void LinearProgram::AddEqConstraint(std::string eqConstraint)
{
    eqConstraints.Add(eqConstraint);
}

/**
 * Destructor for linear program. Currently does nothing.
 */
inline LinearProgram::~LinearProgram()
{
}

#endif
