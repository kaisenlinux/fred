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

#ifndef REGISTRYHIVE_H
#define REGISTRYHIVE_H

#include <QObject>
#include <QMap>

#ifndef HIVEX_STATIC
  #include <hivex.h>
#else
  #include "hivex/lib/hivex.h"
#endif

class RegistryHive : public QObject {
  Q_OBJECT

  public:
    typedef enum eHiveType {
      eHiveType_UNKNOWN=0,
      eHiveType_SYSTEM,
      eHiveType_SOFTWARE,
      eHiveType_SAM,
      eHiveType_SECURITY,
      eHiveType_NTUSER
    } teHiveType;

    explicit RegistryHive(QObject *p_parent=0);
    ~RegistryHive();

    bool Error();
    QString GetErrorMsg();

    bool Open(QString file, bool read_only=true);
    bool Reopen(bool read_only=true);
    bool CommitChanges();
    bool Close();

    QString Filename();
    teHiveType HiveType();
    QString HiveTypeToString(teHiveType hive_type);
    bool HasChangesToCommit();

    QMap<QString,int> GetNodes(QString path="\\");
    QMap<QString,int> GetNodes(int parent_node=0);
    QMap<QString,int> GetKeys(QString path="\\");
    QMap<QString,int> GetKeys(int parent_node=0);
    bool GetKeyName(int hive_key, QString &key_name);
    QByteArray GetKeyValue(QString path,
                           QString key,
                           int *p_value_type,
                           size_t *p_value_len);
    QByteArray GetKeyValue(int hive_key,
                           int *p_value_type,
                           size_t *p_value_len);
    qint64 GetNodeModTime(QString path);
    qint64 GetNodeModTime(int node);
    static QString KeyValueToString(QByteArray value, int value_type);
    static QString KeyValueToString(QByteArray value,
                                    QString format,
                                    int offset=0,
                                    int length=-1,
                                    bool little_endian=true);
    static QStringList KeyValueToStringList(QByteArray value,
                                            bool little_endian=true,
                                            bool *p_ansi_encoded=NULL);
    static QByteArray StringListToKeyValue(QStringList strings,
                                           bool little_endian=true,
                                           bool ansi_encoded=false);
    static QStringList GetKeyValueTypes();
    static QString KeyValueTypeToString(int value_type);
    static int StringToKeyValueType(QString value_type);
    static quint64 FiletimeToUnixtime(qint64 filetime);

    int AddNode(QString parent_node_path, QString node_name);
    bool DeleteNode(QString node_path);

    int AddKey(QString parent_node_path,
               QString key_name,
               QString key_value_type,
               QByteArray key_value);
    int UpdateKey(QString parent_node_path,
                  QString key_name,
                  QString key_value_type,
                  QByteArray key_value);
    bool DeleteKey(QString parent_node_path, QString key_name);

  private:
    QString erro_msg;
    bool is_error;
    QString hive_file;
    hive_h *p_hive;
    bool is_hive_open;
    bool is_hive_writable;
    bool has_changes_to_commit;

    QString HivexError2String(int error);
    void SetError(QString msg);
    bool GetNodeHandle(QString &path, hive_node_h *p_node);
    bool GetKeyHandle(QString &parent_node_path,
                      QString &key_name,
                      hive_value_h *p_key);
    QMap<QString,int> GetNodesHelper(hive_node_h parent_node);
    QMap<QString,int> GetKeysHelper(hive_node_h parent_node);
    QByteArray GetKeyValueHelper(hive_value_h hive_key,
                                 int *p_value_type,
                                 size_t *p_value_len);
    bool PathExists(QString path);
    int SetKey(QString &parent_node_path,
               QString &key_name,
               QString &key_value_type,
               QByteArray &key_value,
               bool create_key);
    static int FindUnicodeStringEnd(QByteArray data, int offset=0);

};

#endif // REGISTRYHIVE_H
