/* TestTF.cpp */
static char rcsid[] = "$Id$";

#pragma hdrstop
//#include <condefs.h>

#include "NaTrFunc.h"

//---------------------------------------------------------------------------
//USELIB("NeuArch.lib");
//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char **argv)
{
    NaOpenLogFile("TestTF.log");
#if 0
    NaTransFunc     tf(0, 1.0,
                       1, 1.0, 3.0);
    //tf.PrintLog();

    NaTransFunc     tf1(0, 2.0,
                        1, 1.0, -3.5);
    //tf1.PrintLog();

    NaTransFunc     tf2(0, 3.0,
                        1, -2.3, 0.5);
    //tf2.PrintLog();

    NaTransFunc     tf3(1, 4.0, 2.5,
                        1, 0.4, 1.1);
    //tf3.PrintLog();

    tf = tf1 + tf2;
    tf.PrintLog();

    tf *= tf3;
    tf.PrintLog();

    {
        NaConfigPart    *conf_list[] = { &tf };
        NaConfigFile    conf_file(";NeuCon transfer", 1, 0);
        conf_file.AddPartitions(NaNUMBER(conf_list), conf_list);
        conf_file.SaveToFile("test.cfg");
    }

    NaTransFunc     tf_in;
    {
        NaConfigPart    *conf_list[] = { &tf_in };
        NaConfigFile    conf_file(";NeuCon transfer", 1, 0);
        conf_file.AddPartitions(NaNUMBER(conf_list), conf_list);
        conf_file.LoadFromFile("test.cfg");
    }
    tf_in.PrintLog();
#else
    NaTransFunc     tf_arma(1, 2.0, 0.7,
                            1, 1.0, 0.3);
    NaReal          x[] = { 1.2, -3.0, 0.9, -0.4 };
    NaReal          y[NaNUMBER(x)];
    unsigned        i;
    tf_arma.PrintLog();
    tf_arma.Reset();
    for(i = 0; i < NaNUMBER(x); ++i){
        tf_arma.Function(x + i, y + i);
        NaPrintLog("x[%d]=%g\t y[%d]=%g\n", i, x[i], i, y[i]);
    }

    /* TEST #1
Logging started.
Transfer function:
  (2[1]+0.7[0])/(1[1]+0.3[0])
x[0]=1.2	 y[0]=2.4
x[1]=-3	 y[1]=-5.88
x[2]=0.9	 y[2]=1.464
x[3]=-0.4	 y[3]=-0.6092
    */

    /* TEST #2
Logging started.
Transfer function:
  (2[1]+0.7[0])/(1[2]+0.3[1]+-0.1[0])
x[0]=1.2	 y[0]=0
x[1]=-3	 y[1]=2.4
x[2]=0.9	 y[2]=-5.88
x[3]=-0.4	 y[3]=1.704
    */
#endif

    return 0;
}
