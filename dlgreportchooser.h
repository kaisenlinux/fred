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

#ifndef DLGREPORTCHOOSER_H
#define DLGREPORTCHOOSER_H

#include <QDialog>
#include <QList>
#include <QTreeWidgetItem>

#include "reports.h"
#include "settings.h"

namespace Ui {
  class DlgReportChooser;
}

class DlgReportChooser : public QDialog {
  Q_OBJECT

  public:
    explicit DlgReportChooser(Reports *p_reps,
                              QString hive_type_string,
                              Settings *p_sets,
                              QWidget *p_parent=0);
    ~DlgReportChooser();

    QList<ReportTemplate*> GetSelectedReports();

  protected:
    void changeEvent(QEvent *p_event);

  private slots:
    void on_BtnCancel_clicked();
    void on_TrReports_currentItemChanged(QTreeWidgetItem *p_current,
                                         QTreeWidgetItem *p_previous);
    void on_BtnGenerate_clicked();

  private:
    Ui::DlgReportChooser *ui;
    Reports *p_reports;
    Settings *p_settings;
    QString hive_type;
    QList<QTreeWidgetItem*> tree_category_items;
    QList<ReportTemplate*> selected_reports;
};

#endif // DLGREPORTCHOOSER_H
