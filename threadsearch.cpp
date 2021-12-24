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
#include <QMetaType>

#include "threadsearch.h"
#include "registryhive.h"

ThreadSearch::ThreadSearch(QObject *p_parent) : QThread(p_parent) {
  this->hive_file="";
  this->h_hive=NULL;
  this->keywords=QList<QByteArray>();
  this->search_nodes=false;
  this->search_keys=false;
  this->search_values=false;
  this->root_node=0;

  // Register meta types to be used in signals
  qRegisterMetaType<ThreadSearch::eMatchType>("ThreadSearch::eMatchType");
}

bool ThreadSearch::Search(QString registry_hive,
                          QList<QByteArray> search_keywords,
                          bool search_node_names,
                          bool search_key_names,
                          bool search_key_values,
                          QString search_path)
{
  this->hive_file=registry_hive;
  this->keywords=search_keywords;
  this->search_nodes=search_node_names;
  this->search_keys=search_key_names;
  this->search_values=search_key_values;
  this->root_path=search_path=="\\" ? "" : search_path;

  // Try to open hive
  this->h_hive=hivex_open(this->hive_file.toLatin1().constData(),0);
  if(this->h_hive==NULL) return false;

  // Get root node
  this->root_node=hivex_root(this->h_hive);
  if(this->root_node==0) {
    hivex_close(this->h_hive);
    return false;
  }

  // If a root path was specified, iterate to it
  if(this->root_path!="") {
    QStringList path_nodes=search_path.split("\\",QString::SkipEmptyParts);
    int i;
    for(i=0;i<path_nodes.count();i++) {
      this->root_node=hivex_node_get_child(this->h_hive,
                                           this->root_node,
                                           path_nodes.at(i).toLatin1().constData());
      if(this->root_node==0) {
        hivex_close(this->h_hive);
        return false;
      }
    }
  }

  this->start();
  return true;
}

void ThreadSearch::run() {
  this->Match();
  hivex_close(this->h_hive);
}

void ThreadSearch::Match(QString path, hive_node_h node) {
  char *p_node_name;
  int i,ii;
  hive_node_h *p_node_childs;
  QByteArray *p_byte_array;

  if(node!=0) {

    p_node_name=hivex_node_name(this->h_hive,node);
    if(p_node_name==NULL) return;

    if(this->search_nodes) {
      // Compare node name to keywords
      p_byte_array=new QByteArray(p_node_name);
      for(i=0;i<this->keywords.count();i++) {
        if(p_byte_array->indexOf(this->keywords.at(i))!=-1) {
          emit(SignalFoundMatch(ThreadSearch::eMatchType_NodeName,
                                path,
                                QString(p_node_name),
                                QString()));
          break;
        }
      }
      delete p_byte_array;
    }

    if(this->search_keys || this->search_values) {
      // Get key,value pairs for current node
      hive_value_h *p_keys=hivex_node_values(this->h_hive,node);
      if(p_keys==NULL) {
        delete p_node_name;
        return;
      }

      if(this->search_keys) {
        // Compare key names to keywords
        char *p_keyname;
        for(i=0;p_keys[i];i++) {
          p_keyname=hivex_value_key(this->h_hive,p_keys[i]);
          if(p_keyname==NULL) continue;
          p_byte_array=new QByteArray(p_keyname);
          for(ii=0;ii<this->keywords.count();ii++) {
            if(p_byte_array->indexOf(this->keywords.at(ii))!=-1) {
              emit(SignalFoundMatch(ThreadSearch::eMatchType_KeyName,
                                    path+"\\"+p_node_name,
                                    strlen(p_keyname)==0 ? QString("(default)") : QString(p_keyname),
                                    QString()));
              break;
            }
          }
          delete p_byte_array;
          delete p_keyname;
        }
      }
      if(this->search_values) {
        // Compare key values to keywords
        char *p_value;
        hive_type val_type;
        size_t val_len;

        for(i=0;p_keys[i];i++) {
          p_value=hivex_value_value(this->h_hive,p_keys[i],&val_type,&val_len);
          if(p_value==NULL) continue;
          p_byte_array=new QByteArray(p_value,val_len);
          for(ii=0;ii<this->keywords.count();ii++) {
            if(p_byte_array->indexOf(this->keywords.at(ii))!=-1) {
              char *p_keyname=hivex_value_key(this->h_hive,p_keys[i]);
              if(p_keyname==NULL) continue;
              emit(SignalFoundMatch(ThreadSearch::eMatchType_KeyValue,
                                    path+"\\"+p_node_name,
                                     strlen(p_keyname)==0 ? QString("(default)") : QString(p_keyname),
                                    RegistryHive::KeyValueToString(*p_byte_array,val_type)));
              delete p_keyname;
              break;
            }
          }
          delete p_byte_array;
          delete p_value;
        }

      }
      delete p_keys;
    }

    // Search in subnodes
    p_node_childs=hivex_node_children(this->h_hive,node);
    if(p_node_childs!=NULL) {
      i=0;
      while(p_node_childs[i]) {
        this->Match(path+"\\"+p_node_name,p_node_childs[i]);
        i++;
      }
      delete p_node_childs;
    }
    delete p_node_name;
  } else {
    p_node_childs=hivex_node_children(this->h_hive,this->root_node);
    if(p_node_childs!=NULL) {
      i=0;
      while(p_node_childs[i]) {
        this->Match(this->root_path,p_node_childs[i]);
        i++;
      }
      delete p_node_childs;
    }
  }
}
