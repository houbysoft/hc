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


#ifndef HC_LIST_H
#define HC_LIST_H

extern char hc_get_by_index(char *data, char *type, char *scan, int *i);
extern char *list_mul_div(char *list, M_APM n_r, M_APM n_i, char *str, char mode);
extern char *list_add_sub(char *l1, char *l2, char mode);
extern char *list_neg(char *list);
extern char *list_clean(char *list);
extern char *list_simplify(char *list);
extern int list_compare(char *l1, char *l2);

#endif
