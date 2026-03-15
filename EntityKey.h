#pragma once
//cyq写
//主键服务业务逻辑层头文件
#ifndef ENTITYKEY_H
#define ENTITYKEY_H

//函数：获取新主键值
long EntKey_Srv_GetNewKeys(const char entyName[], int count);
#endif