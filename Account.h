// Account.h
#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include "Common.h"  // 包含Common.h，使用其中定义的类型
#include "List.h"

// 删除这里的重复定义，因为Common.h中已经定义了：
// 1. 删除 account_type_t 枚举定义
// 2. 删除 account_t 结构体定义
// 3. 删除 account_list_node_t 和 account_list_t 定义

// 只保留全局变量声明
extern account_t gl_CurUser;  // Common.h中已经声明，这里不需要重复声明
extern char* filename;

// 保留账户链表声明
extern account_list_t account_list;  // 注意：这里使用的是Common.h中定义的account_list_t

// 函数声明保持不变
void Account_Srv_InitSys(void);
int Account_Srv_Verify(const char* usrName, const char* pwd);
int Account_Srv_GetNextId(void);
int Account_Srv_LoadAll(void);
account_list_t Account_Srv_FindByUsrName(const char* usrName);
account_t* Account_Srv_FindById(int id);
int Account_Srv_Add(const account_t* data);
int Account_Srv_Modify(const account_t* data);
int Account_Srv_Del(int id);

// UI函数声明
void Account_UI_MgtEntry(void);
int SysLogin(void);
void Account_UI_Add(void* pg_ptr);
void Account_UI_Modify(void* pg_ptr);
void Account_UI_Delete(void* pg_ptr);
void Account_UI_Search(void* pg_ptr);
void Account_UI_FetchAll(void);

// 辅助函数
const char* getAccountTypeString(account_type_t type);

#endif