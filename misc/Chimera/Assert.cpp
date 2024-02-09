#include "Assert.h"




void debugAssert(bool expressionResult, const char* expression, const char* module, int line)
{
	if (!expressionResult)
	{
		char text[256];
		sprintf(text, "Assertion failed\n%s\nin %s at line %d", expression, module, line);
#ifdef _WIN32
		MessageBox(0, text, "Assertion failed", MB_ICONERROR);
		RaiseException(EXCEPTION_BREAKPOINT, 0, 0, 0);
#else
		throw __FUNCTION__;
#endif
	}
}

