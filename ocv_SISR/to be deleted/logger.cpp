#include "stdafx.h"
/*
* Copyright (c) 2011,
*      ASMlove. All rights reserved.
*
*      http://blog.csdn.net/zfxfcx/
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
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include "logger.h"

#if _WIN32 || _WIN64
#define inline __inline
#pragma warning(disable:4996)
#endif


struct __log_time {
	unsigned char hour_;
	unsigned char min_;
	unsigned char sec_;
	unsigned short millitm_;
};


static inline void
	__get_log_time(struct __log_time* lt)
{
	struct timeb tb = {0};
	struct tm* local = NULL;

	if (NULL != lt) {
		ftime(&tb);
		local = localtime(&tb.time);
		lt->hour_ = local->tm_hour;
		lt->min_ = local->tm_min;
		lt->sec_ = local->tm_sec;
		lt->millitm_ = tb.millitm;
	}
}

static int
	__log_access(const char* path, int mode)
{
	return access(path, mode);
}

static int
	__log_mkdir(const char* path)
{
#if _WIN32 || _WIN64
	return mkdir(path);
#else
	return mkdir(path, S_IRWXU);
#endif
}

static int
	__log_snprintf(char* dest, size_t count, const char* fmt, ...)
{
	int ret = 0;
	va_list ap;

	va_start(ap, fmt);
	ret = vsnprintf(dest, count, fmt, ap);
	va_end(ap);

	return ret;
}

static void
	__sl_log(const char* file, const char* func, 
	int line, int severity, const char* msg)
{
	const char* severity_str;
	FILE* pFile = NULL;
	char fname[100] = {0};
	struct __log_time lt = {0};

	switch (severity) {
	case SL_LOG_DEBUG:
		severity_str = "debug";
		break;
	case SL_LOG_MSG:
		severity_str = "msg";
		break;
	case SL_LOG_WARN:
		severity_str = "warn";
		break;
	case SL_LOG_ERR:
		severity_str = "err";
		break;
	default:
		severity_str = "???";
	}

	__log_snprintf(fname, sizeof(fname), "./sl_log/%s.log", severity_str);
	if (0 != __log_access("./sl_log", 0))
		__log_mkdir("./sl_log");
	pFile = fopen(fname, "a+");

	if (NULL != pFile) {
		__get_log_time(&lt);
		/*fprintf(pFile, "[%.2d:%.2d:%.2d:%.3d] - %s\n",
			lt.hour_, lt.min_, lt.sec_, lt.millitm_, msg);*/
		fprintf(pFile, "[%.2d:%.2d:%.2d:%.3d] %s, %s[%d] - %s\n",
		lt.hour_, lt.min_, lt.sec_, lt.millitm_, file, func, line, msg);
		fclose(pFile);
	}
}

static void
	__log_helper(const char* file, const char* func, 
	int line, int severity, int log_errno, const char* fmt, va_list ap)
{
	char   buf[1024] = {0};
	size_t len = 0;

	if (NULL != fmt)
		vsnprintf(buf, sizeof(buf), fmt, ap);

	if (log_errno >= 0) {
		len = strlen(buf);
		if (len < sizeof(buf) - 3) {
			__log_snprintf(buf + len,
				sizeof(buf) - len, ": %s", strerror(log_errno));
		}
	}
	__sl_log(file, func, line, severity, buf);
}



void
	sl_log_err(const char* file, const char* func, 
	int line, int v, const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	__log_helper(file, func, line, SL_LOG_ERR, errno, fmt, ap);
	va_end(ap);
	exit(v);
}

void
	sl_log_warn(const char* file, const char* func, 
	int line, const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	__log_helper(file, func, line, SL_LOG_WARN, errno, fmt, ap);
	va_end(ap);
}

void
	sl_log_errx(const char* file, const char* func, 
	int line, int v, const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	__log_helper(file, func, line, SL_LOG_ERR, -1, fmt, ap);
	va_end(ap);
	exit(v);
}

void
	sl_log_warnx(const char* file, const char* func, 
	int line, const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	__log_helper(file, func, line, SL_LOG_WARN, -1, fmt, ap);
	va_end(ap);
}

void
	sl_log_msgx(const char* file, const char* func, 
	int line, const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	__log_helper(file, func, line, SL_LOG_MSG, -1, fmt, ap);
	va_end(ap);
}

void
	sl_log_debugx(const char* file, const char* func, 
	int line, const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	__log_helper(file, func, line, SL_LOG_DEBUG, -1, fmt, ap);
	va_end(ap);
}