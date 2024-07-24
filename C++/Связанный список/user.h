#ifndef USER_H
#define USER_H

#define USER_NAME_LEN 50
#define TELEPHONE_LEN 15

#pragma pack(push, 1)  
struct {
	int   id;
	char  name[USER_NAME_LEN];
	char  surname[USER_NAME_LEN];
	int   age;
	float sallary;
} typedef User;
#pragma pack(pop)

User* User_create(int id, char *name, char *surname, int age, float sallary){ //заполнение данными нового пользователя
	
	User* user = (User*)malloc(sizeof(User));
	user->id = id;
	strcpy(user->name, name);
	strcpy(user->surname, surname);
	user->age = age;
	user->sallary = sallary;
	
	return user;
}

#endif
