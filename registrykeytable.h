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

#ifndef REGISTRYKEYTABLE_H
#define REGISTRYKEYTABLE_H

#include <QTableView>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>

class RegistryKeyTable : public QTableView {
  Q_OBJECT

  public:
    RegistryKeyTable(QWidget *p_parent=0);
    ~RegistryKeyTable();

    void setModel(QAbstractItemModel *p_model, bool writable=false);
    void SetWritable(bool writable);
    //void selectRow(QString key_name);

  Q_SIGNALS:
    void CurrentItemChanged(QModelIndex current);
    void SignalAddKey();
    void SignalEditKey(QModelIndex node);
    void SignalDeleteKey(QModelIndex node);

  protected:
    int sizeHintForColumn(int column) const;
    void contextMenuEvent(QContextMenuEvent *p_event);
    void currentChanged(const QModelIndex &current,
                        const QModelIndex &previous);

  private:
    bool is_writable;
    QAction *p_action_add_key;
    QAction *p_action_edit_key;
    QAction *p_action_delete_key;
    QMenu *p_menu_copy;
    QAction *p_action_copy_key_name;
    QAction *p_action_copy_key_value;

  private slots:
    void SlotAddKey();
    void SlotEditKey();
    void SlotDeleteKey();
    void SlotCopyKeyName();
    void SlotCopyKeyValue();
};

#endif // REGISTRYKEYTABLE_H
