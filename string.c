//
// Created by liwei1 on 2021/5/5.
//

#include "string.h"

// 计算字符串长度
int strLen(char *str) {
    int i;
    for (i = 0;; ++i) {
        if (str[i] == '\0') {
            break;
        }
    }
    return i;
}

bool strEqual(char *lhs, char *rhs) {
    if (lhs == rhs) {
        return true;
    }
    int lhsLen, rhsLen;
    lhsLen = strLen(lhs);
    rhsLen = strLen(rhs);
    if (lhsLen != rhsLen) {
        return false;
    }
    int i;
    for (i = 0; i < lhsLen; ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}