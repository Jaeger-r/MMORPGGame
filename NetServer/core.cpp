#include "core.h"
#include <QDateTime>

core *core::m_pcore = new core;
core::core() {
    JaegerDebug();
    m_pTCPNet = new TCPNet;
    m_pSQL = new CMySql;
#ifdef UDPSERVER
    m_pUDPNet = new udpnet;
#endif
}

core:: ~core(){
    JaegerDebug();
    delete m_pTCPNet;
    delete m_pSQL;
#ifdef UDPSERVER
    delete m_pUDPNet;
#endif
}

bool core::open(){
    //JaegerDebug();
    if(!m_pTCPNet->initNetWork()){
        qDebug()<<"Initialize TCPNetWork Failed！";
        return false;
    }
    else {
        qDebug()<<"Initialize TCPNetWork Success!";
    }
    if (m_pSQL->ConnectMySql("127.0.0.1","root","zhangwenjie172","disk")) {
        qDebug()<<"Database Connect Success!";
    }else {
        qDebug()<<"Database Connect Fail!";
    }
#ifdef UDPSERVER
    if(!m_pUDPNet->initNetWork()){
        cout<<"Initialize UDPNetWork Failed！"<<endl;
        return false;
    }
#endif
    outFile.open("chat_history.txt", ios::app); // 创建或打开txt文件，使用追加模式
    if (!outFile.is_open()) {
        qDebug()<< "Error opening file!";
    } else {
        qDebug()<< "File opened successfully!";
    }
    return true;
}

void core::close(){
    JaegerDebug();
    m_pTCPNet->unInitNetWork();
#ifdef UDPSERVER
    m_pUDPNet->unInitNetWork("UDPover");
#endif
    outFile.close();
}

void core::dealData(QTcpSocket* sock,char *szbuf){
    //JaegerDebug();
    switch(*szbuf){
    case _default_protocol_login_rq:
        Login_Request(sock,szbuf);
        break;
    case _default_protocol_register_rq:
        Register_Request(sock,szbuf);
        break;
    case _default_protocol_initialize_rq:
        Initialize_Request(sock,szbuf);
        break;
    case _default_protocol_location_rq:
        Location_Request(sock,szbuf);
        break;
    case _default_protocol_chat_rq:
        Sendmessage_Request(sock,szbuf);
        break;
    case _default_protocol_save_rq:
        Save_Request(sock,szbuf);
        break;
    case _default_protocol_dazuo_rq:
        Dazuo_Request(sock,szbuf);
        break;
    case _default_protocol_attack_rq:
        HandleAttack(sock,szbuf);
        break;
    case _default_protocol_initbag_rq:
        InitializeBag_Request(sock,szbuf);
        break;
    case _default_protocol_playerlist_rq:
        PlayerList_Request(sock,szbuf);
        break;
    }
}

void core::Sendmessage_Request(QTcpSocket* sock, char *szbuf){
    JaegerDebug();

    STRU_CHAT_RQ* scq = reinterpret_cast<STRU_CHAT_RQ*>(szbuf);
    STRU_CHAT_RS scs;

    // 获取当前时间
    time_t now = time(nullptr);
    tm* localTime = localtime(&now);
    char timeStr[128];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localTime);

    // 复制玩家信息，防止溢出
    strncpy(scs.player_Name, scq->player_Name, sizeof(scs.player_Name) - 1);
    scs.player_Name[sizeof(scs.player_Name) - 1] = '\0';  // 确保字符串结尾

    strncpy(scs.szbuf, scq->szbuf, sizeof(scs.szbuf) - 1);
    scs.szbuf[sizeof(scs.szbuf) - 1] = '\0';  // 确保字符串结尾

    // 获取所有客户端 socket
    QMap<quint64, QTcpSocket*>& clients = m_pTCPNet->getClientSockets();

    // 遍历 QMap 进行广播
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        QTcpSocket* client = it.value();  // 获取 socket 指针
        m_pTCPNet->sendData(client, reinterpret_cast<char*>(&scs), sizeof(scs));
    }

    // 记录聊天内容到日志文件
    std::cout << "[System Record] User " << scs.player_Name << ": " << scs.szbuf << std::endl;

    std::ofstream outFile("chat_history.txt", std::ios::app);  // 追加模式打开文件
    if (outFile.is_open()) {
        outFile << "[" << timeStr << "] User " << scs.player_Name << ": " << scs.szbuf << "\n";
        outFile.flush();
    } else {
        std::cerr << "Failed to open chat log file." << std::endl;
    }
}

void core::Register_Request(QTcpSocket *sock, char *szbuf) {
    if (!sock || !szbuf) return;  // 防止空指针异常
    JaegerDebug();
    STRU_REGISTER_RQ* srq = (STRU_REGISTER_RQ*)szbuf;
    STRU_REGISTER_RS srs;
    srs.player_Result = _register_err_;
    char szsql[MAXSIZE] = {0};
    list<string> liststr;
    sprintf(szsql,"insert into user(u_name,u_password,u_tel) values('%s','%s','%lld');",srq->player_Name,srq->player_Password,srq->player_tel);
    if (m_pSQL->UpdateMySql(szsql)) {
        sprintf(szsql,"select u_id, u_name from user where u_tel = %lld",srq->player_tel);
        m_pSQL->SelectMySql(szsql,2,liststr);
        if (liststr.size() > 0) {
            string strUserId = liststr.front();
            liststr.pop_front();
            string strUserName = liststr.front();
            liststr.pop_front();
            sprintf(szsql,"insert into user_basic_information(u_id,u_name) values('%d','%s');",std::stoi(strUserId),strUserName.c_str());
            if (m_pSQL->UpdateMySql(szsql)) {
                srs.player_Result = _register_success_;
            }
            else {
                srs.player_Result = _register_err_;
                qDebug()<<"玩家未能添加初始属性";
            }
        }
        else {
            srs.player_Result = _register_err_;
            qDebug()<<"未在user表中找到玩家";
        }
    }
    else {
        srs.player_Result = _register_err_;
        qDebug()<<"未能完成数据库注入";
    }
    m_pTCPNet->sendData(sock,(char*)&srs,sizeof(srs));
}

void core::Login_Request(QTcpSocket *sock, char *szbuf) {
    JaegerDebug();
    STRU_LOGIN_RQ* slq = (STRU_LOGIN_RQ*)szbuf;
    STRU_LOGIN_RS sls;
    char szsql[MAXSIZE] = {0};
    list<string>liststr;
    sls.player_Result = _login_usernoexist;
    sprintf(szsql,"select u_id,u_password from user where u_name = '%s';",slq->player_Name);
    m_pSQL->SelectMySql(szsql,2,liststr);
    if (liststr.size() > 0) {
        sls.player_Result = _login_passworderr;
        string strUserId = liststr.front();
        liststr.pop_front();
        string strUserPassword = liststr.front();
        liststr.pop_front();
        if (strcmp(slq->player_Password,strUserPassword.c_str()) == 0) {
            sls.player_Result = _login_success;
            sls.player_UserId = std::stoi(strUserId);
            m_pTCPNet->sendData(sock,(char*)&sls,sizeof(sls));
        }else {
            m_pTCPNet->sendData(sock,(char*)&sls,sizeof(sls));
        }
    }
}

void core::Initialize_Request(QTcpSocket *sock, char *szbuf){
    JaegerDebug();
    STRU_INITIALIZE_RQ* siq = (STRU_INITIALIZE_RQ*)szbuf;
    STRU_INITIALIZE_RS sis;

    sis.Initialize_Result = _initialize_fail;
    char szsql[MAXSIZE] = {0};
    list<string>liststr;
    sprintf(szsql,"select u_name,u_health,u_attackpower,u_attackrange,u_experience,u_level,u_defence,u_critrate,u_critdamage,u_position_x,u_position_y from user_basic_information where u_id = '%d';",siq->player_UserId);
    m_pSQL->SelectMySql(szsql,11,liststr);
    if (liststr.size() > 0) {
        //1
        string strUserName = liststr.front();
        strcpy(sis.player_Name, strUserName.c_str());
        liststr.pop_front();
        //2
        string strUserHealth = liststr.front();
        sis.health = std::stoi(strUserHealth);
        liststr.pop_front();
        //3
        string strUserAttackPower = liststr.front();
        sis.attackPower = std::stof(strUserAttackPower);
        liststr.pop_front();
        //4
        string strUserAttackRange = liststr.front();
        sis.attackRange = std::stof(strUserAttackRange);
        liststr.pop_front();
        //5
        string strUserExperience = liststr.front();
        sis.experience = std::stoll(strUserExperience);
        liststr.pop_front();
        //6
        string strUserLevel = liststr.front();
        sis.level = std::stoi(strUserLevel);
        liststr.pop_front();
        //7
        string strUserDefence = liststr.front();
        sis.defence = std::stoi(strUserDefence);
        liststr.pop_front();
        //8
        string strUserCritRate = liststr.front();
        sis.critical_rate = std::stof(strUserCritRate);
        liststr.pop_front();
        //9
        string strUserCritDamage = liststr.front();
        sis.critical_damage = std::stof(strUserCritDamage);
        liststr.pop_front();
        //10
        string strUserPositionX = liststr.front();
        sis.x = std::stof(strUserPositionX);
        liststr.pop_front();
        //11
        string strUserPositionY = liststr.front();
        sis.y = std::stof(strUserPositionY);
        liststr.pop_front();
        //12
        sis.Initialize_Result = _initialize_success;
        //13
        sis.player_UserId = siq->player_UserId;
        m_pTCPNet->sendData(sock,(char*)&sis,sizeof(sis));
    }else{
        sis.Initialize_Result = _initialize_error;
        m_pTCPNet->sendData(sock,(char*)&sis,sizeof(sis));
    }
}

void core::InitializeBag_Request(QTcpSocket *sock, char *szbuf)
{
    JaegerDebug();
    STRU_INITBAG_RQ *siq = (STRU_INITBAG_RQ*)szbuf;
    STRU_INITBAG_RS sis;
    char szsql[MAXSIZE] = {0};
    list<string>liststr;
    vector<STRU_ITEM> itemVec;
    sprintf(szsql,"select item_id,item_count from user_item where u_id = %d",siq->playerId);
    m_pSQL->SelectMySql(szsql,2,liststr);
    while(!liststr.empty()){
        string item_id = liststr.front();
        liststr.pop_front();
        string item_count = liststr.front();
        liststr.pop_front();
        itemVec.push_back({std::stoi(item_id),std::stoi(item_count)});
    }
    sis.itemCount = std::min((int)itemVec.size(), MAX_BAG_ITEM_NUM);
    for (int i = 0; i < sis.itemCount; ++i) {
        sis.playerBag[i] = itemVec[i];
    }
    m_pTCPNet->sendData(sock,(char*)&sis,sizeof(sis));
}
void core::Item_Request(QTcpSocket *sock, char *szbuf)
{
    if (!sock || !szbuf) return;  // 防止空指针异常
    JaegerDebug();
    STRU_ITEM_RQ* siq = (STRU_ITEM_RQ*)szbuf;
    STRU_ITEM_RS sis;
    char szsql[MAXSIZE] = {0};

}

void core::Location_Request(QTcpSocket *sock, char *szbuf)
{
    JaegerDebug();
    STRU_LOCATION_RQ* slq = (STRU_LOCATION_RQ*)szbuf;

    //qDebug() << "UserName:"<<slq->player_Name<<" UserID =" << slq->player_UserId
    //        << ", x =" << slq->x << ", y =" << slq->y;

    // 更新玩家位置,将玩家位置存入vector数组
    bool found = false;
    for (auto& player : players) {
        if (player.player_UserId == slq->player_UserId) {
            player.playerName = slq->player_Name;
            player.x = slq->x;
            player.y = slq->y;
            found = true;
            break;
        }
    }
    //如果没有记录，则新插入
    if (!found) {
        players.emplace_back(slq->player_UserId,slq->player_Name,slq->x, slq->y);
    }

    // 构造位置同步数据包
    STRU_LOCATION_RS sls;
    sls.playerCount = players.size();

    int i = 0;
    for (const auto& player : players) {
        if (i >= 50) break;  // 防止数组越界
        strcpy(sls.players[i].player_Name,player.playerName.toStdString().c_str());
        sls.players[i].player_UserId = player.player_UserId;
        sls.players[i].x = player.x;
        sls.players[i].y = player.y;
        ++i;
    }
    // 获取所有客户端 socket
    QMap<quint64, QTcpSocket*>& clients = m_pTCPNet->getClientSockets();

    // 遍历 QMap 进行广播
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        QTcpSocket* client = it.value();  // 获取 socket 指针
        //qDebug() << "Client ID:" << it.key()
        //         << "Address:" << client->peerAddress().toString()
        //         << "Port:" << client->peerPort();
        if (client && client != sock) {
            m_pTCPNet->sendData(client, reinterpret_cast<char*>(&sls), sizeof(sls));
        }
    }
}
void core::PlayerList_Request(QTcpSocket *sock, char *szbuf){

}

void core::Save_Request(QTcpSocket *sock, char *szbuf){
    JaegerDebug();
    STRU_SAVE_RQ* ssq = (STRU_SAVE_RQ*)szbuf;
    STRU_SAVE_RS sss;

    sss.Save_Result = _save_fail_;
    char szsql[MAXSIZE] = {0};
    list<string>liststr;
    sprintf(szsql,
            "UPDATE user_basic_information SET "
            "u_health = '%d', "
            "u_attackpower = '%d', "
            "u_attackrange = '%d', "
            "u_experience = '%lld', "
            "u_level = '%d', "
            "u_defence = '%d', "
            "u_critrate = '%.2f', "
            "u_critdamage = '%.2f' "
            "WHERE u_id = '%d';",
            ssq->health, ssq->attackPower, ssq->attackRange, ssq->experience,
            ssq->level, ssq->defence, ssq->critical_rate, ssq->critical_damage, ssq->player_UserId
            );
    if(m_pSQL->UpdateMySql(szsql)){
        sss.Save_Result = _save_success_;
        m_pTCPNet->sendData(sock,(char*)&sss,sizeof(sss));
    }else{
        sss.Save_Result = _save_error_;
        m_pTCPNet->sendData(sock,(char*)&sss,sizeof(sss));
    }
}

void core::Dazuo_Request(QTcpSocket *sock, char *szbuf)
{
    JaegerDebug();
    STRU_DAZUO_RQ* sdq = (STRU_DAZUO_RQ*)szbuf;
    STRU_DAZUO_RS sds;
    int tempid = sdq->player_UserId;
    sds.dazuoConfirm = false;

    if (sdq->isDazuo)
    {
        char szsql[MAXSIZE] = {0};
        std::list<std::string> liststr;
        sprintf(szsql, "SELECT u_level, u_experience FROM user_basic_information WHERE u_id = '%d';", tempid);
        m_pSQL->SelectMySql(szsql, 2, liststr);

        if (liststr.size() >= 2)
        {
            std::string userLevel = liststr.front(); liststr.pop_front();
            std::string userExp = liststr.front(); liststr.pop_front();

            auto playerInfo = std::make_shared<PlayerExpInfo>();
            playerInfo->level = std::stoi(userLevel);
            playerInfo->exp = std::stoll(userExp);
            playerInfo->startExp = playerInfo->exp;
            m_mapPlayerInfo[tempid] = playerInfo;

            if (m_mapDazuoTimer[tempid])
            {
                m_mapDazuoTimer[tempid]->stop();
                delete m_mapDazuoTimer[tempid];
                m_mapDazuoTimer[tempid] = nullptr;
            }

            QTimer* timer = new QTimer(this);
            QTcpSocket* psock = sock; // 避免 lambda 捕获悬空引用
            connect(timer, &QTimer::timeout, this, [=]() {
                auto info = m_mapPlayerInfo[tempid];
                long long gainedExp = info->level * 2;
                info->exp += gainedExp;
                qDebug() << "玩家ID：" << tempid << ", 打坐中... 获得经验：" << gainedExp << " 当前总经验：" << info->exp;
                bool temp = LevelUp(info->level, info->exp, tempid);
                qDebug()<<"是否升级："<<temp;
                if (temp)
                {
                    qDebug() << "玩家升级, 当前等级：" << info->level;
                    STRU_LEVELUP_RS sls;
                    sls.MaxExp = getEXP(info->level);
                    sls.userlevel = info->level;
                    m_pTCPNet->sendData(psock, (char*)&sls, sizeof(sls));
                }
            });
            timer->start(2000);
            m_mapDazuoTimer[tempid] = timer;

            sds.exp = playerInfo->exp;
            sds.userlevel = playerInfo->level;
            sds.dazuoConfirm = true;
            sds.MaxExp = getEXP(playerInfo->level);
            m_pTCPNet->sendData(sock, (char*)&sds, sizeof(sds));
        }
    }
    else
    {
        if (m_mapDazuoTimer[tempid])
        {
            m_mapDazuoTimer[tempid]->stop();
            delete m_mapDazuoTimer[tempid];
            m_mapDazuoTimer.erase(tempid);
            qDebug() << "玩家ID：" << tempid << ", 打坐结束";
        }

        // 读取经验信息
        auto infoIt = m_mapPlayerInfo.find(tempid);
        if (infoIt != m_mapPlayerInfo.end())
        {
            auto info = infoIt->second;
            char szsql[MAXSIZE] = {0};
            sprintf(szsql,
                    "UPDATE user_basic_information SET "
                    "u_experience = '%lld', "
                    "u_level = '%d' "
                    "WHERE u_id = '%d';",
                    info->exp, info->level, tempid);
            m_pSQL->UpdateMySql(szsql);

            sds.exp = info->exp;
            sds.userlevel = info->level;
            sds.dazuoConfirm = false;
            sds.addedExp = info->exp - info->startExp;
            qDebug() << "玩家ID:" << tempid << "打坐获得经验:" << sds.addedExp << "总经验:" << sds.exp;
            m_pTCPNet->sendData(sock, (char*)&sds, sizeof(sds));

            m_mapPlayerInfo.erase(tempid);
        }
    }
}

bool core::LevelUp(int& lvl,long long& userExp,int player_id)
{
    JaegerDebug();
    bool leveledUp = false;
    while(userExp>=getEXP(lvl)){
        userExp -= getEXP(lvl);
        ++lvl;
        leveledUp = true;
    }
    if(leveledUp){
        char szsql[MAXSIZE] = {0};
        sprintf(szsql,
                "UPDATE user_basic_information SET "
                "u_experience = '%lld', "
                "u_level = '%d' "
                "WHERE u_id = '%d';",
                0,lvl,player_id
                );
        m_mapPlayerInfo[player_id]->exp = 0;
        return m_pSQL->UpdateMySql(szsql);
    }
    return false;
}
void core::HandleAttack(QTcpSocket* sock, char* szbuf) {
    STRU_ATTACK_RQ* req = (STRU_ATTACK_RQ*)szbuf;

    // 获取目标玩家的所有属性
    PlayerAttributes targetAttributes = getPlayerAttributesFromDB(req->targetId);
    if (targetAttributes.health == 0) { // Use level to check invalid data
        qWarning() << "获取目标玩家" << req->targetId << "的属性失败";
        return;
    }

    // 获取攻击者的所有属性
    PlayerAttributes attackerAttributes = getPlayerAttributesFromDB(req->attackerId);
    if (attackerAttributes.health == 0) {
        qWarning() << "获取攻击者玩家" << req->attackerId << "的属性失败";
        return;
    }

    // 计算实际伤害
    int finalDamage = calculateDamage(req->attackerId, req->targetId, req->damage);
    if (finalDamage == 0) {
        qWarning() << "玩家" << req->attackerId << "的攻击无效";
        return;
    }

    // 更新目标玩家血量
    targetAttributes.health = std::max(0, targetAttributes.health - finalDamage);

    if(!updateHealthInDB(req->targetId,targetAttributes.health)){
        qWarning() << "更新目标玩家" << req->targetId << "的属性失败";
        return;
    }
    STRU_ATTACK_RS rs;
    rs.targetId = req->targetId;
    rs.currentHealth = targetAttributes.health;
    rs.isDead = (targetAttributes.health <= 0);

    // 向所有客户端发送更新
    QMap<quint64, QTcpSocket*>& clients = m_pTCPNet->getClientSockets();

    if(rs.isDead){
        updateHealthInDB(rs.targetId,0);
        // 启动3秒后复活的定时器
        QTimer::singleShot(10000, this, [=]() {
            updateHealthInDB(rs.targetId, 100); // 复活并恢复满血
            STRU_REVIVE_RS srr;
            srr.playerId = rs.targetId;
            for (auto it = clients.begin(); it != clients.end(); ++it) {
                QTcpSocket* client = it.value();
                if (client && client->state() == QAbstractSocket::ConnectedState) {
                    m_pTCPNet->sendData(client, (char*)&srr, sizeof(srr));
                }
            }
        });
    }
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        QTcpSocket* client = it.value();
        if (client && client->state() == QAbstractSocket::ConnectedState) {
            m_pTCPNet->sendData(client, (char*)&rs, sizeof(rs));
        }
    }
}



int core::calculateDamage(int attackedId, int targetId, int clientDamage)
{
    // 获取攻击者和目标的属性
    PlayerAttributes attackerAttributes = getPlayerAttributesFromDB(attackedId);
    PlayerAttributes targetAttributes = getPlayerAttributesFromDB(targetId);

    // 如果任何一个玩家的属性失败，返回0伤害
    if (attackerAttributes.health <= 0 || targetAttributes.health <= 0) {
        return 0;
    }

    // 基础伤害 = 客户端计算的伤害 + 攻击者的攻击力
    int baseDamage = clientDamage + attackerAttributes.attackPower;

    // 计算目标的防御值，减少伤害
    int reducedDamage = baseDamage - targetAttributes.defense;
    if (reducedDamage < 0) {
        reducedDamage = 0;  // 防止伤害为负数
    }

    // 判断是否暴击，暴击率计算
    float critChance = attackerAttributes.critRate;
    bool isCrit = ((rand() % 100) < (critChance * 100));  // 暴击发生的概率（比如暴击率是20%，则概率是20%）
    if (isCrit) {
        // 暴击伤害，假设暴击伤害为2倍
        reducedDamage = reducedDamage * attackerAttributes.critDamage;
        qDebug() << "暴击！伤害倍增！";
    }

    // 最终伤害 = 最大0和计算的伤害
    return std::max(0, reducedDamage);
}

PlayerAttributes core::getPlayerAttributesFromDB(int userId) {
    char szsql[MAXSIZE] = {0};
    list<string> liststr;
    PlayerAttributes attributes;

    // 查询所有需要的字段
    sprintf(szsql,
            "SELECT u_health, u_attackpower, u_defence, u_critrate, u_critdamage, "
            "u_attackrange, u_experience, u_level, u_position_x, u_position_y "
            "FROM user_basic_information WHERE u_id = %d;", userId);

    m_pSQL->SelectMySql(szsql, 10, liststr);

    if (liststr.size() < 10) {
        qWarning() << "获取玩家" << userId << "的属性失败，数据不完整";
        return attributes; // 返回默认值
    }

    // 解析数据
    attributes.health = std::stoi(liststr.front()); liststr.pop_front();
    attributes.attackPower = std::stoi(liststr.front()); liststr.pop_front();
    attributes.defense = std::stoi(liststr.front()); liststr.pop_front();
    attributes.critRate = std::stof(liststr.front()); liststr.pop_front();
    attributes.critDamage = std::stof(liststr.front()); liststr.pop_front();
    attributes.attackRange = std::stof(liststr.front()); liststr.pop_front();
    attributes.experience = std::stoll(liststr.front()); liststr.pop_front();
    attributes.level = std::stoi(liststr.front()); liststr.pop_front();
    attributes.positionX = std::stof(liststr.front()); liststr.pop_front();
    attributes.positionY = std::stof(liststr.front()); liststr.pop_front();
    // attributes.state = static_cast<PlayerState>(std::stof(liststr.front()));
    // liststr.pop_front();
    //attributes.ghostEndTime = std::stoi(liststr.front()); liststr.pop_front();

    return attributes;
}

bool core:: updateHealthInDB(int userId, int newHealth) {
    char szsql[MAXSIZE] = {0};
    sprintf(szsql, "UPDATE user_basic_information SET u_health = %d WHERE u_id = %d;",
            newHealth, userId);

    if (!m_pSQL->UpdateMySql(szsql)) {
        qCritical() << "更新玩家" << userId << "血量失败";
        return false;
    }
    return true;
}





























