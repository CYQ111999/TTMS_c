// Utils.h
// cyq写
// 功能函数包
// 主要包含链表和文件的操作，大家尽量直接调用，因为这些都有完善的错误处理机制
// 另外诸如快排之类的记得调用库函数，不知道的问一下ai，尽量少手动实现多用库函数
// 效率高不容易错，实在没有还不太会写的跟我说一声我再补充到这个文件里
// 最后调用时记得传对参，是传地址还是变量看清楚

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

// 通用链表节点结构（教材风格）
typedef struct list_node {
    struct list_node* prev;
    struct list_node* next;
} list_node_t;

// 1. 初始化链表（创建头结点）
void List_Init(void** list, size_t node_size) {
    if (!list || node_size == 0) return;
    *list = malloc(node_size);
    if (!*list) {
        printf("错误：无法为头结点分配内存\n");
        return;
    }
    list_node_t* head = (list_node_t*)(*list);
    head->prev = head;
    head->next = head;
}

// 2. 释放所有数据结点（保留头结点）
void List_Free(void* list, void (*free_func)(void*)) {
    if (!list) return;
    list_node_t* head = (list_node_t*)list;
    list_node_t* cur = head->next;
    while (cur != head) {
        list_node_t* next = cur->next;
        if (free_func) free_func((void*)((char*)cur + sizeof(list_node_t)));
        free(cur);
        cur = next;
    }
    head->next = head;
    head->prev = head;
}

// 3. 销毁整个链表（含头结点）
void List_Destroy(void** list, void (*free_func)(void*)) {
    if (!list || !*list) return;
    List_Free(*list, free_func);
    free(*list);
    *list = NULL;
}

// 4. 头插法
void List_AddHead(void* list, void* newNode) {
    if (!list || !newNode) return;
    list_node_t* head = (list_node_t*)list;
    list_node_t* node = (list_node_t*)newNode;
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

// 5. 尾插法
void List_AddTail(void* list, void* newNode) {
    if (!list || !newNode) return;
    list_node_t* head = (list_node_t*)list;
    list_node_t* node = (list_node_t*)newNode;
    node->prev = head->prev;
    node->next = head;
    head->prev->next = node;
    head->prev = node;
}

// 6. 在指定结点前插入
void List_InsertBefore(void* node, void* newNode) {
    if (!node || !newNode) return;
    list_node_t* cur = (list_node_t*)node;
    list_node_t* new_node = (list_node_t*)newNode;  // 修改变量名
    new_node->prev = cur->prev;
    new_node->next = cur;
    cur->prev->next = new_node;
    cur->prev = new_node;
}

// 7. 在指定结点后插入
void List_InsertAfter(void* node, void* newNode) {
    if (!node || !newNode) return;
    list_node_t* cur = (list_node_t*)node;
    list_node_t* new_node = (list_node_t*)newNode;  // 修改变量名
    new_node->next = cur->next;
    new_node->prev = cur;
    cur->next->prev = new_node;
    cur->next = new_node;
}

// 8. 判断链表是否为空
bool List_IsEmpty(const void* list) {
    if (!list) return true;
    const list_node_t* head = (const list_node_t*)list;
    return (head->next == head) && (head->prev == head);
}

// 9. 从链表中移除结点（不释放）
void List_DelNode(void* node) {
    if (!node) return;
    list_node_t* cur = (list_node_t*)node;
    cur->prev->next = cur->next;
    cur->next->prev = cur->prev;
    cur->next = cur;
    cur->prev = cur;
}

// 10. 移除并释放结点
void List_FreeNode(void* node, void (*free_func)(void*)) {
    if (!node) return;
    List_DelNode(node);
    list_node_t* cur = (list_node_t*)node;
    if (free_func) free_func((void*)((char*)cur + sizeof(list_node_t)));
    free(cur);
}

// 11. 获取第一个数据结点
list_node_t* List_First(const void* list) {
    if (!list || List_IsEmpty(list)) return NULL;
    const list_node_t* head = (const list_node_t*)list;
    return head->next;
}

// 12. 获取下一个结点
list_node_t* List_Next(const list_node_t* cur) {
    return cur ? cur->next : NULL;
}

// 13. 获取链表长度
int List_GetSize(void* list) {
    if (!list) return 0;
    list_node_t* head = (list_node_t*)list;
    list_node_t* cur = head->next;
    int count = 0;
    while (cur != head) {
        count++;
        cur = cur->next;
    }
    return count;
}

//下面开始都是文件了

// 保存链表到文件
int File_SaveList(const char* filename, void* head,
    void* (*getNextFunc)(void*),
    int (*writeDataFunc)(FILE*, void*)) {
    if (!filename || !writeDataFunc) {
        printf("[文件IO] 错误：无效参数。\n");
        return -1;
    }
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        printf("[文件IO] 错误：无法创建文件 '%s'。\n", filename);
        return -1;
    }
    int count = 0;
    void* current = head;
    while (current) {
        if (writeDataFunc(fp, current) != 1) {
            printf("[文件IO] 错误：写入节点数据失败。\n");
            fclose(fp);
            return -1;
        }
        count++;
        current = getNextFunc(current);
    }
    fclose(fp);
    printf("[文件IO] 成功保存 %d 条记录到 '%s'。\n", count, filename);
    return count;
}

// 从文件加载链表
void* File_LoadList(const char* filename,
    void* (*readDataFunc)(FILE*),
    void* (*createNodeFunc)(void*)) {
    if (!filename || !readDataFunc || !createNodeFunc) {
        printf("[文件IO] 错误：无效参数。\n");
        return NULL;
    }
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        // 文件不存在是正常情况，比如第一次运行程序
        printf("[文件IO] 提示：数据文件 '%s' 不存在，将创建新文件。\n", filename);
        return NULL;
    }
    // 创建头结点
    void* head = NULL;
    List_Init(&head, sizeof(list_node_t));
    int count = 0;
    while (!feof(fp)) {
        // 尝试读取一个数据项
        void* data = readDataFunc(fp);
        if (!data) {
            // 可能是文件结束或读取失败
            if (feof(fp)) break;
            printf("[文件IO] 错误：从文件读取数据失败。\n");
            List_Destroy(&head, NULL);
            fclose(fp);
            return NULL;
        }
        // 创建新节点
        void* newNode = createNodeFunc(data);
        if (!newNode) {
            free(data);
            printf("[文件IO] 错误：创建链表节点失败。\n");
            List_Destroy(&head, NULL);
            fclose(fp);
            return NULL;
        }
        // 将节点添加到链表尾部
        List_AddTail(head, newNode);
        count++;
        free(data);
    }
    fclose(fp);
    printf("[文件IO] 成功从 '%s' 加载 %d 条记录。\n", filename, count);
    return head;
}

// 安全写入文件
int File_WriteSafe(const char* filename, void* data, size_t dataSize, int numItems) {
    if (!filename || !data || dataSize == 0) {
        return 0;
    }
    // 先写入临时文件
    char tempFile[256];
    sprintf(tempFile, "%s.tmp", filename);
    FILE* fp = fopen(tempFile, "wb");
    if (!fp) {
        printf("error:无法创建临时文件。\n");
        return 0;
    }
    size_t written = fwrite(data, dataSize, numItems, fp);
    fclose(fp);
    if (written != numItems) {
        printf("error:写入数据不完整。\n");
        remove(tempFile);
        return 0;
    }
    // 重命名临时文件为正式文件
    if (rename(tempFile, filename) != 0) {
        printf("error:无法重命名临时文件。\n");
        remove(tempFile);
        return 0;
    }
    return 1;
}

// 安全读取文件
int File_ReadSafe(const char* filename, void* data, size_t dataSize, int maxNumItems) {
    if (!filename || !data) {
        return -1;
    }
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        return 0; // 文件不存在不算错误
    }
    // 获取文件大小
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (fileSize % dataSize != 0) {
        printf("error:文件大小与数据结构不匹配。\n");
        fclose(fp);
        return -1;
    }
    int numItems = fileSize / dataSize;
    if (numItems > maxNumItems) {
        printf("error:文件数据量超过缓冲区容量。\n");
        fclose(fp);
        return -1;
    }
    size_t read = fread(data, dataSize, numItems, fp);
    fclose(fp);
    if (read != numItems) {
        printf("error:读取数据不完整。\n");
        return -1;
    }
    return numItems;
}

#endif // UTILS_H