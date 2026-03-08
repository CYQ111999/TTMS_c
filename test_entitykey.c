#include "EntityKey_Persist.h"
#include <stdio.h>

int main() {
    long newKey;
    printf("=== 测试主键分配服务（修正版）===\n\n");
    // 清理之前的测试数据
    remove("data/EntityKey.dat");
    // 测试1：为用户分配单个主键
    printf("1. 第一次分配用户主键...\n");
    newKey = EntKey_Perst_GetNewKeys("Account", 1);
    printf("   结果: %ld (期望: 1)\n\n", newKey);
    // 测试2：再次为用户分配单个主键
    printf("2. 第二次分配用户主键...\n");
    newKey = EntKey_Perst_GetNewKeys("Account", 1);
    printf("   结果: %ld (期望: 2)\n\n", newKey);
    // 测试3：为剧目分配3个连续主键
    printf("3. 为剧目分配3个主键...\n");
    newKey = EntKey_Perst_GetNewKeys("Play", 3);
    printf("   起始值: %ld (期望: 1)\n", newKey);
    printf("   区间: [%ld, %ld]\n\n", newKey, newKey + 2);
    // 测试4：为票务分配5个连续主键
    printf("4. 为票务分配5个主键...\n");
    newKey = EntKey_Perst_GetNewKeys("Ticket", 5);
    printf("   起始值: %ld (期望: 1)\n", newKey);
    printf("   区间: [%ld, %ld]\n\n", newKey, newKey + 4);
    // 测试5：无效参数测试
    printf("5. 测试无效数量(count=0)...\n");
    newKey = EntKey_Perst_GetNewKeys("Studio", 0);
    printf("   结果: %ld (期望: 0，有错误提示)\n\n", newKey);
    // 测试6：再次测试用户，看是否连续
    printf("6. 第三次分配用户主键...\n");
    newKey = EntKey_Perst_GetNewKeys("Account", 1);
    printf("   结果: %ld (期望: 3)\n\n", newKey);
    // 测试7：测试新实体
    printf("7. 测试新实体'Schedule'...\n");
    newKey = EntKey_Perst_GetNewKeys("Schedule", 2);
    printf("   起始值: %ld (期望: 1)\n", newKey);
    printf("\n=== 测试完成 ===\n");
    return 0;
}