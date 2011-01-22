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

#include <QObject>
#include <QThread>
#ifdef WIN32
#include <hul.h>
#endif
#include "../hc_info.h"
#include "hcgthreads.hpp"
#include "main.hpp"


HCGScriptThread::HCGScriptThread(QString fname)
{
  filename = fname;
}


void HCGScriptThread::run()
{
  hcgcore->load(filename);
}


HCGResultThread::HCGResultThread(QString e)
{
  expr = e;
}


void HCGResultThread::run()
{
  emit computation_finished(expr, hcgcore->result(expr));
}


#ifdef WIN32
void HCGUpdateThread::run()
{
  emit checking_finished(hul_checkupdates((char *)VERSION,(char *)STATUS_URL_GUI));
}
#endif
