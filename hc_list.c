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


#include <stdlib.h>
#include "hc.h"
#include "hc_functions.h"
#include "hc_list.h"


// returns 0 on error
char hc_get_by_index(char *data, char *type, char *scan, int *i)
{
  char *scanres = hc_result((char *)(scan + *i));
  *i += strlen((char *)(scan + *i));
  // FIXME : hc_result() is expensive, should use hc_result_() instead, but this needs the result to be nicely parsed (ie. the radix and zeroes after an integer stripped)
  // scan[*i] points at the '[' of the beginning of the index
  char tmp_idx[MAX_DOUBLE_STRING];
  int j=0,k=1;
  for (; scanres[k]!=']'; k++)
  {
    if (!isdigit(scanres[k]))
    {
      free(scanres);
      arg_error("only integer indexes are accepted.");
    }
    tmp_idx[j++] = scanres[k];
  }
  k++;
  tmp_idx[j] = 0;
  free(scanres);
  char *res = NULL;
  if (is_string(data))
  {
    data = strip_spaces(data);
    data[strlen(data)-1] = '\0'; // delete the last '"'
    if (atoi(tmp_idx) >= strlen((char *)(data + 1)))
      arg_error("index out of range.");
    res = malloc(4);
    if (!res)
      mem_error();
    res[0] = '"';
    res[1] = ((char *)(data + 1))[atoi(tmp_idx)];
    res[2] = '"';
    res[3] = '\0';
  } else if (is_vector(data))
  {
    data[strlen(data)-1] = '\0'; // delete the last ']'
    res = hc_get_arg((char *)(data + 1), atoi(tmp_idx) + 1);
    if (!res)
    {
      arg_error("index out of range.");
    }
  } else {
    arg_error("only vectors/lists and strings can be indexed.");
  }

  if (is_string(res))
    *type = HC_VAR_STR;
  else if (is_vector(res))
    *type = HC_VAR_VEC;
  else
    *type = HC_VAR_NUM;

  strcpy(data, res);
  free(res);

  return 1;
}


char *list_neg(char *list)
{
  list++;
  list[strlen(list)-1] = '\0';
  char *res = malloc(2);
  strcpy(res,"[");
  long idx = 1;
  char *curarg = hc_get_arg(list,idx);
  while (curarg)
  {
    char *curtmp = malloc(2+strlen(curarg)+2);
    if (!curtmp)
      mem_error();
    sprintf(curtmp,"-(%s)",curarg);
    char *curres = hc_result_(curtmp);
    if (!curres)
    {
      free(curtmp); free(curarg); free(res);
      return NULL;
    }
    res = realloc(res,strlen(curres)+strlen(res)+2);
    strcat(res,curres);
    strcat(res,",");
    free(curres); free(curtmp); free(curarg);
    curarg = hc_get_arg(list,++idx);
  }
  res[strlen(res)-1] = ']';
  return res;
}


char *list_mul_div(char *list, M_APM n_r, M_APM n_i, char *str, char mode)
{
  list++; // skip the initial '['; this assumes valid data is sent in
  list[strlen(list)-1] = '\0';
  char *n_str = NULL;
  if (!str)
  {
    char *n_str_r = m_apm_to_fixpt_stringexp(HC_DEC_PLACES, n_r, '.', 0, 0);
    char *n_str_i = m_apm_to_fixpt_stringexp(HC_DEC_PLACES, n_i, '.', 0, 0);
    n_str = malloc(strlen(n_str_r)+1+strlen(n_str_i)+1);
    if (!n_str)
      mem_error();
    sprintf(n_str,"%si%s",n_str_r,n_str_i);
    free(n_str_r);
    free(n_str_i);
  } else {
    n_str = malloc(strlen(str)+3);
    if (!n_str)
      mem_error();
    sprintf(n_str,"\"%s\"",str);
  }
  char *res = malloc(2);
  if (!res)
    mem_error();
  strcpy(res,"[");
  long idx = 1;
  char *curarg = hc_get_arg(list,idx);
  while (curarg)
  {
    char *tmp = malloc(strlen(curarg)+1+strlen(n_str)+1);
    if (!tmp)
      mem_error();
    sprintf(tmp,"%s%c%s",curarg,mode,n_str);
    char *tmp_r = hc_result_(tmp);
    if (!tmp_r)
    {
      free(tmp);
      free(res);
      free(curarg);
      free(n_str);
      return NULL;
    }
    free(tmp);
    res = realloc(res, strlen(res)+strlen(tmp_r)+2);
    if (!res)
      mem_error();
    strcat(res,tmp_r);
    strcat(res,",");
    free(tmp_r);
    free(curarg);
    curarg = hc_get_arg(list,++idx);
  }

  free(n_str);
  res[strlen(res)-1] = ']';
  return res;
}


char *list_add_sub(char *l1, char *l2, char mode)
{
  l1++; l2++;
  l1[strlen(l1)-1]='\0'; l2[strlen(l2)-1]='\0';
  char *res = malloc(2);
  if (!res)
    mem_error();
  strcpy(res,"[");
  long idx = 1;
  char *curarg1 = hc_get_arg(l1,idx);
  char *curarg2 = hc_get_arg(l2,idx);
  while (curarg1 && curarg2)
  {
    char *tmp = malloc(strlen(curarg1)+1+strlen(curarg2)+1);
    if (!tmp)
      mem_error();
    sprintf(tmp,"%s%c%s",curarg1,mode,curarg2);
    char *tmp_r = hc_result_(tmp);
    free(tmp);
    if (!tmp_r)
    {
      free(curarg1); free(curarg2);
      free(res);
      return NULL;
    }
    res = realloc(res,strlen(res)+strlen(tmp_r)+2);
    strcat(res,tmp_r);
    strcat(res,",");
    free(tmp_r);
    free(curarg1);
    free(curarg2);
    curarg1 = hc_get_arg(l1,++idx);
    curarg2 = hc_get_arg(l2,idx);
  }

  res[strlen(res)-1] = ']';

  if (curarg1 || curarg2)
  {
    free(curarg1); free(curarg2); free(res);
    dim_error();
    return NULL;
  }

  return res;
}


char *list_simplify(char *list)
{
  unsigned int alloc = 2;
  char *new = malloc(alloc);
  if (!new)
    mem_error();
  strcpy(new,"[");
  char *old = strdup((char *)(list+1));
  if (!old)
    mem_error();
  old[strlen(old)-1] = '\0';
  long idx = 1;
  char *curarg = hc_get_arg(old,idx);
  while (curarg)
  {
    char *curres = hc_result_(curarg);
    if (!curres)
    {
      free(old);
      free(curarg);
      return NULL;
    }
    alloc += strlen(curres) + 1;
    new = realloc(new,alloc);
    if (!new)
      mem_error();
    strcat(new,curres);
    strcat(new,",");
    free(curarg);
    free(curres);
    curarg = hc_get_arg(old,++idx);
  }
  new[strlen(new)-1] = ']';
  free(old);
  return new;
}


char *list_clean(char *list)
{
  char *r = strip_spaces(list) + 1;
  while (r[strlen(r)-1]!=']')
    r[strlen(r)-1] = '\0';
  r[strlen(r)-1] = '\0';
  return r;
}
