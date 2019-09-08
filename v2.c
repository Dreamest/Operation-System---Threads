#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "v2.h"

int result = 1;
int counter = 0;
int threads_finished = THREADS_N;
Sudoku S;
Task bank[27];
pthread_mutex_t lock_tasks;
pthread_mutex_t lock_while;
pthread_mutex_t condLock;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

int main(int argc, char* argv[])
{
	int i, j, fileFlag = 0, rc = 0;
	FILE* fp;
	char* filename;
	char buffer[100];
	pthread_t threads[THREADS_N];

	if (argc > 2)
	{
		printf("provide only one file argument\n");
		exit(0);
	}

	if (argc < 2)
	{
		fileFlag = 1;
		printf("Enter file name:\n");
		fgets(buffer, 100, stdin);
		filename = strtok(buffer, "\n");
		fp = fopen(filename, "r");
	}


	if (fileFlag == 0)
	{
		fp = fopen(argv[1], "r");
		filename = argv[1];
	}

	if (fp)
		for (i = 0; i < ROWS * COLS; i++)
			fscanf(fp, "%d", &S.fields[i]);
	else
	{
		printf("Failed to open the file\n");
		return 0;
	}

	//populate bank for tasks
	for (i = 0; i < 3; i++)
		for (j = 0; j < 9; j++)
			bank[j + i * 9].offset = j;

	//task lock initialization
	if (pthread_mutex_init(&lock_tasks, NULL) != 0)
	{
		printf("\nlock init has failed\n");
		exit(-1);
	}	//task lock initialization

	if (pthread_mutex_init(&lock_while, NULL) != 0)
	{
		printf("\nlock init has failed\n");
		exit(-1);
	}

	if (pthread_mutex_init(&condLock, NULL) != 0)
	{
		printf("\nlock init has failed\n");
		exit(-1);
	}

	

	//N threads initialization
	for (i = 0; i < THREADS_N; i++)
	{
		rc = pthread_create(&threads[i], NULL, taskExecution, NULL);

		if (rc != 0)
		{
			printf("Error: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}


	pthread_mutex_lock(&condLock);
	pthread_cond_wait(&condition, &condLock);
	pthread_mutex_unlock(&condLock);

	//overall verdict
	if (result == 0)
		printf("solution is not legal\n");
	else
		printf("solution is legal\n");

	//N threads join
	for (i = 0; i < THREADS_N; i++)
		pthread_join(threads[i], NULL);

	pthread_mutex_destroy(&lock_tasks);
	pthread_mutex_destroy(&lock_while);
	pthread_mutex_destroy(&condLock);

	exit(1);
}

void* checkRows(void* row)
{
	int j;
	int r = *(int*) row;
	int check[9] = { 0 };

	for (j = 0; j < COLS; ++j)
	{
		if (S.fields[r * ROWS + j] >= 1 && S.fields[r * ROWS + j] <= 9)
		{
			check[(S.fields[r * ROWS + j]) - 1] = 1;
		} else
			return (void*) -1; //faulty information
	}

	for (j = 0; j < 9; ++j)
		if (check[j] != 1)
			return (void*) 0; //incorrect information

	return (void*) 1;  //valid info
}

void* checkCols(void* column)
{
	int j;
	int c = *(int*) column;
	int check[9] = { 0 };

	for (j = 0; j < COLS; ++j)
	{
		if (S.fields[c + j * COLS] >= 1 && S.fields[c + j * COLS] <= 9)
			check[S.fields[c + j * COLS] - 1] = 1;
		else {
			printf("%d\n", S.fields[c + j * COLS]);
			return (void*) -1; //invalid character
		}
	}

	for (j = 0; j < COLS; ++j)
	{
		if (check[j] != 1)
			return (void*) 0; //incorrect Sudoku
	}

	return (void*) 1; //valid result
}

void* checkBoxes(void* box)
{
	int j, k;
	int b = *(int*) box;
	int check[BOX * BOX] = { 0 };
	int mult = 0;

	if (b >= 3 && b <= 5)
		mult = 18;
	else if (b >= 6 && b <= 8)
		mult = 36;

	for (j = 0; j < BOX; ++j)
	{
		for (k = 0; k < BOX; ++k)
		{
			if (S.fields[b * BOX + mult + ROWS * j + k] >= 1
					&& S.fields[b * BOX + mult + ROWS * j + k] <= 9)
				check[S.fields[b * BOX + mult + ROWS * j + k] - 1] = 1;
			else
				return (void*) -1; //invalid character
		}
	}

	for (j = 0; j < COLS; ++j)
		if (check[j] != 1)
			return (void*) 0; //incorrect Sudoku

	return (void*) 1; //valid solution
}

void* taskExecution()
{
	Task task_current;
	int task_index;
	void* res;

	while (counter < TASKS && result == 1)
	{
		pthread_mutex_lock(&lock_tasks);
		task_current = bank[counter];
		task_index = counter;
		counter++;
		pthread_mutex_unlock(&lock_tasks);

		res = tasks[task_index % 3]((void*) &task_current.offset);

		if (res != (void*) 1)
			result = 0;
	}

	pthread_mutex_lock(&lock_while);
	threads_finished--;
	pthread_mutex_unlock(&lock_while);

	if (threads_finished == 0)
	{
		pthread_mutex_lock(&condLock);
		pthread_cond_signal(&condition);
		pthread_mutex_unlock(&condLock);
	}

	return NULL;
}

