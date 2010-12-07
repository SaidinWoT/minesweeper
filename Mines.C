/* Minesweeper. Attempt.
 * Isn't it exciting?
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define ROWS 10
#define COLS 10

struct board {
	int board[ROWS][COLS];
	int mines;
};

typedef struct board board;

board* createBoard();
void populate(board* gameBoard, int mines);
void printBoard(board* gameBoard);
int checkSpot(board* gameBoard, int row, int col);
void firstTurn(board* gameBoard, int mines);
int takeTurn(board* gameBoard);

int main(void) {
	int mines;
	srand(time(0));
	board* game = createBoard();
	printf("How many mines would you like? ");
	scanf("%d", &mines);
	firstTurn(game, mines);
	while(takeTurn(game)) {
	}
	printf("Game Over!\n");
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

void firstTurn(board* gameBoard, int mines) {
	int row, col;
	printBoard(gameBoard);
	printf("Select a row to check: ");
	scanf("%d", &row);
	printf("Select a column to check: ");
	scanf("%d", &col);
	gameBoard->board[row][col] = -2;
	populate(gameBoard, mines);
	gameBoard->board[row][col] = -1;
	checkSpot(gameBoard, row, col);
}

void populate(board* gameBoard, int mines) {
	while(gameBoard->mines < mines) {
		int posR = rand()%ROWS;
		int posC = rand()%COLS;
		if(gameBoard->board[posR][posC] != -2) {
			gameBoard->board[posR][posC] = -2;
			gameBoard->mines++;
		}
	}
}

void printBoard(board* gameBoard) {
	int i, j;
	printf("  ");
	for(j = 0; j < COLS; j++) {
		printf("%2d ", j);
	}
	printf("\n");
	for(i = 0; i < ROWS; i++) {
		printf("%2d ", i);
		for(j = 0; j < COLS; j++) {
			if(gameBoard->board[i][j] < 0) {
				printf("[*]");
			} else if(gameBoard->board[i][j] == 0) {
				printf("   ");
			} else {
				printf("[%d]", gameBoard->board[i][j]);
			}
		}
		printf("\n");
	}
}

int checkSpot(board* gameBoard, int row, int col) {
	if(gameBoard->board[row][col] == -2) {
		return 0;
	}
	gameBoard->board[row][col] = 0;
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
			if(gameBoard->board[i][j] == -2) {
				gameBoard->board[row][col]++;
			}
		}
	}
	if(gameBoard->board[row][col] == 0) {
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
				if(gameBoard->board[i][j] == -1) {
					checkSpot(gameBoard, i, j);	
				}
			}
		}
	}
	return 1;
}

int takeTurn(board* gameBoard) {
	int row, col;
	printBoard(gameBoard);
	printf("Select a row to check: ");
	scanf("%d", &row);
	printf("Select a column to check: ");
	scanf("%d", &col);
	if(checkSpot(gameBoard, row, col)) {
		return 1;
	} else {
		return 0;
	}
}
