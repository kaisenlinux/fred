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

#ifndef DLGSEARCH_H
#define DLGSEARCH_H

#include <QDialog>
#include <QList>
#include <QByteArray>
#include <QKeyEvent>

namespace Ui {
  class DlgSearch;
}

class DlgSearch : public QDialog {
  Q_OBJECT

  public:
    explicit DlgSearch(QWidget *parent = 0);
    ~DlgSearch();

    QList<QByteArray> Keywords();
    bool SearchNodeNames();
    bool SearchKeyNames();
    bool SearchKeyValues();

  protected:
    void changeEvent(QEvent *e);
    void keyPressEvent(QKeyEvent *p_event);

  private slots:
    void on_BtnCancel_clicked();
    void on_BtnSearch_clicked();
    void on_CbAscii_toggled(bool checked);
    void on_CbUtf16_toggled(bool checked);
    void on_CbHex_toggled(bool checked);

  private:
    Ui::DlgSearch *ui;
    QList<QByteArray> keywords;
    bool search_nodes;
    bool search_keys;
    bool search_values;
};

#endif // DLGSEARCH_H
