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
#include <QCoreApplication>
#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QSignalMapper>
#include <QWidgetAction>
#include <QCompleter>
#include <QMessageBox>
#include <QShortcut>
#include <QInputDialog>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QSpinBox>
#ifdef WIN32
#include <hul.h>
#endif
#include "hcgbasewindow.hpp"
#include "hcgscriptwindow.hpp"
#include "main.hpp"
#include "../hc_names.h"
#include "../hc_info.h"


HCGBaseWindow::HCGBaseWindow() : QMainWindow() {
  connect(this, SIGNAL(notify_signal(QString)), this, SLOT(notify_slot(QString)), Qt::BlockingQueuedConnection);
  connect(this, SIGNAL(notify_console_signal(QString)), this, SLOT(notify_console_slot(QString)), Qt::BlockingQueuedConnection);
  connect(this, SIGNAL(notify_error_signal(QString)), this, SLOT(notify_error_slot(QString)), Qt::BlockingQueuedConnection);
  connect(this, SIGNAL(disp_rgb_signal(unsigned int, unsigned int, void *)), this, SLOT(disp_rgb_slot(unsigned int, unsigned int, void *)), Qt::BlockingQueuedConnection);
  connect(this, SIGNAL(prompt_signal(QString, QString *)), this, SLOT(prompt_slot(QString, QString *)), Qt::BlockingQueuedConnection);
  connect(hcgcore, SIGNAL(closeAll_signal()), this, SLOT(close()));
  createShortcut("Ctrl+O", this, SLOT(openScript()));
  createShortcut("Ctrl+N", this, SLOT(newScript()));

  log_edit = NULL;
}


HCGBaseWindow::~HCGBaseWindow() {
}


void HCGBaseWindow::createMenus() {
  createOptionMenu();

  QList<struct SubMenu> TopMenus;
  QList<struct SubMenu> *top = &TopMenus;

  QSignalMapper *insertMapper = new QSignalMapper(this);
  connect(insertMapper, SIGNAL(mapped(QString)), this, SLOT(insert(QString)));
  QMenu *menu = NULL,*topmenu = NULL;
  for (unsigned int i=0; i < HC_NAMES; i++)
  {
    if (hc_names[i][2][0] == '#')
      continue;
    top = &TopMenus;
    QStringList path = QString(hc_names[i][2]).split("/");
    int nested=0;
    while (!path.isEmpty())
    {
      QString curpath = path.takeFirst();
      menu = NULL;
      for (int j = 0; j < top->size(); j++)
      {
        if (top->at(j).menu->title() == curpath)
        {
          menu = topmenu = top->at(j).menu;
          top = (QList<struct SubMenu> *)&(top->at(j).menus);
          break;
        }
      }
      if (!menu)
      {
        struct SubMenu newmenu;
        if (nested == 0)
          newmenu.menu = menu = topmenu = menuBar()->addMenu(curpath);
        else
          newmenu.menu = menu = topmenu = topmenu->addMenu(curpath);
        top->append(newmenu);
        top = (QList<struct SubMenu> *)&(top->at(top->size()-1).menus);
      }
      nested++;
    }
    QString txt,inserttxt;
    if (QString(hc_names[i][1]).startsWith("func"))
    {
      txt = QString(hc_names[i][0]) + "(" + QString(hc_names[i][1]).mid(5) + ") - " + QString(hc_names[i][3]);
      inserttxt = QString(hc_names[i][0]) + "(";
    } else {
      if (QString(hc_names[i][3]).size() != 0)
        txt = QString(hc_names[i][0]) + " - " + QString(hc_names[i][3]);
      else
        txt = hc_names[i][0];
      inserttxt = hc_names[i][0];
    }
    QAction *act = new QAction(txt, menu);
    insertMapper->setMapping(act, inserttxt);
    connect(act, SIGNAL(triggered()), insertMapper, SLOT(map()));
    menu->addAction(act);
  }

  {
    struct SubMenu newmenu;
    newmenu.menu = menuBar()->addMenu("Help");
    QWidgetAction *findhelp = new QWidgetAction(newmenu.menu);
    helpline = new QLineEdit(newmenu.menu);
    QStringList helplinelist;
    for (unsigned int i = 0; i < HC_NAMES; i++)
    {
      if (QString(hc_names[i][3]).length() > 0 || QString(hc_names[i][4]).length() > 0)
        helplinelist << hc_names[i][0];
    }
    QCompleter *helpcompleter = new QCompleter(helplinelist, helpline);
    connect(helpcompleter, SIGNAL(activated(const QString)), this, SLOT(completerHelp(const QString)));
    helpcompleter->setCaseSensitivity(Qt::CaseInsensitive);
    helpcompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    helpline->setCompleter(helpcompleter);
    connect(helpline, SIGNAL(returnPressed()), this, SLOT(enterHelp()));
    findhelp->setDefaultWidget(helpline);
    newmenu.menu->addAction(findhelp);
    QAction *help = new QAction("Help", newmenu.menu);
    connect(help, SIGNAL(triggered()), this, SLOT(generalHelp()));
    newmenu.menu->addAction(help);
#ifdef WIN32
    QAction *update = new QAction("Check for updates", newmenu.menu);
    connect(update, SIGNAL(triggered()), this, SLOT(checkUpdates()));
    newmenu.menu->addAction(update);
#endif
    QAction *about = new QAction("About", newmenu.menu);
    connect(about, SIGNAL(triggered()), this, SLOT(about()));
    newmenu.menu->addAction(about);
  }
}


void HCGBaseWindow::setRPN(bool enabled)
{
  rpn->setChecked(enabled);
}


void HCGBaseWindow::setPrecision(int precision)
{
  prec_spinbox->setValue(precision);
}


void HCGBaseWindow::setAngleMode(QString mode)
{
  if (mode == "r")
    rad->setChecked(true);
  else if (mode == "d")
    deg->setChecked(true);
  else if (mode == "g")
    grad->setChecked(true);
}


void HCGBaseWindow::setExpMode(QString mode)
{
  if (mode == "n")
    normal->setChecked(true);
  else if (mode == "s")
    sci->setChecked(true);
  else if (mode == "e")
    eng->setChecked(true);
}


void HCGBaseWindow::createOptionMenu() {
  QMenu *optmenu = menuBar()->addMenu("&Options");

  QSignalMapper *angle_mapper = new QSignalMapper(this);
  connect(angle_mapper, SIGNAL(mapped(QString)), hcgcore, SLOT(setAngleMode(QString)));
  QMenu *angle_mode = optmenu->addMenu("Angle mode");
  QActionGroup *angle_group = new QActionGroup(this);
  deg = new QAction("Degrees", angle_group); deg->setCheckable(true);
  rad = new QAction("Radians", angle_group); rad->setCheckable(true);
  grad = new QAction("Gradian", angle_group); grad->setCheckable(true);
  angle_mapper->setMapping(deg, "d");
  angle_mapper->setMapping(rad, "r");
  angle_mapper->setMapping(grad,"g");
  connect(deg, SIGNAL(triggered()), angle_mapper, SLOT(map()));
  connect(rad, SIGNAL(triggered()), angle_mapper, SLOT(map()));
  connect(grad, SIGNAL(triggered()), angle_mapper, SLOT(map()));
  angle_mode->addAction(deg); angle_mode->addAction(rad); angle_mode->addAction(grad);

  QSignalMapper *exp_mapper = new QSignalMapper(this);
  connect(exp_mapper, SIGNAL(mapped(QString)), hcgcore, SLOT(setExpMode(QString)));
  QMenu *exp_mode = optmenu->addMenu("Exponent mode");
  QActionGroup *exp_group = new QActionGroup(this);
  normal = new QAction("Normal", exp_group); normal->setCheckable(true);
  sci = new QAction("Scientific", exp_group); sci->setCheckable(true);
  eng = new QAction("Engineering", exp_group); eng->setCheckable(true);
  exp_mapper->setMapping(normal, "n");
  exp_mapper->setMapping(sci, "s");
  exp_mapper->setMapping(eng, "e");
  connect(normal, SIGNAL(triggered()), exp_mapper, SLOT(map()));
  connect(sci, SIGNAL(triggered()), exp_mapper, SLOT(map()));
  connect(eng, SIGNAL(triggered()), exp_mapper, SLOT(map()));
  exp_mode->addAction(normal); exp_mode->addAction(sci); exp_mode->addAction(eng);

  rpn = new QAction("Use RPN notation", optmenu); rpn->setCheckable(true);
  connect(rpn, SIGNAL(triggered(bool)), hcgcore, SLOT(setRPN(bool)));
  optmenu->addAction(rpn);

  QMenu *prec_menu = optmenu->addMenu("Precision digits");
  QWidgetAction *prec = new QWidgetAction(prec_menu);
  prec_spinbox = new QSpinBox(prec_menu);
  prec_spinbox->setMinimum(1); prec_spinbox->setMaximum(8192);
  connect(prec_spinbox, SIGNAL(valueChanged(int)), hcgcore, SLOT(setPrecision(int)));
  prec->setDefaultWidget(prec_spinbox);
  prec_menu->addAction(prec);

  connect(hcgcore, SIGNAL(precisionChanged(int)), this, SLOT(setPrecision(int)));
  connect(hcgcore, SIGNAL(angleModeChanged(QString)), this, SLOT(setAngleMode(QString)));
  connect(hcgcore, SIGNAL(expModeChanged(QString)), this, SLOT(setExpMode(QString)));
  connect(hcgcore, SIGNAL(RPNChanged(bool)), this, SLOT(setRPN(bool)));
  hcgcore->emitSignals();
}


void HCGBaseWindow::newScript() {
  new HCGScriptWindow();
}


void HCGBaseWindow::newEval() {
  new HCGWindow();
}


void HCGBaseWindow::about() {
  QMessageBox::about(this,"About HC","HC "+QString(VERSION)+"\n\nA high-precision scientific programmable calculator.\n\nAvailable under the terms of the GNU GPL v3 License.\n(c) Jan Dlabal, 2010-2011.\n\nhttp://houbysoft.com/hc/");
}


#ifdef WIN32
void HCGBaseWindow::processUpdate(HUL *update)
{
  if (update && update->version)
  {
    if (QMessageBox::question(hcg, "Update", "An update is available, would you like to download it?\n(Warning: this will close HC, please save your work)", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
    {
      hcgcore->closeAll();
      system("start updater.exe");
      QCoreApplication::exit(0);
    }
  } else if (update) {
    notify_slot((char *)"You have the latest version.");
  } else {
    notify_error_slot((char *)"An error occured while checking for updates.");
  }
}
#endif


void HCGBaseWindow::checkUpdates() {
#ifdef WIN32
  HCGUpdateThread *upThread = new HCGUpdateThread();
  connect(upThread, SIGNAL(checking_finished(HUL *)), this, SLOT(processUpdate(HUL *)));
  upThread->start();
#endif
}


void HCGBaseWindow::completerHelp(const QString text)
{
  this->help(COMPLETER);
}


void HCGBaseWindow::enterHelp()
{
  this->help(ENTER_PRESSED);
}


void HCGBaseWindow::help(char caller)
{
  if ((caller == COMPLETER && helpline->text().length() != 0) || (caller == ENTER_PRESSED))
  {
    HCGResultThread *resThread = new HCGResultThread(QString("help(\""+helpline->text()+"\")"));
    connect(resThread, SIGNAL(computation_finished(QString, char *)), this, SLOT(freeResult(QString, char *)));
    resThread->start();
    helpline->clear();
  }
}


void HCGBaseWindow::freeResult(QString expr, char *result)
{
  free(result);
}


void HCGBaseWindow::generalHelp()
{
  QMessageBox::information(this, "How to use Help", "The input line in the \"Help\" menu serves for giving you specific help information. To use it, simply enter the name of a function, constant, or a topic and press Enter.\nYou can also leave it blank and press Enter; it will then give you some general help information and names of topics you can use.\nAlternatively, you can also type help(\"name of topic goes here\") into the normal input line on the bottom of the calculator.");
}


void HCGBaseWindow::createShortcut(const char *seq, const QObject *rec, const char *method)
{
  QShortcut *sc = new QShortcut(QKeySequence(tr(seq)), this);
  QObject::connect(sc, SIGNAL(activated()), rec, method);
}


void HCGBaseWindow::createLogEdit()
{
  log_edit = new QTextEdit();
  log_edit->setReadOnly(true);
  log_edit->document()->setMaximumBlockCount(MAX_LOG_BLOCKS);
  log_edit->show();
}


void HCGBaseWindow::notify_slot(QString str)
{
  QMessageBox::information(0, "", str);
}


void HCGBaseWindow::notify_console_slot(QString str)
{
  if (!log_edit)
    createLogEdit();
  if (!log_edit->isVisible())
    log_edit->show();
  log_edit->moveCursor(QTextCursor::End);
  log_edit->insertPlainText(str);
  log_edit->moveCursor(QTextCursor::End);
}


void HCGBaseWindow::notify_error_slot(QString str)
{
  QMessageBox::critical(0, "", str);
}


void HCGBaseWindow::prompt_slot(QString str, QString *answer)
{
  *answer = QInputDialog::getText(0, "", str);
}


void HCGBaseWindow::disp_rgb_slot(unsigned int x, unsigned int y, void *data)
{
  QImage graph((uchar *)data, x, y, QImage::Format_RGB888);
  QLabel *label = new QLabel();
  label->setPixmap(QPixmap::fromImage(graph));
  label->setFixedSize(x, y);
  label->show();
}


void HCGBaseWindow::notify(QString str)
{
  emit notify_signal(str);
}


void HCGBaseWindow::notify_console(QString str)
{
  emit notify_console_signal(str);
}


void HCGBaseWindow::notify_error(QString str)
{
  emit notify_error_signal(str);
}


void HCGBaseWindow::prompt(QString str, QString *answer)
{
  emit prompt_signal(str, answer);
}


void HCGBaseWindow::disp_rgb(unsigned int x, unsigned int y, void *data)
{
  emit disp_rgb_signal(x, y, data);
}
