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
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include "hcgwindow.hpp"
#include "hcgcore.hpp"
#include "main.hpp"
#include "../hc_info.h"


HCGCore::HCGCore() : QObject() {
  hc_load_cfg();
}


HCGCore::~HCGCore() {
  cleanup();
}


char *HCGCore::result(QString expr)
{
  QMutexLocker locker(&hc_mutex);
  return hc_result((char *)expr.toAscii().data());
}


void HCGCore::load(QString filename)
{
  QMutexLocker locker(&hc_mutex);
  hc_load((char *)filename.toAscii().data());
}


void HCGCore::setPrecision(int precision)
{
  QMutexLocker locker(&hc_mutex);
  hc.precision = precision;
  emit precisionChanged(hc.precision);
}


void HCGCore::setAngleMode(QString mode)
{
  QMutexLocker locker(&hc_mutex);
  hc.angle = mode.toAscii().data()[0];
  emit angleModeChanged(QString(hc.angle));
}


void HCGCore::setExpMode(QString mode)
{
  QMutexLocker locker(&hc_mutex);
  hc.exp = mode.toAscii().data()[0];
  emit expModeChanged(QString(hc.exp));
}


void HCGCore::setRPN(bool enabled)
{
  QMutexLocker locker(&hc_mutex);
  if (enabled)
    hc.rpn = 1;
  else
    hc.rpn = 0;
  emit RPNChanged(bool(hc.rpn));
}


void HCGCore::emitSignals()
{
  emit RPNChanged(bool(hc.rpn));
  emit expModeChanged(QString(hc.exp));
  emit angleModeChanged(QString(hc.angle));
  emit precisionChanged(hc.precision);
}


void HCGCore::cleanup()
{
  hc_save_cfg();
}
