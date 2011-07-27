/*
 * NCurses Minesweeper with arrow keys!
 */

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#define ROWS 9 
#define COLS 9
#define loc(y,x) game->board[y][x]

typedef struct board {
	int board[ROWS][COLS];
	int mines;
} board;

int mines = 10;
int cursY, cursX, choice, clean, i, j;
board* game;

board* createBoard();
void printBoard();
int firstTurn(int mines);
int takeTurn();
int moveCursor();
int checkSpot(int row, int col);
int checkAround(int row, int col, int opt);
void flag();
void mvatprintw(int y, int x, int attr, const char* msg);

int main() {
	initscr();
	if(has_colors() == TRUE) {
		start_color();
		init_pair(1, COLOR_WHITE, COLOR_BLUE);
		init_pair(2, COLOR_BLACK, COLOR_CYAN);
		init_pair(3, COLOR_BLACK, COLOR_YELLOW);
	}
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);

	srand(time(0));
	clean = (ROWS * COLS) - mines;
	game = createBoard();

	if(firstTurn(mines)) {
		while(takeTurn()) {}
	}

	printBoard();
	mvatprintw(ROWS, 0, COLOR_PAIR(1) | A_BOLD, clean > 0 ? "LOL YOU FAILED!" : "EPIC WINZ!");
	getch();
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
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS; j++) {
			if(loc(i, j)%2 == -1) {
				mvatprintw(i, 3*j, COLOR_PAIR(3), "[F]");
			} else if(loc(i, j) < 0) {
				mvatprintw(i, 3*j, COLOR_PAIR(1), "[*]");
			} else if(loc(i, j) == 0) {
				mvatprintw(i, 3*j, COLOR_PAIR(2), "   ");
			} else {
				attron(COLOR_PAIR(2));
				mvprintw(i, 3*j, "[%d]", loc(i, j));
				attroff(COLOR_PAIR(2));
			}
		}
	}
	mvchgat(cursY, cursX*3, 3, A_REVERSE, NULL, NULL);
}

int firstTurn(int mines) {
	printBoard();
	while(choice != 1) {
		choice = moveCursor();
		if(choice == 0) { 
			return 0;
		} else if(choice == 1) {
			loc(cursY, cursX) = -4;
			while(game->mines < mines) {
				i = rand()%ROWS;
				j = rand()%COLS;
				if(loc(i, j) > -4) {
					loc(i, j) = (loc(i, j) % 2) - 4;
					game->mines++;
				}
			}
			loc(cursY, cursX) = -2;
		}
	}
	return checkSpot(cursY, cursX);
}

int takeTurn() {
	printBoard();
	choice = moveCursor();
	if(choice == 0) {
		return 0;
	} else if(choice == 1) {
		return checkSpot(cursY, cursX);
	}
}

int moveCursor() {
	int ch;
	for(ch = getch(); ch != 'q'; ch = getch()) {
		mvchgat(cursY, cursX*3, 3, A_NORMAL, (loc(cursY, cursX) % 2 == -1 ? 3 : (loc(cursY, cursX) < 0 ? 1 : 2)), NULL);
		if(ch == 'c') {
			return 1;
		} else if(ch == 'f') {
			if(loc(cursY, cursX)%2 == -1) {
				loc(cursY, cursX)++;
			} else if(loc(cursY, cursX) < 0) {
				loc(cursY, cursX)--;
			}
			printBoard();
		}  else if((ch == KEY_LEFT || ch == 'a') && cursX > 0) {
			cursX--;
		} else if((ch == KEY_RIGHT || ch == 'd') && cursX < COLS - 1) {
			cursX++;
		} else if((ch == KEY_UP || ch == 'w') && cursY > 0) {
			cursY--;
		} else if((ch == KEY_DOWN || ch == 's') && cursY < ROWS - 1) {
			cursY++;
		} 
		mvchgat(cursY, cursX*3, 3, A_REVERSE, (loc(cursY, cursX) % 2 == -1 ? 3 : (loc(cursY, cursX) < 0 ? 1 : 2)), NULL);
	}
	return 0;
}

int checkSpot(int row, int col) {
	if(loc(row,col)%2 == -1) {
		return 1;
	}
	if(loc(row, col) <= -4) {
		return 0;
	}
	if(loc(row, col) <= -2) {
		loc(row, col) = checkAround(row, col, 0);
		if(loc(row, col) == 0) {
			checkAround(row, col, 1);
		}
		if(--clean <= 0) {
			return 0;
		}
	} else if(loc(row, col) > 0 && loc(row, col) == checkAround(row, col, 2)) {
		mvprintw(ROWS+1, 0, "HAI %d", loc(row,col));
		getch();
		return checkAround(row, col, 3);
	}
	return 1;
}

int checkAround(int row, int col, int opt) {
	int y, x, total = 0;
	for(y = row < 1 ? 0 : row - 1; y <= (row == ROWS - 1 ? ROWS - 1 : row + 1); y++) {
		for(x = col < 1 ? 0 : col - 1; x <= (col == COLS - 1 ? COLS - 1 : col + 1); x++) {
			if(opt == 0 && loc(y, x)/2 == -2) {
				total++;
			} else if(opt == 1 && loc(y, x)/2 == -1) {
				checkSpot(y, x);
			} else if(opt == 2 && loc(y, x)%2 == -1) {
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

void mvatprintw(int y, int x, int attr, const char* msg) {
	attron(attr);
	mvprintw(y, x, msg);
	attroff(attr);
}
