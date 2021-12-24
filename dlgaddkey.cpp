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

#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QRegExp>
#include <QtEndian>

#include <QDebug>

#include <stdlib.h>

#include "dlgaddkey.h"
#include "ui_dlgaddkey.h"

#include "registryhive.h"

DlgAddKey::DlgAddKey(QWidget *p_parent,
                     QString key_name,
                     QString key_value_type,
                     QByteArray key_value) :
  QDialog(p_parent),
  ui(new Ui::DlgAddKey)
{
  this->p_current_widget=NULL;
  ui->setupUi(this);
  this->ansi_encoded=false;

  // Create widgets
  this->CreateValueWidgets();

  // Set dialog title
  if(key_name.isEmpty() && key_value_type.isEmpty() && key_value.isEmpty()) {
    // If no values were passed, we consider this the ddd key dialog
    this->setWindowTitle(tr("Add key"));
  } else {
    // If a value was passed, this is considered the edit key dialog
    this->setWindowTitle(tr("Edit key"));
    this->ui->EdtKeyName->setEnabled(false);
    this->ui->CmbKeyType->setEnabled(false);
  }

  // Preload key value type values
  QStringList value_types=RegistryHive::GetKeyValueTypes();
  this->ui->CmbKeyType->addItems(value_types);

  // Load values
  if(!key_name.isEmpty()) this->ui->EdtKeyName->setText(key_name);
  if(!key_value_type.isEmpty())
    this->ui->CmbKeyType->setCurrentIndex(value_types.indexOf(key_value_type));
  if(!key_value.isEmpty()) this->SetValueWidgetData(key_value,key_value_type);

  // Connect signals
  this->connect(this->p_number_widget_rb_dec,
                SIGNAL(clicked(bool)),
                this,
                SLOT(SlotNumberWidgetRbDecClicked(bool)));
  this->connect(this->p_number_widget_rb_hex,
                SIGNAL(clicked(bool)),
                this,
                SLOT(SlotNumberWidgetRbHexClicked(bool)));
}

DlgAddKey::~DlgAddKey() {
  this->DestroyValueWidgets();
  delete ui;
}

QString DlgAddKey::KeyName() {
  return this->ui->EdtKeyName->text();
}

QString DlgAddKey::KeyType() {
  return this->ui->CmbKeyType->currentText();
}

QByteArray DlgAddKey::KeyValue() {
  return this->GetValueWidgetData();
}

void DlgAddKey::on_BtnCancel_clicked() {
  this->reject();
}

void DlgAddKey::on_BtnOk_clicked() {
  QString key_value_type=this->KeyType();

  // Check entered data for correctness
  if(key_value_type=="REG_MULTI_SZ") {
    // REG_MULTI_SZ's can't contain empty sub-strings
    QString cur_data=this->p_text_widget_text_edit->toPlainText();
    QString new_data=cur_data;
    // TODO: Do we need to check for \r\n on Windows??
    new_data.replace(QRegExp("\n\n*"),"\n");
    if(new_data.startsWith("\n")) new_data.remove(0,1);
    if(new_data.endsWith("\n")) new_data.chop(1);
    if(cur_data!=new_data) {
      if(QMessageBox::information(this,
                                  tr("Invalid data"),
                                  tr("A REG_MULTI_SZ can not contain empty "
                                       "sub-strings! If you continue, they "
                                       "will be removed."),
                                  QMessageBox::Yes,
                                  QMessageBox::No)==QMessageBox::Yes)
      {
        this->p_text_widget_text_edit->setPlainText(new_data);
      } else {
        return;
      }
    }
  } else if(key_value_type=="REG_DWORD" ||
            key_value_type=="REG_DWORD_BIG_ENDIAN")
  {
    bool ok=false;
    if(this->p_number_widget_rb_dec->isChecked()) {
      this->p_number_widget_line_edit->text().toUInt(&ok);
    } else {
      this->p_number_widget_line_edit->text().toUInt(&ok,16);
    }
    if(!ok) {
      QMessageBox::information(this,
                               tr("Invalid data"),
                               tr("The value you entered could not be "
                                    "converted to a %1! Please correct it.")
                                 .arg(key_value_type),
                               QMessageBox::Ok);
      return;
    }
  } else if(key_value_type=="REG_QWORD") {
    bool ok=false;
    if(this->p_number_widget_rb_dec->isChecked()) {
      this->p_number_widget_line_edit->text().toULongLong(&ok);
    } else {
      this->p_number_widget_line_edit->text().toULongLong(&ok,16);
    }
    if(!ok) {
      QMessageBox::information(this,
                               tr("Invalid data"),
                               tr("The value you entered could not be "
                                    "converted to a %1! Please correct it.")
                                 .arg(key_value_type),
                               QMessageBox::Ok);
      return;
    }
  }

  this->accept();
}

void DlgAddKey::on_CmbKeyType_currentIndexChanged(const QString &arg1) {
  // Remove current widget from grid layout
  if(this->p_current_widget!=NULL) {
    this->ui->gridLayout->removeWidget(this->p_current_widget);
    this->p_current_widget->setVisible(false);
    this->p_current_widget=NULL;
  }

  // Add new widget for selected value type
  if(arg1=="REG_SZ" || arg1=="REG_EXPAND_SZ") {
    // Line edit widget for REG_SZ and REG_EXPAND_SZ
    this->ui->gridLayout->addWidget(this->p_line_widget,2,1);
    this->p_current_widget=this->p_line_widget;
  } else if(arg1=="REG_MULTI_SZ") {
    // Text edit widget for REG_MULTI_SZ
    this->ui->gridLayout->addWidget(this->p_text_widget,2,1);
    this->p_current_widget=this->p_text_widget;
  } else if(arg1=="REG_DWORD" ||
            arg1=="REG_DWORD_BIG_ENDIAN" ||
            arg1=="REG_QWORD")
  {
    // Number widget for REG_DWORD, REG_DWORD_BIG_ENDIAN and REG_QWORD
    this->ui->gridLayout->addWidget(this->p_number_widget,2,1);
    this->p_current_widget=this->p_number_widget;
  } else if(arg1=="REG_BINARY" ||
            arg1=="REG_LINK" ||
            arg1=="REG_RESOURCE_LIST" ||
            arg1=="REG_FULL_RESOURCE_DESC" ||
            arg1=="REG_RESOURCE_REQ_LIST")
  {
    // Binary widget for all other types
    this->ui->gridLayout->addWidget(this->p_binary_widget,2,1);
    this->p_current_widget=this->p_binary_widget;
  }

  if(arg1!="REG_NONE") {
    this->p_current_widget->setVisible(true);
    this->ui->LblKeyValue->setVisible(true);
  } else {
    this->ui->LblKeyValue->setVisible(false);
  }
}

void DlgAddKey::SlotNumberWidgetRbDecClicked(bool checked) {
  if(checked) {
    bool ok=false;
    if(this->KeyType()=="REG_QWORD") {
      quint64 num=this->p_number_widget_line_edit->text().toULongLong(&ok,16);
      if(ok) {
        // Convert number to dec
        this->p_number_widget_line_edit->setText(QString("%1").arg(num));
      }
    } else {
      quint32 num=this->p_number_widget_line_edit->text().toUInt(&ok,16);
      if(ok) {
        // Convert number to dec
        this->p_number_widget_line_edit->setText(QString("%1").arg(num));
      }
    }
    if(!ok) {
      QMessageBox::warning(this,
                           tr("Errror"),
                           tr("Unable to convert entered value to decimal!"),
                           QMessageBox::Ok);
      this->p_number_widget_rb_hex->setChecked(true);
    }
  }
}

void DlgAddKey::SlotNumberWidgetRbHexClicked(bool checked) {
  if(checked) {
    bool ok=false;
    if(this->KeyType()=="REG_QWORD") {
      quint64 num=this->p_number_widget_line_edit->text().toULongLong(&ok);
      if(ok) {
        // Convert number to hex
        this->p_number_widget_line_edit->setText(QString("%1")
                                                   .arg(num,16,16,QChar('0')));
      }
    } else {
      quint32 num=this->p_number_widget_line_edit->text().toUInt(&ok);
      if(ok) {
        // Convert number to hex
        this->p_number_widget_line_edit->setText(QString("%1")
                                                   .arg(num,8,16,QChar('0')));
      }
    }
    if(!ok) {
      QMessageBox::warning(this,
                           tr("Errror"),
                           tr("Unable to convert entered value to hex!"),
                           QMessageBox::Ok);
      this->p_number_widget_rb_dec->setChecked(true);
    }
  }
}

void DlgAddKey::CreateValueWidgets() {
  this->p_line_widget=new QWidget();
  this->p_line_widget_layout=new QHBoxLayout(this->p_line_widget);
  //this->p_line_widget_layout_rb_ansi=new QRadioButton(tr("Ansi"));
  //this->p_line_widget_layout_rb_unicode=new QRadioButton(tr("Unicode"));
  //this->p_line_widget_layout_rb_unicode->setChecked(true);
  this->p_line_widget_line_edit=new QLineEdit();
  this->p_line_widget->setContentsMargins(0,0,0,0);
  this->p_line_widget_layout->setContentsMargins(0,0,0,0);
  this->p_line_widget_layout->addWidget(this->p_line_widget_line_edit);
  //this->p_line_widget_layout->addWidget(this->p_line_widget_layout_rb_ansi);
  //this->p_line_widget_layout->addWidget(this->p_line_widget_layout_rb_unicode);

  this->p_text_widget=new QWidget();
  this->p_text_widget_layout=new QHBoxLayout(this->p_text_widget);
  this->p_text_widget_text_edit=new QPlainTextEdit();
  this->p_text_widget->setContentsMargins(0,0,0,0);
  this->p_text_widget_layout->setContentsMargins(0,0,0,0);
  this->p_text_widget_layout->addWidget(this->p_text_widget_text_edit);

  this->p_number_widget=new QWidget();
  this->p_number_widget_layout=new QHBoxLayout(this->p_number_widget);
  this->p_number_widget_line_edit=new QLineEdit();
  this->p_number_widget_rb_dec=new QRadioButton(tr("Dec base"));
  this->p_number_widget_rb_dec->setChecked(true);
  this->p_number_widget_rb_hex=new QRadioButton(tr("Hex base"));
  this->p_number_widget->setContentsMargins(0,0,0,0);
  this->p_number_widget_layout->setContentsMargins(0,0,0,0);
  this->p_number_widget_layout->addWidget(this->p_number_widget_line_edit);
  this->p_number_widget_layout->addWidget(this->p_number_widget_rb_dec);
  this->p_number_widget_layout->addWidget(this->p_number_widget_rb_hex);

  this->p_binary_widget=new QWidget();
  this->p_binary_widget_layout=new QHBoxLayout(this->p_binary_widget);
  this->p_binary_widget_hex_edit=new HexEditWidget(this,false,false);
  this->p_binary_widget->setContentsMargins(0,0,0,0);
  this->p_binary_widget_layout->setContentsMargins(0,0,0,0);
  this->p_binary_widget_layout->addWidget(this->p_binary_widget_hex_edit);
}

void DlgAddKey::DestroyValueWidgets() {
  //delete this->p_line_widget_layout_rb_ansi;
  //delete this->p_line_widget_layout_rb_unicode;
  delete this->p_line_widget_line_edit;
  delete this->p_line_widget_layout;
  delete this->p_line_widget;

  delete this->p_text_widget_text_edit;
  delete this->p_text_widget_layout;
  delete this->p_text_widget;

  delete this->p_number_widget_rb_hex;
  delete this->p_number_widget_rb_dec;
  delete this->p_number_widget_line_edit;
  delete this->p_number_widget_layout;
  delete this->p_number_widget;

  delete this->p_binary_widget_hex_edit;
  delete this->p_binary_widget_layout;
  delete this->p_binary_widget;
}

void DlgAddKey::SetValueWidgetData(QByteArray &key_value,
                                   QString &key_value_type)
{
  if(key_value_type=="REG_SZ" || key_value_type=="REG_EXPAND_SZ") {
    this->p_line_widget_line_edit->setText(
      RegistryHive::KeyValueToString(key_value,
                                     RegistryHive::StringToKeyValueType(
                                       key_value_type)));
  } else if(key_value_type=="REG_MULTI_SZ") {
    QStringList strings=
      RegistryHive::KeyValueToStringList(key_value,&(this->ansi_encoded));
    this->p_text_widget_text_edit->setPlainText(strings.join("\n"));
  } else if(key_value_type=="REG_DWORD") {
    this->p_number_widget_line_edit->setText(
      RegistryHive::KeyValueToString(key_value,"uint32"));
  } else if(key_value_type=="REG_DWORD_BIG_ENDIAN") {
    this->p_number_widget_line_edit->setText(
      RegistryHive::KeyValueToString(key_value,"uint32",0,0,false));
  } else if(key_value_type=="REG_QWORD") {
    this->p_number_widget_line_edit->setText(
      RegistryHive::KeyValueToString(key_value,"uint64",0,0,false));
  } else if(key_value_type=="REG_BINARY" ||
            key_value_type=="REG_LINK" ||
            key_value_type=="REG_RESOURCE_LIST" ||
            key_value_type=="REG_FULL_RESOURCE_DESC" ||
            key_value_type=="REG_RESOURCE_REQ_LIST")
  {
    this->p_binary_widget_hex_edit->SetData(key_value);
  }
}

QByteArray DlgAddKey::GetValueWidgetData() {
  QString key_value_type=this->KeyType();

  if(key_value_type=="REG_SZ" || key_value_type=="REG_EXPAND_SZ") {
    // TODO: Wouldn't it be wise to let the user choose the encoding?
    // Get data
    QString data=this->p_line_widget_line_edit->text();
    // Convert data to UTF16LE buffer
    quint16 *p_buf=NULL;
    int buf_len=this->ToUtf16LeBuf(&p_buf,data.utf16(),data.size());
    if(p_buf==NULL || buf_len==0) {
      // TODO: Inform user there was an error???
      return QByteArray("\x00\x00",2);
    }
    // Construct ByteArray, free buffer and return
    QByteArray ret=QByteArray((char*)p_buf,buf_len);
    free(p_buf);
    return ret;
  } else if(key_value_type=="REG_MULTI_SZ") {
    // TODO: Let the user choose the encoding / endianness
    // TODO: Do we need to check for \r\n on Windows??
    return RegistryHive::StringListToKeyValue(
      this->p_text_widget_text_edit->
        toPlainText().split("\n",
                            QString::SkipEmptyParts),
      true,
      this->ansi_encoded);
  } else if(key_value_type=="REG_DWORD" ||
            key_value_type=="REG_DWORD_BIG_ENDIAN")
  {
    quint32 val;
    if(this->p_number_widget_rb_dec->isChecked()) {
      val=this->p_number_widget_line_edit->text().toUInt();
    } else {
      val=this->p_number_widget_line_edit->text().toUInt(0,16);
    }
    if(key_value_type=="REG_DWORD") val=qToLittleEndian(val);
    else val=qToBigEndian(val);
    return QByteArray((char*)&val,4);
  } else if(key_value_type=="REG_QWORD") {
    quint64 val;
    if(this->p_number_widget_rb_dec->isChecked()) {
      val=this->p_number_widget_line_edit->text().toULongLong();
    } else {
      val=this->p_number_widget_line_edit->text().toULongLong(0,16);
    }
    val=qToLittleEndian(val);
    return QByteArray((char*)&val,8);
  } else if(key_value_type=="REG_BINARY" ||
            key_value_type=="REG_LINK" ||
            key_value_type=="REG_RESOURCE_LIST" ||
            key_value_type=="REG_FULL_RESOURCE_DESC" ||
            key_value_type=="REG_RESOURCE_REQ_LIST")
  {
    return this->p_binary_widget_hex_edit->GetData();
  }
  return QByteArray();
}

int DlgAddKey::ToUtf16LeBuf(quint16 **pp_buf,
                            const quint16 *p_data,
                            int ascii_len)
{
  // Calculate utf16 buffer size
  // TODO: This fails if there are chars that need more than 16bit!!
  int buf_len=(ascii_len*2)+2;

  // Alloc buffer and set to 0x00h
  *pp_buf=(quint16*)malloc(buf_len);
  if(*pp_buf==NULL) return 0;
  memset(*pp_buf,0,buf_len);
  if(ascii_len==0) {
    // Empty string, we're done (buffer contains \0\0)
    return buf_len;
  }
  // Fill buffer with UTF16 string (ignoring \0\0 at the end)
  memcpy(*pp_buf,p_data,buf_len-2);
  // Make sure endianness is LE
  for(int i=0;i<ascii_len;i++) {
    (*pp_buf)[i]=qToLittleEndian((*pp_buf)[i]);
  }

  return buf_len;
}
