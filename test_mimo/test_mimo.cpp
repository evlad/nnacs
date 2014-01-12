// test_mimo.cpp
// Test Multiple-Input Multiple-Output object

#include <iostream>

#include <NaSSModel.h>


int
main (int argc, char* argv[])
{
    NaStateSpaceModel	ssm;
    NaMatrix A(2,2), B(2, 1);

    try {

	// A = [1 0.5; 0 2]
	A[0][0] = 1.0;
	A[0][1] = 0.5;
	A[1][0] = 0.0;
	A[1][1] = 2.0;

	// B = [0; 1]
	B[0][0] = 0.0;
	B[0][1] = 1.0;

	ssm.SetMatrices(A, B);

	ssm.Save("mimo_test.ssm");

	ssm.Load("mimo_test.ssm");

	// Start modeling

	// Initial state x0=[0; 0] but may be changed
	// using ssm.SetInitialState()
	ssm.Reset();

	// Feed several input values and get output

	// u = [0 0 1 0 0 2 0 0 0 0 0]
	NaVector u(10);
	u.init_zero();
	u[2] = 1.0;
	u[5] = 2.0;

	for(unsigned i = 0; i < u.dim(); ++i) {
	    NaReal y[2];
	    ssm.Function(&u[i], y);
	    std::cout << "t=" << i
		      << ", u=" << u[i]
		      << ", y=[" << y[0] << " " << y[1] << "]"
		      << std::endl;
	}
    }
    catch(NaException ex) {
	std::cerr << "Exception: " << NaExceptionMsg(ex) << std::endl;
    }
    return 0;
}
