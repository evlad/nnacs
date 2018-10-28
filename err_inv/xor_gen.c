#include <stdio.h>
#include <stdlib.h>

int
main (int argc, char** argv)
{
    if(argc != 4) {
	fprintf(stderr, "Usage: %s NumOfSteps XorArgsFile XorResFile\n",
		argv[0]);
	return 1;
    } else {
	double	x1, x2, dx;
	int	i1, i2, nSteps = atoi(argv[1]);
	FILE	*fpXorArgs = fopen(argv[2], "w");
	FILE	*fpXorRes = fopen(argv[3], "w");

	dx = 1.0 / (nSteps - 1);
	for(i1 = 0; i1 < nSteps; ++i1) {
	    for(i2 = 0; i2 < nSteps; ++i2) {
		fprintf(fpXorArgs, "%g\t%g\n", i1 * dx, i2 * dx);
		if((i1 * dx <= 0.5 && i2 * dx > 0.5) ||
		   (i1 * dx > 0.5 && i2 * dx <= 0.5)) {
		    fprintf(fpXorRes, "1\n");
		} else {
		    fprintf(fpXorRes, "0\n");
		}
	    }
	}
	fclose(fpXorRes);
	fclose(fpXorArgs);
    }
    return 0;
}
