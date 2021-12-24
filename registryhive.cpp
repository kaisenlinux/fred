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

#include <QStringList>
#include <QDateTime>
#include <QtEndian>

#include <QDebug>

#include <stdlib.h>
#include <stdio.h>

#include "registryhive.h"

// TODO: __WORDSIZE is not defined under mingw and I currently have no idea how
// to identify a 64bit windows
#ifndef __WORDSIZE
  #define __WORDSIZE 32
#endif

#if __WORDSIZE == 64
  #define EPOCH_DIFF 0x19DB1DED53E8000
#else
  #define EPOCH_DIFF 0x19DB1DED53E8000LL
#endif

// Macros to ease UTF16 endianness conversions
#undef UTF16LETOH
#define UTF16LETOH(buf,buf_len) {                                                 \
  for(int buf_off=0;buf_off<((buf_len)-1);buf_off+=2) {                           \
    *((quint16*)((buf)+buf_off))=qFromLittleEndian(*((quint16*)((buf)+buf_off))); \
  }                                                                               \
}
#undef UTF16BETOH
#define UTF16BETOH(buf,buf_len) {                                              \
  for(int buf_off=0;buf_off<((buf_len)-1);buf_off+=2) {                        \
    *((quint16*)((buf)+buf_off))=qFromBigEndian(*((quint16*)((buf)+buf_off))); \
  }                                                                            \
}
#undef HTOUTF16LE
#define HTOUTF16LE(buf,buf_len) {                                               \
  for(int buf_off=0;buf_off<((buf_len)-1);buf_off+=2) {                         \
    *((quint16*)((buf)+buf_off))=qToLittleEndian(*((quint16*)((buf)+buf_off))); \
  }                                                                             \
}
#undef HTOUTF16BE
#define HTOUTF16BE(buf,buf_len) {                                            \
  for(int buf_off=0;buf_off<((buf_len)-1);buf_off+=2) {                      \
    *((quint16*)((buf)+buf_off))=qToBigEndian(*((quint16*)((buf)+buf_off))); \
  }                                                                          \
}

// Some errno numbers that hivex uses are not defined under Windows
#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
  // These are the same values as defined by MSVC 10, for interoperability.
  #ifndef ENOTSUP
    #define ENOTSUP 129
  #endif
  #ifndef ELOOP
    #define ELOOP 114
  #endif
#endif

/*******************************************************************************
 * Public
 ******************************************************************************/

/*
 * RegistryHive
 */
RegistryHive::RegistryHive(QObject *p_parent) : QObject(p_parent) {
  this->erro_msg="";
  this->is_error=false;
  this->hive_file="";
  this->p_hive=NULL;
  this->is_hive_open=false;
  this->is_hive_writable=false;
  this->has_changes_to_commit=false;
}

/*
 * ~RegistryHive
 */
RegistryHive::~RegistryHive() {
  if(this->is_hive_open) this->Close();
}

/*
 * Error
 */
bool RegistryHive::Error() {
  return this->is_error;
}

/*
 * GetErrorMsg
 */
QString RegistryHive::GetErrorMsg() {
  QString msg=this->erro_msg;
  this->erro_msg="";
  this->is_error=false;
  return msg;
}

/*
 * Open
 */
bool RegistryHive::Open(QString file, bool read_only) {
  if(this->is_hive_open) return false;

  // Open hive file
  this->p_hive=hivex_open(file.toLocal8Bit().constData(),
                          read_only ? 0 : HIVEX_OPEN_WRITE);
  if(this->p_hive==NULL) return false;

  // Set local vars
  this->hive_file=file;
  this->is_hive_open=true;
  this->is_hive_writable=!read_only;
  this->has_changes_to_commit=false;

  return true;
}

/*
 * Reopen
 */
bool RegistryHive::Reopen(bool read_only) {
  if(!this->is_hive_open) return false;

  // Close hive first
  if(hivex_close(this->p_hive)!=0) {
    // According to the docs, even if hivex_close fails, it frees all handles.
    // So we consider this fatal and final!
    this->hive_file="";
    this->is_hive_open=false;
    this->is_hive_writable=false;
    this->has_changes_to_commit=false;
    return false;
  }

  // Reopen same hive
  this->p_hive=hivex_open(this->hive_file.toLocal8Bit().constData(),
                          read_only ? 0 : HIVEX_OPEN_WRITE);
  if(this->p_hive==NULL) {
    this->hive_file="";
    this->is_hive_open=false;
    this->is_hive_writable=false;
    this->has_changes_to_commit=false;
    return false;
  }

  // Update local vars
  this->is_hive_writable=!read_only;
  this->has_changes_to_commit=false;

  return true;
}

/*
 * CommitChanges
 */
bool RegistryHive::CommitChanges() {
  if(!this->is_hive_open || !this->is_hive_writable) return false;
  if(!this->has_changes_to_commit) return true;

  // TODO: Maybe it would be more secure to commit changes to a new file and
  // then move it over the original one.
  if(hivex_commit(this->p_hive,NULL,0)!=0) {
    return false;
  }

  this->has_changes_to_commit=false;
  return true;
}

/*
 * Close
 */
bool RegistryHive::Close() {
  if(this->is_hive_open) {
    // As hivex_close will _ALWAYS_ free the handle, we don't need the following
    // values anymore
    this->hive_file="";
    this->is_hive_open=false;
    this->is_hive_writable=false;
    this->has_changes_to_commit=false;

    // Close hive
    if(hivex_close(this->p_hive)!=0) return false;
  }
  return true;
}

/*
 * Filename
 */
QString RegistryHive::Filename() {
  if(this->is_hive_open) return this->hive_file;
  return QString();
}

/*
 * HiveType
 */
RegistryHive::teHiveType RegistryHive::HiveType() {
  // Check for SYSTEM hive
  if(this->PathExists("\\Select") && this->PathExists("\\MountedDevices"))
    return RegistryHive::eHiveType_SYSTEM;
  // Check for SOFTWARE hive
  if(this->PathExists("\\Microsoft\\Windows\\CurrentVersion") &&
     this->PathExists("\\Microsoft\\Windows NT\\CurrentVersion"))
    return RegistryHive::eHiveType_SOFTWARE;
  // Check for SAM
  if(this->PathExists("SAM\\Domains\\Account\\Users"))
    return RegistryHive::eHiveType_SAM;
  // Check for SECURITY
  if(this->PathExists("\\Policy\\Accounts") &&
     this->PathExists("\\Policy\\PolAdtEv"))
    return RegistryHive::eHiveType_SECURITY;
  // Check for NTUSER.DAT
  if(this->PathExists("\\Software\\Microsoft\\Windows\\CurrentVersion"))
    return RegistryHive::eHiveType_NTUSER;
  // Unknown hive
  return RegistryHive::eHiveType_UNKNOWN;
}

/*
 * HiveTypeToString
 */
QString RegistryHive::HiveTypeToString(teHiveType hive_type) {
  switch(hive_type) {
    case RegistryHive::eHiveType_SYSTEM:
      return "SYSTEM";
      break;
    case RegistryHive::eHiveType_SOFTWARE:
      return "SOFTWARE";
      break;
    case RegistryHive::eHiveType_SAM:
      return "SAM";
      break;
    case RegistryHive::eHiveType_SECURITY:
      return "SECURITY";
      break;
    case RegistryHive::eHiveType_NTUSER:
      return "NTUSER";
      break;
    default:
      return "UNKNOWN";
  }
}

/*
 * HasChangesToCommit
 */
bool RegistryHive::HasChangesToCommit() {
  return this->has_changes_to_commit;
}

/*
 * GetNodes
 */
QMap<QString,int> RegistryHive::GetNodes(QString path) {
  hive_node_h parent_node;

  // Get handle to last node in path
  if(!this->GetNodeHandle(path,&parent_node)) return QMap<QString,int>();

  // Get and return nodes
  return this->GetNodesHelper(parent_node);
}

/*
 * GetNodes
 */
QMap<QString,int> RegistryHive::GetNodes(int parent_node) {
  if(parent_node==0) {
    this->SetError(tr("Invalid parent node handle specified!"));
    return QMap<QString,int>();
  }

  // Get and return nodes
  return this->GetNodesHelper(parent_node);
}

/*
 * GetKeys
 */
QMap<QString,int> RegistryHive::GetKeys(QString path) {
  hive_node_h parent_node;

  // Get handle to last node in path
  if(!this->GetNodeHandle(path,&parent_node)) return QMap<QString,int>();

  // Get and return keys
  return this->GetKeysHelper(parent_node);
}

/*
 * GetKeys
 */
QMap<QString,int> RegistryHive::GetKeys(int parent_node) {
  if(parent_node==0) {
    this->SetError(tr("Invalid parent node handle specified!"));
    return QMap<QString,int>();
  }

  // Get and return keys
  return this->GetKeysHelper(parent_node);
}

/*
 * GetKeyName
 */
bool RegistryHive::GetKeyName(int hive_key, QString &key_name) {
  char *buf;

  if(!this->is_hive_open) {
    this->SetError(tr("Need to operate on an open hive!"));
    return false;
  }

  buf=hivex_value_key(this->p_hive,(hive_value_h)hive_key);
  if(buf==NULL) {
    this->SetError(tr("Unable to get key name for key '%1'").arg(hive_key));
    return false;
  }

  key_name=QString(buf);
  free(buf);

  return true;
}

/*
 * GetKeyValue
 */
QByteArray RegistryHive::GetKeyValue(QString path,
                                     QString key,
                                     int *p_value_type,
                                     size_t *p_value_len)
{
  hive_node_h parent_node;
  hive_value_h hive_key;

  // Get handle to last node in path
  if(!this->GetNodeHandle(path,&parent_node)) return QByteArray();

  // Get key handle
  hive_key=hivex_node_get_value(this->p_hive,
                                parent_node,key.toLatin1().constData());
  if(hive_key==0) {
    this->SetError(tr("Unable to get key handle!"));
    *p_value_len=-1;
    return QByteArray();
  }

  // Get and return key value
  return this->GetKeyValueHelper(hive_key,p_value_type,p_value_len);
}

/*
 * GetKeyValue
 */
QByteArray RegistryHive::GetKeyValue(int hive_key,
                                     int *p_value_type,
                                     size_t *p_value_len)
{
  if(hive_key==0) {
    this->SetError(tr("Invalid key handle specified!"));
    *p_value_type=-1;
    return QByteArray();
  }

  // Get and return key value
  return this->GetKeyValueHelper(hive_key,p_value_type,p_value_len);
}

/*
 * GetKeyModTime
 */
qint64 RegistryHive::GetNodeModTime(QString path) {
  hive_node_h node;

  // Get handle to last node in path
  if(!this->GetNodeHandle(path,&node)) {
    this->SetError(tr("Unable to get node handle!"));
    return 0;
  }

  // Get and return node's last modification timestamp
  return this->GetNodeModTime(node);
}

/*
 * GetKeyModTime
 */
qint64 RegistryHive::GetNodeModTime(int node) {
  if(node==0) {
    this->SetError(tr("Invalid node handle specified!"));
    return 0;
  }

  // Get and return key's last modification timestamp
  return hivex_node_timestamp(this->p_hive,node);
}

/*
 * KeyValueToString
 */
QString RegistryHive::KeyValueToString(QByteArray value, int value_type) {
  QString ret="";

  #define ToHexStr() {                                              \
    for(int i=0;i<value.size();i++) {                               \
      ret.append(QString("%1 ").arg((quint8)(value.constData()[i]), \
                                    2,16,QChar('0')));              \
    }                                                               \
    ret.chop(1);                                                    \
  }

  // Convert value according to it's type
  switch(value_type) {
    case hive_t_REG_SZ:
    case hive_t_REG_EXPAND_SZ:
      // A Windows string (REG_SZ), or a Windows string containing %env%
      // (environment variable expansion) elements (REG_EXPAND_SZ).
      // Encoding is unknown, but often UTF16-LE (isn't this great?)
      // Try to detect ANSI vs UNICODE encoding
      if(value.size()==0) {
        ret=QString();
      } else if(value.size()>=2 && value.endsWith(QByteArray("\x00\x00",2))) {
        // Seems to be a unicode string, convert to host endianness and return
        // TODO: What if it is UTF16-BE?? Thx Billy!
        QByteArray buf=value;
        UTF16LETOH(buf.data(),buf.size());
        ret=QString().fromUtf16((ushort*)(buf.constData())).toHtmlEscaped();
      } else if(value.endsWith(QByteArray("\x00",1))) {
        // Seems to be an ansi string
        ret=QString().fromLatin1((char*)value.constData()).toHtmlEscaped();
      } else {
        // If we can't detect encoding, return string as hex
        ToHexStr();
      }
      break;
    case hive_t_REG_MULTI_SZ:
      // Multiple Windows strings.
      // I suppose this is always LE encoded! M$ devs really suck!
      ret=RegistryHive::KeyValueToStringList(value).join("\n");
      break;
    case hive_t_REG_DWORD:
      // DWORD (32 bit integer), little endian
      ret=QString("0x%1")
            .arg(qFromLittleEndian(*(quint32*)value.constData()),
                 8,16,QChar('0'));
      break;
    case hive_t_REG_DWORD_BIG_ENDIAN:
      // DWORD (32 bit integer), big endian
      ret=QString("0x%1")
            .arg(qFromBigEndian(*(quint32*)value.constData()),
                 8,16,QChar('0'));
      break;
    case hive_t_REG_QWORD:
      // QWORD (64 bit integer). Usually little endian (grrrr).
      ret=QString("0x%1")
            .arg(qFromLittleEndian(*(quint64*)value.constData()),
                 16,16,QChar('0'));
      break;
    case hive_t_REG_NONE:
    case hive_t_REG_BINARY:
    case hive_t_REG_LINK:
    case hive_t_REG_RESOURCE_LIST:
    case hive_t_REG_FULL_RESOURCE_DESCRIPTOR:
    case hive_t_REG_RESOURCE_REQUIREMENTS_LIST:
    default:
      // A key without a value (REG_NONE), a blob of binary (REG_BINARY), a
      // symbolic link to another part of the registry tree (REG_LINK), a
      // resource list (REG_RESOURCE_LIST), a resource descriptor
      // (FULL_RESOURCE_DESCRIPTOR), a resource requirements list
      // (REG_RESOURCE_REQUIREMENTS_LIST) or something unknown.
      // All these are converted to hex.
      ToHexStr();
  }

  #undef ToHexStr

  return ret;
}

/*
 * KeyValueToString
 */
QString RegistryHive::KeyValueToString(QByteArray key_value,
                                       QString format,
                                       int offset,
                                       int length,
                                       bool little_endian)
{
  int remaining_data_len;
  const char *p_data;
  QString ret="";

  // Calculate how many bytes are remainig after specified offset
  remaining_data_len=key_value.size()-offset;
  if(!(remaining_data_len>0)) {
    // Nothing to show
    return QString();
  }

  // Get pointer to data at specified offset
  p_data=key_value.constData();
  p_data+=offset;

  // Convert value
  if(format=="int8" && remaining_data_len>=1) {
    ret=QString("%1").arg(*(qint8*)p_data);
  } else if(format=="uint8" && remaining_data_len>=1) {
    ret=QString("%1").arg(*(quint8*)p_data);
  } else if(format=="int16" && remaining_data_len>=2) {
    qint16 val;
    if(little_endian) val=qFromLittleEndian(*(qint16*)p_data);
    else val=qFromBigEndian(*(qint16*)p_data);
    ret=QString("%1").arg(val);
  } else if(format=="uint16" && remaining_data_len>=2) {
    quint16 val;
    if(little_endian) val=qFromLittleEndian(*(quint16*)p_data);
    else val=qFromBigEndian(*(quint16*)p_data);
    ret=QString("%1").arg(val);
  } else if(format=="int32" && remaining_data_len>=4) {
    qint32 val;
    if(little_endian) val=qFromLittleEndian(*(qint32*)p_data);
    else val=qFromBigEndian(*(qint32*)p_data);
    ret=QString("%1").arg(val);
  } else if(format=="uint32" && remaining_data_len>=4) {
    quint32 val;
    if(little_endian) val=qFromLittleEndian(*(quint32*)p_data);
    else val=qFromBigEndian(*(quint32*)p_data);
    ret=QString("%1").arg(val);
  } else if(format=="unixtime" && remaining_data_len>=4) {
    quint32 val;
    if(little_endian) val=qFromLittleEndian(*(quint32*)p_data);
    else val=qFromBigEndian(*(quint32*)p_data);
    if(val==0) {
      ret="n/a";
    } else {
      QDateTime date_time;
      date_time.setTimeSpec(Qt::UTC);
      date_time.setTime_t(val);
      ret=date_time.toString("yyyy/MM/dd hh:mm:ss");
    }
  } else if(format=="int64" && remaining_data_len>=8) {
    qint64 val;
    if(little_endian) val=qFromLittleEndian(*(qint64*)p_data);
    else val=qFromBigEndian(*(qint64*)p_data);
    ret=QString("%1").arg(val);
  } else if(format=="uint64" && remaining_data_len>=8) {
    quint64 val;
    if(little_endian) val=qFromLittleEndian(*(quint64*)p_data);
    else val=qFromBigEndian(*(quint64*)p_data);
    ret=QString("%1").arg(val);
/*
  // TODO: Check how one could implement this
  } else if(format=="unixtime64" && remaining_data_len>=8) {
    if(*(quint64*)p_data==0) {
      ret="n/a";
    } else {
      quint64 secs=*(quint64*)p_data;
      QDateTime date_time;
      date_time.setTimeSpec(Qt::UTC);
      // Set 32bit part of date/time
      date_time.setTime_t(secs&0xFFFFFFFF);
      // Now add high 32bit part of date/time
      date_time.addSecs(secs>>32);
      ret=date_time.toString("yyyy/MM/dd hh:mm:ss");
    }
*/
  } else if(format=="filetime" && remaining_data_len>=8) {
    quint64 val;
    if(little_endian) val=qFromLittleEndian(*(quint64*)p_data);
    else val=qFromBigEndian(*(quint64*)p_data);
    if(val==0) {
      ret="n/a";
    } else {
      // TODO: Warn if >32bit
      QDateTime date_time;
      date_time.setTimeSpec(Qt::UTC);
      date_time.setTime_t(RegistryHive::FiletimeToUnixtime(val));
      ret=date_time.toString("yyyy/MM/dd hh:mm:ss");
    }
  } else if(format=="ascii") {
    if(length!=-1) {
      // User specified how many bytes to convert
      ret=QString().fromLatin1((char*)p_data,length).toHtmlEscaped();
    } else {
      // User did not specify how many bytes to convert, make sure data is 0
      // terminated
      if(key_value.indexOf("\x00",offset)!=-1) {
        // Data is 0 terminated
        ret=QString().fromLatin1((char*)p_data).toHtmlEscaped();
      } else {
        // Data is not 0 terminated, convert all remaining_data_len bytes
        ret=QString().fromLatin1((char*)p_data,remaining_data_len).toHtmlEscaped();
      }
    }
  } else if(format=="utf16" && remaining_data_len>=2) {
    QByteArray buf;
    if(length!=-1) {
      // User specified how many bytes to convert
      buf=key_value.mid(offset,(length%2)==0 ? length : length-1);
      buf.append("\x00\x00",2);
    } else {
      // User did not specify how many bytes to convert, make sure data is
      // double 0 terminated
      int null_offset=RegistryHive::FindUnicodeStringEnd(key_value.mid(offset));
      if(null_offset!=-1) {
        // Data is double 0 terminated
        buf=key_value.mid(offset,null_offset+2);
      } else {
        // Data is not double 0 terminated, convert all remaining_data_len bytes
        buf=key_value.mid(offset,
                          (remaining_data_len%2)==0 ?
                            remaining_data_len : remaining_data_len-1);
        buf.append("\x00\x00",2);
      }
    }
    // Convert from requested endianness to host
    if(little_endian) {
      UTF16LETOH(buf.data(),buf.size());
    } else {
      UTF16BETOH(buf.data(),buf.size());
    }
    ret=QString().fromUtf16((ushort*)buf.constData()).toHtmlEscaped();
  } else {
    // Unknown variant type or another error
    // TODO: Maybe return an error
    return QString();
  }

  return ret;
}

/*
 * KeyValueToStringList
 *
 * Should only be used for REG_MULTI_SZ values
 */
QStringList RegistryHive::KeyValueToStringList(QByteArray value,
                                               bool little_endian,
                                               bool *p_ansi_encoded)
{
  // Try to find value encoding (ANSI vs UNICODE)
  bool is_ansi;
  if(value.size()<=2) {
    // http://blogs.msdn.com/b/oldnewthing/archive/2009/10/08/9904646.aspx
    // Ansi version of a REG_MULTI_SZ needs to be terminated by 2 \0 chars.
    // So as long as the byte array has less or equal to 2 chars, it must be
    // empty.
    return QStringList();
  } else if(value.size()==3) {
    // Only 3 chars, this can only be an ansi string consisting of 1 char and 2
    // \0 to terminate it
    return QStringList()
      <<QString(QChar((char)*((quint8*)(value.constData())))).toHtmlEscaped();
  } else if(value.size()==4) {
    if((quint32)*((quint32*)(value.constData()))==0) {
      // http://blogs.msdn.com/b/oldnewthing/archive/2009/10/08/9904646.aspx
      // Unicode version of a REG_MULTI_SZ needs to be terminated by 4 \0 chars.
      // So as long as the byte array has less or equal to 4 chars, and they are
      // all 0 it must be empty.
      return QStringList();
    } else {
      // Must be the ansi version of REG_MULTI_SZ
      is_ansi=true;
    }
  } else if((quint32)*((quint32*)(value.right(4).constData()))==0) {
    // Value ends with 4 \0 chars, it must be unicode
    is_ansi=false;
  } else if((quint32)*((quint32*)(value.right(3).constData()))==0) {
    // Value ends with 3 \0 chars. Not possible according to the specs, but
    // already seen in values M$ is storing! Those were unicode.
    is_ansi=false;
  } else if((quint16)*((quint16*)(value.right(2).constData()))==0) {
    // Value only ends with 2 \0 chars, it must be ansi
    is_ansi=true;
  } else {
    // Value has more than 4 chars but does not end in 2 or 4 \0 chars. This
    // is not according to specs!
    return QStringList();
  }

  // Convert value to string list
  QStringList result=QStringList();
  QByteArray buf;
  int last_pos=0,cur_pos=0;
  if(!is_ansi) {
    // Extract unicode strings
    while(last_pos<value.size() &&
          (cur_pos=RegistryHive::FindUnicodeStringEnd(value,last_pos))!=-1)
    {
      if(cur_pos==last_pos) break;
      buf=value.mid(last_pos,(cur_pos-last_pos)+2);
      if(little_endian) {
        // Convert from LE to host
        UTF16LETOH(buf.data(),buf.size());
      } else {
        // Convert from BE to host
        UTF16BETOH(buf.data(),buf.size());
      }
      result.append(QString().fromUtf16((ushort*)buf.constData()).toHtmlEscaped());
      last_pos=cur_pos+2;
    }
  } else {
    // Extract ansi strings
    while(last_pos<value.count() &&
          (cur_pos=value.indexOf(QByteArray("\x00",1),last_pos))!=-1)
    {
      if(cur_pos==last_pos) break;
      result.append(QString().fromLocal8Bit(value.mid(last_pos,
                                                      (cur_pos-last_pos)+1)
                                            .constData()).toHtmlEscaped());
      last_pos=cur_pos+1;
    }
  }

  if(p_ansi_encoded!=NULL) *p_ansi_encoded=is_ansi;
  return result;
}

/*
 * StringListToKeyValue
 */
QByteArray RegistryHive::StringListToKeyValue(QStringList strings,
                                              bool little_endian,
                                              bool ansi_encoded)
{
  // Return empty key value if there are no strings
  if(strings.count()==0) {
    if(ansi_encoded) return QByteArray("\x00\x00",2);
    else return QByteArray("\x00\x00\x00\x00",4);
  }

  // Convert string list
  QByteArray result=QByteArray();
  QString cur_string;
  QByteArray buf;
  QListIterator<QString> strings_it(strings);
  while(strings_it.hasNext()) {
    cur_string=strings_it.next();
    if(ansi_encoded) {
      // Ansi encoding, simply append char string and terminating \0
      result.append(cur_string.toLatin1().constData(),cur_string.size());
      result.append("\x00",1);
    } else {
      // Unicode encoding
      // First, convert value to utf16
      // TODO: May fail if there is a char that needs more than 16 bit
      buf=QByteArray((char*)(cur_string.utf16()),cur_string.size()*2);
      // Then convert to correct endianness
      if(little_endian) {
        HTOUTF16LE(buf.data(),buf.size());
      } else {
        HTOUTF16BE(buf.data(),buf.size());
      }
      // And finally append converted value and terminating \0\0 to result
      result.append(buf);
      result.append("\x00\x00",2);
    }
  }

  // Append terminating \0 chars and return
  if(ansi_encoded) result.append("\x00",1);
  else result.append("\x00\x00",2);
  return result;
}

/*
 * GetKeyValueTypes
 */
QStringList RegistryHive::GetKeyValueTypes() {
  return QStringList()<<"REG_NONE"
                        <<"REG_SZ"
                        <<"REG_EXPAND_SZ"
                        <<"REG_BINARY"
                        <<"REG_DWORD"
                        <<"REG_DWORD_BIG_ENDIAN"
                        <<"REG_LINK"
                        <<"REG_MULTI_SZ"
                        <<"REG_RESOURCE_LIST"
                        <<"REG_FULL_RESOURCE_DESC"
                        <<"REG_RESOURCE_REQ_LIST"
                        <<"REG_QWORD";
}

/*
 * KeyTypeToString
 */
QString RegistryHive::KeyValueTypeToString(int value_type) {
  QString ret="";

  switch(value_type) {
    case hive_t_REG_NONE:
      ret="REG_NONE";
      break;
    case hive_t_REG_SZ:
      ret="REG_SZ";
      break;
    case hive_t_REG_EXPAND_SZ:
      ret="REG_EXPAND_SZ";
      break;
    case hive_t_REG_BINARY:
      ret="REG_BINARY";
      break;
    case hive_t_REG_DWORD:
      ret="REG_DWORD";
      break;
    case hive_t_REG_DWORD_BIG_ENDIAN:
      ret="REG_DWORD_BIG_ENDIAN";
      break;
    case hive_t_REG_LINK:
      ret="REG_LINK";
      break;
    case hive_t_REG_MULTI_SZ:
      ret="REG_MULTI_SZ";
      break;
    case hive_t_REG_RESOURCE_LIST:
      ret="REG_RESOURCE_LIST";
      break;
    case hive_t_REG_FULL_RESOURCE_DESCRIPTOR:
      ret="REG_FULL_RESOURCE_DESC";
      break;
    case hive_t_REG_RESOURCE_REQUIREMENTS_LIST:
      ret="REG_RESOURCE_REQ_LIST";
      break;
    case hive_t_REG_QWORD:
      ret="REG_QWORD";
      break;
    default:
      ret=QString("0x%1").arg((quint32)value_type,8,16,QChar('0'));
  }

  return ret;
}

/*
 * StringToKeyValueType
 */
int RegistryHive::StringToKeyValueType(QString value_type) {
  if(value_type=="REG_NONE") return hive_t_REG_NONE;
  if(value_type=="REG_SZ") return hive_t_REG_SZ;
  if(value_type=="REG_EXPAND_SZ") return hive_t_REG_EXPAND_SZ;
  if(value_type=="REG_BINARY") return hive_t_REG_BINARY;
  if(value_type=="REG_DWORD") return hive_t_REG_DWORD;
  if(value_type=="REG_DWORD_BIG_ENDIAN") return hive_t_REG_DWORD_BIG_ENDIAN;
  if(value_type=="REG_LINK") return hive_t_REG_LINK;
  if(value_type=="REG_MULTI_SZ") return hive_t_REG_MULTI_SZ;
  if(value_type=="REG_RESOURCE_LIST") return hive_t_REG_RESOURCE_LIST;
  if(value_type=="REG_FULL_RESOURCE_DESC")
    return hive_t_REG_FULL_RESOURCE_DESCRIPTOR;
  if(value_type=="REG_RESOURCE_REQ_LIST")
    return hive_t_REG_RESOURCE_REQUIREMENTS_LIST;
  if(value_type=="REG_QWORD") return hive_t_REG_QWORD;

  // I think this might be a good default :-)
  return hive_t_REG_BINARY;
}

/*
 * FiletimeToUnixtime
 */
quint64 RegistryHive::FiletimeToUnixtime(qint64 filetime) {
  return (unsigned)((filetime-EPOCH_DIFF)/10000000);
}

/*
 * AddNode
 */
int RegistryHive::AddNode(QString parent_node_path, QString node_name) {
  if(!this->is_hive_writable) return 0;

  // Make sure name does not contain a backslash char
  if(node_name.contains('\\')) {
    this->SetError(tr("Unable to add node with name '%1'. "
                        "Names can not include a backslash character.")
                     .arg(node_name));
    return 0;
  }

  // Get node handle to the parent where the new node should be created
  hive_node_h parent_node;
  if(!this->GetNodeHandle(parent_node_path,&parent_node)) {
    this->SetError(tr("Unable to get node handle for '%1'!")
                     .arg(parent_node_path));
    return 0;
  }

  // Make sure there is no other node with same name
  QMap<QString,int> child_nodes=this->GetNodes(parent_node);
  if(child_nodes.contains(node_name.toLatin1())) {
    this->SetError(tr("The node '%1\\%2' already exists!")
                     .arg(parent_node_path,node_name));
    return 0;
  }

  // Add new node
  hive_node_h new_node=hivex_node_add_child(this->p_hive,
                                            parent_node,
                                            node_name.toLatin1().constData());
  if(new_node==0) {
    this->SetError(tr("Unable to create new node '%1\\%2'!")
                     .arg(parent_node_path,node_name));
    return 0;
  }

  this->has_changes_to_commit=true;
  return new_node;
}

/*
 * DeleteNode
 */
bool RegistryHive::DeleteNode(QString node_path) {
  if(!this->is_hive_writable) return false;

  // Get node handle to the node that should be deleted
  hive_node_h node;
  if(!this->GetNodeHandle(node_path,&node)) {
    this->SetError(tr("Unable to get node handle for '%1'!")
                     .arg(node_path));
    return false;
  }

  // Delete node
  if(hivex_node_delete_child(this->p_hive,node)==-1) {
    this->SetError(tr("Unable to delete node '%1'!")
                     .arg(node_path));
    return false;
  }

  this->has_changes_to_commit=true;
  return true;
}

/*
 * AddKey
 */
int RegistryHive::AddKey(QString parent_node_path,
                         QString key_name,
                         QString key_value_type,
                         QByteArray key_value)
{
  if(!this->is_hive_open || !this->is_hive_writable) {
    this->SetError(tr("Hive has not been opened or opened read-only!"));
    return false;
  }

  return this->SetKey(parent_node_path,
                      key_name,
                      key_value_type,
                      key_value,
                      true);
}

/*
 * UpdateKey
 */
int RegistryHive::UpdateKey(QString parent_node_path,
                            QString key_name,
                            QString key_value_type,
                            QByteArray key_value)
{
  if(!this->is_hive_open || !this->is_hive_writable) {
    this->SetError(tr("Hive has not been opened or opened read-only!"));
    return false;
  }

  return this->SetKey(parent_node_path,
                      key_name,
                      key_value_type,
                      key_value,
                      false);
}

/*
 * DeleteKey
 */
bool RegistryHive::DeleteKey(QString parent_node_path, QString key_name) {
  if(!this->is_hive_open || !this->is_hive_writable) {
    this->SetError(tr("Hive has not been opened or opened read-only!"));
    return false;
  }

  // libhivex offers no possibility to delete a single key :-(
  // As a work around, this function temporarly stores all keys of the specified
  // node, then deletes them all an re-creates all but the one that should be
  // deleted.

  // Get handle to parent node
  hive_node_h parent_node;
  if(!this->GetNodeHandle(parent_node_path,&parent_node)) {
    return false;
  }

  // Get all child keys
  hive_value_h *p_keys=hivex_node_values(this->p_hive,parent_node);
  if(p_keys==NULL) {
    this->SetError(tr("Unable to enumerate child keys for parent '%1'!")
                     .arg(parent_node_path));
    return false;
  }

  // Get all child key values except the one that should be deleted
  int i=0;
  char *p_name;
  int node_keys_count=0;
  hive_set_value *node_keys=NULL;

#define FREE_NODE_KEYS() {             \
  for(int x=0;x<node_keys_count;x++) { \
    free(node_keys[x].key);            \
    free(node_keys[x].value);          \
  }                                    \
  free(node_keys);                     \
}

  while(p_keys[i]) {
    p_name=hivex_value_key(this->p_hive,p_keys[i]);
    if(p_name==NULL) {
      this->SetError(tr("Unable to get key name for a child of '%1'!")
                       .arg(parent_node_path));
      return false;
    }
    if(QString(p_name)!=key_name) {
      // Current key is not the one that should be deleted, save it
      // Alloc mem for new hive_set_value struct in node_keys array
      node_keys=(hive_set_value*)realloc(node_keys,
                                         sizeof(hive_set_value)*
                                           (node_keys_count+1));
      if(node_keys==NULL) {
        this->SetError(tr("Unable to alloc enough memory for all child keys!"));
        return false;
      }
      // Save key name in hive_set_value struct
      node_keys[node_keys_count].key=p_name;
      // Get key value, key value type and key value len and save to
      // hive_set_value struct
      node_keys[node_keys_count].value=
        hivex_value_value(this->p_hive,
                          p_keys[i],
                          &(node_keys[node_keys_count].t),
                          &(node_keys[node_keys_count].len));
      if(node_keys[node_keys_count].value==NULL) {
        this->SetError(tr("Unable to get value for key '%1'!").arg(p_name));
        free(p_name);
        // Free all temporary stored keys
        FREE_NODE_KEYS();
        return false;
      }
      node_keys_count++;
    } else {
      // Current key is to be deleted, ignore it
      free(p_name);
    }
    i++;
  }

  // Save all stored keys to hive, which will discard the one that should be
  // deleted
  if(hivex_node_set_values(this->p_hive,
                           parent_node,
                           node_keys_count,
                           node_keys,
                           0)!=0)
  {
    this->SetError(tr("Unable to re-save all child keys! Please discard any "
                      "changes you made and start over. No doing so might end "
                      "in data loss!"));
    // Free all temporary stored keys
    FREE_NODE_KEYS();
    return false;
  }

  // Free all temporary stored keys and return
  FREE_NODE_KEYS();

#undef FREE_NODE_KEYS

  this->has_changes_to_commit=true;
  return true;
}

/*******************************************************************************
 * Private
 ******************************************************************************/

/*
 * HivexError2String
 */
QString RegistryHive::HivexError2String(int error) {
  switch(error) {
    case ENOTSUP:
      return QString("Corrupt or unsupported Registry file format.");
      break;
    case HIVEX_NO_KEY:
      return QString("Missing root key.");
      break;
    case EINVAL:
      return QString("Passed an invalid argument to the function.");
      break;
    case EFAULT:
      return QString("Followed a Registry pointer which goes outside the "
                       "registry or outside a registry block.");
      break;
    case ELOOP:
      return QString("Registry contains cycles.");
      break;
    case ERANGE:
      return QString("Field in the registry out of range.");
      break;
    case EEXIST:
      return QString("Registry key already exists.");
      break;
    case EROFS:
      return QString("Tried to write to a registry which is not opened for "
                       "writing.");
      break;
    default:
      return QString("Unknown error.");
  }
}

/*
 * SetError
 */
void RegistryHive::SetError(QString msg) {
  this->erro_msg=msg;
  this->is_error=true;
}

/*
 * GetNodeHandle
 */
bool RegistryHive::GetNodeHandle(QString &path, hive_node_h *p_node) {
  QStringList nodes;
  int i=0;

  // Get root node handle
  *p_node=hivex_root(this->p_hive);
  if(*p_node==0) {
    this->SetError(tr("Unable to get root node!"));
    return false;
  }

  if(path!="\\") {
    // If we aren't listing the root node, we have to get a handle to the
    // last node in the path. Split path into nodes
    nodes=path.split('\\',QString::SkipEmptyParts);

    // Iterate to the correct parent node
    for(i=0;i<nodes.count();i++) {
      *p_node=hivex_node_get_child(this->p_hive,
                                   *p_node,
                                   nodes.value(i).toLatin1().constData());
      if(*p_node==0) {
        this->SetError(tr("Unable to find node '%1'!").arg(nodes.value(i)));
        return false;
      }
    }
  }

  return true;
}

/*
 * GetKeyHandle
 */
bool RegistryHive::GetKeyHandle(QString &parent_node_path,
                                QString &key_name,
                                hive_value_h *p_key)
{
  // Get handle to parent node
  hive_node_h parent_node;
  if(!this->GetNodeHandle(parent_node_path,&parent_node)) {
    return false;
  }

  // Get handle to key
  *p_key=hivex_node_get_value(this->p_hive,
                              parent_node,
                              key_name.toLatin1().constData());
  if(*p_key==0) {
    this->SetError(tr("Unable to get handle to key '%1\\%2'!")
                     .arg(parent_node_path,key_name));
    return false;
  }

  return true;
}

/*
 * GetNodesHelper
 */
QMap<QString,int> RegistryHive::GetNodesHelper(hive_node_h parent_node) {
  QMap<QString,int> keys;
  char *p_name;
  int i=0;

  // Get child nodes
  hive_node_h *child_nodes=hivex_node_children(this->p_hive,parent_node);
  if(child_nodes==NULL) {
    this->SetError(
      tr("Unable to enumerate child nodes!"));
    return QMap<QString,int>();
  }

  // Build result
  keys.clear();
  i=0;
  while(child_nodes[i]) {
    p_name=hivex_node_name(this->p_hive,child_nodes[i]);
    if(p_name==NULL) {
      this->SetError(tr("Unable to get node name!"));
      free(child_nodes);
      return QMap<QString,int>();
    }
    keys.insert(QString(p_name),(int)child_nodes[i]);
    free(p_name);
    i++;
  }
  free(child_nodes);

  return keys;
}

/*
 * GetKeysHelper
 */
QMap<QString,int> RegistryHive::GetKeysHelper(hive_node_h parent_node) {
  QMap<QString,int> keys;
  char *p_name;
  int i=0;

  // Get child keys
  hive_value_h *p_keys=hivex_node_values(this->p_hive,parent_node);
  if(p_keys==NULL) {
    this->SetError(
      tr("Unable to enumerate child keys!"));
    return QMap<QString,int>();
  }

  // Build result list
  keys.clear();
  i=0;

  while(p_keys[i]) {
    p_name=hivex_value_key(this->p_hive,p_keys[i]);
    if(p_name==NULL) {
      this->SetError(tr("Unable to get key name!"));
      return QMap<QString,int>();
    }
    keys.insert(QString(p_name),p_keys[i]);
    free(p_name);
    i++;
  }
  free(p_keys);

  return keys;
}

/*
 * GetKeyValueHelper
 */
QByteArray RegistryHive::GetKeyValueHelper(hive_value_h hive_key,
                                           int *p_value_type,
                                           size_t *p_value_len)
{
  QByteArray key_value;
  char *p_key_value;

  p_key_value=hivex_value_value(this->p_hive,
                                hive_key,
                                (hive_type*)p_value_type,
                                p_value_len);
  if(p_key_value==NULL) {
    this->SetError(tr("Unable to get key value!"));
    *p_value_type=-1;
    return QByteArray();
  }

  // Feed QByteArray and free p_key_value
  key_value=QByteArray(p_key_value,*p_value_len);
  free(p_key_value);

  return key_value;
}

/*
 * PathExists
 */
bool RegistryHive::PathExists(QString path) {
  bool ret;
  hive_node_h node;

  ret=this->GetNodeHandle(path,&node);
  if(!ret || this->Error()) {
    // Clear error and return false
    this->GetErrorMsg();
    return false;
  }

  return true;
}

/*
 * SetKey
 */
int RegistryHive::SetKey(QString &parent_node_path,
                         QString &key_name,
                         QString &key_value_type,
                         QByteArray &key_value,
                         bool create_key)
{
  // Get node handle to the node that holds the key to create/update
  hive_node_h parent_node;
  if(!this->GetNodeHandle(parent_node_path,&parent_node)) {
    return 0;
  }

  // Make sure key exists if we should update it
  if(!create_key) {
    hive_value_h temp_key=hivex_node_get_value(this->p_hive,
                                               parent_node,
                                               key_name.toLatin1().constData());
    if(temp_key==0) {
      this->SetError(tr("Inexisting key '%1\\%2' can't be updated!")
                       .arg(parent_node_path,key_name));
      return 0;
    }
  }

  // Create and populate hive_set_value structure
  hive_set_value key_val;
  key_val.key=(char*)malloc((sizeof(char)*key_name.toLatin1().count())+1);
  key_val.value=(char*)malloc(sizeof(char)*key_value.size());
  if(key_val.key==NULL || key_val.value==NULL) {
    this->SetError(tr("Unable to alloc memory for hive_set_value struct!"));
    return 0;
  }
  strcpy(key_val.key,key_name.toLatin1().constData());
  key_val.t=(hive_type)this->StringToKeyValueType(key_value_type);
  key_val.len=key_value.size();
  memcpy(key_val.value,key_value.constData(),key_value.size());

  // Create/Update key
  if(hivex_node_set_value(this->p_hive,parent_node,&key_val,0)!=0) {
    this->SetError(tr("Unable to update key '%1\\%2'!")
                     .arg(parent_node_path,key_name));
    return 0;
  }

  // Free the hive_set_value structure
  free(key_val.key);
  free(key_val.value);

  // To make sure everything worked, a hadle to the new key is now requeried
  // from hive and then returned
  hive_value_h key;
  if(!this->GetKeyHandle(parent_node_path,key_name,&key)) {
    return 0;
  }

  this->has_changes_to_commit=true;
  return key;
}

/*
 * FindUnicodeStringEnd
 */
int RegistryHive::FindUnicodeStringEnd(QByteArray data, int offset) {
  int end_pos;
  for(end_pos=offset;end_pos<(data.size()-1);end_pos+=2) {
    if(*((quint16*)(data.constData()+end_pos))==0) break;
  }
  return end_pos<(data.size()-1) ? end_pos : -1;
}
