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
#include "hc_fnp.h"
#include "hc_functions.h"
#include "hc_list.h"


char *hc_map(char *e)
{
  char *fn = hc_get_arg(e,1);
  char *list_orig = hc_get_arg_r(e,2);
  if (!fn || !list_orig || !is_list(list_orig))
  {
    free(fn); free(list_orig);
    arg_error("map() : first argument must be a function, and the second a vector/list.");
  }

  unsigned long int alloc = 1;
  char *res = malloc(alloc);
  if (!res) mem_error();
  res[0] = '\0';
  char *list = list_clean(list_orig);
  unsigned long int idx = 0;
  char *cur = NULL;
  unsigned int fnstrlen = strlen(fn);

  while ((cur = hc_get_arg(list,++idx)) != NULL)
  {
    char *tmpexpr = malloc(fnstrlen + 1 + strlen(cur) + 2);
    if (!tmpexpr) mem_error();
    sprintf(tmpexpr,"%s(%s)",fn,cur);
    char *tmpres = hc_result_(tmpexpr);
    free(tmpexpr);
    if (!tmpres)
    {
      free(cur); free(list_orig); free(fn); free(res);
      return FAIL;
    }
    alloc += strlen(tmpres) + 1;
    res = realloc(res,alloc);
    strcat(res,","); strcat(res,tmpres);
    free(tmpres);
    free(cur);
  }
  res[0] = '[';
  res[strlen(res)-1] = ']';
  free(list_orig); free(fn);
  return res;
}
