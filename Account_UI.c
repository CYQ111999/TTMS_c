#include "Account_Srv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>


// ��������
#ifdef _WIN32
#define clear_screen() system("cls")
#else
#define clear_screen() system("clear")
#endif

// �ȴ��û�����
void press_any_key() {
	printf("\n�����������...");
	getchar();
}

// ��ʾ��ǰҳ���û���Ϣ
void display_current_page(paging_t* pg) {
	if (!account_list || pg->totalRecords == 0) {
		printf("\n��ǰû���û����ݣ�\n");
		return;
	}

	printf("\n�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("                      �û��б�\n");
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

	int i = 0;
	account_list_t pos = NULL;

	printf("���  ID   �û���       ����       �û�����\n");
	printf("----  ----  ----------  ----------  --------------\n");

	Paging_ViewPage_ForEach(account_list, *pg, account_list_node_t, pos, i) {
		if (pos) {
			char* type_str = "δ֪";
			switch (pos->data.type) {
			case USR_ANOMY: type_str = "�����û�"; break;
			case USR_CLERK: type_str = "����Ա"; break;
			case USR_MANG:  type_str = "����"; break;
			case USR_ADMIN: type_str = "����Ա"; break;
			}

			printf("%-4d  %-4d  %-10s  %-10s  %s\n",
				i + 1,
				pos->data.id,
				pos->data.username,
				pos->data.password,
				type_str);
		}
	}

	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
}

// ��ʾ��ҳ��Ϣ
void display_page_info(paging_t* pg) {
	int current_page = Pageing_CurPage(*pg);
	int total_pages = Pageing_TotalPages(*pg);

	printf("��ǰ�� %d/%d ҳ | �ܼ�¼��: %d | ÿҳ��ʾ: %d ��\n",
		current_page, total_pages, pg->totalRecords, pg->pageSize);
}

// ���¼����ܼ�¼���ĸ�������
void recalc_total_records(paging_t* pg) {
	if (!account_list) {
		pg->totalRecords = 0;
		return;
	}

	int count = 0;
	account_list_t cur = NULL;
	List_ForEach(account_list, cur) {
		count++;
	}
	pg->totalRecords = count;
}

// �����û�����
void Account_UI_Add(paging_t* pg) {
	clear_screen();
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("                      �����û�\n");
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

	// ��ȡ��һ�����õ�ID
	int next_id = getNextId();
	printf("���û�ID���Զ�����Ϊ: %d\n", next_id);
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

	account_t new_user;
	new_user.id = next_id;

	// �����û���
	int valid_username = 0;
	while (!valid_username) {
		printf("�������û��� (3-20���ַ�, ���ܰ����ո�): ");
		fgets(new_user.username, 30, stdin);
		new_user.username[strcspn(new_user.username, "\n")] = '\0';  // �Ƴ����з�

		// ��֤�û���
		if (strlen(new_user.username) < 3) {
			printf("�����û���̫�̣�������Ҫ3���ַ���\n");
		}
		else if (strlen(new_user.username) > 20) {
			printf("�����û���̫�������20���ַ���\n");
		}
		else if (strchr(new_user.username, ' ') != NULL) {
			printf("�����û������ܰ����ո�\n");
		}
		else {
			valid_username = 1;
		}
	}

	// ��������
	int valid_password = 0;
	while (!valid_password) {
		printf("���������� (6-20���ַ�): ");
		fgets(new_user.password, 30, stdin);
		new_user.password[strcspn(new_user.password, "\n")] = '\0';  // �Ƴ����з�

		if (strlen(new_user.password) < 6) {
			printf("��������̫�̣�������Ҫ6���ַ���\n");
		}
		else if (strlen(new_user.password) > 20) {
			printf("��������̫�������20���ַ���\n");
		}
		else {
			valid_password = 1;
		}
	}

	// �����û�����
	int valid_type = 0;
	while (!valid_type) {
		printf("\n��ѡ���û����ͣ�\n");
		printf("  1. ����Ա (USR_CLERK)\n");
		printf("  2. ���� (USR_MANG)\n");
		printf("  3. ����Ա (USR_ADMIN)\n");
		printf("  4. �����û� (USR_ANOMY)\n");
		printf("���������ͱ�� (1-4): ");

		int type_choice;
		if (scanf("%d", &type_choice) != 1) {
			printf("������������Ч�����֣�\n");
			while (getchar() != '\n');  // ������뻺����
			continue;
		}
		getchar();  // ��ջ��з�

		switch (type_choice) {
		case 1:
			new_user.type = USR_CLERK;
			valid_type = 1;
			break;
		case 2:
			new_user.type = USR_MANG;
			valid_type = 1;
			break;
		case 3:
			new_user.type = USR_ADMIN;
			valid_type = 1;
			break;
		case 4:
			new_user.type = USR_ANOMY;
			valid_type = 1;
			break;
		default:
			printf("������Ч�����ͱ�ţ�������ѡ��\n");
			break;
		}
	}

	// ȷ����Ϣ
	printf("\n�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("��ȷ���û���Ϣ��\n");
	printf("  ID: %d\n", new_user.id);
	printf("  �û���: %s\n", new_user.username);
	printf("  ����: %s\n", new_user.password);
	printf("  �û�����: %d", new_user.type);

	switch (new_user.type) {
	case USR_ANOMY: printf(" (�����û�)\n"); break;
	case USR_CLERK: printf(" (����Ա)\n"); break;
	case USR_MANG:  printf(" (����)\n"); break;
	case USR_ADMIN: printf(" (����Ա)\n"); break;
	}

	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("ȷ����Ӵ��û���(y/n): ");

	char confirm;
	scanf("%c", &confirm);
	getchar();  // ��ջ��з�

	if (confirm == 'y' || confirm == 'Y') {
		// �������ݲ㺯������û�
		int result = Account_Srv_Add(&new_user);

		if (result) {
			printf("\n�û���ӳɹ���\n");

			// ֱ�Ӹ��·�ҳ�����ܼ�¼������Ϊ�����Ѿ������ˣ�
			pg->totalRecords++;

			// �������û����ڵ�ҳ��
			int total_pages = Pageing_TotalPages(*pg);

			// ��λ�����һҳ
			Paging_Locate_LastPage(account_list, *pg, account_list_node_t);

			printf("���Զ���ת�����һҳ�鿴���������\n");
			printf("���û�λ�ڵ� %d/%d ҳ\n", total_pages, total_pages);
		}
		else {
			printf("\n�û����ʧ�ܣ��������û����Ѵ��ڻ���������\n");
		}
	}
	else {
		printf("\n��ȡ������û���\n");
	}

	press_any_key();
}

// �޸��û����� - ʹ�ø�����ģ����ѯ����
void Account_UI_Modify(paging_t* pg) {
	clear_screen();
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("                      �޸��û�\n");
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

	if (List_IsEmpty(account_list)) {
		printf("��ǰû���û����ݣ�\n");
		press_any_key();
		return;
	}

	// 1. ���û���ģ������
	char search_keyword[30];
	printf("������Ҫ���ҵ��û�����֧��ģ��������: ");
	fgets(search_keyword, 30, stdin);
	search_keyword[strcspn(search_keyword, "\n")] = '\0';

	if (strlen(search_keyword) == 0) {
		printf("�����ؼ��ֲ���Ϊ�գ�\n");
		press_any_key();
		return;
	}

	// ʹ�ø�����ģ����ѯ����
	account_list_t search_results = Account_Srv_FindByUsrName(search_keyword);

	if (search_results == NULL) {
		printf("û���ҵ����� '%s' ���û�\n", search_keyword);
		press_any_key();
		return;
	}

	// ��ʾ�������
	printf("\n�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("                   �������\n");
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

	printf("���  ID   �û���       �û�����\n");
	printf("----  ----  ----------  --------------\n");

	int result_count = 0;
	account_list_t pos = NULL;
	List_ForEach(search_results, pos) {
		if (pos) {
			result_count++;
			char* type_str = "δ֪";
			switch (pos->data.type) {
			case USR_ANOMY: type_str = "�����û�"; break;
			case USR_CLERK: type_str = "����Ա"; break;
			case USR_MANG:  type_str = "����"; break;
			case USR_ADMIN: type_str = "����Ա"; break;
			}

			printf("%-4d  %-4d  %-10s  %s\n",
				result_count,
				pos->data.id,
				pos->data.username,
				type_str);
		}
	}

	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("���ҵ� %d ��ƥ����û�\n", result_count);

	// 2. ���û�ָ��Ҫ�޸ĵ��û�ID
	int target_id;
	printf("\n������Ҫ�޸ĵ��û�ID: ");
	scanf("%d", &target_id);
	getchar();  // ��ջ��з�

	// ���������û�ID�Ƿ������������
	int id_found_in_results = 0;
	account_list_t cur_result = NULL;
	List_ForEach(search_results, cur_result) {
		if (cur_result->data.id == target_id) {
			id_found_in_results = 1;
			break;
		}
	}

	if (!id_found_in_results) {
		printf("����IDΪ %d ���û�������������У�\n", target_id);
		List_Destroy(search_results, account_list_node_t);
		press_any_key();
		return;
	}

	// ����Ŀ���û�
	account_t* target_user = Account_Srv_FindById(target_id);

	if (target_user == NULL) {
		printf("δ�ҵ�IDΪ %d ���û�\n", target_id);
		List_Destroy(search_results, account_list_node_t);
		press_any_key();
		return;
	}

	// 3. ��ʾ��ǰ��Ϣ�������޸�����
	printf("\n�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("              �û� %d �ĵ�ǰ��Ϣ\n", target_user->id);
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

	printf("��ǰ��Ϣ��\n");
	printf("  �û���: %s\n", target_user->username);
	printf("  ����: %s\n", target_user->password);
	printf("  �û�����: %d", target_user->type);

	switch (target_user->type) {
	case USR_ANOMY: printf(" (�����û�)\n"); break;
	case USR_CLERK: printf(" (����Ա)\n"); break;
	case USR_MANG:  printf(" (����)\n"); break;
	case USR_ADMIN: printf(" (����Ա)\n"); break;
	}

	printf("\n�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("�������޸ĺ����Ϣ������򱣳�ԭֵ����\n");

	// ׼���޸ĺ���û�����
	account_t modified_user = *target_user;

	// �������û���
	char new_username[30];
	printf("���û�������ձ���ԭֵ '%s'��: ", target_user->username);
	fgets(new_username, 30, stdin);
	new_username[strcspn(new_username, "\n")] = '\0';

	if (strlen(new_username) > 0) {
		// ����û����Ƿ��������û��ظ��������Լ���
		int username_exists = 0;
		account_list_t cur = NULL;
		List_ForEach(account_list, cur) {
			if (cur->data.id != target_user->id && strcmp(cur->data.username, new_username) == 0) {
				username_exists = 1;
				break;
			}
		}

		if (username_exists) {
			printf("�����û��� '%s' �Ѵ��ڣ�\n", new_username);
			List_Destroy(search_results, account_list_node_t);
			press_any_key();
			return;
		}

		strcpy(modified_user.username, new_username);
	}

	// ����������
	if (strcmp(target_user->username, "admin") == 0) {
		printf("ע�⣺����Ա�˻����벻���޸ģ�������ԭֵ��\n");
	}
	else {
		char new_password[30];
		printf("�����루��ձ���ԭֵ '%s'��: ", target_user->password);
		fgets(new_password, 30, stdin);
		new_password[strcspn(new_password, "\n")] = '\0';

		if (strlen(new_password) > 0) {
			strcpy(modified_user.password, new_password);
		}
	}

	// �������û�����
	int new_type_choice = -1;
	printf("���û����ͣ�0-����ԭֵ��1-����Ա��2-�����3-����Ա��4-������: ");
	char type_input[10];
	fgets(type_input, 10, stdin);
	type_input[strcspn(type_input, "\n")] = '\0';

	if (strlen(type_input) > 0) {
		sscanf(type_input, "%d", &new_type_choice);

		if (new_type_choice != 0) {  // 0��ʾ����ԭֵ
			switch (new_type_choice) {
			case 4: modified_user.type = USR_ANOMY; break;
			case 1: modified_user.type = USR_CLERK; break;
			case 2: modified_user.type = USR_MANG; break;
			case 3: modified_user.type = USR_ADMIN; break;
			default:
				printf("������Ч���û����ͣ�\n");
				List_Destroy(search_results, account_list_node_t);
				press_any_key();
				return;
			}
		}
	}

	// 4. ȷ���޸�
	printf("\n�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("             ��ȷ���޸ĺ����Ϣ\n");
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

	printf("�޸ĺ���Ϣ��\n");
	printf("  ID: %d\n", modified_user.id);
	printf("  �û���: %s\n", modified_user.username);
	printf("  ����: %s\n", modified_user.password);
	printf("  �û�����: %d", modified_user.type);

	switch (modified_user.type) {
	case USR_ANOMY: printf(" (�����û�)\n"); break;
	case USR_CLERK: printf(" (����Ա)\n"); break;
	case USR_MANG:  printf(" (����)\n"); break;
	case USR_ADMIN: printf(" (����Ա)\n"); break;
	}

	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("ȷ���޸Ĵ��û���(y/n): ");

	char confirm;
	scanf("%c", &confirm);
	getchar();  // ��ջ��з�

	if (confirm == 'y' || confirm == 'Y') {
		// 5. �������ݲ㺯���޸��û�
		int result = Account_Srv_Modify(&modified_user);

		if (result) {
			printf("\n�û���Ϣ�޸ĳɹ���\n");

			// 6. �����û����ڵ�ҳ��
			int user_page_index = 0;
			int found = 0;
			account_list_t cur = NULL;

			// �����û�����������
			List_ForEach(account_list, cur) {
				if (cur->data.id == target_user->id) {
					found = 1;
					break;
				}
				user_page_index++;
			}

			if (found) {
				// ����ҳ�루��1��ʼ��
				int user_page = (user_page_index / pg->pageSize) + 1;

				// ��ת�����û����ڵ�ҳ��
				int total_pages = Pageing_TotalPages(*pg);
				if (user_page > total_pages) {
					user_page = total_pages;
				}

				pg->offset = (user_page - 1) * pg->pageSize;
				List_Paging(account_list, *pg, account_list_node_t);

				printf("���Զ���ת���� %d ҳ�����û�����ҳ�棩\n", user_page);
			}
			else {
				printf("�޷���λ�û������ص�ǰҳ�档\n");
			}
		}
		else {
			printf("\n�û���Ϣ�޸�ʧ�ܣ�\n");
		}
	}
	else {
		printf("\n��ȡ���޸ġ�\n");
	}

	// ���������������
	List_Destroy(search_results, account_list_node_t);

	press_any_key();
}

// ɾ���û�����
void Account_UI_Delete(paging_t* pg) {
	clear_screen();
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("                      ɾ���û�\n");
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

	if (List_IsEmpty(account_list)) {
		printf("��ǰû���û����ݣ�\n");
		press_any_key();
		return;
	}

	// 1. ���û���ģ������
	char search_keyword[30];
	printf("������Ҫ���ҵ��û�����֧��ģ��������: ");
	fgets(search_keyword, 30, stdin);
	search_keyword[strcspn(search_keyword, "\n")] = '\0';

	if (strlen(search_keyword) == 0) {
		printf("�����ؼ��ֲ���Ϊ�գ�\n");
		press_any_key();
		return;
	}

	// ʹ��ģ����ѯ����
	account_list_t search_results = Account_Srv_FindByUsrName(search_keyword);

	if (search_results == NULL) {
		printf("û���ҵ����� '%s' ���û�\n", search_keyword);
		press_any_key();
		return;
	}

	// ��ʾ�������
	printf("\n�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("                   �������\n");
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

	printf("���  ID   �û���       �û�����\n");
	printf("----  ----  ----------  --------------\n");

	int result_count = 0;
	account_list_t pos = NULL;
	List_ForEach(search_results, pos) {
		if (pos) {
			result_count++;
			char* type_str = "δ֪";
			switch (pos->data.type) {
			case USR_ANOMY: type_str = "�����û�"; break;
			case USR_CLERK: type_str = "����Ա"; break;
			case USR_MANG:  type_str = "����"; break;
			case USR_ADMIN: type_str = "����Ա"; break;
			}

			printf("%-4d  %-4d  %-10s  %s\n",
				result_count,
				pos->data.id,
				pos->data.username,
				type_str);
		}
	}

	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("���ҵ� %d ��ƥ����û�\n", result_count);

	// 2. ���û�ָ��Ҫɾ�����û�ID
	int target_id;
	printf("\n������Ҫɾ�����û�ID: ");
	scanf("%d", &target_id);
	getchar();  // ��ջ��з�

	// ���������û�ID�Ƿ������������
	int id_found_in_results = 0;
	account_list_t cur_result = NULL;
	List_ForEach(search_results, cur_result) {
		if (cur_result->data.id == target_id) {
			id_found_in_results = 1;
			break;
		}
	}

	if (!id_found_in_results) {
		printf("����IDΪ %d ���û�������������У�\n", target_id);
		List_Destroy(search_results, account_list_node_t);
		press_any_key();
		return;
	}

	// ����Ŀ���û�
	account_t* target_user = Account_Srv_FindById(target_id);

	if (target_user == NULL) {
		printf("δ�ҵ�IDΪ %d ���û�\n", target_id);
		List_Destroy(search_results, account_list_node_t);
		press_any_key();
		return;
	}

	// ����Ƿ�Ϊadmin�û�
	if (strcmp(target_user->username, "admin") == 0) {
		printf("���󣺲���ɾ������Ա�˻���\n");
		List_Destroy(search_results, account_list_node_t);
		press_any_key();
		return;
	}

	// ��ʾҪɾ�����û���Ϣ
	printf("\n�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("              ��Ҫɾ�����û���Ϣ\n");
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

	printf("�û���Ϣ��\n");
	printf("  ID: %d\n", target_user->id);
	printf("  �û���: %s\n", target_user->username);
	printf("  �û�����: %d", target_user->type);

	switch (target_user->type) {
	case USR_ANOMY: printf(" (�����û�)\n"); break;
	case USR_CLERK: printf(" (����Ա)\n"); break;
	case USR_MANG:  printf(" (����)\n"); break;
	case USR_ADMIN: printf(" (����Ա)\n"); break;
	}

	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

	// 3. ȷ��ɾ��
	printf("���棺�˲������ɻָ���\n");
	printf("ȷ��ɾ�����û���(y/n): ");

	char confirm;
	scanf("%c", &confirm);
	getchar();  // ��ջ��з�

	if (confirm == 'y' || confirm == 'Y') {
		// 4. ��ɾ��ǰ�ȼ�¼�û����ڵ�ҳ��
		int user_page_index = 0;
		int found = 0;
		account_list_t cur = NULL;

		// �����û�����������
		List_ForEach(account_list, cur) {
			if (cur->data.id == target_user->id) {
				found = 1;
				break;
			}
			user_page_index++;
		}

		int user_page_before_delete = 0;
		if (found) {
			// ����ҳ�루��1��ʼ��
			user_page_before_delete = (user_page_index / pg->pageSize) + 1;
		}

		// 5. �������ݲ㺯��ɾ���û�
		int result = Account_Srv_Del(target_id);

		if (result) {
			printf("\n�û�ɾ���ɹ���\n");

			// 6. ���¼����ܼ�¼��
			int new_total = 0;
			account_list_t cur2 = NULL;
			List_ForEach(account_list, cur2) {
				new_total++;
			}
			pg->totalRecords = new_total;

			// 7. ����ɾ����Ӧ����ʾ��ҳ��
			int target_page = user_page_before_delete;

			// ��鵱ǰҳ�Ƿ�������
			if (pg->totalRecords > 0) {
				int total_pages = Pageing_TotalPages(*pg);

				// ���Ŀ��ҳ������ҳ������ʾ���һҳ
				if (target_page > total_pages) {
					target_page = total_pages;
				}

				// ���Ŀ��ҳ��0���û������ڣ�������ʾ��һҳ
				if (target_page <= 0) {
					target_page = 1;
				}

				// ��ת��Ŀ��ҳ
				pg->offset = (target_page - 1) * pg->pageSize;
				List_Paging(account_list, *pg, account_list_node_t);

				printf("���Զ���ת���� %d ҳ\n", target_page);
			}
			else {
				// û�������ˣ����õ���һҳ
				pg->offset = 0;
				pg->curPos = NULL;
				printf("�����û���ɾ�����б�Ϊ�ա�\n");
			}
		}
		else {
			printf("\n�û�ɾ��ʧ�ܣ�\n");
		}
	}
	else {
		printf("\n��ȡ��ɾ����\n");
	}

	// ���������������
	List_Destroy(search_results, account_list_node_t);

	press_any_key();
}

// �����û�����
void Account_UI_Search(paging_t* pg) {
	clear_screen();
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
	printf("                      �����û�\n");
	printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

	if (List_IsEmpty(account_list)) {
		printf("��ǰû���û����ݣ�\n");
		press_any_key();
		return;
	}

	int search_again = 1;

	do {
		clear_screen();
		printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
		printf("                      �����û�\n");
		printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

		// 1. ���������ؼ���
		char search_keyword[30];
		printf("������Ҫ���ҵ��û�����֧��ģ��������: ");
		fgets(search_keyword, 30, stdin);
		search_keyword[strcspn(search_keyword, "\n")] = '\0';

		if (strlen(search_keyword) == 0) {
			printf("�����ؼ��ֲ���Ϊ�գ�\n");
			press_any_key();
			continue;
		}

		// 2. ʹ��ģ����ѯ����
		account_list_t search_results = Account_Srv_FindByUsrName(search_keyword);

		if (search_results == NULL) {
			printf("\nû���ҵ����� '%s' ���û�\n", search_keyword);

			printf("\n�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
			printf("����ѡ�\n");
			printf("  1. ��������\n");
			printf("  0. �������˵�\n");
			printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
			printf("��ѡ��: ");

			int choice;
			scanf("%d", &choice);
			getchar();

			if (choice == 0) {
				search_again = 0;
			}
			continue;
		}

		// 3. ��ʾ�������
		printf("\n�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
		printf("                   �������\n");
		printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

		printf("���  ID   �û���       ����       �û�����\n");
		printf("----  ----  ----------  ----------  --------------\n");

		int result_count = 0;
		account_list_t pos = NULL;
		List_ForEach(search_results, pos) {
			if (pos) {
				result_count++;
				char* type_str = "δ֪";
				switch (pos->data.type) {
				case USR_ANOMY: type_str = "�����û�"; break;
				case USR_CLERK: type_str = "����Ա"; break;
				case USR_MANG:  type_str = "����"; break;
				case USR_ADMIN: type_str = "����Ա"; break;
				}

				printf("%-4d  %-4d  %-10s  %-10s  %s\n",
					result_count,
					pos->data.id,
					pos->data.username,
					pos->data.password,
					type_str);
			}
		}

		printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
		printf("���ҵ� %d ������ '%s' ���û�\n", result_count, search_keyword);

		// 4. ��ʾ��ϸ�û���Ϣѡ��
		printf("\n�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
		printf("����ѡ�\n");
		printf("  1. �鿴�û���ϸ��Ϣ\n");
		printf("  2. ��������\n");
		printf("  0. �������˵�\n");
		printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
		printf("��ѡ��: ");

		int choice;
		scanf("%d", &choice);
		getchar();

		switch (choice) {
		case 1:  // �鿴�û���ϸ��Ϣ
		{
			int target_id;
			printf("������Ҫ�鿴���û�ID: ");
			scanf("%d", &target_id);
			getchar();

			// ���������û�ID�Ƿ������������
			int id_found_in_results = 0;
			account_list_t cur_result = NULL;
			List_ForEach(search_results, cur_result) {
				if (cur_result->data.id == target_id) {
					id_found_in_results = 1;
					break;
				}
			}

			if (!id_found_in_results) {
				printf("����IDΪ %d ���û�������������У�\n", target_id);
				press_any_key();
				break;
			}

			// ����Ŀ���û�
			account_t* target_user = Account_Srv_FindById(target_id);

			if (target_user) {
				printf("\n�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
				printf("                  �û���ϸ��Ϣ\n");
				printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

				printf("�û�ID: %d\n", target_user->id);
				printf("�û���: %s\n", target_user->username);
				printf("����: %s\n", target_user->password);
				printf("�û�����: %d", target_user->type);

				switch (target_user->type) {
				case USR_ANOMY: printf(" (�����û�)\n"); break;
				case USR_CLERK: printf(" (����Ա)\n"); break;
				case USR_MANG:  printf(" (����)\n"); break;
				case USR_ADMIN: printf(" (����Ա)\n"); break;
				}

				printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
				press_any_key();
			}
			else {
				printf("δ�ҵ�IDΪ %d ���û�\n", target_id);
				press_any_key();
			}
		}
		break;

		case 2:  // ��������
			search_again = 1;
			break;

		case 0:  // �������˵�
			search_again = 0;
			break;

		default:
			printf("��Ч��ѡ����������˵���\n");
			press_any_key();
			search_again = 0;
			break;
		}

		// ���������������
		List_Destroy(search_results, account_list_node_t);

	} while (search_again);
}

// ��ҳ���������
void Account_UI_FetchAll() {
	// ��������
	int count = loadAllFromFile();
	if (count == 0) {
		printf("û���ҵ��û����ݣ�\n");
		printf("������Ĭ�Ϲ���Ա�˻���\n");
		press_any_key();
	}

	// ��ʼ����ҳ��
	paging_t pg;
	pg.pageSize = 5;  // Ĭ��ÿҳ5��
	pg.totalRecords = 0;
	pg.offset = 0;

	// �����ܼ�¼��
	recalc_total_records(&pg);

	// ��λ����һҳ
	Paging_Locate_FirstPage(account_list, pg);

	int choice = 0;
	int exit_browse = 0;

	do {
		clear_screen();

		// ��ʾ��ҳ��Ϣ�͵�ǰҳ����
		display_page_info(&pg);
		display_current_page(&pg);

		// ��ʾ�����˵����������·���
		printf("\n�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
		printf("                   �û��������\n");
		printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
		printf("��ҳ������\n");
		printf("  1. ��ҳ 2. ĩҳ 3. ��һҳ 4. ��һҳ\n");
		printf("  5. ��ת��ָ��ҳ 6. �޸�ÿҳ��ʾ����\n");
		printf("  7. ���¼�������\n");
		printf("\n�û������\n");
		printf("  8. �����û� 9. �޸��û� 10. ɾ���û� 11. �����û�\n");
		printf("\nϵͳ������\n");
		printf("  0. �˳�ϵͳ\n");
		printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
		printf("������������: ");

		scanf("%d", &choice);
		getchar();  // ��ջ�����

		switch (choice) {
		case 1:  // ��ҳ
			Paging_Locate_FirstPage(account_list, pg);
			break;

		case 2:  // ĩҳ
			Paging_Locate_LastPage(account_list, pg, account_list_node_t);
			break;

		case 3:  // ��һҳ
			if (!Pageing_IsFirstPage(pg)) {
				Paging_Locate_OffsetPage(account_list, pg, -1, account_list_node_t);
			}
			else {
				printf("�Ѿ��ǵ�һҳ�ˣ�\n");
				press_any_key();
			}
			break;

		case 4:  // ��һҳ
			if (!Pageing_IsLastPage(pg)) {
				Paging_Locate_OffsetPage(account_list, pg, 1, account_list_node_t);
			}
			else {
				printf("�Ѿ������һҳ�ˣ�\n");
				press_any_key();
			}
			break;

		case 5:  // ��ת��ָ��ҳ
		{
			int total_pages = Pageing_TotalPages(pg);
			if (total_pages <= 1) {
				printf("ֻ��1ҳ��������ת��\n");
				press_any_key();
				break;
			}

			int target_page;
			printf("������Ҫ��ת��ҳ�� (1-%d): ", total_pages);
			scanf("%d", &target_page);
			getchar();

			if (target_page < 1) target_page = 1;
			if (target_page > total_pages) target_page = total_pages;

			pg.offset = (target_page - 1) * pg.pageSize;
			List_Paging(account_list, pg, account_list_node_t);

			printf("����ת���� %d ҳ\n", target_page);
			press_any_key();
		}
		break;

		case 6:  // �޸�ÿҳ��ʾ����
		{
			int new_size;
			printf("��ǰÿҳ��ʾ: %d ��\n", pg.pageSize);
			printf("�������µ�ÿҳ��ʾ���� (1-20): ");
			scanf("%d", &new_size);
			getchar();

			if (new_size < 1) new_size = 1;
			if (new_size > 20) new_size = 20;

			pg.pageSize = new_size;

			// ���¼����ܼ�¼��
			recalc_total_records(&pg);

			// ������ǰƫ����
			if (pg.offset >= pg.totalRecords) {
				pg.offset = 0;
			}

			// ���¶�λ
			List_Paging(account_list, pg, account_list_node_t);

			printf("���޸�Ϊÿҳ��ʾ %d ��\n", new_size);
			press_any_key();
		}
		break;

		case 7:  // ���¼�������
			count = loadAllFromFile();
			if (count > 0) {
				// ���¼����ܼ�¼��
				recalc_total_records(&pg);

				// ���õ���һҳ
				Paging_Locate_FirstPage(account_list, pg);
				printf("�ɹ����¼��� %d ���û�\n", count);
			}
			else {
				printf("��������ʧ�ܣ�\n");
			}
			press_any_key();
			break;

		case 8:  // �����û�
			Account_UI_Add(&pg);
			break;

		case 9:  // �޸��û�
			Account_UI_Modify(&pg);
			break;

		case 10: // ɾ���û�
			Account_UI_Delete(&pg);
			break;

		case 11: // �����û�
			Account_UI_Search(&pg);
			break;

		case 0:  // �˳�ϵͳ
			exit_browse = 1;
			if (account_list) {
				List_Destroy(account_list, account_list_node_t);
			}
			printf("��лʹ�ã��ټ���\n");
			exit(0);
			break;

		default:
			printf("��Ч��ѡ�������ѡ��\n");
			press_any_key();
			break;
		}

	} while (!exit_browse);
}

// ��ȡ�������룬��ʾΪ*
void read_password_stars(char* password, int max_length) {
	int i = 0;
	char ch;

	while (1) {
		ch = _getch();  // ����ʾ������ַ�

		// �س�����������
		if (ch == '\r' || ch == '\n') {
			break;
		}
		// �˸������
		else if (ch == '\b') {
			if (i > 0) {
				i--;
				printf("\b \b");  // ɾ��һ���Ǻ�
			}
		}
		// ��ͨ�ַ�
		else if (i < max_length - 1 && isprint(ch)) {
			password[i++] = ch;
			printf("*");
		}
	}

	password[i] = '\0';
	printf("\n");
}

// ϵͳ��¼UI
int SysLogin() {
	int max_attempts = 3;  // ����Դ���
	int attempts = 0;

	while (attempts < max_attempts) {
		system("cls");  // Windows����

		printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
		printf("                    �û���¼\n");
		printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");

		// ��ʾʣ�ೢ�Դ���
		printf("ʣ�ೢ�Դ���: %d/%d\n\n", max_attempts - attempts, max_attempts);

		char username[30];
		char password[30];

		// �����û���
		printf("�û���: ");
		fgets(username, 30, stdin);
		username[strcspn(username, "\n")] = '\0';  // �Ƴ����з�

		if (strlen(username) == 0) {
			printf("�û�������Ϊ�գ�\n");
			printf("�����������...");
			_getch();
			continue;
		}

		// �������루��ʾΪ*��
		printf("����: ");
		read_password_stars(password, 30);

		if (strlen(password) == 0) {
			printf("���벻��Ϊ�գ�\n");
			printf("�����������...");
			_getch();
			continue;
		}

		// ��֤�û�
		int result = Account_Srv_Vertify(username, password);

		if (result) {
			printf("\n��¼�ɹ���\n");

			// ��ȡ��ǰ�û���Ϣ
			account_list_t user_list = Account_Srv_FindByUsrName(username);
			if (user_list) {
				// ��ȡ��һ���û�
				account_list_t first_user = NULL;
				List_ForEach(user_list, first_user) {
					if (first_user) {
						printf("\n�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
						printf("                    ��ӭ������%s��\n", username);
						printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
						printf("�û�����: %s\n", getAccountTypeString(first_user->data.type));
						printf("�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T\n");
						break;
					}
				}
				List_Destroy(user_list, account_list_node_t);
			}

			printf("�����������������...");
			_getch();
			return 1;  // ��¼�ɹ�
		}
		else {
			attempts++;
			printf("\n��¼ʧ�ܣ��û������������\n");

			if (attempts < max_attempts) {
				printf("����������³���...");
				_getch();
			}
			else {
				printf("�ѳ�������Դ�����ϵͳ���˳���\n");
				printf("��������˳�...");
				_getch();
			}
		}
	}

	return 0;  // ��¼ʧ��
}
