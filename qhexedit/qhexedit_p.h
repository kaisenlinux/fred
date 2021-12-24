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

#ifndef QHEXEDIT_P_H
#define QHEXEDIT_P_H

/** \cond docNever */

#include <QPoint>
#include <QTimer>
#include <QWidget>
#include <QScrollArea>
#include <QMenu>

class QHexEditPrivate : public QWidget {
  Q_OBJECT

  public:
    QHexEditPrivate(QScrollArea *parent);
    ~QHexEditPrivate();

    void setAddressOffset(int offset);
    int addressOffset();

    void setData(QByteArray const &data);
    QByteArray data();

    void setAddressAreaColor(QColor const &color);
    QColor addressAreaColor();

    void setHighlightingColor(QColor const &color);
    QColor highlightingColor();

    void setOverwriteMode(bool overwriteMode);
    bool overwriteMode();
    void setReadOnly(bool read_only);
    bool readOnly();

    void insert(int i, const QByteArray & ba);
    void insert(int i, char ch);
    void remove(int index, int len=1);

    void setAddressArea(bool addressArea);
    void setAddressWidth(int addressWidth);
    void setAsciiArea(bool asciiArea);
    void setHighlighting(bool mode);
    virtual void setFont(const QFont &font);

  signals:
    void currentAddressChanged(int address);
    void currentSizeChanged(int size);
    void dataChanged();
    void overwriteModeChanged(bool state);

  protected:
    void keyPressEvent(QKeyEvent * event);
    void mousePressEvent(QMouseEvent *p_event);
    void mouseMoveEvent(QMouseEvent *p_event);
    void paintEvent(QPaintEvent *event);
    void setCursorPos(int position);
    void contextMenuEvent(QContextMenuEvent *p_event);

  private slots:
    void updateCursor();
    void SlotCopySelectedBytes();
    void SlotCopySelectedTextAsAscii();
/*
    void SlotCopySelectedTextAsUtf8();
*/

  private:
    void adjust();
    int Point2Char(QPoint pos);

    QColor _addressAreaColor;
    QByteArray _data;
    QByteArray _originalData;
    QColor _highlightingColor;
    QScrollArea *_scrollArea;
    QTimer _cursorTimer;
    QPoint sel_origin;
    QPoint sel_start;
    QPoint sel_end;
    QMenu *p_menu_copy;
    QAction *p_action_copy_selected_bytes;
    QAction *p_action_copy_selected_text_ascii;
/*
    QAction *p_action_copy_selected_text_utf8;
*/

    bool _blink;
    bool _addressArea;
    bool _asciiArea;
    bool _highlighting;
    bool _overwriteMode;
    bool _readOnly;

    int _addressNumbers, _realAddressNumbers;
    int _addressOffset;
    int _charWidth, _charHeight;
    int _cursorX, _cursorY, _cursorPosition;
    int _xPosAdr, _xPosHex, _xPosAscii;
    int _size;
};

/** \endcond docNever */

#endif

