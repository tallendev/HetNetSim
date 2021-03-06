/**
 * Main program that is the entry point when used as a shared library. This 
 * file needs to be ran appropriately through SWIG to generate the C++/PHP 
 * bindings - an example is located in the Makefile if you wish to compile it 
 * seperately. 
 *
 *
 * Takes in a string that is the problem in the format:
 * "objeqn;ineq,ineq,;eq,eq,;"
 * and returns the solution, a string with the z value and optimal
 * values for the decision variables.
 *
 * Authors: Tyler Allen, Matthew Leeds
 * Version - 07/24/14
 */
%{
#include "interface.h"
#include "LinearProgram.h"
#include "LPSolution.h"
#include "Solver.h"
#include <sstream>
#include <iostream>

std::string cppMain(std::string problem)
{
    #ifdef SERVER_DEBUG

        std::cerr << "Problem: " << problem << std::endl;
    #endif 

    LPSolution* answer = Solver::getInstance().solve(problem);

    #ifdef SERVER_DEBUG
        std::cerr << "answer error code = " << answer->getErrorCode() << std::endl;
    #endif
    std::ostringstream s;
    if (answer->getErrorCode() == 0) {
        double* answervals = answer->getOptimalValues();
        s << "z value: " << answer->getZValue() << " <br /> " << "answer values: ";
        for (int i = 0; i < answer->getNumOptimalValues(); i++)
        {
            s << answervals[i] << " ";
        }
        s << "\n";
    }
    delete answer;

    return s.str();
}
%}
%include "std_string.i"
%include "interface.h"
%module solver
