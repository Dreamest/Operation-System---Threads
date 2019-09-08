#ifndef V2_H_
#define V2_H_

#define COLS 9
#define ROWS 9
#define BOX 3
#define TASKS 27
#define THREADS_N 5

typedef struct {
	int fields[COLS * ROWS];
}Sudoku;

typedef struct {
	int offset;
	int func;
}Task;

void* checkRows(void* row);
void* checkCols(void* column);
void* checkBoxes(void* box);
void* taskExecution();

void*(*tasks[3])(void*) = {checkRows, checkCols, checkBoxes};

#endif /* V2_H_ */
