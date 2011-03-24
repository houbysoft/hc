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

#include <QApplication>
#include <QCoreApplication>
#include <QObject>
#include "hcgwindow.hpp"
#include "hcgcore.hpp"
#include "hcggraphwindow.hpp"
#include "main.hpp"


extern "C" void hc_load_cfg();


HCGWindow *hcg;
HCGCore *hcgcore;
HCGGraphWindow *graph_window = NULL;


int main(int argc, char *argv[])
{
  hc_load_cfg();
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("HoubySoft");
  QCoreApplication::setOrganizationDomain("houbysoft.com");
  QCoreApplication::setApplicationName("HCG");
  hcgcore = new HCGCore;
  hcg = new HCGWindow;
  QObject::connect(&app, SIGNAL(aboutToQuit()), hcgcore, SLOT(cleanup()));
  return app.exec();
}
