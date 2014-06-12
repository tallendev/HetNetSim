/**
 * This class represents a Linear Program in our model. It contains our 
 * optimization problem, variable limitations, as well as the list of 
 * constraints. 
 */

#ifndef LINEARPROGRAM_H
#define LINEARPROGRAM_H

#include <string>
#include "LinkedList.h"

class LinearProgram
{
    private:
        std::string equation;
        LinkedList<std::string> constraints;

    public:
        LinearProgram(std::string equation);
        void addConstraint(std::string constraint);
        
};

#include "LinearProgram.inl"

#endif
