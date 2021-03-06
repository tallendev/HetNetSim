/**
 * This class represents a Linear Program in our model. It contains our
 * optimization problem, variable limitations, as well as the list of
 * constraints.
 *
 * Author: Tyler Allen
 * Author: Matthew Leeds
 *
 * Version 07/24/14
 */

#ifndef LINEARPROGRAM_H
#define LINEARPROGRAM_H

#include <string>
#include "LinkedList.h"

class LinearProgram {
    private:
        /* The objective equation.*/
        std::string equation;
        // example: 2 3 -4
        // meaning: 2x1 + 3x2 - 4x3 = z
        /** A list of less than or equal constraints. */
        LinkedList<std::string> leqConstraints; // <= constraints
        /** List of equality constraints. */
        LinkedList<std::string> eqConstraints; // = constraints

    public:
        LinearProgram(std::string equation);
        ~LinearProgram();
        void addLeqConstraint(std::string leqConstraint);
        // example: 3 0 -7 42
        // meaning: 3x1 -7x3 <= 42
        void addEqConstraint(std::string eqConstraint);
        // example 5 4 3 9
        // meaning: 5x1 + 4x2 + 3x3 = 9
        std::string& getEquation();
        LinkedList<std::string>* getLeqConstraints();
        LinkedList<std::string>* getEqConstraints();

};

#include "LinearProgram.inl"

#endif
