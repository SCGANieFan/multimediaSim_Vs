/*___________________________________________________________________________
|                                                                           |
|   Include-Files                                                           |
|___________________________________________________________________________|
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "typedef.h"
#include "table.h"
/*___________________________________________________________________________
 |                                                                           |
 |   Constants and Globals                                                   |
 | $Id $
 |___________________________________________________________________________|
*/
extern Flag Overflow;
extern Flag Carry;

#define MAX_32 (int)0x7fffffffL
#define MIN_32 (int)0x80000000L

#define MAX_16 (short)0x7fff
#define MIN_16 (short)0x8000

#define PLATFORM

/*___________________________________________________________________________
|                                                                           |
| Basic arithmetic operators.                                               |
|                                                                           |
| $Id $                                                                     |
|                                                                           |
|       saturate()                                                          |
|       add()                                                               |
|       sub()                                                               |
|       abs_s()                                                             |
|       divide_s()                                                          |
|       extract_h()                                                         |
|       extract_l()                                                         |
|       L_abs()                                                             |
|       L_add()                                                             |
|       L_deposit_h()                                                       |
|       L_deposit_l()                                                       |
|       L_mac()                                                             |
|       L_msu()                                                             |
|       L_mult()                                                            |
|       L_negate()                                                          |
|       L_shl()                                                             |
|       L_shr()                                                             |
|       L_sub()                                                             |
|       mac_r()                                                             |
|       msu_r()                                                             |
|       mult()                                                              |
|       mult_r()                                                            |
|       negate()                                                            |
|       norm_l()                                                            |
|       norm_s()                                                            |
|       round()                                                             |
|       shl()                                                               |
|       shr()                                                               |
|___________________________________________________________________________|
*/

#if (WMOPS)
extern BASIC_OP multiCounter[MAXCOUNTERS];
extern int currCounter;
#endif


/*___________________________________________________________________________
|                                                                           |
|   Local Functions                                                         |
|___________________________________________________________________________|
*/
static inline short saturate(int L_var1);
static inline short shl(short var1, short var2);
static inline short shr(short var1, short var2);
static inline int L_shl(int L_var1, short var2);
static inline int L_shr(int L_var1, short var2);
static inline short extract_l(int L_var1);
static inline void L_Extract(Word32 L_32, Word16 *hi, Word16 *lo);
static inline short norm_l(int x);
static inline int L_add(int L_var1, int L_var2);
static inline int L_sub(int L_var1, int L_var2);
static inline int L_add_c(int L_var1, int L_var2);
static inline int L_sub_c(int L_var1, int L_var2);

/*___________________________________________________________________________
|                                                                           |
|   Constants and Globals                                                   |
|___________________________________________________________________________|
*/
int Overflow = 0;
int Carry = 0;


/*___________________________________________________________________________
|                                                                           |
|   Functions                                                               |
|___________________________________________________________________________|
*/
static inline bool __get_Overflow(void)
{
    return Overflow;
}

static inline void __set_Overflow(bool flag)
{
    Overflow = flag;
}

#if 0//def PLATFORM
//function shl when var2>0
static inline short SSAT16_SHIFTL_1(short ARG){	//shl(ARG,1)
	short __RES;
	__asm ("ssat %0, #16, %1, lsl #1;\n" : "=r" (__RES) : "r" (ARG));
#if (WMOPS)
	multiCounter[currCounter].shl++;
#endif
	return __RES;
}

static inline short SSAT16_SHIFTL_2(short ARG){	//shl(ARG,2)
	short __RES;
	__asm ("ssat %0, #16, %1, lsl #2;\n" : "=r" (__RES) : "r" (ARG));
#if (WMOPS)
	multiCounter[currCounter].shl++;
#endif
	return __RES;
}

static inline short SSAT16_SHIFTL_3(short ARG){	//shl(ARG,3)
	short __RES;
	__asm ("ssat %0, #16, %1, lsl #3;\n" : "=r" (__RES) : "r" (ARG));
#if (WMOPS)
	multiCounter[currCounter].shl++;
#endif
	return __RES;
}

static inline short SSAT16_SHIFTL_4(short ARG){	//shl(ARG,4)
	short __RES;
	__asm ("ssat %0, #16, %1, lsl #4;\n" : "=r" (__RES) : "r" (ARG));
#if (WMOPS)
	multiCounter[currCounter].shl++;
#endif
	return __RES;
}

static inline short SSAT16_SHIFTL_6(short ARG){	//shl(ARG,6)
	short __RES;
	__asm ("ssat %0, #16, %1, lsl #6;\n" : "=r" (__RES) : "r" (ARG));
#if (WMOPS)
	multiCounter[currCounter].shl++;
#endif
	return __RES;
}

static inline short SSAT16_SHIFTL_9(short ARG){	//shl(ARG,9)
	short __RES;
	__asm ("ssat %0, #16, %1, lsl #9;\n" : "=r" (__RES) : "r" (ARG));
#if (WMOPS)
	multiCounter[currCounter].shl++;
#endif
	return __RES;
}

//function shr when var2>0
static inline short SSAT16_SHIFTR_1(short ARG){	//shr(ARG,1)
	short __RES;
	__asm ("ssat %0, #16, %1, asr #1;\n" : "=r" (__RES) : "r" (ARG));
#if (WMOPS)
	multiCounter[currCounter].shr++;
#endif
	return __RES;
}

static inline short SSAT16_SHIFTR_2(short ARG){	//shr(ARG,2)
	short __RES;
	__asm ("ssat %0, #16, %1, asr #2;\n" : "=r" (__RES) : "r" (ARG));
#if (WMOPS)
	multiCounter[currCounter].shr++;
#endif
	return __RES;
}

static inline short SSAT16_SHIFTR_3(short ARG){	//shr(ARG,3)
	short __RES;
	__asm ("ssat %0, #16, %1, asr #3;\n" : "=r" (__RES) : "r" (ARG));
#if (WMOPS)
	multiCounter[currCounter].shr++;
#endif
	return __RES;
}

static inline short SSAT16_SHIFTR_6(short ARG){	//shr(ARG,6)
	short __RES;
	__asm ("ssat %0, #16, %1, asr #6;\n" : "=r" (__RES) : "r" (ARG));
#if (WMOPS)
	multiCounter[currCounter].shr++;
#endif
	return __RES;
}

//function L_shr when var2>0
static inline int SSAT_SHIFTR_1(int L_var1){	//L_shr(L_var1,1)
	int L_var_out;
	__asm("ssat %0,#32,%1,asr #1;\n" : "=r"(L_var_out) : "r"(L_var1));
#if (WMOPS)
	multiCounter[currCounter].L_shr++;
#endif
	return L_var_out;
}

static inline int SSAT_SHIFTR_3(int L_var1){	//L_shr(L_var1,3)
	int L_var_out;
	__asm("ssat %0,#32,%1,asr #3;\n" : "=r"(L_var_out) : "r"(L_var1));
#if (WMOPS)
	multiCounter[currCounter].L_shr++;
#endif
	return L_var_out;
}

static inline int SSAT_SHIFTR_4(int L_var1){	//L_shr(L_var1,4)
	int L_var_out;
	__asm("ssat %0,#32,%1,asr #4;\n" : "=r"(L_var_out) : "r"(L_var1));
#if (WMOPS)
	multiCounter[currCounter].L_shr++;
#endif
	return L_var_out;
}

static inline int SSAT_SHIFTR_7(int L_var1){	//L_shr(L_var1,7)
	int L_var_out;
	__asm("ssat %0,#32,%1,asr #7;\n" : "=r"(L_var_out) : "r"(L_var1));
#if (WMOPS)
	multiCounter[currCounter].L_shr++;
#endif
	return L_var_out;
}
#else
//function shl when var2>0
#define SSAT16_SHIFTL_1(x) shl((x), 1)

#define SSAT16_SHIFTL_2(x) shl((x), 2)

#define SSAT16_SHIFTL_3(x) shl((x), 3)

#define SSAT16_SHIFTL_4(x) shl((x), 4)

#define SSAT16_SHIFTL_6(x) shl((x), 6)

#define SSAT16_SHIFTL_9(x) shl((x), 9)

//function shr when var2>0
#define SSAT16_SHIFTR_1(x) shr((x), 1)

#define SSAT16_SHIFTR_2(x) shr((x), 2)

#define SSAT16_SHIFTR_3(x) shr((x), 3)

#define SSAT16_SHIFTR_6(x) shr((x), 6)

//function L_shr when var2>0
#define SSAT_SHIFTR_1(x) L_shr((x), 1)

#define SSAT_SHIFTR_3(x) L_shr((x), 3)

#define SSAT_SHIFTR_4(x) L_shr((x), 4)

#define SSAT_SHIFTR_7(x) L_shr((x), 7)
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : saturate                                                |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|    Limit the 32 bit input to the range of a 16 bit word.                  |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1                                                                 |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline short saturate(int L_var1)
{
	short var_out;
	__asm(
	"ssat %0,#16,%1;\n"
		:"=r"(var_out)
		:"r"(L_var1)
	);
	return (var_out);
}
#else
static inline short saturate(int L_var1)
{
	short var_out;

	if (L_var1 > 0X00007fffL)
	{
		Overflow = 1;
		var_out = MAX_16;
	}
	else if (L_var1 < (int)0xffff8000L)
	{
		Overflow = 1;
		var_out = MIN_16;
	}
	else
	{
		var_out = extract_l(L_var1);

#if (WMOPS)
		multiCounter[currCounter].extract_l--;
#endif
	}

	return (var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : add                                                     |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|    Performs the addition (var1+var2) with overflow control and saturation;|
|    the 16 bit result is set at +32767 when overflow occurs or at -32768   |
|    when underflow occurs.                                                 |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline short add(short var1, short var2)
{
	short var_out;
	__asm(
	"qadd16 %0,%1,%2;\n"
		:"=r"(var_out)
		:"r"(var1),"r"(var2)
		);
#if (WMOPS)
	multiCounter[currCounter].add++;
#endif
	return (var_out);
}
#else
static inline short add(short var1, short var2)
{
	short var_out;
	int L_sum;

	L_sum = (int)var1 + var2;
	var_out = saturate(L_sum);

#if (WMOPS)
	multiCounter[currCounter].add++;
#endif
	return (var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : sub                                                     |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|    Performs the subtraction (var1+var2) with overflow control and satu-   |
|    ration; the 16 bit result is set at +32767 when overflow occurs or at  |
|    -32768 when underflow occurs.                                          |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline short sub(short var1, short var2)
{
	int var_out;
	int x = (int)var1;
	__asm(
		"sub %0,%1,%2;\n"
		"ssat %0,#16,%0;\n"
		:"=r"(var_out)
		:"r"(x),"r"(var2)
		);
#if (WMOPS)
	multiCounter[currCounter].sub++;
#endif
	return (short)(var_out);
}
#else
static inline short sub(short var1, short var2)
{
	short var_out;
	int L_diff;

	L_diff = (int)var1 - var2;
	var_out = saturate(L_diff);

#if (WMOPS)
	multiCounter[currCounter].sub++;
#endif
	return (var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : abs_s                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|    Absolute value of var1; abs_s(-32768) = 32767.                         |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0x0000 0000 <= var_out <= 0x0000 7fff.                |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline short abs_s(short var1)
{
	short var_out;

	if (var1 >= 0)
	{
#if (WMOPS)
		multiCounter[currCounter].abs_s++;
#endif
		return (var1);
	}
	else
	{
		short var_out;
		short temp = 0;
		__asm(
		"qsub16	%0,%1,%2\n"
			:"=r"(var_out)
			: "r"(temp), "r"(var1)
			);
#if (WMOPS)
		multiCounter[currCounter].abs_s++;
#endif
		return (var_out);
	}
}
#else
static inline short abs_s(short var1)
{
	short var_out;

	if (var1 == (short)MIN_16)
	{
		var_out = MAX_16;
	}
	else
	{
		if (var1 < 0)
		{
			var_out = -var1;
		}
		else
		{
			var_out = var1;
		}
	}

#if (WMOPS)
	multiCounter[currCounter].abs_s++;
#endif
	return (var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : shl                                                     |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Arithmetically shift the 16 bit input var1 left var2 positions.Zero fill|
|   the var2 LSB of the result. if var2 is negative, arithmetically shift   |
|   var1 right by -var2 with sign extension. Saturate the result in case of |
|   underflows or overflows.                                                |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline short shl(short x, short shift)
{
    if (shift <= 0 || x == 0) {
        if (shift < -16) shift = -16;
#if (WMOPS)
	multiCounter[currCounter].shl++;
#endif
        return (short) (x >> (-shift));
    }
    if (shift > 15)
        shift = 16;
#if (WMOPS)
	multiCounter[currCounter].shl++;
#endif
    return saturate(x << shift);
}
#else
static inline short shl(short var1, short var2)
{
        short var_out;
	int result;

	if (var2 < 0)
	{
		if (var2 < -16)
			var2 = -16;
		var2 = -var2;
		var_out = shr(var1, var2);

#if (WMOPS)
		multiCounter[currCounter].shr--;
#endif
	}
	else
	{
		result = (int)var1 *((int)1 << var2);

		if ((var2 > 15 && var1 != 0) || (result != (int)((short)result)))
		{
			Overflow = 1;
			var_out = (var1 > 0) ? MAX_16 : MIN_16;
		}
		else
		{
			var_out = extract_l(result);

#if (WMOPS)
			multiCounter[currCounter].extract_l--;
#endif
		}
	}

#if (WMOPS)
	multiCounter[currCounter].shl++;
#endif
	return (var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : shr                                                     |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Arithmetically shift the 16 bit input var1 right var2 positions with    |
|   sign extension. if var2 is negative, arithmetically shift var1 left by  |
|   -var2 with sign extension. Saturate the result in case of underflows or |
|   overflows.                                                              |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline short shr(short x, short shift)
{
    if (shift >= 0 || x == 0) {
        if (shift > 15) shift = 15;
#if (WMOPS)
        multiCounter[currCounter].shr++;
#endif
        return (short) (x >> shift);
    }
    if (shift < -15)
        shift = -16;
#if (WMOPS)
    multiCounter[currCounter].shr++;
#endif
    return saturate(x << (-shift));
}
#else
static inline short shr(short var1, short var2)
{
	short var_out;

	if (var2 < 0)
	{
		if (var2 < -16)
			var2 = -16;
		var2 = -var2;
		var_out = shl(var1, var2);

#if (WMOPS)
		multiCounter[currCounter].shl--;
#endif
	}
	else
	{
		if (var2 >= 15)
		{
			var_out = (var1 < 0) ? -1 : 0;
		}
		else
		{
			if (var1 < 0)
			{
				var_out = ~((~var1) >> var2);
			}
			else
			{
				var_out = var1 >> var2;
			}
		}
	}

#if (WMOPS)
	multiCounter[currCounter].shr++;
#endif
	return (var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : mult                                                    |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|    Performs the multiplication of var1 by var2 and gives a 16 bit result  |
|    which is scaled i.e.:                                                  |
|             mult(var1,var2) = extract_l(L_shr((var1 times var2),15)) and  |
|             mult(-32768,-32768) = 32767.                                  |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
|___________________________________________________________________________|
*/
static inline short mult(short var1, short var2)
{
	short var_out;
	int L_product;

	L_product = (int)var1 *(int)var2;

	L_product = (L_product & (int)0xffff8000L) >> 15;

	if (L_product & (int)0x00010000L)
		L_product = L_product | (int)0xffff0000L;

	var_out = saturate(L_product);

#if (WMOPS)
	multiCounter[currCounter].mult++;
#endif
	return (var_out);
}

/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_mult                                                  |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   L_mult is the 32 bit result of the multiplication of var1 times var2    |
|   with one shift left i.e.:                                               |
|        L_mult(var1,var2) = L_shl((var1 times var2),1) and                 |
|        L_mult(-32768,-32768) = 2147483647.                                |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline int L_mult(short var1, short var2)
{
	int L_var_out;
	__asm(
		"mul %0,%1,%2;\n"
		"qadd %0,%0,%0;\n"
		:"=r"(L_var_out)
		: "r"(var1), "r"(var2)
		);
#if (WMOPS)
	multiCounter[currCounter].L_mult++;
#endif
	return (L_var_out);
}
#else
static inline int L_mult(short var1, short var2)
{
	int L_var_out;

	L_var_out = (int)var1 *(int)var2;

	if (L_var_out != (int)0x40000000L)
	{
		L_var_out *= 2;
	}
	else
	{
		Overflow = 1;
		L_var_out = MAX_32;
	}

#if (WMOPS)
	multiCounter[currCounter].L_mult++;
#endif
	return (L_var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : negate                                                  |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Negate var1 with saturation, saturate in the case where input is -32768:|
|                negate(var1) = sub(0,var1).                                |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline short negate(short var1)
{
	short var_out;
	short temp = 0;

	__asm(
		"qsub16   %0,%1,%2;\n"
		:"=r"(var_out)
		: "r"(temp), "r"(var1)
		);

#if (WMOPS)
	multiCounter[currCounter].negate++;
#endif
	return (var_out);
}
#else
static inline short negate(short var1)
{
	short var_out;

	var_out = (var1 == MIN_16) ? MAX_16 : -var1;

#if (WMOPS)
	multiCounter[currCounter].negate++;
#endif
	return (var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : extract_h                                               |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Return the 16 MSB of L_var1.                                            |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1                                                                 |
|             32 bit long signed integer (int ) whose value falls in the |
|             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline short extract_h(int L_var1)
{
	short var_out;

	__asm(
		"asr %0,%1,#16;\n"
		:"=r"(var_out)
		: "r"(L_var1)
		);
#if (WMOPS)
	multiCounter[currCounter].extract_h++;
#endif
	return (var_out);
}
#else
static inline short extract_h(int L_var1)
{
	short var_out;

	var_out = (short)(L_var1 >> 16);

#if (WMOPS)
	multiCounter[currCounter].extract_h++;
#endif
	return (var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : extract_l                                               |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Return the 16 LSB of L_var1.                                            |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1                                                                 |
|             32 bit long signed integer (int ) whose value falls in the |
|             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
|___________________________________________________________________________|
*/
static inline short extract_l(int L_var1)
{
	short var_out;

	var_out = (short)L_var1;

#if (WMOPS)
	multiCounter[currCounter].extract_l++;
#endif
	return (var_out);
}


/*___________________________________________________________________________
|                                                                           |
|   Function Name : round                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Round the lower 16 bits of the 32 bit input number into the MS 16 bits  |
|   with saturation. Shift the resulting bits right by 16 and return the 16 |
|   bit number:                                                             |
|               round(L_var1) = extract_h(L_add(L_var1,32768))              |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1                                                                 |
|             32 bit long signed integer (int ) whose value falls in the |
|             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
|___________________________________________________________________________|
*/
static inline short roundInt(int L_var1)
{
	short var_out;
	int L_rounded;

	L_rounded = L_add(L_var1, (int)0x00008000L);
	var_out = extract_h(L_rounded);

#if (WMOPS)
	multiCounter[currCounter].L_add--;
	multiCounter[currCounter].extract_h--;
	multiCounter[currCounter].round++;
#endif
	return (var_out);
}


/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_mac                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Multiply var1 by var2 and shift the result left by 1. Add the 32 bit    |
|   result to L_var3 with saturation, return a 32 bit result:               |
|        L_mac(L_var3,var1,var2) = L_add(L_var3,L_mult(var1,var2)).         |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var3   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|___________________________________________________________________________|
*/
static inline int L_mac(int L_var3, short var1, short var2)
{
	int L_var_out;
	int L_product;

	L_product = L_mult(var1, var2);
	L_var_out = L_add(L_var3, L_product);

#if (WMOPS)
	multiCounter[currCounter].L_mult--;
	multiCounter[currCounter].L_add--;
	multiCounter[currCounter].L_mac++;
#endif
	return (L_var_out);
}


/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_msu                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Multiply var1 by var2 and shift the result left by 1. Subtract the 32   |
|   bit result from L_var3 with saturation, return a 32 bit result:         |
|        L_msu(L_var3,var1,var2) = L_sub(L_var3,L_mult(var1,var2)).         |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var3   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|___________________________________________________________________________|
*/
static inline int L_msu(int L_var3, short var1, short var2)
{
	int L_var_out;
	int L_product;

	L_product = L_mult(var1, var2);
	L_var_out = L_sub(L_var3, L_product);

#if (WMOPS)
	multiCounter[currCounter].L_mult--;
	multiCounter[currCounter].L_sub--;
	multiCounter[currCounter].L_msu++;
#endif
	return (L_var_out);
}


/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_macNs                                                 |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Multiply var1 by var2 and shift the result left by 1. Add the 32 bit    |
|   result to L_var3 without saturation, return a 32 bit result. Generate   |
|   carry and overflow values :                                             |
|        L_macNs(L_var3,var1,var2) = L_add_c(L_var3,L_mult(var1,var2)).     |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var3   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|                                                                           |
|   Caution :                                                               |
|                                                                           |
|    In some cases the Carry flag has to be cleared or set before using     |
|    operators which take into account its value.                           |
|___________________________________________________________________________|
*/
static inline int L_macNs(int L_var3, short var1, short var2)
{
	int L_var_out;

	L_var_out = L_mult(var1, var2);
	L_var_out = L_add_c(L_var3, L_var_out);

#if (WMOPS)
	multiCounter[currCounter].L_mult--;
	multiCounter[currCounter].L_add_c--;
	multiCounter[currCounter].L_macNs++;
#endif
	return (L_var_out);
}


/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_msuNs                                                 |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Multiply var1 by var2 and shift the result left by 1. Subtract the 32   |
|   bit result from L_var3 without saturation, return a 32 bit result. Ge-  |
|   nerate carry and overflow values :                                      |
|        L_msuNs(L_var3,var1,var2) = L_sub_c(L_var3,L_mult(var1,var2)).     |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var3   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|                                                                           |
|   Caution :                                                               |
|                                                                           |
|    In some cases the Carry flag has to be cleared or set before using     |
|    operators which take into account its value.                           |
|___________________________________________________________________________|
*/
static inline int L_msuNs(int L_var3, short var1, short var2)
{
	int L_var_out;

	L_var_out = L_mult(var1, var2);
	L_var_out = L_sub_c(L_var3, L_var_out);

#if (WMOPS)
	multiCounter[currCounter].L_mult--;
	multiCounter[currCounter].L_sub_c--;
	multiCounter[currCounter].L_msuNs++;

#endif
	return (L_var_out);
}


/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_add                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   32 bits addition of the two 32 bits variables (L_var1+L_var2) with      |
|   overflow control and saturation; the result is set at +2147483647 when  |
|   overflow occurs or at -2147483648 when underflow occurs.                |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    L_var2   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|___________________________________________________________________________|
*/
// TODO: not bitexact
#if 0//def PLATFORM
static inline int L_add(int L_var1, int L_var2)
{
	int L_var_out;
	__asm(
	"qadd %0,%1,%2;\n"
		:"=r"(L_var_out)
		:"r"(L_var1),"r"(L_var2)
	);
#if (WMOPS)
	multiCounter[currCounter].L_add++;
#endif
	return (L_var_out);
}
#else
static inline int L_add(int L_var1, int L_var2)
{
	int L_var_out;

	L_var_out = L_var1 + L_var2;

	if (((L_var1 ^ L_var2) & MIN_32) == 0)
	{
		if ((L_var_out ^ L_var1) & MIN_32)
		{
			L_var_out = (L_var1 < 0) ? MIN_32 : MAX_32;
			Overflow = 1;
		}
	}

#if (WMOPS)
	multiCounter[currCounter].L_add++;
#endif
	return (L_var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_sub                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   32 bits subtraction of the two 32 bits variables (L_var1-L_var2) with   |
|   overflow control and saturation; the result is set at +2147483647 when  |
|   overflow occurs or at -2147483648 when underflow occurs.                |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    L_var2   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline int L_sub(int L_var1, int L_var2)
{
	int L_var_out;
	__asm(
	"qsub %0,%1,%2;\n"
		:"=r"(L_var_out)
		:"r"(L_var1),"r"(L_var2)
	);
#if (WMOPS)
	multiCounter[currCounter].L_sub++;
#endif
	return (L_var_out);
}
#else
static inline int L_sub(int L_var1, int L_var2)
{
	int L_var_out;

	L_var_out = L_var1 - L_var2;

	if (((L_var1 ^ L_var2) & MIN_32) != 0)
	{
		if ((L_var_out ^ L_var1) & MIN_32)
		{
			L_var_out = (L_var1 < 0L) ? MIN_32 : MAX_32;
			Overflow = 1;
		}
	}

#if (WMOPS)
	multiCounter[currCounter].L_sub++;
#endif
	return (L_var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_add_c                                                 |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Performs 32 bits addition of the two 32 bits variables (L_var1+L_var2+C)|
|   with carry. No saturation. Generate carry and Overflow values. The car- |
|   ry and overflow values are binary variables which can be tested and as- |
|   signed values.                                                          |
|                                                                           |
|   Complexity weight : 2                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    L_var2   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|                                                                           |
|   Caution :                                                               |
|                                                                           |
|    In some cases the Carry flag has to be cleared or set before using     |
|    operators which take into account its value.                           |
|___________________________________________________________________________|
*/
static inline int L_add_c(int L_var1, int L_var2)
{
	int L_var_out;
	int L_test;
	Flag carry_int = 0;

	L_var_out = L_var1 + L_var2 + Carry;

	L_test = L_var1 + L_var2;

	if ((L_var1 > 0) && (L_var2 > 0) && (L_test < 0))
	{
		Overflow = 1;
		carry_int = 0;
	}
	else
	{
		if ((L_var1 < 0) && (L_var2 < 0))
		{
			if (L_test >= 0)
			{
				Overflow = 1;
				carry_int = 1;
			}
			else
			{
				Overflow = 0;
				carry_int = 1;
			}
		}
		else
		{
			if (((L_var1 ^ L_var2) < 0) && (L_test >= 0))
			{
				Overflow = 0;
				carry_int = 1;
			}
			else
			{
				Overflow = 0;
				carry_int = 0;
			}
		}
	}

	if (Carry)
	{
		if (L_test == MAX_32)
		{
			Overflow = 1;
			Carry = carry_int;
		}
		else
		{
			if (L_test == (int)0xFFFFFFFFL)
			{
				Carry = 1;
			}
			else
			{
				Carry = carry_int;
			}
		}
	}
	else
	{
		Carry = carry_int;
	}

#if (WMOPS)
	multiCounter[currCounter].L_add_c++;
#endif
	return (L_var_out);
}

/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_sub_c                                                 |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Performs 32 bits subtraction of the two 32 bits variables with carry    |
|   (borrow) : L_var1-L_var2-C. No saturation. Generate carry and Overflow  |
|   values. The carry and overflow values are binary variables which can    |
|   be tested and assigned values.                                          |
|                                                                           |
|   Complexity weight : 2                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    L_var2   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|                                                                           |
|   Caution :                                                               |
|                                                                           |
|    In some cases the Carry flag has to be cleared or set before using     |
|    operators which take into account its value.                           |
|___________________________________________________________________________|
*/
static inline int L_sub_c(int L_var1, int L_var2)
{
	int L_var_out;
	int L_test;
	Flag carry_int = 0;

	if (Carry)
	{
		Carry = 0;
		if (L_var2 != MIN_32)
		{
			L_var_out = L_add_c(L_var1, -L_var2);
#if (WMOPS)
			multiCounter[currCounter].L_add_c--;
#endif
		}
		else
		{
			L_var_out = L_var1 - L_var2;
			if (L_var1 > 0L)
			{
				Overflow = 1;
				Carry = 0;
			}
		}
	}
	else
	{
		L_var_out = L_var1 - L_var2 - (int)0X00000001L;
		L_test = L_var1 - L_var2;

		if ((L_test < 0) && (L_var1 > 0) && (L_var2 < 0))
		{
			Overflow = 1;
			carry_int = 0;
		}
		else if ((L_test > 0) && (L_var1 < 0) && (L_var2 > 0))
		{
			Overflow = 1;
			carry_int = 1;
		}
		else if ((L_test > 0) && ((L_var1 ^ L_var2) > 0))
		{
			Overflow = 0;
			carry_int = 1;
		}
		if (L_test == MIN_32)
		{
			Overflow = 1;
			Carry = carry_int;
		}
		else
		{
			Carry = carry_int;
		}
	}

#if (WMOPS)
	multiCounter[currCounter].L_sub_c++;
#endif
	return (L_var_out);
}

/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_negate                                                |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Negate the 32 bit variable L_var1 with saturation; saturate in the case |
|   where input is -2147483648 (0x8000 0000).                               |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline int L_negate(int L_var1)
{
	int L_var_out;
	int temp = 0;
	__asm(
	"qsub   %0,%1,%2;\n"
		:"=r"(L_var_out)
		: "r"(temp), "r"(L_var1)
		);
#if (WMOPS)
	multiCounter[currCounter].L_negate++;
#endif
	return (L_var_out);
}
#else
static inline int L_negate(int L_var1)
{
	int L_var_out;

	L_var_out = (L_var1 == MIN_32) ? MAX_32 : -L_var1;

#if (WMOPS)
	multiCounter[currCounter].L_negate++;
#endif
	return (L_var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : mult_r                                                  |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Same as mult with rounding, i.e.:                                       |
|     mult_r(var1,var2) = extract_l(L_shr(((var1 * var2) + 16384),15)) and  |
|     mult_r(-32768,-32768) = 32767.                                        |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0x8000 <= var_out <= 0x7fff.                          |
|___________________________________________________________________________|
*/
static inline short mult_r(short var1, short var2)
{
	short var_out;
	int L_product_arr;

	L_product_arr = (int)var1 *(int)var2;       /* product */
	L_product_arr += (int)0x00004000L;      /* round */
	L_product_arr &= (int)0xffff8000L;
	L_product_arr >>= 15;       /* shift */

	if (L_product_arr & (int)0x00010000L)   /* sign extend when necessary */
	{
		L_product_arr |= (int)0xffff0000L;
	}
	var_out = saturate(L_product_arr);

#if (WMOPS)
	multiCounter[currCounter].mult_r++;
#endif
	return (var_out);
}


/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_shl                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Arithmetically shift the 32 bit input L_var1 left var2 positions. Zero  |
|   fill the var2 LSB of the result. if var2 is negative, arithmetically    |
|   shift L_var1 right by -var2 with sign extension. Saturate the result in |
|   case of underflows or overflows.                                        |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline int L_shl(int x, short shift)
{
	if (shift <= 0) {
		if (shift < -32) shift = -32;
#if (WMOPS)
		multiCounter[currCounter].L_shl++;
#endif
		return x >> (-shift);
	}
	if (shift <= norm_l(x) || x == 0){
#if (WMOPS)
		multiCounter[currCounter].L_shl++;
#endif
		return x << shift;
	}
#if (WMOPS)
	multiCounter[currCounter].L_shl++;
#endif
	return ((x < 0) ? MIN_32 : MAX_32);
}
#else
static inline int L_shl(int L_var1, short var2)
{
	int L_var_out = 0L;

	if (var2 <= 0)
	{
		if (var2 < -32)
			var2 = -32;
		var2 = -var2;
		L_var_out = L_shr(L_var1, var2);
#if (WMOPS)
		multiCounter[currCounter].L_shr--;
#endif
	}
	else
	{
		for (; var2 > 0; var2--)
		{
			if (L_var1 > (int)0X3fffffffL)
			{
				Overflow = 1;
				L_var_out = MAX_32;
				break;
			}
			else
			{
				if (L_var1 < (int)0xc0000000L)
				{
					Overflow = 1;
					L_var_out = MIN_32;
					break;
				}
			}
			L_var1 *= 2;
			L_var_out = L_var1;
		}
	}
#if (WMOPS)
	multiCounter[currCounter].L_shl++;
#endif
	return (L_var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_shr                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Arithmetically shift the 32 bit input L_var1 right var2 positions with  |
|   sign extension. if var2 is negative, arithmetically shift L_var1 left   |
|   by -var2 and zero fill the -var2 LSB of the result. Saturate the result |
|   in case of underflows or overflows.                                     |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|___________________________________________________________________________|
*/
static inline int L_shr(int L_var1, short var2)
{
	int L_var_out;

	if (var2 < 0)
	{
		if (var2 < -32)
			var2 = -32;
		L_var_out = L_shl(L_var1, -var2);
#if (WMOPS)
		multiCounter[currCounter].L_shl--;
#endif
	}
	else
	{
		if (var2 >= 31)
		{
			L_var_out = (L_var1 < 0L) ? -1 : 0;
		}
		else
		{
			if (L_var1 < 0)
			{
				L_var_out = ~((~L_var1) >> var2);
			}
			else
			{
				L_var_out = L_var1 >> var2;
			}
		}
	}
#if (WMOPS)
	multiCounter[currCounter].L_shr++;
#endif
	return (L_var_out);
}

/*___________________________________________________________________________
|                                                                           |
|   Function Name : shr_r                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Same as shr(var1,var2) but with rounding. Saturate the result in case of|
|   underflows or overflows :                                               |
|    - if var2 is greater than zero :                                       |
|          if (sub(shl(shr(var1,var2),1),shr(var1,sub(var2,1))))            |
|          is equal to zero                                                 |
|                     then                                                  |
|                     shr_r(var1,var2) = shr(var1,var2)                     |
|                     else                                                  |
|                     shr_r(var1,var2) = add(shr(var1,var2),1)              |
|    - if var2 is less than or equal to zero :                              |
|                     shr_r(var1,var2) = shr(var1,var2).                    |
|                                                                           |
|   Complexity weight : 3                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
|___________________________________________________________________________|
*/
static inline short shr_r(short var1, short var2)
{
	short var_out;

	if (var2 > 15)
	{
		var_out = 0;
	}
	else
	{
		var_out = shr(var1, var2);

#if (WMOPS)
		multiCounter[currCounter].shr--;
#endif

		if (var2 > 0)
		{
			if ((var1 & ((short)1 << (var2 - 1))) != 0)
			{
				var_out++;
			}
		}
	}

#if (WMOPS)
	multiCounter[currCounter].shr_r++;
#endif
	return (var_out);
}


/*___________________________________________________________________________
|                                                                           |
|   Function Name : mac_r                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Multiply var1 by var2 and shift the result left by 1. Add the 32 bit    |
|   result to L_var3 with saturation. Round the LS 16 bits of the result    |
|   into the MS 16 bits with saturation and shift the result right by 16.   |
|   Return a 16 bit result.                                                 |
|            mac_r(L_var3,var1,var2) = round(L_mac(L_var3,var1,var2))       |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var3   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0x0000 8000 <= L_var_out <= 0x0000 7fff.              |
|___________________________________________________________________________|
*/
static inline short mac_r(int L_var3, short var1, short var2)
{
	short var_out;

	L_var3 = L_mac(L_var3, var1, var2);
	L_var3 = L_add(L_var3, (int)0x00008000L);
	var_out = extract_h(L_var3);

#if (WMOPS)
	multiCounter[currCounter].L_mac--;
	multiCounter[currCounter].L_add--;
	multiCounter[currCounter].extract_h--;
	multiCounter[currCounter].mac_r++;
#endif
	return (var_out);
}


/*___________________________________________________________________________
|                                                                           |
|   Function Name : msu_r                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Multiply var1 by var2 and shift the result left by 1. Subtract the 32   |
|   bit result from L_var3 with saturation. Round the LS 16 bits of the res-|
|   ult into the MS 16 bits with saturation and shift the result right by   |
|   16. Return a 16 bit result.                                             |
|            msu_r(L_var3,var1,var2) = round(L_msu(L_var3,var1,var2))       |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var3   32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0x0000 8000 <= L_var_out <= 0x0000 7fff.              |
|___________________________________________________________________________|
*/
static inline short msu_r(int L_var3, short var1, short var2)
{
	short var_out;

	L_var3 = L_msu(L_var3, var1, var2);
	L_var3 = L_add(L_var3, (int)0x00008000L);
	var_out = extract_h(L_var3);

#if (WMOPS)
	multiCounter[currCounter].L_msu--;
	multiCounter[currCounter].L_add--;
	multiCounter[currCounter].extract_h--;
	multiCounter[currCounter].msu_r++;
#endif
	return (var_out);
}


/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_deposit_h                                             |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Deposit the 16 bit var1 into the 16 MS bits of the 32 bit output. The   |
|   16 LS bits of the output are zeroed.                                    |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= var_out <= 0x7fff 0000.                |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline int L_deposit_h(short var1)
{
	int L_var_out;

	__asm(
	"lsl %0,%1,#16;\n"
		:"=r"(L_var_out)
		: "r"(var1)
		);
#if (WMOPS)
	multiCounter[currCounter].L_deposit_h++;
#endif
	return (L_var_out);
}
#else
static inline int L_deposit_h(short var1)
{
	int L_var_out;

	L_var_out = (int)var1 << 16;

#if (WMOPS)
	multiCounter[currCounter].L_deposit_h++;
#endif
	return (L_var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_deposit_l                                             |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Deposit the 16 bit var1 into the 16 LS bits of the 32 bit output. The   |
|   16 MS bits of the output are sign extended.                             |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0xFFFF 8000 <= var_out <= 0x0000 7fff.                |
|___________________________________________________________________________|
*/
static inline int L_deposit_l(short var1)
{
	int L_var_out;

	L_var_out = (int)var1;

#if (WMOPS)
	multiCounter[currCounter].L_deposit_l++;
#endif
	return (L_var_out);
}


/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_shr_r                                                 |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Same as L_shr(L_var1,var2) but with rounding. Saturate the result in    |
|   case of underflows or overflows :                                       |
|    - if var2 is greater than zero :                                       |
|          if (L_sub(L_shl(L_shr(L_var1,var2),1),L_shr(L_var1,sub(var2,1))))|
|          is equal to zero                                                 |
|                     then                                                  |
|                     L_shr_r(L_var1,var2) = L_shr(L_var1,var2)             |
|                     else                                                  |
|                     L_shr_r(L_var1,var2) = L_add(L_shr(L_var1,var2),1)    |
|    - if var2 is less than or equal to zero :                              |
|                     L_shr_r(L_var1,var2) = L_shr(L_var1,var2).            |
|                                                                           |
|   Complexity weight : 3                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1                                                                 |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= var_out <= 0x7fff ffff.                |
|___________________________________________________________________________|
*/
static inline int L_shr_r(int L_var1, short var2)
{
	int L_var_out;

	if (var2 > 31)
	{
		L_var_out = 0;
	}
	else
	{
		L_var_out = L_shr(L_var1, var2);

#if (WMOPS)
		multiCounter[currCounter].L_shr--;
#endif
		if (var2 > 0)
		{
			if ((L_var1 & ((int)1 << (var2 - 1))) != 0)
			{
				L_var_out++;
			}
		}
	}

#if (WMOPS)
	multiCounter[currCounter].L_shr_r++;
#endif
	return (L_var_out);
}


/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_abs                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|    Absolute value of L_var1; Saturate in case where the input is          |
|                                                               -2147483648 |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1                                                                 |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x0000 0000 <= var_out <= 0x7fff ffff.                |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline int L_abs(int L_var1)
{
	if (L_var1 >= 0)
	{
#if (WMOPS)
		multiCounter[currCounter].L_abs++;
#endif
		return (L_var1);
	}
	else
	{
		int L_var_out;
		int temp = 0;
		__asm(
			"qsub	%0,%1,%2;\n"
			:"=r"(L_var_out)
			:"r"(temp),"r"(L_var1)
			);
#if (WMOPS)
		multiCounter[currCounter].L_abs++;
#endif
		return (L_var_out);
	}
}
#else
static inline int L_abs(int L_var1)
{
	int L_var_out;

	if (L_var1 == MIN_32)
	{
		L_var_out = MAX_32;
	}
	else
	{
		if (L_var1 < 0)
		{
			L_var_out = -L_var1;
		}
		else
		{
			L_var_out = L_var1;
		}
	}

#if (WMOPS)
	multiCounter[currCounter].L_abs++;
#endif
	return (L_var_out);
}
#endif

/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_sat                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|    32 bit L_var1 is set to 2147483647 if an overflow occured or to        |
|    -2147483648 if an underflow occured on the most recent L_add_c,        |
|    L_sub_c, L_macNs or L_msuNs operations. The carry and overflow values  |
|    are binary values which can be tested and assigned values.             |
|                                                                           |
|   Complexity weight : 4                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1                                                                 |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= var_out <= 0x7fff ffff.                |
|___________________________________________________________________________|
*/
static inline int L_sat(int L_var1)
{
	int L_var_out;

	L_var_out = L_var1;

	if (Overflow)
	{

		if (Carry)
		{
			L_var_out = MIN_32;
		}
		else
		{
			L_var_out = MAX_32;
		}

		Carry = 0;
		Overflow = 0;
	}

#if (WMOPS)
	multiCounter[currCounter].L_sat++;
#endif
	return (L_var_out);
}


/*___________________________________________________________________________
|                                                                           |
|   Function Name : norm_s                                                  |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Produces the number of left shift needed to normalize the 16 bit varia- |
|   ble var1 for positive values on the interval with minimum of 16384 and  |
|   maximum of 32767, and for negative values on the interval with minimum  |
|   of -32768 and maximum of -16384; in order to normalize the result, the  |
|   following operation must be done :                                      |
|                    norm_var1 = shl(var1,norm_s(var1)).                    |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0x0000 0000 <= var_out <= 0x0000 000f.                |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline short norm_s(short x)
{
	int var_out = x ^ ((int)x << 17);
	__asm(
		"clz %0,%0;\n"
		:"=r"(var_out)
		: "r"(var_out)
		);
#if (WMOPS)
	multiCounter[currCounter].norm_s++;
#endif
	return (var_out & 15);
}
#else
static inline short norm_s(short var1)
{
	short var_out;

	if (var1 == 0)
	{
		var_out = 0;
	}
	else
	{
		if (var1 == (short)0xffff)
		{
			var_out = 15;
		}
		else
		{
			if (var1 < 0)
			{
				var1 = ~var1;
			}
			for (var_out = 0; var1 < 0x4000; var_out++)
			{
				var1 <<= 1;
			}
		}
	}

#if (WMOPS)
	multiCounter[currCounter].norm_s++;
#endif
	return (var_out);
}
#endif


/*___________________________________________________________________________
|                                                                           |
|   Function Name : div_s                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Produces a result which is the fractional integer division of var1  by  |
|   var2; var1 and var2 must be positive and var2 must be greater or equal  |
|   to var1; the result is positive (leading bit equal to 0) and truncated  |
|   to 16 bits.                                                             |
|   if var1 = var2 then div(var1,var2) = 32767.                             |
|                                                                           |
|   Complexity weight : 18                                                  |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    var1                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0x0000 0000 <= var1 <= var2 and var2 != 0.            |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : var1 <= var2 <= 0x0000 7fff and var2 != 0.            |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0x0000 0000 <= var_out <= 0x0000 7fff.                |
|             It's a Q15 value (point between b15 and b14).                 |
|___________________________________________________________________________|
*/
#ifdef PLATFORM
static inline short div_s(short x, short y)
{
	if ((x > y) || (x < 0) || (y < 0))
	{
		printf("Division Error var1=%d  var2=%d\n", x, y);
		abort(); /* exit (0); */
	}
	if (y == 0)
	{
		printf("Division by 0, Fatal error \n");
		abort(); /* exit (0); */
	}
	if (x == 0)
	{
#if (WMOPS)
		multiCounter[currCounter].div_s++;
#endif
		return 0;
	}
	if (x == y)
	{
#if (WMOPS)
		multiCounter[currCounter].div_s++;
#endif
		return MAX_16;
	}
	int quot;
	quot = 0x8000 * x;
	quot /= y;
#if (WMOPS)
	multiCounter[currCounter].div_s++;
#endif
	if (quot > MAX_16)
		return MAX_16;
	else
		return (short)quot;
}
#else
static inline short div_s(short var1, short var2)
{
	short var_out = 0;
	short iteration;
	int L_num;
	int L_denom;

	if ((var1 > var2) || (var1 < 0) || (var2 < 0))
	{
		printf("Division Error var1=%d  var2=%d\n", var1, var2);
		abort(); /* exit (0); */
	}
	if (var2 == 0)
	{
		printf("Division by 0, Fatal error \n");
		abort(); /* exit (0); */
	}
	if (var1 == 0)
	{
		var_out = 0;
	}
	else
	{
		if (var1 == var2)
		{
			var_out = MAX_16;
		}
		else
		{
			L_num = L_deposit_l(var1);
			L_denom = L_deposit_l(var2);

#if (WMOPS)
			multiCounter[currCounter].L_deposit_l--;
			multiCounter[currCounter].L_deposit_l--;
#endif

			for (iteration = 0; iteration < 15; iteration++)
			{
				var_out <<= 1;
				L_num <<= 1;

				if (L_num >= L_denom)
				{
					L_num = L_sub(L_num, L_denom);
					var_out = add(var_out, 1);
#if (WMOPS)
					multiCounter[currCounter].L_sub--;
					multiCounter[currCounter].add--;
#endif
				}
			}
		}
	}

#if (WMOPS)
	multiCounter[currCounter].div_s++;
#endif
	return (var_out);
}
#endif


/*___________________________________________________________________________
|                                                                           |
|   Function Name : norm_l                                                  |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Produces the number of left shifts needed to normalize the 32 bit varia-|
|   ble L_var1 for positive values on the interval with minimum of          |
|   1073741824 and maximum of 2147483647, and for negative values on the in-|
|   terval with minimum of -2147483648 and maximum of -1073741824; in order |
|   to normalize the result, the following operation must be done :         |
|                   norm_L_var1 = L_shl(L_var1,norm_l(L_var1)).             |
|                                                                           |
|   Complexity weight : 1                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1                                                                 |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0x0000 0000 <= var_out <= 0x0000 001f.                |
|___________________________________________________________________________|
*/
// TODO: not bitexact
#if 0//def PLATFORM
static inline short norm_l(int x)
{
	int var_out = x ^ (x << 1);
	__asm(
	"clz %0,%0;\n"
		:"=r"(var_out)
		: "r"(var_out)
		);
#if (WMOPS)
	multiCounter[currCounter].norm_l++;
#endif
	return (var_out & 31);
}
#else
static inline short norm_l(int L_var1)
{
	short var_out;

	if (L_var1 == 0)
	{
		var_out = 0;
	}
	else
	{
		if (L_var1 == (int)0xffffffffL)
		{
			var_out = 31;
		}
		else
		{
			if (L_var1 < 0)
			{
				L_var1 = ~L_var1;
			}
			for (var_out = 0; L_var1 < (int)0x40000000L; var_out++)
			{
				L_var1 <<= 1;
			}
		}
	}

#if (WMOPS)
	multiCounter[currCounter].norm_l++;
#endif
	return (var_out);
}
#endif

/*
******************************************************************************
* Additional operators extracted from the G.723.1 Library
* Adapted for WMOPS calculations
******************************************************************************
*/

/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_mls                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Multiplies a 16 bit word v by a 32 bit word Lv and returns a 32 bit     |
|   word (multiplying 16 by 32 bit words gives 48 bit word; the function    |
|   extracts the 32 MSB and shift the result to the left by 1).             |
|                                                                           |
|   A 32 bit word can be written as                                         |
|    Lv = a  +  b * 2^16                                                    |
|   where a= unsigned 16 LSBs and b= signed 16 MSBs.                        |
|   The function returns v * Lv  /  2^15  which is equivalent to            |
|        a*v / 2^15 + b*v*2                                                 |
|                                                                           |
|   Complexity weight : 5													 |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|   Lv                                                                      |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
|   v                                                                       |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0x8000 <= var1 <= 0x7fff.                             |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x8000 0000 <= var_out <= 0x7fff ffff.                |
|                                                                           |
|___________________________________________________________________________|
*/
static inline int L_mls(int Lv, short v)
{
	int   Temp;

	Temp = Lv & (int)0x0000ffff;
	Temp = Temp * (int)v;
	Temp = L_shr(Temp, (short)15);
	Temp = L_mac(Temp, v, extract_h(Lv));

#if (WMOPS)
	multiCounter[currCounter].L_shr--;
	multiCounter[currCounter].L_mac--;
	multiCounter[currCounter].extract_h--;
	multiCounter[currCounter].L_mls++;
#endif

	return Temp;
}


/*__________________________________________________________________________
|                                                                           |
|   Function Name : div_l                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Produces a result which is the fractional integer division of L_var1 by |
|   var2; L_var1 and var2 must be positive and var2 << 16 must be greater or|
|   equal to L_var1; the result is positive (leading bit equal to 0) and    |
|   truncated to 16 bits.                                                   |
|   if L_var1 == var2 << 16 then div_l(L_var1,var2) = 32767.                |
|                                                                           |
|   Complexity weight : 32                                                  |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1                                                                 |
|             32 bit long signed integer (int) whose value falls in the  |
|             range : 0x0000 0000 <= var1 <= (var2 << 16)  and var2 != 0.   |
|             L_var1 must be considered as a Q.31 value                     |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (short) whose value falls in the |
|             range : var1 <= (var2<< 16) <= 0x7fff0000 and var2 != 0.      |
|             var2 must be considered as a Q.15 value                       |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit short signed integer (short) whose value falls in the |
|             range : 0x0000 0000 <= var_out <= 0x0000 7fff.                |
|             It's a Q15 value (point between b15 and b14).                 |
|___________________________________________________________________________|
*/
static inline short div_l(int  L_num, short den)
{
	short   var_out = (short)0;
	int   L_den;
	short   iteration;

#if (WMOPS)
	multiCounter[currCounter].div_l++;
#endif

	if (den == (short)0) {
		printf("Division by 0 in div_l, Fatal error \n");
		exit(0);
	}

	if ((L_num < (int)0) || (den < (short)0)) {
		printf("Division Error in div_l, Fatal error \n");
		exit(0);
	}

	L_den = L_deposit_h(den);
#if (WMOPS)
	multiCounter[currCounter].L_deposit_h--;
#endif

	if (L_num >= L_den){
		return MAX_16;
	}
	else {
		L_num = L_shr(L_num, (short)1);
		L_den = L_shr(L_den, (short)1);
#if (WMOPS)
		multiCounter[currCounter].L_shr -= 2;
#endif
		for (iteration = (short)0; iteration< (short)15; iteration++) {
			var_out = shl(var_out, (short)1);
			L_num = L_shl(L_num, (short)1);
#if (WMOPS)
			multiCounter[currCounter].shl--;
			multiCounter[currCounter].L_shl--;
#endif
			if (L_num >= L_den) {
				L_num = L_sub(L_num, L_den);
				var_out = add(var_out, (short)1);
#if (WMOPS)
				multiCounter[currCounter].L_sub--;
				multiCounter[currCounter].add--;
#endif
			}
		}

		return var_out;
	}
}


/*__________________________________________________________________________
|                                                                           |
|   Function Name : i_mult                                                  |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Integer 16-bit multiplication. No overflow protection is performed if   |
|   ORIGINAL_G7231 is defined.                                              |
|                                                                           |
|   Complexity weight : 3                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    a                                                                      |
|             16 bit short signed integer (short).                         |
|                                                                           |
|    b                                                                      |
|             16 bit short signed integer (short).                         |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|             16 bit short signed integer (short). No overflow checks      |
|             are performed if ORIGINAL_G7231 is defined.                   |
|___________________________________________________________________________|
*/
#if 0//def PLATFORM
static inline short i_mult(short a, short b)
{
#ifdef ORIGINAL_G7231
	return a*b;
#else
	int register c;
	short out;
	__asm(
	     "mul      %1,%2,%3;\n"
	     "ssat     %0,#16,%1;\n"
	     :"=r"(out)
	     :"r"(c),"r"(a),"r"(b)
	     );
#if (WMOPS)
	multiCounter[currCounter].i_mult++;
#endif
	return out;
#endif
}
#else
static inline short i_mult(short a, short b)
{
#ifdef ORIGINAL_G7231
	return a*b;
#else
	int register c = a*b;
#if (WMOPS)
	multiCounter[currCounter].i_mult++;
#endif
	return saturate(c);
#endif
}
#endif


/*
******************************************************************************
* The following three operators are not part of the original
* G.729/G.723.1 set of basic operators and implement shiftless
* accumulation operation.
******************************************************************************
*/

/*___________________________________________________________________________
|
|   Function Name : L_mult0
|
|   Purpose :
|
|   L_mult0 is the 32 bit result of the multiplication of var1 times var2
|   without one left shift.
|
|   Complexity weight : 1
|
|   Inputs :
|
|    var1     16 bit short signed integer (short) whose value falls in the
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.
|
|    var2     16 bit short signed integer (short) whose value falls in the
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.
|
|   Return Value :
|
|    L_var_out
|             32 bit long signed integer (int) whose value falls in the
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
|___________________________________________________________________________
*/
// TODO: Original function generate smulbb, optimized version use mul, thus cause system halt
#if 0//def PLATFORM
static inline int L_mult0(short var1, short var2)
{
	int L_var_out;

	__asm(
              "mul %0,%1,%2;\n"
              :"=r"(L_var_out)
              :"r"(var1),"r"(var2)
              );

#if (WMOPS)
	multiCounter[currCounter].L_mult0++;
#endif
	return(L_var_out);
}
#else
static inline int L_mult0(short var1, short var2)
{
	int L_var_out;

	L_var_out = (int)var1 * (int)var2;

#if (WMOPS)
	multiCounter[currCounter].L_mult0++;
#endif
	return(L_var_out);
}
#endif

/*___________________________________________________________________________
|
|   Function Name : L_mac0
|
|   Purpose :
|
|   Multiply var1 by var2 (without left shift) and add the 32 bit result to
|   L_var3 with saturation, return a 32 bit result:
|        L_mac0(L_var3,var1,var2) = L_add(L_var3,(L_mult0(var1,var2)).
|
|   Complexity weight : 1
|
|   Inputs :
|
|    L_var3   32 bit long signed integer (int) whose value falls in the
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.
|
|    var1     16 bit short signed integer (short) whose value falls in the
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.
|
|    var2     16 bit short signed integer (short) whose value falls in the
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.
|
|   Return Value :
|
|    L_var_out
|             32 bit long signed integer (int) whose value falls in the
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
|___________________________________________________________________________
*/
// TODO: not bitexact
#if 0//def PLATFORM
static inline int L_mac0(int L_var3, short var1, short var2)
{
	int L_var_out;
	int L_product;

	__asm(
		"mul %1,%2,%3;\n"
		"qadd %0,%1,%4;\n"
		:"=r"(L_var_out)
		:"r"(L_product),"r"(var1),"r"(var2),"r"(L_var3)
		);
	if(L_var_out!=(L_product+L_var3))
		Overflow = 1;
#if (WMOPS)
	multiCounter[currCounter].L_mac0++;
#endif
	return(L_var_out);
}
#else
static inline int L_mac0(int L_var3, short var1, short var2)
{
	int L_var_out;
	int L_product;

	L_product = L_mult0(var1, var2);
	L_var_out = L_add(L_var3, L_product);

#if (WMOPS)
	multiCounter[currCounter].L_mac0++;
	multiCounter[currCounter].L_mult0--;
	multiCounter[currCounter].L_add--;
#endif
	return(L_var_out);
}
#endif

/*___________________________________________________________________________
|
|   Function Name : L_msu0
|
|   Purpose :
|
|   Multiply var1 by var2 (without left shift) and subtract the 32 bit
|   result to L_var3 with saturation, return a 32 bit result:
|        L_msu0(L_var3,var1,var2) = L_sub(L_var3,(L_mult0(var1,var2)).
|
|   Complexity weight : 1
|
|   Inputs :
|
|    L_var3   32 bit long signed integer (int) whose value falls in the
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.
|
|    var1     16 bit short signed integer (short) whose value falls in the
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.
|
|    var2     16 bit short signed integer (short) whose value falls in the
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.
|
|   Return Value :
|
|    L_var_out
|             32 bit long signed integer (int) whose value falls in the
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
|___________________________________________________________________________
*/
#if 0//def PLATFORM
static inline int L_msu0(int L_var3, short var1, short var2)
{
	int L_var_out;
	int L_product;

	__asm(
		"mul      %1,%2,%3;\n"
		"qsub     %0,%4,%1;\n"
		:"=r"(L_var_out)
		:"r"(L_product),"r"(var1),"r"(var2),"r"(L_var3)
		);

#if (WMOPS)
	multiCounter[currCounter].L_msu0++;
#endif
	return(L_var_out);
}
#else
static inline int L_msu0(int L_var3, short var1, short var2)
{
	int L_var_out;
	int L_product;

	L_product = L_mult0(var1, var2);
	L_var_out = L_sub(L_var3, L_product);

#if (WMOPS)
	multiCounter[currCounter].L_msu0++;
	multiCounter[currCounter].L_mult0--;
	multiCounter[currCounter].L_sub--;
#endif
	return(L_var_out);
}
#endif

static inline void W16copy(Word16 *y, Word16 *x, int size)
{
   int i;

   for (i=0;i<size;i++)
   {
     *y++ = *x++;
#if (WMOPS)
     move16();
#endif
   }
}

static inline void W16zero(Word16 *x, int size)
{
  int i;

  for (i=0;i<size;i++)
  {
#if (WMOPS)
     move16();
#endif
     *x++ = 0;
  }
}


/*___________________________________________________________________________
 | Function Mpy_32()                                                         |
 |                                                                           |
 |   Multiply two 32 bit integers (DPF). The result is divided by 2**31      |
 |                                                                           |
 |   L_32 = (hi1*hi2)<<1 + ( (hi1*lo2)>>15 + (lo1*hi2)>>15 )<<1              |
 |                                                                           |
 |   This operation can also be viewed as the multiplication of two Q31      |
 |   number and the result is also in Q31.                                   |
 |                                                                           |
 | Arguments:                                                                |
 |                                                                           |
 |  hi1         hi part of first number                                      |
 |  lo1         lo part of first number                                      |
 |  hi2         hi part of second number                                     |
 |  lo2         lo part of second number                                     |
 |                                                                           |
 |___________________________________________________________________________|
*/
static inline Word32 Mpy_32(Word16 hi1, Word16 lo1, Word16 hi2, Word16 lo2)
{
  Word32 L_32;

  L_32 = L_mult(hi1, hi2);
  L_32 = L_mac(L_32, mult(hi1, lo2) , 1);
  L_32 = L_mac(L_32, mult(lo1, hi2) , 1);

  return( L_32 );
}

/*___________________________________________________________________________
 | Function Mpy_32_16()                                                      |
 |                                                                           |
 |   Multiply a 16 bit integer by a 32 bit (DPF). The result is divided      |
 |   by 2**15                                                                |
 |                                                                           |
 |   This operation can also be viewed as the multiplication of a Q31        |
 |   number by a Q15 number, the result is in Q31.                           |
 |                                                                           |
 |   L_32 = (hi1*lo2)<<1 + ((lo1*lo2)>>15)<<1                                |
 |                                                                           |
 | Arguments:                                                                |
 |                                                                           |
 |  hi          hi part of 32 bit number.                                    |
 |  lo          lo part of 32 bit number.                                    |
 |  n           16 bit number.                                               |
 |                                                                           |
 |___________________________________________________________________________|
*/
static inline Word32 Mpy_32_16(Word16 hi, Word16 lo, Word16 n)
{
  Word32 L_32;

  L_32 = L_mult(hi, n);
  L_32 = L_mac(L_32, mult(lo, n) , 1);

  return( L_32 );
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : Div_32                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |             Fractional integer division of two 32 bit numbers.            |
 |             L_num / L_denom.                                              |
 |             L_num and L_denom must be positive and L_num < L_denom.       |
 |             L_denom = denom_hi<<16 + denom_lo<<1                          |
 |             denom_hi is a normalize number.                               |
 |             The result is in Q30.                                         |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_num                                                                  |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x0000 0000 < L_num < L_denom                         |
 |                                                                           |
 |    L_denom = denom_hi<<16 + denom_lo<<1      (DPF)                        |
 |                                                                           |
 |       denom_hi                                                            |
 |             16 bit positive normalized integer whose value falls in the   |
 |             range : 0x4000 < hi < 0x7fff                                  |
 |       denom_lo                                                            |
 |             16 bit positive integer whose value falls in the              |
 |             range : 0 < lo < 0x7fff                                       |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_div                                                                  |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x0000 0000 <= L_div <= 0x7fff ffff.                  |
 |             It's a Q31 value                                              |
 |                                                                           |
 |  Algorithm:                                                               |
 |                                                                           |
 |  - find = 1/L_denom.                                                      |
 |      First approximation: approx = 1 / denom_hi                           |
 |      1/L_denom = approx * (2.0 - L_denom * approx )                       |
 |                                                                           |
 |  -  result = L_num * (1/L_denom)                                          |
 |___________________________________________________________________________|
*/
static inline Word32 Div_32(Word32 L_num, Word16 denom_hi, Word16 denom_lo)
{
  Word16 approx, hi, lo, n_hi, n_lo;
  Word32 L_32;


  /* First approximation: 1 / L_denom = 1/denom_hi */

  approx = div_s( (Word16)0x3fff, denom_hi);    /* result in Q14 */
                                                /* Note: 3fff = 0.5 in Q15 */

  /* 1/L_denom = approx * (2.0 - L_denom * approx) */

  L_32 = Mpy_32_16(denom_hi, denom_lo, approx); /* result in Q30 */


  L_32 = L_sub( (Word32)0x7fffffffL, L_32);      /* result in Q30 */

  L_Extract(L_32, &hi, &lo);

  L_32 = Mpy_32_16(hi, lo, approx);             /* = 1/L_denom in Q29 */

  /* L_num * (1/L_denom) */

  L_Extract(L_32, &hi, &lo);
  L_Extract(L_num, &n_hi, &n_lo);
  L_32 = Mpy_32(n_hi, n_lo, hi, lo);            /* result in Q29   */
  L_32 = L_shl(L_32, 2);                        /* From Q29 to Q31 */

  return( L_32 );
}

/*___________________________________________________________________________
 |                                                                           |
 |  Function L_Extract()                                                     |
 |                                                                           |
 |  Extract from a 32 bit integer two 16 bit DPF.                            |
 |                                                                           |
 |  Arguments:                                                               |
 |                                                                           |
 |   L_32      : 32 bit integer.                                             |
 |               0x8000 0000 <= L_32 <= 0x7fff ffff.                         |
 |   hi        : b16 to b31 of L_32                                          |
 |   lo        : (L_32 - hi<<16)>>1                                          |
 |___________________________________________________________________________|
*/
static inline void L_Extract(Word32 L_32, Word16 *hi, Word16 *lo)
{
  *hi  = extract_h(L_32);
  *lo  = extract_l( L_msu( L_shr(L_32, 1) , *hi, 16384));  /* lo = L_32>>1   */

//#if (WMOPS)
 // multiCounter[currCounter].extract_h--;
 // multiCounter[currCounter].extract_l--;
 // multiCounter[currCounter].L_Extract++;
//#endif

  return;
}

/*___________________________________________________________________________
 |                                                                           |
 |  Function L_Comp()                                                        |
 |                                                                           |
 |  Compose from two 16 bit DPF a 32 bit integer.                            |
 |                                                                           |
 |     L_32 = hi<<16 + lo<<1                                                 |
 |                                                                           |
 |  Arguments:                                                               |
 |                                                                           |
 |   hi        msb                                                           |
 |   lo        lsf (with sign)                                               |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_32 <= 0x7fff fff0.                   |
 |                                                                           |
 |___________________________________________________________________________|
*/
static inline Word32 L_Comp(Word16 hi, Word16 lo)
{
  Word32 L_32;

  L_32 = L_deposit_h(hi);

//#if (WMOPS)
 // multiCounter[currCounter].L_deposit_h--;
 // multiCounter[currCounter].L_mac--;
 // multiCounter[currCounter].L_Comp++;
//#endif

  return( L_mac(L_32, lo, 1));          /* = hi<<16 + lo<<1 */
}

static inline Word16 s_and(Word16 var1, Word16 var2) {
	Word16 var_out;

	var_out = var1 & var2;

#if (WMOPS)
	multiCounter[currCounter].s_and++;
#endif /* ifdef WMOPS */

	return(var_out);
}

static inline Word32 L_max(Word32 L_var1, Word32 L_var2) {
	Word32 L_var_out;

	if (L_var1 >= L_var2)
		L_var_out = L_var1;
	else
		L_var_out = L_var2;

#if (WMOPS)
	multiCounter[currCounter].L_max++;
#endif /* ifdef WMOPS */

	return(L_var_out);
}

static inline Word16 s_max(Word16 var1, Word16 var2) {
	Word16 var_out;

	if (var1 >= var2)
		var_out = var1;
	else
		var_out = var2;

#if (WMOPS)
	multiCounter[currCounter].s_max++;
#endif /* ifdef WMOPS */

	return(var_out);
}

static inline Word16 s_min(Word16 var1, Word16 var2) {
	Word16 var_out;

	if (var1 <= var2)
		var_out = var1;
	else
		var_out = var2;

#if (WMOPS)
	multiCounter[currCounter].s_min++;
#endif /* ifdef WMOPS */

	return(var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : Log2()                                                  |
 |                                                                           |
 |       Compute log2(L_x).                                                  |
 |       L_x is positive.                                                    |
 |                                                                           |
 |       if L_x is negative or zero, result is 0.                            |
 |---------------------------------------------------------------------------|
 |  Algorithm:                                                               |
 |                                                                           |
 |   The function Log2(L_x) is approximated by a table and linear            |
 |   interpolation.                                                          |
 |                                                                           |
 |   1- Normalization of L_x.                                                |
 |   2- exponent = 30-exponent                                               |
 |   3- i = bit25-b31 of L_x,    32 <= i <= 63  ->because of normalization.  |
 |   4- a = bit10-b24                                                        |
 |   5- i -=32                                                               |
 |   6- fraction = tablog[i]<<16 - (tablog[i] - tablog[i+1]) * a * 2            |
 |___________________________________________________________________________|
*/
static inline void Log2(
  Word32 L_x,       /* (i) Q0 : input value                                 */
  Word16 *exponent, /* (o) Q0 : Integer part of Log2.   (range: 0<=val<=30) */
  Word16 *fraction  /* (o) Q15: Fractional  part of Log2. (range: 0<=val<1) */
)
{
  Word16 exp, i, a, tmp;
  Word32 L_y;

  if( L_x <= (Word32)0 )
  {
    *exponent = 0;
    *fraction = 0;
#if WMOPS
    move16();move16();
#endif
    return;
  }

  exp = norm_l(L_x);
  L_x = L_shl(L_x, exp );               /* L_x is normalized */

  *exponent = sub(30, exp);

  L_x = L_shr(L_x, 9);
  i   = extract_h(L_x);                 /* Extract b25-b31 */
  L_x = L_shr(L_x, 1);
  a   = extract_l(L_x);                 /* Extract b10-b24 of fraction */
  a   = s_and(a, (Word16)0x7fff);

  i   = sub(i, 32);

  L_y = L_deposit_h(tablog[i]);          /* tablog[i] << 16        */
  tmp = sub(tablog[i], tablog[add(i, 1)]);      /* tablog[i] - tablog[i+1] */
  L_y = L_msu(L_y, tmp, a);             /* L_y -= tmp*a*2        */

  *fraction = extract_h( L_y);
#if WMOPS
  move16();move16();
#endif
  return;
}
