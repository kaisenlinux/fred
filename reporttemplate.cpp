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

#include "reporttemplate.h"

ReportTemplate::ReportTemplate(QString report_template_file,
                               QString report_category,
                               QString report_name,
                               QString report_author,
                               QString report_desc,
                               QString report_hive)
{
  this->template_file=report_template_file;
  this->category=report_category;
  this->name=report_name;
  this->author=report_author;
  this->description=report_desc;
  this->hive=report_hive;
}

void ReportTemplate::SetFile(QString new_file) {
  this->template_file=new_file;
}

void ReportTemplate::SetCategory(QString new_category) {
  this->category=new_category;
}

void ReportTemplate::SetName(QString new_name) {
  this->name=new_name;
}

void ReportTemplate::SetAuthor(QString new_author) {
  this->author=new_author;
}

void ReportTemplate::SetDescription(QString new_desc) {
  this->description=new_desc;
}

void ReportTemplate::SetHive(QString new_hive) {
  this->hive=new_hive;
}

QString ReportTemplate::ReportTemplate::File() {
  return this->template_file;
}

QString ReportTemplate::ReportTemplate::Category() {
  return this->category;
}

QString ReportTemplate::ReportTemplate::Name() {
  return this->name;
}

QString ReportTemplate::ReportTemplate::Author() {
  return this->author;
}

QString ReportTemplate::ReportTemplate::Description() {
  return this->description;
}

QString ReportTemplate::ReportTemplate::Hive() {
  return this->hive;
}
