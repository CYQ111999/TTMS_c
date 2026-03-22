#include "Account.h"
#include "Common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
void MaiAccount_UI_MgtEntry() {
	//选择
	int choice;
	//定义变量cdata存放当前系统用户信息，gl_CurUser为全局变量，存放当前系统用户信息
	account_t cdata = gl_CurUser;
	//定义变量cpassword存储当前系统用户密码
	char cpassword[30];
	//用于确认新密码
	char confirmPassword[30];
	//接收业务逻辑层修改操作的结果
	int modifyResult;
	do {
		//显示个人资料维护菜单
		system("cls");
		printf("\n=================== 维护个人资料 ===================\n");
		printf("当前账户: %s (ID: %d)\n", cdata.username, cdata.id);
		printf("用户类型: %s\n", getAccountTypeString(cdata.type));
		printf("----------------------------------------------------\n");
		printf("1.修改登录密码\n");
		printf("0.返回主菜单\n");
		printf("----------------------------------------------------\n");
		printf("请选择操作(0 - 1) : ");
		if (scanf("%d", &choice) != 1) {
			printf("输入无效,请输入数字。\n");
			while (getchar() != '\n');
			printf("按任意键继续..\n");
			getchar();
			continue;
		}
		while (getchar() != '\n');
		switch (choice) {
		case 1://修改密码
			printf("\n--- 修改登录密码 ---\n");
			//检查当前用户是否为管理员
			if (strcmp(cdata.username, "admin") == 0) {
				printf("警告：超级管理员账号的密码修改功能已被禁用。\n");
				printf("“出于系统安全考虑，请通过其他指定流程修改。\n");
				break; // 直接跳出，不执行后续修改密码的步骤
			}
			printf("请输入新密码(不超过30个字符); ");
			scanf("%s", cpassword);
			while (getchar() != '\n');

			printf("请再次输入新密码以确认: ");
			scanf("%s", confirmPassword);
			while (getchar() != '\n');
			if (strcmp(cpassword, confirmPassword) != 0) {
				printf("错误：两次输入的密码不一致，修改取消。\n");
				break;
			}
			if (strlen(cpassword) < 6) {
				printf("错误：密码长度至少需要6位。\n");
				break;
			}

			strcpy(cdata.password, cpassword);
			modifyResult = Account_Srv_Modify(&cdata);

			if (modifyResult) {
				printf("恭喜！密码修改成功。\n");
				strcpy(cpassword, cdata.password);
				printf("请注意,下次登录请使用新密码。\n");
			}
			else {
				printf("抱歉，密码修改失败。\n");
			}
			break;

		case 0:
			printf("正在返回主菜单...\n");
			break;

		default:
			printf("错误:无效的选择,请重新输入。\n");
			break;
		}
		if (choice != 0) {
			printf("按任意键继续..\n");
			getchar();
		}
	} while (choice != 0);
}