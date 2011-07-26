/*
 * NCurses Minesweeper.
 */

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#define ROWS 10 
#define COLS 10

typedef struct board {
	int board[ROWS][COLS];
	int mines;
} board;

board* createBoard();
void populate(board* gameBoard, int mines);
void printBoard(board* gameBoard);
int checkSpot(board* gameBoard, int r, int c);
void checkAround(board* gameBoard, int r, int c, int opt);
int firstTurn(board* gameBoard, int mines);
int takeTurn(board* gameBoard);
int eolprintw(int y, int x, const char* msg);

int mines, clean, row, col;

int main() {
	initscr();
	if(has_colors() == TRUE) {
		start_color();
		init_pair(1, COLOR_WHITE, COLOR_BLUE);
		init_pair(2, COLOR_BLACK, COLOR_CYAN);
	}
	noecho();
	cbreak();

	srand(time(0));
	board* game = createBoard();
	mines = eolprintw(0, 0, "How many mines would you like? ");
	clean = (ROWS * COLS) - mines;
	clear();

	if(firstTurn(game, mines)) {
		while(takeTurn(game)) {}
	}
	
	printBoard(game);
	attron(COLOR_PAIR(1) | A_BOLD);
	clean > 0 ? eolprintw(ROWS+2, 0, "LOL YOU FAILED!") : eolprintw(ROWS+2, 0, "EPIC WINZ!");
	attroff(COLOR_PAIR(1) | A_BOLD);
	endwin();

	return 0;
}

board* createBoard() {
	board* newBoard = (board*)malloc(sizeof(board));
	int i;
	for(i = 0; i < ROWS * COLS; i++) {
		newBoard->board[i/COLS][i%COLS] = -1;
	}
	newBoard->mines = 0;
	return newBoard;
}

int firstTurn(board* gameBoard, int mines) {
	printBoard(gameBoard);
	row = eolprintw(ROWS+2, 0, "Select a row to check: ");
	col = eolprintw(ROWS+2, 0, "Select a column to check: ");
	gameBoard->board[row][col] = -2;
	populate(gameBoard, mines);
	gameBoard->board[row][col] = -1;
	return checkSpot(gameBoard, row, col);
}

void populate(board* gameBoard, int mines) {
	int posR, posC;
	while(gameBoard->mines < mines) {
		posR = rand()%ROWS;
		posC = rand()%COLS;
		if(gameBoard->board[posR][posC] != -2) {
			gameBoard->board[posR][posC] = -2;
			gameBoard->mines++;
		}
	}
}

void printBoard(board* gameBoard) {
	int i, j;
	for(j = 0; j < COLS; j++) {
		mvprintw(0, 3*(j+1), "%2d", j);
	}
	for(i = 0; i < ROWS; i++) {
		mvprintw(i+1, 0, "%2d", i);
		for(j = 0; j < COLS; j++) {
			if(gameBoard->board[i][j] < 0) {
				attron(COLOR_PAIR(1));
				mvprintw(i+1, 3*(j+1), "[*]");
				attroff(COLOR_PAIR(1));
			} else if(gameBoard->board[i][j] == 0) {
				attron(COLOR_PAIR(2));
				mvprintw(i+1, 3*(j+1), "   ");
				attroff(COLOR_PAIR(2));
			} else {
				attron(COLOR_PAIR(2));
				mvprintw(i+1, 3*(j+1), "[%d]", gameBoard->board[i][j]);
				attroff(COLOR_PAIR(2));
			}
		}
	}
	refresh();
}

int checkSpot(board* gameBoard, int r, int c) {
	if(gameBoard->board[r][c] == -2) {
		return 0;
	}
	if(gameBoard->board[r][c] == -1) {
		gameBoard->board[r][c] = 0;
		checkAround(gameBoard, r, c, 0);
		if(gameBoard->board[r][c] == 0) {
			checkAround(gameBoard, r, c, 1);
		}
		if(--clean <= 0) {
			return 0;
		}
	}
	return 1;
}

void checkAround(board* gameBoard, int r, int c, int opt) {
	int i, j;
	for(i = r - 1; i <= r + 1; i++) {
		if(i < 0) {
			i = 0;
		} else if(i == ROWS) {
			break;
		}
		for(j = c - 1; j <= c + 1; j++) {
			if(j < 0) {
				j = 0;
			} else if(j == COLS) {
				break;
			}
			if(opt == 0 && gameBoard->board[i][j] == -2) {
				gameBoard->board[r][c]++;
			} else if(opt == 1 && gameBoard->board[i][j] == -1) {
				checkSpot(gameBoard, i, j);
			}
		}
	}
}

int takeTurn(board* gameBoard) {
	printBoard(gameBoard);
	row = eolprintw(ROWS+2, 0, "Select a row to check: ");
	col = eolprintw(ROWS+2, 0, "Select a column to check: ");
	return checkSpot(gameBoard, row, col);
}

int eolprintw(int y, int x, const char* msg) {
	move(y, x);
	clrtoeol();
	printw(msg);
	refresh();
	return getch() - 48;
}
