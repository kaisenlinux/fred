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

#ifndef DATAINTERPRETERWIDGET_H
#define DATAINTERPRETERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QString>
#include <QByteArray>

#include "datainterpretertable.h"

class DataInterpreterWidget : public QWidget {
  Q_OBJECT

  public:
    enum Endianness {
      Endianness_LittleEndian=0,
      Endianness_BigEndian
    };

    explicit DataInterpreterWidget(QWidget *p_parent=0);
    ~DataInterpreterWidget();

    /*
     * SetData
     *
     * Set data to be interpreted (will also interpret it).
     */
    void SetData(QByteArray new_data);

  private:
    // Widget layout
    QVBoxLayout *p_widget_layout;
    // Sub-widgets
    DataInterpreterTable *p_data_interpreter_table;
    QHBoxLayout *p_endianness_selector_layout;
    QRadioButton *p_endianness_selector_le;
    QRadioButton *p_endianness_selector_be;
    // Vars
    QByteArray data;
    int endianness;

    void InterpretData();

  private slots:
    void SlotEndiannessSelectorLeClicked(bool checked);
    void SlotEndiannessSelectorBeClicked(bool checked);

};

#endif // DATAINTERPRETERWIDGET_H
