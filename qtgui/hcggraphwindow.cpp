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


#include "main.hpp"
#include "hcggraphwindow.hpp"


HCGGraphWindow::HCGGraphWindow() : QMainWindow() {
  hbox = new QWidget(this);
  vbox = new QWidget(this);
  gdisp = new HCGGraphDisplay();
  lineedit = new QLineEdit(this);

  vbox_layout = new QVBoxLayout(vbox);
  vbox_layout->setSpacing(0);
  vbox_layout->setContentsMargins(1,1,1,1);
  vbox_layout->setAlignment(Qt::AlignTop);

  vbox_layout->addWidget(new QLabel(" Graph type"));
  gtypes = new QComboBox(this);
  QStringList gtypeslist;
  gtypeslist << "2D" << "Parametric" << "Values (points)" << "Values (xyline)" << "3D" << "Slope Field";
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

  vbox_layout->addWidget(optionsBox);
  connect(gtypes, SIGNAL(activated(int)), options, SLOT(setCurrentIndex(int)));

  go = new QPushButton(" OK ", this);
  vbox_layout->addWidget(go);

  vbox->setLayout(vbox_layout);

  hbox_layout = new QHBoxLayout(hbox);
  hbox_layout->setSpacing(0);
  hbox_layout->setContentsMargins(1,1,1,1);
  hbox_layout->addWidget(gdisp);
  hbox_layout->addWidget(vbox);
  hbox->setLayout(hbox_layout);

  setCentralWidget(hbox);
  show();
}


void HCGGraphWindow::updateGraph(QPixmap map, unsigned int x, unsigned int y, char *args)
{
  gdisp->setPixmap(map);
  gdisp->setFixedSize(x,y);
  lineedit->setText(QString(args));
  show();
  activateWindow();
  raise();
}


void HCGGraphWindow::typechanged(QString &t)
{

}


void HCGGraphDisplay::mousePressEvent(QMouseEvent *event)
{
  switch (event->button())
  {
  case Qt::LeftButton:
    hcg->notify_slot("Should zoom in!");
    break;
  
  case Qt::RightButton:
    hcg->notify_slot("Should zoom out!");
    break;

  default:
    break;
  }
}
