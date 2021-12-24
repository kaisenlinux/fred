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

#include <QString>
#include <QStringList>
#include <QListIterator>

#include <QDebug>

#include "dlgreportchooser.h"
#include "ui_dlgreportchooser.h"

DlgReportChooser::DlgReportChooser(Reports *p_reps,
                                   QString hive_type_string,
                                   Settings *p_sets,
                                   QWidget *p_parent)
  : QDialog(p_parent), ui(new Ui::DlgReportChooser)
{
  QStringList tree_cats;
  QString cur_tree_cat;
  QTreeWidgetItem *p_tree_cat_widget;
  QTreeWidgetItem *p_tree_cat_rep_widget;
  QList<ReportTemplate*> tree_cat_reports;
  ReportTemplate* p_report;

  // Init private vars
  this->ui->setupUi(this);
  this->p_reports=p_reps;
  this->p_settings=p_sets;
  this->hive_type=hive_type_string;
  this->tree_category_items.clear();
  this->selected_reports.clear();

  // Restore dialog geometry if possible
  QByteArray geometry=this->p_settings->GetWindowGeometry("DlgReportChooser");
  if(!geometry.isEmpty()) this->restoreGeometry(geometry);

  // Populate tree with reports
  tree_cats=this->p_reports->GetAvailableReportCategories();
  QListIterator<QString> tree_cat_it(tree_cats);
  while(tree_cat_it.hasNext()) {
    cur_tree_cat=tree_cat_it.next();
    p_tree_cat_widget=new QTreeWidgetItem(this->ui->TrReports);
    p_tree_cat_widget->setText(0,cur_tree_cat);
    p_tree_cat_widget->setFlags(Qt::ItemIsEnabled|
                                Qt::ItemIsUserCheckable|
                                Qt::ItemIsTristate);
    p_tree_cat_widget->setCheckState(0,Qt::Unchecked);
    tree_cat_reports=this->p_reports->GetAvailableReports(cur_tree_cat);
    QListIterator<ReportTemplate*> tree_cat_rep_it(tree_cat_reports);
    while(tree_cat_rep_it.hasNext()) {
      p_report=tree_cat_rep_it.next();

      p_tree_cat_rep_widget=new QTreeWidgetItem(p_tree_cat_widget);
      // Save pointer to ReportTemplate alongside
      p_tree_cat_rep_widget->setData(0,
                                     Qt::UserRole,
                                     QVariant().fromValue(p_report));
      p_tree_cat_rep_widget->setText(0,p_report->Name());
      p_tree_cat_rep_widget->setFlags(Qt::ItemIsEnabled|
                                      Qt::ItemIsSelectable|
                                      Qt::ItemIsUserCheckable);
      if(this->hive_type!="UNKNOWN" && this->hive_type==p_report->Hive()) {
        p_tree_cat_rep_widget->setCheckState(0,Qt::Checked);
      } else {
        p_tree_cat_rep_widget->setCheckState(0,Qt::Unchecked);
      }
    }
    this->tree_category_items.append(p_tree_cat_widget);
  }

  // Finally, expand all categories
  this->ui->TrReports->expandAll();
}

DlgReportChooser::~DlgReportChooser() {
  // Save dialog geometry
  this->p_settings->SetWindowGeometry("DlgReportChooser",this->saveGeometry());
  delete this->ui;
}

QList<ReportTemplate*> DlgReportChooser::GetSelectedReports()
{
  return this->selected_reports;
}

void DlgReportChooser::changeEvent(QEvent *p_event) {
  QDialog::changeEvent(p_event);
  switch (p_event->type()) {
    case QEvent::LanguageChange:
      this->ui->retranslateUi(this);
      break;
    default:
      break;
  }
}

void DlgReportChooser::on_BtnCancel_clicked() {
  this->reject();
}

void DlgReportChooser::on_TrReports_currentItemChanged(
  QTreeWidgetItem *p_current, QTreeWidgetItem *p_previous)
{
  Q_UNUSED(p_previous)

  // If item has no parent, clear labels and return
  if(p_current->parent()==NULL) {
    this->ui->LblHive->clear();
    this->ui->LblAuthor->clear();
    this->ui->LblDesc->clear();
    return;
  }

  // Update labels
  this->ui->LblHive->setText(
    p_current->data(0,Qt::UserRole).value<ReportTemplate*>()->Hive());
  this->ui->LblAuthor->setText(
    p_current->data(0,Qt::UserRole).value<ReportTemplate*>()->Author());
  this->ui->LblDesc->setText(
    p_current->data(0,Qt::UserRole).value<ReportTemplate*>()->Description());
}

void DlgReportChooser::on_BtnGenerate_clicked() {
  QTreeWidgetItem *p_cat;
  int i;

  // Add selected reports to selected_reports
  QListIterator<QTreeWidgetItem*> cat_it(this->tree_category_items);
  while(cat_it.hasNext()) {
    p_cat=cat_it.next();
    for(i=0;i<p_cat->childCount();i++) {
      if(p_cat->child(i)->checkState(0)==Qt::Checked) {
        // Get saved pointer to ReportTemplate and append it to selected_reps
        this->selected_reports.append(p_cat->
          child(i)->data(0,Qt::UserRole).value<ReportTemplate*>());
      }
    }
  }

  this->accept();
}
