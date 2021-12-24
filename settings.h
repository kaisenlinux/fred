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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QByteArray>

class Settings : public QObject {
  Q_OBJECT

  public:
    explicit Settings(QObject *p_parent=0);

    void Reset();

    void SetReportTemplateDirs(QStringList &dirs);
    QStringList GetReportTemplateDirs();

    void SetWindowGeometryStatus(bool save);
    bool GetWindowGeometryStatus();
    void SetWindowGeometry(QString window_name, QByteArray geometry);
    QByteArray GetWindowGeometry(QString window_name);

    void SetRecentFilesDepth(int depth);
    int GetRecentFilesDepth();
    void SetRecentFiles(QStringList &recent_files);
    void AddRecentFile(QString file);
    QStringList GetRecentFiles();

    void SetLastOpenLocation(QString dir);
    QString GetLastOpenLocation();

    void SetOpenHivesReadOnly(bool read_only);
    bool GetOpenHivesReadOnly();

  private:
    QSettings *p_settings;
    bool initialized;
    QString user_settings_dir;
    QString user_report_template_dir;
};

#endif // SETTINGS_H
