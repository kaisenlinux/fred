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

#include <QMessageBox>

#include "dlgsearch.h"
#include "ui_dlgsearch.h"


DlgSearch::DlgSearch(QWidget *parent) : QDialog(parent), ui(new Ui::DlgSearch)
{
  ui->setupUi(this);
}

DlgSearch::~DlgSearch() {
  delete ui;
}

QList<QByteArray> DlgSearch::Keywords() {
  return this->keywords;
}

bool DlgSearch::SearchNodeNames() {
  return this->search_nodes;
}

bool DlgSearch::SearchKeyNames() {
  return this->search_keys;
}

bool DlgSearch::SearchKeyValues() {
  return this->search_values;
}

void DlgSearch::changeEvent(QEvent *e) {
  QDialog::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
    default:
      break;
  }
}

void DlgSearch::keyPressEvent(QKeyEvent *p_event) {
  if(p_event->key()==Qt::Key_Return || p_event->key()==Qt::Key_Enter) {
    this->on_BtnSearch_clicked();
  } else {
    QDialog::keyPressEvent(p_event);
  }
}

void DlgSearch::on_BtnCancel_clicked() {
  this->reject();
}

void DlgSearch::on_BtnSearch_clicked() {
  if(this->ui->EdtValue->text()=="" ||
     (!this->ui->CbAscii->isChecked() &&
      !this->ui->CbUtf16->isChecked() &&
      !this->ui->CbHex->isChecked()))
  {
    // No value type specified
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Please specify a search value and type!"));
    return;
  }

  if(!this->ui->CbNodeNames->isChecked() &&
     !this->ui->CbKeyNames->isChecked() &&
     !this->ui->CbKeyValues->isChecked())
  {
    // No target specified
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Please specify a search target!"));
    return;
  }

  // Save settings
  QString keyword=this->ui->EdtValue->text();
  this->keywords.clear();
  if(this->ui->CbAscii->isChecked()) this->keywords.append(QByteArray(keyword.toLatin1()));
  if(this->ui->CbUtf16->isChecked()) {
    // TODO: .size()*2 will definetly fail sometimes!!!!
    this->keywords.append(QByteArray((char*)(keyword.utf16()),keyword.size()*2));
  }
  if(this->ui->CbHex->isChecked()) {
    // TODO: Convert to hex
  }

  this->search_nodes=this->ui->CbNodeNames->isChecked();
  this->search_keys=this->ui->CbKeyNames->isChecked();
  this->search_values=this->ui->CbKeyValues->isChecked();

  this->accept();
}

void DlgSearch::on_CbAscii_toggled(bool checked) {
  // It is not possible to search for text and hex
  if(checked && this->ui->CbHex->isChecked())
    this->ui->CbHex->setChecked(false);
}

void DlgSearch::on_CbUtf16_toggled(bool checked) {
  // It is not possible to search for text and hex
  if(checked && this->ui->CbHex->isChecked())
    this->ui->CbHex->setChecked(false);
}

void DlgSearch::on_CbHex_toggled(bool checked) {
  // It is not possible to search for text and hex
  if(checked) {
    this->ui->CbAscii->setChecked(false);
    this->ui->CbUtf16->setChecked(false);
  }
}
