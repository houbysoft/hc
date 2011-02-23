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


#ifndef MAIN_HPP
#define MAIN_HPP

#include "hcgwindow.hpp"
#include "hcgcore.hpp"

extern HCGCore *hcgcore;
extern HCGWindow *hcg;
extern "C" void notify(char *str);
extern "C" void notify_console(char *str);
extern "C" void notify_error(char *str);
#ifdef WIN32
extern "C" void updateStatusCallback(int status);
#endif
extern "C" char *prompt(char *str);
extern "C" char *hc_result(char *);
extern "C" void hc_load(char *fname_);
extern "C" void hc_load_cfg();
extern "C" void hc_save_cfg();

#endif
