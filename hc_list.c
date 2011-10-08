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
#include <ctype.h>
#include "hc.h"
#include "hc_functions.h"
#include "hc_list.h"
#include "hc_utils.h"


// returns 0 (FAIL) on error
char hc_get_by_index(char *data, char *type, char *scan, unsigned int *i)
{
  int pct = 1;
  unsigned int oldi = *i;
  char ignore = FALSE;
  while (pct!=0 && scan[*i])
  {
    *i += 1;
    if (scan[*i]=='\"')
    {
      ignore = ignore == FALSE ? TRUE : FALSE;
      continue;
    }
    if (ignore)
      continue;
    if (scan[*i]=='[')
      pct++;
    else if (scan[*i]==']')
      pct--;
  }
  *i += 1;
  char *resme = malloc(*i - oldi + 1);
  if (!resme) mem_error();
  strncpy(resme,(char *)(scan + oldi),*i - oldi);
  resme[*i - oldi] = 0;
  char *scanres = hc_result(resme);
  free(resme);
  if (!scanres)
    return FAIL;
  //*i += strlen((char *)(scan + *i));
  // FIXME : hc_result() is expensive, should use hc_result_() instead, but this needs the result to be nicely parsed (ie. the radix and zeroes after an integer stripped)
  // scan[*i] points at the '[' of the beginning of the index
  unsigned int tmp_idx_alloc = MAX_EXPR;
  char *tmp_idx = malloc(tmp_idx_alloc);
  unsigned int j=0,k=1;
  for (; scanres[k]!=']'; k++)
  {
    if (!isdigit(scanres[k]))
    {
      free(scanres); free(tmp_idx);
      arg_error("only integer indexes are accepted.");
    }
    if (j >= tmp_idx_alloc)
    {
      tmp_idx = hc_enlarge_buffer(tmp_idx, &tmp_idx_alloc);
    }
    tmp_idx[j++] = scanres[k];
  }
  k++;
  if (j >= tmp_idx_alloc)
  {
    tmp_idx = hc_enlarge_buffer(tmp_idx, &tmp_idx_alloc);
  }
  tmp_idx[j] = 0;
  free(scanres);
  char *res = NULL;
  if (is_string(data))
  {
    data = strip_spaces(data);
    data[strlen(data)-1] = '\0'; // delete the last '"'
    if ((unsigned int)atoi(tmp_idx) >= strlen((char *)(data + 1)))
    {
      free(tmp_idx);
      arg_error("index out of range.");
    }
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
      free(tmp_idx);
      arg_error("index out of range.");
    }
  } else {
    free(tmp_idx);
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
  free(tmp_idx);

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


// returns TRUE if the list supplied in a is a valid matrix (ie, it is rectangular), and places the number of rows and columns in rows and cols respectively.
// Assumes that the list sent in is valid and cleaned.
char list_check_is_matrix(char *a, unsigned int *rows, unsigned int *cols)
{
  a++; a[strlen(a)-1] = '\0';

  // establish the dimensions from the first row
  char *firstrow = hc_get_arg_r(a, 1);
  if (!firstrow || !strlen(firstrow)) {
    free(firstrow);
    arg_error("supplied matrix is empty");
  }
  if (!is_list(firstrow)) {
    free(firstrow);
    arg_error("error : supplied list is not a 2D matrix");
    return FALSE;
  } else {
    if (!hc_length_core(NULL, cols, HC_UINT, firstrow)) {
      free(firstrow);
      return FALSE;
    }
  }
  free(firstrow);

  // check all other rows to have the same dimensions
  char *row = NULL;
  unsigned int r = 1;
  unsigned int tmpcols = 0;
  while ((row = hc_get_arg_r(a, ++r)) != NULL) {
    if (is_list(row)) {
      if (!hc_length_core(NULL, &tmpcols, HC_UINT, row)) {
        free(row);
        return FALSE;
      }
    } else {
      tmpcols = 1;
    }
    if (*cols != tmpcols) {
      free(row);
      arg_error("error : supplied list is not a matrix (it is not rectangular)");
    }
    free(row);
  }

  *rows = r - 1;
  return TRUE;
}


char *list_matrix_mul(char *a, char *b)
{
  unsigned int col_a, col_b, row_a, row_b;
  // check if the supplied lists are actually matrices (ie, if they are rectangular) and that the number of columns of the first matrix equals the number of rows of the second matrix
  if (!list_check_is_matrix(a, &row_a, &col_a) || !list_check_is_matrix(b, &row_b, &col_b)) {
    return NULL;
  }
  if (col_a != row_b) {
    arg_error("* : matrix multiplication : the number of columns of the first matrix does not match the number of rows of the second matrix.");
  }

  unsigned int need_space_col_a = hc_need_space_int(col_a);
  // multiply the matrices
  unsigned int rmalloc = 2;
  char *r = malloc(rmalloc); if (!r) mem_error();
  r[0] = '['; r[1] = '\0';

  unsigned int i, j;

  // This uses the naive algorithm, as this is unlikely to be used for huge matrices. Nevertheless,
  // it might be worth implementing a smarter algorithm, such as Strassen's.
  for (i=0; i < row_a; i++) {
    rmalloc += 1;
    r = realloc(r, rmalloc); if (!r) mem_error();
    strcat(r, "[");

    unsigned int need_space_i = hc_need_space_int(i);

    for (j=0; j < col_b; j++) {
      // ith row of a times jth column of b

      char *res_expr = malloc(4 + strlen(a) + 1 + 4 + need_space_i + 1 + 1 + strlen(b) + 1 + 4 + hc_need_space_int(j) + 1 + 1 + 2 + need_space_col_a + 3 + 1); if (!res_expr) mem_error();
      sprintf(res_expr, "sum(%s][%i][x]*%s][x][%i],0,%i-1)", a, i, b, j, col_a); // the last ] is stripped from a and b by list_check_is_matrix()
      char *res_tmp = hc_result_(res_expr);
      free(res_expr);
      if (!res_tmp) {
        free(r);
        return NULL;
      }
      rmalloc += 1+strlen(res_tmp);
      r = realloc(r, rmalloc); if (!r) mem_error();
      strcat(r, res_tmp);
      free(res_tmp);
      strcat(r, ",");
    }

    rmalloc += 1;
    r = realloc(r, rmalloc); if (!r) mem_error();
    r[strlen(r)-1] = ']';
    strcat(r, ",");
  }
  r[strlen(r)-1] = ']';

  return r;
}


// returns 0 if the lists are equal
int list_compare(char *l1, char *l2)
{
  l1++; l2++;
  l1[strlen(l1)-1]='\0'; l2[strlen(l2)-1]='\0';
  if (!strlen(l1) && !strlen(l2)) // the lists are both empty
    return 0;
  long idx = 1;
  char *curarg1 = hc_get_arg(l1,idx);
  char *curarg2 = hc_get_arg(l2,idx);
  while (curarg1 && curarg2)
  {
    char *tmp = malloc(strlen(curarg1)+2+strlen(curarg2)+1);
    if (!tmp)
      mem_error();
    sprintf(tmp,"%s==%s",curarg1,curarg2);
    char *tmp_r = hc_result_(tmp);
    free(tmp);
    if (!tmp_r || !is_positive_int(tmp_r) || strstr(tmp_r,"1.") != tmp_r)
    {
      free(curarg1); free(curarg2); free(tmp_r);
      return 1;
    }
    free(tmp_r);
    free(curarg1);
    free(curarg2);
    curarg1 = hc_get_arg(l1,++idx);
    curarg2 = hc_get_arg(l2,idx);
  }

  if (curarg1 || curarg2)
  {
    free(curarg1); free(curarg2);
    return 1;
  }

  return 0;
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
  if (strlen(old))
    old[strlen(old)-1] = '\0';
  long idx = 1;
  char *curarg = hc_get_arg(old,idx);
  while (curarg)
  {
    char *curres = hc_result_(curarg);
    if (!curres)
    {
      free(new);
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
