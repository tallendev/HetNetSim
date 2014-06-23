// main program. currently used for testing

#include "Solver.h"
#include <iostream>

int main(void)
{
    LinearProgram* pTestProblem = new LinearProgram("1 -1 1");
    pTestProblem->AddLeqConstraint("2 -1 2 4");
    pTestProblem->AddLeqConstraint("2 -3 1 -5");
    pTestProblem->AddLeqConstraint("-1 1 -2 -1");
    LPSolution answer = Solver::getInstance().SimplexSolve(pTestProblem);
    std::cout << "answer error code = " << answer.GetErrorCode() << std::endl;
    if (answer.GetErrorCode() == 0) {
    std::cout << "z value: " << answer.GetZValue() << std::endl;
    std::vector<double> answervals = answer.GetOptimalValues();
    std::cout << "answer values: " << answervals[0] << std::endl; 
    std::cout << "answer values: " << answervals[1] << std::endl; 
    std::cout << "answer values: " << answervals[2] << std::endl; 
    }
    delete pTestProblem;
    return 0;
}
