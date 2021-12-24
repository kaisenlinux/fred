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
#include <QApplication>
#include <QClipboard>

#include "registrykeytable.h"

/*******************************************************************************
 * Public
 ******************************************************************************/

RegistryKeyTable::RegistryKeyTable(QWidget *p_parent) : QTableView(p_parent) {
  this->is_writable=false;

  // Configure widget
  this->setSelectionMode(QAbstractItemView::SingleSelection);
  this->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->setAutoScroll(false);
  this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  this->verticalHeader()->setHidden(true);
  this->setTextElideMode(Qt::ElideNone);

  // Create context menu item
  this->p_action_add_key=new QAction(tr("Add new key"),this);
  this->p_action_edit_key=new QAction(tr("Edit selected key"),this);
  this->p_action_delete_key=new QAction(tr("Delete selected key"),this);
  this->p_menu_copy=new QMenu(tr("Copy"),this);
  this->p_action_copy_key_name=
    new QAction(tr("Selected key name"),this->p_menu_copy);
  this->p_menu_copy->addAction(this->p_action_copy_key_name);
  this->p_action_copy_key_value=
    new QAction(tr("Selected key value"),this->p_menu_copy);
  this->p_menu_copy->addAction(this->p_action_copy_key_value);

  // Connect context menu signals
  this->connect(this->p_action_add_key,
                SIGNAL(triggered()),
                this,
                SLOT(SlotAddKey()));
  this->connect(this->p_action_edit_key,
                SIGNAL(triggered()),
                this,
                SLOT(SlotEditKey()));
  this->connect(this->p_action_delete_key,
                SIGNAL(triggered()),
                this,
                SLOT(SlotDeleteKey()));
  this->connect(this->p_action_copy_key_name,
                SIGNAL(triggered()),
                this,
                SLOT(SlotCopyKeyName()));
  this->connect(this->p_action_copy_key_value,
                SIGNAL(triggered()),
                this,
                SLOT(SlotCopyKeyValue()));
}

RegistryKeyTable::~RegistryKeyTable() {
  // Delete context menu
  delete this->p_action_copy_key_name;
  delete this->p_action_copy_key_value;
  delete this->p_menu_copy;
  delete this->p_action_delete_key;
  delete this->p_action_edit_key;
  delete this->p_action_add_key;
}

void RegistryKeyTable::setModel(QAbstractItemModel *p_model, bool writable) {
  QTableView::setModel(p_model);
  // Resize table rows / columns to fit data
  this->resizeColumnsToContents();
  this->resizeRowsToContents();
  this->horizontalHeader()->stretchLastSection();
  if(p_model!=NULL && p_model->rowCount()>0) {
    // Select first table item
    this->selectRow(0);
  }

  // Set writable status
  this->SetWritable(writable);
}

void RegistryKeyTable::SetWritable(bool writable) {
  this->is_writable=writable;
  this->p_action_add_key->setEnabled(this->is_writable);
  this->p_action_edit_key->setEnabled(this->is_writable);
  this->p_action_delete_key->setEnabled(this->is_writable);
}

/*
void RegistryKeyTable::selectRow(QString key_name) {
  int i;

  this->clearSelection();
  for(i=0;i<this->model()->rowCount();i++) {
    if(this->model())
  }
}
*/

/*******************************************************************************
 * Protected
 ******************************************************************************/

int RegistryKeyTable::sizeHintForColumn(int column) const {
  int size_hint=-1;
  int i=0;
  int item_width=0;
  QFontMetrics fm(this->fontMetrics());
  QModelIndex idx;

  if(this->model()==NULL) return -1;

  // Find string that needs the most amount of space
  idx=this->model()->index(i,column);
  while(idx.isValid()) {
    item_width=fm.width(this->model()->data(idx).toString())+10;
    if(item_width>size_hint) size_hint=item_width;
    idx=this->model()->index(++i,column);
  }

  return size_hint;
}

void RegistryKeyTable::contextMenuEvent(QContextMenuEvent *p_event) {
  // Only show context menu if a hive is open (a model was set)
  if(this->model()==NULL) return;

  // Decide what menus should be enabled
  if(this->selectedIndexes().count()==3) {
    // A row is selected, enable full context menu
    this->p_action_add_key->setEnabled(this->is_writable);
    this->p_action_edit_key->setEnabled(this->is_writable);
    this->p_action_delete_key->setEnabled(this->is_writable);
    this->p_menu_copy->setEnabled(true);
  } else {
    // No row is selected, disable all menu items except AddKey
    this->p_action_add_key->setEnabled(this->is_writable);
    this->p_action_edit_key->setEnabled(false);
    this->p_action_delete_key->setEnabled(false);
    this->p_menu_copy->setEnabled(false);
  }

  // Emit clicked signal (makes sure item under cursor is selected if it wasn't)
  emit(this->clicked(this->indexAt(p_event->pos())));

  // Create context menu, add actions and show it
  QMenu context_menu(this);
  context_menu.addAction(this->p_action_add_key);
  context_menu.addAction(this->p_action_edit_key);
  context_menu.addAction(this->p_action_delete_key);
  context_menu.addSeparator();
  context_menu.addMenu(this->p_menu_copy);
  context_menu.exec(p_event->globalPos());
}

void RegistryKeyTable::currentChanged(const QModelIndex &current,
                                      const QModelIndex &previous)
{
  // Call parent class's currentChanged first
  QTableView::currentChanged(current,previous);

  // Now emit our signal
  QModelIndex current_item=QModelIndex(current);
  emit(RegistryKeyTable::CurrentItemChanged(current_item));
}

/*******************************************************************************
 * Private slots
 ******************************************************************************/

void RegistryKeyTable::SlotAddKey() {
  emit(this->SignalAddKey());
}

void RegistryKeyTable::SlotEditKey() {
  emit(this->SignalEditKey(this->selectedIndexes().at(0)));
}

void RegistryKeyTable::SlotDeleteKey() {
  emit(this->SignalDeleteKey(this->selectedIndexes().at(0)));
}

void RegistryKeyTable::SlotCopyKeyName() {
  QApplication::clipboard()->
    setText(this->selectedIndexes().at(0).data().toString(),
            QClipboard::Clipboard);
}

void RegistryKeyTable::SlotCopyKeyValue() {
  QApplication::clipboard()->
    setText(this->selectedIndexes().at(2).data().toString(),
            QClipboard::Clipboard);
}
