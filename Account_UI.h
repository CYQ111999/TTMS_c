#ifndef ACCOUNT_UI_H
#define ACCOUNT_UI_H

#include "Common.h"

#ifdef __cplusplus
extern "C" {	
#endif

	// 账户管理UI函数声明
	void Account_UI_MgtEntry(void);
	void Account_UI_FetchAll(void);
	void Account_UI_Add(void* pg);
	void Account_UI_Modify(void* pg);
	void Account_UI_Delete(void* pg);
	void Account_UI_Search(void* pg);
	int SysLogin(void);
	int Account_UI_Login(void);

#ifdef __cplusplus
}
#endif

#endif /* ACCOUNT_UI_H */