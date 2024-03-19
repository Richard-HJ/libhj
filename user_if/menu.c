/*
	menu.c	R.E. Hughes-Jones  	University of Manchester, ATLAS
*/


#include <stdio.h>
#include <ctype.h>
#include "stdio.h"

#include "user_if.h"

int menu(char *node_name,char *title, struct menu_items items[])
/* --------------------------------------------------------------------- */
/*   present menu items
     ask for input
     on esc or dot return from menu() with value -9999
     if input number matches a number of the menu item list 
             execute the associated function - if any
	     OR return from menu() with value of index
*/
{
  int i;
  int index;
  char input[64];
  int ret;

  for(;;){
    printf("\n%s ======> %s\n",node_name, title);
    printf("enter <esc> or <.> dot to exit this menu\n\n");
/* display menu */
      i=0;
      while(items[i].num != MENU_END){
	  if(items[i].num ==0) printf(" -- %s\n", items[i].text );
	  else printf(" %2d %s\n", items[i].num, items[i].text);
	  i++;
      }
      printf("Select Item :");
      ret = GetLine(input, 64, 0);
      if(ret == 0) return (MENU_END);
       
      index = MENU_END;
 
      sscanf(input, "%3d", &index);
/* see if entered number corresponds to an item in the menu 
   - if so check for a function and call the function 
   - then return index value */

      i=0;
      while(items[i].num != MENU_END){
	  if(items[i].num == index) {
	    if (items[i].routine != NULL){
	        items[i].routine(items[i].func_param);
		if (items[i].pause == 1){
		    printf("\n Press return to continue :");
		    ret = GetLine(input, 64, 0);
		}
	    }
	    return(index);
	  }
	  i++;
      } /* end of while() */
  } 
}
