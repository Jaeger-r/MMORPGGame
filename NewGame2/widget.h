#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "core.h"
#include "login.h"
#include "gameclient.h"
#include "packdef.h"
#include "chat.h"
#include <QTimer>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void gameLoop();
    void updatePosition();
    void updateInformation();

    void gameSend();

    ICore *m_core;
    Login *m_login;
    //gameclient* m_gameclient = gameclient::getGameclient();
    gameclient *m_gameclient;
    Chat *m_chat;

private slots:
    void slot_login(STRU_LOGIN_RS*szbuf);
    void slot_register(STRU_REGISTER_RS*szbuf);
    void slot_initialize(STRU_INITIALIZE_RS*szbuf);
    void slot_location(STRU_LOCATION_RS*szbuf);
    void slot_save(STRU_SAVE_RS*szbuf);
    void slot_dazuo(STRU_DAZUO_RS*szbuf);
    void slot_levelup(STRU_LEVELUP_RS*szbuf);
    void slot_initBag(STRU_INITBAG_RS*szbuf);
private:
    Ui::Widget *ui;
    QTimer *gameLoopTimer; // 游戏主循环定时器
    QTimer *gameSendTimer; // 游戏定时发送属性保存
    QTimer *dazuoTimer;
    int MaxHealth;
    int MaxLevel;
    long long MaxExperience;
private:
    long long exp(int x){
        return 100 * x + 50 * pow(x - 1, 3);
    }
};
#endif // WIDGET_H
