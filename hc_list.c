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
char hc_list_get(char *data, char *type, char *scan, int *i)
{
  // scan[*i] points at the '[' of the beginning of the index
  *i += 1;
  char tmp_idx[MAX_DOUBLE_STRING];
  int j=0;
  while (scan[*i]!=']')
  {
    if (!isdigit(scan[*i]))
    {
      arg_error("only integer indexes are accepted.");
      return 0;
    }
    tmp_idx[j++] = scan[*i];
    *i += 1;
  }
  *i += 1;
  tmp_idx[j] = 0;
  data[strlen(data)-1] = '\0'; // delete the last ']'
  char *res = hc_get_arg((char *)(data + 1), atoi(tmp_idx) + 1);
  if (!res)
  {
    arg_error("index out of range.");
    return 0;
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


char *list_multiply(char *list, M_APM n)
{
  list++; // skip the initial '['; this assumes valid data is sent in
  list[strlen(list)-1] = '\0';
  char *n_str = m_apm_to_fixpt_stringexp(0, n, '.', 0, 0); // only integers
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
    sprintf(tmp,"%s*%s",curarg,n_str);
    char *tmp_r = hc_result_(tmp);
    if (!tmp_r)
      return NULL; // TODO. multiplying by an integer should always work for all types, though
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
