/* CMPSC 311, Spring 2013, Project 8
 *
 * Author:   Joshua Kuiros
 * Email:    jjk523@psu.edu
 *
 */
#ifndef CMPSC311_TARGET_H
#define CMPSC311_TARGET_H


#include "recipe.h"
#include "source.h"
#include <stdbool.h>


struct target                       // prototype for a target node
{
  struct target *next;              // used to iterate through list
  char *name;                       // name of target
  struct list_recipe *recipe;       // recipe list attached to target node
  struct list_source *source;       // source list attached to target node
  bool is_last_targ;                // check if target was the last one added
};

struct list_target                  // prototype for a target linked list
{
  struct target *head;
  struct target *tail;
  struct target *next_tgt;
};

// function prototypes
void list_target_init(void);
void list_target_new_target(char *name);
struct target *get_target(char* name);
int list_target_number(void);
int target_check(char *name);
struct target *list_target_get_next(void);
void list_target_print(void);
struct target *get_targl_head();


#endif
