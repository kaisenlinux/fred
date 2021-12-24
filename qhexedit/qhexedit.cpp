/*******************************************************************************
* qhexedit Copyright (c) 2011-2020 by Gillen Daniel <gillen.dan@pinguin.lu>    *
*                                                                              *
* Simple hex editor widget for Qt.                                             *
*                                                                              *
* Derived from code by Simon Winfried under a compatible license:              *
* Copyright (c) 2010 by Simon Winfried                                         *
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

#include <QScrollArea>
#include "qhexedit.h"

QHexEdit::QHexEdit(QWidget *parent) : QScrollArea(parent)
{
    qHexEdit_p = new QHexEditPrivate(this);
    setWidget(qHexEdit_p);
    setWidgetResizable(true);

    connect(qHexEdit_p, SIGNAL(currentAddressChanged(int)), this, SIGNAL(currentAddressChanged(int)));
    connect(qHexEdit_p, SIGNAL(currentSizeChanged(int)), this, SIGNAL(currentSizeChanged(int)));
    connect(qHexEdit_p, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
    connect(qHexEdit_p, SIGNAL(overwriteModeChanged(bool)), this, SIGNAL(overwriteModeChanged(bool)));
}

void QHexEdit::insert(int i, const QByteArray & ba)
{
    qHexEdit_p->insert(i, ba);
}

void QHexEdit::insert(int i, char ch)
{
    qHexEdit_p->insert(i, ch);
}

void QHexEdit::remove(int pos, int len)
{
    qHexEdit_p->remove(pos, len);
}

void QHexEdit::setAddressArea(bool addressArea)
{
    qHexEdit_p->setAddressArea(addressArea);
}

void QHexEdit::setAddressWidth(int addressWidth)
{
    qHexEdit_p->setAddressWidth(addressWidth);
}

void QHexEdit::setAsciiArea(bool asciiArea)
{
    qHexEdit_p->setAsciiArea(asciiArea);
}

void QHexEdit::setHighlighting(bool mode)
{
    qHexEdit_p->setHighlighting(mode);
}

void QHexEdit::setAddressOffset(int offset)
{
    qHexEdit_p->setAddressOffset(offset);
}

int QHexEdit::addressOffset()
{
    return addressOffset();
}

void QHexEdit::setData(const QByteArray &data)
{
    qHexEdit_p->setData(data);
}

QByteArray QHexEdit::data()
{
    return qHexEdit_p->data();
}

void QHexEdit::setAddressAreaColor(const QColor &color)
{
    qHexEdit_p->setAddressAreaColor(color);
}

QColor QHexEdit::addressAreaColor()
{
    return qHexEdit_p->addressAreaColor();
}

void QHexEdit::setHighlightingColor(const QColor &color)
{
    qHexEdit_p->setHighlightingColor(color);
}

QColor QHexEdit::highlightingColor()
{
    return qHexEdit_p->highlightingColor();
}

void QHexEdit::setOverwriteMode(bool overwriteMode)
{
    qHexEdit_p->setOverwriteMode(overwriteMode);
}

bool QHexEdit::overwriteMode()
{
    return qHexEdit_p->overwriteMode();
}

void QHexEdit::setReadOnly(bool read_only) {
  qHexEdit_p->setReadOnly(read_only);
}

bool QHexEdit::readOnly() {
  return qHexEdit_p->readOnly();
}

void QHexEdit::setFont(const QFont &font)
{
    qHexEdit_p->setFont(font);
}

