//zyp
//Studio_Persist.c
#include "Common.h"
#include "List.h"
#include "Utils.h"
#include "Studio.h"
#include "Seat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define STUDIO_DATA_FILE "STUDIO_DATA_FILE"
#define STUDIO_DATA_TEMP_FILE "STUDIO_DATA_TEMP_FILE"
#define _CRT_SECURE_NO_WARNINGS
//用于向文件中添加一个新演出厅数据
int Studio_Perst_Insert(studio_t* data)
{
    int rtn = 0;                // 返回值标志
    FILE* fp = NULL;            // 文件指针
    int key = 0;                // 主键值
    // 调用 EntKey_Perst_GetNewKeys() 函数为新演出厅分配获取的主键key
    key = EntKey_Perst_GetNewKeys();
    // 若主键分配失败，则直接返回0
    if (key <= 0) {
        printf("主键分配失败！\n");
        return 0;
    }
    // 若主键分配成功，则将传递过来的参数一一演出厅数据结点的ID设置为key
    data->id = key;
    // 添加演出厅的标志rtn置0（已在声明时完成）
    // 以"ab"方式打开STUDIO_DATA_FILE文件
    fp = fopen(STUDIO_DATA_FILE, "ab");
    //  若打开文件失败，则输出打开文件失败，返回0
    if (fp == NULL) {
        printf("打开文件失败！\n");
        return 0;
    }
    // 若打开文件成功，则将一条演出厅记录写入文件，rtn=1
    if (fwrite(data, sizeof(studio_t), 1, fp) == 1) {
        rtn = 1;
    }
    // 关闭文件
    if (fp != NULL) {
        fclose(fp);
    }
    // 返回rtn
    return rtn;
}

//用于在文件中更新一个演出厅数据
int Studio_Perst_Update(const studio_t* data)
{
    int found = 0;              //  成功更新演出厅的标志found置0
    FILE* fp = NULL;            // 文件指针
    studio_t buf;               // 读取数据的缓冲区
    // 参数验证
    if (data == NULL) {
        printf("错误：传入的数据指针为空！\n");
        return 0;
    }
    if (data->id <= 0) {
        printf("错误：演出厅ID（%d）无效！\n", data->id);
        return 0;
    }
    // 以"rb+"方式打开STUDIO_DATA_FILE文件
    fp = fopen(STUDIO_DATA_FILE, "rb+");

    //  若打开失败，则输出打开文件失败，返回0
    if (fp == NULL) {
        printf("打开文件失败！\n");
        return 0;
    }
    // 循环读取文件中的演出厅记录
    while (1) {
        // e) 从文件读一条演出厅记录至buf
        size_t read_count = fread(&buf, sizeof(studio_t), 1, fp);

        // d) 若读到文件末尾，则转g)
        if (read_count != 1) {
            break;  // 跳出循环，进入步骤g)
        }
        if (buf.id == data->id) {
            // 将文件指针向文件头方向移动sizeof(studio_t)个字节
            fseek(fp, -sizeof(studio_t), SEEK_CUR);

            // 使用fwrite函数将data中数据写入到文件中
            if (fwrite(data, sizeof(studio_t), 1, fp) == 1) {
                found = 1;  // 设置成功标志
                printf("演出厅（ID: %d）更新成功！\n", data->id);
            }
            else {
                printf("写入文件失败！\n");
            }
            break;
        }
        // 否则，继续循环读取下一条记录
    }
    // 关闭文件
    if (fp != NULL) {
        fclose(fp);
    }
    if (!found) {
        printf("未找到ID为%d的演出厅！\n", data->id);
    }
    return found;
}

//用于从文件中载入一个演出厅的数据
int Studio_Perst_SelectByID(int ID, studio_t* buf)
{
    int found = 0;              // 成功载入演出厅数据的标志found置0
    FILE* fp = NULL;            // 文件指针
    studio_t data;              // 读取数据的临时变量
    // 参数验证
    if (ID <= 0) {
        printf("错误：演出厅ID（%d）无效！\n", ID);
        return 0;
    }
    if (buf == NULL) {
        printf("错误：缓冲区指针为空！\n");
        return 0;
    }
    //  以"rb"方式打开STUDIO_DATA_FILE文件
    fp = fopen(STUDIO_DATA_FILE, "rb");

    //  若打开失败，则输出打开文件失败，返回0
    if (fp == NULL) {
        printf("打开文件失败！\n");
        return 0;
    }
    // 循环读取文件，查找匹配的ID
    while (1) {
        // e) 从文件读一条演出厅记录至data
        size_t read_count = fread(&data, sizeof(studio_t), 1, fp);
        //  若读到文件末尾，则转g)
        if (read_count != 1) {
            break;
        }
        //  若ID==data.id
        if (ID == data.id) {
            *buf = data;
            found = 1;
            break;
        }
        // 否则，继续循环读取下一条记录
    }
    // g) 关闭文件
    if (fp != NULL) {
        fclose(fp);
    }
    return found;
}

//用于从文件中载入所有演出厅数据
int Studio_Perst_SelectAll(studio_list_t* list)
{
    int recCount = 0;           //  成功载入演出厅的个数recCount置0
    FILE* fp = NULL;            // 文件指针
    studio_t data;              // 临时存储读取的数据
    studio_list_node_t* newNode = NULL; // 新节点
    studio_list_node_t* tail = NULL;    // 链表尾指针

    //  以"rb"方式打开STUDIO_DATA_FILE文件
    fp = fopen(STUDIO_DATA_FILE, "rb");
    // 若打开失败，则输出打开文件失败，返回0
    if (fp == NULL) {
        printf("打开文件失败！\n");
        return 0;
    }

    // 释放list链表
    if (*list != NULL) {
        studio_list_node_t* current = *list;
        studio_list_node_t* next;

        while (current != NULL) {
            // 通过 list_node_t 结构获取下一个节点
            // 注意：list_node_t 应该包含 prev 和 next 指针
            next = (studio_list_node_t*)(current->node.next);
            free(current);
            current = next;
        }
        *list = NULL;
    }
    // 循环读取文件
    while (1) {
        // e) 若读到文件末尾，则转h)
        // 先尝试读取一条记录
        if (fread(&data, sizeof(studio_t), 1, fp) != 1) {
            // 可能到达文件末尾或读取失败，跳出循环转到步骤h)
            break;
        }
        // 从文件读一条演出厅记录至data（已经通过fread完成）

        //  新申请结点newNode，将data值置入新结点
        newNode = (studio_list_node_t*)malloc(sizeof(studio_list_node_t));
        if (newNode == NULL) {
            printf("内存分配失败！\n");
            fclose(fp);
            return 0;
        }
        // 将data值复制到新节点
        newNode->data = data;
        // 初始化链表节点的指针
        newNode->node.next = NULL;
        newNode->node.prev = NULL;

        // 将新结点通过尾插法插入链表list中
        if (*list == NULL) {
            *list = newNode;
            tail = newNode;
        }
        else {
            // 链表不为空，插入到尾部
            tail->node.next = (list_node_t*)newNode;
            newNode->node.prev = (list_node_t*)tail;
            tail = newNode;
        }
        recCount++;
    }
    //  关闭文件
    if (fp != NULL) {
        fclose(fp);
    }
    return recCount;
}

//用于在文件中删除指定ID的演出厅数据
int Studio_Perst_RemoveByID(int ID)
{
    int found = 0;                      // a) 成功删除演出厅的标志found置0
    FILE* fp_src = NULL;                // 源文件指针（临时文件）
    FILE* fp_dst = NULL;                // 目标文件指针（原始文件）
    studio_t data;                      // 临时存储读取的数据
    int rename_result = 0;              // 文件重命名结果
    // 参数验证
    if (ID <= 0) {
        printf("错误：演出厅ID（%d）无效！\n", ID);
        return 0;
    }
    // 检查文件是否存在
    FILE* fp_check = fopen(STUDIO_DATA_FILE, "rb");
    if (fp_check == NULL) {
        printf("演出厅数据文件不存在！\n");
        return 0;
    }
    fclose(fp_check);

    //  将演出厅原始数据文件 STUDIO_DATA_FILE 重命名为 STUDIO_DATA_TEMP_FILE
    rename_result = rename(STUDIO_DATA_FILE, STUDIO_DATA_TEMP_FILE);
    if (rename_result != 0) {
        printf("重命名文件失败！\n");
        return 0;
    }
    //  以"rb"方式打开 STUDIO_DATA_TEMP_FILE 文件，以"wb"方式打开 STUDIO_DATA_FILE 文件
    fp_src = fopen(STUDIO_DATA_TEMP_FILE, "rb");
    if (fp_src == NULL) {
        printf("打开临时文件失败！\n");
        // 尝试恢复原始文件名
        (void)rename(STUDIO_DATA_TEMP_FILE, STUDIO_DATA_FILE);
        return 0;
    }
    fp_dst = fopen(STUDIO_DATA_FILE, "wb");
    if (fp_dst == NULL) {
        printf("创建新文件失败！\n");
        fclose(fp_src);
        // 恢复原始文件名
        (void)rename(STUDIO_DATA_TEMP_FILE, STUDIO_DATA_FILE);
        return 0;
    }
    // 循环读取临时文件
    while (1) {
        if (fread(&data, sizeof(studio_t), 1, fp_src) != 1) {
            break;
        }
        //  若从 STUDIO_DATA_TEMP_FILE 文件中读取的演出厅 ID 不等于 ID
        if (data.id != ID) {
            // 则将该条记录写入 STUDIO_DATA_FILE 文件
            if (fwrite(&data, sizeof(studio_t), 1, fp_dst) != 1) {
                printf("写入文件失败！\n");
                fclose(fp_src);
                fclose(fp_dst);
                // 删除可能损坏的新文件
                remove(STUDIO_DATA_FILE);
                // 恢复原始文件名
                (void)rename(STUDIO_DATA_TEMP_FILE, STUDIO_DATA_FILE);
                return 0;
            }
        }
        else {
            //  否则，found = 1
            found = 1;
            printf("找到并删除演出厅（ID: %d）\n", ID);
            // 继续循环，但不写入该记录（即跳过）
        }
    }
    //  关闭两个文件
    if (fp_src != NULL) {
        fclose(fp_src);
    }
    if (fp_dst != NULL) {
        fclose(fp_dst);
    }
    // 删除临时文件
    if (remove(STUDIO_DATA_TEMP_FILE) != 0) {
        printf("删除临时文件失败\n");
    }
    if (found) {
        printf("成功删除演出厅（ID: %d）\n", ID);
    }
    else {
        printf("未找到ID为%d的演出厅！\n", ID);
    }
    return found;
}