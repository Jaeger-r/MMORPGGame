/**
 * 2025.4.8 代码2404行
 *
 *
 *
 *
 *
 *
 */



#ifndef TOU_H
#define TOU_H
#include <QDebug>
#include <QDateTime>
#include <QDialog>

#include <QFile>

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>

#include <QKeyEvent>

#include <QLabel>
#include <QList>

#include <QMap>
#include <QMessageBox>

#include <QObject>

#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>

#include <QString>

#include <QTableView>
#include <QTableWidgetItem>
#include <QTimer>
#include <QToolTip>

#include <QUdpSocket>

#include <QVBoxLayout>

#include <QWidget>

#define HOSTIP "127.0.0.1"
#define HOSTPORT 1234
#define MAX_BAG_ITEM_NUM 50

#define DEBUG

#ifdef DEBUG
#define JaegerDebug() qDebug()<< "[" << __FILE__ << ":" << __LINE__ << "]" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << __FUNCTION__ << "()"
#define JaegerInfo() qWarning()<< "[" << __FILE__ << ":" << __LINE__ << "]" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << __FUNCTION__ << "()"
#define JaegerCritical() qCritical()<< "[" << __FILE__ << ":" << __LINE__ << "]" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << __FUNCTION__ << "()"
#endif

static const int MAP_WIDTH = 800;
static const int MAP_HEIGHT = 800;
static const int PLAYER_SIZE = 30;
static const int PLAYER_MOVE = 10;
static const int PLAYER_CHUSHILOCATION_X = 50;
static const int PLAYER_CHUSHILOCATION_Y = 50;
static const int PLAYER_TEXTPOSITION = 20;
static const int NPC_SIZE = 30;
static const int NPC_RECTRANGE = 60;

static const int MONEY_VALUE = 1;
static const int INVENTORY_MAXSIZE = 30;
enum PlayerDirection{ n,w,e,s,nw,ne,sw,se };
#endif // TOU_H





