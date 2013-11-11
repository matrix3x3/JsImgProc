/*
*	debug.h
*/

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "dbgspt.h"

#define __dbgbreak() {__asm int 3}

#ifdef _DEBUG

#define ASSERT(e) do\
	{\
		if (!(e))\
		{\
			printf("\nASSERT failed at:\n >File name: %s\n >Function :\
			%s\n >Line No. : %d\n >Condition: %s\n", \
			__FILE__,__FUNCTION__, __LINE__, #e);\
			__dbgbreak();\
		}\
	} while (false)

#else
	#define ASSERT(unused) do {} while (false)
#endif

/*-----------------------------
*  newly added , 2013.7.03
*/
#if _WIN32 || _WIN64
	#define inline __inline
	//#define DEBUG  _DEBUG
#endif
DBGSUPPORT_API extern void dbgPrintA(const char* file, const char* func, int line, const char* format, ...);
#ifdef _DEBUG
	#define dbgPrint(_format, ...)  dbgPrintA(__FILE__, __FUNCTION__, __LINE__, _format, __VA_ARGS__)
#else
	#define dbgPrint(_format, ...) do {} while (false)
#endif /* !DEBUG */

/* like printf */
#ifdef _DEBUG
	#define PRT(fmt, ...) \
	do\
	{\
		/*printf("[DEBUG, %s:%s:%d]" fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\*/\
		printf("[DEBUG, %s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
		printf("\n");\
	} while(false)

#else
	#define PRT(fmt, ...) do {} while (false)
#endif

#include "logger.h"

#define PrtAndLog(fmt, ...) \
	/*dbgPrint(fmt, __VA_ARGS__);\*/\
	sl_debug(fmt, __VA_ARGS__);\
	PRT(fmt, __VA_ARGS__)


/*----------------------------*/


#endif