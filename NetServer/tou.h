#ifndef TOU_H
#define TOU_H

#define HOSTIP "127.0.0.1"
#define HOSTPORT 1234

#include <QTcpSocket>
#include <QTcpServer>
#include <qthread.h>
#include <map>
#include <QByteArray>
#include <QDateTime>

enum PlayerDirection{ n,w,e,s,nw,ne,sw,se };
#define MAX_BAG_ITEM_NUM 50
#define DEBUG

#ifdef DEBUG
#define JaegerDebug() qDebug()<< "[" << __FILE__ << ":" << __LINE__ << "]" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << __FUNCTION__ << "()"
#define JaegerInfo() qWarning()<< "[" << __FILE__ << ":" << __LINE__ << "]" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << __FUNCTION__ << "()"
#define JaegerCritical() qCritical()<< "[" << __FILE__ << ":" << __LINE__ << "]" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << __FUNCTION__ << "()"
#endif

#endif // TOU_H
