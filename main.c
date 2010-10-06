/*    (c) 2009-2010 Jan Dlabal <dlabaljan@gmail.com>                          */
/*                                                                            */
/*     This file is part of HC (HoubySoft Calculator).                                  */
/*                                                                            */
/*     HC (HoubySoft Calculator) is free software: you can redistribute it and/or modify*/
/*     it under the terms of the GNU General Public License as published by   */
/*     the Free Software Foundation, either version 3 of the License, or      */
/*     any later version.                                                     */
/*                                                                            */
/*     HC (HoubySoft Calculator) is distributed in the hope that it will be useful,     */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          */
/*     GNU General Public License for more details.                           */
/*                                                                            */
/*     You should have received a copy of the GNU General Public License     */
/*     along with HC (HoubySoft Calculator). If not, see <http://www.gnu.org/licenses/>.*/

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef I_HATE_TIPS
#include <time.h>
#endif
#include <string.h>
#include "hc.h"
#include "hc_functions.h"


#ifndef I_HATE_TIPS
char *tips[] = {
  "We may be in the matrix right now.",
  "To report a bug or suggest a feature, you can go to bugs.houbysoft.com.",
  "If you don't like these tips, you can recompile HC with the -DI_HATE_TIPS flag.",
  "To get the factorial of n, type factorial(n) or use the '!' notation.",
  "To recall the last result, you can use ans().",
  "You can use \\rpn to switch between RPN-mode and infix-mode.",
  "You can use \\deg, \\rad, and \\grad to switch the angle mode.",
  "HC is also a good converter. For example, to convert 1 US Fl Oz to mL, type floztoml(1). Type conversions to get a list of other built-in conversions!",
  "You can type a constant's name with english characters to use it in a calculation -- for example, try pi.",
  "This calculator can provide you The Answer.",
  "From version 0.4.1, you can use the '!' notation for factorial and the '%' notation for modulus.",
  "You can use implied multiplication in hc.",
  "You can graph multiple functions on one graph with the gmul() function, or with the graph() function, passing a list of functions as the function (for example, graph([x,x^2,x^3])).",
  "You can enter numbers not only in decimal, but also hexadecimal and binary form, using the 0x and 0b prefixes respectively.",
  "Need help about something? Call help(\"nameoftopic\"), for example help(\"totient\") to get help about the totient function.",
  "There is an easy way to reuse the last result. Call help(\"ans\") for more details."
};
#define TIP_COUNT 16
#endif


#ifdef WIN32
void exit_fn() {printf("Press Enter to close this window.");getchar();}
#endif

char **hc_completion(const char *text, int start, int end);
char *hc_cmd_generator(const char *text, int state);


int main(int argc, char *argv[])
{
  char c;
  char err=0;
  char *eval = NULL;
  char *import = NULL;
  while ((c = getopt(argc, argv, "e:i:"))!=-1)
    switch (c)
    {
    case 'e':
      eval = optarg;
      break;
    case 'i':
      import = optarg;
      break;
    default:
      err++;
    }

  if (err || ((eval || import) && argv[optind]!=NULL) || (!eval && !import && (argc!=2 && argc!=1)))
  {
    printf("%s\nUsage:\n%s [[-i] FILE] [-e EXPRESSION]\n - use either FILE to load FILE into HC and execute it (add -i to leave the calculator open after execution)\n - OR use -e EXPRESSION to directly compute EXPRESSION and exit\n - OR pass no arguments to launch the default CLI interface and have access to more help and options.\n",eval ? "Invalid arguments (did you enclose the expression to calculate in quotes?)." : "Invalid arguments.",argv[0]);
    exit(1);
  }

  if (eval)
  {
    hc.announce = FALSE;
    char *fme = hc_result(eval);
    if (fme && strlen(fme))
      printf("%s",fme);
    free(fme);
    return 0;
  } else if (argv[optind]!=NULL) {
    hc.announce = FALSE;
    hc_load(argv[optind]);
    return 0;
  } else {
    hc.announce = TRUE;
    if (import)
      hc_load(import);
  }

#ifdef WIN32
  atexit(exit_fn);
#endif
  char *fme;

  using_history();
  rl_readline_name = "HC";
  rl_attempted_completion_function = hc_completion;
  rl_basic_word_break_characters = " \t\n\"'`@$><=;|&{(*/+-)^,!%";

  printf("%s(type help for help, and exit to exit)\n",NAME_VERSION);

#ifndef I_HATE_TIPS
  srand(time(NULL));
  printf("Did you know? %s\n",tips[rand()%TIP_COUNT]);
#endif

  hc_load_cfg();

  char *expr;

  while (42)
  {
    expr = readline("> ");
    if (!expr)
      exit(1);
    while (!check_completeness(expr))
    {
      char *expr2 = readline(" . ");
      if (!expr2)
	exit(1);
      expr = realloc(expr,strlen(expr)+strlen(expr2)+2); // +2 = space and \0
      strcat(expr," ");
      strcat(expr,expr2);
      free(expr2);
    }

    if (strlen(expr)>=MAX_EXPR)
      overflow_error();
    if (strcmp(expr,"exit")==0 || strcmp(expr,"quit")==0)
      exit(0);
    fme = hc_result(expr);
    if (fme && strlen(fme))
    {
      printf(" %s\n",fme);
    }
    if (fme)
      free(fme);
    add_history(expr);
    free(expr);
  }

  return 0;
}


char **hc_completion(const char *text, int start, int end)
{
  rl_completion_suppress_append = 1;
  char **matches = (char **)NULL;
  matches = rl_completion_matches(text, hc_cmd_generator);
  return matches;
}


char *hc_cmd_generator(const char *text, int state)
{
  static int list_index, len;
  static struct hc_ventry *var_tmp;
  char *name;

  if (!state)
  {
    list_index = 0;
    len = strlen (text);
    var_tmp = hc_var_first;
  }

  char isfunc;

  /* Return the next name which partially matches from the command list. */
  while (list_index < HC_NAMES)
  {
    name = (char *)hc_names[list_index][0];
    if (strcmp((char *)hc_names[list_index][1],"func")==0)
      isfunc = 1;
    else
      isfunc = 0;
    list_index++;

    if (strncmp (name, text, len) == 0)
    {
      char *ret;
      if (!isfunc)
	ret = malloc(strlen(name)+1);
      else
	ret = malloc(strlen(name)+2);
      if (!ret)
	mem_error();
      strcpy(ret,name);
      if (isfunc)
	strcat(ret,"(");
      return ret;
    }
  }
  
  while (var_tmp)
  {
    if (var_tmp->name)
    {
      name = var_tmp->name;
      if (var_tmp->type == HC_USR_FUNC)
	isfunc = 1;
      else
	isfunc = 0;
      if (var_tmp->next)
	var_tmp = var_tmp->next;
      else
	var_tmp = NULL;

      if (strncmp (name, text, len) == 0)
      {
	char *ret;
	if (!isfunc)
	  ret = malloc(strlen(name)+1);
	else
	  ret = malloc(strlen(name)+2);
	if (!ret)
	mem_error();
	strcpy(ret,name);
	if (isfunc)
	  strcat(ret,"(");
	return ret;
      }
    } else {
      var_tmp = NULL;
      break;
    }
  }

  /* If no names matched, then return NULL. */
  return ((char *)NULL);
}
