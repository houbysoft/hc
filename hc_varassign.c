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
/*     You should have received a copy of the GNU General Public License      */
/*     along with HC (HoubySoft Calculator). If not, see <http://www.gnu.org/licenses/>.*/


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <m_apm.h>
#include "hc.h"
#include "hc_functions.h"
#include "hash.h"


struct hc_ventry *hc_var_first;
char hc_check_funcname(char *e);
char hc_check_varname(char *e);
char hc_check_not_recursive(char *n, char *e);


void hc_varassign(char *e)
{
  if (!strchr(e,'='))
    varname_error();
  char *expr = strchr(e,'=') + sizeof(char);
  strchr(e,'=')[0] = '\0';
  char *var = e;

  if (!var || !expr)
    varname_error();

  // clean up var name
  while (isspace(var[0]))
    var+=sizeof(char);
  int i=0;
  while (!isspace(var[i]) && var[i])
    i++;
  var[i]=0;

  if (!hc_check_varname(var))
  {
    if (!hc_check_funcname(var))
    {
      varname_error();
    } else {
      // function
      char *name = strtok(var,"(");
      char *args = strtok(NULL,"(");
      args[strlen(args)-1]=0;

      if (!hc_check_not_recursive(name,expr))
	recursive_error();

#ifdef DBG
      printf("[%s] [%s]\n",name,args);
#endif

      struct hc_ventry *tmp = hc_var_first;
      while ((tmp->next) && (tmp->name!=NULL) && (strcmp(tmp->name,name)!=0))
	tmp = tmp->next;
      if ((tmp->name!=NULL) && (strcmp(tmp->name,name)!=0))
      {
	tmp->next = malloc(sizeof (struct hc_ventry));
	if (!tmp->next)
	  mem_error();
	tmp = tmp->next;
	tmp->next = NULL;
	tmp->name = NULL;
      }
      
      tmp->type = HC_USR_FUNC;
      tmp->args = malloc((strlen(args)+1) * sizeof(char));
      if (!tmp->args)
	mem_error();
      strcpy(tmp->args,args);
      if ((tmp->name==NULL) || (strcmp(tmp->name,name)!=0))
      {
	// new function name
	tmp->name = malloc((strlen(name)+1) * sizeof(char));
	if (!tmp->name)
	  mem_error();
	strcpy(tmp->name,name);
      } else {
	// function name already defined, just changing expression
	free(tmp->value);
      }
      tmp->value = malloc((strlen(expr)+1) * sizeof(char));
      if (!tmp->value)
	mem_error();
      strcpy(tmp->value,expr);
    }

  } else {

    // variable
    char *value = hc_result_(expr);
    if (!value)
      return;

    struct hc_ventry *tmp = hc_var_first;
    while ((tmp->next) && (tmp->name!=NULL) && (strcmp(tmp->name,var)!=0))
      tmp = tmp->next;
    if ((tmp->name!=NULL) && (strcmp(tmp->name,var)!=0))
    {
      tmp->next = malloc(sizeof (struct hc_ventry));
      if (!tmp->next)
	mem_error();
      tmp = tmp->next;
      tmp->next = NULL;
      tmp->name = NULL;
    }

    tmp->type = HC_USR_VAR;
    tmp->args = NULL;
    if ((tmp->name==NULL) || (strcmp(tmp->name,var)!=0))
    {
      // new variable name
      tmp->name = malloc((strlen(var)+1) * sizeof(char));
      if (!tmp->name)
	mem_error();
      strcpy(tmp->name,var);
    } else {
      // variable name already defined, just changing value
      free(tmp->value);
    }
    tmp->value = value;
  }
}


char hc_check_funcname(char *f)
{
  // strtok modifies its argument, this would mess up the rest, so let's copy f to a temporary buffer
  char *e = malloc((strlen(f)+1)*sizeof(char));
  strcpy(e,f);

  if (strchr(e,'(')==NULL || e[strlen(e)-1]!=')')
  {
    free(e);
    return 0;
  }

  char *name = strtok(e,"(");
  char *args = strtok(NULL,"(");
  args[strlen(args)-1]=0; // delete the trailing )

  if (!hc_check_varname(name))
  {
    free(e);
    return 0;
  }

  char *TMP = args;
  char *checkme;
  while ((checkme = strtok(TMP,","))!=NULL)
  {
    if (TMP)
      TMP = NULL;
    if (!hc_check_varname(checkme))
    {
      free(e);
      return 0;
    }
  }

  free(e);
  return 1;
}


char hc_check_varname(char *e)
{
  unsigned int ii = 0;
  unsigned int name_hash = simple_hash(e);
  for (; ii < HC_FNAMES; ii++)
  {
    if (name_hash == simple_hash((char *)hc_fnames[ii][0]))
    {
      if (!graphing_ignore_errors)
        varname_predefined_error()
      else
	return 0;
    }
  }

  unsigned int i = 0;
  while (e[i]!=0)
  {
    if ((i==0) && ((!isalpha(e[i])) || (tolower(e[i])=='e') || (tolower(e[i])=='i')))
      return 0;
    else
      if (!isalnum(e[i]))
	return 0;
    i++;
  }

  return 1;
}


char hc_check_not_recursive(char *n, char *e)
{
  // this functions has to check for definitions which would lead to recursion, such as:
  // f(x) = f(x) + 1
  char *found = e;
  while ((found = strstr(found,n))!=NULL)
  {
    if ((found != e) && ((!isalpha((char)(found-sizeof(char))[0])) && !isalpha((char)(found+strlen(n)*sizeof(char))[0])))
      return 0;
    if ((found == e) && !isalpha((char)(found + strlen(n)*sizeof(char))[0]))
      return 0;
    found++;
  }
  return 1;
}
