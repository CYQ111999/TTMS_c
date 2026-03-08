//cyq写
// 实体主键持久化模块

#pragma once
#ifndef ENTITYKEY_PERSIST_H
#define ENTITYKEY_PERSIST_H

#include "Common.h"

// 主键服务函数声明
long EntKey_Perst_GetNewKeys(const char* entyName, int count);

#endif /* ENTITYKEY_PERSIST_H */