#*******************************************************************************
# fred Copyright (c) 2011-2020 by Gillen Daniel <gillen.dan@pinguin.lu>        *
#                                                                              *
# Forensic Registry EDitor (fred) is a cross-platform M$ registry hive editor  *
# with special feautures useful during forensic analysis.                      *
#                                                                              *
# This program is free software: you can redistribute it and/or modify it      *
# under the terms of the GNU General Public License as published by the Free   *
# Software Foundation, either version 3 of the License, or (at your option)    *
# any later version.                                                           *
#                                                                              *
# This program is distributed in the hope that it will be useful, but WITHOUT  *
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for     *
# more details.                                                                *
#                                                                              *
# You should have received a copy of the GNU General Public License along with *
# this program. If not, see <http://www.gnu.org/licenses/>.                    *
#******************************************************************************/

# Generate compileinfo.h
system(bash compileinfo.sh > compileinfo.h)
#compileinfo.target   = compileinfo.h
#compileinfo.commands = $$PWD/compileinfo.sh > compileinfo.h
#QMAKE_EXTRA_TARGETS += compileinfo
#PRE_TARGETDEPS      += compileinfo.h

# Check command line args
!isEmpty(HIVEX_STATIC) {
  DEFINES       += "HIVEX_STATIC"
}

# Configure fred
QMAKE_CXXFLAGS  += -Wall

QT              += core \
                   widgets \
                   printsupport \
                   script \
                   webenginewidgets

CONFIG          += console

TARGET           = fred

TEMPLATE         = app

SOURCES         += main.cpp\
                   mainwindow.cpp \
                   registrynode.cpp \
                   registrynodetreemodel.cpp \
                   registrykey.cpp \
                   registrykeytablemodel.cpp \
                   dlgabout.cpp \
                   qhexedit/qhexedit_p.cpp \
                   qhexedit/qhexedit.cpp \
                   reporttemplate.cpp \
                   registryhive.cpp \
                   qtscript_types/bytearray.cpp \
                   qtscript_types/bytearrayprototype.cpp \
                   qtscript_types/bytearrayiterator.cpp \
                   dlgreportviewer.cpp \
                   registrykeytable.cpp \
                   registrynodetree.cpp \
                   dlgsearch.cpp \
                   threadsearch.cpp \
                   searchresultwidget.cpp \
                   tabwidget.cpp \
                   argparser.cpp \
                   datainterpretertable.cpp \
                   datainterpreterwidget.cpp \
                   hexeditwidget.cpp \
                   settings.cpp \
                   searchresulttabledelegate.cpp \
                   registrynodetreemodelproxy.cpp \
                   reports.cpp \
                   reportengine.cpp \
                   dlgreportchooser.cpp \
                   dlgpreferences.cpp \
                   dlgaddkey.cpp

HEADERS         += mainwindow.h \
                   registrynode.h \
                   registrynodetreemodel.h \
                   registrykey.h \
                   registrykeytablemodel.h \
                   dlgabout.h \
                   qhexedit/qhexedit_p.h \
                   qhexedit/qhexedit.h \
                   reporttemplate.h \
                   registryhive.h \
                   qtscript_types/bytearray.h \
                   qtscript_types/bytearrayprototype.h \
                   qtscript_types/bytearrayiterator.h \
                   dlgreportviewer.h \
                   registrykeytable.h \
                   registrynodetree.h \
                   dlgsearch.h \
                   threadsearch.h \
                   searchresultwidget.h \
                   tabwidget.h \
                   argparser.h \
                   datainterpretertable.h \
                   datainterpreterwidget.h \
                   hexeditwidget.h \
                   settings.h \
                   searchresulttabledelegate.h \
                   registrynodetreemodelproxy.h \
                   reports.h \
                   reportengine.h \
                   dlgreportchooser.h \
                   dlgpreferences.h \
                   dlgaddkey.h

FORMS           += mainwindow.ui \
                   dlgabout.ui \
                   dlgreportviewer.ui \
                   dlgsearch.ui \
                   dlgreportchooser.ui \
                   dlgpreferences.ui \
                   dlgaddkey.ui

!isEmpty(HIVEX_STATIC) {
  LIBS          += $$PWD/hivex/lib/.libs/libhivex.a
} else {
  LIBS          += -lhivex
}

win32:LIBS      += -liconv
mac:LIBS        += -liconv

RESOURCES       += fred.qrc
RC_FILE          = fred.rc
ICON             = resources/fred.icns
