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


#ifndef HC_UTILS_H
#define HC_UTILS_H

#include <m_apm.h>

typedef enum {
  SYNTAX, NOT_ENOUGH_OPERANDS, TYPE, LOAD, ARG, WARNING, ERROR
} HC_ERRORS;

extern void hc_error(char type, char *xformat, ...);
extern void hc_set_from_string(M_APM re, M_APM im, char *num);
extern char *hc_enlarge_buffer(char *buffer, unsigned int *sz);

#endif
