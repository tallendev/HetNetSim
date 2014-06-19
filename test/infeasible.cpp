// main program. currently used for testing

#include "Solver.h"
#include <iostream>

int main(void)
{
    LinearProgram* pTestProblem = new LinearProgram("5 4 3 2");
    pTestProblem->AddLeqConstraint("1 3 2 4 5");
    pTestProblem->AddLeqConstraint("3 1 2 1 4");
    pTestProblem->AddLeqConstraint("0 0 -1 0 0");
    pTestProblem->AddLeqConstraint("0 0 0 -1 0");
    pTestProblem->AddEqConstraint("5 3 3 3 9");
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
