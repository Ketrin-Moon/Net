#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <sys/types.h>
#include "interface.h"

void init_w()
{
	system("clear");
	initscr();
	curs_set(0);
	refresh();
	keypad(stdscr, true);
	cbreak();
	noecho();
}

void color_pair(WINDOW *win, int color_bg)
{
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_BLUE, COLOR_WHITE);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);
	wbkgd(win, COLOR_PAIR(color_bg));
}

void window()
{
//	WINDOW *chat_window, *user_window, *message_window;

	int row, col;
	int user_char;

	init_w();
	getmaxyx(stdscr, row, col);
	current_window = newwin(row, col, 0,0);
	color_pair(current_window, 2);
	wrefresh(current_window);
	chat_window = derwin(current_window,row-13, col - 32, 1, 1);
	box(chat_window, 0, 0);
	color_pair(chat_window, 3);
	wrefresh(chat_window);
	user_window = derwin(current_window, row-13, 30, 1, col-31);
//	user_window = derwin(current_window, row-13, 30, 31, 10);
	box(user_window, 0, 0);
	color_pair(user_window, 3);
	wrefresh(user_window);
	message_window = derwin(current_window, 11, col-2, row-12, 1);
	box(message_window, 0, 0);
	color_pair(message_window, 3);
	wrefresh(message_window);
/*	color_pair(user_window, 3);
	color_pair(message_window, 3);
	wrefresh(chat_window);
	wrefresh(user_window);
	wrefresh(message_window);
*/	user_char = getch();
	if(user_char == 'q'){
		endwin();
		delwin(chat_window);
		system("clear");
		exit(0);
	}
}
/*
int main()
{
	window();

	return 0;
}
*/