/*
 * NCurses Minesweeper.
 */

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#define ROWS 10
#define COLS 10
#define pair(y,x) (game[y][x] % 2 == -1 ? 1 : (game[y][x] < 0 ? 2 : 3))

int game[ROWS][COLS];
int mines, clean, i, j, r, c;

void printBoard();
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
	for(i = 0; i < ROWS * COLS; i++) {
		game[i/COLS][i%COLS] = -2;
	}
	printBoard();
	do {
		mines = eolprintw("How many mines would you like? ", 0);
	} while(mines < 1);
	clean = (ROWS * COLS) - mines;
	
	while(takeTurn()) {}
	
	printBoard();
	attron(COLOR_PAIR(1) | A_BOLD);
	eolprintw(clean > 0 ?  "LOL YOU FAILED!" : "EPIC WINZ!", 0);
	attroff(COLOR_PAIR(1) | A_BOLD);
	endwin();

	return 0;
}

void printBoard() {
	for(j = 0; j < COLS; j++) {
		mvprintw(0, 3*(j+1), "%2d", j);
	}
	for(i = 0; i < ROWS; i++) {
		mvprintw(i+1, 0, "%2d", i);
		for(j = 0; j < COLS; j++) {
			if(game[i][j] > 0) {
				attron(COLOR_PAIR(3));
				mvprintw(i+1, 3*(j+1), "[%d]", game[i][j]);
				attroff(COLOR_PAIR(3));
			} else {
				attron(COLOR_PAIR(pair(i,j)));
				mvprintw(i+1, 3*(j+1), game[i][j] % 2 == -1 ? "[F]" : (game[i][j] < 0 ? "[*]" : "   "));
				attroff(COLOR_PAIR(pair(i,j)));
			}
		}
	}
}

int takeTurn() {
	printBoard();
	r = eolprintw("Select a row to check: ", 1);
	if(r != 51 && r != 54) {
		c = eolprintw("Select a column to check: ", 1);
	}
	if(mines > 0) {
		game[r][c] -= 2;
		while(mines > 0) {
			i = rand()%ROWS;
			j = rand()%COLS;
			if(game[i][j] > -4) {
				game[i][j] = (game[i][j] % 2) - 4;
				mines--;
			}
		}
		game[r][c] += 2;
	}
	return checkSpot(r, c);
}

int checkSpot(int row, int col) {
	if(game[row][col] % 2 == -1) {
		return 1;
	} else if(game[row][col] == -4) {
		return 0;
	} else if(game[row][col] == -2) {
		game[row][col] = checkAround(row, col, 0);
		if(game[row][col] == 0) {
			checkAround(row, col, 1);
		}
		if(--clean <= 0) {
			return 0;
		}
	} else if(game[row][col] > 0 && game[row][col] == checkAround(row, col, 2)) {
		return checkAround(row, col, 3);
	}
	return 1;
}

int checkAround(int row, int col, int opt) {
	int y, x, total = 0;
	for(y = row < 1 ? 0 : row - 1; y <= (row == ROWS - 1 ? ROWS - 1 : row + 1); y++) {
		for(x = col < 1 ? 0 : col - 1; x <= (col == COLS - 1 ? COLS - 1 : col + 1); x++) {
			if(opt == 0 && game[y][x] / 2 == -2) {
				total++;
			} else if(opt == 1 && game[y][x] / 2 == -1) {
				checkSpot(y, x);
			} else if(opt == 2 && game[y][x] % 2 == -1) {
				total++;
			} else if(opt == 3 && game[y][x] < 0) {
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
			if(game[r][c] % 2 == -1) {
				game[r][c]++;
			} else if(game[r][c] < 0) {
				game[r][c]--;
			}
			printBoard();
		} else if(ch == 'c') {
			takeTurn();
		}
	}
	return ch - 48;
}
