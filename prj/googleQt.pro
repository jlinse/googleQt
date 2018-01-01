QT       += network xml sql
QT       -= gui
#CONFIG += staticlib
DEFINES += GOOGLEQT_DLL
CONFIG -= flat
TEMPLATE = lib

GM_MODULES = users messages labels threads drafts errors history attachments
GT_MODULES = tasks tasklists
GD_MODULES = about files permissions comments revisions
GC_MODULES = contacts

SRC_DIR = ../src
G_DIR = $${SRC_DIR}/google
GM_DIR = $${SRC_DIR}/gmail
GT_DIR = $${SRC_DIR}/gtask
GD_DIR = $${SRC_DIR}/gdrive
GC_DIR = $${SRC_DIR}/gcontact

HEADERS += $${SRC_DIR}/*.h
SOURCES += $${SRC_DIR}/*.cpp
HEADERS += $${G_DIR}/endpoint/*.h
SOURCES += $${G_DIR}/endpoint/*.cpp
HEADERS += $${G_DIR}/demo/*.h
SOURCES += $${G_DIR}/demo/*.cpp
HEADERS += $${GM_DIR}/*.h $${GT_DIR}/*.h $${GD_DIR}/*.h $${GC_DIR}/*.h
SOURCES += $${GM_DIR}/*.cpp $${GT_DIR}/*.cpp $${GD_DIR}/*.cpp $${GC_DIR}/*.cpp


for(m, GM_MODULES){
       d = $${GM_DIR}/$${m}
       exists($${d}){
	   HEADERS += $${d}/*.h	
	   SOURCES += $${d}/*.cpp
       }
}

for(m, GT_MODULES){
       d = $${GT_DIR}/$${m}
       exists($${d}){
	   HEADERS += $${d}/*.h	
	   SOURCES += $${d}/*.cpp
       }
}

for(m, GD_MODULES){
       d = $${GD_DIR}/$${m}
       exists($${d}){
	   HEADERS += $${d}/*.h	
	   SOURCES += $${d}/*.cpp
       }
}

for(m, GC_MODULES){
       d = $${GC_DIR}/$${m}
       exists($${d}){
	   HEADERS += $${d}/*.h	
	   SOURCES += $${d}/*.cpp
       }
}

################################################################
# some diagnostics, some non-intence logging/tracing
################################################################
DEFINES += API_QT_DIAGNOSTICS

################################################################
# autotest generation (internal profiling usage)               
################################################################
ARD_AUTOTEST=$$(ARD_AUTOTEST)
if(!isEmpty( ARD_AUTOTEST )){
    HEADERS += $${G_DIR}/AUTOTEST/*.h
    SOURCES += $${G_DIR}/AUTOTEST/*.cpp
    HEADERS += $${GM_DIR}/AUTOTEST/*.h $${GT_DIR}/AUTOTEST/*.h $${GD_DIR}/AUTOTEST/*.h $${GC_DIR}/AUTOTEST/*.h
    SOURCES += $${GM_DIR}/AUTOTEST/*.cpp $${GT_DIR}/AUTOTEST/*.cpp $${GD_DIR}/AUTOTEST/*.cpp $${GC_DIR}/AUTOTEST/*.cpp
    DEFINES += API_QT_AUTOTEST
    DEFINES += API_QT_DIAGNOSTICS
    !build_pass:message("+autotest")
}
################################################################
ARD_DEBUG = $$(ARD_DEBUG)
if(!isEmpty( ARD_DEBUG )){
	  CONFIG += debug
	  CONFIG -= release
      unix {
           QMAKE_CXXFLAGS += -O0
           }
      !build_pass:message("+debug")
}

INCLUDEPATH += $${SRC_DIR}

