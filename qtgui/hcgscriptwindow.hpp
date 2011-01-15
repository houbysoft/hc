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
#include <QTextEdit>
#include <QCloseEvent>
#include <QSettings>
#include <QVBoxLayout>
#include <QPushButton>
#include "hcgbasewindow.hpp"
#include "hcgthreads.hpp"

#ifndef HCGSCRIPTWINDOW_HPP
#define HCGSCRIPTWINDOW_HPP


class HCGScriptWindow : public HCGBaseWindow {
  Q_OBJECT


  protected:
  void closeEvent(QCloseEvent *event);


  private:
  QWidget *vbox;
  QPushButton *runbutton;
  QVBoxLayout *vbox_layout;
  QTextEdit *scripteditor;
  QSettings *settings;
  QString filename;
  HCGScriptThread *runThread;
  void setRunning(bool running);

  void loadSettings();
  void writeSettings();
  void readFile(QString fname);
  void saveFile(QString fname);
  void updateTitle();
  void createFileMenu();
  bool maybeSave();


  public slots:
  void resetFont(const QTextCharFormat f = QTextCharFormat());
  void textChanged();
  void insert(QString string);
  void openScript();
  void saveScript();
  void saveScriptAs();
  void runScript();
  void threadFinished();
  void resetRunButton();


  public:
  HCGScriptWindow(QString fname = "");
  ~HCGScriptWindow();
};

#endif
