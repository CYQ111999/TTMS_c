#include "Account.h"
#include "StaSales_UI.h"
#include "Common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void StaSales_UI_MgtEntry() {
	if (gl_CurUser.type == USR_CLERK) {
		void StaSales_UI_Self();
	}
	else if (gl_CurUser.type == USR_MANG) {
		void StaSales_UI_Clerk();
	}
	else {
		printf("账号没有权限");
		return 0;
	}
}