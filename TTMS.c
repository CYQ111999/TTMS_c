// main.c
//cyq
#include "Main_Menu.h"
#include "Account.h"
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
int main() {
    // 初始化系统
    Account_Srv_InitSys();
    // 用户登录
    if (SysLogin()) {
        // 进入主菜单，主菜单返回0表示退出系统
        int result = Main_Menu();
        printf("程序退出，返回值: %d\n", result);
    }
    else {
        printf("登录失败，程序退出。\n");
    }
    return 0;
}