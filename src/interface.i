// main program. currently used for testing
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
    /*
    LinearProgram* pTestProblem = new LinearProgram("5 4 3");
    pTestProblem->addLeqConstraint("2 3 1 5");
    pTestProblem->addLeqConstraint("4 1 2 11");
    pTestProblem->addLeqConstraint("3 4 2 8");
    */
    LPSolution* answer = Solver::getInstance().solve(problem);

    #ifdef SERVER_DEBUG
        std::cerr << "answer error code = " << answer->getErrorCode() << std::endl;
    #endif
    std::ostringstream s;
    if (answer->getErrorCode() == 0) {
        double* answervals = answer->getOptimalValues();
        s << "z value: " << answer->getZValue() << "\n" << "answer values: " <<
              answervals[0] << " " << answervals[1] << " " << answervals[2] <<
              "\n";
    }
    delete answer;

    return s.str();
}
%}
%include "std_string.i"
%include "interface.h"
%module solver
