/*
	user_if.h	R.E. Hughes-Jones  	University of Manchester, ATLAS
	include file defining the user interface functions
*/

#ifndef  _USER_IF_H_
#define  _USER_IF_H_

#define MENU_END    -9999

struct menu_items{
  int num;                         /* item number */
  char *text;                      /* text for menu */
  void (*routine)(void *param );   /* pointer to function */
  void *func_param;                /* parameter for routine(param) */
  int pause;                       /* =1 menu() will ask "to continue" */
};

/* functions */
int GetLine(char *s, int lim, FILE *fp);
int readline(char *s, int lim, FILE *fp);

int menu(char *node_name, char *title, struct menu_items items[]);

#endif /* _USER_IF_H_ */
