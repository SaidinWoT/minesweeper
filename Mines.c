/* Minesweeper. Attempt.
 * Isn't it exciting?
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define ROWS 10
#define COLS 10
#define loc(y,x) game->board[y][x]

typedef struct board {
	int board[ROWS][COLS];
	int mines;
} board;

int mines, clean, i, j;
board* game;

board* createBoard();
int takeTurn();
void printBoard();
int checkSpot(int row, int col);
int checkAround(int row, int col, int opt);
int main(void) {
	srand(time(0));
	game = createBoard();
	printf("How many mines would you like? ");
	scanf("%d", &mines);
	clean = (ROWS * COLS) - mines;
	while(takeTurn()) {}
	printf(clean > 0 ? "Game Over!\n" : "WOOT!\n");

	free(game);
	return 0;
}

board* createBoard() {
	board* newBoard = (board*)malloc(sizeof(board));
	for(i = 0; i < ROWS * COLS; i++) {
		newBoard->board[i/COLS][i%COLS] = -1;
	}
	newBoard->mines = 0;
	return newBoard;
}

int takeTurn() {
	int row, col;
	printBoard();
	do {
		printf("Select a row to check: ");
		scanf("%d", &row);
	} while(row > ROWS - 1);
	do {
		printf("Select a column to check: ");
		scanf("%d", &col);
	} while(col > COLS - 1);
	if(game->mines < mines) {
		loc(row, col) = -2;
		while(game->mines < mines) {
			i = rand()%ROWS;
			j = rand()%COLS;
			if(loc(i, j) != -2) {
				loc(i, j) = -2;
				game->mines++;
			}
		}
		loc(row, col) = -1;
	}
	return checkSpot(row, col);
}

void printBoard() {
	int i, j;
	printf("   ");
	for(j = 0; j < COLS; j++) {
		printf("%2d ", j);
	}
	printf("\n");
	for(i = 0; i < ROWS; i++) {
		printf("%2d ", i);
		for(j = 0; j < COLS; j++) {
			if(loc(i, j) > 0) {
				printf("[%d]", loc(i, j));
			} else {
				printf(loc(i, j) < 0 ? "[*]" : "   ");
			}
		}
		printf("\n");
	}
}

int checkSpot(int row, int col) {
	if(loc(row, col) == -2) {
		return 0;
	} else if(loc(row, col) == -1) {
		loc(row, col) = checkAround(row, col, 0);
		if(loc(row, col) == 0) {
			checkAround(row, col, 1);
		}
		if(--clean <= 0) {
			return 0;
		}
	}
	return 1;
}

int checkAround(int row, int col, int opt) {
	int y, x, total = 0;
	for(y = row < 1 ? 0 : row - 1; y <= ( row == ROWS - 1 ? ROWS - 1 : row + 1); y++) {
		for(x = col < 1 ? 0 : col - 1; x <= (col == COLS - 1 ? COLS - 1 : col + 1); x++) {
			if(opt == 0 && loc(y, x) == -2) {
				total++;
			} else if(opt == 1 && loc(y, x) == -1) {
				checkSpot(y, x);
			}
		}
	}
	return total;
}
