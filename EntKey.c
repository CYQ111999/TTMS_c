//cyq写
//主键服务接口实现文件，提供获取新主键的接口
#include "EntityKey.h"
#include "EntityKey_Persist.h"
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
long EntKey_Srv_GetNewKeys(const char entyName[], int count) {
    if (count < 1) {
        printf("数量不对\n");
        return 0;
    }
    long id = EntKey_Perst_GetNewKeys(entyName, count);
    if (id > 0) {
        printf("OK: %s -> %ld\n", entyName, id);
    }
    else {
        printf("拿主键失败\n");
    }
    return id;
}