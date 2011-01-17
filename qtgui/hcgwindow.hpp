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

#ifndef HCGWINDOW_HPP
#define HCGWINDOW_HPP

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QSettings>
#include <QCloseEvent>
#include <QList>
#include "hcgbasewindow.hpp"
#include "hcginputline.hpp"


#define PLAIN 0
#define HTML 1


class HCGWindow : public HCGBaseWindow {
  Q_OBJECT


  protected:
  void closeEvent(QCloseEvent *event);


  private:
  QWidget *vbox;
  QVBoxLayout *vbox_layout;
  QTextEdit *scrollback;
  HCGinputline *inputline;
  QSettings *settings;

  void loadSettings();
  void writeSettings();
  void createFileMenu();
  void setRunning(bool running);


  public slots:
  void getInputResult();
  void insert(QString string);
  void openScript();
  void computation_finished(QString, char *);


  public:
  HCGWindow();
  ~HCGWindow();
  void insertAtEnd(char type, QString text);
};

#endif