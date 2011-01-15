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
#include <QLineEdit>
#include <QKeyEvent>


#ifndef HCGINPUTLINE_HPP
#define HCGINPUTLINE_HPP

#define HCG_IL_BACK 1
#define HCG_IL_FORWARD 2
#define HCG_IL_MAXITEMS 100

class HCGinputline : public QLineEdit {
  Q_OBJECT

  protected:
  void keyPressEvent(QKeyEvent *event);

  private:
  QStringList history_list;
  int history_cur;
  QString history(char direction);

  public:
  HCGinputline(QWidget *parent);
  ~HCGinputline();
  void history_insert(QString text, int index = -1);

  public slots:
  void insert(QString text);
};

#endif