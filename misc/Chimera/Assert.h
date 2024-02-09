#pragma once

#include "Windows.h"
#include <stdio.h>

extern void debugAssert(bool expressionResult, const char* expression, const char* module, int line);

#define DBG(expr) debugAssert(!!(expr), #expr, __FUNCTION__, __LINE__)
#define ASSERT DBG
