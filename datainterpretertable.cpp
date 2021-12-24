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

#include <QHeaderView>
#include <QFontMetrics>
#include <QMenu>
#include <QApplication>
#include <QClipboard>

#include "datainterpretertable.h"

DataInterpreterTable::DataInterpreterTable(QWidget *p_parent)
  : QTableWidget(p_parent)
{
  this->setColumnCount(2);
  this->setTextElideMode(Qt::ElideNone);
  this->horizontalHeader()->setHidden(true);
  this->verticalHeader()->setHidden(true);
  this->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->setSelectionMode(QAbstractItemView::SingleSelection);

  // Create context menu actions
  this->p_action_copy_value=new QAction(tr("Copy value"),this);
  this->connect(this->p_action_copy_value,
                SIGNAL(triggered()),
                this,
                SLOT(SlotCopyValue()));
}

DataInterpreterTable::~DataInterpreterTable() {
  // Free table widget items
  this->ClearValues();

  // Delete context menu actions
  delete this->p_action_copy_value;
}

void DataInterpreterTable::AddValue(QString name, QString value) {
  QTableWidgetItem *p_name_item=new QTableWidgetItem(name);
  p_name_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  QTableWidgetItem *p_value_item=new QTableWidgetItem(value);
  p_value_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  this->setRowCount(this->rowCount()+1);
  this->setItem(this->rowCount()-1,0,p_name_item);
  this->setItem(this->rowCount()-1,1,p_value_item);
  this->resizeColumnsToContents();
  this->resizeRowsToContents();
}

void DataInterpreterTable::ClearValues() {
  // Free all items
  while(this->rowCount()>0) {
    delete this->item(0,0);
    delete this->item(0,1);
    this->setRowCount(this->rowCount()-1);
  }
}

int DataInterpreterTable::sizeHintForColumn(int column) const {
  int size_hint=0;
  int i=0;
  int item_width=0;
  QFontMetrics fm(this->fontMetrics());

  // Find string that needs the most amount of space
  for(i=0;i<this->rowCount();i++) {
    item_width=fm.width(this->item(i,column)->text())+10;
    if(item_width>size_hint) size_hint=item_width;
  }

  return size_hint;
}

void DataInterpreterTable::contextMenuEvent(QContextMenuEvent *p_event) {
  // Only show context menu when a node is selected
  if(this->selectedIndexes().count()!=2) return;
  // Only show context menu when user clicked on selected row
  if(!(this->indexAt(p_event->pos())==this->selectedIndexes().at(0) ||
       this->indexAt(p_event->pos())==this->selectedIndexes().at(1)))
  {
    return;
  }

  // Create context menu and add actions
  QMenu context_menu(this);
  context_menu.addAction(this->p_action_copy_value);
  context_menu.exec(p_event->globalPos());
}

void DataInterpreterTable::SlotCopyValue() {
  QApplication::clipboard()->
    setText(this->selectedIndexes().at(1).data().toString(),
            QClipboard::Clipboard);
}
