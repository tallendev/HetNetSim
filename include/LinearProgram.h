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
        // example: 2 3 -4
        // meaning: 2x1 + 3x2 - 4x3 = z
        LinkedList<std::string> constraints;

    public:
        LinearProgram(std::string equation);
        ~LinearProgram();
        void AddConstraint(std::string constraint);
        // example: 3 0 -7 42
        // meaning: 3x1 -7x3 <= 42
        std::string GetEquation();
        LinkedList<std::string>* GetConstraints();
        // int ValidateFormat();
        
};

#include "LinearProgram.inl"

#endif
