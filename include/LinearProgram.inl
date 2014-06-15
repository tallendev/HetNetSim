/**
 * Definition of inline functions for using the Linked List template.
 *
 * Version: 06/12/2014
 * Author: Tyler Allen
 * Author: Matthew Leads
 */

#ifndef LINEARPROGRAM_INL
#define LINEARPROGRAM_INL

/**
 * Add a constraint to the list of constrains.
 *
 * Param: constrain - The constraint to be added.
 */
inline void LinearProgram::AddConstraint(std::string constraint)
{
    constraints.Add(constraint);
}

/**
 * Destructor for linear program. Currently does nothing.
 */
inline LinearProgram::~LinearProgram()
{
}

#endif
