// main program. currently used for testing

#include "Solver.h"
#include <iostream>

int main(void)
{
    LinearProgram* pTestProblem = new LinearProgram("5 4 3");
    pTestProblem->AddLeqConstraint("2 3 1 5");
    pTestProblem->AddLeqConstraint("4 1 2 11");
    pTestProblem->AddLeqConstraint("3 4 2 8");
    LPSolution answer = Solver::getInstance().SimplexSolve(pTestProblem);
    std::cout << "answer error code = " << answer.GetErrorCode() << std::endl;
    std::vector<double> answervals = answer.GetOptimalValues();
    std::cout << "answer values: " << answervals[0] << std::endl; 
    std::cout << "answer values: " << answervals[1] << std::endl; 
    std::cout << "answer values: " << answervals[2] << std::endl; 
    delete pTestProblem;
    return 0;
}
