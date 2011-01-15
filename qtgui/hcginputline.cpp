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
#include <QWidget>
#include <QLineEdit>
#include <QKeyEvent>
#include "hcginputline.hpp"


HCGinputline::HCGinputline(QWidget *parent) : QLineEdit(parent) {
  history_cur = -1;
  setModified(FALSE);
}


HCGinputline::~HCGinputline() {

}


QString HCGinputline::history(char direction)
{
  if (direction == HCG_IL_BACK)
  {
    if (history_cur > 0)
      return history_list[--history_cur];
    else if (history_cur == 0)
      return history_list[0];
    else
      return NULL;
  } else { // direction == HCG_IL_FORWARD
    if (history_cur >= 0 && history_list.length() > 0)
    {
      if (history_cur < history_list.length() - 1)
        return history_list[++history_cur];
      else
        return history_list[history_list.length() - 1];
    } else {
      return NULL;
    }
  }
}


void HCGinputline::history_insert(QString text, int index)
{
  if (index == -1) // insert at the end of the list
  {
    if (history_list.length() > 0 && history_list[history_list.length() - 1].length() == 0)
      history_list[history_list.length() - 1] = text;
    else if (history_list.length() == 0 || history_list[history_list.length() - 1] != text)
    {
      if (history_list.length() >= HCG_IL_MAXITEMS)
        history_list.removeFirst();
      history_list += text;
    }
    history_cur = history_list.length() - 1;
  } else { // insert at the specified index
    if (index == history_list.length() - 1)
      history_insert(text);
    else
      history_list[index] = text;
  }
}


void HCGinputline::insert(QString text)
{
  QLineEdit::insert(text);
}


void HCGinputline::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
  {
    char direction = event->key() == Qt::Key_Up ? HCG_IL_BACK : HCG_IL_FORWARD;
    if (isModified()) history_insert(text(), history_cur);
    setModified(FALSE);
    QString tmp = history(direction);
    if (!tmp.isNull())
      setText(tmp);
  } else {
    QLineEdit::keyPressEvent(event);
  }
}