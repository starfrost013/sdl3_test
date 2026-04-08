
#include <util/util.hpp>

namespace Capy
{
    // single
    float Util_RandomSingle()
    {
        std::mt19937 rng(std::random_device{}());

        return std::generate_canonical<float, 128>(rng);
    }

    // double
    double Util_RandomDouble()
    {
        std::mt19937 rng(std::random_device{}());

        return std::generate_canonical<double, 128>(rng);
    }

    // get token separated part n of the string
    char* Util_GetTokenSeparatedPart(char* fmt, const char* delim, uint32_t n)
    {
        /* sanity checks */

        if (!fmt ||  !delim)
            return nullptr; 

        size_t fmtLen = strlen(fmt);

        if (fmtLen < 0)
            return nullptr;

        char* tok = strtok(fmt, delim);

        for (uint32_t i = 0; i < n; i++)
        {
            tok = strtok(NULL, delim);

            if (!tok)
                break; 
        }

        return tok;
    }
}
