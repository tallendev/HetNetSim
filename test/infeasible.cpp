// main program. currently used for testing

#include "Solver.h"
#include <iostream>

int main(void)
{
    LinearProgram* pTestProblem = new LinearProgram("3 -7");
    pTestProblem->addLeqConstraint("5 10 10");
    pTestProblem->addEqConstraint("-3 -10 -10");
    pTestProblem->addLeqConstraint("-8 -7 6");
    LPSolution* answer = Solver::getInstance().solve(pTestProblem);
    std::cout << "answer error code = " << answer->getErrorCode() << std::endl;
    if (answer->getErrorCode() == 0) {
    std::cout << "z value: " << answer->getZValue() << std::endl;
    double* answervals = answer->getOptimalValues();
    std::cout << "answer values: " << answervals[0] << std::endl; 
    std::cout << "answer values: " << answervals[1] << std::endl; 
    }
    delete pTestProblem;
    return 0;
}
