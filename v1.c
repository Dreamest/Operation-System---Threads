#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "v1.h"

Sudoku S;
void* PT_RESULTS[27] = {0};

int main(int argc, char* argv[])
{
	int i, rc, fileFlag = 0;
	FILE* fp;
	char* filename;
	char buffer[100];
	pthread_t threads[THREADS];

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

	for (i = 0; i < 27; i++)
	{
		rc = pthread_create(&threads[i], NULL, functionDecider, (void*) &i);

		if (rc != 0)
		{
			printf("Error: return code from pthread_create() is %d\n", rc);
			exit(0);
		}
	}

	for (i = 0; i < 27; i++)
	{
		pthread_join(threads[i], &PT_RESULTS[i]);

		if (PT_RESULTS[i] != (void*) 1)
		{
			printf("solution is not legal\n");
			exit(0);
		}
	}

	printf("solution is legal\n");
	exit(1);
}

void* checkRows(int r)
{
	int j;
	int check[9] = { 0 };

	for (j = 0; j < COLS; ++j)
	{
		if (S.fields[r * ROWS + j] >= 1 && S.fields[r * ROWS + j] <= 9)
			check[(S.fields[r * ROWS + j]) - 1] = 1;
		else
			return (void*) -1; //faulty information
	}

	for (j = 0; j < 9; ++j)
		if (check[j] != 1)
			return (void*) 0; //incorrect information

	return (void*) 1;  //valid info
}

void* checkCols(int c)
{
	int j;
	int check[9] = { 0 };

	for (j = 0; j < COLS; ++j)
	{
		if (S.fields[c + j * COLS] >= 1 && S.fields[c + j * COLS] <= 9)
			check[S.fields[c + j * COLS] - 1] = 1;
		else
		{
			printf("%d\n", S.fields[c + j * COLS]);
			return (void*) -1; //invalid character
		}
	}

	for (j = 0; j < COLS; ++j)
		if (check[j] != 1)
			return (void*) 0; //incorrect Sudoku

	return (void*) 1; //valid result
}

void* checkBoxes(int b)
{
	int j, k;
	int check[BOX * BOX] = {0};
	int mult = 0;

	if (b >= 3 && b <= 5)
		mult = 18;
	else if (b >= 6 && b <= 8)
		mult = 36;

	for (j = 0; j < BOX; ++j)

		for (k = 0; k < BOX; ++k)
		{
			if (S.fields[b * BOX + mult + ROWS * j + k] >= 1
					&& S.fields[b * BOX + mult + ROWS * j + k] <= 9)
				check[S.fields[b * BOX + mult + ROWS * j + k] - 1] = 1;
			else
				return (void*) -1; //invalid character
		}

	for (j = 0; j < COLS; ++j)
		if (check[j] != 1)
			return (void*) 0; //incorrect Sudoku

	return (void*) 1; //valid solution
}

void* functionDecider(void* i)
{
	int j = *(int*)i;
	int function = j % 3;
	int offset = j % 9;
	void* result;

	switch (function)
	{
		case 0:
			result = checkRows(offset);
			break;
		case 1:
			result = checkCols(offset);
			break;
		case 2:
			result = checkBoxes(offset);
			break;
	}

	return result;
}
