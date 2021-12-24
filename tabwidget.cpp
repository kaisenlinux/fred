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

#include <QTabBar>
#include <QPushButton>
#include <QIcon>
#include <QSize>
#include <QPoint>

#include "tabwidget.h"

TabWidget::TabWidget(QWidget *p_parent) : QTabWidget(p_parent) {}

int TabWidget::addTab(QWidget *p_widget,
                      const QString &title,
                      bool close_button)
{
  // Add tab
  int tab_index=QTabWidget::addTab(p_widget,title);

  // If desired, add a close button to the tab
  if(close_button) {
    // Create close button
    QPushButton *p_close_button=
      new QPushButton(QIcon(":/icons/close_button"),QString());
    p_close_button->setFlat(true);
    p_close_button->setIconSize(QSize(14,14));
    p_close_button->setGeometry(p_close_button->x(),p_close_button->y(),14,14);
    // Connect clicked signal
    this->connect(p_close_button,
                  SIGNAL(clicked()),
                  this,
                  SLOT(SlotCloseButtonClicked()));
    this->tabBar()->setTabButton(tab_index,QTabBar::RightSide,p_close_button);
  }

  return tab_index;
}

void TabWidget::SlotCloseButtonClicked() {
  // Find index of tab to close. The trolls do it by iterating over all tabs
  // and comparing their widget with QObject::sender().
  QPushButton *p_close_button=(QPushButton*)(QObject::sender());
  int index=this->tabBar()->tabAt(QPoint(p_close_button->x(),
                                         p_close_button->y()));
  // Emit tabCloseRequested
  emit(this->tabCloseRequested(index));
}
