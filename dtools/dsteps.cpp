/* dsteps.cpp */

#include <stdlib.h>
#include <stdio.h>

/// Generate steps:
///
/// a2->      +-------+
///           |       |
/// a1-> -----+       |
/// a3->              +----
///      <-n1-><--n2-><-n3>
///
/// given in command lines as: a1 n1 a2 n2 a3 n3

int main (int argc, char* argv[])
{
    if(argc < 3) {
	printf("Generate steps:\n"\
	       "\n"\
	       " a2->      +-------+\n"\
	       "           |       |\n"\
	       " a1-> -----+       |\n"\
	       " a3->              +----\n"\
	       "      <-n1-><--n2-><-n3>\n"\
	       "\n"\
	       "given in command lines as: a1 n1 a2 n2 a3 n3\n");
	return 1;
    }
    for(int i = 1; i+1 < argc; i += 2) {
	char	*a = argv[i];
	int	n = atoi(argv[i+1]);
	for(int j = 0; j < n; ++j) {
	    printf("%s\n", a);
	}
    }
    return 0;
}
