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

#include <string.h>
#include "hcg_history.h"

char history[HISTORY_MAX][HISTORY_ITEM_MAX];
int history_cur;
int history_last;

void hcg_history_insert(char *str)
{
  if (strlen(str) >= HISTORY_ITEM_MAX)
    return;
  else {
    if (history_last >= 0 && history[history_last][0] == '\0')
      history_last--;
    if (history_last + 1 >= HISTORY_MAX)
    {
      memmove((void *)&history, (const void *)&history + HISTORY_ITEM_MAX, (HISTORY_MAX - 1) * HISTORY_ITEM_MAX);
      history_last--;
    }
    strcpy(history[++history_last],str);
    history_cur = history_last;
  }
}

void hcg_history_insert_at_current(char *str)
{
  if (history_cur == history_last)
    hcg_history_insert(str);
  else if (strlen(str) >= HISTORY_ITEM_MAX)
    return;
  else {
    strcpy(history[history_cur],str);
  }
}

char *hcg_history_back()
{
  if (history_cur > 0)
    return history[--history_cur];
  else if (history_cur == 0)
    return history[0];
  else
    return NULL;
}

char *hcg_history_forward()
{
  if (history_cur >= 0 && history_last >= 0)
  {
    if (history_cur < history_last)
      return history[++history_cur];
    else
      return history[history_last];
  } else {
    return NULL;
  }
}
