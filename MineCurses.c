/*
 * NCurses Minesweeper.
 */

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#define ROWS 10
#define COLS 10
#define loc(y,x) game->board[y][x]

typedef struct board {
	int board[ROWS][COLS];
	int mines;
} board;

int mines, clean, i, j, r, c;
board* game;

board* createBoard();
void printBoard();
int firstTurn();
int takeTurn();
int checkSpot(int row, int col);
int checkAround(int row, int col, int opt);
int eolprintw(const char* msg, int opt);

int main() {
	initscr();
	if(has_colors() == TRUE) {
		start_color();
		init_pair(1, COLOR_BLACK, COLOR_YELLOW);
		init_pair(2, COLOR_WHITE, COLOR_BLUE);
		init_pair(3, COLOR_BLACK, COLOR_CYAN);
	}
	noecho();
	cbreak();

	srand(time(0));
	game = createBoard();
	printBoard();
	mines = eolprintw("How many mines would you like? ", 0);
	clean = (ROWS * COLS) - mines;
	
	if(firstTurn()) {
		while(takeTurn()) {}
	}
	
	printBoard();
	attron(COLOR_PAIR(1) | A_BOLD);
	eolprintw(clean > 0 ?  "LOL YOU FAILED!" : "EPIC WINZ!", 0);
	attroff(COLOR_PAIR(1) | A_BOLD);
	endwin();

	return 0;
}

board* createBoard() {
	board* newBoard = (board*)malloc(sizeof(board));
	for(i = 0; i < ROWS * COLS; i++) {
		newBoard->board[i/COLS][i%COLS] = -2;
	}
	newBoard->mines = 0;
	return newBoard;
}

void printBoard() {
	for(j = 0; j < COLS; j++) {
		mvprintw(0, 3*(j+1), "%2d", j);
	}
	for(i = 0; i < ROWS; i++) {
		mvprintw(i+1, 0, "%2d", i);
		for(j = 0; j < COLS; j++) {
			if(loc(i, j) > 0) {
				attron(COLOR_PAIR(3));
				mvprintw(i+1, 3*(j+1), "[%d]", loc(i, j));
				attroff(COLOR_PAIR(3));
			} else {
				attron(COLOR_PAIR(loc(i, j) % 2 == -1 ? 1 : (loc(i, j) < 0 ? 2 : 3)));
				mvprintw(i+1, 3*(j+1), loc(i, j) % 2 == -1 ? "[F]" : (loc(i, j) < 0 ? "[*]" : "   "));
				attroff(COLOR_PAIR(loc(i, j) % 2 == -1 ? 1 : (loc(i, j) < 0 ? 2 : 3)));
			}
		}
	}
	refresh();
}

int firstTurn() {
	printBoard();
	r = eolprintw("Select a row to check: ", 1);
	if(r != 51 && r != 54) {
		c = eolprintw("Select a column to check: ", 1);
	}
	loc(r, c) -= 2;
	while(game->mines < mines) {
		i = rand()%ROWS;
		j = rand()%COLS;
		if(loc(i, j) > -4) {
			loc(i, j) = (loc(i, j) % 2) - 4;
			game->mines++;
		}
	}
	loc(r, c) += 2;
	return checkSpot(r, c);
}

int takeTurn() {
	printBoard();
	r = eolprintw("Select a row to check: ", 1);
	if(r != 51 && r != 54) {
		c = eolprintw("Select a column to check: ", 1);
	}
	return checkSpot(r, c);
}

int checkSpot(int row, int col) {
	if(loc(row, col) % 2 == -1) {
		return 1;
	} else if(loc(row, col) == -4) {
		return 0;
	} else if(loc(row, col) == -2) {
		loc(row, col) = checkAround(row, col, 0);
		if(loc(row, col) == 0) {
			checkAround(row, col, 1);
		}
		if(--clean <= 0) {
			return 0;
		}
	} else if(loc(row, col) > 0 && loc(row, col) == checkAround(row, col, 2)) {
		return checkAround(row, col, 3);
	}
	return 1;
}

int checkAround(int row, int col, int opt) {
	int y, x, total = 0;
	for(y = row < 1 ? 0 : row - 1; y <= (row == ROWS - 1 ? ROWS - 1 : row + 1); y++) {
		for(x = col < 1 ? 0 : col - 1; x <= (col == COLS - 1 ? COLS - 1 : col + 1); x++) {
			if(opt == 0 && loc(y, x) / 2 == -2) {
				total++;
			} else if(opt == 1 && loc(y, x) / 2 == -1) {
				checkSpot(y, x);
			} else if(opt == 2 && loc(y, x) % 2 == -1) {
				total++;
			} else if(opt == 3 && loc(y, x) < 0) {
				total++;
				if(checkSpot(y, x) == 0) {
					return 0;
				}
			}
		}
	}
	return total;
}

int eolprintw(const char* msg, int opt) {
	move(ROWS+2, 0);
	clrtoeol();
	printw(msg);
	refresh();
	int ch = getch();
	if(opt == 1) {
		if(ch == 'q') {
			endwin();
			exit(0);
		} else if(ch == 'f') {
			r = eolprintw("Select a row to flag: ", 1);
			if(r != 51 && r != 54) {
				c = eolprintw("Select a column to flag: ", 1);
			}
			if(loc(r, c) % 2 == -1) {
				loc(r, c)++;
			} else {
				loc(r, c)--;
			}
			printBoard();
		} else if(ch == 'c') {
			takeTurn();
		}
	}
	return ch - 48;
}
