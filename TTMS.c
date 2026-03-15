//TTMS项目主文件，项目执行入口

#include <stdio.h>
//cyq写
//完成主函数，项目的入口
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include "Main_Menu.h" 
#include "Account_UI.h"  

int main(void) {
    int running = 1; // 主循环控制标志
    int choice = 0;  // 用户菜单选择
    //用户登录验证循环
    while (1) {
        if (SysLogin() == 1) {
            printf("登录成功，欢迎 %s！\n", gl_CurUser.username);
            break; // 登录成功，跳出循环
        }
        else {
            printf("登录失败，请重试。\n");
            // 可根据情况加入重试次数限制或退出选项
        }
    }
    //主功能菜单循环
    while (running) {
        choice = Main_Menu();
        if (choice == 9) { 
            running = 0; // 结束主循环
            printf("正在注销...\n");
        }
        else if (choice > 0) {
            
        }
    }
    printf("感谢使用剧院票务管理系统，再见！\n");
    return 0;
}