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
#include <math.h>
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


void HCGZoomThread::run()
{
  double distx,disty;
  double oldx,oldy;
  double newx,newy;
  double xmin2d,xmax2d,ymin2d,ymax2d;
  int i;
  QString cmd;
  switch (type)
  {
  case HCGT_2D:
  case HCGT_PARAMETRIC:
  case HCGT_SLPFLD:
  case HCGT_POLAR:
    i = hcgt_get_idx(type);
    hcgcore->lock();
    distx = hc.xmax2d[i] - hc.xmin2d[i];
    disty = hc.ymax2d[i] - hc.ymin2d[i];
    oldx = (hc.xmin2d[i] + hc.xmax2d[i]) / 2;
    oldy = (hc.ymin2d[i] + hc.ymax2d[i]) / 2;
    newx = oldx + movefactor * ((x * (distx / 494) + hc.xmin2d[i]) - oldx);
    newy = oldy + movefactor * ((y * (disty / 368) + hc.ymin2d[i]) - oldy);
    hcgcore->unlock();
    xmin2d = newx - fabs(distx) / 2;
    xmax2d = newx + fabs(distx) / 2;
    ymin2d = newy - fabs(disty) / 2;
    ymax2d = newy + fabs(disty) / 2;
    xmin2d += ((xmax2d - xmin2d) - ((xmax2d - xmin2d) / zoomfactor)) / 2.0;
    xmax2d -= ((xmax2d - xmin2d) - ((xmax2d - xmin2d) / zoomfactor)) / 2.0;
    ymin2d += ((ymax2d - ymin2d) - ((ymax2d - ymin2d) / zoomfactor)) / 2.0;
    ymax2d -= ((ymax2d - ymin2d) - ((ymax2d - ymin2d) / zoomfactor)) / 2.0;
    break;

  case HCGT_3D:
    // TODO
    return;

  default:
    return;
  }

  switch (type)
  {
  case HCGT_2D:
    cmd = "graph(";
    break;

  case HCGT_PARAMETRIC:
    cmd = "graphpeq(";
    break;

  case HCGT_SLPFLD:
    cmd = "slpfld(";
    break;

  case HCGT_POLAR:
    cmd = "graphpolar(";
    break;

  case HCGT_3D:
    // TODO
    return;
  }
  cmd += expr;
  switch (type)
  {
  case HCGT_2D:
    cmd += "," + QString().setNum(xmin2d) + "," + QString().setNum(xmax2d) + "," + QString().setNum(ymin2d) + "," + QString().setNum(ymax2d);
    break;

  case HCGT_PARAMETRIC:
    hcgcore->lock();
    cmd += "," + QString().setNum(hc.tminpeq) + "," + QString().setNum(hc.tmaxpeq) + "," + QString().setNum(xmin2d) + "," + QString().setNum(xmax2d) + "," + QString().setNum(ymin2d) + "," + QString().setNum(ymax2d);
    hcgcore->unlock();
    break;

  case HCGT_SLPFLD:
    cmd += "," + QString().setNum(xmin2d) + "," + QString().setNum(xmax2d) + "," + QString().setNum(ymin2d) + "," + QString().setNum(ymax2d);
    break;

  case HCGT_POLAR:
    hcgcore->lock();
    cmd += "," + QString().setNum(hc.tminpolar) + "," + QString().setNum(hc.tmaxpolar) + "," + QString().setNum(xmin2d) + "," + QString().setNum(xmax2d) + "," + QString().setNum(ymin2d) + "," + QString().setNum(ymax2d);
    hcgcore->unlock();
    break;

  case HCGT_3D:
    // TODO
    return;
  }
  cmd += ")";
  free(hcgcore->result(cmd));
}


#ifdef WIN32
void HCGUpdateThread::run()
{
  emit checking_finished(hul_checkupdates((char *)VERSION,(char *)STATUS_URL_GUI));
}


HCGApplyUpdateThread::HCGApplyUpdateThread(HUL *update)
{
  u = update;
}


void HCGApplyUpdateThread::run()
{
  emit finished(hul_applyupdate(u, updateStatusCallback));
}
#endif
