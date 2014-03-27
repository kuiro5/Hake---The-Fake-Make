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
#include <stdbool.h>

#include "cmpsc311.h"
#include "names.h"
#include "target.h"
#include "source.h"
#include "recipe.h"

struct list_target *targl;                          // target list initialiezed and manipulated in target.c

void list_target_init(void)                         // create a new target list
{
  targl = malloc(sizeof(struct list_target));

  targl->head = NULL;
  targl->tail = NULL;
  targl->next_tgt = NULL;
}
struct target *get_targl_head(void)                 // return head of the target linked list
{
    return targl->head;
}

void list_target_new_target(char *name)             // create a new target node on the target linked list
{
  struct target *t = Malloc(sizeof(struct target));
    
  struct list_recipe *r = list_recipe_init();
  struct list_source *s = list_source_init();

  if(targl->head == NULL)
  {
    targl->head = t;
    targl->tail = t;
  }
  else
  {
    (targl->tail)->next = t;
    targl->tail = t;  
  }

  t->name = Strdup(name);
  t->source = s;
  t->recipe = r;
  t->is_last_targ = true;
  
  t->next = NULL;
    
}


struct target *get_target(char* name)                       // return a target struct given by name
{
  struct target *iter;

  for( iter = targl->head; iter != NULL; iter = iter->next )
  {
      //printf("6\n");
    if( strcmp( iter->name, name ) == 0 )
      return iter; 
  }

  return NULL;
  
}

int target_check(char *name)                                // check to see if a target exists
{
  if(get_target(name) != NULL)
  {return 1;}
  else
  {return 0;}
}

struct target *list_target_get_next(void)               // get next target if it exists, ow return the head
{
  struct target *iter;

  if(targl->next_tgt == NULL)
  {
    targl->next_tgt = targl->head;
  }

  iter = targl->next_tgt;
  targl->next_tgt = iter->next;

  return iter;
}

void list_target_print(void)                            // print the target linked list with its sources and recipes
{
   struct target *iter;

   printf("Target List:  \n");

   for( iter = targl->head; iter != NULL; iter = iter->next )
   {
     printf("Target:  %s\n", iter->name );
     list_source_print(iter->source);
     list_recipe_print(iter->recipe);
   }

}
