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


#ifndef HCGCORE_HPP
#define HCGCORE_HPP

#include <QObject>
#include <QString>
#include <QMutex>


class HCGCore : public QObject {
  Q_OBJECT


  public:
  HCGCore();
  ~HCGCore();
  char *result(QString expr);
  void load(QString filename);
  void emitSignals();
  void closeAll();

  public slots:
  void setPrecision(int precision);
  void setAngleMode(QString mode);
  void setExpMode(QString mode);
  void setRPN(bool enabled);
  void cleanup();


  private:
  QMutex hc_mutex;


  signals:
  void precisionChanged(int precision);
  void angleModeChanged(QString mode);
  void expModeChanged(QString mode);
  void RPNChanged(bool rpn_enabled);
  void closeAll_signal();
};

#endif
