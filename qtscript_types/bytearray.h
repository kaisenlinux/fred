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

// Description: http://cs.karelia.ru/~aborod/doc/qt/script-customclass.html

#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include <QObject>
#include <QScriptClass>
#include <QScriptString>

class QScriptContext;

class ByteArray : public QObject, public QScriptClass {
    Q_OBJECT
  public:
    ByteArray(QScriptEngine *engine);
    ~ByteArray();

    QScriptValue constructor();

    QScriptValue newInstance(int size = 0);
    QScriptValue newInstance(const QByteArray &ba);

    QueryFlags queryProperty(const QScriptValue &object,
                             const QScriptString &name,
                             QueryFlags flags,
                             uint *id);
    QScriptValue property(const QScriptValue &object,
                          const QScriptString &name,
                          uint id);
    void setProperty(QScriptValue &object,
                     const QScriptString &name,
                     uint id,
                     const QScriptValue &value);
    QScriptValue::PropertyFlags propertyFlags(const QScriptValue &object,
                                              const QScriptString &name,
                                              uint id);
    QScriptClassPropertyIterator *newIterator(const QScriptValue &object);

    QString name() const;

    QScriptValue prototype() const;

  private:
    static QScriptValue construct(QScriptContext *ctx, QScriptEngine *eng);

    static QScriptValue toScriptValue(QScriptEngine *eng, const QByteArray &ba);
    static void fromScriptValue(const QScriptValue &obj, QByteArray &ba);

    void resize(QByteArray &ba, int newSize);

    QScriptString length;
    QScriptValue proto;
    QScriptValue ctor;
};

#endif // BYTEARRAY_H
