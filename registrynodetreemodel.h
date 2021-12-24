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

#ifndef REGISTRYNODETREEMODEL_H
#define REGISTRYNODETREEMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QString>

#include "registrynode.h"
#include "registryhive.h"

class RegistryNodeTreeModel : public QAbstractItemModel {
  Q_OBJECT

  public:
    enum ColumnContent {
      ColumnContent_NodeName=0,
      ColumnContent_NodeModTime
    };

    RegistryNodeTreeModel(RegistryHive *p_hive, QObject *p_parent=0);
    ~RegistryNodeTreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role=Qt::DisplayRole) const;
    QModelIndex index(int row,
                      int column,
                      const QModelIndex &parent=QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;

    QList<QModelIndex> GetIndexListOf(QString path);
    QString GetNodePath(QModelIndex child_index);
    QModelIndex AddNode(RegistryHive *p_hive, QModelIndex parent_index,
                        int new_node_id,
                        QString new_node_name);
    QModelIndex RemoveNode(QModelIndex index);

  private:
    RegistryNode *p_root_node;

    void SetupModelData(RegistryHive *p_hive,
                        RegistryNode *p_parent,
                        int hive_node=0);
    QModelIndex GetNodeNameIndex(QModelIndex index);
};

#endif // REGISTRYNODETREEMODEL_H
