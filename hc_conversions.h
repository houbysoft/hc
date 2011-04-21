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


#include <m_apm.h>


#ifndef HC_CONVERSIONS_H
#define HC_CONVERSIONS_H

#define MAX_CONVERSION_FIELDS 10
#define HC_CONVERSIONS_BASE_UNIT 1
#define HC_CONVERSIONS_NORMAL_UNIT 2

#define HC_CONVERSIONS 30
#define HC_CONVERSIONS_NAME_START_IDX 2

#define HC_CONVERSIONS_BASENAMES 4

extern const char *hc_conversions[][MAX_CONVERSION_FIELDS];
extern const char *hc_conversions_basenames[][MAX_CONVERSION_FIELDS];
extern int hc_convert(M_APM result, char *e);

#endif
