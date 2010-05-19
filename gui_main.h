#ifndef MAIN_H
#define MAIN_H

extern void notify(char *string);
extern void notify_error(char *string);
extern char *hc_result(char *e);
extern void menu_append(GtkMenu *menu, char *visible, GCallback callback, char *insert);

#endif
