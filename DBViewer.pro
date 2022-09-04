TEMPLATE = app
TARGET = DBviewer

QT += core sql network widgets printsupport

CONFIG += app_bundle c++17

FRONT = $$PWD/src/front
FRONT_UI = $$PWD/src/front/ui
FRONT_STYLE = $$PWD/src/front/style/QDarkStyle

BACK = $$PWD/src/back
BACK_UI = $$PWD/src/back/ui

Release:DESTDIR = ../Compiles/DBviewer/Release
Release:OBJECTS_DIR = Build/Release/obj/
Release:MOC_DIR = Build/Release/moc/
Release:UI_DIR = Build/Debug/ui/
Release:RCC_DIR = Build/Release/rcc/

Debug:DESTDIR = ../Compiles/DBviewer/Debug
Debug:OBJECTS_DIR = Build/Debug/obj/
Debug:MOC_DIR = Build/Debug/moc/
Debug:UI_DIR = Build/Debug/ui/
Debug:RCC_DIR = Build/Debug/rcc/

HEADERS += $$BACK/Database.h \
	\
	$$BACK_UI/LoginForm.h \
	$$BACK_UI/DBViewForm.h \
	$$BACK_UI/ViewTableTabsForm.h \
	\
	$$BACK_UI/SearchDialog.h \
	\
	$$BACK_UI/QueryListForm.h \
	$$BACK_UI/QueryAddDialog.h \
	\
	$$FRONT_STYLE/DarkStyle.h

SOURCES += $$BACK/main.cpp \
	$$BACK/Database.cpp \
	\
	$$BACK_UI/LoginForm.cpp \
	$$BACK_UI/DBViewForm.cpp \
	$$BACK_UI/ViewTableTabsForm.cpp \
	\
	$$BACK_UI/SearchDialog.cpp \
	\
	$$BACK_UI/QueryListForm.cpp \
	$$BACK_UI/QueryAddDialog.cpp \
	\
	$$FRONT_STYLE/DarkStyle.cpp
	
FORMS += $$FRONT_UI/LoginForm.ui \
	$$FRONT_UI/DBViewForm.ui \ 
	$$FRONT_UI/ViewTableTabsForm.ui \
	\
	$$FRONT_UI/SearchDialog.ui \
	\
	$$FRONT_UI/QueryListForm.ui \
	$$FRONT_UI/QueryAddDialog.ui

RESOURCES += $$FRONT/content.qrc \
	$$FRONT_STYLE/darkstyle.qrc