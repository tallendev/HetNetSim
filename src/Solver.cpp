/**
 * This class represents a Linear Program solver (may later be extended
 * to solve other types of problems?). This is a Singleton class that receives
 * LinearProgram objects and returns LinearProgramSolutions.
 *
 * Version: 07/01/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 */

#include "Solver.h"
#include "Simplex.h"

/**
 * This function parses out the Linear Program from a string into a 
 * LinearProgram object. This does minimal error checking and errors will 
 * happen if the correct format is not followed! Please error check
 * beforehand until this function is improved!
 *
 * Afterwards, the program is sent off to be solved by the appropriate Solver
 * (currently only the simplex method is supported.)
 *
 * Param: problem - The Linear Program in string form.
 * Return: The final solution.
 */
LPSolution* Solver::solve(std::string& problem)
{
    size_t firstsemicolon = problem.find(";", 0);
    size_t secondsemicolon = problem.find(";", firstsemicolon + 1);
    size_t thirdsemicolon = problem.find(";", secondsemicolon + 1);
    LinearProgram* lp = new LinearProgram(problem.substr(0, firstsemicolon));

    #ifdef SERVER_DEBUG
        std::cerr << "objeqn: " << problem.substr(0, firstsemicolon) << std::endl;
    #endif
    
    if (secondsemicolon > firstsemicolon + 1)
    {
        std::string str = problem.substr(firstsemicolon + 1, 
                                         secondsemicolon - firstsemicolon - 1);
        std::istringstream split(str);
        std::string token;
        while (std::getline(split, token, ','))
        {
            lp->addLeqConstraint(token);
            #ifdef SERVER_DEBUG
                std::cerr << "ineq: " << token << std::endl;
            #endif
        }
    }
    if (thirdsemicolon > secondsemicolon + 1)
    {
        std::string str = problem.substr(secondsemicolon + 1, 
                                         thirdsemicolon - secondsemicolon - 1);
        std::istringstream split(str);
        std::string token;
        while (std::getline(split, token, ','))
        {
            lp->addLeqConstraint(token);
            #ifdef SERVER_DEBUG
                std::cerr << "ineq: " << token << std::endl;
            #endif
        }
    }
    LPSolution* sol = Simplex(lp).solve();
    delete lp;
    return sol;
}

