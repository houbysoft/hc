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
#include <m_apm.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "hc_complex.h"
#include "hc_functions.h"
#include "hc.h"


#define HC_EXEC_RESULT {if (strstr(strip_spaces(expr),"return ")!=strip_spaces(expr)) \
    {char *tmp = hc_result(strip_spaces(expr)); if (!tmp) {error = 1;} else {free(tmp);}}else{ \
  char *r = hc_result(strip_spaces(expr)+strlen("return "));		\
  free(expr_orig);							\
  return r;								\
    }}


char *hc_result_mul(char *e)
{
  char error = 0;
  if (!e)
    return NULL;
  char *expr = malloc(strlen(e)+1);
  char *expr_orig = expr;
  if (!expr)
    mem_error();
  strcpy(expr,e);
  int pos=0;
  char stop=0;
  while (expr[pos]!='\0')
  {
    if (expr[pos]=='{')
    {
      int par=1;
      pos++;
      while (par && expr[pos])
      {
	if (expr[pos]=='{')
	  par++;
	if (expr[pos]=='}')
	  par--;
	pos++;
      }
      if (par)
      {
	syntax_error2();
	return NULL;
      }
      if (strstr(strip_spaces(expr+sizeof(char)*pos),"else")!=strip_spaces(expr+sizeof(char)*pos))
      {
	stop = 1;
      }
    }
    if (expr[pos]==';' || stop)
    {
      char save=expr[pos];
      expr[pos]=0;
      HC_EXEC_RESULT
      expr[pos]=save;
      if (expr[pos]!=0)
	expr += sizeof(char)*(pos+1);
      else
	expr += sizeof(char)*(pos);
      stop = 0;
      pos=-1;
      if (error)
      {
	free(expr_orig);
	return NULL;
      }
    }
    if (pos==-1 || expr[pos])
      pos++;
  }
  if (strlen(strip_spaces(expr))!=0)
  {
    HC_EXEC_RESULT
      if (error)
      {
	free(expr_orig);
	return NULL;
      }
  }
  free(expr_orig);
  expr = malloc(1);
  expr[0] = 0;
  return expr;
}


char hc_prep_exec_block(char *e, int *end, int *pos)
{
  while (isspace(e[*end]))
    *end += 1;
  if (e[*end]!='{')
  {
    syntax_error2();
    return FALSE;
  }
  int par = 1;
  *pos = *end+1;
  while (par && e[*pos])
  {
    if (e[*pos]=='{')
      par++;
    if (e[*pos]=='}')
      par--;
    *pos += 1;
  }
  if (par)
  {
    syntax_error2();
    return FALSE;
  }
  return TRUE;
}


char *hc_get_cond(char *e, int *end, int result)
{
  if (e[0]!='(') // need a condition in parenthesis
  {
    syntax_error2();
    return NULL;
  }
  e++;
  int par = 1;
  *end = 0;
  while (par!=0 && e[*end]!='\0')
  {
    if (e[*end]=='(')
      par++;
    if (e[*end]==')')
      par--;
      *end += 1;
  }
  if (par)
  {
    syntax_error2();
    return NULL;
  }
  e[*end-1]=0;
  if (result)
    return hc_result(e);
  else
  {
    char *tmp = malloc(strlen(e)+1);
    strcpy(tmp,e);
    return tmp;
  }
}


char *hc_exec_struct(char *f)
{
  char *ret=NULL;
  char *e = malloc(strlen(f)+1);
  char *fme = e; // needed because the starting address of e gets modified
  if (!e)
    mem_error();
  strcpy(e,f);
  HC_EXECS type;
  if (strstr(e,"if ")==e)
    type = HC_EXEC_IF;
  if (strstr(e,"while ")==e)
    type = HC_EXEC_WHILE;
  if (strstr(e,"for ")==e)
    type = HC_EXEC_FOR;

  // Strip the structure name now that we don't need it
  while (!isspace(e[0]))
    e++;
  e++;

  char *cond = NULL;
  char *tmp = NULL;
  char *exec = NULL;
  int end;
  int pos;

  switch (type)
  {
  case HC_EXEC_IF:
    cond = hc_get_cond(e,&end,1); // the 1 means get its result immediately
    end++;
    if (!hc_prep_exec_block(e,&end,&pos)) // prepare end and pos counters to point to a suitable position so that we can easily execute the block
    {
      free(fme);
      free(cond);
      return NULL;
    }
    if (cond && strcmp(cond,"0")!=0)
    {
      e[pos-1]=0;
      ret = hc_result_mul((char *)e+sizeof(char)*(end+1));
    } else {
      char *else_ = strstr((char *)e+sizeof(char)*(pos-1),"else");
      if (else_)
      {
	else_ += sizeof(char)*strlen("else");
	else_ = strchr(else_,'{');
	else_ += sizeof(char);
	strrchr(else_,'}')[0] = 0;
	ret = hc_result_mul((char *)else_);
      }
    }
    if (ret && !strlen(ret))
    {
      free(ret);
      ret = NULL;
    }
    break;

  case HC_EXEC_WHILE:
    cond = hc_get_cond(e,&end,0); // the 0 means not to compute the result; we need to compute it every iteration
    end++;
    if (cond)
      tmp = hc_result(cond);
    else
      tmp = NULL;
    while (tmp && strcmp(tmp,"0")!=0)
    {
      if (!exec)
      {
	if (!hc_prep_exec_block(e,&end,&pos))
	{
	  free(tmp); free(cond); free(fme);
	  return NULL;
	}
	e[pos-1]=0;
	exec = ((char *)e+sizeof(char)*(end+1));
      }
      ret = hc_result_mul(exec);
      if ((ret && strlen(ret)) || !ret)
	break;
      if (ret && !strlen(ret))
      {
	free(ret);
	ret = NULL;
      }
      free(tmp);
      tmp = hc_result(cond);
    }
    free(tmp);
    break;

  case HC_EXEC_FOR:
    cond = hc_get_cond(e,&end,0); // 0 means don't compute the result; this is needed because we need to split the "condition" into the three parts
    end++;
    if (!cond)
      break;
    char *start = strtok(cond,";");
    char *cond2 = strtok(NULL,";");
    char *iter = strtok(NULL,";");
    if (!iter) // start was not passed
    {
      iter = cond2; cond2 = start; start = NULL;
    }
    if (!cond2 || !strlen(strip_spaces(cond2)))
    {
      error_nq("Error : you need to specify at least the condition for 'for'.");
      free(cond); free(fme);
      return NULL;
    }
    free(hc_result_mul(start));
    tmp = hc_result(cond2);
    while (tmp && strcmp(tmp,"0")!=0)
    {
      if (!exec)
      {
	if (!hc_prep_exec_block(e,&end,&pos))
	{
	  free(tmp); free(cond); free(fme);
	  return NULL;
	}
	e[pos-1]=0;
	exec = ((char *)e+sizeof(char)*(end+1));
      }
      ret = hc_result_mul(exec);
      if ((ret && strlen(ret)) || !ret)
	break;
      if (ret && !strlen(ret))
      {
	free(ret);
	ret = NULL;
      }
      free(tmp);
      free(hc_result_mul(iter));
      tmp = hc_result(cond2);
    }
    free(tmp);
    break;
  }
  
  free(cond);
  free(fme);
  return ret;
}
