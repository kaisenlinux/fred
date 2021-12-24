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

#ifndef BYTEARRAYPROTOTYPE_H
#define BYTEARRAYPROTOTYPE_H

#include <QByteArray>
#include <QObject>
#include <QtScript/QScriptable>
#include <QtScript/QScriptValue>

class ByteArrayPrototype : public QObject, public QScriptable {
  Q_OBJECT

  public:
    ByteArrayPrototype(QObject *p_parent=0);
    ~ByteArrayPrototype();

  public slots:
    void chop(int n);
    bool equals(const QByteArray &other);
    QByteArray left(int len) const;
    QByteArray mid(int pos, int len = -1) const;
    QScriptValue remove(int pos, int len);
    QScriptValue append(const QByteArray &ba);
    QScriptValue appendByte(char byte) const;
    QByteArray right(int len) const;
    //QByteArray simplified() const;
    int size() const;
    QByteArray toBase64() const;
    QByteArray toLower() const;
    QByteArray toUpper() const;
    QByteArray trimmed() const;
    void truncate(int pos);
    QString toLatin1String() const;
    QScriptValue valueOf() const;

  private:
    QByteArray *thisByteArray() const;
};

#endif // BYTEARRAYPROTOTYPE_H

