/* CMPSC 311, Spring 2013, Project 8
 *
 * Author:   Joshua Kuiros
 * Email:    jjk523@psu.edu
 */

#ifndef CMPSC311_RECIPE_H
#define CMPSC311_RECIPE_H

struct recipe               // recipe node prototype
{
  struct recipe *next;      // used to iterate through recipe list
  char *name;               // name of recipe
};

struct list_recipe
{
  struct recipe *head;
  struct recipe *tail;
  struct recipe *next_rcp;
};

// function prototypes
void recipe_find_target(char *name);
struct list_recipe *list_recipe_init(void);
void list_recipe_new(char *name, struct list_recipe *node);
void list_recipe_print(struct list_recipe *node);

#endif
