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

#ifndef HCGBASEWINDOW_HPP
#define HCGBASEWINDOW_HPP

#include <QObject>
#include <QMainWindow>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QProgressBar>
#include <QComboBox>
#include <QPushButton>
#include <QMenu>
#ifdef WIN32
#include <hul.h>
#endif

#define COMPLETER 0
#define ENTER_PRESSED 1
#define MAX_LOG_BLOCKS 2000


struct SubMenu {
  QMenu *menu;
  QList<struct SubMenu> menus;
};


class HCGBaseWindow : public QMainWindow {
  Q_OBJECT


  protected:
  void createMenus();
  void createShortcut(const char *seq, const QObject *rec, const char *method);
  QString opendir;
  QString savedir;
  QLineEdit *helpline;
  QLineEdit *convert_input;
  QComboBox *convert_unit_in;
  QComboBox *convert_unit_out;
  QPushButton *convert_go;

  private:
  void help(char caller);
  virtual void createFileMenu() = 0;
  void createOptionMenu();
  void createLogEdit();
  QSpinBox *prec_spinbox;
  QAction *rad,*grad,*deg,*normal,*sci,*eng,*rpn,*autoupdate,*white,*black;
  QTextEdit *log_edit;
#ifdef WIN32
  QProgressBar *updateBar;
#endif
  QMenu *conversionMenu;

  public:
  HCGBaseWindow();
  ~HCGBaseWindow();
  void notify(QString str);
  void notify_console(QString str);
  void notify_error(QString str);
  void prompt(QString str, QString *answer);
#ifdef WIN32
  void updateStatus(int status);
#endif
  void disp_rgb(unsigned int x, unsigned int y, void *data, int type, char *args);

  public slots:
  void newEval();
  void newScript();
  void checkUpdates();
#ifdef WIN32
  void checkUpdatesSilent();
#endif
  void about();
  void completerHelp(const QString text);
  void enterHelp();
  void generalHelp();
  void notify_slot(QString str);
  void notify_console_slot(QString str);
  void notify_error_slot(QString str);
  void prompt_slot(QString str, QString *answer);
#ifdef WIN32
  void updateStatus_slot(int status);
#endif
  void disp_rgb_slot(unsigned int x, unsigned int y, void *data, int type, char *args);
  void freeResult(QString expr, char *result);
  void setPrecision(int precision);
  void setAngleMode(QString mode);
  void setExpMode(QString mode);
  void setRPN(bool enabled);
  void setColor(char bgcolor);
#ifdef WIN32
  void setAutoUpdate(bool enabled);
  void processUpdateSilent(HUL *update);
  void processUpdateLoud(HUL *update);
  void processUpdate(HUL *update, bool silent);
  void applyUpdateThreadFinished(bool success);
#endif
  void doConversion();
  void showGraph();
  virtual void insert(QString string) = 0;
  virtual void focusInput() = 0;
  virtual void getInputResult() = 0;
  virtual void openScript() = 0;

  signals:
  void notify_signal(QString str);
  void notify_console_signal(QString str);
  void notify_error_signal(QString str);
  void prompt_signal(QString str, QString *answer);
#ifdef WIN32
  void updateStatus_signal(int status);
#endif
  void disp_rgb_signal(unsigned int x, unsigned int y, void *data, int type, char *args);
};

#endif
