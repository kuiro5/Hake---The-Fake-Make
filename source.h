/* CMPSC 311, Spring 2013, Project 8
 *
 * Author:   Joshua Kuiros
 * Email:    jjk523@psu.edu
 *
 */

#ifndef CMPSC311_SOURCE_H
#define CMPSC311_SOURCE_H

#include "target.h"

struct source               // source node prototype
{
  struct source *next;      // used to iterate through the source list
  char *name;               // name of source
};

struct list_source          // source linked list prototype
{
  struct source *head;
  struct source *tail;
  struct source *next_src;  // next source in linked list
};

// function prototypes
struct list_source *list_source_init(void);
void list_source_new(char *name, struct list_source *node);
void list_source_print(struct list_source *node);
void source_string_manip(char *source_name, char *target_name);


#endif
