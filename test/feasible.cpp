// main program. currently used for testing

#include "Solver.h"
#include <iostream>

int main(void)
{
    LinearProgram* pTestProblem = new LinearProgram("5 4 3");
    pTestProblem->addLeqConstraint("2 3 1 5");
    pTestProblem->addLeqConstraint("4 1 2 11");
    pTestProblem->addLeqConstraint("3 4 2 8");
    LPSolution* answer = Solver::getInstance().solve(pTestProblem);
    std::cout << "answer error code = " << answer->getErrorCode() << std::endl;
    if (answer->getErrorCode() == 0) {
    std::cout << "z value: " << answer->getZValue() << std::endl;
    double* answervals = answer->getOptimalValues();
    std::cout << "answer values: " << answervals[0] << std::endl; 
    std::cout << "answer values: " << answervals[1] << std::endl; 
    std::cout << "answer values: " << answervals[2] << std::endl; 
    }
    delete pTestProblem;
    return 0;
}
