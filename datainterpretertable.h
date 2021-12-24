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

#ifndef DATAINTERPRETERTABLE_H
#define DATAINTERPRETERTABLE_H

#include <QWidget>
#include <QTableWidget>
#include <QContextMenuEvent>
#include <QAction>

class DataInterpreterTable : public QTableWidget {
  Q_OBJECT

  public:
    DataInterpreterTable(QWidget *p_parent=0);
    ~DataInterpreterTable();

    /*
     * AddValue
     *
     * Add a value pair (name,value) to data interprter.
     */
    void AddValue(QString name, QString value);
    /*
     * ClearValues
     *
     * Remove all value pairs from table
     */
    void ClearValues();

  protected:
    /*
     * sizeHintForColumn
     *
     * Needed reimplementation in order to allow resizeColumnsToContent
     * to resize hidden columns too.
     */
    int sizeHintForColumn(int column) const;
    void contextMenuEvent(QContextMenuEvent *p_event);

  private:
    QAction *p_action_copy_value;

  private slots:
    void SlotCopyValue();


};

#endif // DATAINTERPRETERTABLE_H
