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
  connect(gtypes, SIGNAL(activated(QString &)), this, SLOT(typechanged(QString &)));
  vbox_layout->addWidget(gtypes);

  options = new QStackedWidget(this);
  options->addWidget(new QLabel("TODO"));
  vbox_layout->addWidget(options);

  vbox_layout->addWidget(new QLabel(" Expression"));
  vbox_layout->addWidget(lineedit);

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
