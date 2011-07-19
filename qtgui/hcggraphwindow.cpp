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
#include "hcgscriptwindow.hpp"
#include "hcgthreads.hpp"
#include "../hc_info.h"


HCGGraphWindow::HCGGraphWindow() : HCGBaseWindow() {
  setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
  setFixedSize(sizeHint());

  createFileMenu();
  createMenus();

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
  QGridLayout *l2DGrid = new QGridLayout(this);
  l2D->setLayout(l2DGrid);
  l2DGrid->addWidget(new QLabel("y1(x), y2(x), ... = "), 0, 0);
  line2D = new QLineEdit();
  connect(line2D, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  l2DGrid->addWidget(line2D, 0, 1);
  graph_imag_checkbox = new QCheckBox("Show imaginary part", this);
  connect(graph_imag_checkbox, SIGNAL(clicked(bool)), this, SLOT(setShowImag(bool)));
  connect(hcgcore, SIGNAL(showImagChanged(bool)), graph_imag_checkbox, SLOT(setChecked(bool)));
  hcgcore->emitSignals();
  l2DGrid->addWidget(graph_imag_checkbox, 1, 1);
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
  lvpBox->addWidget(new QLabel("value_1, value_2, ... = "));
  linevp = new QLineEdit();
  connect(linevp, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  lvpBox->addWidget(linevp);
  lineedits->addWidget(lvp);

  // Values (xyline) line edit
  QWidget *lvl = new QWidget(this);
  QHBoxLayout *lvlBox = new QHBoxLayout(this);
  lvl->setLayout(lvlBox);
  lvlBox->addWidget(new QLabel("value_1, value_2, ... = "));
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
  lbxBox->addWidget(new QLabel("value_1, value_2, ... = "));
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
  QGridLayout *o2DL = new QGridLayout(this);
  o2D->setLayout(o2DL);
  xmin2D = new QLineEdit(this);
  connect(xmin2D, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  xmax2D = new QLineEdit(this);
  connect(xmax2D, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  ymin2D = new QLineEdit(this);
  connect(ymin2D, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  ymax2D = new QLineEdit(this);
  connect(ymax2D, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  reset2Dbtn = new QPushButton("Reset to defaults", this);
  connect(reset2Dbtn, SIGNAL(clicked()), this, SLOT(reset2D()));
  o2DL->addWidget(new QLabel("xmin: ", o2D), 0, 0);
  o2DL->addWidget(xmin2D, 0, 1);
  o2DL->addWidget(new QLabel("xmax: ", o2D), 1, 0);
  o2DL->addWidget(xmax2D, 1, 1);
  o2DL->addWidget(new QLabel("ymin: ", o2D), 2, 0);
  o2DL->addWidget(ymin2D, 2, 1);
  o2DL->addWidget(new QLabel("ymax: ", o2D), 3, 0);
  o2DL->addWidget(ymax2D, 3, 1);
  o2DL->addWidget(reset2Dbtn, 4, 1);
  options->addWidget(o2D);

  // Parametric options
  QWidget *oP = new QWidget(this);
  QGridLayout *oPL = new QGridLayout(this);
  oP->setLayout(oPL);
  tminP = new QLineEdit(this);
  connect(tminP, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  tmaxP = new QLineEdit(this);
  connect(tmaxP, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  xminP = new QLineEdit(this);
  connect(xminP, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  xmaxP = new QLineEdit(this);
  connect(xmaxP, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  yminP = new QLineEdit(this);
  connect(yminP, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  ymaxP = new QLineEdit(this);
  connect(ymaxP, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  resetPbtn = new QPushButton("Reset to defaults", this);
  connect(resetPbtn, SIGNAL(clicked()), this, SLOT(resetP()));
  oPL->addWidget(new QLabel("tmin: ", oP), 0, 0);
  oPL->addWidget(tminP, 0, 1);
  oPL->addWidget(new QLabel("tmax: ", oP), 1, 0);
  oPL->addWidget(tmaxP, 1, 1);
  oPL->addWidget(new QLabel("xmin: ", oP), 2, 0);
  oPL->addWidget(xminP, 2, 1);
  oPL->addWidget(new QLabel("xmax: ", oP), 3, 0);
  oPL->addWidget(xmaxP, 3, 1);
  oPL->addWidget(new QLabel("ymin: ", oP), 4, 0);
  oPL->addWidget(yminP, 4, 1);
  oPL->addWidget(new QLabel("ymax: ", oP), 5, 0);
  oPL->addWidget(ymaxP, 5, 1);
  oPL->addWidget(resetPbtn, 6, 1);
  options->addWidget(oP);

  // Values (points) options
  options->addWidget(new QLabel("No options available for this graph type"));

  // Values (xyline) options
  options->addWidget(new QLabel("No options available for this graph type"));

  // 3D options
  QWidget *o3D = new QWidget(this);
  QGridLayout *o3DL = new QGridLayout(this);
  o3D->setLayout(o3DL);
  xmin3D = new QLineEdit(this);
  connect(xmin3D, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  xmax3D = new QLineEdit(this);
  connect(xmax3D, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  ymin3D = new QLineEdit(this);
  connect(ymin3D, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  ymax3D = new QLineEdit(this);
  connect(ymax3D, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  zmin3D = new QLineEdit(this);
  connect(zmin3D, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  zmax3D = new QLineEdit(this);
  connect(zmax3D, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  reset3Dbtn = new QPushButton("Reset to defaults", this);
  connect(reset3Dbtn, SIGNAL(clicked()), this, SLOT(reset3D()));
  o3DL->addWidget(new QLabel("xmin: ", o3D), 0, 0);
  o3DL->addWidget(xmin3D, 0, 1);
  o3DL->addWidget(new QLabel("xmax: ", o3D), 1, 0);
  o3DL->addWidget(xmax3D, 1, 1);
  o3DL->addWidget(new QLabel("ymin: ", o3D), 2, 0);
  o3DL->addWidget(ymin3D, 2, 1);
  o3DL->addWidget(new QLabel("ymax: ", o3D), 3, 0);
  o3DL->addWidget(ymax3D, 3, 1);
  o3DL->addWidget(new QLabel("zmin: ", o3D), 4, 0);
  o3DL->addWidget(zmin3D, 4, 1);
  o3DL->addWidget(new QLabel("zmax: ", o3D), 5, 0);
  o3DL->addWidget(zmax3D, 5, 1);
  o3DL->addWidget(reset3Dbtn, 6, 1);
  options->addWidget(o3D);

  // Slope Field options
  QWidget *oS = new QWidget(this);
  QGridLayout *oSL = new QGridLayout(this);
  oS->setLayout(oSL);
  xminS = new QLineEdit(this);
  connect(xminS, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  xmaxS = new QLineEdit(this);
  connect(xmaxS, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  yminS = new QLineEdit(this);
  connect(yminS, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  ymaxS = new QLineEdit(this);
  connect(ymaxS, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  resetSbtn = new QPushButton("Reset to defaults", this);
  connect(resetSbtn, SIGNAL(clicked()), this, SLOT(resetS()));
  oSL->addWidget(new QLabel("xmin: ", oS), 0, 0);
  oSL->addWidget(xminS, 0, 1);
  oSL->addWidget(new QLabel("xmax: ", oS), 1, 0);
  oSL->addWidget(xmaxS, 1, 1);
  oSL->addWidget(new QLabel("ymin: ", oS), 2, 0);
  oSL->addWidget(yminS, 2, 1);
  oSL->addWidget(new QLabel("ymax: ", oS), 3, 0);
  oSL->addWidget(ymaxS, 3, 1);
  oSL->addWidget(resetSbtn, 4, 1);
  options->addWidget(oS);

  // Boxplot options
  options->addWidget(new QLabel("No options available for this graph type"));

  // Polar options
  QWidget *oPO = new QWidget(this);
  QGridLayout *oPOL = new QGridLayout(this);
  oPO->setLayout(oPOL);
  tminPO = new QLineEdit(this);
  connect(tminPO, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  tmaxPO = new QLineEdit(this);
  connect(tmaxPO, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  xminPO = new QLineEdit(this);
  connect(xminPO, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  xmaxPO = new QLineEdit(this);
  connect(xmaxPO, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  yminPO = new QLineEdit(this);
  connect(yminPO, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  ymaxPO = new QLineEdit(this);
  connect(ymaxPO, SIGNAL(returnPressed()), this, SLOT(drawGraph()));
  resetPObtn = new QPushButton("Reset to defaults", this);
  connect(resetPObtn, SIGNAL(clicked()), this, SLOT(resetPO()));
  oPOL->addWidget(new QLabel("tmin: "), 0, 0);
  oPOL->addWidget(tminPO, 0, 1);
  oPOL->addWidget(new QLabel("tmax: "), 1, 0);
  oPOL->addWidget(tmaxPO, 1, 1);
  oPOL->addWidget(new QLabel("xmin: "), 2, 0);
  oPOL->addWidget(xminPO, 2, 1);
  oPOL->addWidget(new QLabel("xmax: "), 3, 0);
  oPOL->addWidget(xmaxPO, 3, 1);
  oPOL->addWidget(new QLabel("ymin: "), 4, 0);
  oPOL->addWidget(yminPO, 4, 1);
  oPOL->addWidget(new QLabel("ymax: "), 5, 0);
  oPOL->addWidget(ymaxPO, 5, 1);
  oPOL->addWidget(resetPObtn, 6, 1);
  options->addWidget(oPO);

  vbox_layout->addWidget(optionsBox);
  connect(gtypes, SIGNAL(activated(int)), this, SLOT(setCurrentIndex(int)));

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
}


void HCGGraphWindow::setShowImag(bool enabled) {
  hcgcore->setShowImag(enabled);
  drawGraph();
}


void HCGGraphWindow::updateGraph(QPixmap map, unsigned int x, unsigned int y, int type, char *args2)
{
  gdisp->setPixmap(map);
  gdisp->setFixedSize(x,y);
  QString args = QString(args2).trimmed();
  if (type == HCGT_VALUESPOINTS || type == HCGT_VALUESLINE || type == HCGT_BOXPLOT) {
    if (args.startsWith('['))
      args.remove(0, 1);
    if (args.endsWith(']'))
      args.chop(1);
  }
  switch (type)
  {
  case HCGT_2D:
    line2D->setText(args);
    break;
  case HCGT_PARAMETRIC:
    lineparx->setText(args.split(",").first());
    linepary->setText(args.split(",").last());
    break;
  case HCGT_3D:
    line3D->setText(args);
    break;
  case HCGT_SLPFLD:
    linesf->setText(args);
    break;
  case HCGT_VALUESPOINTS:
    linevp->setText(args);
    break;
  case HCGT_VALUESLINE:
    linevl->setText(args);
    break;
  case HCGT_BOXPLOT:
    linebx->setText(args);
    break;
  case HCGT_POLAR:
    linepl->setText(args);
  default:
    break;
  }
  updateOptions(type);
  show();
  activateWindow();
  raise();
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



QLineEdit *HCGGraphWindow::getCurrentLineEdit() {
  switch (gtypes->currentIndex() + 1) {
  case HCGT_2D:
    return line2D;
  case HCGT_PARAMETRIC:
    if (lineparx->hasFocus())
      return lineparx;
    else if (linepary->hasFocus())
      return linepary;
    else
      return lineparx;
  case HCGT_VALUESPOINTS:
    return linevp;
  case HCGT_VALUESLINE:
    return linevl;
  case HCGT_3D:
    return line3D;
  case HCGT_SLPFLD:
    return linesf;
  case HCGT_BOXPLOT:
    return linebx;
  case HCGT_POLAR:
    return linepl;

  default:
    return line2D; // should never get here
  }
}


void HCGGraphWindow::insert(QString string) {
  getCurrentLineEdit()->insert(string);
  focusInput();
}


void HCGGraphWindow::focusInput() {
  getCurrentLineEdit()->setFocus();
}


void HCGGraphWindow::openScript() {
  QString filename = QFileDialog::getOpenFileName(this, "Open File", opendir);
  if (!filename.isNull()) {
    opendir = QFileInfo(filename).dir().absolutePath();
    new HCGScriptWindow(filename);
  }
}


void HCGGraphWindow::createFileMenu() {
  struct SubMenu newmenu;
  newmenu.menu = menuBar()->addMenu("&File");
  QAction *newEval = new QAction("&New Evaluation Window", newmenu.menu);
  newmenu.menu->addAction(newEval);
  connect(newEval, SIGNAL(triggered()), this, SLOT(newEval()));
  QAction *newAct = new QAction("&New Script", newmenu.menu);
  newAct->setShortcut(tr("Ctrl+N"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newScript()));
  newmenu.menu->addAction(newAct);
  QAction *openAct = new QAction("&Open Script...", newmenu.menu);
  openAct->setShortcut(tr("Ctrl+O"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(openScript()));
  newmenu.menu->addAction(openAct);
  QAction *quit = new QAction("&Close", newmenu.menu);
  quit->setShortcut(tr("Ctrl+W"));
  connect(quit, SIGNAL(triggered()), this, SLOT(close()));
  newmenu.menu->addAction(quit);
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
    cmd += "graphpoints([";
    break;

  case HCGT_VALUESLINE:
    cmd += "graphvalues([";
    break;

  case HCGT_3D:
    cmd += "graph3(";
    break;

  case HCGT_SLPFLD:
    cmd += "slpfld(";
    break;

  case HCGT_BOXPLOT:
    cmd += "boxplot([";
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

  case HCGT_VALUESPOINTS:
  case HCGT_VALUESLINE:
    cmd += "]";
    break;

  case HCGT_3D:
    cmd += "," + xmin3D->text() + "," + xmax3D->text() + "," + ymin3D->text() + "," + ymax3D->text() + "," + zmin3D->text() + "," + zmax3D->text();
    break;

  case HCGT_SLPFLD:
    cmd += "," + xminS->text() + "," + xmaxS->text() + "," + yminS->text() + "," + ymaxS->text();
    break;

  case HCGT_BOXPLOT:
    cmd += "]";
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
