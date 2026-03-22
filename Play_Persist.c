#include <stdio.h>
#include <string.h>
#include <io.h>
#include "Play.h"
#include "EntityKey_Persist.h"
#include "List.h"
#define _CRT_SECURE_NO_WARNINGS

#define PLAY_DATA_FILE "Play.dat"
#define PLAY_DATA_TEMP_FILE "PlayTmp.dat"

//从剧目信息文件Play.dat中载入剧目名称包含指定字符串的剧目信息
int Play_Perst_SelectByName(play_list_t list, char condt[])
{
    int recCount = 0;        // 记录计数器
    FILE* fp = NULL;         // 文件指针
    play_t data;             // 临时存储读取的数据
    play_list_node_t* newNode = NULL;  // 新节点指针
    play_list_node_t* tail = NULL;     // 链表尾指针
    // 参数验证
    if (list == NULL) {
        printf("错误：链表头指针为空！\n");
        return 0;
    }
    if (condt == NULL) {
        printf("错误：查询条件为空！\n");
        return 0;
    }
    // a局部变量recCount赋初值0
    recCount = 0;
    // 判断剧目数据文件Play.dat是否存在
    fp = fopen(PLAY_DATA_FILE, "rb");
    if (fp == NULL) {
        // 若不存在，返回0，函数结束
        printf("剧目数据文件不存在！\n");
        return 0;
    }
    // b 初始化list为空链表
    // 由于list是一级指针，我们无法将调用方的指针置为NULL
    // 但我们可以清空链表（如果链表不为空的话）
    // 这里我们假设链表已经初始化，只需要设置next和prev指针
    list->node.next = NULL;
    list->node.prev = NULL;
    // 判断打开剧目数据文件Play.dat是否成功
    // 上面已经打开了文件，这里只是验证
    if (fp == NULL) {
        printf("打开剧目数据文件失败！\n");
        return 0;
    }
    // 设置尾指针初始为链表头
    tail = list;
    // 循环读取文件
    while (1) {
        // c) 判断剧目数据文件Play.dat是否读到末尾
        if (feof(fp)) {
            // 若是，执行步骤e)
            break;
        }
        // d 从剧目数据文件Play.dat中读出一条记录到play_t类型临时变量data
        size_t read_count = fread(&data, sizeof(play_t), 1, fp);
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
        // 判断data.name是否包含待查字符串condt
        if (strstr(data.name, condt) != NULL) {
            // 若包含，则构造newNode结点
            // 分配新节点内存
            newNode = (play_list_node_t*)malloc(sizeof(play_list_node_t));
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
        }

        // 执行步骤c（继续循环）
    }
    fclose(fp);
    return recCount;
}

int Play_Perst_SelectAll(play_list_t list) {
    int recCount = 0;
    play_t data;

    FILE* fp = fopen(PLAY_DATA_FILE, "rb");
    if (!fp) {
        return 0;
    }

    List_Free(list, play_list_node_t);

    while (fread(&data, sizeof(play_t), 1, fp) == 1) {
        play_list_node_t* newNode = (play_list_node_t*)malloc(sizeof(play_list_node_t));
        if (!newNode) break;

        newNode->data = data;
        List_AddTail(list, newNode);
        recCount++;
    }

    fclose(fp);
    return recCount;
}

int Play_Perst_Insert(play_t* data) {
    long key = EntKey_Perst_GetNewKeys("play", 1);
    if (key <= 0) return 0;
    data->id = (int)key;

    FILE* fp = fopen(PLAY_DATA_FILE, "ab");
    if (!fp) {
        printf("打开文件 %s 失败！\n", PLAY_DATA_FILE);
        return 0;
    }

    int ret = fwrite(data, sizeof(play_t), 1, fp);
    fclose(fp);
    return ret;
}

int Play_Perst_Update(const play_t* data) {
    int found = 0;
    play_t buf;

    FILE* fp = fopen(PLAY_DATA_FILE, "rb+");
    if (!fp) {
        printf("打开文件 %s 失败！\n", PLAY_DATA_FILE);
        return 0;
    }

    while (fread(&buf, sizeof(play_t), 1, fp) == 1) {
        if (buf.id == data->id) {
            fseek(fp, -(long)sizeof(play_t), SEEK_CUR);
            fwrite(data, sizeof(play_t), 1, fp);
            found = 1;
            break;
        }
    }

    fclose(fp);
    return found;
}

int Play_Perst_RemByID(int id) {
    int found = 0;
    play_t buf;

    if (rename(PLAY_DATA_FILE, PLAY_DATA_TEMP_FILE) != 0) {
        return 0;
    }

    FILE* fpSrc = fopen(PLAY_DATA_TEMP_FILE, "rb");
    FILE* fpDst = fopen(PLAY_DATA_FILE, "wb");

    if (!fpSrc || !fpDst) {
        if (fpSrc) fclose(fpSrc);
        if (fpDst) fclose(fpDst);
        return 0;
    }

    while (fread(&buf, sizeof(play_t), 1, fpSrc) == 1) {
        if (buf.id != id) {
            fwrite(&buf, sizeof(play_t), 1, fpDst);
        }
        else {
            found = 1;
        }
    }

    fclose(fpSrc);
    fclose(fpDst);
    remove(PLAY_DATA_TEMP_FILE);

    return found;
}

int Play_Perst_SelectByID(int id, play_t* buf) {
    int found = 0;
    play_t data;

    FILE* fp = fopen(PLAY_DATA_FILE, "rb");
    if (!fp) {
        return 0;
    }

    while (fread(&data, sizeof(play_t), 1, fp) == 1) {
        if (data.id == id) {
            *buf = data;
            found = 1;
            break;
        }
    }

    fclose(fp);
    return found;
}