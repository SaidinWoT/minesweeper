/*
 * NCurses Minesweeper with arrow keys!
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

int cursorX = 0;
int cursorY = 0;
int mines = 9;
int choice, clean;

board* createBoard();
int firstTurn(board* gameBoard, int mines);
void populate(board* gameBoard, int mines);
void printBoard(board* gameBoard);
int checkSpot(board* gameBoard, int row, int col);
void checkAround(board* gameBoard, int row, int col, int opt);
int moveCursor(board* gameBoard);
int takeTurn(board* gameBoard);

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
	board* game = createBoard();

	if(firstTurn(game, mines)) {
		while(takeTurn(game)) {}
	}

	printBoard(game);
	attron(COLOR_PAIR(1) | A_BOLD);
	mvprintw(ROWS, 0, clean > 0 ? "LOL YOU FAILED!" : "EPIC WINZ!");
	attroff(COLOR_PAIR(1) | A_BOLD);
	getch();
	endwin();

	return 0;
}

board* createBoard() {
	board* newBoard = (board*)malloc(sizeof(board));
	int i;
	for(i = 0; i < ROWS * COLS; i++) {
		newBoard->board[i/COLS][i%COLS] = -2;
	}
	newBoard->mines = 0;
	return newBoard;
}

int firstTurn(board* gameBoard, int mines) {
	printBoard(gameBoard);
	int choice = moveCursor(gameBoard);
	if(choice == 0) { 
		return 0;
	} else if(choice == 1) {
		gameBoard->board[cursorY][cursorX/3] = -4;
		populate(gameBoard, mines);
		gameBoard->board[cursorY][cursorX/3] = -2;
	} else if(choice == 2) {
		gameBoard->board[cursorY][cursorX/3] = -4;
		populate(gameBoard, mines);
		gameBoard->board[cursorY][cursorX/3] = -3;
		return 1;
	}
	return checkSpot(gameBoard, cursorY, cursorX/3);
}

void populate(board* gameBoard, int mines) {
	int posR, posC;
	while(gameBoard->mines < mines) {
		posR = rand()%ROWS;
		posC = rand()%COLS;
		if(gameBoard->board[posR][posC] != -4) {
			gameBoard->board[posR][posC] = -4;
			gameBoard->mines++;
		}
	}
}

void printBoard(board* gameBoard) {
	int i, j;
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS; j++) {
			if(gameBoard->board[i][j]%2 == -1) {
				attron(COLOR_PAIR(3));
				mvprintw(i, 3*j, "[F]");
				attroff(COLOR_PAIR(3));
			} else if(gameBoard->board[i][j] < 0) {
				attron(COLOR_PAIR(1));
				mvprintw(i, 3*j, "[*]");
				attroff(COLOR_PAIR(1));
			} else if(gameBoard->board[i][j] == 0) {
				attron(COLOR_PAIR(2));
				mvprintw(i, 3*j, "   ");
				attroff(COLOR_PAIR(2));
			} else {
				attron(COLOR_PAIR(2));
				mvprintw(i, 3*j, "[%d]", gameBoard->board[i][j]);
				attroff(COLOR_PAIR(2));
			}
		}
	}
	mvchgat(cursorY, cursorX, 3, A_REVERSE, NULL, NULL);
	refresh();
}

int checkSpot(board* gameBoard, int row, int col) {
	if(gameBoard->board[row][col] <= -4) {
		return 0;
	} else if(gameBoard->board[row][col] <= -2) {
		gameBoard->board[row][col] = 0;
		checkAround(gameBoard, row, col, 0);
		if(gameBoard->board[row][col] == 0) {
			checkAround(gameBoard, row, col, 1);
		}
		if(--clean <= 0) {
			return 0;
		}
	}
	return 1;
}

void checkAround(board* gameBoard, int row, int col, int opt) {
	int i, j;
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
			if(opt == 0 && gameBoard->board[i][j]/2 == -2) {
				gameBoard->board[row][col]++;
			} else if(opt == 1 && gameBoard-> board[i][j]/2 == -1) {
				checkSpot(gameBoard, i, j);
			}
		}
	}
}

int moveCursor(board* gameBoard) {
	int ch;
	for(ch = getch(); ch != 'q'; ch = getch()) {
		mvchgat(cursorY, cursorX, 3, A_NORMAL, (gameBoard->board[cursorY][cursorX/3] % 2 == -1 ? 3 : (gameBoard->board[cursorY][cursorX/3] < 0 ? 1 : 2)), NULL);
		if(ch == 'c') {
			return 1;
		} else if(ch == 'f') {
			return 2;
		} else if(ch == KEY_LEFT && cursorX > 0) {
			cursorX -= 3;
		} else if(ch == KEY_RIGHT && cursorX < (COLS - 1)*3) {
			cursorX += 3;
		} else if(ch == KEY_UP && cursorY > 0) {
			cursorY--;
		} else if(ch == KEY_DOWN && cursorY < ROWS - 1) {
			cursorY++;
		} 
		mvchgat(cursorY, cursorX, 3, A_REVERSE, (gameBoard->board[cursorY][cursorX/3] % 2 == -1 ? 3 : (gameBoard->board[cursorY][cursorX/3] < 0 ? 1 : 2)), NULL);
	}
	return 0;
}

int takeTurn(board* gameBoard) {
	printBoard(gameBoard);
	choice = moveCursor(gameBoard);
	if(choice == 0) {
		return 0;
	} else if(choice == 1) {
		return checkSpot(gameBoard, cursorY, cursorX/3);
	} else if(choice == 2) {
		if(gameBoard->board[cursorY][cursorX/3]%2 == -1) {
			gameBoard->board[cursorY][cursorX/3]++;
		} else if(gameBoard->board[cursorY][cursorX/3] < 0) {
			gameBoard->board[cursorY][cursorX/3]--;
		}
		return 1;
	}
}
