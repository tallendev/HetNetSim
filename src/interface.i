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
    std::cerr << "Problem: " << problem << std::endl;
    LinearProgram* pTestProblem = new LinearProgram("5 4 3");
    pTestProblem->addLeqConstraint("2 3 1 5");
    pTestProblem->addLeqConstraint("4 1 2 11");
    pTestProblem->addLeqConstraint("3 4 2 8");
    LPSolution* answer = Solver::getInstance().solve(pTestProblem);
    std::cerr << "answer error code = " << answer->getErrorCode() << std::endl;
    std::ostringstream s;
    if (answer->getErrorCode() == 0) {
        double* answervals = answer->getOptimalValues();
        s << "z value: " << answer->getZValue() << "\n" << "answer values: " <<
              answervals[0] << " " << answervals[1] << " " << answervals[2] <<
              "\n";
    }
    delete answer;
    delete pTestProblem;
    return s.str();
}
%}
%include "std_string.i"
%include "interface.h"
%module solver
