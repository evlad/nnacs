#include <math.h>
#include <stdio.h>

typedef double	NaReal;

int
ccf1c (NaReal F1[], NaReal F2[], int n, NaReal B[], int ip, int ind1, int ind2);

/* Right output:
$ ./TestCCF
f1: 1 0 0 0 0
f2: 0 1 0 0 0
XC: 0 0 0.25
f1: 1 0 0 0 0
f2: 0 1 0 0 0
*/

int main ()
{
    NaReal  f1[] = {1.0, 0.0, 0.0, 0.0, 0.0};
    NaReal  f2[] = {0.0, 1.0, 0.0, 0.0, 0.0};
    NaReal  xc[] = {-1.0, -1.0, -1.0};

    int	i;
    printf("f1:");
    for(i = 0; i < 5; ++i)
	printf(" %g", f1[i]);
    printf("\n");
    printf("f2:");
    for(i = 0; i < 5; ++i)
	printf(" %g", f2[i]);
    printf("\n");

    ccf1c(f1, f2, 5, xc, 1, 0, 0);

    printf("XC:");
    for(i = 0; i < 3; ++i)
	printf(" %g", xc[i]);
    printf("\n");

    printf("f1:");
    for(i = 0; i < 5; ++i)
	printf(" %g", f1[i]);
    printf("\n");
    printf("f2:");
    for(i = 0; i < 5; ++i)
	printf(" %g", f2[i]);
    printf("\n");

    return 0;
}


int
ccf1c (NaReal F1[], NaReal F2[], int n, NaReal B[], int ip, int ind1, int ind2)
{
    int	imax, ierr, i, j, k;
    NaReal sum, average1, average2, b0, r01, r02, zero = 1e-6;

    ierr = 0;
	
    if (ind2 != 0) {
	average1 = 0;
	average2 = 0;

	for(j = 0; j < n; ++j) {
	    average1 += F1[j];
	    average2 += F2[j];
	}
	average1 /= n;
	average2 /= n;

	// Minus average
	for(j = 0; j < n; ++j) {
	    F1[j] -= average1;
	    F2[j] -= average2;
	}
    }

    if (ind1 == 0) {
	for(k = 1; k <= ip; ++k) {
	    sum = 0;
	    for(i = 0; i < n-k; ++i)
		sum += F1[i+k] * F2[i];

	    B[ip-k] = sum / (n-k);
	}

	for(k = 0; k <= ip; ++k) {
	    sum = 0;
	    for(i = 0; i < n-k; ++i)
		sum += F1[i] * F2[i+k];

	    B[ip+k] = sum / (n-k);
	}
    }
    else {
      // Normalized CCF
      for(k = 0; k < ip; ++k) {
	sum = 0.0;
	r01 = 0.0;
	r02 = 0.0;

	for(i = 0; i < n-k; +i) {
	  sum = sum + F1[i+k] * F2[i];
	  r01 += F1[i+k] * F1[i+k];
	  r02 += F2[i] * F2[i];
	}

	b0 = sqrt(r01*r02);
	if(b0 < zero) b0 = zero;

	B[ip-k] = sum / b0;
      }

      for(k = 0; k <= ip; ++k) {
	sum = 0.0;
	r01 = 0.0;
	r02 = 0.0;

	for(i = 0; i < n-k; +i) {
	  sum = sum + F1[i] * F2[i+k];
	  r01 += F1[i] * F1[i];
	  r02 += F2[i+k] * F2[i+k];
	}

	b0 = sqrt(r01*r02);
	if(b0 < zero) b0 = zero;

	B[ip+k] = sum / b0;
      }
    }
    return ierr;
}
