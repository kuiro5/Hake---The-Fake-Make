/* CMPSC 311, Spring 2013, Project 8
 *
 * Author:   Joshua Kuiros
 * Email:    jjk523@psu.edu
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "cmpsc311.h"
#include "source.h"
#include "recipe.h"
#include "names.h"

//------------------------------------------------------------------------------
struct list_recipe *list_recipe_init(void)                          // create new recipe
{
  struct list_recipe *r = Malloc(sizeof(struct list_recipe));

  r->head = NULL;
  r->tail = NULL;
  //r->recipe_num = 0;
  r->next_rcp = NULL;

  return r;
}
void recipe_find_target(char *name)
{
    struct target *t = get_targl_head();
    
        while( t!= NULL)
        {
            if(t->is_last_targ)
            {
                list_recipe_new(name, t->recipe);
            }
            t = t->next;
            //printf("2\n");
    }
}



void list_recipe_new(char *name, struct list_recipe *node)
{
  struct recipe *r = Malloc(sizeof(struct list_recipe));

  if(node->head == NULL)
  {
    node->head = r;
    node->tail = r;
  }
  else
  {
    (node->tail)->next = r;
    node->tail = r;
  }
    r->name = Strdup(name);
   // node->recipe_num = node->recipe_num + 1;
    r->next = NULL;
    //printf("adding new recipe %s to list\n", r->name);
}

void list_recipe_print(struct list_recipe *node)
{
  struct recipe *iter;

  for( iter = node->head; iter != NULL; iter = iter->next )
  {
    printf("%s\n", iter->name);
  }

}
 

//------------------------------------------------------------------------------

