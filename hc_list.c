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
      return 0; // only integer indexes are accepted
    tmp_idx[j++] = scan[*i];
    *i += 1;
  }
  *i += 1;
  tmp_idx[j] = 0;
  data[strlen(data)-1] = '\0'; // delete the last ']'
  char *res = hc_get_arg((char *)(data + 1), atoi(tmp_idx) + 1);
  if (!res)
    return 0;

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
