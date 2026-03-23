#include "Account.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#define _CRT_SECURE_NO_WARNINGS

// 修改这里：只声明，不定义全局变量
extern account_t gl_CurUser;  // 在 Service/Account.c 中定义
extern account_list_t account_list;  // 在 Service/Account.c 中定义

// 清屏函数
#ifdef _WIN32
#define clear_screen() system("cls")
#else
#define clear_screen() system("clear")
#endif

// 等待用户按键
void press_any_key() {
    printf("\n按任意键继续...");
    _getch();
}

// 获取用户类型字符串
static const char* getAccountTypeString(account_type_t type) {
    switch (type) {
    case USR_CLERK: return "销售员";
    case USR_MANG:  return "经理";
    case USR_ADMIN: return "管理员";
    case USR_ANOMY: return "匿名用户";
    default:        return "未知";
    }
}

// 账户管理主菜单入口函数
void Account_UI_MgtEntry(void) {
    int choice = 0;

    do {
        clear_screen();
        printf("\n=================== 账户管理系统 ===================\n");
        printf("当前用户: %s (", gl_CurUser.username);

        switch (gl_CurUser.type) {
        case USR_CLERK: printf("销售员"); break;
        case USR_MANG:  printf("经理"); break;
        case USR_ADMIN: printf("管理员"); break;
        case USR_ANOMY: printf("匿名用户"); break;
        default:        printf("未知"); break;
        }
        printf(")\n");
        printf("===============================================\n");
        printf("1. 浏览所有账户\n");
        printf("2. 添加账户\n");
        printf("3. 修改账户\n");
        printf("4. 删除账户\n");
        printf("5. 查找账户\n");
        printf("0. 返回上级菜单\n");
        printf("===============================================\n");
        printf("请选择功能: ");

        if (scanf("%d", &choice) != 1) {
            printf("输入错误！\n");
            getchar(); getchar();
            continue;
        }
        getchar();

        switch (choice) {
        case 1:
            Account_UI_FetchAll();
            break;
        case 2:
            Account_UI_Add(NULL);
            break;
        case 3:
            Account_UI_Modify(NULL);
            break;
        case 4:
            Account_UI_Delete(NULL);
            break;
        case 5:
            Account_UI_Search(NULL);
            break;
        case 0:
            printf("返回上级菜单...\n");
            break;
        default:
            printf("无效选择！\n");
            _getch();
            break;
        }
    } while (choice != 0);
}

// 显示当前页的用户信息
void display_current_page(paging_t* pg) {
    if (!account_list || pg->totalRecords == 0) {
        printf("\n当前没有用户数据！\n");
        return;
    }

    printf("\n═══════════════════════════════════════════════════════\n");
    printf("                      用户列表\n");
    printf("═══════════════════════════════════════════════════════\n");

    int i = 0;
    account_list_t pos = NULL;

    printf("序号  ID   用户名       密码       用户类型\n");
    printf("----  ----  ----------  ----------  --------------\n");

    // 修改这里：使用正确的分页遍历宏
    Paging_ViewPage_ForEach(account_list, *pg, account_node_t, pos, i) {
        if (pos) {
            const char* type_str = getAccountTypeString(pos->data.type);

            printf("%-4d  %-4d  %-10s  %-10s  %s\n",
                i + 1,
                pos->data.id,
                pos->data.username,
                pos->data.password,
                type_str);
        }
    }

    printf("═══════════════════════════════════════════════════════\n");
}

// 显示分页信息
void display_page_info(paging_t* pg) {
    int current_page = Pageing_CurPage(*pg);
    int total_pages = Pageing_TotalPages(*pg);

    printf("当前第 %d/%d 页 | 总记录数: %d | 每页显示: %d 条\n",
        current_page, total_pages, pg->totalRecords, pg->pageSize);
}

// 重新计算总记录数的辅助函数
void recalc_total_records(paging_t* pg) {
    if (!account_list) {
        pg->totalRecords = 0;
        return;
    }

    int count = 0;
    account_list_t cur = NULL;
    List_ForEach(account_list, cur) {
        count++;
    }
    pg->totalRecords = count;
}

// 新增用户功能
void Account_UI_Add(void* pg_ptr) {
    paging_t* pg = (paging_t*)pg_ptr;
    clear_screen();
    printf("═══════════════════════════════════════════════════════\n");
    printf("                      新增用户\n");
    printf("═══════════════════════════════════════════════════════\n");

    // 获取下一个可用的ID
    int next_id = Account_Srv_GetNextId();
    printf("新用户ID将自动分配为: %d\n", next_id);
    printf("═══════════════════════════════════════════════════════\n");

    account_t new_user;
    memset(&new_user, 0, sizeof(account_t));
    new_user.id = next_id;

    // 输入用户名
    int valid_username = 0;
    while (!valid_username) {
        printf("请输入用户名 (3-20个字符, 不能包含空格): ");
        fgets(new_user.username, 30, stdin);
        new_user.username[strcspn(new_user.username, "\n")] = '\0';  // 移除换行符

        // 验证用户名
        if (strlen(new_user.username) < 3) {
            printf("错误：用户名太短，至少需要3个字符！\n");
        }
        else if (strlen(new_user.username) > 20) {
            printf("错误：用户名太长，最多20个字符！\n");
        }
        else if (strchr(new_user.username, ' ') != NULL) {
            printf("错误：用户名不能包含空格！\n");
        }
        else {
            valid_username = 1;
        }
    }

    // 输入密码
    int valid_password = 0;
    while (!valid_password) {
        printf("请输入密码 (6-20个字符): ");
        fgets(new_user.password, 30, stdin);
        new_user.password[strcspn(new_user.password, "\n")] = '\0';  // 移除换行符

        if (strlen(new_user.password) < 6) {
            printf("错误：密码太短，至少需要6个字符！\n");
        }
        else if (strlen(new_user.password) > 20) {
            printf("错误：密码太长，最多20个字符！\n");
        }
        else {
            valid_password = 1;
        }
    }

    // 输入用户类型
    int valid_type = 0;
    while (!valid_type) {
        printf("\n请选择用户类型：\n");
        printf("  1. 销售员 (USR_CLERK)\n");
        printf("  2. 经理 (USR_MANG)\n");
        printf("  3. 管理员 (USR_ADMIN)\n");
        printf("  4. 匿名用户 (USR_ANOMY)\n");
        printf("请输入类型编号 (1-4): ");

        int type_choice;
        if (scanf("%d", &type_choice) != 1) {
            printf("错误：请输入有效的数字！\n");
            while (getchar() != '\n');  // 清空输入缓冲区
            continue;
        }
        getchar();  // 清空换行符

        switch (type_choice) {
        case 1:
            new_user.type = USR_CLERK;
            valid_type = 1;
            break;
        case 2:
            new_user.type = USR_MANG;
            valid_type = 1;
            break;
        case 3:
            new_user.type = USR_ADMIN;
            valid_type = 1;
            break;
        case 4:
            new_user.type = USR_ANOMY;
            valid_type = 1;
            break;
        default:
            printf("错误：无效的类型编号，请重新选择！\n");
            break;
        }
    }

    // 确认信息
    printf("\n═══════════════════════════════════════════════════════\n");
    printf("请确认用户信息：\n");
    printf("  ID: %d\n", new_user.id);
    printf("  用户名: %s\n", new_user.username);
    printf("  密码: %s\n", new_user.password);
    printf("  用户类型: ");

    switch (new_user.type) {
    case USR_ANOMY: printf("匿名用户\n"); break;
    case USR_CLERK: printf("销售员\n"); break;
    case USR_MANG:  printf("经理\n"); break;
    case USR_ADMIN: printf("管理员\n"); break;
    default:        printf("未知\n"); break;
    }

    printf("═══════════════════════════════════════════════════════\n");
    printf("确认添加此用户吗？(y/n): ");

    char confirm;
    scanf("%c", &confirm);
    getchar();  // 清空换行符

    if (confirm == 'y' || confirm == 'Y') {
        // 调用数据层函数添加用户
        int result = Account_Srv_Add(&new_user);

        if (result) {
            printf("\n用户添加成功！\n");

            // 重新加载数据
            Account_Srv_LoadAll();

            // 重新计算总记录数
            if (pg != NULL) {
                recalc_total_records(pg);

                // 定位到最后一页
                Paging_Locate_LastPage(account_list, *pg, account_node_t);

                int total_pages = Pageing_TotalPages(*pg);
                printf("已自动跳转到最后一页查看新增结果。\n");
                printf("新用户位于第 %d/%d 页\n", total_pages, total_pages);
            }
        }
        else {
            printf("\n用户添加失败！可能是用户名已存在或其他错误。\n");
        }
    }
    else {
        printf("\n已取消添加用户。\n");
    }

    press_any_key();
}

// 修改用户功能
void Account_UI_Modify(void* pg_ptr) {
    paging_t* pg = (paging_t*)pg_ptr;
    clear_screen();
    printf("═══════════════════════════════════════════════════════\n");
    printf("                      修改用户\n");
    printf("═══════════════════════════════════════════════════════\n");

    int id_to_modify;
    printf("请输入要修改的用户ID: ");
    if (scanf("%d", &id_to_modify) != 1) {
        printf("无效的用户ID！\n");
        getchar();
        press_any_key();
        return;
    }
    getchar();  // 清空换行符

    // 查找用户
    account_t* user_to_modify = Account_Srv_FindById(id_to_modify);
    if (!user_to_modify) {
        printf("未找到ID为 %d 的用户！\n", id_to_modify);
        press_any_key();
        return;
    }

    // 显示当前用户信息
    printf("\n当前用户信息：\n");
    printf("  ID: %d\n", user_to_modify->id);
    printf("  用户名: %s\n", user_to_modify->username);
    printf("  密码: %s\n", user_to_modify->password);
    printf("  用户类型: %s\n", getAccountTypeString(user_to_modify->type));
    printf("═══════════════════════════════════════════════════════\n");

    // 创建修改副本
    account_t modified_user = *user_to_modify;

    int choice = 0;
    int modified = 0;

    do {
        printf("\n请选择要修改的项目：\n");
        printf("1. 修改用户名\n");
        printf("2. 修改密码\n");
        printf("3. 修改用户类型\n");
        printf("4. 查看当前修改\n");
        printf("5. 保存修改\n");
        printf("6. 取消修改\n");
        printf("请选择: ");

        if (scanf("%d", &choice) != 1) {
            printf("无效的选择！\n");
            getchar(); getchar();
            continue;
        }
        getchar();  // 清空换行符

        switch (choice) {
        case 1:  // 修改用户名
        {
            int valid_username = 0;
            while (!valid_username) {
                printf("请输入新用户名 (3-20个字符, 不能包含空格): ");
                fgets(modified_user.username, 30, stdin);
                modified_user.username[strcspn(modified_user.username, "\n")] = '\0';

                if (strlen(modified_user.username) < 3) {
                    printf("错误：用户名太短，至少需要3个字符！\n");
                }
                else if (strlen(modified_user.username) > 20) {
                    printf("错误：用户名太长，最多20个字符！\n");
                }
                else if (strchr(modified_user.username, ' ') != NULL) {
                    printf("错误：用户名不能包含空格！\n");
                }
                else {
                    // 检查用户名是否与其他用户重复（除了自己）
                    int is_duplicate = 0;
                    account_list_t cur = NULL;
                    List_ForEach(account_list, cur) {
                        if (cur->data.id != modified_user.id &&
                            strcmp(cur->data.username, modified_user.username) == 0) {
                            is_duplicate = 1;
                            break;
                        }
                    }

                    if (is_duplicate) {
                        printf("错误：用户名已存在！\n");
                    }
                    else {
                        valid_username = 1;
                        modified = 1;
                    }
                }
            }
            printf("用户名已修改为: %s\n", modified_user.username);
            break;
        }

        case 2:  // 修改密码
        {
            int valid_password = 0;
            while (!valid_password) {
                printf("请输入新密码 (6-20个字符): ");
                fgets(modified_user.password, 30, stdin);
                modified_user.password[strcspn(modified_user.password, "\n")] = '\0';

                if (strlen(modified_user.password) < 6) {
                    printf("错误：密码太短，至少需要6个字符！\n");
                }
                else if (strlen(modified_user.password) > 20) {
                    printf("错误：密码太长，最多20个字符！\n");
                }
                else {
                    valid_password = 1;
                    modified = 1;
                }
            }
            printf("密码已修改\n");
            break;
        }

        case 3:  // 修改用户类型
        {
            int valid_type = 0;
            while (!valid_type) {
                printf("\n请选择新用户类型：\n");
                printf("  1. 销售员 (USR_CLERK)\n");
                printf("  2. 经理 (USR_MANG)\n");
                printf("  3. 管理员 (USR_ADMIN)\n");
                printf("  4. 匿名用户 (USR_ANOMY)\n");
                printf("请输入类型编号 (1-4): ");

                int type_choice;
                if (scanf("%d", &type_choice) != 1) {
                    printf("错误：请输入有效的数字！\n");
                    while (getchar() != '\n');
                    continue;
                }
                getchar();

                switch (type_choice) {
                case 1:
                    modified_user.type = USR_CLERK;
                    valid_type = 1;
                    modified = 1;
                    break;
                case 2:
                    modified_user.type = USR_MANG;
                    valid_type = 1;
                    modified = 1;
                    break;
                case 3:
                    modified_user.type = USR_ADMIN;
                    valid_type = 1;
                    modified = 1;
                    break;
                case 4:
                    modified_user.type = USR_ANOMY;
                    valid_type = 1;
                    modified = 1;
                    break;
                default:
                    printf("错误：无效的类型编号，请重新选择！\n");
                    break;
                }
            }
            printf("用户类型已修改为: %s\n", getAccountTypeString(modified_user.type));
            break;
        }

        case 4:  // 查看当前修改
            printf("\n当前修改信息：\n");
            printf("  ID: %d\n", modified_user.id);
            printf("  用户名: %s\n", modified_user.username);
            printf("  密码: %s\n", modified_user.password);
            printf("  用户类型: %s\n", getAccountTypeString(modified_user.type));

            if (modified) {
                printf("状态: 已修改，等待保存\n");
            }
            else {
                printf("状态: 未修改\n");
            }
            break;

        case 5:  // 保存修改
            if (!modified) {
                printf("没有需要保存的修改！\n");
                break;
            }

            printf("\n保存修改吗？(y/n): ");
            char confirm_save;
            scanf("%c", &confirm_save);
            getchar();

            if (confirm_save == 'y' || confirm_save == 'Y') {
                int result = Account_Srv_Modify(&modified_user);
                if (result) {
                    printf("用户信息已成功保存！\n");

                    // 如果是当前登录用户修改了自己的信息，更新全局变量
                    if (gl_CurUser.id == modified_user.id) {
                        gl_CurUser = modified_user;
                    }

                    // 重新加载数据
                    Account_Srv_LoadAll();
                    if (pg != NULL) {
                        recalc_total_records(pg);
                    }
                }
                else {
                    printf("保存失败！可能是用户名已存在或其他错误。\n");
                }
            }
            else {
                printf("已取消保存。\n");
            }
            return;  // 退出修改界面

        case 6:  // 取消修改
            printf("取消修改，不保存任何更改。\n");
            return;  // 退出修改界面

        default:
            printf("无效的选择！\n");
            break;
        }

        printf("\n按任意键继续...");
        _getch();
    } while (1);
}

// 删除用户功能
void Account_UI_Delete(void* pg_ptr) {
    paging_t* pg = (paging_t*)pg_ptr;
    clear_screen();
    printf("═══════════════════════════════════════════════════════\n");
    printf("                      删除用户\n");
    printf("═══════════════════════════════════════════════════════\n");

    int id_to_delete;
    printf("请输入要删除的用户ID: ");
    if (scanf("%d", &id_to_delete) != 1) {
        printf("无效的用户ID！\n");
        getchar();
        press_any_key();
        return;
    }
    getchar();

    // 查找用户
    account_t* user_to_delete = Account_Srv_FindById(id_to_delete);
    if (!user_to_delete) {
        printf("未找到ID为 %d 的用户！\n", id_to_delete);
        press_any_key();
        return;
    }

    // 显示用户信息
    printf("\n要删除的用户信息：\n");
    printf("  ID: %d\n", user_to_delete->id);
    printf("  用户名: %s\n", user_to_delete->username);
    printf("  密码: %s\n", user_to_delete->password);
    printf("  用户类型: %s\n", getAccountTypeString(user_to_delete->type));
    printf("═══════════════════════════════════════════════════════\n");

    // 如果是管理员用户，不能删除
    if (strcmp(user_to_delete->username, "admin") == 0) {
        printf("不能删除管理员账户！\n");
        press_any_key();
        return;
    }

    // 如果是当前登录用户，不能删除
    if (gl_CurUser.id == user_to_delete->id) {
        printf("不能删除当前登录的账户！\n");
        press_any_key();
        return;
    }

    // 确认删除
    printf("确认删除此用户吗？此操作不可恢复！(y/n): ");
    char confirm;
    scanf("%c", &confirm);
    getchar();

    if (confirm == 'y' || confirm == 'Y') {
        int result = Account_Srv_Del(id_to_delete);
        if (result) {
            printf("用户删除成功！\n");

            // 重新加载数据
            Account_Srv_LoadAll();
            if (pg != NULL) {
                recalc_total_records(pg);
            }
        }
        else {
            printf("删除失败！\n");
        }
    }
    else {
        printf("已取消删除。\n");
    }

    press_any_key();
}

// 查找用户功能
void Account_UI_Search(void* pg_ptr) {
    paging_t* pg = (paging_t*)pg_ptr;
    clear_screen();
    printf("═══════════════════════════════════════════════════════\n");
    printf("                      查找用户\n");
    printf("═══════════════════════════════════════════════════════\n");

    char search_username[30];
    printf("请输入要查找的用户名（支持模糊查询）: ");
    fgets(search_username, 30, stdin);
    search_username[strcspn(search_username, "\n")] = '\0';

    if (strlen(search_username) == 0) {
        printf("用户名不能为空！\n");
        press_any_key();
        return;
    }

    // 查找用户
    account_list_t result_list = Account_Srv_FindByUsrName(search_username);
    if (!result_list) {
        printf("未找到包含 '%s' 的用户！\n", search_username);
        press_any_key();
        return;
    }

    // 显示查找结果
    printf("\n═══════════════════════════════════════════════════════\n");
    printf("                查找结果\n");
    printf("═══════════════════════════════════════════════════════\n");

    int count = 0;
    account_list_t cur = NULL;
    List_ForEach(result_list, cur) {
        if (cur) {
            count++;
            printf("\n用户 #%d:\n", count);
            printf("  ID: %d\n", cur->data.id);
            printf("  用户名: %s\n", cur->data.username);
            printf("  密码: %s\n", cur->data.password);
            printf("  用户类型: %s\n", getAccountTypeString(cur->data.type));
        }
    }

    printf("\n═══════════════════════════════════════════════════════\n");
    printf("共找到 %d 个用户\n", count);
    printf("═══════════════════════════════════════════════════════\n");

    // 清理结果链表
    List_Destroy(result_list, account_node_t);

    press_any_key();
}

// 分页浏览主界面
void Account_UI_FetchAll() {
    // 加载数据
    int count = Account_Srv_LoadAll();
    if (count == 0) {
        printf("没有找到用户数据！\n");
        printf("将创建默认管理员账户。\n");
        press_any_key();
    }

    // 初始化分页器
    paging_t pg;
    pg.pageSize = 5;  // 默认每页5条
    pg.totalRecords = 0;
    pg.offset = 0;
    pg.curPos = NULL;

    // 计算总记录数
    recalc_total_records(&pg);

    // 定位到第一页
    Paging_Locate_FirstPage(account_list, pg);

    int choice = 0;
    int exit_browse = 0;

    do {
        clear_screen();

        // 显示分页信息和当前页数据
        display_page_info(&pg);
        display_current_page(&pg);

        // 显示操作菜单（在数据下方）
        printf("\n═══════════════════════════════════════════════════════\n");
        printf("                   用户管理操作\n");
        printf("═══════════════════════════════════════════════════════\n");
        printf("分页操作：\n");
        printf("  1. 首页 2. 末页 3. 上一页 4. 下一页\n");
        printf("  5. 跳转到指定页 6. 修改每页显示条数\n");
        printf("  7. 重新加载数据\n");
        printf("\n用户管理：\n");
        printf("  8. 新增用户 9. 修改用户 10. 删除用户 11. 查找用户\n");
        printf("\n系统操作：\n");
        printf("  0. 退出系统\n");
        printf("═══════════════════════════════════════════════════════\n");
        printf("请输入操作编号: ");

        if (scanf("%d", &choice) != 1) {
            printf("输入错误！\n");
            getchar(); getchar();
            continue;
        }
        getchar();  // 清空缓冲区

        switch (choice) {
        case 1:  // 首页
            Paging_Locate_FirstPage(account_list, pg);
            break;

        case 2:  // 末页
            Paging_Locate_LastPage(account_list, pg, account_node_t);
            break;

        case 3:  // 上一页
            if (!Pageing_IsFirstPage(pg)) {
                Paging_Locate_OffsetPage(account_list, pg, -1, account_node_t);
            }
            else {
                printf("已经是第一页了！\n");
                press_any_key();
            }
            break;

        case 4:  // 下一页
            if (!Pageing_IsLastPage(pg)) {
                Paging_Locate_OffsetPage(account_list, pg, 1, account_node_t);
            }
            else {
                printf("已经是最后一页了！\n");
                press_any_key();
            }
            break;

        case 5:  // 跳转到指定页
        {
            int total_pages = Pageing_TotalPages(pg);
            if (total_pages <= 1) {
                printf("只有1页，无需跳转！\n");
                press_any_key();
                break;
            }

            int target_page;
            printf("请输入要跳转的页码 (1-%d): ", total_pages);
            if (scanf("%d", &target_page) != 1) {
                printf("输入错误！\n");
                getchar();
                break;
            }
            getchar();

            if (target_page < 1) target_page = 1;
            if (target_page > total_pages) target_page = total_pages;

            pg.offset = (target_page - 1) * pg.pageSize;
            List_Paging(account_list, pg, account_node_t);

            printf("已跳转到第 %d 页\n", target_page);
            press_any_key();
        }
        break;

        case 6:  // 修改每页显示条数
        {
            int new_size;
            printf("当前每页显示: %d 条\n", pg.pageSize);
            printf("请输入新的每页显示条数 (1-20): ");
            if (scanf("%d", &new_size) != 1) {
                printf("输入错误！\n");
                getchar();
                break;
            }
            getchar();

            if (new_size < 1) new_size = 1;
            if (new_size > 20) new_size = 20;

            pg.pageSize = new_size;

            // 重新计算总记录数
            recalc_total_records(&pg);

            // 调整当前偏移量
            if (pg.offset >= pg.totalRecords && pg.totalRecords > 0) {
                pg.offset = ((pg.totalRecords - 1) / pg.pageSize) * pg.pageSize;
            }

            // 重新定位
            Paging_Locate_FirstPage(account_list, pg);

            printf("已修改为每页显示 %d 条\n", new_size);
            press_any_key();
        }
        break;

        case 7:  // 重新加载数据
            count = Account_Srv_LoadAll();
            if (count > 0) {
                // 重新计算总记录数
                recalc_total_records(&pg);

                // 重置到第一页
                Paging_Locate_FirstPage(account_list, pg);
                printf("成功重新加载 %d 个用户\n", count);
            }
            else {
                printf("加载数据失败！\n");
            }
            press_any_key();
            break;

        case 8:  // 新增用户
            Account_UI_Add(&pg);
            break;

        case 9:  // 修改用户
            Account_UI_Modify(&pg);
            break;

        case 10: // 删除用户
            Account_UI_Delete(&pg);
            break;

        case 11: // 查找用户
            Account_UI_Search(&pg);
            break;

        case 0:  // 退出系统
            exit_browse = 1;
            if (account_list) {
                List_Destroy(account_list, account_node_t);
            }
            printf("感谢使用，再见！\n");
            exit(0);
            break;

        default:
            printf("无效的选项，请重新选择！\n");
            press_any_key();
            break;
        }

    } while (!exit_browse);
}

// 系统登录UI
int SysLogin() {
    int max_attempts = 3;  // 最大尝试次数
    int attempts = 0;

    while (attempts < max_attempts) {
        clear_screen();

        printf("═══════════════════════════════════════════════════════\n");
        printf("                    用户登录\n");
        printf("═══════════════════════════════════════════════════════\n");

        // 显示剩余尝试次数
        printf("剩余尝试次数: %d/%d\n\n", max_attempts - attempts, max_attempts);

        char username[30];
        char password[30];

        // 输入用户名
        printf("用户名: ");
        fgets(username, 30, stdin);
        username[strcspn(username, "\n")] = '\0';  // 移除换行符

        if (strlen(username) == 0) {
            printf("用户名不能为空！\n");
            printf("按任意键继续...");
            _getch();
            continue;
        }

        // 输入密码（显示为*）
        printf("密码: ");

        int i = 0;
        char ch;
        while (1) {
            ch = _getch();
            if (ch == '\r' || ch == '\n') {
                break;
            }
            else if (ch == '\b') {
                if (i > 0) {
                    i--;
                    printf("\b \b");
                }
            }
            else if (i < 29 && isprint(ch)) {
                password[i++] = ch;
                printf("*");
            }
        }
        password[i] = '\0';
        printf("\n");

        if (strlen(password) == 0) {
            printf("密码不能为空！\n");
            printf("按任意键继续...");
            _getch();
            continue;
        }

        // 验证用户
        int result = Account_Srv_Verify(username, password);

        if (result) {
            printf("\n登录成功！\n");

            // 获取当前用户信息
            account_list_t user_list = Account_Srv_FindByUsrName(username);
            if (user_list) {
                // 获取第一个用户
                account_list_t first_user = NULL;
                List_ForEach(user_list, first_user) {
                    if (first_user) {
                        printf("\n═══════════════════════════════════════════════════════\n");
                        printf("                    欢迎回来，%s！\n", username);
                        printf("═══════════════════════════════════════════════════════\n");
                        printf("用户类型: %s\n", getAccountTypeString(first_user->data.type));
                        printf("═══════════════════════════════════════════════════════\n");
                        gl_CurUser = first_user->data;
                        break;
                    }
                }
                List_Destroy(user_list, account_node_t);
            }

            printf("按任意键进入主界面...");
            _getch();
            return 1;  // 登录成功
        }
        else {
            attempts++;
            printf("\n登录失败！用户名或密码错误。\n");

            if (attempts < max_attempts) {
                printf("按任意键重新尝试...");
                _getch();
            }
            else {
                printf("已超过最大尝试次数，系统将退出。\n");
                printf("按任意键退出...");
                _getch();
            }
        }
    }

    return 0;  // 登录失败
}

int Account_UI_Login(void) {
    // 直接调用现有的 SysLogin 函数
    return SysLogin();
}