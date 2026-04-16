/* test_vsscanf.cpp */
//#include "vsscanf.win.c"
#include <stdio.h>
#include <stdarg.h>

#ifndef __GNUC__
extern "C" int vsscanf (const char* s, const char* f, va_list vargs);
#endif /* __GNUC__ */

void
getf (const char* szData, const char* szFormat, ...)
{
    va_list argptr;

    va_start(argptr, szFormat);
    vsscanf(szData, szFormat, argptr);

    va_end(argptr);
}

int
main ()
{
	char s[] = "10 20";
	int n1, n2;
	printf("test vsscanf(\"%s\")\n", s);
	getf(s, "%d %d", &n1, &n2);
	printf("n1=%d n2=%d\n", n1, n2);
	return 0;
}
