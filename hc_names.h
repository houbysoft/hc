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


#ifndef HC_NAMES_H
#define HC_NAMES_H

#ifdef __cplusplus
extern "C" {
#endif

#define HC_NAMES 114 // number of fields in the hc_names array
#define HC_NAMES_CNST_START 86 // first field with a constant in the hc_names array
#define HC_NAMES_CNST_STOP 92 // last field with a constant in the hc_names array

extern const char *hc_names[][5];

#ifdef __cplusplus
}
#endif

#endif
