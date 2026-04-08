/* Utility Headers */

#pragma once 
#include <cstdint>
#include <cstring>
#include <random>

namespace Capy
{
    float Util_RandomSingle();
    double Util_RandomDouble();
    char* Util_GetTokenSeparatedPart(char* fmt, const char* delim, uint32_t n);
}
