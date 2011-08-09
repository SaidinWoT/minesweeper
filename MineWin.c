/*
 * NCurses Minesweeper with windows!
 */

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#define ROWS 10 
#define COLS 10
#define loc(y,x) game->board[y][x]		// Less typing

/*
 *	board struct
 *	Tracks values of each square as well as total number of mines
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
typedef struct board {
	int board[ROWS][COLS];
	int mines;
} board;

int cursY, cursX, clean, i, j;
board* game;
WINDOW* game_win;
WINDOW* stat_win;

board* createBoard();
void printBoard();
int takeTurn();
int checkSpot(int row, int col);
int checkAround(int row, int col, int opt);
void moveCursor();
void mvwatprintw(WINDOW* win, int y, int x, int attr, const char* msg);

int main() {
	initscr();
	if(has_colors() == TRUE) {
		start_color();
		init_pair(1, COLOR_BLACK, COLOR_YELLOW);
		init_pair(2, COLOR_WHITE, COLOR_BLUE);
		init_pair(3, COLOR_BLACK, COLOR_CYAN);
	}
	game_win = newwin(ROWS, COLS*3, 0, 0);
	stat_win = newwin(1, 34, ROWS, 0);
	cbreak();
	keypad(game_win, TRUE);
	curs_set(0);

	srand(time(0));
	game = createBoard();
	printBoard();
	do {
		mvwprintw(stat_win, 0, 0, "How many mines would you like? ");
		wclrtoeol(stat_win);
		wscanw(stat_win, "%d", &game->mines);
	} while(game->mines < 1);
	//} while(game->mines < (ROWS * COLS * 0.075) || game->mines > (ROWS - 1) * (COLS - 1));
	wclear(stat_win);
	wrefresh(stat_win);
	clean = (ROWS * COLS) - game->mines;
	noecho();

	while(takeTurn()) {}

	printBoard();
	mvwatprintw(stat_win, 0, 0, COLOR_PAIR(1) | A_BOLD, clean > 0 ? "LOL YOU FAILED!" : "EPIC WINZ!");
	wgetch(stat_win);
	delwin(game_win);
	delwin(stat_win);
	endwin();

	free(game);
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
			if(loc(i, j) > 0) {
				wattron(game_win, COLOR_PAIR(3));
				mvwprintw(game_win, i, 3*j, "[%d]", loc(i, j));
				wattroff(game_win, COLOR_PAIR(3));
			} else {
				mvwatprintw(game_win, i, 3*j, COLOR_PAIR(loc(i, j) % 2 == -1 ? 1 : (loc(i, j) < 0 ? 2 : 3)), loc(i, j) % 2 == -1 ? "[F]" : (loc(i, j) < 0 ? "[*]" : "   "));
			}
		}
	}
	mvwchgat(game_win, cursY, cursX*3, 3, A_REVERSE, (loc(cursY, cursX) % 2 == -1 ? 1 : (loc(cursY, cursX) < 0 ? 2 : 3)), NULL);
}

int takeTurn() {
	moveCursor();
	if(game->mines > 0) {
		loc(cursY, cursX) -= 2;
		while(game->mines > 0) {
			i = rand()%ROWS;
			j = rand()%COLS;
			if(loc(i, j) > -4) {
				loc(i, j) = (loc(i, j) % 2) - 4;
				game->mines--;
			}
		}
		loc(cursY, cursX) += 2;
	}
	return checkSpot(cursY, cursX);
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

void moveCursor() {
	printBoard();
	int ch;
	for(ch = wgetch(game_win); ch != 'c'; ch = wgetch(game_win)) {
		mvwchgat(game_win, cursY, cursX*3, 3, A_NORMAL, (loc(cursY, cursX) % 2 == -1 ? 1 : (loc(cursY, cursX) < 0 ? 2 : 3)), NULL);
		if(ch == 'q') {
			endwin();
			exit(0);
		} else if(ch == 'f') {
			if(loc(cursY, cursX) % 2 == -1) {
				loc(cursY, cursX)++;
			} else if(loc(cursY, cursX) < 0) {
				loc(cursY, cursX)--;
			}
			printBoard();
		} else if((ch == 'w' || ch == KEY_UP) && cursY > 0) {
			cursY--;
		} else if((ch == 'a' || ch == KEY_LEFT) && cursX > 0) {
			cursX--;
		} else if((ch == 's' || ch == KEY_DOWN) && cursY < COLS - 1) {
			cursY++;
		} else if((ch == 'd' || ch == KEY_RIGHT) && cursX < COLS - 1) {
			cursX++;
		}
		mvwchgat(game_win, cursY, cursX*3, 3, A_REVERSE, (loc(cursY, cursX) % 2 == -1 ? 1 : (loc(cursY, cursX) < 0 ? 2 : 3)), NULL);
	}
}

/*
 *	Turn an attribute on, call mvprintw, and turn the attribute back off
 */
void mvwatprintw(WINDOW* win, int y, int x, int attr, const char* msg) {
	wattron(win, attr);
	mvwprintw(win, y, x, msg);
	wattroff(win, attr);
	wrefresh(win);
}