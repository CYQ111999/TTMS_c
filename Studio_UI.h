#pragma once
//Studio_UI.h
//zyp
#ifndef STUDIO_UI_H
#define STUDIO_UI_H

 //管理演出厅入口函数
void Studio_UI_MgtEntry(void);

//添加新演出厅界面
int Studio_UI_Add(void);

//修改演出厅界面
int Studio_UI_Modify(int id);

// 删除演出厅界面
int Studio_UI_Delete(int id);

#endif