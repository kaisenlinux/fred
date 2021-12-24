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

#include "datainterpreterwidget.h"
#include "registryhive.h"

DataInterpreterWidget::DataInterpreterWidget(QWidget *p_parent) :
  QWidget(p_parent)
{
  // Init private vars
  this->data=QByteArray();
  this->endianness=DataInterpreterWidget::Endianness_LittleEndian;

  // Set widget layout. Setting it's parent to "this" will also call
  // this->SetLayout.
  this->p_widget_layout=new QVBoxLayout(this);

  // Create sub-widgets
  this->p_data_interpreter_table=new DataInterpreterTable();
  this->p_endianness_selector_layout=new QHBoxLayout();
  this->p_endianness_selector_le=new QRadioButton(tr("Little endian"));
  this->p_endianness_selector_be=new QRadioButton(tr("Big endian"));

  // Add endianness selector buttons to their layout
  this->p_endianness_selector_layout->addWidget(this->p_endianness_selector_le);
  this->p_endianness_selector_layout->addWidget(this->p_endianness_selector_be);

  // Add sub-widgets to our layout
  this->p_widget_layout->addWidget(this->p_data_interpreter_table);
  this->p_widget_layout->addLayout(this->p_endianness_selector_layout);

  // Configure widget and sub-widgets
  this->setContentsMargins(0,0,0,0);
  this->p_widget_layout->setContentsMargins(0,0,0,0);
  this->p_endianness_selector_layout->setContentsMargins(0,0,0,0);
  this->p_endianness_selector_le->setContentsMargins(0,0,0,0);
  this->p_endianness_selector_be->setContentsMargins(0,0,0,0);

  // Set initial endianness selector state
  this->p_endianness_selector_le->setChecked(
    (this->endianness==DataInterpreterWidget::Endianness_LittleEndian));
  this->p_endianness_selector_be->setChecked(
    (this->endianness==DataInterpreterWidget::Endianness_BigEndian));

  // Connect signals
  this->connect(this->p_endianness_selector_le,
                SIGNAL(clicked(bool)),
                this,
                SLOT(SlotEndiannessSelectorLeClicked(bool)));
  this->connect(this->p_endianness_selector_be,
                SIGNAL(clicked(bool)),
                this,
                SLOT(SlotEndiannessSelectorBeClicked(bool)));
}

DataInterpreterWidget::~DataInterpreterWidget() {
  delete this->p_endianness_selector_le;
  delete this->p_endianness_selector_be;
  delete this->p_endianness_selector_layout;
  delete this->p_data_interpreter_table;
  delete this->p_widget_layout;
}

void DataInterpreterWidget::SetData(QByteArray new_data) {
  // Save new data and interpret it
  this->data=new_data;
  this->InterpretData();
}

void DataInterpreterWidget::InterpretData() {
  // Get data length
  int data_length=this->data.size();

  // Remove old values from data interpreter table
  this->p_data_interpreter_table->ClearValues();

  if(data_length>=1) {
    this->p_data_interpreter_table->AddValue("int8:",
                                       RegistryHive::KeyValueToString(
                                         this->data,
                                         "int8"));
    this->p_data_interpreter_table->AddValue("uint8:",
                                       RegistryHive::KeyValueToString(
                                         this->data,
                                         "uint8"));
  }
  if(data_length>=2) {
    this->p_data_interpreter_table->AddValue(
      "int16:",
      RegistryHive::KeyValueToString(
        this->data,
        "int16",
        0,
        0,
        this->endianness==DataInterpreterWidget::Endianness_LittleEndian));
    this->p_data_interpreter_table->AddValue(
      "uint16:",
      RegistryHive::KeyValueToString(
        this->data,
        "uint16",
        0,
        0,
        this->endianness==DataInterpreterWidget::Endianness_LittleEndian));
  }
  if(data_length>=4) {
    this->p_data_interpreter_table->AddValue(
      "int32:",
      RegistryHive::KeyValueToString(
        this->data,
        "int32",
        0,
        0,
        this->endianness==DataInterpreterWidget::Endianness_LittleEndian));
    this->p_data_interpreter_table->AddValue(
      "uint32:",
      RegistryHive::KeyValueToString(
        this->data,
        "uint32",
        0,
        0,
        this->endianness==DataInterpreterWidget::Endianness_LittleEndian));
    this->p_data_interpreter_table->AddValue(
      "unixtime:",
      RegistryHive::KeyValueToString(
        this->data,
        "unixtime",
        0,
        0,
        this->endianness==DataInterpreterWidget::Endianness_LittleEndian));
  }
  if(data_length>=8) {
    this->p_data_interpreter_table->AddValue(
      "int64:",
      RegistryHive::KeyValueToString(
        this->data,
        "int64",
        0,
        0,
        this->endianness==DataInterpreterWidget::Endianness_LittleEndian));
    this->p_data_interpreter_table->AddValue(
      "uint64:",
      RegistryHive::KeyValueToString(
        this->data,
        "uint64",
        0,
        0,
        this->endianness==DataInterpreterWidget::Endianness_LittleEndian));
/*
    TODO: Check how one could implement this
    this->p_data_interpreter_table->AddValue(
      "unixtime64:",
      RegistryHive::KeyValueToString(
        this->data,
        "unixtime64",
        0,
        0,
        this->endianness==DataInterpreterWidget::Endianness_LittleEndian));
*/
    this->p_data_interpreter_table->AddValue(
      "filetime64:",
      RegistryHive::KeyValueToString(
        this->data,
        "filetime",
        0,
        0,
        this->endianness==DataInterpreterWidget::Endianness_LittleEndian));
  }
}

void DataInterpreterWidget::SlotEndiannessSelectorLeClicked(bool checked) {
  if(checked) {
    // Save selected endianness and update interpreted values
    this->endianness=DataInterpreterWidget::Endianness_LittleEndian;
    this->InterpretData();
  }
}

void DataInterpreterWidget::SlotEndiannessSelectorBeClicked(bool checked) {
  if(checked) {
    // Save selected endianness and update interpreted values
    this->endianness=DataInterpreterWidget::Endianness_BigEndian;
    this->InterpretData();
  }
}
