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

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStringList>

#include <QtDebug>

#include "reportengine.h"

/*******************************************************************************
 * Public
 ******************************************************************************/

ReportEngine::ReportEngine(RegistryHive *p_hive) : QScriptEngine() {
  // Init vars
  this->p_registry_hive=p_hive;
  this->report_content="";

  // Add our constants
  this->globalObject().setProperty("ENGINE_API_VERSION",
                                   FRED_REPORTENGINE_API_VERSION,
                                   QScriptValue::ReadOnly|
                                     QScriptValue::Undeletable);
/*
  this->globalObject().setProperty("HIVE_FILE",
                                   this->p_registry_hive->Filename(),
                                   QScriptValue::ReadOnly|
                                     QScriptValue::Undeletable);
*/
  // Add our types to engine
  qScriptRegisterMetaType<s_RegistryKeyValue>(this,
                                              this->RegistryKeyValueToScript,
                                              this->RegistryKeyValueFromScript);
  this->p_type_byte_array=new ByteArray(this);
  this->globalObject().setProperty("ByteArray",
                                   this->p_type_byte_array->constructor());

  // TODO: Is it really necessary to explicitly export these functions
  // here ??????????
  // Add our functions
  // print
  QScriptValue func_print=this->newFunction(this->Print);
  this->globalObject().setProperty("print",func_print);
  // println
  QScriptValue func_println=this->newFunction(this->PrintLn);
  this->globalObject().setProperty("println",func_println);

  // RegistryKeyValueToString
  QScriptValue func_value_to_string=
    this->newFunction(this->RegistryKeyValueToString,2);
  this->globalObject().setProperty("RegistryKeyValueToString",
                                   func_value_to_string);
  // RegistryKeyValueToVariant
  QScriptValue func_value_to_variant=
    this->newFunction(this->RegistryKeyValueToVariant);
  this->globalObject().setProperty("RegistryKeyValueToVariant",
                                   func_value_to_variant);
  // RegistryKeyValueToStringList
  QScriptValue func_value_to_string_list=
    this->newFunction(this->RegistryKeyValueToStringList);
  this->globalObject().setProperty("RegistryKeyValueToStringList",
                                   func_value_to_string_list);
  // RegistryKeyTypeToString
  QScriptValue func_type_to_string=
    this->newFunction(this->RegistryKeyTypeToString,1);
  this->globalObject().setProperty("RegistryKeyTypeToString",
                                   func_type_to_string);
}

ReportEngine::~ReportEngine() {
  delete this->p_type_byte_array;
}

/*
 * GetReportTemplateInfo
 */
QMap<QString,QVariant> ReportEngine::GetReportTemplateInfo(QString file) {
  // Read template file
  QString report_code;
  if(!this->GetReportTemplateFileContents(file,report_code)) {
    QMap<QString,QVariant> error_msg;
    error_msg["error"]=report_code;
    return error_msg;
  }

  // Evaluate report template script
  QScriptValue report_result=this->evaluate(report_code,file);
  if (report_result.isError() || this->hasUncaughtException()) {
    QMap<QString,QVariant> error_msg;
    error_msg["error"]=QString("File: %1\n Line: %2\nError: %3")
                         .arg(file)
                         .arg(report_result.property("lineNumber").toInt32())
                         .arg(report_result.toString());
    return error_msg;
  }

  // Try to call the fred_report_info script function and return result
  QScriptValue fred_report_info_func=
    this->globalObject().property("fred_report_info");
  if(!fred_report_info_func.isFunction()) {
    QMap<QString,QVariant> error_msg;
    error_msg["error"]=
      QString("Report template '%1' does not have a fred_report_info function!")
        .arg(file)
        .arg(report_result.property("lineNumber").toInt32())
        .arg(report_result.toString());
    return error_msg;
  }
  QScriptValue fred_report_info_res=fred_report_info_func.call();
  // TODO: Maybe do more checking on return value
  return fred_report_info_res.toVariant().toMap();
}

/*
 * GenerateReport
 */
bool ReportEngine::GenerateReport(RegistryHive *p_hive,
                                  QString report_file,
                                  QString &report_result,
                                  bool console_mode)
{
  // TODO: Support or remove console_mode
  Q_UNUSED(console_mode);

  // Clear internal buffer
  this->report_content.clear();

  // Update exported functions
  this->UpdateExportedFunctions(p_hive);

  // Read template file
  QString report_code;
  if(!this->GetReportTemplateFileContents(report_file,report_code)) {
    report_result=report_code;
    return false;
  }

  // Evaluate report template script
  QScriptValue script_result=this->evaluate(report_code,report_file);
  if (script_result.isError() || this->hasUncaughtException()) {
    script_result=QString("File: %1\n Line: %2\nError: %3")
                    .arg(report_file)
                    .arg(script_result.property("lineNumber").toInt32())
                    .arg(script_result.toString());
    return false;
  }

  // Try to call the fred_report_html script function and return result
  QScriptValue fred_report_html_func=
    this->globalObject().property("fred_report_html");
  if(!fred_report_html_func.isFunction()) {
    report_result=
      QString("Report template '%1' does not have a fred_report_info function!")
        .arg(report_file)
        .arg(script_result.property("lineNumber").toInt32())
        .arg(script_result.toString());
    return false;
  }
  QScriptValue fred_report_html_res=fred_report_html_func.call();

  // TODO: Maybe do more checking on return value
  report_result=this->report_content;
  return true;
}

/*******************************************************************************
 * Public Slots
 ******************************************************************************/

/*******************************************************************************
 * Private
 ******************************************************************************/

/*
 * Print
 */
QScriptValue ReportEngine::Print(QScriptContext *context,
                                 QScriptEngine *engine)
{
  int i;
  QString content;

  // Append all arguments to content
  for(i=0;i<context->argumentCount();++i) {
    //if(i>0) content.append(" ");
    content.append(context->argument(i).toString());
  }

  qobject_cast<ReportEngine*>(engine)->report_content.append(content);

  return engine->undefinedValue();
}

/*
 * PrintLn
 */
QScriptValue ReportEngine::PrintLn(QScriptContext *context,
                                   QScriptEngine *engine)
{
  int i;
  QString content;

  // Append all arguments to content
  for(i=0;i<context->argumentCount();++i) {
    //if(i>0) content.append(" ");
    content.append(context->argument(i).toString());
  }

  qobject_cast<ReportEngine*>(engine)->
    report_content.append(content).append("\n");

  return engine->undefinedValue();
}

/*
 * GetRegistryNodes
 */
QScriptValue ReportEngine::GetRegistryNodes(QScriptContext *context,
                                            QScriptEngine *engine)
{
  QScriptValue calleeData;
  RegistryHive *p_hive;
  QMap<QString,int> nodes;
  QScriptValue ret_nodes;
  int ii=0;

  // This function needs one argument, parent node path
  if(context->argumentCount()!=1) return engine->undefinedValue();

  // Get calle data (Pointer to RegistryHive class)
  calleeData=context->callee().data();
  p_hive=qobject_cast<RegistryHive*>(calleeData.toQObject());

  // Get nodes
  nodes=p_hive->GetNodes(context->argument(0).toString());
  if(p_hive->Error()) {
    // Clear error state
    p_hive->GetErrorMsg();
    return engine->undefinedValue();
  }

  // Build script array
  ret_nodes=engine->newArray(nodes.count());
  QMapIterator<QString,int> i(nodes);
  while(i.hasNext()) {
    i.next();
    ret_nodes.setProperty(ii++,QScriptValue(i.key()));
  }

  return ret_nodes;
}

/*
 * GetRegistryKeys
 */
QScriptValue ReportEngine::GetRegistryKeys(QScriptContext *context,
                                           QScriptEngine *engine)
{
  QScriptValue calleeData;
  RegistryHive *p_hive;
  QMap<QString,int> keys;
  QScriptValue ret_keys;
  int ii=0;

  // This function needs one argument, parent node path
  if(context->argumentCount()!=1) return engine->undefinedValue();

  // Get calle data (Pointer to RegistryHive class)
  calleeData=context->callee().data();
  p_hive=qobject_cast<RegistryHive*>(calleeData.toQObject());

  // Get keys
  keys=p_hive->GetKeys(context->argument(0).toString());
  if(p_hive->Error()) {
    // Clear error state
    p_hive->GetErrorMsg();
    return engine->undefinedValue();
  }

  // Build script array
  ret_keys=engine->newArray(keys.count());
  QMapIterator<QString,int> i(keys);
  while(i.hasNext()) {
    i.next();
    ret_keys.setProperty(ii++,QScriptValue(i.key()));
  }

  return ret_keys;
}

/*
 * RegistryKeyValueToScript
 */
QScriptValue ReportEngine::RegistryKeyValueToScript(QScriptEngine *engine,
                                                    const s_RegistryKeyValue &s)
{
  QScriptValue obj=engine->newObject();
  obj.setProperty("type",s.type);
  obj.setProperty("length",s.length);
  ByteArray *p_byte_array=new ByteArray(engine);
  obj.setProperty("value",p_byte_array->newInstance(s.value));
  return obj;
}

/*
 * RegistryKeyValueFromScriptValue
 */
void ReportEngine::RegistryKeyValueFromScript(const QScriptValue &obj,
                                              s_RegistryKeyValue &s)
{
  s.type=obj.property("type").toInt32();
  s.length=obj.property("length").toInt32();
  s.value=qvariant_cast<QByteArray>(obj.property("value").data().toVariant());
}

/*
 * GetRegistryKeyValue
 */
QScriptValue ReportEngine::GetRegistryKeyValue(QScriptContext *context,
                                               QScriptEngine *engine)
{
  QScriptValue calleeData;
  RegistryHive *p_hive;
  QByteArray key_value;
  int key_type=0;
  size_t key_length=0;
  s_RegistryKeyValue script_key_value;

  // This function needs two arguments, key path and key name
  if(context->argumentCount()!=2) return engine->undefinedValue();

  // Get calle data (Pointer to RegistryHive class)
  calleeData=context->callee().data();
  p_hive=qobject_cast<RegistryHive*>(calleeData.toQObject());

  // Get key value
  key_value=p_hive->GetKeyValue(context->argument(0).toString(),
                                context->argument(1).toString(),
                                &key_type,
                                &key_length);
  if(p_hive->Error()) {
    // Get error message to clear error state
    p_hive->GetErrorMsg();
//    printf("\nError: %s\n",p_hive->GetErrorMsg().toLatin1().constData());
    return engine->undefinedValue();
  }

  // Save key value to s_RegistryKeyValue struct
  script_key_value.type=key_type;
  script_key_value.length=key_length;
  script_key_value.value=key_value;

  return ReportEngine::RegistryKeyValueToScript(engine,script_key_value);
}

/*
 * RegistryKeyValueToString
 */
QScriptValue ReportEngine::RegistryKeyValueToString(QScriptContext *context,
                                                    QScriptEngine *engine)
{
  QByteArray key_value;
  QString ret="";

  // This function needs two arguments, key value and value type
  if(context->argumentCount()!=2) return engine->undefinedValue();

  // Cast ByteArray argument to QByteArray and convert
  key_value=qvariant_cast<QByteArray>(context->argument(0).data().toVariant());
  ret=RegistryHive::KeyValueToString(key_value,
                                     context->argument(1).toInt32());

  return engine->newVariant(ret);
}

/*
 * RegistryKeyValueToVariant
 */
QScriptValue ReportEngine::RegistryKeyValueToVariant(QScriptContext *context,
                                                     QScriptEngine *engine)
{
  int offset=0;
  int length=-1;
  bool little_endian=true;
  QByteArray key_value;
  QString format="";
  QString ret="";

  // This function needs at least two arguments, key value and variant type,
  // and may have three optional arguments, offset, length and little_endian
  if(context->argumentCount()<2 || context->argumentCount()>5) {
    return engine->undefinedValue();
  }
  if(context->argumentCount()==3) {
    offset=context->argument(2).toInt32();
  }
  if(context->argumentCount()==4) {
    offset=context->argument(2).toInt32();
    length=context->argument(3).toInt32();
  }
  if(context->argumentCount()==5) {
    offset=context->argument(2).toInt32();
    length=context->argument(3).toInt32();
    little_endian=(context->argument(4).toInt32()==1);
  }

  // Cast ByteArray argument to QByteArray
  key_value=qvariant_cast<QByteArray>(context->argument(0).data().toVariant());
  format=context->argument(1).toString();

  ret=RegistryHive::KeyValueToString(key_value,format,offset,length,little_endian);

  return engine->newVariant(ret);
}

/*
 * RegistryKeyValueToStringList
 */
QScriptValue ReportEngine::RegistryKeyValueToStringList(QScriptContext *context,
                                                        QScriptEngine *engine)
{
  QByteArray value;
  QStringList strings;
  QScriptValue ret;
  int i=0;
  bool little_endian=true;

  // This function needs one arguments, key value, and may have a second
  // specifying endianness
  if(context->argumentCount()==0 || context->argumentCount()>2)
    return engine->undefinedValue();
  if(context->argumentCount()==2) {
    little_endian=context->argument(2).toBool();
  }

  // Cast ByteArray argument to QByteArray and convert
  value=qvariant_cast<QByteArray>(context->argument(0).data().toVariant());
  strings=RegistryHive::KeyValueToStringList(value,little_endian);

  // Build script array
  ret=engine->newArray(strings.count());
  QListIterator<QString> str_it(strings);
  while(str_it.hasNext()) {
    ret.setProperty(i++,QScriptValue(str_it.next()));
  }

  return ret;
}

/*
 * RegistryKeyTypeToString
 */
QScriptValue ReportEngine::RegistryKeyTypeToString(QScriptContext *context,
                                                   QScriptEngine *engine)
{
  QString ret="";

  // This function needs one argument, key type
  if(context->argumentCount()!=1) return engine->undefinedValue();

  ret=RegistryHive::KeyValueTypeToString(context->argument(0).toInt32());

  return engine->newVariant(ret);
}

/*
 * GetRegistryNodeModTime
 */
QScriptValue ReportEngine::GetRegistryNodeModTime(QScriptContext *context,
                                                  QScriptEngine *engine)
{
  QScriptValue calleeData;
  RegistryHive *p_hive;
  qint64 mod_time=0;

  // This function needs one argument, node path
  if(context->argumentCount()!=1) return engine->undefinedValue();

  // Get calle data (Pointer to RegistryHive class)
  calleeData=context->callee().data();
  p_hive=qobject_cast<RegistryHive*>(calleeData.toQObject());

  mod_time=p_hive->GetNodeModTime(context->argument(0).toString());
  if(p_hive->Error()) {
    // Get error message to clear error state
    p_hive->GetErrorMsg();
    return engine->undefinedValue();
  }

  QDateTime date_time;
  date_time.setTimeSpec(Qt::UTC);
  date_time.setTime_t(RegistryHive::FiletimeToUnixtime(mod_time));

  return engine->newVariant(date_time.toString("yyyy/MM/dd hh:mm:ss"));
}

/*
 * GetReportTemplateFileContents
 */
bool ReportEngine::GetReportTemplateFileContents(QString file,
                                                 QString &contents)
{
  // Open report template file
  QFile template_file(file);
  if(!template_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    contents=QString("Couldn't open report template file '%1'!").arg(file);
    return false;
  }

  // Read template file and close it
  contents.clear();
  QTextStream in(&template_file);
  while(!in.atEnd()) contents.append(in.readLine()).append("\n");
  template_file.close();

  return true;
}

/*
 * UpdateExportedFunctions
 */
void ReportEngine::UpdateExportedFunctions(RegistryHive *p_hive) {
  this->p_registry_hive=p_hive;
  // GetRegistryNodes
  QScriptValue func_get_nodes=this->newFunction(this->GetRegistryNodes,1);
  func_get_nodes.setData(this->newQObject(this->p_registry_hive));
  this->globalObject().setProperty("GetRegistryNodes",func_get_nodes);
  // GetRegistryKeys
  QScriptValue func_get_keys=this->newFunction(this->GetRegistryKeys,1);
  func_get_keys.setData(this->newQObject(this->p_registry_hive));
  this->globalObject().setProperty("GetRegistryKeys",func_get_keys);
  // GetRegistryKeyValue
  QScriptValue func_get_key_value=this->newFunction(this->GetRegistryKeyValue,
                                                    2);
  func_get_key_value.setData(this->newQObject(this->p_registry_hive));
  this->globalObject().setProperty("GetRegistryKeyValue",func_get_key_value);
  // GetRegistryNodeModTime
  QScriptValue func_get_node_modt=
    this->newFunction(this->GetRegistryNodeModTime,1);
  func_get_node_modt.setData(this->newQObject(this->p_registry_hive));
  this->globalObject().setProperty("GetRegistryNodeModTime",func_get_node_modt);
}
