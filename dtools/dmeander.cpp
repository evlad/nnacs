/* dmeander.cpp */

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>


/***********************************************************************
 * Generate meander of given length, with given period and phase shift.
 * Test:
 * ./dmeander 5 2 
 * 1
 * 1
 * -1
 * -1
 * 1
 ***********************************************************************/
int main (int argc, char* argv[])
{
    if(3 != argc && 4 != argc) {
	fprintf(stderr, "Usage: dmeander Length HalfPeriod [PhaseShift]\n");
	return 1;
    }

    int	nLength = atoi(argv[1]);
    if(nLength <= 0) {
	fprintf(stderr, "Length must be positive\n");
	return 1;
    }
    int	iHalfPeriod = atoi(argv[2]);
    if(iHalfPeriod <= 0) {
	fprintf(stderr, "Half of period must be positive\n");
	return 1;
    }
    int	iPeriod = 2 * iHalfPeriod;
    int iPhase = (4 == argc)? atoi(argv[3]): 0;

    int	i, j = iPhase % iPeriod;
    for(i = 0; i < nLength; ) {
	if(j < iHalfPeriod) {
	    printf("1\n");
	    ++j;
	} else if(j < iPeriod) {
	    printf("-1\n");
	    ++j;
	} else {
	    j = 0;
	    continue;
	}
	++i;
    }
    return 0;
}
