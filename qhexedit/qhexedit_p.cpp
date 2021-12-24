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

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QPainter>

#include "qhexedit_p.h"

const int HEXCHARS_IN_LINE = 47;
const int GAP_ADR_HEX = 10;
const int GAP_HEX_ASCII = 16;
const int BYTES_PER_LINE = 16;

QHexEditPrivate::QHexEditPrivate(QScrollArea *parent) : QWidget(parent) {
  _scrollArea = parent;
  setAddressWidth(4);
  setAddressOffset(0);
  setAddressArea(true);
  setAsciiArea(true);
  setHighlighting(true);
  setOverwriteMode(true);
  setAddressAreaColor(QColor(Qt::lightGray).lighter(110));
  setHighlightingColor(QColor(Qt::yellow).lighter(160));
  this->setReadOnly(true);
  this->sel_origin=QPoint(0,0);
  this->sel_start=QPoint(0,0);
  this->sel_end=QPoint(0,0);

  setFont(QFont("Mono", 10));
  connect(&_cursorTimer, SIGNAL(timeout()), this, SLOT(updateCursor()));

  _cursorTimer.setInterval(500);

  setFocusPolicy(Qt::StrongFocus);
  _size = -1;

  // Create context menu
  this->p_menu_copy=new QMenu(tr("Copy"),this);
  this->p_action_copy_selected_bytes=
      new QAction(tr("Selected bytes"),this->p_menu_copy);
  this->p_action_copy_selected_text_ascii=
      new QAction(tr("Selected bytes converted to ASCII"),this->p_menu_copy);
/*
  this->p_action_copy_selected_text_utf8=
      new QAction(tr("Selected text as UTF8"),this->p_menu_copy);
*/

  this->p_menu_copy->addAction(this->p_action_copy_selected_bytes);
  this->p_menu_copy->addAction(this->p_action_copy_selected_text_ascii);
/*
  this->p_menu_copy->addAction(this->p_action_copy_selected_text_utf8);
*/

  this->connect(this->p_action_copy_selected_bytes,
                SIGNAL(triggered()),
                this,
                SLOT(SlotCopySelectedBytes()));
  this->connect(this->p_action_copy_selected_text_ascii,
                SIGNAL(triggered()),
                this,
                SLOT(SlotCopySelectedTextAsAscii()));
/*
  this->connect(this->p_action_copy_selected_text_utf8,
                SIGNAL(triggered()),
                this,
                SLOT(SlotCopySelectedTextAsUtf8()));
*/
}

QHexEditPrivate::~QHexEditPrivate() {
  // Delete context menu
  delete this->p_action_copy_selected_bytes;
  delete this->p_action_copy_selected_text_ascii;
/*
  delete this->p_action_copy_selected_text_utf8;
*/
  delete this->p_menu_copy;
}

void QHexEditPrivate::setAddressOffset(int offset)
{
    _addressOffset = offset;
    adjust();
}

int QHexEditPrivate::addressOffset()
{
    return _addressOffset;
}

void QHexEditPrivate::setData(const QByteArray &data)
{
  // Delete any previous selections
  this->sel_origin.setX(0);
  this->sel_origin.setY(0);
  this->sel_start=this->sel_origin;
  this->sel_end=this->sel_origin;

  if(!data.isNull() && !data.isEmpty()) this->_cursorTimer.start();
  else this->_cursorTimer.stop();
  this->_data = data;
  this->_originalData = data;
  this->adjust();
  this->setCursorPos(0);
  this->setFocus();
}

QByteArray QHexEditPrivate::data()
{
    return _data;
}

void QHexEditPrivate::setAddressAreaColor(const QColor &color)
{
    _addressAreaColor = color;
    update();
}

QColor QHexEditPrivate::addressAreaColor()
{
    return _addressAreaColor;
}

void QHexEditPrivate::setHighlightingColor(const QColor &color)
{
    _highlightingColor = color;
    update();
}

QColor QHexEditPrivate::highlightingColor()
{
    return _highlightingColor;
}

void QHexEditPrivate::setOverwriteMode(bool overwriteMode)
{
    if (overwriteMode != _overwriteMode)
    {
        emit overwriteModeChanged(overwriteMode);
        _overwriteMode = overwriteMode;
        adjust();
    }
}

bool QHexEditPrivate::overwriteMode()
{
    return _overwriteMode;
}

void QHexEditPrivate::setReadOnly(bool read_only) {
  this->_readOnly=read_only;
}

bool QHexEditPrivate::readOnly() {
  return this->_readOnly;
}

void QHexEditPrivate::insert(int i, const QByteArray & ba)
{
    _data.insert(i, ba);
    _originalData.insert(i, ba);
}

void QHexEditPrivate::insert(int i, char ch)
{
    _data.insert(i, ch);
    _originalData.insert(i, ch);
}

void QHexEditPrivate::remove(int index, int len)
{
    _data.remove(index, len);
    _originalData.remove(index, len);
}

void QHexEditPrivate::setAddressArea(bool addressArea)
{
    _addressArea = addressArea;
    adjust();
    setCursorPos(_cursorPosition);
}

void QHexEditPrivate::setAddressWidth(int addressWidth)
{
    if ((addressWidth >= 0) and (addressWidth<=6))
    {
        _addressNumbers = addressWidth;
        adjust();
        setCursorPos(_cursorPosition);
    }
}

void QHexEditPrivate::setAsciiArea(bool asciiArea)
{
    _asciiArea = asciiArea;
    adjust();
}

void QHexEditPrivate::setFont(const QFont &font)
{
    QWidget::setFont(font);
    adjust();
}

void QHexEditPrivate::setHighlighting(bool mode)
{
    _highlighting = mode;
    update();
}

void QHexEditPrivate::keyPressEvent(QKeyEvent *event)
{
    bool down = false;
    int charX = (_cursorX - _xPosHex) / _charWidth;
    int posX = (charX / 3) * 2 + (charX % 3);
    int posBa = (_cursorY / _charHeight) * BYTES_PER_LINE + posX / 2;

    int key = int(event->text()[0].toLatin1());
    if (!this->_readOnly &&
        ((key>='0' && key<='9') || (key>='a' && key <= 'f')))
    {
        // insert char
        if (_overwriteMode == false)
            if ((charX % 3) == 0)
            {
                insert(posBa, char(0));
                adjust();
            }
        if (_data.size() > 0)
        {
            QByteArray hexValue = _data.mid(posBa, 1).toHex();
            if ((charX % 3) == 0)
                hexValue[0] = key;
            else
                hexValue[1] = key;
            _data.replace(posBa, 1, QByteArray().fromHex(hexValue));
            emit dataChanged();

            setCursorPos(_cursorPosition + 1);
            down = true;
        }
    }

    // delete char
    if (!this->_readOnly && event->matches(QKeySequence::Delete)) {
      remove(posBa);
    }
    if (!this->_readOnly && event->key() == Qt::Key_Backspace) {
      remove(posBa - 1);
      setCursorPos(_cursorPosition - 2);
    }

    // handle other function keys
    if(!this->_readOnly && event->key() == Qt::Key_Insert) {
      setOverwriteMode(!_overwriteMode);
      setCursorPos(_cursorPosition);
    }

    if (event->matches(QKeySequence::MoveToNextChar))
    {
        setCursorPos(_cursorPosition + 1);
        down = true;
    }
    if (event->matches(QKeySequence::MoveToPreviousChar))
            setCursorPos(_cursorPosition - 1);
    if (event->matches(QKeySequence::MoveToStartOfLine))
            setCursorPos(_cursorPosition - (_cursorPosition % (2 * BYTES_PER_LINE)));
    if (event->matches(QKeySequence::MoveToEndOfLine))
            setCursorPos(_cursorPosition | (2 * BYTES_PER_LINE -1));
    if (event->matches(QKeySequence::MoveToPreviousLine))
            setCursorPos(_cursorPosition - (2 * BYTES_PER_LINE));
    if (event->matches(QKeySequence::MoveToPreviousPage))
            setCursorPos(_cursorPosition - (((_scrollArea->viewport()->height() / _charHeight) - 1) * 2 * BYTES_PER_LINE));
    if (event->matches(QKeySequence::MoveToStartOfDocument))
        setCursorPos(0);
    if (event->matches(QKeySequence::MoveToNextLine))
    {
        setCursorPos(_cursorPosition + (2 * BYTES_PER_LINE));
        down = true;
    }
    if (event->matches(QKeySequence::MoveToEndOfDocument))
    {
        setCursorPos(_data.size() * 2);
        down = true;
    }
    if (event->matches(QKeySequence::MoveToNextPage))
    {
        setCursorPos(_cursorPosition + (((_scrollArea->viewport()->height() / _charHeight) - 1) * 2 * BYTES_PER_LINE));
        down = true;
    }

    // when we move downwards, we have to go a little further
    if (down)
        _scrollArea->ensureVisible(_cursorX, _cursorY, 3, 3 + _charHeight);
    else
        _scrollArea->ensureVisible(_cursorX, _cursorY, 3, 3);
    update();
}

void QHexEditPrivate::mousePressEvent(QMouseEvent *p_event) {
  if(p_event->button()==Qt::LeftButton) {
    // Init selection origin, start and end point
    this->sel_origin=p_event->pos();
    this->sel_end=this->sel_start=this->sel_origin;

    // Set cursor to current pos
    int curs_pos=this->Point2Char(this->sel_start);
    if(curs_pos!=-1) setCursorPos(curs_pos);
  } else {
    QWidget::mousePressEvent(p_event);
  }
}

void QHexEditPrivate::mouseMoveEvent(QMouseEvent *p_event) {
  if(p_event->buttons()==Qt::LeftButton) {
    // Update current selection and repaint hexedit
    if(this->Point2Char(p_event->pos())>this->Point2Char(this->sel_origin)) {
      this->sel_start=this->sel_origin;
      this->sel_end=p_event->pos();
    } else {
      this->sel_end=this->sel_origin;
      this->sel_start=p_event->pos();
    }
    this->update();
  } else {
    QWidget::mouseMoveEvent(p_event);
  }
}

void QHexEditPrivate::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  // Draw some patterns if needed
  painter.fillRect(event->rect(), this->palette().color(QPalette::Base));
  if(_addressArea) {
    painter.fillRect(QRect(_xPosAdr,
                           event->rect().top(),
                           _xPosHex-GAP_ADR_HEX+2,
                           height()),
                     _addressAreaColor);
  }
  if(_asciiArea) {
    int linePos=_xPosAscii-(GAP_HEX_ASCII / 2);
    painter.setPen(Qt::gray);
    painter.drawLine(linePos,event->rect().top(),linePos,height());
  }

  painter.setPen(this->palette().color(QPalette::WindowText));

  // Calc positions
  int firstLineIdx=
      ((event->rect().top()/_charHeight)-_charHeight)*BYTES_PER_LINE;
  if(firstLineIdx<0) firstLineIdx=0;
  int lastLineIdx=
      ((event->rect().bottom()/_charHeight)+_charHeight)*BYTES_PER_LINE;
  if(lastLineIdx>_data.size()) lastLineIdx=_data.size();
  int yPosStart=((firstLineIdx)/BYTES_PER_LINE)*_charHeight+_charHeight;

  // Paint address area
  if(_addressArea) {
    for(int lineIdx=firstLineIdx, yPos=yPosStart;
        lineIdx<lastLineIdx;
        lineIdx+=BYTES_PER_LINE, yPos+=_charHeight)
    {
      QString address=QString("%1").arg(lineIdx+_addressOffset,
                                        _realAddressNumbers,
                                        16,
                                        QChar('0'));
      painter.drawText(_xPosAdr, yPos, address);
    }
  }

  // Prepare values for a selection
  int selection_start=0,selection_end=0;
  bool selection=false;
  if(!(this->sel_start.isNull() && this->sel_end.isNull()) &&
     this->sel_start!=this->sel_end)
  {
    selection_start=this->Point2Char(this->sel_start)/2;
    selection_end=this->Point2Char(this->sel_end)/2;
    selection=true;
  }

  // Paint hex area
  QByteArray hexBa(_data.mid(firstLineIdx,lastLineIdx-firstLineIdx+1).toHex());
  QBrush highLighted=QBrush(_highlightingColor);
  painter.setBackground(highLighted);
  painter.setBackgroundMode(Qt::TransparentMode);
  for(int lineIdx=firstLineIdx, yPos=yPosStart;
      lineIdx<lastLineIdx;
      lineIdx+=BYTES_PER_LINE, yPos+=_charHeight)
  {
    QByteArray hex;
    int xPos=_xPosHex;
    for(int colIdx=0;
        ((lineIdx+colIdx)<_data.size() && (colIdx<BYTES_PER_LINE));
        colIdx++)
    {
      // Highlight diff bytes
      if(_highlighting) {
        int posBa=lineIdx+colIdx;
        if(posBa>=_originalData.size()) {
          painter.setBackgroundMode(Qt::TransparentMode);
        } else {
          if(_data[posBa]==_originalData[posBa]) {
            painter.setBackgroundMode(Qt::TransparentMode);
          } else {
            painter.setBackgroundMode(Qt::OpaqueMode);
          }
        }
      }

      // Highlight selection
      if(selection) {
        int cur_char=lineIdx+colIdx;
        if(cur_char>=selection_start && cur_char<=selection_end) {
          painter.setBackgroundMode(Qt::OpaqueMode);
        } else {
          painter.setBackgroundMode(Qt::TransparentMode);
        }
      }

      // Render hex value
      if(colIdx==0) {
        hex=hexBa.mid((lineIdx-firstLineIdx)*2,2);
        painter.drawText(xPos,yPos,hex);
        xPos+=2*_charWidth;
      } else {
        hex=hexBa.mid((lineIdx+colIdx-firstLineIdx)*2,2).prepend(" ");
        painter.drawText(xPos,yPos,hex);
        xPos+=3*_charWidth;
      }
    }
  }

  // Paint ascii area
  if(_asciiArea) {
    for(int lineIdx=firstLineIdx, yPos=yPosStart;
        lineIdx<lastLineIdx;
        lineIdx+=BYTES_PER_LINE, yPos+=_charHeight)
    {
      QByteArray ascii=_data.mid(lineIdx,BYTES_PER_LINE);
      for(int idx=0, xpos=_xPosAscii;
          idx<ascii.size();
          idx++, xpos+=_charWidth)
      {
        // Highlight selection
        if(selection) {
          int cur_char=lineIdx+idx;
          if(cur_char>=selection_start && cur_char<=selection_end) {
            painter.setBackgroundMode(Qt::OpaqueMode);
          } else {
            painter.setBackgroundMode(Qt::TransparentMode);
          }
        }

        // Render char
        if(((char)ascii[idx]<0x20) || ((char)ascii[idx]>0x7e)) {
          painter.drawText(xpos, yPos, QString("."));
        } else {
          painter.drawText(xpos, yPos, QString(ascii.at(idx)));
        }
      }
    }
  }

  // Reset painter background if it is still set from highlighting
  painter.setBackgroundMode(Qt::TransparentMode);

  // Paint cursor
  if(_blink && !this->_data.isNull() && !this->_data.isEmpty()) {
    if(_overwriteMode) {
      painter.fillRect(_cursorX,
                       _cursorY+_charHeight-2,
                       _charWidth,
                       2,
                       this->palette().color(QPalette::WindowText));
    } else {
      painter.fillRect(_cursorX,
                       _cursorY,
                       2,
                       _charHeight,
                       this->palette().color(QPalette::WindowText));
    }
  }

  if(_size!=_data.size()) {
    _size=_data.size();
    emit currentSizeChanged(_size);
  }
}

void QHexEditPrivate::setCursorPos(int position)
{
  // delete cursor
  _blink=false;
  update();

  // cursor in range?
  if(_overwriteMode) {
    if(position>(_data.size()*2-1)) position=_data.size()*2-1;
  } else {
    if(position>(_data.size()*2)) position=_data.size()*2;
  }

  if(position < 0) position=0;

  // calc position
  _cursorPosition=position;
  _cursorY=(position/(2*BYTES_PER_LINE))*_charHeight+4;
  int x=(position%(2*BYTES_PER_LINE));
  _cursorX=(((x/2)*3)+(x%2))*_charWidth+_xPosHex;

  // immiadately draw cursor
  _blink=true;
  update();
  emit currentAddressChanged(_cursorPosition/2);
}

void QHexEditPrivate::contextMenuEvent(QContextMenuEvent *p_event) {
  // Only show context menu when something is selected
  if(!(this->sel_start.isNull() && this->sel_end.isNull()) &&
     this->sel_start!=this->sel_end)
  {
    // Create context menu and add actions
    QMenu context_menu(this);
    context_menu.addMenu(this->p_menu_copy);
    context_menu.exec(p_event->globalPos());
  }
}

void QHexEditPrivate::updateCursor()
{
    if (_blink)
        _blink = false;
    else
        _blink = true;
    update(_cursorX, _cursorY, _charWidth, _charHeight);
}

void QHexEditPrivate::SlotCopySelectedBytes() {
  int selection_start=this->Point2Char(this->sel_start)/2;
  int selection_count=this->Point2Char(this->sel_end)/2;
  selection_count-=(selection_start-1);

  QByteArray hex(this->_data.mid(selection_start,selection_count).toHex());

  QApplication::clipboard()->setText(hex,QClipboard::Clipboard);
}

void QHexEditPrivate::SlotCopySelectedTextAsAscii() {
  int selection_start=this->Point2Char(this->sel_start)/2;
  int selection_count=this->Point2Char(this->sel_end)/2;
  selection_count-=(selection_start-1);

  QByteArray values(this->_data.mid(selection_start,selection_count));

  QString ascii="";
  int i=0;
  for(i=0;i<values.size();i++) {
    if(!(((char)values[i]<0x20) || ((char)values[i]>0x7e))) {
      ascii.append(values.at(i));
    }
  }

  QApplication::clipboard()->setText(ascii,QClipboard::Clipboard);
}

/*
void QHexEditPrivate::SlotCopySelectedTextAsUtf8() {
  // TODO: Implement
}
*/

void QHexEditPrivate::adjust()
{
    _charWidth = fontMetrics().width(QLatin1Char('9'));
    _charHeight = fontMetrics().height();

    // is addressNumbers wide enought?
    QString test = QString("%1")
                  .arg(_data.size() + _addressOffset, _addressNumbers, 16, QChar('0'));
    _realAddressNumbers = test.size();

    _xPosAdr = 0;
    if (_addressArea)
        _xPosHex = _realAddressNumbers *_charWidth + GAP_ADR_HEX;
    else
        _xPosHex = 0;
    _xPosAscii = _xPosHex + HEXCHARS_IN_LINE * _charWidth + GAP_HEX_ASCII;

    // tell QAbstractScollbar, how big we are
    setMinimumHeight(((_data.size()/16 + 1) * _charHeight) + 3);
    setMinimumWidth(_xPosAscii + (BYTES_PER_LINE * _charWidth));

    update();
}

int QHexEditPrivate::Point2Char(QPoint pos) {
  // find char under cursor
  if((pos.x()>=_xPosHex) && (pos.x()<(_xPosHex+HEXCHARS_IN_LINE*_charWidth))) {
    int x=(pos.x()-_xPosHex)/_charWidth;
    if((x%3)==0) x=(x/3)*2;
    else x=((x/3)*2)+1;
    int y=(pos.y()/_charHeight)*2*BYTES_PER_LINE;
    return x+y;
  }
  return -1;
}
