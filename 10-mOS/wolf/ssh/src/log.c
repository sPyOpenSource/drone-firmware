/* log.c
 *
 * Copyright (C) 2014-2016 wolfSSL Inc.
 *
 * This file is part of wolfSSH.
 *
 * wolfSSH is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfSSH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with wolfSSH.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * The log module contains the interface to the logging function. When
 * debugging is enabled and turned on, the logger will output to STDOUT.
 * A custom logging callback may be installed.
 */


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolf/ssh/headers/ssh.h>
#include <wolf/ssh/headers/log.h>
#include <wolf/ssh/headers/error.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#ifndef NO_TIMESTAMP
    //#include <time.h>
#endif

//#include "esp_log.h"


#ifndef WOLFSSH_DEFAULT_LOG_WIDTH
    #define WOLFSSH_DEFAULT_LOG_WIDTH 120
#endif


static const char *TAG = "WOLFSSH";
 

static void DefaultLoggingCb(enum wolfSSH_LogLevel, const char *const);

static wolfSSH_LoggingCb logFunction = DefaultLoggingCb;
static enum wolfSSH_LogLevel logLevel = WS_LOG_USER; // esp32: max
#ifdef DEBUG_WOLFSSH
    static int logEnable = 0;
#endif


/* turn debugging on if supported */
void wolfSSH_Debugging_ON(void)
{
#ifdef DEBUG_WOLFSSH
    logEnable = 1;
#endif
}


/* turn debugging off */
void wolfSSH_Debugging_OFF(void)
{
#ifdef DEBUG_WOLFSSH
    logEnable = 0;
#endif
}


/* set logging callback function */
void wolfSSH_SetLoggingCb(wolfSSH_LoggingCb logF)
{
    if (logF)
        logFunction = logF;
}


int wolfSSH_LogEnabled(void)
{
#ifdef DEBUG_WOLFSSH
    return logEnable;
#else
    return 0;
#endif
}


/* log level string */
static const char* GetLogStr(enum wolfSSH_LogLevel level)
{
    switch (level) {
        case WS_LOG_INFO:
            return "INFO";

        case WS_LOG_WARN:
            return "WARNING";

        case WS_LOG_ERROR:
            return "ERROR";

        case WS_LOG_DEBUG:
            return "DEBUG";

        case WS_LOG_USER:
            return "USER";

        default:
            return "UNKNOWN";
    }
}


void DefaultLoggingCb(enum wolfSSH_LogLevel level, const char *const msgStr)
{
    char    timeStr[80];
    timeStr[0] = '\0';
#ifndef NO_TIMESTAMP
    {
        time_t  current;
        struct  tm local;

        current = time(NULL);
        if (WLOCALTIME(&current, &local)) {
            /* make pretty */
            strftime(timeStr, sizeof(timeStr), "%b %d %T %Y: ", &local);
        }
    }
#endif /* NO_TIMESTAMP */
    //fprintf(stdout, "%s[%s] %s\n", timeStr, GetLogStr(level), msgStr);
	//ESP_LOGI(TAG, "%s[%s] %s\n", timeStr, GetLogStr(level), msgStr);
}


/* our default logger */
void wolfSSH_Log(enum wolfSSH_LogLevel level, const char *const fmt, ...)
{
    va_list vlist;
    char    msgStr[WOLFSSH_DEFAULT_LOG_WIDTH];

    if (level < logLevel)
        return;   /* don't need to output */

    /* format msg */
    va_start(vlist, fmt);
    WVSNPRINTF(msgStr, sizeof(msgStr), fmt, vlist);
    va_end(vlist);

    if (logFunction)
        logFunction(level, msgStr);
}
