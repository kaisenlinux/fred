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

#include "registrykeytablemodel.h"

/*******************************************************************************
 * Public
 ******************************************************************************/

RegistryKeyTableModel::RegistryKeyTableModel(RegistryHive *p_hive,
                                             QString node_path,
                                             QObject *p_parent)
  : QAbstractTableModel(p_parent)
{
  // Create the "root" key. It's values will be used as header values.
  this->p_keys=new RegistryKey(QList<QVariant>()<<
                               tr("Key")<<
                               tr("Type")<<
                               tr("Value"));
  // Build key list
  this->SetupModelData(p_hive,node_path);
}

RegistryKeyTableModel::~RegistryKeyTableModel() {
  delete this->p_keys;
}

QVariant RegistryKeyTableModel::data(const QModelIndex &index, int role) const {
  bool ok;

  if(!index.isValid()) return QVariant();

  RegistryKey *p_key=static_cast<RegistryKey*>(index.internalPointer());

  switch(role) {
    case Qt::DisplayRole: {
      switch(index.column()) {
        case RegistryKeyTableModel::ColumnContent_KeyName: {
          return p_key->Data(index.column());
          break;
        }
        case RegistryKeyTableModel::ColumnContent_KeyType: {
          int value_type=p_key->Data(index.column()).toInt(&ok);
          if(!ok) return QVariant();
          return RegistryHive::KeyValueTypeToString(value_type);
          break;
        }
        case RegistryKeyTableModel::ColumnContent_KeyValue: {
          // Get index to value type
          QModelIndex type_index=this->index(index.row(),
                                             RegistryKeyTableModel::
                                               ColumnContent_KeyType);
          // Get value type
          int value_type=this->data(type_index,
                                    RegistryKeyTableModel::
                                      AdditionalRoles_GetRawData).toInt(&ok);
          if(!ok) return QVariant();
          // Return value converted to human readeable string
          QByteArray value_array=p_key->Data(index.column()).toByteArray();
          return RegistryHive::KeyValueToString(value_array,value_type);
          break;
        }
        default:
          return QVariant();
      }
      break;
    }
    case RegistryKeyTableModel::AdditionalRoles_GetRawData: {
      return p_key->Data(index.column());
      break;
    }
    default:
      return QVariant();
  }
  return QVariant();
}

Qt::ItemFlags RegistryKeyTableModel::flags(const QModelIndex &index) const {
  if(!index.isValid()) return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant RegistryKeyTableModel::headerData(int section,
                                           Qt::Orientation orientation,
                                           int role) const
{
  // Only horizontal header is supported
  if(orientation!=Qt::Horizontal) return QVariant();

  switch(role) {
    case Qt::TextAlignmentRole:
      // Handle text alignment
      if(section==2) return Qt::AlignLeft;
      else return Qt::AlignCenter;
      break;
    case Qt::DisplayRole:
      // Header text
      return this->p_keys->Data(section);
      break;
    default:
      return QVariant();
  }
}

QModelIndex RegistryKeyTableModel::index(int row,
                                         int column,
                                         const QModelIndex &parent) const
{
  if(!this->hasIndex(row,column,parent)) return QModelIndex();

  RegistryKey *p_key=this->p_keys->Key(row);

  return this->createIndex(row,column,p_key);
}

int RegistryKeyTableModel::rowCount(const QModelIndex &parent) const {
  // According to Qt doc, when parent in TableModel is valid, we should return 0
  if(parent.isValid()) return 0;
  // Get and return row count from the keys list
  return this->p_keys->RowCount();
}

int RegistryKeyTableModel::columnCount(const QModelIndex &parent) const {
  // According to Qt doc, when parent in TableModel is valid, we should return 0
  if(parent.isValid()) return 0;
  // There are always 3 columns
  return 3;
}

int RegistryKeyTableModel::GetKeyRow(QString key_name) const {
  int i;

  for(i=0;i<this->p_keys->RowCount();i++) {
    if(this->p_keys->Key(i)->Data(0)==key_name) {
      return i;
    }
  }

  // When key isn't found, return the first row
  return 0;
}

QModelIndex RegistryKeyTableModel::AddKey(RegistryHive *p_hive,
                                          int new_key_id)
{
  RegistryKey *p_key;
  QString key_name;
  QByteArray key_value;
  int key_value_type;
  size_t key_value_len;

  // Tell users of this model that we are going to add a row
  emit(RegistryKeyTableModel::beginInsertRows(QModelIndex(),
                                              this->p_keys->RowCount(),
                                              this->p_keys->RowCount()));

  // Get key name
  if(!p_hive->GetKeyName(new_key_id,key_name)) {
    return QModelIndex();
  }
  // Get key value, value type and value length
  key_value=p_hive->GetKeyValue(new_key_id,&key_value_type,&key_value_len);
  if(p_hive->GetErrorMsg()!="") {
    return QModelIndex();
  }
  // Create new RegistryKey object and add it to our internal list
  p_key=new RegistryKey(QList<QVariant>()<<
                          QString(key_name.length() ? key_name : "(default)")<<
                          QVariant(key_value_type)<<
                          key_value);
  this->p_keys->Append(p_key);

  // Tell users of this model we have finished adding a row
  emit(RegistryKeyTableModel::endInsertRows());

  // Return an index to the new row
  return this->index(this->p_keys->RowCount()-1,0);
}

QModelIndex RegistryKeyTableModel::UpdateKey(RegistryHive *p_hive,
                                             int new_key_id)
{
  QString key_name;
  QByteArray key_value;
  int key_value_type;
  size_t key_value_len;
  int key_row=-1;

  // Get key name
  if(!p_hive->GetKeyName(new_key_id,key_name)) {
    return QModelIndex();
  }
  // Get key value, value type and value length
  key_value=p_hive->GetKeyValue(new_key_id,&key_value_type,&key_value_len);
  if(p_hive->GetErrorMsg()!="") {
    return QModelIndex();
  }

  // Find row containig the key to update
  for(int i=0;i<this->p_keys->RowCount();i++) {
    if(this->p_keys->Key(i)->Data(0).toString().toLower()==key_name.toLower()) {
      key_row=i;
    }
  }
  if(key_row==-1) return QModelIndex();

  // Update values
  this->p_keys->Key(key_row)->SetData(QList<QVariant>()<<
                                        QString(key_name.length() ?
                                                  key_name : "(default)")<<
                                        QVariant(key_value_type)<<
                                        key_value);

  // Tell users of this model that data has changed
  emit(RegistryKeyTableModel::dataChanged(this->index(key_row,0),
                                          this->index(key_row,0)));

  return this->index(key_row,0);
}

QModelIndex RegistryKeyTableModel::RemoveKey(const QModelIndex &index) {
  // Tell users of this model that a row is going to be removed
  emit(RegistryKeyTableModel::beginRemoveRows(QModelIndex(),
                                              index.row(),
                                              index.row()));
  // Remove row
  this->p_keys->Remove(index.row());
  // Tell users of this model that a row has been removed
  emit(RegistryKeyTableModel::endRemoveRows());
  // Return a valid index to be selected
  if(this->rowCount()==0) return QModelIndex();
  else if(index.row()==0) return this->index(0,0);
  else if(index.row()<this->rowCount()) return this->index(index.row(),0);
  else return this->index(index.row()-1,0);
}

/*******************************************************************************
 * Private
 ******************************************************************************/

void RegistryKeyTableModel::SetupModelData(RegistryHive *p_hive,
                                           QString &node_path)
{
  QMap<QString,int> node_keys;
  RegistryKey *p_key;
  QByteArray key_value;
  int key_value_type;
  size_t key_value_len;

  // Get all keys for current node
  node_keys=p_hive->GetKeys(node_path);
  if(node_keys.isEmpty()) return;

  // Add all keys to list
  QMapIterator<QString,int> i(node_keys);
  while(i.hasNext()) {
    i.next();
    key_value=p_hive->GetKeyValue(i.value(),
                                  &key_value_type,
                                  &key_value_len);
    if(p_hive->GetErrorMsg()!="") continue;
    p_key=new RegistryKey(QList<QVariant>()<<
                            QString(i.key().length() ? i.key() : tr("(default)"))<<
                            QVariant(key_value_type)<<
                            key_value);
    this->p_keys->Append(p_key);
  }
}
