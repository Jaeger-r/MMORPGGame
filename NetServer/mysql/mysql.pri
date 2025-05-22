HEADERS += \
    $$PWD/CMySql.h

SOURCES += \
    $$PWD/CMySql.cpp


LIBS+= -L/usr/local/mysql-8.0.35-macos13-arm64/lib

INCLUDEPATH += /usr/local/mysql-8.0.35-macos13-arm64/include
LIBS += -L/usr/local/mysql-8.0.35-macos13-arm64/lib -lmysqlclient
LIBS += /usr/local/mysql-8.0.35-macos13-arm64/lib/libmysqlclient.dylib
