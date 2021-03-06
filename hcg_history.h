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


#ifndef HCG_HISTORY_H
#define HCG_HISTORY_H

#include "hc.h"

#define HISTORY_MAX 100
#define HISTORY_ITEM_MAX MAX_EXPR

extern char history[HISTORY_MAX][HISTORY_ITEM_MAX];
extern int history_cur;
extern int history_last;

extern void hcg_history_insert(char *str);
extern void hcg_history_insert_at_current(char *str);
extern char *hcg_history_back();
extern char *hcg_history_forward();

#endif
