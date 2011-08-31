/* Minesweeper. Attempt.
 * Isn't it exciting?
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define ROWS 10
#define COLS 10

int game[ROWS][COLS];
int mines, clean, i, j;

void printBoard();
int takeTurn();
int checkSpot(int row, int col);
int checkAround(int row, int col, int opt);

int main(void) {
	srand(time(0));
	for(i = 0; i < ROWS * COLS; i++) {
		game[i/COLS][i%COLS] = -1;
	}
	do {
		printf("How many mines would you like? ");
		scanf("%d", &mines);
	} while(mines < 1);
	clean = (ROWS * COLS) - mines;
	while(takeTurn()) {}
	printf(clean > 0 ? "Game Over!\n" : "WOOT!\n");

	return 0;
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
			if(game[i][j] > 0) {
				printf("[%d]", game[i][j]);
			} else {
				printf(game[i][j] < 0 ? "[*]" : "   ");
			}
		}
		printf("\n");
	}
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
	if(mines > 0) {
		game[row][col] = -2;
		while(mines > 0) {
			i = rand()%ROWS;
			j = rand()%COLS;
			if(game[i][j] != -2) {
				game[i][j] = -2;
				mines--;
			}
		}
		game[row][col] = -1;
	}
	return checkSpot(row, col);
}

int checkSpot(int row, int col) {
	if(game[row][col] == -2) {
		return 0;
	} else if(game[row][col] == -1) {
		game[row][col] = checkAround(row, col, 0);
		if(game[row][col] == 0) {
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
			if(opt == 0 && game[y][x] == -2) {
				total++;
			} else if(opt == 1 && game[y][x] == -1) {
				checkSpot(y, x);
			}
		}
	}
	return total;
}
