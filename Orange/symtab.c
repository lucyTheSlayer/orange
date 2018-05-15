#include "symtab.h"

#define SHIFT 4

void st_init(Symtab* symtab) {
	if (symtab == NULL) {
		return;
	}
	for (int i = 0; i < SIZE; i++) {
		symtab->hashTable[i] = NULL;
	}
}

static int hash(char* key) {
	int temp = 0;
	int i = 0;
	while (key[i] != '\0') {
		temp = ((temp << SHIFT) + key[i]) % SIZE;
		++i;
	}
	return temp;
}

void st_insert(Symtab* symtab, char* name, int lineno, int loc) {
	int h = hash(name);
	BucketList l = symtab->hashTable[h];
	while (l != NULL && strcmp(name, l->name) != 0) {
		l = l->next;
	}
	if (l == NULL) {
		l = (BucketList)malloc(sizeof(struct BucketListRec));
		l->name = name;
		l->lines = (LineList)malloc(sizeof(struct LineListRec));
		l->lines->lineno = lineno;
		l->memloc = loc;
		l->lines->next = NULL;
		l->next = symtab->hashTable[h];
		symtab->hashTable[h] = l;
	}
	else {
		LineList t = l->lines;
		while (t->next != NULL) {
			t = t->next;
		}
		t->next = (LineList)malloc(sizeof(struct LineListRec));
		t->next->lineno = lineno;
		t->next->next = NULL;
	}
}
int st_lookup(Symtab* symtab, char* name) {
	int h = hash(name);
	BucketList l = symtab->hashTable[h];
	while (l != NULL && strcmp(name, l->name) != 0) {
		l = l->next;
	}
	if (l == NULL) {
		return -1;
	}
	else {
		return l->memloc;
	}
}

void printSymTab(Symtab* symtab) {
	int i;
	printf("Variable Name  Location  Line Numbers\n\n");
	for (i = 0; i < SIZE; ++i) {
		if (symtab->hashTable[i] != NULL) {
			BucketList l = symtab->hashTable[i];
			while (l != NULL) {
				LineList t = l->lines;
				printf("%-14s ", l->name);
				printf("%-8d  ", l->memloc);
				while (t != NULL) {
					printf("%4d ", t->lineno);
					t = t->next;
				}
				printf("\n");
				l = l->next;
			}
		}
	}
}

