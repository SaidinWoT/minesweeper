/* Minesweeper. Attempt.
 * Isn't it exciting?
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define ROWS 10
#define COLS 10

typedef struct board {
	int board[ROWS][COLS];
	int mines;
} board;

board* createBoard();
void populate(board* game, int mines);
void printBoard(board* game);
int checkSpot(board* game, int row, int col);
void firstTurn(board* game, int mines);
int takeTurn(board* game);

int mines, clean;

int main(void) {
	srand(time(0));
	board* game = createBoard();
	printf("How many mines would you like? ");
	scanf("%d", &mines);
	clean = (ROWS * COLS) - mines;
	firstTurn(game, mines);
	while(takeTurn(game)) {}
	if(clean > 0) {
		printf("Game Over!\n");
	} else {
		printf("WOOT!\n");
	}
}

board* createBoard() {
	board* newBoard = (board*)malloc(sizeof(board));
	int i, j;
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS; j++) {
			newBoard->board[i][j] = -1;
		}
	}
	newBoard->mines = 0;
	return newBoard;
}

void firstTurn(board* game, int mines) {
	int row, col;
	printBoard(game);
	printf("Select a row to check: ");
	scanf("%d", &row);
	printf("Select a column to check: ");
	scanf("%d", &col);
	game->board[row][col] = -2;
	populate(game, mines);
	game->board[row][col] = -1;
	checkSpot(game, row, col);
}

void populate(board* game, int mines) {
	while(game->mines < mines) {
		int posR = rand()%ROWS;
		int posC = rand()%COLS;
		if(game->board[posR][posC] != -2) {
			game->board[posR][posC] = -2;
			game->mines++;
		}
	}
}

void printBoard(board* game) {
	int i, j;
	printf("   ");
	for(j = 0; j < COLS; j++) {
		printf("%2d ", j);
	}
	printf("\n");
	for(i = 0; i < ROWS; i++) {
		printf("%2d ", i);
		for(j = 0; j < COLS; j++) {
			if(game->board[i][j] < 0) {
				printf("[*]");
			} else if(game->board[i][j] == 0) {
				printf("   ");
			} else {
				printf("[%d]", game->board[i][j]);
			}
		}
		printf("\n");
	}
}

int checkSpot(board* game, int row, int col) {
	if(game->board[row][col] == -2) {
		return 0;
	}
	if(game->board[row][col] == -1) {
		game->board[row][col] = 0;
		int i, j;
		for(i = (row - 1); i <= (row + 1); i++) {
			if(i < 0) {
				i = 0;
			} else if(i == ROWS) {
				break;
			}
			for(j = col - 1; j <= col + 1; j++) {
				if(j < 0) {
					j = 0;
				} else if(j == COLS) {
					break;
				}
				if(game->board[i][j] == -2) {
					game->board[row][col]++;
				}
			}
		}
		if(game->board[row][col] == 0) {
			for(i = row - 1; i <= row + 1; i++) {
				if(i < 0) {
					i = 0;
				} else if(i == ROWS) {
					break;
				}
				for(j = col - 1; j <= col + 1; j++) {
					if(j < 0) {
						j = 0;
					} else if(j == COLS) {
						break;
					}
					if(game->board[i][j] == -1) {
						checkSpot(game, i, j);	
					}
				}
			}
		}
		clean--;
		if(clean <= 0) {
			return 0;
		}
	}
	return 1;
}

int takeTurn(board* game) {
	int row, col;
	printBoard(game);
	printf("Select a row to check: ");
	scanf("%d", &row);
	printf("Select a column to check: ");
	scanf("%d", &col);
	if(checkSpot(game, row, col)) {
		return 1;
	} else {
		printBoard(game);
		return 0;
	}
}
