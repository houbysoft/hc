/*    (c) 2009-2011 Jan Dlabal <dlabaljan@gmail.com>                          */
/*                                                                            */
/*     This file is part of HC (HoubySoft Calculator).                        */
/*                                                                            */
/*     HC (HoubySoft Calculator) is free software: you can redistribute it    */
/*     and/or modify it under the terms of the GNU General Public License as  */
/*     published by the Free Software Foundation, either version 3 of the     */
/*     License, or any later version.                                         */
/*                                                                            */
/*     HC (HoubySoft Calculator) is distributed in the hope that it will be   */
/*     useful, but WITHOUT ANY WARRANTY; without even the implied warranty of */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          */
/*     GNU General Public License for more details.                           */
/*                                                                            */
/*     You should have received a copy of the GNU General Public License      */
/*     along with HC (HoubySoft Calculator). If not, see                      */
/*     <http://www.gnu.org/licenses/>.                                        */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hc.h"
#include "hc_functions.h"
#include "hash.h"
#include "hc_utils.h"


inline char *allocline(char *line)
{
  char *r = malloc(strlen(line)+2);
  if (!r)
    mem_error();
  sprintf(r,"%s\n",line);
  return r;
}


int hc_help(char *e)
{
  e = hc_result_(e);
  if (!e || !is_string(e))
  {
    free(e);
    arg_error("help() : a string argument containing (ONLY, without parentheses etc.) the name of the function/variable you want to learn about is required.");
  }
  char *search = get_string(e);
  free(e);
  unsigned int hash = simple_hash(search);
  char *print = NULL;

  if (hash == 0)
  {
    print = allocline("Welcome to HC! To evaluate an expression, simply type it and press enter. To get a list of all operators you can use, type help(\"operators\"). To get a list of functions you can use, press TAB twice, and to get more details about a function, call help(\"nameoffunction\"). To get more information about conversion functions, type help(\"conversions\"). To get more information about logic and programming, type help(\"logic\"). For information about complex numbers in HC, type help(\"complex\"). There are also a few configuration directives you can use to switch between modes etc.; to learn more, type help(\"directives\").\n\nTo get credits information, simply type credits. Thanks for using HC, and please do not hesitate to suggest new features or report bugs at bugs.houbysoft.com.");
  } else {
    unsigned int i = 0;
    unsigned int needmem = 0;
    for (; i < HC_NAMES; i++)
    {
      if (hash == hc_hashes[i])
      {
        needmem += strlen(hc_names[i][0]);
        if (strstr(hc_names[i][1],"func") == hc_names[i][1])
          needmem += 2 + (strlen(hc_names[i][1]) - 5); // +2 for parenthesis, rest for arguments (-5 to remove "func:")
        needmem += 5; // strlen(" - ") + \n\0 at the end
        if (strlen(hc_names[i][4]))
        {
          needmem += strlen(hc_names[i][4]);
        } else if (strlen(hc_names[i][3]))
        {
          needmem += strlen(hc_names[i][3]);
        } else {
          print = allocline("Sorry, no help is available for the topic you specified.");
          break;
        }
        print = malloc(needmem); if (!print) mem_error();
        if (strstr(hc_names[i][1],"func") == hc_names[i][1])
          sprintf(print,"%s(%s) - %s\n",hc_names[i][0],(char *)(hc_names[i][1] + 5),strlen(hc_names[i][4]) == 0 ? hc_names[i][3] : hc_names[i][4]);
        else
          sprintf(print,"%s - %s\n",hc_names[i][0],strlen(hc_names[i][4]) == 0 ? hc_names[i][3] : hc_names[i][4]);
        break;
      }
    }
    if (i == HC_NAMES)
    {
#ifndef HCG
      print = malloc(strlen("Sorry, no help was found for \"\". Try hitting TAB twice to get a list of functions.\n")+strlen(search)+1);
#else
      print = malloc(strlen("Sorry, no help was found for \"\".\n")+strlen(search)+1);
#endif
      if (!print)
        mem_error();
#ifndef HCG
      sprintf(print,"Sorry, no help was found for \"%s\". Try hitting TAB twice to get a list of functions.\n",search);
#else
      sprintf(print,"Sorry, no help was found for \"%s\".\n",search);
#endif
    }
  }

  notify_nonmodal(print);
  free(print);
  free(search);
  return SUCCESS;
}
