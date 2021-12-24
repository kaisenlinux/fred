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

#ifndef DLGPREFERENCES_H
#define DLGPREFERENCES_H

#include <QDialog>
#include <QModelIndex>

#include "settings.h"

namespace Ui {
  class DlgPreferences;
}

class DlgPreferences : public QDialog {
  Q_OBJECT

  public:
    explicit DlgPreferences(Settings *p_sets, QWidget *p_parent=0);
    ~DlgPreferences();

  private slots:
    void on_BtnCancel_clicked();
    void on_ListReportLocations_clicked(const QModelIndex &index);
    void on_BtnAddReportLoc_clicked();
    void on_BtnEditReportLoc_clicked();
    void on_BtnRemoveReportLoc_clicked();
    void on_BtnMoveReportLocUp_clicked();
    void on_BtnMoveReportLocDown_clicked();
    void on_BtnReset_clicked();

    void on_BtnOk_clicked();

  private:
    Ui::DlgPreferences *ui;
    Settings *p_settings;

    void LoadPreferences();
    void SavePreferences();
};

#endif // DLGPREFERENCES_H
