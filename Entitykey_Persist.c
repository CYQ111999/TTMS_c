//cyq写
//这个太难了我试着帮你们写了
#include "EntityKey_Persist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 常量定义
#define ENTITY_KEY_FILE "data/EntityKey.dat"
// 内部函数创建数据目录
static void _ensureDataDir() {
    //在Windows中创建data目录
    system("mkdir data 2>nul");
}
//主键分配函数
long EntKey_Perst_GetNewKeys(const char* entyName, int count) {
    FILE* fp = NULL;
    entity_key_t key;
    int found = 0;
    long newKey = 1;
    //参数检查
    if (count < 1) {
        printf("错误：请求的主键数量必须大于0!\n");
        return 0;
    }
    if (entyName == NULL || strlen(entyName) == 0) {
        printf("错误：实体名称不能为空！\n");
        return 0;
    }
    //确保数据目录存在
    _ensureDataDir();
    //尝试打开文件
    fp = fopen(ENTITY_KEY_FILE, "rb+");
    if (fp == NULL) {
        //文件不存在，创建新文件
        fp = fopen(ENTITY_KEY_FILE, "wb+");
        if (fp == NULL) {
            printf("错误：无法创建主键文件！\n");
            return 0;
        }
    }
    //查找现有记录
    found = 0;
    fseek(fp, 0, SEEK_SET);
    while (fread(&key, sizeof(entity_key_t), 1, fp) == 1) {
        if (strcmp(key.entyName, entyName) == 0) {
            found = 1;
            break;
        }
    }
    if (found) {
        // 5. 找到记录，分配新键值
        newKey = key.key + 1;
        key.key += count;
        //回退到记录开始位置
        fseek(fp, -sizeof(entity_key_t), SEEK_CUR);
        fwrite(&key, sizeof(entity_key_t), 1, fp);
    }
    else {
        //新实体，创建记录
        strncpy(key.entyName, entyName, 40);
        key.entyName[40] = '\0';
        key.key = count;  //存储分配的最大键值

        //移动到文件末尾添加
        fseek(fp, 0, SEEK_END);
        fwrite(&key, sizeof(entity_key_t), 1, fp);

        newKey = 1;  //新实体从1开始
    }
    //关闭文件
    fclose(fp);
    //记录日志
    printf("[主键服务] 为实体 '%s' 分配了 %d 个主键，起始值为: %ld\n",
        entyName, count, newKey);
    return newKey;
}