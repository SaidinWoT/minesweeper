/*
 * NCurses Minesweeper with arrow keys!
 */

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#define ROWS 10 
#define COLS 10
#define pair(y,x) (game[y][x] % 2 == -1 ? 1 : (game[y][x] < 0 ? 2 : 3))

/*
 *	game array
 *	Tracks values of each square
 *	Squares each maintain a single integer value to indicate their state:
 *	A value of...
 *		0 or greater indicates a checked spot with that number of
 *		mines in the adjacent squares
 *		
 *		-2 indicates an unflagged, unchecked, empty spot
 *
 *		-3 indicates a flagged, unchecked, empty spot
 *
 *		-4 indicates an unflagged, unchecked spot containing a mine
 *
 *		-5 indicates a flagged, unchecked spot containing a mine
 */
int game[ROWS][COLS];
int cursY, cursX, clean, i, j, mines;

void printBoard();
int takeTurn();
int checkSpot(int row, int col);
int checkAround(int row, int col, int opt);
void moveCursor();
void mvatprintw(int y, int x, int attr, const char* msg);

int main() {
	initscr();
	if(has_colors() == TRUE) {
		start_color();
		init_pair(1, COLOR_BLACK, COLOR_YELLOW);
		init_pair(2, COLOR_WHITE, COLOR_BLUE);
		init_pair(3, COLOR_BLACK, COLOR_CYAN);
	}
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);

	srand(time(0));
	for(i = 0; i < ROWS * COLS; i++) {
		game[i/COLS][i%COLS] = -2;
	}
	printBoard();
	do {
		mvprintw(ROWS + 1, 0, "How many mines would you like? ");
		clrtoeol();
		scanw("%2d", &mines);
	} while(mines < 1);
	//} while(mines < (ROWS * COLS * 0.075) || mines > (ROWS - 1) * (COLS - 1));
	clear();
	clean = (ROWS * COLS) - mines;
	noecho();

	while(takeTurn()) {}

	printBoard();
	mvatprintw(ROWS + 1, 0, COLOR_PAIR(1) | A_BOLD, clean > 0 ? "LOL YOU FAILED!" : "EPIC WINZ!");
	getch();
	endwin();

	return 0;
}

void printBoard() {
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS; j++) {
			if(game[i][j] > 0) {
				attron(COLOR_PAIR(3));
				mvprintw(i, 3*j, "[%d]", game[i][j]);
				attroff(COLOR_PAIR(3));
			} else {
				mvatprintw(i, 3*j, COLOR_PAIR(pair(i, j)), game[i][j] % 2 == -1 ? "[F]" : (game[i][j] < 0 ? "[*]" : "   "));
			}
		}
	}
	mvchgat(cursY, cursX*3, 3, A_REVERSE, pair(cursY, cursX) , NULL);
}

int takeTurn() {
	moveCursor();
	if(mines > 0) {
		game[cursY][cursX] -= 2;
		while(mines > 0) {
			i = rand()%ROWS;
			j = rand()%COLS;
			if(game[i][j] > -4) {
				game[i][j] = (game[i][j] % 2) - 4;
				mines--;
			}
		}
		game[cursY][cursX] += 2;
	}
	return checkSpot(cursY, cursX);
}

int checkSpot(int row, int col) {
	if(game[row][col] == -4) {
		return 0;
	} else if(game[row][col] == -2) {
		game[row][col] = checkAround(row, col, 0);
		if(game[row][col] == 0) {
			checkAround(row, col, 1);
		}
		return --clean;
	} else if(game[row][col] > 0 && game[row][col] == checkAround(row, col, 2)) {
		return checkAround(row, col, 3);
	}
	return 1;
}

int checkAround(int row, int col, int opt) {
	int y, x, total = 0;
	for(y = row - 1; y <= row + 1; y++) {
		for(x = col - 1; x <= col + 1; x++) {
			i = (y+ROWS)%ROWS;
			j = (x+COLS)%COLS;
			if(opt == 0 && game[i][j] / 2 == -2) {
				total++;
			} else if(opt == 1 && game[i][j] / 2 == -1) {
				checkSpot(i, j);
			} else if(opt == 2 && game[i][j] % 2 == -1) {
				total++;
			} else if(opt == 3 && game[i][j] < 0) {
				total++;
				if(checkSpot(i, j) == 0) {
					return 0;
				}
			}
		}
	}
	return total;
}

void moveCursor() {
	printBoard();
	static int ch;
	for(ch = getch(); ch != 'c'; ch = getch()) {
		mvchgat(cursY, cursX*3, 3, A_NORMAL, pair(cursY, cursX), NULL);
		switch(ch) {
			case 'o':
				printBoard(1);
				break;
			case 'w':
			case KEY_UP:
				cursY = (cursY + ROWS - 1) % ROWS;
				break;
			case 'a':
			case KEY_LEFT:
				cursX = (cursX + COLS - 1) % COLS;
				break;
			case 's':
			case KEY_DOWN:
				cursY = (cursY + ROWS + 1) % ROWS;
				break;
			case 'd':
			case KEY_RIGHT:
				cursX = (cursX + COLS + 1) % COLS;
				break;
			case 'f':
				if(game[cursY][cursX] % 2 == -1) {
					game[cursY][cursX]++;
				} else if(game[cursY][cursX] < 0) {
					game[cursY][cursX]--;
				}
				printBoard();
				break;
			case 'q':
				endwin();
				exit(0);
		}
		mvchgat(cursY, cursX*3, 3, A_REVERSE, pair(cursY, cursX), NULL);
	}
}

/*
 *	Turn an attribute on, call mvprintw, and turn the attribute back off
 */
void mvatprintw(int y, int x, int attr, const char* msg) {
	attron(attr);
	mvprintw(y, x, msg);
	attroff(attr);
}
