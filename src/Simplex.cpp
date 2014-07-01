/**
 *
 */

void Simplex::displayMatrix(double** matrix, int x, int y)
{
    for (unsigned long long i = 0; i < x; i++)
    {
        for (unsigned long long j = 0; j < y; j++)
        {
            printf("%7.2f", matrix[i][j]);
        }

        printf("\n");
    }

    printf("\n");
}

int Simplex::gcd(int x, int y)
{
    while (y != 0)
    {
        unsigned long long t = x % y;
        x = y;
        y = t;
    }

    return x;
}

int Simplex::choose(unsigned long long n, unsigned long long k)
{
    static const unsigned long long MAX_LONG =
        std::numeric_limits<unsigned long long>::max();
    unsigned long long r = 1;

    // check for invalid parameters
    if (k <= n)
    {
        bool overflow = false;

        for (unsigned long long d = 1; d <= k && !overflow; ++d, --n)
        {
            unsigned long long g = gcd(r, d);
            r /= g;
            unsigned long long t = n / (d / g);

            if (r > MAX_LONG / t)
                // overflow error
            {
                overflow = true;
            }
            else
            {
                r *= t;
            }
        }
    }

    return r;
}

void Simplex::lpToTable(LinearProgram* lp)
{

    tokenizeToMatrix(lp->getLeqConstraints(), 0);
    tokenizeToMatrix(lp->getEqConstraints(), numLeqConstraints);
    std::istringstream split(lp->getEquation());
    std::string token;

    for (unsigned long long i = 0; std::getline(split, token, ' '); i++)
    {
        std::istringstream(token) >> table[numConstraints][i];
    }
}

void Simplex::tokenizeToMatrix(LinkedList<std::string>* list, int start)
{
    LinkedList<std::string>::ListIterator iterator = list->iterator();
    for (int i = start; i < numLeqConstraints; i++)
    {
        int j = 0;
        std::istringstream split(leqConstraintsIter.next());
        std::string token;

        while (std::getline(split, token, ' '))
        {
            std::istringstream(token) >> table[i][j++];
        }

        j--;
        table[i][numDecisionVars + numConstraints] = table[i][j];
        table[i][j] = 0;
        table[i][numDecisionVars + i] = 1;
    }

}

void Simplex::pivot(int pivotRow, int pivotCol, int numRows, int numCols)
{
    std::cout << "pivoting on row " << pivotRow << " col " << pivotCol << std::endl;
    double pivotNumber = table[pivotRow][pivotCol];

    for (int col = 0; col < numCols; col++)
    {
        if (table[pivotRow][col] != 0) // so it doesn't produce -0
        {
            table[pivotRow][col] = table[pivotRow][col] / pivotNumber;
        }
    }

    for (int row = 0; row < numRows; row++)
    {
        if (std::abs(table[row][pivotCol]) > ZERO_TOLERANCE &&
            row != pivotRow)
        {
            double multiple = table[row][pivotCol] /
                              table[pivotRow][pivotCol];

            for (int col = 0; col < numCols; col++)
            {
                table[row][col] = table[row][col] -
                                    (multiple * table[pivotRow][col]);
            }
        }
    }
}
