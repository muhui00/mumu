/*****************************************
 * Copyright (C) 2022 * Ltd. All rights reserved.
 *
 * File name   : macro.h
 * Author      : muhui
 * Created date: 2022-11-13 15:15:27
 * Description : 常用宏的封装
 *
 *******************************************/

#ifndef __MACRO_H__
#define __MACRO_H__

#include <assert.h>
#include <string.h>
#include "util.h"
#include "log.h"

#define MUHUI_ASSERT(x) \
    if(!(x)) { \
        MUHUI_LOG_ERROR(MUHUI_LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace\n" \
            << muhui::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define MUHUI_ASSERT2(x, w) \
    if(!(x)) { \
        MUHUI_LOG_ERROR(MUHUI_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace\n" \
            << muhui::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#endif //__MACRO_H__

