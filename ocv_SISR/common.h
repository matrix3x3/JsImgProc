#pragma once

#ifdef OCV_SISR_EXPORTS
#define OCV_SISR_API __declspec(dllexport) 
#else
#define OCV_SISR_API __declspec(dllimport) 
#endif


/*
*	function return values
*/

/* Add log infos for reason why function fails or successes */

enum 
{
	SUCCESS = 0,FAIL = 1
};

#define SISR_DOUBLE /*double*/ float
#define SISR_INT	 int
#define SISR_MAT_TYPE CV_32F

/* check status */
/* validate input parameters */
/*	if (X.cols == 0)
	{
		PrtAndLog("Fail: X is empty");
		ASSERT(0);
		return FAIL;
	}
*/
/* if expr == true then fail this function then return */
#define CheckFail(expr, str) \
	if((expr))\
	{\
		PrtAndLog(str);\
		ASSERT(0);\
		return FAIL;\
	}
