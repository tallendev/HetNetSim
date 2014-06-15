// main program. currently used for testing

#include "Solver.h"
#include <iostream>

int main(void)
{
    LinearProgram testProblem("5 4 3");
    testProblem.SetObjective(true);
    testProblem.AddConstraint("2 3 1 5");
    testProblem.AddConstraint("4 1 2 11");
    testProblem.AddConstraint("3 4 2 8");
    Solver::getInstance().SimplexSolve(testProblem);
    return 0;
}
