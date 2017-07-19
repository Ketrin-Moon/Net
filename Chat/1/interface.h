#include <curses.h>
/*
typedef struct window{
    WINDOW *chat_window;
    WINDOW *user_window;
    WINDOW *message_window;
}Window;
*/
WINDOW *current_window;
WINDOW *chat_window;
WINDOW *user_window;
WINDOW *message_window;

void init_w();
void color_pair(WINDOW *win, int color_bg);
void window();
