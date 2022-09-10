#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include <inttypes.h>
#define MAX_STR_LENGTH 257

// print function for all 3 data types that use pointer arithmetic
// based on the size of every component of the current data piece
void print_type_3(void *p)
{
	void *tmp = p;
	char *name1 = (char *)tmp;
	tmp += strlen(name1) + 1;
	int32_t money1 = *((int32_t *)tmp);
	tmp += sizeof(int32_t);
	int32_t money2 = *((int32_t *)tmp);
	tmp += sizeof(int32_t);
	char *name2 = (char *)tmp;
	printf("%s pentru %s\n", name1, name2);
	printf("%"PRId32"\n",money1);
	printf("%"PRId32"\n",money2);
}

void print_type_2(void *p)
{
	void *tmp = p;
	char *name1 = (char *)tmp;
	tmp += strlen(name1) + 1;
	int16_t money1 = *((int16_t *)tmp);
	tmp += sizeof(int16_t);
	int32_t money2 = *((int32_t *)tmp);
	tmp += sizeof(int32_t);
	char *name2 = (char *)tmp;
	printf("%s pentru %s\n", name1, name2);
	printf("%"PRId32"\n",money1);
	printf("%"PRId32"\n",money2);
}

void print_type_1(void *p)
{
	void *tmp = p;
	char *name1 = (char *)tmp;
	tmp += strlen(name1) + 1;
	int8_t money1 = *((int8_t *)tmp);
	tmp += sizeof(int8_t);
	int8_t money2 = *((int8_t *)tmp);
	tmp += sizeof(int8_t);
	char *name2 = (char *)tmp;
	printf("%s pentru %s\n", name1, name2);
	printf("%"PRId32"\n",money1);
	printf("%"PRId32"\n",money2);
}

// printing the info for the data located at address
// at pointer p
void print_info(void *p) 
{
	head *header = (head *)p;
	printf("Tipul %c\n", header->type);
	p += sizeof(head);
	if (header->type == '1')
		print_type_1(p);
	else if (header->type == '2')
		print_type_2(p);
	else
		print_type_3(p);
	
	
	printf("\n");
}

// printing the data at a specific index from the data array
void find(void *data_block, int len, int index) 
{
	// if the index is not valid we return
	if (index < 0 || index >= len) {
		printf("%s\n", "The given index is invalid");
		return;
	}
	
	// iterating through the array
	void *p = data_block;
	for (int i = 0; i < index; i++) {
		head *header = (head *)p;
		p = p + header->len;
	}

	// calling the print function for the current data piece
	print_info(p);
}

// adding a new element at the end of the array
int add_last(void **arr, int *len, data_structure *data)
{
	// iterating through the array and calculating the current size
	// for realloc
	unsigned int sz = 0;
	void *p = *arr;
	for (int i = 0 ; i < *len; i++) {
		head *header = (head *)p;
		sz += header->len;
		p = p + header->len;
	}

	// reallocating the array with the new size and testing if the operation
	// completed without any errors
	unsigned int new_sz = data->header->len;
	void *tmp = realloc(*arr, sz + new_sz);

	if (!tmp) {
		fprintf(stderr, "Realloc failed\n");
		return -2;
	}

	*arr = tmp;

	// copying the new data at the end of the array and incrementing the length
	memcpy(*arr + sz, data->header, sizeof(head));
	memcpy(*arr + sz + sizeof(head), data->data, data->header->len - sizeof(head));
	*len = *len + 1;

	return 0;
}

// adding a new element at a specific index in the array
// or at the end if the index is greater than len
int add_at(void **arr, int *len, data_structure *data, int index)
{
	// if the index is invalid we return an error
	if (index < 0) {
		printf("%s\n", "The given index is invalid");
		return -1;
	}

	// iterating through the array and calculating the size of elements
	// before index and after index
	unsigned int szl = 0, szr = 0;
	void *p = *arr;
	for (int i = 0 ; i < *len; i++) {
		head *header = (head *)p;
		if (i < index)
			szl += header->len;
		else
			szr += header->len;
		
		p = p + header->len;
	}

	// reallocating the array with the new size and testing if the operation
	// completed without any errors
	unsigned int new_sz = data->header->len;
	void *tmp = realloc(*arr, szl + szr + new_sz);

	if (!tmp) {
		fprintf(stderr, "Realloc failed\n");
		return -2;
	}

	*arr = tmp;

	// shifting the elements after index to the right and copying
	// the new data at the given index
	void *tmp1 = malloc(szr);
	memcpy(tmp1, *arr + szl, szr);
	memcpy(*arr + szl + new_sz, tmp1, szr);
	memcpy(*arr + szl, data->header, sizeof(head));
	memcpy(*arr + szl + sizeof(head), data->data, data->header->len - sizeof(head));
	free(tmp1);
	*len = *len + 1;

	return 0;
}

// deleting the element at a given index in the array
int delete_at(void **arr, int *len, int index)
{
	// if the index is invalid we return an error
	if (index < 0) {
		printf("%s\n", "The given index is invalid");
		return -1;
	}

	// iterating through the array and calculating the size of elements
	// before index and after index
	unsigned int szl = 0, szr = 0;
	void *p = *arr;
	for (int i = 0 ; i < *len; i++) {
		head *header = (head *)p;
		if (i < index)
			szl += header->len;
		else
			szr += header->len;
		
		p = p + header->len;
	}

	// deleting the element at position index and shifting
	// the elements after index to the left
	p = *arr + szl;
	head *header = (head *)p;
	void *next = p + sizeof(head);
	if (header->type == '1')
		next += 2 * sizeof(int8_t) + sizeof(char *);
	else if (header->type == '2')
		next += sizeof(int16_t) + sizeof(int32_t) + sizeof(char *);
	else 
		next += 2 * sizeof(int32_t) + sizeof(char *);
	
	unsigned int sz = header->len;
	void *tmp = malloc(szr - header->len);
	memcpy(tmp, p + header->len, szr - sz);
	memcpy(p, tmp, szr - sz);
	free(tmp);

	// reallocating the array with the new size and testing if the operation
	// completed without any errors
	void *tmp1 = realloc(*arr, szl + szr - sz + 1);
	if (!tmp1) {
		fprintf(stderr, "Realloc failed\n");
		return -2;
	}

	*arr = tmp1;
	*len = *len - 1;

	return 0;
}

// printing the information for every piece of data in the array
void print(void *arr, int len)
{
	void *p = arr;
	for (int i = 0 ; i < len; i++) {
		print_info(p);
		head *header = (head *)p;
		p = p + header->len;
	}
}

// freeing all the used memory from the program
void free_all(void **arr, int len, char **command)
{
	free(*arr);
	free(*command);
	*arr = NULL;
	*command = NULL;
}

// parsing the input and building the data_structure element for
// adding the data to the array
data_structure* build_data(char *p)
{
	// allocating memory for the new data
	data_structure *add = malloc(sizeof(data_structure));
	if (!add) {
		fprintf(stderr, "Malloc failed\n");
		return NULL;
	}
	add->header = malloc(sizeof(head));
	if (!add->header) {
		fprintf(stderr, "Malloc failed\n");
		return NULL;
	}
	add->header->len = 0;

	// reading the data type
	p = strtok(NULL, " ");
	add->header->type = p[0];

	// reading the first name
	p = strtok(NULL, " ");
	char *name1;
	name1 = strdup(p);
	if (!name1) {
		fprintf(stderr, "Malloc failed\n");
		free(name1);
		return NULL;
	}
	add->header->len += strlen(name1) + 1;

	// reading the integer types for the money variables depending on
	// the data type and the second name and copying the data in the new
	// structure
	p = strtok(NULL, " ");
	if (add->header->type == '1') {
		int8_t money8_1 = (int8_t)atoi(p);
		p = strtok(NULL, " ");
		int8_t money8_2 = (int8_t)atoi(p);

		add->header->len += 2 * sizeof(int8_t);

		char *name2;
		p = strtok(NULL, " ");
		p[strlen(p) - 1] = '\0';
		name2 = strdup(p);
		if (!name2) {
			fprintf(stderr, "Malloc failed\n");
			free(name1);
			return NULL;
		}

		add->header->len += strlen(name2) + 1;
		add->data = malloc(add->header->len);
		if (!add->data) {
			fprintf(stderr, "Malloc failed\n");
			free(name1);
			free(name2);
			return NULL;
		}

		memcpy(add->data, name1, strlen(name1) + 1);
		memcpy(add->data + strlen(name1) + 1, &money8_1, sizeof(int8_t));
		memcpy(add->data + strlen(name1) + 1 + sizeof(int8_t), &money8_2, sizeof(int8_t));
		memcpy(add->data + strlen(name1) + 1 + 2 * sizeof(int8_t), name2, strlen(name2) + 1);
		free(name2);

	} else if (add->header->type == '2') {
		int16_t money16_1 = (int16_t)atoi(p);
		p = strtok(NULL, " ");
		int32_t money32_2 = (int32_t)atoi(p);

		add->header->len += sizeof(int16_t) + sizeof(int32_t);

		char *name2;
		p = strtok(NULL, " ");
		p[strlen(p) - 1] = '\0';
		name2 = strdup(p);
		if (!name2) {
			fprintf(stderr, "Malloc failed\n");
			free(name1);
			return NULL;
		}

		add->header->len += strlen(name2) + 1;
		add->data = malloc(add->header->len);
		if (!add->data) {
			fprintf(stderr, "Malloc failed\n");
			free(name1);
			free(name2);
			return NULL;
		}

		memcpy(add->data, name1, strlen(name1) + 1);
		memcpy(add->data + strlen(name1) + 1, &money16_1, sizeof(int16_t));
		memcpy(add->data + strlen(name1) + 1 + sizeof(int16_t), &money32_2, sizeof(int32_t));
		memcpy(add->data + strlen(name1) + 1 + sizeof(int16_t) + sizeof(int32_t), name2, strlen(name2) + 1);
		free(name2);
	} else {
		int32_t money32_1 = (int32_t)atoi(p);
		p = strtok(NULL, " ");
		int32_t money32_2 = (int32_t)atoi(p);

		add->header->len += 2 * sizeof(int32_t);

		char *name2;
		p = strtok(NULL, " ");
		p[strlen(p) - 1] = '\0';
		name2 = strdup(p);
		if (!name2) {
			fprintf(stderr, "Malloc failed\n");
			free(name1);
			return NULL;
		}

		add->header->len += strlen(name2) + 1;
		add->data = malloc(add->header->len);
		if (!add->data) {
			fprintf(stderr, "Malloc failed\n");
			free(name1);
			free(name2);
			return NULL;
		}

		memcpy(add->data, name1, strlen(name1) + 1);
		memcpy(add->data + strlen(name1) + 1, &money32_1, sizeof(int32_t));
		memcpy(add->data + strlen(name1) + 1 + sizeof(int32_t), &money32_2, sizeof(int32_t));
		memcpy(add->data + strlen(name1) + 1 + 2 * sizeof(int32_t), name2, strlen(name2) + 1);
		free(name2);
	}
	free(name1);

	add->header->len += sizeof(head);
	return add;
}

int main() {

	void *arr = NULL;
	int len = 0;
	char *command = malloc(MAX_STR_LENGTH);
	if (!command) {
		fprintf(stderr, "Malloc failed\n");
		return -1;
	}
	fgets(command, MAX_STR_LENGTH, stdin);

	while (1) {
		char *p = strtok(command, " ");

		if (strcmp(p, "insert") == 0) {
			data_structure *add = build_data(p);
			if (!add) {
				free_all(&arr, len, &command);
				break;
			}
			int check = add_last(&arr, &len, add);
			free(add->data);
			free(add->header);
			free(add);
			if (check == -2) {
				free_all(&arr, len, &command);
				break;
			}
		} else if (strcmp(p, "find") == 0) {
			p = strtok(NULL, " ");
			int idx = atoi(p);
			find(arr, len, idx);
		} else if (strncmp(p, "print", 5) == 0) {
			print(arr, len);
		} else if (strncmp(p, "exit", 4) == 0) {
			free_all(&arr, len, &command);
			break;
		} else if (strcmp(p, "insert_at") == 0) {
			p = strtok(NULL, " ");
			int index = atoi(p);
			data_structure *add = build_data(p);
			if (!add) {
				free_all(&arr, len, &command);
				break;
			}
			int check = add_at(&arr, &len, add, index);
			free(add->data);
			free(add->header);
			free(add);
			if (check == -2) {
				free_all(&arr, len, &command);
				break;
			} 
		} else if (strcmp(p, "delete_at") == 0) {
			p = strtok(NULL, " ");
			int index = atoi(p);
			delete_at(&arr, &len, index);
		}
		fgets(command, MAX_STR_LENGTH, stdin);
	}

	return 0;
}

