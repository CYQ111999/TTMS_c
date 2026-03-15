//cyq写
//主键服务业务逻辑层源文件
#include "EntityKey.h"         //本层头文件
#include "EntityKey_Persist.h" //包含持久化层，以调用其函数
#include <stdio.h>

//函数：EntKey_Srv_GetNewKeys
//功能：为其他模块提供获取新主键的业务逻辑层接口
//参数：entyName-实体名，count-需要数量
//返回值：成功返回起始键值，失败返回0
//说明：此处可添加统一的业务规则校验，实际文件操作调用持久化层完成
long EntKey_Srv_GetNewKeys(const char entyName[], int count) {
    if (count < 1) {
        printf("[业务层]错误：申请的主键数量必须大于0。\n");
        return 0;
    }
    //调用持久化层函数执行实际的文件读写操作
    long newKey = EntKey_Perst_GetNewKeys(entyName, count);
    if (newKey > 0) {
        printf("[业务层]已为实体'%s'分配起始键值%ld。\n", entyName, newKey);
    }
    return newKey;
}