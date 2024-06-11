#include "Utils.h"

void Utils::DBOUT(const char* szFormat, ...)
{
    char szBuff[1024] = { 0 };
    va_list arg;
    va_start(arg, szFormat);
    vsnprintf_s(szBuff, sizeof(szBuff), szFormat, arg);
    va_end(arg);

    OutputDebugStringA(szBuff);
}