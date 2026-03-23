//cyq写完成主键服务
#define _CRT_SECURE_NO_WARNINGS
#include "EntityKey_Persist.h"
#include "Common.h" 
#include "List.h"
#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 常量定义
#define ENTITY_KEY_FILE "./data/EntityKey.dat"  // 主键数据文件存储路径

long EntKey_Perst_GetNewKeys(const char entyName[], int count) {
    FILE* fp = NULL;               // 文件指针
    entity_key_t currentKey;       // 当前读取的主键记录
    int found = 0;                 // 查找标志：0-未找到，1-找到
    long newEntKey = 1;            // 新主键值，初始为1
    long filePosition = 0;         // 文件位置记录，用于回写更新
    //初始化标志
    found = 0;
    newEntKey = 1;
    //参数检查  若count<1，则提示出错，返回0
    if (count < 1) {
        printf("错误：请求的主键数量必须大于0！\n");
        return 0;
    }
    //文件打开策略 
    fp = fopen(ENTITY_KEY_FILE, "rb+");
    if (fp == NULL) {
        fp = fopen(ENTITY_KEY_FILE, "wb+");
    }
    //文件打开检查 - 若打开失败，则提示出错，返回0
    if (fp == NULL) {
        printf("错误：无法打开或创建主键文件！\n");
        return 0;
    }
    // 循环查找主键记录
    // 重置文件指针到开头
    fseek(fp, 0, SEEK_SET);
    // 循环读取文件中的每条主键记录
    while (fread(&currentKey, sizeof(entity_key_t), 1, fp) == 1) {
        // 比较实体名是否匹配
        if (strcmp(currentKey.name, entyName) == 0) {
            // 找到匹配记录，记录当前位置
            found = 1;
            filePosition = ftell(fp) - sizeof(entity_key_t);
            // 分配主键值区间
            // newEntKey = 当前主键值 + 1，作为新区间的最小值
            newEntKey = currentKey.key + 1;
            // 更新主键记录：当前值增加count
            currentKey.key += count;
            // 将文件指针移回记录开始位置
            fseek(fp, filePosition, SEEK_SET);
            // 将更新后的记录写回文件
            if (fwrite(&currentKey, sizeof(entity_key_t), 1, fp) != 1) {
                printf("错误：更新主键记录失败！\n");
                fclose(fp);
                return 0;
            }
            break;  // 找到并处理完成，退出循环
        }
    }
    //若文件中没有该主键，即found=0
    if (!found) {
        // 创建新的主键记录
        entity_key_t newKey;
        // 复制实体名，确保不超过40字符
        strncpy(newKey.name, entyName, 40);
        newKey.name[40] = '\0';  // 确保字符串终止
        // 设置主键值：分配count个，记录最大键值
        newKey.key = count;
        // 移动到文件末尾
        fseek(fp, 0, SEEK_END);
        // 将新记录写入文件末尾
        if (fwrite(&newKey, sizeof(entity_key_t), 1, fp) != 1) {
            printf("错误：写入新主键记录失败！\n");
            fclose(fp);
            return 0;
        }
        // 新实体的主键区间从1开始
        newEntKey = 1;
    }
    // 关闭文件，返回newEntKey
    fclose(fp);
    printf("[主键服务] 为实体 '%s' 分配了 %d 个主键，起始值为: %ld\n",
        entyName, count, newEntKey);
    return newEntKey;
}