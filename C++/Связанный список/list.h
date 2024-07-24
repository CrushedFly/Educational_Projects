#ifndef LIST_H
#define LIST_H

struct ListElem {
	void* pObj;
	struct ListElem* next;
} typedef ListElem;

struct List {
	ListElem* first;
} typedef List;

List* List_create() { //создание новой таблицы
	List* list = (List*)malloc(sizeof(List));
	list->first = NULL;
	return list;
}

void List_delete(List* const list) { //полное удаление таблицы
	free(list);
}

void List_push(List* const list, void* const object) { //добавление пользователя (узла) в конец таблицы (односвязного списка)
	ListElem* elem = (ListElem*)malloc(sizeof(ListElem));
	elem->pObj = object;
	elem->next = NULL;
	
	if(list->first==NULL) {
		list->first = elem;
		return;
	}
	
	ListElem* current = list->first;
	while(current->next != NULL) {
		current = current->next;
	}
	current->next = elem;
}

size_t List_count(List* const list) { //подсчёт количества пользователей (узлов) в таблице (списке)
	if(list->first==NULL) {
		return 0;
	}
	
	size_t count = 1;
	ListElem* current = list->first;
	while(current->next != NULL) {
		current = current->next;
		count++;
	}
	return count;
}

void* List_at(List* const list, size_t pos) //выдача данных о пользователе с заданным ID
{
	if(list->first==NULL) {
		return NULL;
	}
	
	ListElem* current = list->first;
	while(current->next != NULL && pos > 0) {
		current = current->next;
		pos--;
	}
	return current->pObj;
}

void* List_delete_pos(List* const list, size_t pos) //удаление пользователя с заданным ID
{
	if(list->first==NULL) {
		return NULL;
	}
	
	ListElem* prev = list->first;
	ListElem* current = prev->next;
	
	if(current == NULL) {
		free(prev);
		list->first = NULL;
		return NULL;
	}
	
	while(current->next != NULL && pos-1 > 0) {
		prev = prev->next;
		current = current->next;
		pos--;
	}
	prev->next = current->next;
	free(current);
}

void* List_swap(List* const list, size_t pos) //перестановка соседних пользователей (узлов)
{
	if(list->first==NULL) {
		return NULL;
	}
	
	ListElem* prev = list->first;
	ListElem* current = prev->next;
	
	if(current==NULL) {
		return NULL;
	}
	else if(List_count(list) == 2 || pos-1 == 1) {
		prev->next = current->next;
		current->next = list->first;
		list->first = current;
	}
	else {
		ListElem* prev_2 = list->first;
		ListElem* prev_1 = prev_2->next;
		ListElem* current = prev_1->next;
		
		while(current->next != NULL && pos-3 > 0) {
			prev_2 = prev_2->next;
			prev_1 = prev_1->next;
			current = current->next;
			pos--;
		}
		
		prev_2->next = prev_1->next;
		prev_1->next = current->next;
		current->next = prev_1;
	}	
}
#endif
