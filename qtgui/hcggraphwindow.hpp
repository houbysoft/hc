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

#ifndef HCGGRAPHWINDOW_HPP
#define HCGGRAPHWINDOW_HPP

#include <QObject>
#include <QLabel>
#include <QMouseEvent>
#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QGroupBox>


class HCGGraphDisplay : public QLabel {
  Q_OBJECT

  protected:
  void mousePressEvent(QMouseEvent *event);
  
  public:
  HCGGraphDisplay() {};
  ~HCGGraphDisplay() {};
};


class HCGGraphWindow : public QMainWindow {
  Q_OBJECT
  private:
  HCGGraphDisplay *gdisp;
  QWidget *hbox;
  QWidget *vbox;
  QHBoxLayout *hbox_layout;
  QVBoxLayout *vbox_layout;
  QLineEdit *lineedit;
  QComboBox *gtypes;
  QPushButton *go;
  QStackedWidget *options;
  QLineEdit *xmin2D;
  QLineEdit *xmax2D;
  QLineEdit *ymin2D;
  QLineEdit *ymax2D;
  QLineEdit *tminP;
  QLineEdit *tmaxP;
  QLineEdit *xminP;
  QLineEdit *xmaxP;
  QLineEdit *yminP;
  QLineEdit *ymaxP;
  QLineEdit *xmin3D;
  QLineEdit *xmax3D;
  QLineEdit *ymin3D;
  QLineEdit *ymax3D;
  QLineEdit *zmin3D;
  QLineEdit *zmax3D;
  QLineEdit *xminS;
  QLineEdit *xmaxS;
  QLineEdit *yminS;
  QLineEdit *ymaxS;

  public slots:
  void typechanged(QString &t);

  public:
  HCGGraphWindow();
  ~HCGGraphWindow() {};
  void updateGraph(QPixmap map, unsigned int x, unsigned int y, char *args);
};

#endif
