#include "stdafx.h"
/*
 * Copyright (c) 2010, 
 *      ASMlove. All rights reserved.
 * 
 * Use, modification and distribution are subject to the 
 * "GNU GPL" at listed at <http://www.gnu.org/licenses/>. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/timeb.h>
#if _WIN32 || _WIN64
#include <windows.h>
#pragma warning(disable:4996)
#define sleep(s) Sleep(s)
#endif
#include "debug.h"

struct _LogTime {
    unsigned char  _hour;
    unsigned char  _min;
    unsigned char  _sec;
    unsigned short _usec;
};
static int g_isTake = 0;
static inline void 
_getLogTime(struct _LogTime* _tm) {
    struct timeb tb    = {0};
    struct tm*   local = NULL;
    if (NULL == _tm) return;
    ftime(&tb);
    local = localtime(&(tb.time));
    _tm->_hour = local->tm_hour;
    _tm->_min  = local->tm_min;
    _tm->_sec  = local->tm_sec;
    _tm->_usec = tb.millitm;
} 
static inline int 
_debugPrintA(const char* _debugSTR, 
             const char* _file, const char* _func, int _line) {
    FILE* _File = fopen("debug.log", "a+");
    struct _LogTime lt = {0};
    if (NULL == _debugSTR || NULL == _File || 
        NULL == _file || NULL == _func) 
        return 0;
    _getLogTime(&lt);
    fprintf(_File, "[%.2d:%.2d:%.2d:%.3d]%s, %s[%d]: %s\n", 
        lt._hour, lt._min, lt._sec, lt._usec, _file, _func, _line, _debugSTR);
    fflush(_File);

    if (NULL != _File) {
        fclose(_File);
        _File = NULL;
    }
    return 1;
}
static inline int 
_debugInternalA(const char* _format, va_list _argList, 
                const char* _file, const char* _func, int _line) {
    char buffer[1024] = {0};
    if (NULL == _format || NULL == _argList || 
        NULL == _file || NULL == _func)
        return 0;
    vsprintf(buffer, _format, _argList);
    return _debugPrintA(buffer, _file, _func, _line);
}
void 
dbgPrintA(const char* file, const char* func, 
          int line, const char* format, ...) {
    va_list argList = NULL;
    while (g_isTake) sleep(10);
    g_isTake = 1;
    va_start(argList, format);
    _debugInternalA(format, argList, file, func, line);
    va_end(argList);
    g_isTake = 0;
}