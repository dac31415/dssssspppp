/*
 *  ========xxx.C ========
 *
 * Compiler Version 8.0.4
 reults that should be obtained.
 [C66xx_0]
 COUNT is equal to  = 256
 y = 2829056     [function: dotp1]   [264 cycles]
 y = 2829056     [function: dotp2]   [186 cycles]
 y = 2829056     [function: dotp4h]      [198 cycles]
 y = 2829056     [function: ddotp4h]     [143 cycles]
 y = 2829056     [function: ddotp4h2]    [159 cycles]
 y = 2829056.0000    [function: mpyspdot]    [868 cycles]
 I finished!

 */

#include <xdc/std.h>
#include "c6x.h"   // add this if you want o use intrinsics
#include <c6x.h>

#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Timestamp.h>   //add this for the timestamp

#include "data_float.h"

#define COUNT   256

//16 puts them in memory banks 0 and 4, stopping memory stalls
#pragma DATA_ALIGN(a1, 16);
#pragma DATA_ALIGN(x1, 16);
//32 puts them both in memory bank 0, causing memory stalls
//#pragma DATA_ALIGN(a1, 32);
//#pragma DATA_ALIGN(x1, 32);
//0 puts them both in memory bank 0, causing memory stalls
//#pragma DATA_MEM_BANK(a2, 0);
//#pragma DATA_MEM_BANK(x2, 0);
//0 and 4 puts them in memory banks 0 and 4, stopping stalls
#pragma DATA_MEM_BANK(a2, 0);
#pragma DATA_MEM_BANK(x2, 4);
//Put in separate banks.  Saves ~130 cycles when program/data in cache
//and ~65 cycles when program/data not in cache.
#pragma DATA_MEM_BANK(b, 0);
#pragma DATA_MEM_BANK(y, 4);

/*
 *  ======== main ========
 */

/* Prototype                                   */
extern int dotp1(short *m, short *n, int count);
extern int dotp2(short *m, short *n, int count); // if you do not add this declaration, you have some implicit decalration and the return value
// will NOT be correct.
extern int dotpHW(short *m, short *n, int count);
extern int dotp4h(short *m, short *n, int count);
extern int ddotp4h(short *m, short *n, int count);
extern int ddotp4h2(short *m, short *n, int count);

extern int dotpC_intrinsic(short *m, short *n, int count);

extern float mpyspdot(float *p, float *k, int count);
/* Declarations                                */

//Pad a by 8 bytes (4 shorts) to offset x into a different bank in the cache.
short a1[COUNT + 4] = { A_ARRAY_F, 0, 0, 0, 0 };
short x1[COUNT] = { X_ARRAY_F };

short a2[COUNT] = { A_ARRAY_F };
short x2[COUNT] = { X_ARRAY_F };

float b[COUNT] = { B_ARRAY_F };
float y[COUNT] = { Y_ARRAY_F };

int y0, y1, y2, y3, y4, y5, y7, y8;
float y6;

int delta0, delta1, delta2, delta3, delta4, delta5, delta6, delta7, delta8;
int time1, time2;

Void main() {
    System_printf("Compile Flags: -O2 --disable:sploop, offset x\n");

    y0 = dotpHW(a1, x1, COUNT);  //Get data/program into cache

    time1 = Timestamp_get32();
    y0 = dotpHW(a1, x1, COUNT);
    time2 = Timestamp_get32();
    delta0 = time2 - time1;

    y7 = dotp1(a1, x1, COUNT);  //Get program into cache

    time1 = Timestamp_get32();
    y7 = dotp1(a1, x1, COUNT);
    time2 = Timestamp_get32();
    delta7 = time2 - time1;

    y1 = dotp1(a2, x2, COUNT);  //Get data/program into cache

    time1 = Timestamp_get32();
    y1 = dotp1(a2, x2, COUNT);
    time2 = Timestamp_get32();
    delta1 = time2 - time1;

    y2 = dotp2(a1, x1, COUNT);  //Get data/program into cache

    time1 = Timestamp_get32();
    y2 = dotp2(a1, x1, COUNT);
    time2 = Timestamp_get32();
    delta2 = time2 - time1;

    y3 = dotp4h(a1, x1, COUNT);  //Get program into cache

    time1 = Timestamp_get32();
    y3 = dotp4h(a1, x1, COUNT);
    time2 = Timestamp_get32();
    delta3 = time2 - time1;

    y4 = ddotp4h(a1, x1, COUNT);  //Get program into cache

    //Using single ddotp4h
    time1 = Timestamp_get32();
    y4 = ddotp4h(a1, x1, COUNT);
    time2 = Timestamp_get32();
    delta4 = time2 - time1;

    y5 = ddotp4h2(a1, x1, COUNT);  //Get program into cache

    //Using two ddotp4h
    time1 = Timestamp_get32();
    y5 = ddotp4h2(a1, x1, COUNT);
    time2 = Timestamp_get32();
    delta5 = time2 - time1;

    //Get program/data into cache.  Saves ~235 cycles when data in different
    //banks and ~170 when data in same banks.
    y6 = mpyspdot(b, y, COUNT);

    time1 = Timestamp_get32();
    y6 = mpyspdot(b, y, COUNT);
    time2 = Timestamp_get32();
    delta6 = time2 - time1;




	 y8 =  dotpC_intrinsic(b, y, COUNT);

	    time1 = Timestamp_get32();
	    y8 =  dotpC_intrinsic(a1, x1, COUNT);
	    time2 = Timestamp_get32();
	    delta8 = time2 - time1;

    System_printf("COUNT is equal to  = %d \n", COUNT);
    System_printf("y = %d \t [function: dotp0] \t [%d cycles] (software pipelining disabled)\n", y0, delta0);
    System_printf("y = %d \t [function: dotp1] \t [%d cycles] (DATA_ALIGN + offset)\n", y7, delta7);
    System_printf("y = %d \t [function: dotp1] \t [%d cycles] (DATA_MEM_BANK)\n", y1, delta1);
    System_printf("y = %d \t [function: dotp2] \t [%d cycles]\n", y2, delta2);
    System_printf("y = %d \t [function: dotp4h] \t [%d cycles]\n", y3, delta3);
    System_printf("y = %d \t [function: ddotp4h] \t [%d cycles]\n", y4, delta4);
    System_printf("y = %d \t [function: ddotp4h2] \t [%d cycles]\n", y5,
            delta5);
    System_printf("y = %d \t [dotpC_intrinsic] \t [%d cycles]\n", y8,
            delta8);


    System_printf("y = %f \t [function: mpyspdot] \t [%d cycles]\n", y6,
            delta6);

    System_printf("I finished!\n");

    BIOS_exit(0); /* terminates program and dumps SysMin output */

}
