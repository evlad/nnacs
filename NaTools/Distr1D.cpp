/* Distr1D.cpp */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <NaGenerl.h>
#include <NaExcept.h>
#include <NaDataIO.h>
#include <kbdif.h>


//---------------------------------------------------------------------------
// Structure for one-dimension description
struct DataSet
{
    char    *fname;         // file name
    NaDataFile  *df;        // file handle
    int     cells;          // number of subranges
    NaReal  min, max;       // min and max of the whole range
    NaReal  width, step;    // computed width (max-min) and step (width/cells)

}   ds;


//---------------------------------------------------------------------------
// Preprocess data set (fill min, max, width, cells)
void    dataset_preproc (DataSet& ds);


int
main(int argc, char **argv)
{
  if(argc < 2 || argc > 3){
    printf("Usage: Distr1D Series.dat [Graph.dat]\n"\
	   "       Series.dat - random series;\n"\
	   "       Graph.dat  - gnuplot-ready file:\n"\
	   "                       plot 'Graph.dat' with lines\n");
    return 1;
  }

  FILE    *fpGraph = NULL;
  ds.fname = argv[1];
  if(argc > 2){
    fpGraph = fopen(argv[2], "w");
    if(NULL == fpGraph)
      printf("Failed to create '%s'\n", argv[3]);
  }

  try{
    ds.df = OpenInputDataFile(ds.fname);
    dataset_preproc(ds);

    // allocate distr
    int     *distr = new int[ds.cells];
    int     i, nTotal = 0, nMissed = 0;

    // initialize the distr
    for(i = 0; i < ds.cells; ++i){
	distr[i] = 0;
    }

    // fill the distr
    for(ds.df->GoStartRecord(); ds.df->GoNextRecord(); ++nTotal){
      NaReal  x = ds.df->GetValue();
      i = (x - ds.min) / ds.step;

      if(i < 0 || i >= ds.cells){
	++nMissed;
	continue;
      }

      ++(distr[i]);
    }

    // normalize distribution
    NaReal fMax = 0.0;
    for(i = 0; i < ds.cells; ++i){
	if(distr[i] > fMax)
	    fMax = distr[i];
    }

    // print the distr
    int     nCovered = 0;   // number of covered cells

    for(i = 0; i < ds.cells; ++i){
      if(NULL != fpGraph)
	fprintf(fpGraph, "%g\t%d\t%g\n",
		ds.min + i * ds.step,
		distr[i], fMax > 0.0? distr[i]/fMax: 0.0 );

	if(distr[i] > 0){
	  ++nCovered;
	}
    }

    if(NULL != fpGraph)
      fclose(fpGraph);

    printf("Length of series: %d\n"
	   "Percent of coverage: %4.1f %%\n"
	   "Percent of missing: %4.1f %%\n"
	   "Graph argument: %s (min=%g max=%g step=%g cells=%d)\n",
	   nTotal,
	   (NaReal)nCovered * 100 / ds.cells,
	   (NaReal)nMissed * 100 / nTotal,
	   ds.fname, ds.min, ds.max, ds.step, ds.cells);

    delete ds.df;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }
  return 0;
}


//---------------------------------------------------------------------------
// Preprocess data set (fill min, max, width, cells)
void
dataset_preproc (DataSet& ds)
{
    printf("*** Lets describe data from file '%s' ***\n", ds.fname);

    // read data from file and determine min and max
    if(!ds.df->GoStartRecord()){
        printf("Empty file '%s'\n", ds.fname);
    }else{
        ds.max = ds.min = ds.df->GetValue();
        while(ds.df->GoNextRecord()){
            NaReal  v = ds.df->GetValue();
            if(v > ds.max){
                ds.max = v;
            }
            if(v < ds.min){
                ds.min = v;
            }
        }

        printf("There are %ld values in the file.\n", ds.df->CountOfRecord());

        // ask user about data range (propose defined)
        ds.min = ask_user_real("Enter low bound for data", ds.min);
        ds.max = ask_user_real("Enter high bound for data", ds.max);

        ds.width = ds.max - ds.min;
        printf("Data has range of %g width\n", ds.width);

        // ask user about number of one-dimension cells
        while(ds.cells <= 0){
            ds.cells = ask_user_int("Enter number of subranges", ds.cells);
        }

        ds.step = ds.width / ds.cells;

        printf("Data range subdivided by %d cells of %g width each\n",
               ds.cells, ds.step);
    }
}
