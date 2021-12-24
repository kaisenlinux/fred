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

#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QTextDocument>
#include <QTextDocumentWriter>

#include "dlgreportviewer.h"
#include "ui_dlgreportviewer.h"

DlgReportViewer::DlgReportViewer(QString &report_data,
                                 Settings *p_sets,
                                 QWidget *p_parent)
  : QMainWindow(p_parent,Qt::Dialog | Qt::Popup), ui(new Ui::DlgReportViewer)
{
  // Init local vars
  ui->setupUi(this);
  this->p_local_event_loop=NULL;
  this->orig_report_data=report_data;
  this->p_settings=p_sets;

  // Try to restore dialog geometry
  QByteArray geometry=this->p_settings->GetWindowGeometry("DlgReportViewer");
  if(!geometry.isEmpty()) this->restoreGeometry(geometry);

  // Set report content
  this->ui->WebView->setHtml(report_data);

  // Set dialog title based on report content title
  QString report_title=this->ui->WebView->title();
  this->setWindowTitle("Report Viewer");
}

DlgReportViewer::~DlgReportViewer() {
  // Save dialog geometry
  this->p_settings->SetWindowGeometry("DlgReportViewer",this->saveGeometry());
  delete ui;
  if(this->p_local_event_loop!=NULL) this->p_local_event_loop->exit();
}

void DlgReportViewer::changeEvent(QEvent *p_event) {
  QMainWindow::changeEvent(p_event);
  switch(p_event->type()) {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
    default:
      break;
  }
}

void DlgReportViewer::closeEvent(QCloseEvent *p_event) {
  // Make sure we exit the local event loop on exit
  if(this->p_local_event_loop!=NULL) {
    this->p_local_event_loop->exit();
    this->p_local_event_loop=NULL;
  }
  p_event->accept();
}

void DlgReportViewer::exec() {
  // Create local event loop
  this->p_local_event_loop=new QEventLoop(this);
  // Show window and enter loop
  this->show();
  this->p_local_event_loop->exec();
}

void DlgReportViewer::on_action_Print_triggered() {
  // Print report
  QPrinter printer;
  QPrintDialog *p_dlg_print=new QPrintDialog(&printer);
  if(p_dlg_print->exec()==QDialog::Accepted) {
    this->ui->WebView->page()->print(&printer, [=](bool){});
  }
  delete p_dlg_print;
}

void DlgReportViewer::on_action_Close_triggered() {
  this->close();
}

void DlgReportViewer::on_action_Save_triggered() {
  QString filename=QFileDialog::getSaveFileName(this,
                                                tr("Save report as"),
                                                "",
                                                "ODF file (*.odf)");
  if(filename!="") {
    QTextDocument *p_doc=new QTextDocument(this);
    p_doc->setHtml(this->orig_report_data);
    QTextDocumentWriter *p_doc_writer=new QTextDocumentWriter(filename);
    p_doc_writer->setFormat(QByteArray("ODF"));
    p_doc_writer->write(p_doc);
    delete p_doc_writer;
    delete p_doc;
  }
}
