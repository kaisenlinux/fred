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

#ifndef HEXEDITWIDGET_H
#define HEXEDITWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QLabel>
#include <QByteArray>

#include "qhexedit/qhexedit.h"
#include "datainterpreterwidget.h"

class HexEditWidget : public QWidget {
  Q_OBJECT

  public:
    explicit HexEditWidget(QWidget *p_parent=0,
                           bool with_data_interpreter=true,
                           bool is_read_only=true);
    ~HexEditWidget();

    void SetData(QByteArray const &data);
    QByteArray GetData();

  signals:

  public slots:

  private:
    // Widget layout
    QVBoxLayout *p_widget_layout;
    QSplitter *p_widget_splitter;
    // Sub-widgets
    QWidget *p_hex_edit_layout_widget;
    QVBoxLayout *p_hex_edit_layout;
    QHexEdit *p_hex_edit;
    QLabel *p_hex_edit_status_bar;
    DataInterpreterWidget *p_data_interpreter_widget;
    // Vars
    bool has_data_interpreter;
    bool read_only;
    QByteArray data;

  private slots:
    void SlotHexEditOffsetChanged(int offset);
};

#endif // HEXEDITWIDGET_H
