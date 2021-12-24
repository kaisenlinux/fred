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

#ifndef REPORTENGINE_H
#define REPORTENGINE_H

#include <QObject>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptContext>
#include <QString>
#include <QVariant>
#include <QMap>

#include "registryhive.h"
#include "qtscript_types/bytearray.h"

#define FRED_REPORTENGINE_API_VERSION 2

class ReportEngine : public QScriptEngine {
  Q_OBJECT

  public:
    struct s_RegistryKeyValue {
      int type;
      int length;
      QByteArray value;
    };

    RegistryHive *p_registry_hive;
    QString report_content;

    ReportEngine(RegistryHive *p_hive);
    ~ReportEngine();

    QMap<QString,QVariant> GetReportTemplateInfo(QString file);
    bool GenerateReport(RegistryHive *p_hive,
                        QString report_file,
                        QString &report_result,
                        bool console_mode=true);

  private:
    ByteArray *p_type_byte_array;

    static QScriptValue Print(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue PrintLn(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue GetRegistryNodes(QScriptContext *context,
                                         QScriptEngine *engine);
    static QScriptValue GetRegistryKeys(QScriptContext *context,
                                        QScriptEngine *engine);
    static QScriptValue RegistryKeyValueToScript(QScriptEngine *engine,
                                                 const s_RegistryKeyValue &s);
    static void RegistryKeyValueFromScript(const QScriptValue &obj,
                                           s_RegistryKeyValue &s);
    static QScriptValue GetRegistryKeyValue(QScriptContext *context,
                                            QScriptEngine *engine);
    static QScriptValue RegistryKeyValueToString(QScriptContext *context,
                                                 QScriptEngine *engine);
    static QScriptValue RegistryKeyValueToVariant(QScriptContext *context,
                                                  QScriptEngine *engine);
    static QScriptValue RegistryKeyValueToStringList(QScriptContext *context,
                                                     QScriptEngine *engine);
    static QScriptValue RegistryKeyTypeToString(QScriptContext *context,
                                                QScriptEngine *engine);
    static QScriptValue GetRegistryNodeModTime(QScriptContext *context,
                                               QScriptEngine *engine);

    bool GetReportTemplateFileContents(QString file, QString &contents);
    void UpdateExportedFunctions(RegistryHive *p_hive);
};

Q_DECLARE_METATYPE(ReportEngine::s_RegistryKeyValue)

#endif // REPORTENGINE_H
