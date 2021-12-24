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

#include <QDir>

#include "settings.h"

#ifndef SYSTEM_REPORT_TEMPLATE_DIR
  #ifndef __MINGW32__
    #define SYSTEM_REPORT_TEMPLATE_DIR "/usr/share/fred/report_templates"
  #else
    #define SYSTEM_REPORT_TEMPLATE_DIR ".\\report_templates\\"
  #endif
#endif

#define APP_ORGANIZATION "pinguin.lu"
#define APP_NAME "fred"

// Default settings
#define DEFAULT_REPORT_TEMPLATE_DIRS QStringList()<<SYSTEM_REPORT_TEMPLATE_DIR \
                                                <<this->user_report_template_dir
#define DEFAULT_WINDOW_GEOMETRY_STATUS true
#define DEFAULT_RECENT_FILES_DEPTH 5
#define DEFAULT_LAST_OPEN_LOCATION QDir::homePath()
#define DEFAULT_OPEN_HIVES_READ_ONLY true

/*******************************************************************************
 * Public
 ******************************************************************************/

Settings::Settings(QObject *p_parent) : QObject(p_parent) {
  // Init vars
  this->p_settings=NULL;
  this->initialized=false;
  this->user_settings_dir=QDir::homePath()
    .append(QDir::separator()).append(".fred");
  this->user_report_template_dir=QString(this->user_settings_dir)
                                   .append(QDir::separator())
                                   .append("report_templates");

  // Make sure config dirs exist
  if(!QDir(this->user_settings_dir).exists()) {
    // User config dir does not exists, try to create it
    if(!QDir().mkpath(this->user_settings_dir)) {
      // TODO: Maybe warn user
      return;
    }
  }
  if(!QDir(this->user_report_template_dir).exists()) {
    // Create config dir sub folder for report templates
    if(!QDir().mkpath(this->user_report_template_dir)) {
      // TODO: Maybe warn user
      return;
    }
  }

  // Create / open settings
#ifndef __MINGW32__
  // On any Unix-like OS, settings should be saved in the .fred folder
  this->p_settings=new QSettings(QString(this->user_settings_dir)
                                   .append(QDir::separator())
                                   .append("fred.conf"),
                                 QSettings::NativeFormat,
                                 this);
#else
  // On Windows, it can be stored inside registry
  this->p_settings=new QSettings(QSettings::NativeFormat,
                                 QSettings::UserScope,
                                 APP_ORGANIZATION,
                                 APP_NAME,
                                 this);
#endif
  if(this->p_settings->status()!=QSettings::NoError ||
     !this->p_settings->isWritable())
  {
    return;
  }

  this->initialized=true;
}

void Settings::Reset() {
  if(!this->initialized) return;
  // Clear all current settings
  this->p_settings->clear();
}

void Settings::SetReportTemplateDirs(QStringList &dirs) {
  if(!this->initialized) return;
  this->p_settings->setValue("ReportTemplateDirs",dirs);
}

QStringList Settings::GetReportTemplateDirs() {
  if(!this->initialized) return DEFAULT_REPORT_TEMPLATE_DIRS;
  return this->p_settings->value("ReportTemplateDirs",
                                 DEFAULT_REPORT_TEMPLATE_DIRS).toStringList();
}

void Settings::SetWindowGeometryStatus(bool save) {
  if(!this->initialized) return;
  this->p_settings->setValue("WindowGeometries/EnableSaving",save);
}

bool Settings::GetWindowGeometryStatus() {
  if(!this->initialized) return DEFAULT_WINDOW_GEOMETRY_STATUS;
  return this->p_settings->value("WindowGeometries/EnableSaving",
                                 DEFAULT_WINDOW_GEOMETRY_STATUS).toBool();
}

void Settings::SetWindowGeometry(QString window_name, QByteArray geometry) {
  if(!this->initialized || !this->GetWindowGeometryStatus()) return;
  this->p_settings->setValue(QString("WindowGeometries/%1").arg(window_name),
                             geometry);
}

QByteArray Settings::GetWindowGeometry(QString window_name) {
  if(!this->initialized || !this->GetWindowGeometryStatus()) {
    return QByteArray();
  }
  return this->p_settings->value(QString("WindowGeometries/%1")
                                   .arg(window_name)).toByteArray();
}

void Settings::SetRecentFilesDepth(int depth) {
  if(!this->initialized) return;
  this->p_settings->setValue("RecentFilesDepth",depth);
  // Make sure there are currently not more recent files as allowed
  QStringList recent_files=this->GetRecentFiles();
  while(recent_files.count()>depth) recent_files.removeLast();
  this->SetRecentFiles(recent_files);
}

int Settings::GetRecentFilesDepth() {
  if(!this->initialized) return DEFAULT_RECENT_FILES_DEPTH;
  return this->p_settings->value("RecentFilesDepth",
                                 DEFAULT_RECENT_FILES_DEPTH).toInt();
}

void Settings::SetRecentFiles(QStringList &recent_files) {
  if(!this->initialized) return;
  this->p_settings->setValue("RecentFiles",recent_files);
}

void Settings::AddRecentFile(QString file) {
  // Get recent files
  QStringList recent_files=this->GetRecentFiles();

  if(recent_files.contains(file)) {
    // File already exists in recent list. Simply move it to top
    recent_files.move(recent_files.indexOf(file),0);
  } else {
    // We only save RecentFilesDepth() files at max
    while(recent_files.count()>=this->GetRecentFilesDepth())
      recent_files.removeLast();
    recent_files.prepend(file);
  }

  this->SetRecentFiles(recent_files);
}

QStringList Settings::GetRecentFiles() {
  if(!this->initialized) return QStringList();
  return this->p_settings->value("RecentFiles",QStringList()).toStringList();
}

void Settings::SetLastOpenLocation(QString dir) {
  if(!this->initialized) return;
  this->p_settings->setValue("LastOpenLocation",dir);
}

QString Settings::GetLastOpenLocation() {
  if(!this->initialized) return DEFAULT_LAST_OPEN_LOCATION;
  QString last_loc;
  last_loc=this->p_settings->value("RecentFiles",
                                   DEFAULT_LAST_OPEN_LOCATION).toString();
  if(QDir(last_loc).exists()) return last_loc;
  else return DEFAULT_LAST_OPEN_LOCATION;
}

void Settings::SetOpenHivesReadOnly(bool read_only) {
  if(!this->initialized) return;
  this->p_settings->setValue("OpenFilesReadOnly",read_only);
}

bool Settings::GetOpenHivesReadOnly() {
  if(!this->initialized) return DEFAULT_OPEN_HIVES_READ_ONLY;
  return this->p_settings->value("OpenFilesReadOnly",
                                 DEFAULT_OPEN_HIVES_READ_ONLY).toBool();
}
