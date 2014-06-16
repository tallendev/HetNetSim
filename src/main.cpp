// main program. currently used for testing

#include "Solver.h"
#include <iostream>

int main(void)
{
    LinearProgram* pTestProblem = new LinearProgram("5 4 3");
    pTestProblem->SetObjective(true);
    pTestProblem->AddConstraint("2 3 1 5");
    pTestProblem->AddConstraint("4 1 2 11");
    pTestProblem->AddConstraint("3 4 2 8");
    Solver::getInstance().SimplexSolve(pTestProblem);
    delete pTestProblem;
    return 0;
}
