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
#include "target.h"
#include "recipe.h"
#include "names.h"

//------------------------------------------------------------------------------
struct list_source *list_source_init(void)                          // create a source linked list
{
  struct list_source *s = Malloc(sizeof(struct list_source));

  s->head = NULL;
  s->tail = NULL;
  s->next_src = NULL;

  return s;
}
void list_source_new(char *name, struct list_source *node)            // add new source node
{
  struct source *s = Malloc(sizeof(struct source));

  if(node->head == NULL)
  {
    node->head = s;
    node->tail = s;
  }
  else
  {
    (node->tail)->next = s;
    node->tail = s;  
  }

  s->next = NULL;  
  s->name = Strdup(name);

  

}
void list_source_print(struct list_source *node)                // print source list
{
  struct source *iter;

  for( iter = node->head; iter != NULL; iter = iter->next )
  {
    printf("    Source:  %s\n", iter->name);
  }

}
void source_string_manip(char *source_name, char *target_name)       // manipulate source string before adding
{
    struct target *t = get_target(target_name);
    
    while(source_name != NULL)
    {
        char *temp_space = strchr(source_name, ' ');
        
        if(temp_space)
        {
            *temp_space = '\0';
            
            list_source_new(source_name, t->source);
            
            
            temp_space++;
            
            source_name = temp_space;
            
            while(*source_name == ' ' || *source_name == '\t')
            {
                source_name++;
            }
        }
        else
        {
            list_source_new(source_name, t->source);
            source_name = NULL;
        }
    }
}

//------------------------------------------------------------------------------
