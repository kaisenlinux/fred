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

#include <QFont>
#include <QSizePolicy>

#include "hexeditwidget.h"

HexEditWidget::HexEditWidget(QWidget *p_parent,
                             bool with_data_interpreter,
                             bool is_read_only) : QWidget(p_parent)
{
  // Init private vars
  this->has_data_interpreter=with_data_interpreter;
  this->read_only=is_read_only;
  this->data=QByteArray();

  // Set widget layout. Setting it's parent to "this" will also call
  // this->SetLayout.
  this->p_widget_layout=new QVBoxLayout(this);

  // Create sub-widgets
  if(this->has_data_interpreter) {
    // Widget should include a data interpreter
    this->p_widget_splitter=new QSplitter(this);
    this->p_hex_edit_layout_widget=new QWidget(this->p_widget_splitter);
    this->p_hex_edit_layout=new QVBoxLayout(this->p_hex_edit_layout_widget);
    this->p_hex_edit=new QHexEdit(this->p_hex_edit_layout_widget);
    this->p_hex_edit_status_bar=new QLabel(this);
    this->p_data_interpreter_widget=new DataInterpreterWidget(this);

    // Add hex edit and hex edit status bar to their layout
    this->p_hex_edit_layout->addWidget(this->p_hex_edit);
    this->p_hex_edit_layout->addWidget(this->p_hex_edit_status_bar);

    // Add sub-widgets to splitter and splitter to our layout
    this->p_widget_splitter->addWidget(this->p_hex_edit_layout_widget);
    this->p_widget_splitter->addWidget(this->p_data_interpreter_widget);
    this->p_widget_layout->addWidget(this->p_widget_splitter);

    // Configure sub-widgets
    this->p_widget_splitter->setOrientation(Qt::Horizontal);
    this->p_hex_edit_layout_widget->setContentsMargins(0,0,0,0);
    this->p_hex_edit_layout->setContentsMargins(0,0,0,0);

    // Set size policies of sub-widgets
    QSizePolicy hex_edit_layout_widget_policy=
      this->p_hex_edit_layout_widget->sizePolicy();
    hex_edit_layout_widget_policy.setVerticalStretch(2);
    hex_edit_layout_widget_policy.setHorizontalStretch(200);
    this->p_hex_edit_layout_widget->setSizePolicy(hex_edit_layout_widget_policy);

    QSizePolicy data_interpreter_widget_policy=
        this->p_data_interpreter_widget->sizePolicy();
    data_interpreter_widget_policy.setVerticalStretch(2);
    data_interpreter_widget_policy.setHorizontalStretch(0);
    this->p_data_interpreter_widget->
        setSizePolicy(data_interpreter_widget_policy);
  } else {
    // Widget shouldn't include a data interpreter
    this->p_hex_edit=new QHexEdit(this);
    this->p_hex_edit_status_bar=new QLabel(this);
    this->p_widget_layout->addWidget(this->p_hex_edit);
    this->p_widget_layout->addWidget(this->p_hex_edit_status_bar);
  }

  // Configure widget and sub-widgets
  this->setContentsMargins(0,0,0,0);
  this->p_widget_layout->setContentsMargins(0,0,0,0);
  this->p_hex_edit->setContentsMargins(0,0,0,0);
  // 5 pixel bottom margin makes hex edit and data interpreter lignup correctly
  this->p_hex_edit_status_bar->setContentsMargins(0,0,0,5);
  this->p_hex_edit->setReadOnly(this->read_only);
  // If we're not read only, it should also be possible to add data
  this->p_hex_edit->setOverwriteMode(this->read_only);

  // Make sure hex edit font is monospaced.
  QFont mono_font("Monospace");
  mono_font.setStyleHint(QFont::TypeWriter);
  this->p_hex_edit->setFont(mono_font);

  // Connect signals
  this->connect(this->p_hex_edit,
                SIGNAL(currentAddressChanged(int)),
                this,
                SLOT(SlotHexEditOffsetChanged(int)));
}

HexEditWidget::~HexEditWidget() {
  delete this->p_hex_edit_status_bar;
  delete this->p_hex_edit;
  if(this->has_data_interpreter) {
    delete this->p_data_interpreter_widget;
    delete this->p_hex_edit_layout;
    delete this->p_hex_edit_layout_widget;
    delete this->p_widget_splitter;
  }
  delete this->p_widget_layout;
}

void HexEditWidget::SetData(QByteArray const &new_data) {
  this->data=new_data;

  this->p_hex_edit->setData(this->data);
  if(data.size()!=0) {
    // Data set, update status bar and init data interpreter
    this->SlotHexEditOffsetChanged(0);
  } else {
    // No data set, clear status bar and data interpreter
    this->p_hex_edit_status_bar->setText("");
    this->p_data_interpreter_widget->SetData(QByteArray());
  }
}

QByteArray HexEditWidget::GetData() {
  return this->p_hex_edit->data();
}

void HexEditWidget::SlotHexEditOffsetChanged(int offset) {
  // Update hex edit status bar
  this->p_hex_edit_status_bar->setText(tr("Byte offset: 0x%1 (%2)")
                                         .arg((quint16)offset,
                                              4,
                                              16,
                                              QChar('0'))
                                         .arg(offset));
  // Update data interpreter
  if(this->has_data_interpreter) {
    this->p_data_interpreter_widget->SetData(this->data.mid(offset,8));
  }
}
