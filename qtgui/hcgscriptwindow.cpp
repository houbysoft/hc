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
#include <QMainWindow>
#include <QTextEdit>
#include <QCloseEvent>
#include <QSettings>
#include <QFile>
#include <QFont>
#include <QTextDocument>
#include <QFileDialog>
#include <QFileInfo>
#include <QTimer>
#include <QMenuBar>
#include <QMessageBox>
#include <QShortcut>
#include "hcgscriptwindow.hpp"
#include "main.hpp"


HCGScriptWindow::HCGScriptWindow(QString fname) : HCGBaseWindow()
{
  vbox = new QWidget(this);
  scripteditor = new QTextEdit(this);
  connect(scripteditor, SIGNAL(currentCharFormatChanged(const QTextCharFormat)), this, SLOT(resetFont()));
  connect(scripteditor, SIGNAL(textChanged()), this, SLOT(textChanged()));
  resetFont();
  scripteditor->setAcceptRichText(FALSE);
  if (fname.length() > 0)
    readFile(fname);

  runbutton = new QPushButton(this);
  resetRunButton();
  connect(runbutton, SIGNAL(clicked()), this, SLOT(runScript()));

  vbox_layout = new QVBoxLayout(vbox);
  vbox_layout->setSpacing(0);
  vbox_layout->setContentsMargins(1,1,1,1);
  vbox_layout->addWidget(scripteditor);
  vbox_layout->addWidget(runbutton);
  vbox->setLayout(vbox_layout);

  createFileMenu();
  createMenus();
  QShortcut *sc = new QShortcut(QKeySequence(tr("F9")), this);
  QObject::connect(sc, SIGNAL(activated()), this, SLOT(runScript()));

  setCentralWidget(vbox);
  settings = new QSettings(this);
  loadSettings();
  updateTitle();  
  show();
}


HCGScriptWindow::~HCGScriptWindow() {

}


void HCGScriptWindow::readFile(QString fname)
{
  QFile file(fname);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    notify_error_slot((char *)"Cannot open file.");
    return;
  }

  scripteditor->setPlainText(QString(file.readAll()));
  filename = fname;
  updateTitle();

  file.close();
}


void HCGScriptWindow::saveFile(QString fname)
{
  QFile file(fname);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    notify_error_slot((char *)"Cannot write file.");
    return;
  }

  if (file.write(scripteditor->document()->toPlainText().toAscii().data()) == -1)
  {
    notify_error_slot((char *)"Cannot write file");
    file.close();
    return;
  }

  filename = fname;
  scripteditor->document()->setModified(false);
  updateTitle();
  file.close();
}


void HCGScriptWindow::saveScript()
{
  if (filename.isEmpty())
  {
    saveScriptAs();
  } else {
    saveFile(filename);
  }
}


void HCGScriptWindow::saveScriptAs()
{
  QString filename = QFileDialog::getSaveFileName(this, "Save as", savedir);
  if (!filename.isNull())
  {
    savedir = QFileInfo(filename).dir().absolutePath();
    saveFile(filename);
  }
}


void HCGScriptWindow::runScript()
{
  saveScript();
  if (filename.isEmpty())
    return;
  setRunning(true);
  runThread = new HCGScriptThread(filename);
  connect(runThread, SIGNAL(finished()), this, SLOT(threadFinished()));
  runThread->start();
}


void HCGScriptWindow::setRunning(bool running)
{
  if (running)
  {
    runbutton->setText("Running");
    runbutton->setEnabled(false);
  } else {
    runbutton->setText("Script finished -- &Run Script Again?");
    runbutton->setEnabled(true);
    QTimer::singleShot(10000, this, SLOT(resetRunButton()));
  }
}


void HCGScriptWindow::threadFinished()
{
  setRunning(false);
}


void HCGScriptWindow::resetRunButton()
{
  runbutton->setText("&Run Script (F9)");
  runbutton->setEnabled(true);
}


void HCGScriptWindow::updateTitle()
{
  if (!filename.length())
  {
    setWindowTitle("Script Editor - HC");
  } else {
    if (scripteditor->document()->isModified())
    {
      setWindowTitle("*"+filename+" - Script Editor - HC");
    } else {
      setWindowTitle(filename+" - Script Editor - HC");
    }
  }
}


void HCGScriptWindow::resetFont(const QTextCharFormat f) {
  QFont font("Courier");
  font.setStyleHint(QFont::TypeWriter);
  scripteditor->setCurrentFont(font);
}


void HCGScriptWindow::textChanged() {
  if (filename.length())
    updateTitle();
}


void HCGScriptWindow::loadSettings() {
  settings->beginGroup("ScriptWindow");
  resize(settings->value("size", QSize(500,500)).toSize());
  opendir = settings->value("opendir", "").toString();
  savedir = settings->value("savedir", "").toString();
  settings->endGroup();
}


void HCGScriptWindow::writeSettings() {
  settings->beginGroup("ScriptWindow");
  settings->setValue("size",size());
  settings->setValue("opendir",opendir);
  settings->setValue("savedir",savedir);
  settings->endGroup();
}


bool HCGScriptWindow::maybeSave()
{
  if (scripteditor->document()->isModified())
  {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, "", "Save changes?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
    {
      saveScript();
      return !scripteditor->document()->isModified();
    } else if (ret == QMessageBox::Discard)
      return true;
    else
      return false;
  } else {
    return true;
  }
}


void HCGScriptWindow::closeEvent(QCloseEvent *event) {
  if (maybeSave())
  {
    writeSettings();
    event->accept();
  } else {
    event->ignore();
  }
}


void HCGScriptWindow::insert(QString string) {
  scripteditor->insertPlainText(string);
  focusInput();
}


void HCGScriptWindow::focusInput() {
  scripteditor->setFocus();
}


void HCGScriptWindow::openScript() {
  QString filename = QFileDialog::getOpenFileName(this, "Open File", opendir);
  if (!filename.isNull())
  {
    opendir = QFileInfo(filename).dir().absolutePath();
    if (this->filename.length() == 0 && scripteditor->document()->isEmpty())
    {
      readFile(filename);
    } else {
      new HCGScriptWindow(filename);
    }
  }
}


void HCGScriptWindow::createFileMenu() {
  struct SubMenu newmenu;
  newmenu.menu = menuBar()->addMenu("&File");
  QAction *newEval = new QAction("&New Evaluation Window", newmenu.menu);
  connect(newEval, SIGNAL(triggered()), this, SLOT(newEval()));
  newmenu.menu->addAction(newEval);
  QAction *newAct = new QAction("&New Script", newmenu.menu);
  newAct->setShortcut(tr("Ctrl+N"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newScript()));
  newmenu.menu->addAction(newAct);
  QAction *openAct = new QAction("&Open Script...", newmenu.menu);
  openAct->setShortcut(tr("Ctrl+O"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(openScript()));
  newmenu.menu->addAction(openAct);
  QAction *saveact = new QAction("&Save Script", newmenu.menu);
  saveact->setShortcut(tr("Ctrl+S"));
  connect(saveact, SIGNAL(triggered()), this, SLOT(saveScript()));
  newmenu.menu->addAction(saveact);
  QAction *saveasact = new QAction("&Save Script As...", newmenu.menu);
  saveasact->setShortcut(tr("Shift+Ctrl+S"));
  connect(saveasact, SIGNAL(triggered()), this, SLOT(saveScriptAs()));
  newmenu.menu->addAction(saveasact);
  QAction *quit = new QAction("&Close", newmenu.menu);
  quit->setShortcut(tr("Ctrl+W"));
  connect(quit, SIGNAL(triggered()), this, SLOT(close()));
  newmenu.menu->addAction(quit);
}
