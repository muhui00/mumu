/*****************************************
 * Copyright (C) 2022 * Ltd. All rights reserved.
 *
 * File name   : util.h
 * Author      : muhui
 * Created date: 2022-11-04 21:38:20
 * Description : 常用工具函数
 *
 *******************************************/

//#pragma once
#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <stdint.h>
#include <vector>
#include <iostream>

namespace muhui
{

/**
 * @brief 返回当前线程的ID
 */
pid_t GetThreadId();

/**
 * @brief 返回当前协程的ID
 */
uint32_t GetFiberId();
/**
 * @brief 获取当前的调用栈
 * @param[out] bt 保存调用栈
 * @param[in] size 最多返回层数
 * @param[in] skip 跳过栈顶的层数
 */
void Backtrace(std::vector<std::string>& bt, int size, int skip = 1);
/**
 * @brief 获取当前栈信息的字符串
 * @param[in] size 栈的最大层数
 * @param[in] skip 跳过栈顶的层数
 * @param[in] prefix 栈信息前输出的内容
 */
std::string BacktraceToString(int size, int skip = 2, const std::string& prefix = "");

}

#endif //__UTIL_H__

