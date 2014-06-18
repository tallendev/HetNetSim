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
        LinkedList<std::string> leqConstraints; // <= constraints
        LinkedList<std::string> eqConstraints; // = constraints

    public:
        LinearProgram(std::string equation);
        ~LinearProgram();
        void AddLeqConstraint(std::string leqConstraint);
        // example: 3 0 -7 42
        // meaning: 3x1 -7x3 <= 42
        void AddEqConstraint(std::string eqConstraint);
        // example 5 4 3 9
        // meaning: 5x1 + 4x2 + 3x3 = 9
        std::string GetEquation();
        LinkedList<std::string>* GetLeqConstraints();
        LinkedList<std::string>* GetEqConstraints();
        // int ValidateFormat();
        
};

#include "LinearProgram.inl"

#endif
