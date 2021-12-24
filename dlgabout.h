/*******************************************************************************
* fred Copyright (c) 2011-2020 by Gillen Daniel <gillen.dan@pinguin.lu>        *
*                                                                              *
* Forensic Registry EDitor (fred) is a cross-platform M$ registry hive editor  *
* with special feautures useful during forensic analysis.                      *
*                                                                              *
* This program is free software: you can redistribute it and/or modify it      *
* under the terms of the GNU General Public License as published by the Free   *
* Software Foundation, either version 3 of the License, or (at your option)    *
* any later version.                                                           *
*                                                                              *
* This program is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for     *
* more details.                                                                *
*                                                                              *
* You should have received a copy of the GNU General Public License along with *
* this program. If not, see <http://www.gnu.org/licenses/>.                    *
*******************************************************************************/

#ifndef DLGABOUT_H
#define DLGABOUT_H

#include <QDialog>

namespace Ui {
  class DlgAbout;
}

class DlgAbout : public QDialog {
  Q_OBJECT

  public:
    DlgAbout(QWidget *parent = 0);
    ~DlgAbout();

  protected:
    void changeEvent(QEvent *e);

  private:
    Ui::DlgAbout *m_ui;

  private slots:
    void on_btnClose_clicked();
};

#endif // DLGABOUT_H
