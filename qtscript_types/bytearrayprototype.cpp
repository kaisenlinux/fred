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

#include "bytearrayprototype.h"

Q_DECLARE_METATYPE(QByteArray*)

ByteArrayPrototype::ByteArrayPrototype(QObject *p_parent) : QObject(p_parent) {}

ByteArrayPrototype::~ByteArrayPrototype() {}

QByteArray *ByteArrayPrototype::thisByteArray() const {
  return qscriptvalue_cast<QByteArray*>(thisObject().data());
}

void ByteArrayPrototype::chop(int n) {
  thisByteArray()->chop(n);
}

bool ByteArrayPrototype::equals(const QByteArray &other) {
  return *thisByteArray()==other;
}

QByteArray ByteArrayPrototype::left(int len) const {
  return thisByteArray()->left(len);
}

QByteArray ByteArrayPrototype::mid(int pos, int len) const {
  return thisByteArray()->mid(pos,len);
}

QScriptValue ByteArrayPrototype::remove(int pos, int len) {
  thisByteArray()->remove(pos,len);
  return thisObject();
}

QScriptValue ByteArrayPrototype::append(const QByteArray &ba) {
  thisByteArray()->append(ba);
  return thisObject();
}

QScriptValue ByteArrayPrototype::appendByte(char byte) const {
  thisByteArray()->append(byte);
  return thisObject();
}

QByteArray ByteArrayPrototype::right(int len) const {
  return thisByteArray()->right(len);
}

/*
QByteArray ByteArrayPrototype::simplified() const {
  return thisByteArray()->simplified();
}
*/

int ByteArrayPrototype::size() const {
  return thisByteArray()->size();
}

QByteArray ByteArrayPrototype::toBase64() const {
  return thisByteArray()->toBase64();
}

QByteArray ByteArrayPrototype::toLower() const {
  return thisByteArray()->toLower();
}

QByteArray ByteArrayPrototype::toUpper() const {
  return thisByteArray()->toUpper();
}

QByteArray ByteArrayPrototype::trimmed() const {
  return thisByteArray()->trimmed();
}

void ByteArrayPrototype::truncate(int pos) {
  thisByteArray()->truncate(pos);
}

QString ByteArrayPrototype::toLatin1String() const {
  return QString::fromLatin1(*thisByteArray());
}

QScriptValue ByteArrayPrototype::valueOf() const {
  return thisObject().data();
}

