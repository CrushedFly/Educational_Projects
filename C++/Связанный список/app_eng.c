#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "user.h"
#include "list.h"

void Users_print(List* const list){ //вывод на экран таблицы пользователей
	if(list->first == NULL){
			printf("There is no data in the table.\n");
	}
	else{
		printf("\t ID | Name \t| Surname \t| Age \t| Sallary\n\n");
		for(int i = 0; i < List_count(list);  i++) {
			User* user = List_at(list, i);	
			printf("\t %2.0d | %s \t| %s \t| %d \t| %.2f\n", user->id, user->name, user->surname, user->age, user->sallary);
		}
	}
}

void Users_save_file(List* const list, FILE* fptr){ //сохраняет таблицу в текстовый файл
	fptr = fopen("DATA.txt", "w");
	for(int i = 0; i < List_count(list);  i++) {
		User* user = List_at(list, i);
		fprintf(fptr, "%2.0d  %s \t %s \t %d \t %.2f\n", user->id, user->name, user->surname, user->age, user->sallary);
	}
	fclose(fptr);
}

void Users_load_file(List* const list, FILE* fptr){ //загружает таблицу из текстового файла
	int id;
	char name[USER_NAME_LEN];
	char surname[USER_NAME_LEN];
	int age;
	float sallary;
	
	fptr = fopen("DATA.txt", "r");
	while(!feof(fptr)){
		fscanf(fptr, "%d %s %s %d %f\n", &id, &name, &surname, &age, &sallary);
		List_push(list, User_create(id, name, surname, age, sallary));
	}
	
	for (int i = 0; i < List_count(list); i++) {
		for(int j = 0; j < List_count(list); j++) {
			User* user_1 = List_at(list, i);
			User* user_2 = List_at(list, j);
			if (user_1->id == user_2->id && i != j) List_delete_pos(list, j);
		}
	}
}

void* List_sort_id(List* const list) //сортировка таблицы по ID
{
	int j = List_count(list);
	while (j!=0) {
		for(int i = 1; i <= List_count(list); i++) {
			User* user_1 = List_at(list, i-1);
			User* user_2 = List_at(list, i);
			if (user_1->id > user_2->id) List_swap(list, i+1);
		}
		j--;
	}
}

void* List_sort_int(List* const list, int stage, int stage_sort) //сортировка численных данных таблицы (по возрастанию и по убыванию
{
	int j = List_count(list);
	while (j!=0) {
		for(int i = 1; i <= List_count(list); i++) {
			User* user_1 = List_at(list, i-1);
			User* user_2 = List_at(list, i);
			if (stage == 2) {
				if (stage_sort == 3) {
						if (user_1->age > user_2->age) List_swap(list, i+1);
					}
				if (stage_sort == 4) {
						if (user_1->sallary > user_2->sallary) List_swap(list, i+1);
					}
			}
			if (stage == 3) {
				if (stage_sort == 3) {
						if (user_1->age < user_2->age) List_swap(list, i+1);
					}
				if (stage_sort == 4) {
						if (user_1->sallary < user_2->sallary) List_swap(list, i+1);
					}
			}
		}
		j--;
	}
}

void* List_sort_char(List* const list, int stage, int stage_sort) //сортировка символьных данных (в алфавитном порядке и наоборот)
{
	int k = 0;
	int j = List_count(list);
	while (j!=0) {
		for(int i = 1; i <= List_count(list); i++) {
			User* user_1 = List_at(list, i-1);
			User* user_2 = List_at(list, i);
			if (stage == 2) {
				if (stage_sort == 1) {
					if (strcmp(user_1->name, user_2->name)){
						while (user_1->name[k] == user_2->name[k]) k++;
					}
					if (user_1->name[k] > user_2->name[k]) List_swap(list, i+1);					
				}
				if (stage_sort == 2) {
					if (strcmp(user_1->surname, user_2->surname)){
						while (user_1->surname[k] == user_2->surname[k]) k++;
					}
					if (user_1->surname[k] > user_2->surname[k]) List_swap(list, i+1);
				}
			}
			if (stage == 3) {
				if (stage_sort == 1) {
					if (strcmp(user_1->name, user_2->name)){
						while (user_1->name[k] == user_2->name[k]) k++;
					}
					if (user_1->name[k] < user_2->name[k]) List_swap(list, i+1);					
				}
				if (stage_sort == 2) {
					if (strcmp(user_1->surname, user_2->surname)){
						while (user_1->surname[k] == user_2->surname[k]) k++;
					}
					if (user_1->surname[k] < user_2->surname[k]) List_swap(list, i+1);
				}
			}
			k = 0;
		}
		j--;
	}
}

void* List_filter_char(List* const list, int stage_sort, char name_surname[USER_NAME_LEN]) //фильтрация таблицы по символьным данным
{
	if(list->first == NULL){
			printf("There is no data in the table.\n");
	}
	else{
		printf("\t ID | Name \t| Surname \t| Age \t| Sallary\n\n");
		short check = 0;
		for(int i = 0; i < List_count(list); i++){
			User* user = List_at(list, i);
			if (stage_sort == 2) {
				if (!strcmp(user->name, name_surname)) {
					check = 1;
					printf("\t %2.0d | %s \t| %s \t| %d \t| %.2f\n", user->id, user->name, user->surname, user->age, user->sallary);
				}
			}
			if (stage_sort == 3) {
				if (!strcmp(user->surname, name_surname)) {
					check = 1;
					printf("\t %2.0d | %s \t| %s \t| %d \t| %.2f\n", user->id, user->name, user->surname, user->age, user->sallary);
				}
			}
		}
		if (check == 0) printf("No users found.\n");
	}
}

void* List_filter_int(List* const list, int stage_sort, int number) //фильтрация таблицы по целочисленным данным
{
	if(list->first == NULL){
			printf("There is no data in the table.\n");
	}
	else{
		printf("\t ID | Name \t| Surname \t| Age \t| Sallary\n\n");
		short check = 0;
		for(int i = 0; i < List_count(list); i++){
			User* user = List_at(list, i);
			if (stage_sort == 1) {
				if (user->id == number) {
					check = 1;
					printf("\t %2.0d | %s \t| %s \t| %d \t| %.2f\n", user->id, user->name, user->surname, user->age, user->sallary);
				}
			}
			if (stage_sort == 4) {
				if (user->age == number) {
					check = 1;
					printf("\t %2.0d | %s \t| %s \t| %d \t| %.2f\n", user->id, user->name, user->surname, user->age, user->sallary);
				}
			}
		}
		if (check == 0) printf("No users found.\n");
	}
}

void* List_filter_float(List* const list, int stage_sort, float number) //фильтрация таблицы по вещественным данным
{
	if(list->first == NULL){
			printf("There is no data in the table.\n");
	}
	else{
		printf("\t ID | Name \t| Surname \t| Age \t| Sallary\n\n");
		short check = 0;
		for(int i = 0; i < List_count(list); i++){
			User* user = List_at(list, i);
			if (user->sallary == number) {
				check = 1;
				printf("\t %2.0d | %s \t| %s \t| %d \t| %.2f\n", user->id, user->name, user->surname, user->age, user->sallary);
			}
		}
		if (check == 0) printf("No users found.\n");
	}
}

int main()
{
	int id;
	char name[USER_NAME_LEN];
	char surname[USER_NAME_LEN];
	int age;
	float sallary;

	size_t pos;
	
	int stage, stage_sort;
	
	List *users = List_create();
	
	FILE *fptr;

	printf("Welcome! List of available actions:\n\n\
  1. Displaying the user table (without changing the order)\n\
  2. Displaying the user table (in ascending order)\n\
  3. Displaying the user table (in descending order)\n\
  4. Displaying the user table (by filter)\n\
  5. Adding a new user\n\
  6. Deleting an arbitrary user\n\
  7. Downloading data from a file 'DATA.txt '\n\
  8. Saving data to a file 'DATA.txt '\n\n\
Select an action: ");
 
	while(1){
		scanf("%d", &stage);
		printf("\n");
		switch(stage)
		{
			case 1:
				Users_print(users);
			break;
				
			case 2:
				printf("By which parameter do you want to sort:\n\n\
  1. By name \n\
  2. By surname\n\
  3. By age\n\
  4. By salary\n\
  5. By ID\n\n\
Select an action: ");
				scanf("%d", &stage_sort);
				printf("\n");
				switch(stage_sort)
				{	
					case 1:
						List_sort_char(users, stage, stage_sort);
						Users_print(users);
					break;
						
					case 2:
						List_sort_char(users, stage, stage_sort);
						Users_print(users);
					break;
						
					case 3:
						List_sort_int(users, stage, stage_sort);
						Users_print(users);
					break;
						
					case 4:
						List_sort_int(users, stage, stage_sort);
						Users_print(users);
					break;
						
					case 5:
						List_sort_id(users);
						Users_print(users);
					break;
				}
			break;
				
			case 3:
				printf("By which parameter do you want to sort:\n\
  1. By name \n\
  2. By surname\n\
  3. By age\n\
  4. By salary\n\
Select an action: ");
				scanf("%d", &stage_sort);
				printf("\n");
				switch(stage_sort)
				{
					case 1:
						List_sort_char(users, stage, stage_sort);
						Users_print(users);
						break;
						
					case 2:
						List_sort_char(users, stage, stage_sort);
						Users_print(users);
						break;
						
					case 3:
						List_sort_int(users, stage, stage_sort);
						Users_print(users);
						break;
						
					case 4:
						List_sort_int(users, stage, stage_sort);
						Users_print(users);
						break;
				}
			break;
				
			case 4:
				printf("By which parameter do you want to filter:\n\
  1. By ID\n\
  1. By name \n\
  2. By surname\n\
  4. By age\n\
  5. By salary \n\n\
Select an action: ");
				scanf("%d", &stage_sort);
				printf("\n");
				switch(stage_sort)
				{
					case 1:
						printf("Enter the ID:");
						scanf("%d", &id);
						printf("\n");
						List_filter_int(users, stage_sort, id);
						break;
						
					case 2:
						printf("Enter a name:");
						scanf("%s", &name);
						printf("\n");
						List_filter_char(users, stage_sort, name);
						break;
						
					case 3:
						printf("Enter your surname:");
						scanf("%s", &surname);
						printf("\n");
						List_filter_char(users, stage_sort, surname);
						break;
						
					case 4:
						printf("Enter the age:");
						scanf("%d", &age);
						printf("\n");
						List_filter_int(users, stage_sort, age);
						break;
						
					case 5:
						printf("Enter the salary amount:");
						scanf("%f", &sallary);
						printf("\n");
						List_filter_float(users, stage_sort, sallary);
						break;
				}
			break;
				
			case 5:
				printf("Enter a name: ");
				scanf("%s", &name);
				printf("Enter a surname: ");
				scanf("%s", &surname);
				printf("Enter an age: ");
				scanf("%d", &age);
				printf("Enter a sallary: ");
				scanf("%f", &sallary);
				printf("\n");
				List_sort_id(users);
				int count = 1;
				short check = 1;
				for(int i = 0; i < List_count(users); i++){
					User* user = List_at(users, i);
					if (count != user->id && check == 1) {
						id = count;
						check = 0;
					}
					count++;
				}
				if (check == 1) id = List_count(users)+1;
				List_push(users, User_create(id, name, surname, age, sallary));
				printf("The user has been added to the table.\n");
			break;
				
			case 6:
				printf("Enter the number of the user you want to delete:");
				scanf("%d", &id);
				printf("\n");
				List_sort_id(users);
				check = 0;
				for(int i = 0; i < List_count(users); i++){
					User* user = List_at(users, i);
					if (user->id == id) check = 1;
				}
				if (check == 1) {
					List_delete_pos(users, id-1);
					printf("The user has been removed from the table.\n");
				}
				else printf("The user was not found.\n");
			break;
				
			case 7:
				Users_load_file(users, fptr);
				List_sort_id(users);
				printf("The data has been successfully downloaded from the file 'DATA.txt '.\n");
			break;
				
			case 8:
				List_sort_id(users);
				Users_save_file(users, fptr);
				printf("The data has been successfully saved to a file 'DATA.txt '\n");
			break;
		}
		printf("\nSelect an action:");
	}
	return 0;
}
