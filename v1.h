#ifndef V1_H_
#define V1_H_

#define COLS 9
#define ROWS 9
#define BOX 3
#define THREADS 27

typedef struct {
	int fields[COLS*ROWS];
}Sudoku;

void* checkRows(int r);
void* checkCols(int c);
void* checkBoxes(int b);
void* functionDecider(void* i);

#endif /* V1_H_ */
