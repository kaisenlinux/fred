/*******************************************************************************
* Copyright (c) 2011-2020 by Gillen Daniel <gillen.dan@pinguin.lu>             *
*                                                                              *
* Derived from code by Nokia Corporation and/or its subsidiary(-ies) under a   *
* compatible license:                                                          *
*                                                                              *
* Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).            *
* All rights reserved.                                                         *
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

#include <QtScript/QScriptEngine>

#include "bytearrayiterator.h"

Q_DECLARE_METATYPE(QByteArray*)

ByteArrayIterator::ByteArrayIterator(const QScriptValue &object)
  : QScriptClassPropertyIterator(object)
{
  toFront();
}

ByteArrayIterator::~ByteArrayIterator() {}

bool ByteArrayIterator::hasNext() const {
  QByteArray *ba=qscriptvalue_cast<QByteArray*>(object().data());
  return m_index<ba->size();
}

void ByteArrayIterator::next() {
  m_last=m_index;
  ++m_index;
}

bool ByteArrayIterator::hasPrevious() const {
  return(m_index>0);
}

void ByteArrayIterator::previous() {
  --m_index;
  m_last=m_index;
}

void ByteArrayIterator::toFront() {
  m_index=0;
  m_last=-1;
}

void ByteArrayIterator::toBack() {
  QByteArray *ba=qscriptvalue_cast<QByteArray*>(object().data());
  m_index=ba->size();
  m_last=-1;
}

QScriptString ByteArrayIterator::name() const {
  return this->object().engine()->toStringHandle(QString::number(this->m_last));
}

uint ByteArrayIterator::id() const {
  return m_last;
}
