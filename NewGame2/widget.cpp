#include "widget.h"
#include "ui_widget.h"
#include <string>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    MaxHealth = 100;
    MaxExperience = 100;
    MaxLevel = 100;

    m_core = core::getKernel();
    m_login = new Login;
    m_login->setCore(m_core);

    if (m_core->open()) {
        qDebug()<<"Connect Server Success";
    }else {
        qDebug()<<"Connect Server Fail";
    }
    m_login->show();

    //Login
    connect((core*)m_core,&core::signal_login,this,&Widget::slot_login);
    //Register
    connect((core*)m_core,&core::signal_register,this,&Widget::slot_register);
    //Initialize
    connect((core*)m_core,&core::signal_initialize,this,&Widget::slot_initialize);
    //Location
    connect((core*)m_core,&core::signal_location,this,&Widget::slot_location,Qt::QueuedConnection);
    //Save
    connect((core*)m_core,&core::signal_save,this,&Widget::slot_save,Qt::QueuedConnection);
    //Dazuo
    connect((core*)m_core,&core::signal_dazuo,this,&Widget::slot_dazuo,Qt::QueuedConnection);
    //Levelup
    connect((core*)m_core,&core::signal_levelup,this,&Widget::slot_levelup,Qt::QueuedConnection);
    //InitBag
    connect((core*)m_core,&core::signal_initbag,this,&Widget::slot_initBag,Qt::QueuedConnection);

}

Widget::~Widget()
{
    m_core->close();
    if (ui) {
        delete ui;
        ui = nullptr;
    }

    if (m_core) {
        delete m_core;
        m_core = nullptr;
    }

    if (m_login) {
        delete m_login;
        m_login = nullptr;
    }

    if (m_gameclient) {
        delete m_gameclient;
        m_gameclient = nullptr;
    }

    if (m_chat) {
        m_chat->~Chat();
        delete m_chat;
        m_chat = nullptr;
    }
}
/**
 * @brief 游戏主循环，处理玩家移动、更新场景等
 * @author Jaeger
 * @date 2025.3.28
 */
void Widget::gameLoop()
{
    updatePosition();
    updateInformation();
}
/**
 * @brief 游戏定时发送数据，定时向服务器申请保存
 * @author Jaeger
 * @date 2025.4.8
 */
void Widget::gameSend()
{
    JaegerDebug();
    // STRU_SAVE_RQ ssq;
    // ssq.player_UserId = m_gameclient->getPlayer()->getId();
    // ssq.health = m_gameclient->getPlayer()->getHealth();
    // ssq.attackPower = m_gameclient->getPlayer()->getAttackPower();
    // ssq.attackRange = m_gameclient->getPlayer()->getAttackRange();
    // ssq.experience = m_gameclient->getPlayer()->getExperience();
    // ssq.level = m_gameclient->getPlayer()->getLevel();
    // ssq.defence = m_gameclient->getPlayer()->getDefence();
    // ssq.critical_rate = m_gameclient->getPlayer()->getCritical_Rate();
    // ssq.critical_damage = m_gameclient->getPlayer()->getCritical_Damage();
    // m_core->sendData(m_core->getSocket(),(char*)&ssq,sizeof(ssq));
}
/**
 * @brief 更新玩家坐标
 * @author Jaeger
 * @date 2025.3.28
 */
void Widget::updatePosition()
{
    if(!m_gameclient){return;}

    static float lastX = m_gameclient->getPlayer()->getX();
    static float lastY = m_gameclient->getPlayer()->getY();
    float currentX = m_gameclient->getPlayer()->getX();
    float currentY = m_gameclient->getPlayer()->getY();

    // 仅当坐标发生变化时才发送数据
    if (currentX != lastX || currentY != lastY) {
        STRU_LOCATION_RQ slq;
        strcpy(slq.player_Name,m_gameclient->getPlayer()->getName().toStdString().c_str());
        slq.player_UserId = m_gameclient->getPlayer()->getId();
        slq.x = currentX;
        slq.y = currentY;

        m_core->sendData(m_core->getSocket(), (char*)&slq, sizeof(slq));

        // 更新上次坐标
        lastX = currentX;
        lastY = currentY;
    }
}

void Widget::updateInformation()
{
    m_gameclient->updateExperience(m_gameclient->getPlayer()->getExperience(),MaxExperience);
    m_gameclient->updateHealth(m_gameclient->getPlayer()->getHealth(),MaxHealth);
    m_gameclient->updateLevel(m_gameclient->getPlayer()->getLevel(),MaxLevel);
}

void Widget::slot_login(STRU_LOGIN_RS*szbuf)
{
    if (szbuf->player_Result == _login_success) {
        STRU_INITIALIZE_RQ siq;
        siq.player_UserId = szbuf->player_UserId;
        m_core->sendData(m_core->getSocket(),(char*)&siq,sizeof(siq));
    }
    if (szbuf->player_Result == _login_passworderr) {
        QMessageBox::information(this,"登录","密码错误");
    }
    if (szbuf->player_Result == _login_usernoexist) {
        QMessageBox::information(this,"登录","用户不存在");
    }
}

void Widget::slot_register(STRU_REGISTER_RS*szbuf)
{
    if (szbuf->player_Result == _register_success_) {
        QMessageBox::information(this,"注册","注册成功。");
    }
}

void Widget::slot_initialize(STRU_INITIALIZE_RS *szbuf)
{
    if (szbuf->Initialize_Result == _initialize_success) {
        m_gameclient = new gameclient;
        m_gameclient->setCore(m_core);
        m_chat = m_gameclient->getChat();
        m_chat->setCore(m_core);
        connect((core*)m_core,&core::signal_chat,m_chat,&Chat::slot_chat,Qt::QueuedConnection);
        m_gameclient->createPlayer(szbuf->player_Name,
                                   szbuf->health,
                                   szbuf->attackPower,
                                   szbuf->attackRange,
                                   szbuf->experience,
                                   szbuf->level,
                                   szbuf->defence,
                                   szbuf->critical_rate,
                                   szbuf->critical_damage,
                                   szbuf->x,szbuf->y,
                                   Player::s,
                                   szbuf->player_UserId);

        qDebug()<<"ID:"<<szbuf->player_UserId;
        QString qStr = QString::fromUtf8(szbuf->player_Name);
        m_chat->setName(qStr);
        // 启动游戏主循环
        gameLoopTimer = new QTimer(this);
        connect(gameLoopTimer, &QTimer::timeout, this, &Widget::gameLoop);
        gameLoopTimer->start(100); // 每 100ms 运行一次
        // 启动游戏定时发送数据
        gameSendTimer = new QTimer(this);
        connect(gameSendTimer,&QTimer::timeout,this,&Widget::gameSend);
        //gameSendTimer->start(10000);//60s

        MaxExperience = exp(szbuf->level);
        m_login->hide();
        m_gameclient->show();
        m_gameclient->updateHealth(szbuf->health,MaxHealth);
        m_gameclient->updateLevel(szbuf->level,MaxLevel);
        m_gameclient->updateExperience(szbuf->experience,exp(szbuf->level));
    }
    if (szbuf->Initialize_Result == _initialize_fail) {
        QMessageBox::information(this,"初始化","初始化未完成。");
    }
    if (szbuf->Initialize_Result == _initialize_error) {
        QMessageBox::information(this,"初始化","初始化出现错误。");
    }
}

void Widget::slot_location(STRU_LOCATION_RS *szbuf)
{
    JaegerDebug();
    if(!m_gameclient)return;

    for(int i = 0;i < szbuf->playerCount;i++){
        int userId = szbuf->players[i].player_UserId;
        QString name = szbuf->players[i].player_Name;
        float x = szbuf->players[i].x;
        float y = szbuf->players[i].y;
        if(userId != m_gameclient->getPlayer()->getId()){
            qDebug() <<"userId:"<<userId
                     <<"name:"<<name
                     <<"x:"<<x
                     <<"y:"<<y;
            m_gameclient->updateOtherPlayer(userId,name,x,y);
        }
        if(userId == m_gameclient->getPlayer()->getId()){
            continue;
        }

    }
}

void Widget::slot_save(STRU_SAVE_RS *szbuf)
{
    JaegerDebug();
    if(szbuf->Save_Result == _save_success_){
        qDebug()<<"AutoSave Success";
    }
    if(szbuf->Save_Result == _save_fail_ || szbuf->Save_Result == _save_error_){
        qDebug()<<"AutoSave Failed";
    }
}

void Widget::slot_dazuo(STRU_DAZUO_RS *szbuf)
{
    JaegerDebug();
    if(szbuf->dazuoConfirm){
        dazuoTimer = new QTimer(this);

        connect(dazuoTimer, &QTimer::timeout, this, [=]() {
            QPointF pos(m_gameclient->getPlayer()->getX(), m_gameclient->getPlayer()->getY());
            m_gameclient->showEffect("打坐中", pos, QColor(0, 0, 255));
        });

        if (!dazuoTimer->isActive()) {
            dazuoTimer->start(2000);
        }
    }
    if(!szbuf->dazuoConfirm){
        if (dazuoTimer && dazuoTimer->isActive()) {
            dazuoTimer->stop();
        }
        QPointF pos(m_gameclient->getPlayer()->getX(), m_gameclient->getPlayer()->getY());
        m_gameclient->showEffect("打坐结束",pos,QColor(0, 0, 255));
        qDebug()<<"增加经验："<<szbuf->addedExp;
        long long newExp = m_gameclient->getPlayer()->getExperience() + szbuf->addedExp;
        m_gameclient->getPlayer()->setExperience(newExp);
        m_gameclient->updateExperience(newExp, MaxExperience);
    }
}

void Widget::slot_levelup(STRU_LEVELUP_RS *szbuf)
{
    QPointF pos(m_gameclient->getPlayer()->getX(), m_gameclient->getPlayer()->getY());
    m_gameclient->showEffect("升级！",pos,QColor(128, 0, 128));
    int newLevel = szbuf->userlevel;
    long long maxExp = exp(newLevel);

    m_gameclient->getPlayer()->setLevel(newLevel);
    m_gameclient->getPlayer()->setExperience(0);

    MaxExperience = maxExp;

    qDebug()<<"Lv:"<<newLevel;
    m_gameclient->updateLevel(newLevel, MaxLevel);
    m_gameclient->updateExperience(0, MaxExperience);
}

