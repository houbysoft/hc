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

#ifndef HCGTHREADS_HPP
#define HCGTHREADS_HPP

class HCGScriptThread : public QThread {
  Q_OBJECT

  protected:
  void run();

  private:
  QString filename;

  public:
  HCGScriptThread(QString fname);
  ~HCGScriptThread() {};
};

class HCGResultThread : public QThread {
  Q_OBJECT

  protected:
  void run();

  private:
  QString expr;

  signals:
  void computation_finished(QString expr, char *result);

  public:
  HCGResultThread(QString e);
  ~HCGResultThread() {};
};

#endif
