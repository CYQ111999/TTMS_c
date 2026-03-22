//zyp
//Studio.c
#include "Common.h"
#include "List.h"
#include "Utils.h"
#include "Studio.h"
#include "Seat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define _CRT_SECURE_NO_WARNINGS
//添加一个新演出厅数据
int Studio_Srv_Add(studio_t* data)
{
    // 数据验证
    if (data == NULL) {
        return 0;// 传入空指针，返回失败
    }
    if (data->name[0] == '\0') {
        return 0;// 演出厅名称为空，返回失败
    }
    if (data->rowsCount <= 0 || data->colsCount <= 0) {
        return 0;// 行数或列数无效，返回失败
    }
    if (data->rowsCount > 1000 || data->colsCount > 1000) {
        return 0;// 行数或列数超出范围，返回失败
    }
    // 计算座位总数
    data->seatsCount = data->rowsCount * data->colsCount;
    // 调用持久化层函数
    return Studio_Perst_Insert(data);
}

//用于修改一个演出厅数据
int Studio_Srv_Modify(const studio_t* data)
{
    // 输入参数验证
    if (data == NULL) {
        printf("错误：传入的数据指针为空！\n");
        return 0;  // 参数无效，返回失败
    }
    // 验证演出厅ID
    if (data->id <= 0) {
        printf("错误：演出厅ID（%d）无效！\n", data->id);
        return 0;  // 演出厅ID无效
    }
    // 验证演出厅名称
    if (data->name[0] == '\0') {
        printf("错误：演出厅名称不能为空！\n");
        return 0;  // 演出厅名称为空
    }
    // 验证名称长度（不超过30个字符）
    if (strlen(data->name) >= 30) {
        printf("错误：演出厅名称过长！\n");
        return 0;  // 名称长度超出限制
    }
    // 验证行数和列数为正数
    if (data->rowsCount <= 0) {
        printf("错误：行数（%d）必须大于0！\n", data->rowsCount);
        return 0;  // 行数无效
    }
    if (data->colsCount <= 0) {
        printf("错误：列数（%d）必须大于0！\n", data->colsCount);
        return 0;  // 列数无效
    }
    // 验证行数和列数在合理范围内
    const int MAX_ROWS = 1000;
    const int MAX_COLS = 1000;
    if (data->rowsCount > MAX_ROWS) {
        printf("错误：行数（%d）超出最大限制（%d）！\n", data->rowsCount, MAX_ROWS);
        return 0;  // 行数超出范围
    }
    if (data->colsCount > MAX_COLS) {
        printf("错误：列数（%d）超出最大限制（%d）！\n", data->colsCount, MAX_COLS);
        return 0;  // 列数超出范围
    }
    // 验证座位总数与行列数的一致性
    int expectedSeatsCount = data->rowsCount * data->colsCount;
    if (data->seatsCount != expectedSeatsCount) {
        printf("警告：座位总数（%d）与行数×列数（%d）不一致！\n",
            data->seatsCount, expectedSeatsCount);
        printf("注意：座位总数将自动修正为 %d\n", expectedSeatsCount);
        // 创建一个临时副本以便修正座位总数
        studio_t correctedData = *data;
        correctedData.seatsCount = expectedSeatsCount;
        // 调用持久化层函数 Studio_Perst_Update() 在文件中修改演出厅
        int result = Studio_Perst_Update(&correctedData);
        // 返回持久化层的执行结果
        if (result) {
            printf("演出厅（ID: %d）修改成功！\n", data->id);
        }
        else {
            printf("演出厅（ID: %d）修改失败！\n", data->id);
        }
        return result;
    }

    // 调用持久化层函数 Studio_Perst_Update() 在文件中修改演出厅
    int result = Studio_Perst_Update(data);

    // 根据结果输出相应信息
    if (result) {
        printf("演出厅（ID: %d）修改成功！\n", data->id);
    }
    else {
        printf("演出厅（ID: %d）修改失败！\n", data->id);
    }

    // 返回持久化层的执行结果
    return result;
}

//用于删除一个演出厅的数据
int Studio_Srv_DeleteByID(int id)
{
    // 参数验证
    if (id <= 0) {
        return 0;  // 演出厅ID无效
    }
    // 调用持久化层函数 Studio_Perst_RemoveByID() 在文件中删除演出厅
    int result = Studio_Perst_RemoveByID(id);

    // 返回持久化层的执行结果
    return result;
}

//根据演出厅ID获取一个演出厅的数据
int Studio_Srv_FetchByID(int ID, studio_t* buf)
{
    // 参数验证
    if (ID <= 0) {
        return 0;  // 演出厅ID无效，返回失败
    }

    if (buf == NULL) {
        return 0;  // 缓冲区指针为空，返回失败
    }

    // 调用持久化层函数从文件中获取演出厅数据
    int result = Studio_Perst_SelectByID(ID, buf);

    // 返回持久化层的执行结果
    return result;
}

//获取所有演出厅的数据，形成以 list 为头指针的演出厅链表
int Studio_Srv_FetchAll(studio_list_t* list)
{
    // 参数验证
    if (list == NULL) {
        return 0;  // 链表指针为空，返回失败
    }

    // 调用持久化层函数从文件中获取所有演出厅数据
    int result = Studio_Perst_SelectAll(list);

    // 返回持久化层的执行结果
    return result;
}

//根据演出厅ID和链表头指针获取该链表上相应演出厅的数据
studio_list_node_t* Studio_Srv_FindByID(studio_list_t list, int ID)
{
    studio_list_node_t* ptr = NULL;  // 遍历指针
    //  若演出厅链表头指针 list 不空，则指针 ptr 指向链表第一个结点
    if (list != NULL) {
        ptr = list;  // 假设 list 指向第一个有效节点
        // 处理循环链表的情况
        studio_list_node_t* start_node = list;  // 记录起始节点
        do {
            // b若 ptr == list 时，转 e（对于循环链表）
            // 对于非循环链表，ptr 为 NULL 时结束

            // c 若 ptr->data.id == ID，则返回 ptr
            if (ptr->data.id == ID) {
                return ptr;
            }

            // d 否则，链表指针向后移动
            // 通过 node.next 获取下一个节点，并转换为 studio_list_node_t* 类型
            ptr = (studio_list_node_t*)(ptr->node.next);

            // 对于循环链表，当回到起始节点时结束循环
            if (ptr == start_node) {
                break;  // 转 e)
            }
        } while (ptr != NULL);  // 对于非循环链表，当 ptr 为 NULL 时结束

    }

    // e) 返回 NULL
    return NULL;
}