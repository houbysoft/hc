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

  vbox_layout = new QVBoxLayout(vbox);
  vbox_layout->setSpacing(0);
  vbox_layout->setContentsMargins(1,1,1,1);
  vbox_layout->setAlignment(Qt::AlignTop);

  vbox_layout->addWidget(new QLabel(" Graph type"));
  gtypes = new QComboBox(this);
  QStringList gtypeslist;
  gtypeslist << "2D" << "Parametric" << "Values (points)" << "Values (xyline)" << "3D" << "Slope Field" << "Boxplot" << "Polar";
  gtypes->insertItems(0, gtypeslist);
  vbox_layout->addWidget(gtypes);

  QGroupBox *lineeditBox = new QGroupBox("Expression", this);
  QVBoxLayout *lineeditVBox = new QVBoxLayout(this);
  lineedits = new QStackedWidget(this);
  lineeditVBox->addWidget(lineedits);
  lineeditBox->setLayout(lineeditVBox);

  // 2D line edit
  QWidget *l2D = new QWidget(this);
  QHBoxLayout *l2DBox = new QHBoxLayout(this);
  l2D->setLayout(l2DBox);
  l2DBox->addWidget(new QLabel("y1(x), y2(x), ... = "));
  line2D = new QLineEdit();
  connect(line2D, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  l2DBox->addWidget(line2D);
  lineedits->addWidget(l2D);

  // Parametric line edit
  QWidget *lpar = new QWidget(this);
  QHBoxLayout *lparBox = new QHBoxLayout(this);
  lpar->setLayout(lparBox);
  lparBox->addWidget(new QLabel("x(t) = "));
  lineparx = new QLineEdit();
  connect(lineparx, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  lparBox->addWidget(lineparx);
  lparBox->addWidget(new QLabel("y(t) = "));
  linepary = new QLineEdit();
  connect(linepary, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  lparBox->addWidget(linepary);
  lineedits->addWidget(lpar);

  // Values (points) line edit
  QWidget *lvp = new QWidget(this);
  QHBoxLayout *lvpBox = new QHBoxLayout(this);
  lvp->setLayout(lvpBox);
  lvpBox->addWidget(new QLabel("[v_1, v_2, ...] = "));
  linevp = new QLineEdit();
  connect(linevp, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  lvpBox->addWidget(linevp);
  lineedits->addWidget(lvp);

  // Values (xyline) line edit
  QWidget *lvl = new QWidget(this);
  QHBoxLayout *lvlBox = new QHBoxLayout(this);
  lvl->setLayout(lvlBox);
  lvlBox->addWidget(new QLabel("[v_1, v_2, ...] = "));
  linevl = new QLineEdit();
  connect(linevl, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  lvlBox->addWidget(linevl);
  lineedits->addWidget(lvl);

  // 3D line edit
  QWidget *l3D = new QWidget(this);
  QHBoxLayout *l3DBox = new QHBoxLayout(this);
  l3D->setLayout(l3DBox);
  l3DBox->addWidget(new QLabel("z(x,y) = "));
  line3D = new QLineEdit();
  connect(line3D, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  l3DBox->addWidget(line3D);
  lineedits->addWidget(l3D);

  // Slope Field line edit
  QWidget *lsf = new QWidget(this);
  QHBoxLayout *lsfBox = new QHBoxLayout(this);
  lsf->setLayout(lsfBox);
  lsfBox->addWidget(new QLabel("dy/dx = "));
  linesf = new QLineEdit();
  connect(linesf, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  lsfBox->addWidget(linesf);
  lineedits->addWidget(lsf);

  // Boxplot line edit
  QWidget *lbx = new QWidget(this);
  QHBoxLayout *lbxBox = new QHBoxLayout(this);
  lbx->setLayout(lbxBox);
  lbxBox->addWidget(new QLabel("[v_1, v_2, ...] = "));
  linebx = new QLineEdit();
  connect(linebx, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  lbxBox->addWidget(linebx);
  lineedits->addWidget(lbx);

  // Polar line edit
  QWidget *lpl = new QWidget(this);
  QHBoxLayout *lplBox = new QHBoxLayout(this);
  lpl->setLayout(lplBox);
  lplBox->addWidget(new QLabel("r(t) = "));
  linepl = new QLineEdit();
  connect(linepl, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  lplBox->addWidget(linepl);
  lineedits->addWidget(lpl);

  vbox_layout->addWidget(lineeditBox);


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
  reset2Dbtn = new QPushButton("Reset to defaults", this);
  connect(reset2Dbtn, SIGNAL(clicked()), this, SLOT(reset2D()));
  o2Dbox->addWidget(new QLabel("xmin"));
  o2Dbox->addWidget(xmin2D);
  o2Dbox->addWidget(new QLabel("xmax"));
  o2Dbox->addWidget(xmax2D);
  o2Dbox->addWidget(new QLabel("ymin"));
  o2Dbox->addWidget(ymin2D);
  o2Dbox->addWidget(new QLabel("ymax"));
  o2Dbox->addWidget(ymax2D);
  o2Dbox->addWidget(reset2Dbtn);
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
  resetPbtn = new QPushButton("Reset to defaults", this);
  connect(resetPbtn, SIGNAL(clicked()), this, SLOT(resetP()));
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
  oPbox->addWidget(resetPbtn);
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
  reset3Dbtn = new QPushButton("Reset to defaults", this);
  connect(reset3Dbtn, SIGNAL(clicked()), this, SLOT(reset3D()));
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
  o3Dbox->addWidget(reset3Dbtn);
  options->addWidget(o3D);

  // Slope Field options
  QWidget *oS = new QWidget(this);
  QVBoxLayout *oSbox = new QVBoxLayout(this);
  oS->setLayout(oSbox);
  xminS = new QLineEdit(this);
  xmaxS = new QLineEdit(this);
  yminS = new QLineEdit(this);
  ymaxS = new QLineEdit(this);
  resetSbtn = new QPushButton("Reset to defaults", this);
  connect(resetSbtn, SIGNAL(clicked()), this, SLOT(resetS()));
  oSbox->addWidget(new QLabel("xmin"));
  oSbox->addWidget(xminS);
  oSbox->addWidget(new QLabel("xmax"));
  oSbox->addWidget(xmaxS);
  oSbox->addWidget(new QLabel("ymin"));
  oSbox->addWidget(yminS);
  oSbox->addWidget(new QLabel("ymax"));
  oSbox->addWidget(ymaxS);
  oSbox->addWidget(resetSbtn);
  options->addWidget(oS);

  // Boxplot options
  options->addWidget(new QLabel("No options available for this graph type"));

  // Polar options
  QWidget *oPO = new QWidget(this);
  QVBoxLayout *oPObox = new QVBoxLayout(this);
  oPO->setLayout(oPObox);
  tminPO = new QLineEdit(this);
  tmaxPO = new QLineEdit(this);
  xminPO = new QLineEdit(this);
  xmaxPO = new QLineEdit(this);
  yminPO = new QLineEdit(this);
  ymaxPO = new QLineEdit(this);
  resetPObtn = new QPushButton("Reset to defaults", this);
  connect(resetPObtn, SIGNAL(clicked()), this, SLOT(resetPO()));
  oPObox->addWidget(new QLabel("tmin"));
  oPObox->addWidget(tminPO);
  oPObox->addWidget(new QLabel("tmax"));
  oPObox->addWidget(tmaxPO);
  oPObox->addWidget(new QLabel("xmin"));
  oPObox->addWidget(xminPO);
  oPObox->addWidget(new QLabel("xmax"));
  oPObox->addWidget(xmaxPO);
  oPObox->addWidget(new QLabel("ymin"));
  oPObox->addWidget(yminPO);
  oPObox->addWidget(new QLabel("ymax"));
  oPObox->addWidget(ymaxPO);
  oPObox->addWidget(resetPObtn);
  options->addWidget(oPO);

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


void HCGGraphWindow::reset2D()
{
  xmin2D->setText("-10");
  xmax2D->setText("10");
  ymin2D->setText("-10");
  ymax2D->setText("10");
  drawGraph();
}


void HCGGraphWindow::resetP()
{
  tminP->setText("0");
  tmaxP->setText("2pi");
  xminP->setText("-10");
  xmaxP->setText("10");
  yminP->setText("-10");
  ymaxP->setText("10");
  drawGraph();
}


void HCGGraphWindow::reset3D()
{
  xmin3D->setText("-10");
  xmax3D->setText("10");
  ymin3D->setText("-10");
  ymax3D->setText("10");
  zmin3D->setText("-10");
  zmax3D->setText("10");
  drawGraph();
}


void HCGGraphWindow::resetS()
{
  xminS->setText("-10");
  xmaxS->setText("10");
  yminS->setText("-10");
  ymaxS->setText("10");
  drawGraph();
}


void HCGGraphWindow::resetPO()
{
  tminPO->setText("0");
  tmaxPO->setText("2pi");
  xminPO->setText("-10");
  xmaxPO->setText("10");
  yminPO->setText("-10");
  ymaxPO->setText("10");
  drawGraph();
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
  switch (type)
  {
  case HCGT_2D:
    line2D->setText(QString(args));
    break;
  case HCGT_PARAMETRIC:
    lineparx->setText(QString(args).split(",").first());
    linepary->setText(QString(args).split(",").last());
    break;
  case HCGT_3D:
    line3D->setText(QString(args));
    break;
  case HCGT_SLPFLD:
    linesf->setText(QString(args));
    break;
  case HCGT_VALUESPOINTS:
    linevp->setText(QString(args));
    break;
  case HCGT_VALUESLINE:
    linevl->setText(QString(args));
    break;
  case HCGT_BOXPLOT:
    linebx->setText(QString(args));
    break;
  case HCGT_POLAR:
    linepl->setText(QString(args));
  default:
    break;
  }
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


QString HCGGraphWindow::lineText()
{
  switch (gtypes->currentIndex() + 1)
  {
  case HCGT_2D:
    return "[" + line2D->text() + "]";
  case HCGT_PARAMETRIC:
    return lineparx->text() + "," + linepary->text();
  case HCGT_VALUESPOINTS:
    return linevp->text();
  case HCGT_VALUESLINE:
    return linevl->text();
  case HCGT_3D:
    return line3D->text();
  case HCGT_SLPFLD:
    return linesf->text();
  case HCGT_BOXPLOT:
    return linebx->text();
  case HCGT_POLAR:
    return linepl->text();
  default:
    return "";
  }
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

  case HCGT_POLAR:
    cmd += "graphpolar(";

  default:
    break;
  }
  cmd += this->lineText();
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

  case HCGT_POLAR:
    cmd += "," + tminPO->text() + "," + tmaxPO->text() + "," + xminPO->text() + "," + xmaxPO->text() + "," + yminPO->text() + "," + ymaxPO->text();
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
  lineedits->setCurrentIndex(type - 1);
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

  case HCGT_POLAR:
    tminPO->setText(QString().setNum(hc.tminpolar, 'g', hc.precision));
    tmaxPO->setText(QString().setNum(hc.tmaxpolar, 'g', hc.precision));
    xminPO->setText(QString().setNum(hc.xmin2d[hcgt_get_idx(HCGT_POLAR)], 'g', hc.precision));
    xmaxPO->setText(QString().setNum(hc.xmax2d[hcgt_get_idx(HCGT_POLAR)], 'g', hc.precision));
    yminPO->setText(QString().setNum(hc.ymin2d[hcgt_get_idx(HCGT_POLAR)], 'g', hc.precision));
    ymaxPO->setText(QString().setNum(hc.ymax2d[hcgt_get_idx(HCGT_POLAR)], 'g', hc.precision));  
    break;

  default:
    break;
  }
}


// x and y are relative to hc_graph.c's MEM_DRIVER_X and MEM_DRIVER_Y
void HCGGraphWindow::zoom(double x, double y, double zoomfactor, double movefactor)
{
  HCGZoomThread *zThread = new HCGZoomThread(x, y, zoomfactor, movefactor, gtypes->currentIndex() + 1, this->lineText());
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
  double x,y;
  switch (event->button())
  {
  case Qt::LeftButton:
    setCursor(QCursor(Qt::ArrowCursor));
    HCG_GET_XY(x,y);
    parentWindow->zoom(247 + movex - x, 184 + movey - y, 1, 1);
    break;

  default:
    event->ignore();
  }
}


void HCGGraphDisplay::mouseMoveEvent(QMouseEvent *event)
{
}


void HCGGraphDisplay::wheelEvent(QWheelEvent *event)
{
  if (zoomTimer->isActive()) {
    zoomTimer->stop();
  }
  zoomdelta += event->delta();
  QToolTip::showText(QCursor::pos(), "Release wheel to zoom " + QString().setNum(pow(HCG_ZOOM_FACTOR, zoomdelta / 8 / 15)) + "x", this);
  zoomTimer->start();
}
