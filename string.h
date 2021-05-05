//
// Created by liwei1 on 2021/5/5.
//

#ifndef WRITEOS_STRING_H
#define WRITEOS_STRING_H

#include "global.h"

// 计算字符串长度
int strLen(char *str);
// 比较两个字符串是否相等
bool strEqual(char *lhs, char *rhs);

// 在字符串中寻找某个字符的位置，若不存在，则返回 -1
int findIndex(const char *str, char dst);

// 比较前面 n 个字符是否相同
int strNEqual(char *lhs, char *rhs, int n);

#endif //WRITEOS_STRING_H
