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

#include "dlgabout.h"
#include "ui_dlgabout.h"

#include "compileinfo.h"

DlgAbout::DlgAbout(QWidget *parent) :
  QDialog(parent),
  m_ui(new Ui::DlgAbout)
{
  m_ui->setupUi(this);
  this->setWindowTitle(tr("About %1").arg(APP_NAME));

  // Update dialog with current infos
  this->m_ui->LblAbout->setText(
    this->m_ui->LblAbout->text().replace("%APP_NAME_LONG%",APP_NAME_LONG));
  this->m_ui->LblAbout->setText(
    this->m_ui->LblAbout->text().replace("%APP_NAME%",APP_NAME));
  this->m_ui->LblAbout->setText(
    this->m_ui->LblAbout->text().replace("%APP_COPYRIGHT%",APP_COPYRIGHT));
  this->m_ui->TextEditInfo->setPlainText(
    this->m_ui->TextEditInfo->toPlainText().replace("%APP_NAME%",APP_NAME));
  this->m_ui->TextEditInfo->setPlainText(
    this->m_ui->TextEditInfo->toPlainText().replace("%APP_VERSION%",
                                                    APP_VERSION));
  this->m_ui->TextEditInfo->setPlainText(
    this->m_ui->TextEditInfo->toPlainText().replace("%APP_DESCRIPTION%",
                                                    APP_DESCRIPTION));
  this->m_ui->TextEditCopyright->setPlainText(
    this->m_ui->TextEditCopyright->toPlainText().replace("%APP_COPYRIGHT%",
                                                         APP_COPYRIGHT));
  this->m_ui->TextEditCopyright->setPlainText(
    this->m_ui->TextEditCopyright->toPlainText()
      .replace("%APP_DEVELOPPER_EMAIL%",
               APP_DEVELOPPER_EMAIL));
}

DlgAbout::~DlgAbout() {
  delete m_ui;
}

void DlgAbout::changeEvent(QEvent *e) {
  QDialog::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      m_ui->retranslateUi(this);
      break;
    default:
      break;
  }
}

void DlgAbout::on_btnClose_clicked() {
  this->accept();
}
