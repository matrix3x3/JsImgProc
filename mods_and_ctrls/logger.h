/*
 * Copyright (c) 2011,
 *      ASMlove. All rights reserved.
 *
 *      http://blog.csdn.net/zfxfcx/
 *
 * Use, modification and distribution are subject to the
 * "GNU GPL" at listed at <http://www.gnu.org/licenses/>.
 */
#ifndef __SL_LOG_HEADER_H__
#define __SL_LOG_HEADER_H__

#ifndef _SL_LOG_
#define _SL_LOG_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "dbgspt.h"

	enum {
		SL_LOG_DEBUG = 0,
		SL_LOG_MSG   = 1,
		SL_LOG_WARN = 2,
		SL_LOG_ERR   = 3
	};

DBGSUPPORT_API extern void sl_log_err(const char* file, const char* func, 
		int line, int v, const char* fmt, ...);
DBGSUPPORT_API extern void sl_log_warn(const char* file, const char* func, 
		int line, const char* fmt, ...);
DBGSUPPORT_API extern void sl_log_errx(const char* file, const char* func, 
		int line, int v, const char* fmt, ...);
DBGSUPPORT_API extern void sl_log_warnx(const char* file, const char* func, 
		int line, const char* fmt, ...);
DBGSUPPORT_API extern void sl_log_msgx(const char* file, const char* func, 
		int line, const char* fmt, ...);
DBGSUPPORT_API extern void sl_log_debugx(const char* file, const char* func, 
		int line,const char* fmt, ...);

#if _WIN32 || _WIN64
    #if _DEBUG
        #define sl_debug(fmt, ...)    sl_log_debugx(__FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
    #else
        #define sl_debug(fmt, ...)
    #endif
#else
    #ifndef NDEBUG
        #define sl_debug(fmt, ...)    sl_log_debugx(__FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
    #else
        #define sl_debug(fmt, ...)
    #endif
#endif

//#if _WIN32 || _WIN64
//    #if _DEBUG
//        #define sl_debug    sl_log_debugx
//    #else
//        #define sl_debug
//    #endif
//#else
//    #ifndef NDEBUG
//        #define sl_debug    sl_log_debugx
//    #else
//        #define sl_debug
//    #endif
//#endif
 
 
#ifdef __cplusplus
}
#endif /* __cplusplus */
 
#endif /* _SL_LOG_ */
 
#endif /* __SL_LOG_HEADER_H__ */