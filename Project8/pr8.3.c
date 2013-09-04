/* CMPSC 311, Spring 2013, Project 8 
 *
 * Author:   Joshua Kuiros
 * Email:    jjk523@psu.edu
 *
 * Hake -- a fishy version of Make
 *
 * version 2, 25 Feb. 2013
 *   all required features implemented
 * version 1, 11 Feb. 2013
 *   starter kit, main(), not much else
 *   Makefile
 * 
 * Command-line Options
 *    -h           print help
 *    -v           verbose mode; enable extra printing; can be repeated
 *    -f file      input filename; default is hakefile or Hakefile
 *    -n 	   print hake commands - enabled by default
 *
 */

//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <stdbool.h>

#include "cmpsc311.h"
#include "names.h"
#include "macro.h"
#include "target.h"
#include "source.h"
#include "recipe.h"

//------------------------------------------------------------------------------
// global data
struct timespec mtime(const char *file);
char *goal = NULL;
bool out_of_func = true;

// function prototypes
static int read_file(char *filename, int quiet);
static void read_lines(char *filename, FILE *fp);
void pr8_work(char *goal);


// defintions
#define MAXLINE 4096
#define MAXTARGS 200

//------------------------------------------------------------------------------
// struct definition
struct timespec mtime(const char *file)
{
    // last-modification time of a file
    
    // The appropriate field of struct stat varies according to the OS.
    //   Posix, 2001        time_t st_mtime
    //   Posix, 2004        time_t st_mtime
    //   Posix, 2008        struct timespec st_mtim
    //                      macro st_mtime is st_mtim.tv_sec
    //   Solaris            time_t st_mtime
    //   Linux              time_t st_mtime
    //   Mac OS X           struct timespec st_mtimespec
    //                      or, st_mtime
    
    // The struct timespec fields are defined in <time.h>.
    //   time_t tv_sec;     // seconds
    //   long   tv_nsec;    // nanoseconds
    
    // The type time_t (time in seconds) is defined in <time.h> and <sys/types.h>.
    // The Posix Standard allows time_t to be an integer, or a real-floating type.
    // If time_t isn't an integer, we would be surprised, but the code should
    //   still work properly.
    
    // A nonexistent file is treated as extremely old.
    
    struct stat s;
    struct timespec t = { 0, 0 };
    //printf("inside time function\n");
    
    if (stat(file, &s) == 0)
#if   defined(MTIME) && MTIME == 1      // Linux
    { t = s.st_mtim; }
#elif defined(MTIME) && MTIME == 2      // Mac OS X
    { t = s.st_mtimespec; }
#elif defined(MTIME) && MTIME == 3      // Mac OS X, with some additional settings
    { t.tv_sec = s.st_mtime; t.tv_nsec = s.st_mtimensec; }
#else                                   // Solaris
    { t.tv_sec = s.st_mtime; }
#endif
    
    return t;
}
//------------------------------------------------------------------------------
// function implementations
static void usage(int status)
{
    if (status == EXIT_SUCCESS)
    {
        printf("usage: %s [-h] [-v] [-f file]\n", prog);
        printf("  -h           print help\n");
        printf("  -v           verbose mode; enable extra printing; can be repeated\n");
        printf("  -f file      input filename; default is hakefile or Hakefile\n");
        printf("  -n           print hake commands - enabled by default\n");
    }
    else
    {
        fprintf(stderr, "%s: Try '%s -h' for usage information.\n", prog, prog);
    }
    
    exit(status);
}

void pr8_work(char *goal)
{
    if (goal == NULL) return;
    
    int rec_print = 0;                                          // initially set printing to false
    
    if( target_check( goal ) )
    {
        struct target *temp_tgt = get_target(goal);           // get the goal target
        
        struct source *iter = (temp_tgt->source)->head;       // get head of source list
        while( iter != NULL)
        {
            out_of_func = false;                              // set boolean to be inside the fnction
            pr8_work(iter->name);
            
            
            struct timespec t1 = mtime(temp_tgt->name);         // target
            struct timespec t2 = mtime(iter->name);             // source
            
            if ((t1.tv_sec == 0)                                //if current target is older than current source
                || (t1.tv_sec < t2.tv_sec)
                || ((t1.tv_sec == t2.tv_sec) && (t1.tv_nsec < t2.tv_nsec))) {
                
                rec_print = 1;
            }
            iter = iter->next;                                // iterate through source
        }
        
        if(rec_print == true)                                         // print recipes if current target is older than current source
        {
            list_recipe_print(temp_tgt->recipe);
        }
        else if( out_of_func == true)                                 // if call was out of function then target it up to date
        {
            printf("%s: '%s' is up to date.\n",prog, temp_tgt->name);
        }
    }
	else
	{
        struct timespec t2 = mtime(goal);  // source
        
        if (t2.tv_sec == 0)
        {
            /* source does not exist*/
            printf("Fatal error: Don't know how to make target '%s'\n", goal);
            exit(EXIT_FAILURE);
        }
	}
    
    return;
}

int main(int argc, char *argv[])
{
  // for use with getopt(3)
  int ch = 0;
  int status = EXIT_SUCCESS;
  int i;
  extern char *optarg;
  extern int optind;
  extern int optopt;
  extern int opterr;
  out_of_func = true;

  // create the target list
  list_target_init();

  // set program name
  prog = argv[0];

  // read file
  // option flags and option-arguments set from the command line
  int f_flag = 0;	// number of -f options supplied

  // first pass, everything except -f options (let the -v options accumulate)
  optind = 1;
  while ((ch = getopt(argc, argv, ":hvnf:")) != -1)
    {
      switch (ch) {
        case 'h':
          usage(EXIT_SUCCESS);
          break;
        case 'v':
          verbose++;
          break;
        case 'n':
          break;
	case 'f':
	      f_flag++;
          (void)read_file(optarg,0);
	  break;
        case '?':
          fprintf(stderr, "%s: invalid option '%c'\n", prog, optopt);
          usage(EXIT_FAILURE);
          break;
        case ':':
          fprintf(stderr, "%s: invalid option '%c' (missing argument)\n", prog, optopt);
          usage(EXIT_FAILURE);
          break;
        default:
          usage(EXIT_FAILURE);
          break;
      }
    }

    if (f_flag == 0 && !read_file("hakefile", 1) && !read_file("Hakefile", 1))
    {
      fprintf(stderr, "%s: no input\n", prog);
      usage(EXIT_FAILURE);
    }

    //list_target_print();
  
    goal = ( list_target_get_next() )->name;        // get name of targl->next_tgt

    if( optind == argc )
    {
        pr8_work(goal);
    }
    else
    {
        for(i = optind; i < argc; i++) 
        {
          pr8_work(argv[i]);
        }
    }

  return 0;
}

//------------------------------------------------------------------------------

// return 1 if successful, 0 if not
// "success" means the file could be opened for reading, or that we had seen
//    the file before and don't need to read it again
// quiet == 0 enables error messages if the file can't be opened
// quiet == 1 suppresses error messages if the file can't be opened

static int read_file(char *filename, int quiet)
{
  verify(filename != NULL, "null arg filename");
  verify(filename[0] != '\0', "empty arg filename");

  if (verbose > 0)
    { fprintf(stderr, "%s: read_file(%s)\n", prog, filename); }

  // file names come from -f and include
  static struct list_names * filenames = NULL;
  if (filenames == NULL) filenames = list_names_allocate("filenames");
    // by construction, filenames is now not NULL

  if (strcmp(filename, "-") == 0)
    { read_lines("[stdin]", stdin); return 1; }

  FILE *fp = fopen(filename, "r");
  if (fp == NULL)
    {
      if (quiet == 0)
	fprintf(stderr, "%s: could not open input file %s: %s\n", prog, filename, strerror(errno));
      return 0;
    }

  read_lines(filename, fp);

  if (fclose(fp) != 0)
    {
      fprintf(stderr, "%s: could not close input file %s: %s\n", prog, filename, strerror(errno));
    }

  return 1;
}

//------------------------------------------------------------------------------

static void read_lines(char *filename, FILE *fp)
{
  verify(filename != NULL, "null arg filename");
  verify(filename[0] != '\0', "empty arg filename");
  verify(fp != NULL, "null arg fp");

  char original[MAXLINE+2];	// from fgets()
  char expanded[MAXLINE+2];	// after macro expansion (which isn't implemented here)
  char buffer[MAXLINE+2];	// working copy, safe to modify

  char whsp[] = " \t\n\v\f\r";			// whitespace characters
  int line_number = 0;
  int recipe_line_number = 0;
  bool have_target = false;


  while (fgets(original, MAXLINE, fp) != NULL)
  {
    original[MAXLINE] = '\n';
    original[MAXLINE+1] = '\0';

    line_number++;
    if (verbose > 0) printf("%s: %s: line %d: %s", prog, filename, line_number, original);

    strcpy(expanded, original);			// later, do macro expansion
    macro_expand(original, expanded);
    strcpy(buffer, expanded);			// copy, safe to modify

    char *buf = buffer;

    while (*buf == ' ') buf++;			// skip past leading spaces (not tabs!)

    char *p_hash = strchr(buf, '#');		// a comment starts with #
    if (p_hash != NULL)
      { *p_hash = '\0'; }			// remove the comment

    int n = 0;					// remove trailing whitespace
    while (buf[n] != '\0')
    {
        int n1 = strspn(&buf[n], whsp);		// buf[n .. n+n1-1] is whitespace
        int n2 = strcspn(&buf[n + n1], whsp);	// buf[n+n1 .. n+n1+n2-1] is not
        if (n2 == 0) { buf[n] = '\0'; break; }	// remove trailing whitespace
        n += n1 + n2;
    }

    if (buf[0] == '\0')				// nothing left?
      { continue; }

    char *p_colon = strchr(buf, ':');		// : indicates a target-prerequisite line
    char *p_equal = strchr(buf, '=');		// = indicates a macro definition

    if (buffer[0] == '\t')
    {
        recipe_line_number++;
   
        // (save this for a later project)
        
        char *new_rec = Strdup(buf);        // copy buffer 
        if(have_target == false)               
        { continue; }
        else
        {
            while(*new_rec == ' ' || *new_rec == '\t')
            {
                new_rec++;
            }
            recipe_find_target(new_rec);
        }
      }
    else if (p_colon != NULL)
    {
          
        have_target = true;                              // check for target
        recipe_line_number = 0;                         // set recipe line number to 0
        
        struct target *t = malloc(sizeof(struct target));
        
        for( t = get_targl_head(); t != NULL; t = t->next)      // reset all last targets to false
        {
            t->is_last_targ = false;
        }
        
        char *tgt_str = buffer;
        while (*tgt_str == ' ' || *tgt_str == '\t')       // skip past spaces and tabs
        {
            tgt_str++;
        }
        char *tgt_fnh = p_colon-1;
        while (*tgt_fnh == ' ' || *tgt_fnh == '\t')
        {
            tgt_fnh--;
        }
        tgt_fnh++;
        *tgt_fnh = '\0';
        
        char *src_str = p_colon+1;
        while (*src_str == ' ' || *src_str == '\t')       // skip past spaces and tabs
        {
            //printf("5\n");
            src_str++;
        }
        
        char *tgt_iter = Strdup(tgt_str);               // copy the target string
        char *src_iter = Strdup(src_str);               // copy the source string
        
        int tgt_len = (int)strlen(tgt_str);
        
        int tgt_space = (int)strcspn(tgt_str, " ");
        
        
        if(tgt_space < tgt_len)            // check if the first space comes before the end of the target for multiple targets
        {
            while(tgt_iter != NULL)
            {
                char *temp_space = strchr(tgt_iter, ' ');               // finds the single space character
                
                src_iter = Strdup(src_str);                             // resets temporary source each time
                if(temp_space != NULL)                                  // checks that the single space character has been found
                {
                    *temp_space = '\0';                                 // set space to null
                    
                    list_target_new_target(tgt_iter);                   // add new target
                    source_string_manip(src_iter, tgt_iter);            // add new source
                    
                    temp_space++;                                       // increment to next target
                    tgt_iter = temp_space;                              // get next instance of multiple target
                    
                    while(*tgt_iter == '\t' || *tgt_iter == ' ' )       // skip past white space and tabs
                    {
                        tgt_iter++;
                    }
                }
                else                                                    // if no space was found (error checking)
                {
                    list_target_new_target(tgt_iter);                   // add new target
                    
                    source_string_manip(src_iter, tgt_iter);            // add new source
                    
                    tgt_iter = NULL;
                }
                
            }
        }
        else                                                            // no space was found initially
        {
            list_target_new_target(tgt_iter);                           // add new target
            
            source_string_manip(src_iter, tgt_iter);                    // add new source
            
            src_iter = src_str;
        }
        
        
    }
    else if (p_equal != NULL)
    {
        //if (verbose > 0) printf("  diagnosis: macro definition\n");
	// (save this for a later project)
         have_target = false;
          char *name_start = buf;
          while (*name_start == ' ' || *name_start == '\t')	// skip past spaces and tabs
          { name_start++; }
          char *name_end = p_equal-1;
          while (*name_end == ' ' || *name_end == '\t')
          { name_end--; }
          name_end++;
          *name_end = '\0';
          char *body_start = p_equal+1;
          while (*body_start == ' ' || *body_start == '\t')
          { body_start++; }
          char *body_end = body_start;
          while (*body_end != '\0')	// end of string
          { body_end++; }
          while (*body_end == ' ' || *body_end == '\t')
          { body_end--; }
          body_end++;
          *body_end = '\0';
          //if (verbose > 1) macro_list_print();
          macro_set(name_start, body_start);
          if (verbose > 1) macro_list_print();
      }
    else if (strncmp("include", buf, 7) == 0)
      {
        //if (verbose > 0) printf("  diagnosis: include\n");
          have_target = false;
	char *name_start = buf + 7;				// skip past "include"
	while (*name_start == ' ' || *name_start == '\t')	// skip past spaces and tabs
	  { name_start++; }
	if (*name_start == '\0')
	  {
	    // following GNU Make, this is not an error
	    if (verbose > 0) fprintf(stderr, "%s: %s: line %d: include but no filename\n", prog, filename, line_number);
	    continue;
	  }
	else if (*name_start == '\'' || *name_start == '"')		// quoted filename
	  {
	    // find matching quote, remove it
	    char *q = name_start + 1;				// skip past ' or "
	    while (*q != *name_start && *q != '\0') q++;	// find end of string or line
	    if (*q == '\0')
	      {
		fprintf(stderr, "%s: %s: line %d: file name error [%s]\n", prog, filename, line_number, name_start);
		continue;
	      }
	    name_start++;	// skip past opening quote
	    *q = '\0';		// remove closing quote
	  }
        read_file(name_start, 0);
      }
    else
      {
        if (verbose > 0) printf("  diagnosis: something else\n");
          have_target = false;
	fprintf(stderr, "%s: %s: line %d: not recognized: %s", prog, filename, line_number, original);
      }
  }

  if (ferror(fp))	// error when reading the file
    { fprintf(stderr, "%s: %s: read error: %s\n", prog, filename, strerror(errno)); }

  return;
}

//------------------------------------------------------------------------------
