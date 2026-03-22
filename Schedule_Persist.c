//zyp
/// Persistence / Schedule_Persist.c
#define _CRT_SECURE_NO_WARNINGS
#include "Common.h"
#include "List.h"
#include "Utils.h"
#include "Studio.h"
#include "Seat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
 /* 数据文件名常量 */
#define SCHEDULE_DATA_FILE "Schedule.dat"
#define SCHEDULE_TEMP_FILE "ScheduleTmp.dat"
#define PLAY_DATA_FILE "Play.dat"

//根据剧目ID载入演出计划
int Schedule_Perst_SelectByPlay(schedule_list_t list, int play_id)
{
    int recCount = 0;        // 记录计数器
    FILE* fp = NULL;         // 文件指针
    schedule_t data;         // 临时存储读取的数据
    schedule_list_node_t* newNode = NULL;  // 新节点指针
    schedule_list_node_t* tail = NULL;     // 链表尾指针
    //  定义局部变量recCount，赋初值0，判断演出计划数据文件Schedule.dat是否存在
    // 初始化链表 list (假设调用方已初始化)
    // 这里我们确保list是有效的链表头指针
    // 以只读方式打开演出计划数据文件 Schedule.dat
    fp = fopen(SCHEDULE_DATA_FILE, "rb");
    // 若打开失败,提示失败信息,返回 recCount,结束函数
    if (fp == NULL) {
        printf("打开演出计划数据文件失败！\n");
        return recCount;
    }
    // 查找链表尾节点
    if (list != NULL) {
        schedule_list_node_t* current = list;
        while (current != NULL && ((schedule_list_node_t*)current->node.next) != NULL) {
            current = (schedule_list_node_t*)current->node.next;
        }
        tail = current;
    }
    // 循环读取文件
    while (1) {
        // c) 判断演出计划数据文件是否读到末尾
        if (feof(fp)) {
            break;  // 执行步骤 e)
        }
        // 从演出计划数据文件 Schedule.dat 读出一条记录
        size_t read_count = fread(&data, sizeof(schedule_t), 1, fp);
        // 若失败,执行步骤 c) (继续循环)
        if (read_count != 1) {
            if (feof(fp)) {
                break;
            }
            continue;
        }
        // 判断 data.play_id 是否等于参数 play_id
        if (data.play_id == play_id) {
            // 构造新结点 newNode
            newNode = (schedule_list_node_t*)malloc(sizeof(schedule_list_node_t));
            if (newNode == NULL) {
                printf("内存分配失败！\n");
                fclose(fp);
                return recCount;
            }
            // 将数据复制到新节点
            newNode->data = data;
            newNode->node.prev = NULL;
            newNode->node.next = NULL;
            // 添加到 list 链表尾
            if (list == NULL) {
                // 链表为空，list指向新节点
                // 注意：这里我们无法修改list本身，因为是一级指针
                // 假设调用方已经初始化了list
                printf("错误：链表头指针为空！\n");
                free(newNode);
                fclose(fp);
                return recCount;
            }
            else {
                if (tail == NULL) {
                    // 链表只有一个节点（头节点）
                    list->node.next = (list_node_t*)newNode;
                    newNode->node.prev = (list_node_t*)list;
                }
                else {
                    tail->node.next = (list_node_t*)newNode;
                    newNode->node.prev = (list_node_t*)tail;
                }
                tail = newNode;
            }
            recCount++;
        }
    }
    fclose(fp);
    return recCount;
}

// 存储新演出计划
int Schedule_Perst_Insert(schedule_t* data)
{
    int rtn = 0;        // 返回值
    FILE* fp = NULL;    // 文件指针
    // 参数验证
    if (data == NULL) {
        return 0;
    }
    // 以追加方式打开演出计划数据文件 Schedule.dat
    fp = fopen(SCHEDULE_DATA_FILE, "ab");

    // 若打开失败,提示失败信息,返回 0,结束函数
    if (fp == NULL) {
        printf("打开演出计划数据文件失败！\n");
        return 0;
    }
    // 写入 data 所指的演出计划信息
    if (fwrite(data, sizeof(schedule_t), 1, fp) == 1) {
        rtn = 1;  // 写入成功
    }
    fclose(fp);
    return rtn;
}

//更新演出计划
int Schedule_Perst_Update(const schedule_t* data)
{
    int found = 0;      // 查找标志
    FILE* fp = NULL;    // 文件指针
    schedule_t buf;     // 临时缓冲区
    // 参数验证
    if (data == NULL) {
        return 0;
    }
    //局部变量 found 赋初值 0
    found = 0;
    // 以读写方式打开演出计划数据文件 Schedule.dat
    fp = fopen(SCHEDULE_DATA_FILE, "rb+");
    // 若打开失败,提示失败信息,返回 found,结束函数
    if (fp == NULL) {
        printf("打开演出计划数据文件失败！\n");
        return found;
    }
    // 循环读取文件
    while (1) {
        // b 若文件读写指针已到文件末尾,执行步骤 e
        if (feof(fp)) {
            break;
        }
        // 获取当前文件位置
        long current_pos = ftell(fp);

        // c 读出当前文件读写指针所指的演出计划信息
        size_t read_count = fread(&buf, sizeof(schedule_t), 1, fp);

        // 若读操作失败,执行步骤 b(继续循环)
        if (read_count != 1) {
            if (feof(fp)) {
                break;
            }
            continue;
        }

        // d 判断 buf.id 是否和 data.id 相等
        if (buf.id == data->id) {
            // 回撤读写指针到该记录开始的位置
            fseek(fp, current_pos, SEEK_SET);

            // 将参数 data 所保存的演出计划写入文件覆盖旧数据
            if (fwrite(data, sizeof(schedule_t), 1, fp) == 1) {
                found = 1;  // 更新成功
            }

            break;  // 找到并更新后退出循环
        }
        // 否则,执行步骤 b (继续循环)
    }

    // e 关闭文件,返回 found,结束函数
    fclose(fp);
    return found;
}

//根据ID去除演出计划
int Schedule_Perst_RemByID(int id)
{
    int found = 0;                    // 查找标志
    FILE* fp_src = NULL;              // 源文件指针（临时文件）
    FILE* fp_dst = NULL;              // 目标文件指针（原文件）
    schedule_t buf;                   // 临时缓冲区
    int rename_result = 0;            // 重命名结果
    // a 局部变量 found 赋初值 0
    found = 0;
    // 检查原文件是否存在
    FILE* fp_check = fopen(SCHEDULE_DATA_FILE, "rb");
    if (fp_check == NULL) {
        printf("演出计划数据文件不存在！\n");
        return 0;
    }
    fclose(fp_check);
    // 将演出计划数据文件Schedule.dat改名为ScheduleTmp.dat
    rename_result = rename(SCHEDULE_DATA_FILE, SCHEDULE_TEMP_FILE);
    // 若改名失败，提示失败信息，执行步骤f)
    if (rename_result != 0) {
        printf("重命名演出计划数据文件失败！\n");
        return 0;  // 直接返回0，表示失败
    }
    // b 否则，以只读方式打开ScheduleTmp.dat，只写方式打开Schedule.dat
    fp_src = fopen(SCHEDULE_TEMP_FILE, "rb");
    fp_dst = fopen(SCHEDULE_DATA_FILE, "wb");
    // 若打开失败，提示失败信息，执行步骤f)
    if (fp_src == NULL || fp_dst == NULL) {
        printf("打开文件失败！\n");
        // 关闭已打开的文件
        if (fp_src != NULL) fclose(fp_src);
        if (fp_dst != NULL) fclose(fp_dst);
        // 恢复原文件名
        rename(SCHEDULE_TEMP_FILE, SCHEDULE_DATA_FILE);
        return 0;
    }
    // 循环读取临时文件
    while (1) {
        // 判断ScheduleTmp.dat文件是否读到末尾
        if (feof(fp_src)) {
            break;  // 若是，执行步骤f
        }

        // c 从ScheduleTmp.dat文件读出一条记录到临时变量buf
        size_t read_count = fread(&buf, sizeof(schedule_t), 1, fp_src);
        // 若读操作失败，执行步骤e（继续循环）
        if (read_count != 1) {
            if (feof(fp_src)) {
                break;
            }
            continue;
        }
        // e判断buf.id是否等于参数id
        if (buf.id == id) {
            // 若相等，found置为1
            found = 1;
            // 不写入目标文件，相当于删除
        }
        else {
            // 否则将buf的数据写入Schedule.dat
            fwrite(&buf, sizeof(schedule_t), 1, fp_dst);
        }
        // 跳转至步骤c（继续循环）
    }

    // f关闭文件
    fclose(fp_src);
    fclose(fp_dst);
    // 删除临时文件
    remove(SCHEDULE_TEMP_FILE);
    return found;
}

//从演出计划数据文件Schedule.dat中载入所有演出计划，加载到链表list上
int Schedule_Perst_SelectAll(schedule_list_t list)
{
    int recCount = 0;        // 记录计数器
    FILE* fp = NULL;         // 文件指针
    schedule_t data;         // 临时存储读取的数据
    schedule_list_node_t* newNode = NULL;  // 新节点指针
    schedule_list_node_t* tail = NULL;     // 链表尾指针
    // a定义局部变量recCount，赋初值0
    recCount = 0;
    // 判断演出计划数据文件Schedule.dat是否存在
    fp = fopen(SCHEDULE_DATA_FILE, "rb");
    if (fp == NULL) {
        // 若不存在，返回0，结束函数
        printf("演出计划数据文件不存在！\n");
        return 0;
    }
    // 注意：参数list是一级指针，无法修改调用方传递的指针本身
    if (list == NULL) {
        printf("错误：链表头指针为空！\n");
        fclose(fp);
        return 0;
    }
    // b初始化list为空链表
    // 由于list是一级指针，我们无法将调用方的指针置为NULL
    // 但我们可以清空链表（如果链表不为空的话）
    // 这里我们假设链表已经初始化，只需要设置next和prev指针
    list->node.next = NULL;
    list->node.prev = NULL;
    // 以只读方式打开演出计划数据文件
    // 上面已经打开了文件，这里只是验证
    if (fp == NULL) {
        printf("打开演出计划数据文件失败！\n");
        return 0;
    }
    // 设置尾指针初始为链表头
    tail = list;
    // 循环读取文件
    while (1) {
        // c判断演出计划数据文件是否读到末尾
        if (feof(fp)) {
            // 若是，执行步骤e)
            break;
        }
        // d 从演出计划数据文件读出当前记录
        size_t read_count = fread(&data, sizeof(schedule_t), 1, fp);
        if (read_count != 1) {
            // 可能是文件结束
            if (feof(fp)) {
                break;
            }
            else {
                // 读取失败，跳过这条记录
                continue;
            }
        }
        // 构造结点newNode
        newNode = (schedule_list_node_t*)malloc(sizeof(schedule_list_node_t));
        if (newNode == NULL) {
            printf("内存分配失败！\n");
            fclose(fp);
            return recCount;
        }
        // 初始化新节点
        newNode->data = data;
        newNode->node.next = NULL;
        // 设置prev指针
        if (tail == list && list->node.next == NULL) {
            // 这是第一个数据节点
            newNode->node.prev = (list_node_t*)list;
        }
        else {
            newNode->node.prev = (list_node_t*)tail;
        }
        // 添加在list链表尾
        tail->node.next = (list_node_t*)newNode;
        tail = newNode;
        recCount++;

        // 执行步骤c（继续循环）
    }
    fclose(fp);
    return recCount;
}


//从文件Schedule.dat中载入指定ID的演出计划
int Schedule_Perst_SelectByID(int id, schedule_t* buf)
{
    int found = 0;      /* 查找标志 */
    FILE* fp = NULL;    /* 文件指针 */
    schedule_t data;    /* 临时存储读取的数据 */
    found = 0;
    // 验证参数
    if (id <= 0 || buf == NULL) {
        printf("错误：参数无效！\n");
        return 0;
    }
    // 以只读方式打开Schedule.dat
    fp = fopen(SCHEDULE_DATA_FILE, "rb");
    // 若打开失败，提示失败信息，转去e)
    if (fp == NULL) {
        printf("错误：无法打开演出计划数据文件！\n");
        return 0;
    }
    // 循环读取文件
    while (1) {
        // b 判断Schedule.dat文件是否读到末尾
        if (feof(fp)) {
            // 若是，转去e
            break;
        }
        // c 否则，从Schedule.dat文件读出一条记录保存在局部变量data中
        size_t read_count = fread(&data, sizeof(schedule_t), 1, fp);
        // 若失败，转去b(继续循环)
        if (read_count != 1) {
            // 检查是否文件结束
            if (feof(fp)) {
                break;
            }
            // 读取失败，跳过这条记录
            continue;
        }
        // d否则，判断data.id是否等于参数id
        if (data.id == id) {
            found = 1;
            // 将data的值复制到buf指向的内存
            *buf = data;
            // 找到后跳出循环
            break;
        }
        // 否则，转去执行步骤b (继续循环)
    }
    fclose(fp);
    return found;
}