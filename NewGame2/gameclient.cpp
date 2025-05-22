#include "gameclient.h"
/**
 * @brief gameclient,~gameclient构造，析构函数
 * @author Jaeger
 * @date 2025.3.26
 */
gameclient::gameclient(QWidget *parent) : QWidget(parent)
{
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    view->setSceneRect(0, 0, MAP_WIDTH, MAP_HEIGHT);
    view->setRenderHint(QPainter::Antialiasing);

    playerbag = new Bag();
    playerchat = new Chat();
    player = new Player();

    initNPC();     // 初始化 NPC
    initTimers();  // 初始化定时器
    initUI();      // 初始化 UI（包括状态栏）

    //attack
    connect((core*)m_Core,&core::signal_attack,this,&gameclient::slot_attack);
    //revive
    connect((core*)m_Core,&core::signal_revive,this,&gameclient::slot_revive);
}
void gameclient::initTimers()
{
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &gameclient::processMovement);
    moveTimer->start(10);

    NPCTimer = new QTimer(this);
    connect(NPCTimer,&QTimer::timeout,this,&gameclient::checkAllTaskNPC);
    NPCTimer->start(100);
}
void gameclient::initNPC()
{
    TaskNPC_Dazuo* zhangwendi = new TaskNPC_Dazuo();
    zhangwendi->createNPC(scene,300,300,"张文帝",NPC::Unattackable);
    setNPC(1,zhangwendi);
    //zhangwendi->showDialog(scene,"你好，我是张文帝");
    //npc->createNPC(scene,300,300,"张文帝",NPC::Unattackable);
    //npc->createNPC(scene,450,300,"王梓旭",NPC::Unattackable);
    //npc->createNPC(scene,500,300,"姜昭我",NPC::Unattackable);
}
void gameclient::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *topLayout = new QHBoxLayout();

    // 初始化状态栏（血量、经验、等级）
    QGridLayout *gridLayout = initStatusBar();

    // 按钮
    bag = new QPushButton("背包", this);
    bag->setFixedWidth(120);
    chat = new QPushButton("聊天", this);
    chat->setFixedWidth(120);

    gridLayout->addWidget(bag, 6, 0);
    gridLayout->addWidget(chat, 7, 0);

    gridLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    topLayout->addLayout(gridLayout);
    topLayout->addWidget(view);
    mainLayout->addLayout(topLayout);
    setLayout(mainLayout);

    connect(bag, &QPushButton::clicked, this, &gameclient::openbag);
    connect(chat, &QPushButton::clicked, this, &gameclient::openchat);
}
QGridLayout* gameclient::initStatusBar()
{
    QGridLayout *gridLayout = new QGridLayout();

    // 血量
    healthLabel = new QLabel("Health:");
    healthBar = new QProgressBar(this);
    healthBar->setRange(0, 100);
    healthBar->setValue(player->getHealth());
    healthBar->setTextVisible(true);
    healthBar->setFormat("%v/%m");
    healthBar->setStyleSheet("QProgressBar::chunk {background: red;}");
    healthBar->setFixedWidth(120);

    // 经验
    experienceLabel = new QLabel("Experience:");
    experienceBar = new QProgressBar(this);
    experienceBar->setRange(0, 10);
    experienceBar->setValue(player->getExperience());
    experienceBar->setTextVisible(true);
    experienceBar->setFormat("%v/%m");
    experienceBar->setStyleSheet("QProgressBar::chunk {background: green;}");
    experienceBar->setFixedWidth(120);

    // 等级
    levelLabel = new QLabel("Level:");
    levelBar = new QProgressBar(this);
    levelBar->setRange(0, 200);
    levelBar->setValue(player->getLevel());
    levelBar->setTextVisible(true);
    levelBar->setFormat("%v/%m");
    levelBar->setStyleSheet("QProgressBar::chunk {background: blue;}");
    levelBar->setFixedWidth(120);

    // 布局
    gridLayout->addWidget(healthLabel, 0, 0);
    gridLayout->addWidget(healthBar, 1, 0);
    gridLayout->addWidget(experienceLabel, 2, 0);
    gridLayout->addWidget(experienceBar, 3, 0);
    gridLayout->addWidget(levelLabel, 4, 0);
    gridLayout->addWidget(levelBar, 5, 0);

    return gridLayout;
}
gameclient::~gameclient() {
    delete playerbag;
    delete playerchat;
    delete bag;
    delete chat;
    delete player;
}
/**
 * @brief createPlayer创建玩家函数
 * @author Jaeger
 * @date 2025.3.26
 */
void gameclient::createPlayer(QString name, int hp, int atkPower, int atkRange,
                              long long exp, int lvl, int def, float critRate, float critDmg,
                              float posX, float posY, Player::PlayerDirection dir, int id) {
    // 创建 UI 组件
    player->createPlayer(scene, name, hp, atkPower, atkRange, exp, lvl, def, critRate, critDmg, posX, posY, dir, id);
}
/**
 * @brief keyPressEvent，keyReleaseEvent，processMovement,isValidMove移动逻辑
 * @author Jaeger
 * @date 2025.3.28
 */
void gameclient::keyPressEvent(QKeyEvent *event){
    pressedKeys.insert(event->key());
    switch (event->key()) {
    case Qt::Key_Space:
        if (!isAttacking) { // 防止重复触发
            isAttacking = true;
            playerattack();
        }
        break;
    }
    // 如果定时器未启动，则启动
    if (!moveTimer->isActive()) {
        moveTimer->start(10); // 每 10ms 触发移动
    }
}
void gameclient::keyReleaseEvent(QKeyEvent *event) {
    pressedKeys.remove(event->key()); // 松开按键时移除

    if (pressedKeys.isEmpty() && moveTimer) {
        moveTimer->stop(); // 所有键松开时停止移动
    }
    if (event->key() == Qt::Key_Space) {
        isAttacking = false; // 重置标志位
        qDebug() <<"release";
        hideAttackArea(); // 隐藏攻击范围
    }
}

void gameclient::updateHealth(int currentHealth, int maxHealth)
{
    healthBar->setRange(0, maxHealth);
    healthBar->setValue(currentHealth);
}

void gameclient::updateExperience(int currentExp, int maxExp)
{
    experienceBar->setRange(0,maxExp);
    experienceBar->setValue(currentExp);
}

void gameclient::updateLevel(int currentLvl, int maxLvl)
{
    levelBar->setRange(0,maxLvl);
    levelBar->setValue(currentLvl);
}

void gameclient::processMovement(){
    int dx = 0, dy = 0;
    if (pressedKeys.contains(Qt::Key_W)) {
        dy -= PLAYER_MOVE;
    }
    if (pressedKeys.contains(Qt::Key_S)) {
        dy += PLAYER_MOVE;
    }
    if (pressedKeys.contains(Qt::Key_A)) {
        dx -= PLAYER_MOVE;
    }
    if (pressedKeys.contains(Qt::Key_D)) {
        dx += PLAYER_MOVE;
    }
    if(dy < 0 && dx == 0){player->setDirection(Player::n);}
    if(dy < 0 && dx < 0){player->setDirection(Player::nw);}
    if(dy > 0 && dx == 0){player->setDirection(Player::s);}
    if(dy > 0 && dx > 0){player->setDirection(Player::se);}
    if(dy < 0 && dx > 0){player->setDirection(Player::ne);}
    if(dy == 0 && dx < 0){player->setDirection(Player::w);}
    if(dy == 0 && dx > 0){player->setDirection(Player::e);}
    if(dy > 0 && dx < 0){player->setDirection(Player::sw);}


    float newX = player->getX() + dx / 5;
    float newY = player->getY() + dy / 5;
    if(isValidMove(newX,newY)){
        player->setX(newX);
        player->setY(newY);
        player->getGraphicsItem()->setPos(newX,newY);
        //player->getTextItem()->setPos(player->getX(), player->getY() - PLAYER_MOVE);
        player->updateArrowPosition();
        player->updateTextPosition();
    }
    // 更新所有其他玩家的位置
    for (auto it = otherPlayersMap.begin(); it != otherPlayersMap.end(); ++it) {
        OtherPlayers* otherPlayer = it.value();
        if (otherPlayer) {
            float othernewX = otherPlayer->getX();
            float othernewY = otherPlayer->getY();
            otherPlayer->updateLocation(othernewX, othernewY);
        }
    }

}
bool gameclient::isValidMove(int newX, int newY) {
    return newX >= 0 && newX + PLAYER_SIZE <= MAP_WIDTH &&newY >= 0 && newY + PLAYER_SIZE <= MAP_HEIGHT;
}
/**
 * @brief updateOtherPlayer更新其他玩家,isPlayerExists判断其他玩家是否存在map中
 * @author Jaeger
 * @date 2025.3.29
 */

void gameclient::updateOtherPlayer(int userId, QString name,float x, float y)
{
    JaegerDebug();
    if(!otherPlayersMap.contains(userId)){
        OtherPlayers* newOtherPlayer = new OtherPlayers;
        newOtherPlayer->createPlayer(scene,userId,name,x,y);
        otherPlayersMap.insert(userId,newOtherPlayer);
    }
    otherPlayersMap[userId]->updateLocation(x,y);
}
/**
 * @brief checkAllTaskNPCs轮询所有NPC判断是否有玩家在其范围内
 * @author Jaeger
 * @date 2025.4.8
 */
void gameclient::checkAllTaskNPC()
{
    for (auto it = npcController.begin(); it != npcController.end(); ++it) {
        TaskNPC_Dazuo* taskNpc = dynamic_cast<TaskNPC_Dazuo*>(it.value());
        if (taskNpc) {
            QPointF npcCenter = taskNpc->getGraphicsItem()->sceneBoundingRect().center();
            QPointF playerCenter = player->getGraphicsItem()->sceneBoundingRect().center();
            qreal distance = QLineF(npcCenter, playerCenter).length();

            if (distance < NPC_RECTRANGE) {
                if (!taskNpc->getIsInRange()) {
                    taskNpc->setIsInRange(true);
                    taskNpc->onPlayerEnter(player,scene,m_Core);
                }
            } else {
                if (taskNpc->getIsInRange()) {
                    taskNpc->setIsInRange(false);
                    taskNpc->onPlayerLeave(player,scene,m_Core);
                }
            }
        }
    }
}

/**
 * @brief playerattack攻击逻辑
 * @author Jaeger
 * @date 2025.3.26
 */
void gameclient::playerattack() {

    int attackX = player->getX();
    int attackY = player->getY();
    int attackWidth = PLAYER_SIZE;
    int attackHeight = PLAYER_SIZE;

    int range = player->getAttackRange();
    QPointF center(player->getX() + PLAYER_SIZE / 2, player->getY() + PLAYER_SIZE / 2);
    qreal outerRadius = player->getAttackRange() + PLAYER_SIZE;
    qreal innerRadius = outerRadius * 0.8;  // 控制月牙厚度
    // 根据方向设置起始角度
    int startAngle = 0;
    switch (player->getDirection()) {
    case Player::n:
        startAngle = 45;
        attackY -= range + PLAYER_SIZE/2;
        attackX -= PLAYER_SIZE/2;
        attackWidth = PLAYER_SIZE*2;
        attackHeight = 10;
        break;
    case Player::ne:
        attackX += PLAYER_SIZE;
        attackY -= range;
        attackWidth = range;
        attackHeight = range;
        startAngle = 0;
        break;
    case Player::nw:
        attackX -= range;
        attackY -= range;
        attackWidth = range;
        attackHeight = range;
        startAngle = 90;
        break;
    case Player::s:
        startAngle = 225;
        attackY += range + PLAYER_SIZE/2 + PLAYER_SIZE - 10;
        attackX -= PLAYER_SIZE/2;
        attackWidth = PLAYER_SIZE*2;
        attackHeight = 10;
        break;
    case Player::sw:
        attackX -= range;
        attackY += PLAYER_SIZE;
        attackWidth = range;
        attackHeight = range;
        startAngle = 180;
        break;
    case Player::se:
        attackX += PLAYER_SIZE;
        attackY += PLAYER_SIZE;
        attackWidth = range;
        attackHeight = range;
        startAngle = 270;
        break;
    case Player::w:
        startAngle = 135;
        attackY -= PLAYER_SIZE/2;
        attackX -= range + PLAYER_SIZE/2;
        attackWidth = 10;
        attackHeight = PLAYER_SIZE*2;;
        break;
    case Player::e:
        startAngle = 315;
        attackY -= PLAYER_SIZE/2;
        attackX += range + PLAYER_SIZE/2 + PLAYER_SIZE - 10;
        attackWidth = 10;
        attackHeight = PLAYER_SIZE*2;;
        break;

    }

    QRectF outerRect(center.x() - outerRadius, center.y() - outerRadius, outerRadius * 2, outerRadius * 2);
    QRectF innerRect(center.x() - innerRadius, center.y() - innerRadius, innerRadius * 2, innerRadius * 2);

    QPainterPath outerPath;
    outerPath.moveTo(center);
    outerPath.arcTo(outerRect, startAngle, 90); // 90 度的攻击角度
    outerPath.lineTo(center);

    QPainterPath innerPath;
    innerPath.moveTo(center);
    innerPath.arcTo(innerRect, startAngle, 90); // 同样角度，形成内部弧线
    innerPath.lineTo(center);

    // 弯月形状 = 外圈 - 内圈
    QPainterPath moonShape = outerPath.subtracted(innerPath);
    //实际攻击范围
    QRectF attackRect(attackX, attackY, attackWidth, attackHeight);
    if (isAttacking) {
        //绘制实际攻击范围--测试
        attackArea = new QGraphicsRectItem(attackX, attackY, attackWidth, attackHeight);
        attackArea->setBrush(QBrush(QColor(255, 0, 0, 100)));
        scene->addItem(attackArea);
        //绘制攻击动画--测试
        attackEffect = new QGraphicsPathItem(moonShape);
        attackEffect->setBrush(QBrush(QColor(50, 0, 0, 255)));
        attackEffect->setPen(Qt::NoPen);  // 去掉边框
        scene->addItem(attackEffect);
    }

    STRU_ATTACK_RQ attackRequest;

    for (auto it = otherPlayersMap.begin(); it != otherPlayersMap.end(); ++it) {  // 遍历所有玩家
        OtherPlayers* otherPlayer = it.value();// 获取其他玩家对象

        if (otherPlayer->getId() == player->getId()) {
            continue;  // 跳过自己
        }


        QRectF otherPlayerBounds = otherPlayer->getGraphicsItem()->boundingRect().translated(otherPlayer->getGraphicsItem()->pos()); // 获取其他玩家的边界

        if (attackRect.intersects(otherPlayerBounds)) {
        //if (moonShape.intersects(otherPlayerBounds)) {
            qDebug()<<"攻击目标id："<<otherPlayer->getId();
            attackRequest.attackerId = player->getId();
            attackRequest.targetId = otherPlayer->getId();
            attackRequest.damage = player->getAttackPower();
            m_Core->sendData(m_Core->getSocket(),(char*)&attackRequest, sizeof(attackRequest));

            if(player->getHealth() > 0 && otherPlayer->getHealth() > 0){
                showDamageEffect(player->getAttackPower(), otherPlayer->getGraphicsItem()->scenePos());
            }
        }
    }
    for(auto it = npcController.begin();it != npcController.end();++it){
        NPC* npc = it.value();
        if (npc->getGraphicsItem()) { // 确保 npc 对象和其 rectItem 有效   防止npc已被销毁
            if (npc->getType() == NPC::Attackable) { // 获取类型判断是否为可攻击类型
                QRectF npcBounds = npc->getGraphicsItem()->boundingRect().translated(npc->getGraphicsItem()->pos()); // 获取 NPC 的边界范围

                // 检测攻击范围与 NPC 是否相交 比较两个矩形的坐标范围是否重叠
                if (attackRect.intersects(npcBounds)) {
                //if (moonShape.intersects(npcBounds)) {
                    npc->updateHealth(-player->getAttackPower());             // 更新血量
                    qDebug() << "NPC hit! Remaining health:" << npc->getHealth()<<"NPC Position:"<<npc->getGraphicsItem()->scenePos();
                    showDamageEffect(player->getAttackPower(),npc->getNpcPosition());
                    // 如果血量低于或等于 0，移除 NPC
                    if (npc->getHealth() <= 0) {
                        checkHealth(npc->getGraphicsItem(), npc->getHealth());
                    }
                }
            }
        }
    }
}

void gameclient::showDamageEffect(int damage, QPointF position) {
    QGraphicsTextItem *damageText = new QGraphicsTextItem(QString::number(damage));
    damageText->setDefaultTextColor(Qt::red);
    damageText->setFont(QFont("Arial", 14, QFont::Bold));
    damageText->setPos(position);

    scene->addItem(damageText);

    QPropertyAnimation *animation = new QPropertyAnimation(damageText, "y");
    animation->setDuration(800);
    animation->setStartValue(position.y());
    animation->setEndValue(position.y() - 30); // 上升 30 像素
    animation->setEasingCurve(QEasingCurve::OutQuad);

    connect(animation, &QPropertyAnimation::finished, damageText, &QGraphicsTextItem::deleteLater);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
void gameclient::showEffect(const QString& text, QPointF position,const QColor& color) {
    QGraphicsTextItem *damageText = new QGraphicsTextItem(text);
    damageText->setDefaultTextColor(color);
    damageText->setFont(QFont("Arial", 14, QFont::Bold));
    damageText->setPos(position);

    scene->addItem(damageText);

    QPropertyAnimation *animation = new QPropertyAnimation(damageText, "y");
    animation->setDuration(800);
    animation->setStartValue(position.y());
    animation->setEndValue(position.y() - 30); // 上升 30 像素
    animation->setEasingCurve(QEasingCurve::OutQuad);

    connect(animation, &QPropertyAnimation::finished, damageText, &QGraphicsTextItem::deleteLater);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void gameclient::checkHealth(QGraphicsItem *item, int health) {
    //在迭代过程中收集要移除的物品和玩家，然后在循环后处理它们，以避免迭代器无效和场景冲突。
    if (health <= 0) {
        // 死亡处理容器
        QList<QGraphicsItem*> itemsToRemove;  // NPC需要真正删除
        QList<int> playersToDeactivate;       // 玩家需要隐藏

        //  NPC处理
        for(auto it = npcController.begin();it != npcController.end();++it){
            NPC* npc = it.value();
            if (npc && npc->getGraphicsItem() == item) {
                if (npc->getGraphicsItem()) {
                    scene->removeItem(npc->getGraphicsItem());
                    itemsToRemove.append(npc->getGraphicsItem());
                }
                if (npc->getTextItem()) {
                    scene->removeItem(npc->getTextItem());
                    itemsToRemove.append(npc->getTextItem());
                }
                npc->setGraphicsItem(nullptr);
                npc->setTextItem(nullptr);
                qDebug() << "NPC已被永久移除";
            }
        }

        //  其他玩家处理
        for (auto it = otherPlayersMap.begin(); it != otherPlayersMap.end(); ++it) {
            OtherPlayers* otherPlayer = it.value();
            if (otherPlayer && otherPlayer->getGraphicsItem() == item) {
                // 设置半透明效果
                otherPlayer->getGraphicsItem()->setOpacity(0.4);
                otherPlayer->getTextItem()->setOpacity(0.4);
                playersToDeactivate.append(it.key());
            }
        }

        // 主玩家处理
        if (player && player->getGraphicsItem() == item) {
            player->getGraphicsItem()->setOpacity(0.4);
            player->getTextItem()->setOpacity(0.4);
            //QMessageBox::information(this, "提示", "你已死亡，3秒后复活");
        }

        //实际删除NPC图形项
        for (QGraphicsItem* item : itemsToRemove) {
            delete item;
        }

        //  玩家复活定时器
        if (!playersToDeactivate.isEmpty() || (player && player->getGraphicsItem() == item)) {
            QTimer::singleShot(10000, this, [=]() {
                // 复活其他玩家
                for (int id : playersToDeactivate) {
                    if (otherPlayersMap.contains(id)) {
                        OtherPlayers* p = otherPlayersMap[id];
                        p->getGraphicsItem()->setOpacity(1.0);
                        p->getTextItem()->setOpacity(1.0);
                    }
                }

                // 复活主玩家
                if (player && player->getGraphicsItem() == item) {
                    player->getGraphicsItem()->setOpacity(1.0);
                    player->getTextItem()->setOpacity(1.0);
                }
            });
        }
    }
    // if (health <= 0) {
    //     // 遍历 NPC 列表，找到与 item 匹配的 NPC
    //     if (npc->getGraphicsItem() == item) {
    //         // 从场景中移除 NPC 的矩形和文本
    //         if (npc->getGraphicsItem()) {
    //             scene->removeItem(npc->getGraphicsItem());
    //             delete npc->getGraphicsItem();
    //             npc->setGraphicsItem(nullptr);
    //         }
    //         if (npc->getTextItem()) {
    //             scene->removeItem(npc->getTextItem());
    //             delete npc->getTextItem();
    //             npc->setTextItem(nullptr);
    //         }
    //     }
    // }
}
void gameclient::hideAttackArea() {
    if (attackArea) {
        scene->removeItem(attackArea); // 从场景中移除
        delete attackArea;             // 释放内存
        attackArea = nullptr;          // 避免悬空指针
    }
    if(attackEffect) {
        scene->removeItem(attackEffect);
        delete attackEffect;
        attackEffect = nullptr;
    }
}
void gameclient::slot_attack(STRU_ATTACK_RS *szbuf)
{
    qDebug() << "[攻击响应] 目标ID:" << szbuf->targetId
             << "血量:" << szbuf->currentHealth
             << "是否死亡:" << szbuf->isDead;
    // 更新其他玩家
    if (otherPlayersMap.contains(szbuf->targetId)) {
        OtherPlayers* player = otherPlayersMap[szbuf->targetId];
        player->setHealth(szbuf->currentHealth);
        //qDebug() << player->getId()<<"player hit! Remaining health:" << player->getHealth();
        if (szbuf->isDead) {
            checkHealth(player->getGraphicsItem(), 0);

            //弹窗死亡
            QMessageBox* msgBox = new QMessageBox(this);
            msgBox->setWindowTitle("战斗结果");
            msgBox->setText(QString("玩家 %1 已被击败!").arg(szbuf->targetId));
            msgBox->setIcon(QMessageBox::Information);
            msgBox->setAttribute(Qt::WA_DeleteOnClose);//自动释放内存
            msgBox->show();
        }
    }
    // 更新 NPC
    // else if (npc->getId() == targetId) {
    //     npc->setHealth(health);
    //     if (isDead) {
    //         checkHealth(npc->getGraphicsItem(), 0);
    //     }
    // }
    // 更新自己
    else if (player->getId() == szbuf->targetId) {
        player->setHealth(szbuf->currentHealth);
        if (szbuf->isDead) {
            checkHealth(player->getGraphicsItem(), 0);
            //半透明效果
            // player->getGraphicsItem()->setOpacity(0.5);
            // player->getTextItem()->setOpacity(0.5);
            QMessageBox::critical(this,"游戏结束","你已被击败！");
        }
    }
}

void gameclient::slot_revive(STRU_REVIVE_RS *szbuf)
{
    if(otherPlayersMap.contains(szbuf->playerId)){
        OtherPlayers* player = otherPlayersMap[szbuf->playerId];

        player->getGraphicsItem()->setOpacity(1);
        player->getTextItem()->setOpacity(1);
        //player->setHealth(0);
    }
}
