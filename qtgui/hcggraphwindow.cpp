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


#include <math.h>
#include "main.hpp"
#include "hcggraphwindow.hpp"
#include "hcgthreads.hpp"
#include "../hc_info.h"


HCGGraphWindow::HCGGraphWindow() : QMainWindow() {
  hbox = new QWidget(this);
  vbox = new QWidget(this);
  gdisp = new HCGGraphDisplay(this);
  lineedit = new QLineEdit(this);
  connect(lineedit, SIGNAL(returnPressed()), this, SLOT(drawGraph()));

  vbox_layout = new QVBoxLayout(vbox);
  vbox_layout->setSpacing(0);
  vbox_layout->setContentsMargins(1,1,1,1);
  vbox_layout->setAlignment(Qt::AlignTop);

  vbox_layout->addWidget(new QLabel(" Graph type"));
  gtypes = new QComboBox(this);
  QStringList gtypeslist;
  gtypeslist << "2D" << "Parametric" << "Values (points)" << "Values (xyline)" << "3D" << "Slope Field" << "Boxplot";
  gtypes->insertItems(0, gtypeslist);
  vbox_layout->addWidget(gtypes);

  vbox_layout->addWidget(new QLabel(" Expression"));
  vbox_layout->addWidget(lineedit);

  QGroupBox *optionsBox = new QGroupBox("Options", this);
  QVBoxLayout *optionsVBox = new QVBoxLayout(this);
  options = new QStackedWidget(this);
  optionsVBox->addWidget(options);
  optionsBox->setLayout(optionsVBox);

  // 2D options
  QWidget *o2D = new QWidget(this);
  QVBoxLayout *o2Dbox = new QVBoxLayout(this);
  o2D->setLayout(o2Dbox);
  xmin2D = new QLineEdit(this);
  xmax2D = new QLineEdit(this);
  ymin2D = new QLineEdit(this);
  ymax2D = new QLineEdit(this);
  o2Dbox->addWidget(new QLabel("xmin"));
  o2Dbox->addWidget(xmin2D);
  o2Dbox->addWidget(new QLabel("xmax"));
  o2Dbox->addWidget(xmax2D);
  o2Dbox->addWidget(new QLabel("ymin"));
  o2Dbox->addWidget(ymin2D);
  o2Dbox->addWidget(new QLabel("ymax"));
  o2Dbox->addWidget(ymax2D);
  options->addWidget(o2D);

  // Parametric options
  QWidget *oP = new QWidget(this);
  QVBoxLayout *oPbox = new QVBoxLayout(this);
  oP->setLayout(oPbox);
  tminP = new QLineEdit(this);
  tmaxP = new QLineEdit(this);
  xminP = new QLineEdit(this);
  xmaxP = new QLineEdit(this);
  yminP = new QLineEdit(this);
  ymaxP = new QLineEdit(this);
  oPbox->addWidget(new QLabel("tmin"));
  oPbox->addWidget(tminP);
  oPbox->addWidget(new QLabel("tmax"));
  oPbox->addWidget(tmaxP);
  oPbox->addWidget(new QLabel("xmin"));
  oPbox->addWidget(xminP);
  oPbox->addWidget(new QLabel("xmax"));
  oPbox->addWidget(xmaxP);
  oPbox->addWidget(new QLabel("ymin"));
  oPbox->addWidget(yminP);
  oPbox->addWidget(new QLabel("ymax"));
  oPbox->addWidget(ymaxP);
  options->addWidget(oP);

  // Values (points) options
  options->addWidget(new QLabel("No options available for this graph type"));

  // Values (xyline) options
  options->addWidget(new QLabel("No options available for this graph type"));

  // 3D options
  QWidget *o3D = new QWidget(this);
  QVBoxLayout *o3Dbox = new QVBoxLayout(this);
  o3D->setLayout(o3Dbox);
  xmin3D = new QLineEdit(this);
  xmax3D = new QLineEdit(this);
  ymin3D = new QLineEdit(this);
  ymax3D = new QLineEdit(this);
  zmin3D = new QLineEdit(this);
  zmax3D = new QLineEdit(this);
  o3Dbox->addWidget(new QLabel("xmin"));
  o3Dbox->addWidget(xmin3D);
  o3Dbox->addWidget(new QLabel("xmax"));
  o3Dbox->addWidget(xmax3D);
  o3Dbox->addWidget(new QLabel("ymin"));
  o3Dbox->addWidget(ymin3D);
  o3Dbox->addWidget(new QLabel("ymax"));
  o3Dbox->addWidget(ymax3D);
  o3Dbox->addWidget(new QLabel("zmin"));
  o3Dbox->addWidget(zmin3D);
  o3Dbox->addWidget(new QLabel("zmax"));
  o3Dbox->addWidget(zmax3D);
  options->addWidget(o3D);

  // Slope Field options
  QWidget *oS = new QWidget(this);
  QVBoxLayout *oSbox = new QVBoxLayout(this);
  oS->setLayout(oSbox);
  xminS = new QLineEdit(this);
  xmaxS = new QLineEdit(this);
  yminS = new QLineEdit(this);
  ymaxS = new QLineEdit(this);
  oSbox->addWidget(new QLabel("xmin"));
  oSbox->addWidget(xminS);
  oSbox->addWidget(new QLabel("xmax"));
  oSbox->addWidget(xmaxS);
  oSbox->addWidget(new QLabel("ymin"));
  oSbox->addWidget(yminS);
  oSbox->addWidget(new QLabel("ymax"));
  oSbox->addWidget(ymaxS);
  options->addWidget(oS);

  // Boxplot options
  options->addWidget(new QLabel("No options available for this graph type"));

  vbox_layout->addWidget(optionsBox);
  connect(gtypes, SIGNAL(activated(int)), this, SLOT(setCurrentIndex(int)));

  fullform = new QLineEdit(this);
  fullform->setReadOnly(true);
  vbox_layout->addWidget(fullform);
  updateFullForm();

  go = new QPushButton(" Go ", this);
  connect(go, SIGNAL(clicked()), this, SLOT(drawGraph()));
  vbox_layout->addWidget(go);

  vbox->setLayout(vbox_layout);

  hbox_layout = new QHBoxLayout(hbox);
  hbox_layout->setSpacing(0);
  hbox_layout->setContentsMargins(1,1,1,1);
  hbox_layout->addWidget(gdisp);
  hbox_layout->addWidget(vbox);
  hbox->setLayout(hbox_layout);

  setCentralWidget(hbox);
  setCurrentIndex(0);
  show();
}


void HCGGraphWindow::setCurrentIndex(int i)
{
  updateOptions(i + 1);
  updateFullForm();
}


void HCGGraphWindow::updateGraph(QPixmap map, unsigned int x, unsigned int y, int type, char *args)
{
  gdisp->setPixmap(map);
  gdisp->setFixedSize(x,y);
  lineedit->setText(QString(args));
  updateOptions(type);
  updateFullForm();
  show();
  activateWindow();
  raise();
}


void HCGGraphWindow::updateFullForm()
{
  fullform->setText("Full form : " + getFullForm());
}


QString HCGGraphWindow::getFullForm()
{
  QString cmd;
  switch (gtypes->currentIndex() + 1)
  {
  case HCGT_2D:
    cmd += "graph(";
    break;

  case HCGT_PARAMETRIC:
    cmd += "graphpeq(";
    break;

  case HCGT_VALUESPOINTS:
    cmd += "graphpoints(";
    break;

  case HCGT_VALUESLINE:
    cmd += "graphvalues(";
    break;

  case HCGT_3D:
    cmd += "graph3(";
    break;

  case HCGT_SLPFLD:
    cmd += "slpfld(";
    break;

  case HCGT_BOXPLOT:
    cmd += "boxplot(";
    break;

  default:
    break;
  }
  cmd += lineedit->text();
  switch (gtypes->currentIndex() + 1)
  {
  case HCGT_2D:
    cmd += "," + xmin2D->text() + "," + xmax2D->text() + "," + ymin2D->text() + "," + ymax2D->text();
    break;

  case HCGT_PARAMETRIC:
    cmd += "," + tminP->text() + "," + tmaxP->text() + "," + xminP->text() + "," + xmaxP->text() + "," + yminP->text() + "," + ymaxP->text();
    break;

  case HCGT_3D:
    cmd += "," + xmin3D->text() + "," + xmax3D->text() + "," + ymin3D->text() + "," + ymax3D->text() + "," + zmin3D->text() + "," + zmax3D->text();
    break;

  case HCGT_SLPFLD:
    cmd += "," + xminS->text() + "," + xmaxS->text() + "," + yminS->text() + "," + ymaxS->text();
    break;

  default:
    break;  
  }
  cmd += ")";
  return cmd;
}


void HCGGraphWindow::drawGraph()
{
  updateFullForm();
  HCGResultThread *resThread = new HCGResultThread(getFullForm());
  resThread->start();
}


void HCGGraphWindow::updateOptions(int type)
{
  options->setCurrentIndex(type - 1);
  gtypes->setCurrentIndex(type - 1);
  switch (type)
  {
  case HCGT_2D:
    xmin2D->setText(QString().setNum(hc.xmin2d[hcgt_get_idx(HCGT_2D)], 'g', hc.precision));
    xmax2D->setText(QString().setNum(hc.xmax2d[hcgt_get_idx(HCGT_2D)], 'g', hc.precision));
    ymin2D->setText(QString().setNum(hc.ymin2d[hcgt_get_idx(HCGT_2D)], 'g', hc.precision));
    ymax2D->setText(QString().setNum(hc.ymax2d[hcgt_get_idx(HCGT_2D)], 'g', hc.precision));
    break;

  case HCGT_PARAMETRIC:
    tminP->setText(QString().setNum(hc.tminpeq, 'g', hc.precision));
    tmaxP->setText(QString().setNum(hc.tmaxpeq, 'g', hc.precision));
    xminP->setText(QString().setNum(hc.xmin2d[hcgt_get_idx(HCGT_PARAMETRIC)], 'g', hc.precision));
    xmaxP->setText(QString().setNum(hc.xmax2d[hcgt_get_idx(HCGT_PARAMETRIC)], 'g', hc.precision));
    yminP->setText(QString().setNum(hc.ymin2d[hcgt_get_idx(HCGT_PARAMETRIC)], 'g', hc.precision));
    ymaxP->setText(QString().setNum(hc.ymax2d[hcgt_get_idx(HCGT_PARAMETRIC)], 'g', hc.precision));  
    break;

  case HCGT_3D:
    xmin3D->setText(QString().setNum(hc.xmin3d, 'g', hc.precision));
    xmax3D->setText(QString().setNum(hc.xmax3d, 'g', hc.precision));
    ymin3D->setText(QString().setNum(hc.ymin3d, 'g', hc.precision));
    ymax3D->setText(QString().setNum(hc.ymax3d, 'g', hc.precision));
    zmin3D->setText(QString().setNum(hc.zmin3d, 'g', hc.precision));
    zmax3D->setText(QString().setNum(hc.zmax3d, 'g', hc.precision));
    break;

  case HCGT_SLPFLD:
    xminS->setText(QString().setNum(hc.xmin2d[hcgt_get_idx(HCGT_SLPFLD)], 'g', hc.precision));
    xmaxS->setText(QString().setNum(hc.xmax2d[hcgt_get_idx(HCGT_SLPFLD)], 'g', hc.precision));
    yminS->setText(QString().setNum(hc.ymin2d[hcgt_get_idx(HCGT_SLPFLD)], 'g', hc.precision));
    ymaxS->setText(QString().setNum(hc.ymax2d[hcgt_get_idx(HCGT_SLPFLD)], 'g', hc.precision));
    break;

  default:
    break;
  }
}


// x and y are relative to hc_graph.c's MEM_DRIVER_X and MEM_DRIVER_Y
void HCGGraphWindow::zoom(double x, double y, double zoomfactor, double movefactor)
{
  HCGZoomThread *zThread = new HCGZoomThread(x, y, zoomfactor, movefactor, gtypes->currentIndex() + 1, lineedit->text());
  zThread->start();
}


void HCGGraphDisplay::mousePressEvent(QMouseEvent *event)
{
  switch (event->button())
  {
  case Qt::LeftButton:
    setCursor(QCursor(Qt::SizeAllCursor));
    HCG_GET_XY(movex, movey);
    break;

  default:
    event->ignore();
  }
}


void HCGGraphDisplay::mouseReleaseEvent(QMouseEvent *event)
{
  switch (event->button())
  {
  case Qt::LeftButton:
    setCursor(QCursor(Qt::ArrowCursor));
    break;

  default:
    event->ignore();
  }
}


void HCGGraphDisplay::mouseMoveEvent(QMouseEvent *event)
{
  double x,y;
  HCG_GET_XY(x,y);
  // 247 is the center x coordinate, 184 is the center y coordinate
  if (fabs(movex - x) >= 5 || fabs(movey - y) >= 5)
  {
    // TODO : if the user moves the mouse fast, the move will not be smooth as a result of the graph engine not keeping up; a workaround is needed
    parentWindow->zoom(247 + movex - x, 184 + movey - y, 1, 1);
    movex = x; movey = y;
  }
}


void HCGGraphDisplay::wheelEvent(QWheelEvent *event)
{
  double x,y;
  HCG_GET_XY(x,y);
  parentWindow->zoom(x, y, pow(HCG_ZOOM_FACTOR, event->delta() / 8 / 15), HCG_MOVE_FACTOR);
}
