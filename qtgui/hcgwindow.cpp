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

#include <QMenu>
#include <QAction>
#include <QTextCursor>
#include <QFileDialog>
#include <QCompleter>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QMenuBar>
#include <string.h>
#include "main.hpp"
#include "hcgwindow.hpp"
#include "hcginputline.hpp"
#include "hcgscriptwindow.hpp"


extern "C" void hcg_disp_rgb(unsigned int x, unsigned int y, void *data, int type, char *args, char *tl, char *xl, char *yl, char *zl);


HCGWindow::HCGWindow() : HCGBaseWindow() {
  vbox = new QWidget(this);
  scrollback = new QTextEdit("Welcome to HC! To evaluate an expression, type it below and press enter.",vbox);
  scrollback->document()->setMaximumBlockCount(MAX_SCROLLBACK_BLOCKS);
  scrollback->setReadOnly(TRUE);
  insertAtEnd(PLAIN,"\n");
  inputline = new HCGinputline(vbox);
  connect(inputline, SIGNAL(returnPressed()), this, SLOT(getInputResult()));

  vbox_layout = new QVBoxLayout(vbox);
  vbox_layout->setSpacing(0);
  vbox_layout->setContentsMargins(1,1,1,1);
  vbox_layout->addWidget(scrollback);
  vbox_layout->addWidget(inputline);
  vbox->setLayout(vbox_layout);

  settings = new QSettings(this);
  loadSettings();

  createFileMenu();
  createMenus();

  setCentralWidget(vbox);
  setWindowTitle("HoubySoft Calculator");
  inputline->setFocus();
  show();
#ifdef WIN32
  if (hcgcore->tryUpdate())
  {
    this->checkUpdatesSilent();
  }
#endif
}


HCGWindow::~HCGWindow() {
}


void HCGWindow::loadSettings() {
  settings->beginGroup("Window");
  resize(settings->value("size", QSize(500,500)).toSize());
  opendir = settings->value("opendir", "").toString();
  settings->endGroup();
}


void HCGWindow::writeSettings() {
  settings->beginGroup("Window");
  settings->setValue("size",size());
  settings->setValue("opendir",opendir);
  settings->endGroup();
}


void HCGWindow::closeEvent(QCloseEvent *event) {
  writeSettings();
  event->accept();
}


void HCGWindow::insertAtEnd(char type, QString text) {
  scrollback->moveCursor(QTextCursor::End);
  if (type == HTML)
    scrollback->insertHtml(text);
  else if (type == PLAIN)
    scrollback->insertPlainText(text);
  scrollback->moveCursor(QTextCursor::End);
}


void HCGWindow::getInputResult() {
  setRunning(true);
  HCGResultThread *resThread = new HCGResultThread(inputline->text());
  connect(resThread, SIGNAL(computation_finished(QString, char *)), this, SLOT(computation_finished(QString, char *)));
  resThread->start();
}


void HCGWindow::computation_finished(QString expr, char *r)
{
  insertAtEnd(PLAIN, "> " + expr + "\n");
  inputline->history_insert(expr);

  if (r)
  {
    if (strlen(r))
    {
      insertAtEnd(PLAIN, " = " + QString(r) + "\n");
    }
    free(r);
  }
  setRunning(false);
}


void HCGWindow::setRunning(bool running)
{
  if (running)
  {
    inputline->setEnabled(false);
  } else {
    inputline->setEnabled(true);
    inputline->selectAll();
    inputline->setFocus();
  }
}


void HCGWindow::insert(QString string)
{
  inputline->insert(string);
  focusInput();
}


void HCGWindow::focusInput()
{
  inputline->setFocus();
}


void HCGWindow::openScript() {
  QString filename = QFileDialog::getOpenFileName(this, "Open File", opendir);
  if (!filename.isNull())
  {
    opendir = QFileInfo(filename).dir().absolutePath();
    new HCGScriptWindow(filename);
  }
}


void HCGWindow::createFileMenu() {
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


void notify(char *str)
{
  hcg->notify(QString(str));
}


void notify_console(char *str)
{
  hcg->notify_console(QString(str));
}


void notify_error(char *str)
{
  hcg->notify_error(QString(str));
}


#ifdef WIN32
void updateStatusCallback(int status)
{
  hcg->updateStatus(status);
}
#endif


char *prompt(char *str)
{
  QString answer;
  hcg->prompt(QString(str), &answer);
  return strdup((char *)answer.toAscii().data());
}


void hcg_disp_rgb(unsigned int x, unsigned int y, void *data, int type, char *args, char *tl, char *xl, char *yl, char *zl)
{
  hcg->disp_rgb(x, y, data, type, args, tl, xl, yl, zl);
}
