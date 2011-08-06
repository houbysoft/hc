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
#include <QWheelEvent>
#include <QCursor>
#include <QTimer>
#include <QToolTip>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenuBar>
#include <QCheckBox>
#include <math.h>
#include "hcgbasewindow.hpp"


#define HCG_ZOOM_FACTOR (1.05)
// HCG_MOVE_FACTOR runs from 0 to 1. 0 means no move, 1 means center at new point.
#define HCG_MOVE_FACTOR (0.1)
#define HCG_GET_XY(mx,my) {mx=event->x() - 90;my = event->y() - 56;if (mx < 0 || my < 0 || mx > 494 || my > 368){event->ignore();return;}my = 368 - my;}


class HCGGraphDisplay;


class HCGGraphWindow : public HCGBaseWindow {
  Q_OBJECT
  private:
  void updateOptions(int type);
  QString getFullForm();
  QString lineText();
  QLineEdit *getCurrentLineEdit();
  void createFileMenu();
  HCGGraphDisplay *gdisp;
  QWidget *hbox;
  QWidget *vbox;
  QHBoxLayout *hbox_layout;
  QVBoxLayout *vbox_layout;
  QLineEdit *line2D, *lineparx, *linepary, *linevp, *linevl, *line3D, *linesf, *linebx, *linepl;
  QPushButton *reset2Dbtn, *resetPbtn, *reset3Dbtn, *resetSbtn, *resetPObtn;
  QComboBox *gtypes;
  QStackedWidget *options;
  QStackedWidget *lineedits;
  QCheckBox *graph_imag_checkbox;
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
  QLineEdit *tminPO;
  QLineEdit *tmaxPO;
  QLineEdit *xminPO;
  QLineEdit *xmaxPO;
  QLineEdit *yminPO;
  QLineEdit *ymaxPO;
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
  unsigned int zoomTypesIndex;
  QString zoomLineText;


  public:
  HCGGraphWindow();
  ~HCGGraphWindow() {};
  void updateGraph(QPixmap map, unsigned int x, unsigned int y, int type, char *args);

  public slots:
  void drawGraph();
  void setCurrentIndex(int i);
  void setShowImag(bool enabled);
  void insert(QString string);
  void focusInput();
  void getInputResult() {};
  void openScript();
  void zoom(double x, double y, double zoomfactor, double movefactor);
  void reset2D(); void resetP(); void reset3D(); void resetS(); void resetPO();
};


class HCGGraphDisplay : public QLabel {
  Q_OBJECT

  private:
  HCGGraphWindow *parentWindow;
  double movex, movey;
  double zoomdelta;
  QTimer *zoomTimer;

  protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
  void hideEvent(QHideEvent *event) {
    zoomTimer->stop();
  };

  public:
  HCGGraphDisplay(HCGGraphWindow *pW) {
    parentWindow = pW;
    zoomdelta = 0;
    zoomTimer = new QTimer(this);
    zoomTimer->setInterval(1500);
    zoomTimer->setSingleShot(true);
    connect(zoomTimer, SIGNAL(timeout()), this, SLOT(doZoom()));
  };
  ~HCGGraphDisplay() {};

  public slots:
  void doZoom() {
    QToolTip::showText(QCursor::pos(), "", this);
    parentWindow->zoom(0, 0, pow(HCG_ZOOM_FACTOR, zoomdelta / 8 / 15), 0);
    zoomdelta = 0;
  };
};

#endif
