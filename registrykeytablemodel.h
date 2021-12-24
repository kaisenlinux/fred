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

#ifndef REGISTRYKEYTABLEMODEL_H
#define REGISTRYKEYTABLEMODEL_H

#include <QAbstractTableModel>

#include "registrykey.h"
#include "registryhive.h"

class RegistryKeyTableModel : public QAbstractTableModel {
  Q_OBJECT

  public:
    enum ColumnContent {
      ColumnContent_KeyName=0,
      ColumnContent_KeyType,
      ColumnContent_KeyValue
    };

    enum AdditionalRoles {
      AdditionalRoles_GetRawData=Qt::UserRole
    };

    RegistryKeyTableModel(RegistryHive *p_hive,
                          QString node_path,
                          QObject *p_parent=0);
    ~RegistryKeyTableModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role=Qt::DisplayRole) const;
    QModelIndex index(int row,
                      int column,
                      const QModelIndex &parent=QModelIndex()) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;

    int GetKeyRow(QString key_name) const;
    QModelIndex AddKey(RegistryHive *p_hive, int new_key_id);
    QModelIndex UpdateKey(RegistryHive *p_hive, int new_key_id);
    QModelIndex RemoveKey(const QModelIndex &index);

  private:
    RegistryKey *p_keys;

    void SetupModelData(RegistryHive *p_hive, QString &node_path);
};

#endif // REGISTRYKEYTABLEMODEL_H
