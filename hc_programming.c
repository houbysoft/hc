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
#include <m_apm.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "hc_complex.h"
#include "hc_functions.h"
#include "hc.h"
#include "hc_programming.h"
#include "hc_utils.h"


#define return_result(r) {\
    *type = hc_get_type(r);\
    switch (*type)\
    {\
    case HC_VAR_NUM:\
      hc_2num_nr(res_re,res_im,r);\
      free(r);\
      break;\
    case HC_VAR_EMPTY:\
      *res_str = NULL;\
      free(r);\
      break;\
    case HC_VAR_STR:\
    case HC_VAR_VEC:\
      *res_str = r;\
      break;\
    case HC_VAR_INVALID:\
      free(r);\
      arg_error("received invalid response as result. This should not happen, please report it.");\
    }\
    return SUCCESS;\
}

#define HC_EXEC_RESULT {\
    char *tmpres = hc_result_(expr);\
    if (!tmpres)\
    {\
      error = 1;\
    } else if (!strlen(tmpres))\
    {\
      free(tmpres);\
    } else {\
      free(expr_orig); return tmpres;\
    }\
}


M_APM iz_test;
char iz_init = 0;


char is_zero(char *str);


char *hc_result_mul(char *e)
{
  char error = 0;
  if (!e)
    return NULL;
  char *expr = strdup(e);
  char *expr_orig = expr;
  if (!expr)
    mem_error();
  int pos=0;
  char stop=0;
  int in_par=0;
  char ignore=FALSE;
  while (expr[pos]!='\0')
  {
    if (expr[pos]=='\"')
    {
      ignore = ignore == TRUE ? FALSE : TRUE;
      pos++;
      continue;
    }
    if (ignore)
    {
      pos++;
      continue;
    }
    if (expr[pos]=='(')
      in_par++;
    if (expr[pos]==')')
      in_par--;
    if ((expr[pos]==';' && !in_par) || stop)
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
    // meh, kind of useless, removing. warning("Warning : you should end your command with ';'.\n");
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


int hc_if(char *e, M_APM res_re, M_APM res_im, char **res_str, char *type)
{
  char *cond = hc_get_arg_r(e,1);
  char *true = hc_get_arg(e,2);
  char *false = hc_get_arg(e,3);
  char *r = NULL;
  if (!cond || !true)
  {
    free(cond); free(true); free(false);
    arg_error("if() : at least two arguments are needed (condition, true_branch, false_branch)");
  }
  if (!is_zero(cond))
  {
    r = hc_result_(true);
    if (!r)
    {
      free(cond); free(true); free(false);
      return FAIL;
    }
  } else {
    if (false)
    {
      r = hc_result_(false);
      if (!r)
      {
        free(cond); free(true); free(false);
        return FAIL;
      }
    } else {
      r = NULL;
      *type = HC_VAR_EMPTY;
    }
  }
  free(cond); free(true); free(false);
  if (r)
  {
    return_result(r);
  }
  return SUCCESS;
}


int hc_for(char *e, M_APM res_re, M_APM res_im, char **res_str, char *type)
{
  free(hc_get_arg_r(e,1));
  char *cond = hc_get_arg(e,2);
  char *iter = hc_get_arg(e,3);
  char *body = hc_get_arg(e,4);
  if (!cond || !iter || !body)
  {
    free(cond); free(iter); free(body);
    arg_error("for() : 4 arguments are required (init, cond, iter, body)");
  }

  char *condr = hc_result_(cond);
  char *tmpr = NULL;
  while (condr && !is_zero(condr))
  {
    tmpr = hc_result_(body);
    if (!tmpr)
    {
      free(cond); free(iter); free(body); free(condr);
      return FAIL;
    } else if (strlen(tmpr))
    {
      free(cond); free(iter); free(body); free(condr);
      return_result(tmpr);
    } else {
      free(tmpr);
    }
    tmpr = hc_result_(iter);
    if (!tmpr)
    {
      free(cond); free(iter); free(body); free(condr);
      return FAIL;
    } else {
      free(tmpr);
    }
    free(condr); condr = hc_result_(cond);
  }
  free(cond); free(iter); free(body);
  if (!condr)
  {
    return FAIL;
  } else {
    free(condr);
    *type = HC_VAR_EMPTY;
    return SUCCESS;
  }
}


int hc_while(char *e, M_APM res_re, M_APM res_im, char **res_str, char *type)
{
  char *cond = hc_get_arg(e,1);
  char *body = hc_get_arg(e,2);
  if (!cond || !body)
  {
    free(cond); free(body);
    arg_error("while() : 2 arguments are required (cond,body)");
  }

  char *condr = hc_result_(cond);
  char *tmpr = NULL;
  while (condr && !is_zero(condr))
  {
    tmpr = hc_result_(body);
    if (!tmpr)
    {
      free(cond); free(body); free(condr);
      return FAIL;
    } else if (strlen(tmpr))
    {
      free(cond); free(body); free(condr);
      return_result(tmpr);
    } else {
      free(tmpr);
    }
    free(condr); condr = hc_result_(cond);
  }
  free(cond); free(body);
  if (!condr)
  {
    return FAIL;
  } else {
    free(condr);
    *type = HC_VAR_EMPTY;
    return SUCCESS;
  }
}


char is_zero(char *str)
{
  if (!iz_init)
  {
    iz_test = m_apm_init();
    iz_init = 1;
  }
  m_apm_set_string(iz_test,str);
  if (m_apm_compare(iz_test,MM_Zero)==0)
  {
    return 1;
  } else {
    return 0;
  }
}


void is_zero_free()
{
  if (iz_init)
  {
    m_apm_free(iz_test);
    iz_init = 0;
  }
}
