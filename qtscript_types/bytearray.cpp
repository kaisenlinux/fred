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

#include <QScriptEngine>

#include <stdlib.h>

#include "bytearray.h"
#include "bytearrayiterator.h"
#include "bytearrayprototype.h"

Q_DECLARE_METATYPE(QByteArray*)
Q_DECLARE_METATYPE(ByteArray*)

ByteArray::ByteArray(QScriptEngine *engine)
  : QObject(engine), QScriptClass(engine)
{
  qScriptRegisterMetaType<QByteArray>(engine,
                                      this->toScriptValue,
                                      this->fromScriptValue);

  this->length=engine->toStringHandle(QLatin1String("length"));

  this->proto=engine->newQObject(new ByteArrayPrototype(this),
                                 QScriptEngine::QtOwnership,
                                 QScriptEngine::SkipMethodsInEnumeration |
                                   QScriptEngine::ExcludeSuperClassMethods |
                                   QScriptEngine::ExcludeSuperClassProperties);
  QScriptValue global=engine->globalObject();
  proto.setPrototype(global.property("Object").property("prototype"));

  this->ctor=engine->newFunction(this->construct,this->proto);
  this->ctor.setData(qScriptValueFromValue(engine,this));
}

ByteArray::~ByteArray() {}

QScriptClass::QueryFlags ByteArray::queryProperty(const QScriptValue &object,
                                                  const QScriptString &name,
                                                  QueryFlags flags,
                                                  uint *id)
{
  QByteArray *ba=qscriptvalue_cast<QByteArray*>(object.data());

  if(!ba) return 0;
  if(name!=this->length) {
    bool is_array_index;
    qint32 pos=name.toArrayIndex(&is_array_index);
    if(!is_array_index) return 0;
    *id=pos;
    if((flags & HandlesReadAccess) && (pos>=ba->size()))
      flags &= ~HandlesReadAccess;
  }

  return flags;
}

QScriptValue ByteArray::property(const QScriptValue &object,
                                 const QScriptString &name,
                                 uint id)
{
  QByteArray *ba=qscriptvalue_cast<QByteArray*>(object.data());
  if(!ba) return QScriptValue();
  if(name==length) return ba->length();
  else {
    qint32 pos=id;
    if((pos < 0) || (pos >= ba->size())) return QScriptValue();
    return uint(ba->at(pos)) & 255;
  }

  return QScriptValue();
}

void ByteArray::setProperty(QScriptValue &object,
                            const QScriptString &name,
                            uint id,
                            const QScriptValue &value)
{
  QByteArray *ba=qscriptvalue_cast<QByteArray*>(object.data());
  if(!ba) return;
  if(name==length) this->resize(*ba,value.toInt32());
  else {
    qint32 pos=id;
    if(pos<0) return;
    if(ba->size()<=pos) this->resize(*ba,pos + 1);
    (*ba)[pos]=char(value.toInt32());
  }
}

QScriptValue::PropertyFlags ByteArray::propertyFlags(const QScriptValue &object,
                                                     const QScriptString &name,
                                                     uint id)
{
  Q_UNUSED(object);
  Q_UNUSED(id);

  if(name==length) {
    return QScriptValue::Undeletable | QScriptValue::SkipInEnumeration;
  }
  return QScriptValue::Undeletable;
}

QScriptClassPropertyIterator *ByteArray::newIterator(const QScriptValue &object)
{
  return new ByteArrayIterator(object);
}

QString ByteArray::name() const {
  return QLatin1String("ByteArray");
}

QScriptValue ByteArray::prototype() const {
  return proto;
}

QScriptValue ByteArray::constructor() {
  return ctor;
}

QScriptValue ByteArray::newInstance(int size) {
#if QT_VERSION >= 0x040700
  this->engine()->reportAdditionalMemoryCost(size);
#endif
  return newInstance(QByteArray(size,0));
}

QScriptValue ByteArray::newInstance(const QByteArray &ba) {
  QScriptValue data=engine()->newVariant(qVariantFromValue(ba));
  return engine()->newObject(this,data);
}

QScriptValue ByteArray::construct(QScriptContext *ctx, QScriptEngine *) {
  ByteArray *cls=qscriptvalue_cast<ByteArray*>(ctx->callee().data());
  if(!cls) return QScriptValue();
  QScriptValue arg=ctx->argument(0);
  if(arg.instanceOf(ctx->callee()))
    return cls->newInstance(qscriptvalue_cast<QByteArray>(arg));
  int size=arg.toInt32();
  return cls->newInstance(size);
}

QScriptValue ByteArray::toScriptValue(QScriptEngine *eng, const QByteArray &ba)
{
  QScriptValue ctor=eng->globalObject().property("ByteArray");
  ByteArray *cls=qscriptvalue_cast<ByteArray*>(ctor.data());
  if(!cls) return eng->newVariant(qVariantFromValue(ba));
  return cls->newInstance(ba);
}

void ByteArray::fromScriptValue(const QScriptValue &obj, QByteArray &ba) {
  ba=qvariant_cast<QByteArray>(obj.data().toVariant());
}

void ByteArray::resize(QByteArray &ba, int newSize) {
  int oldSize=ba.size();
  ba.resize(newSize);
#if QT_VERSION >= 0x040700
  if(newSize>oldSize)
    this->engine()->reportAdditionalMemoryCost(newSize-oldSize);
#endif
 }
