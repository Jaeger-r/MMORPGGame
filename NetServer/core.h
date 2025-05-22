#ifndef CORE_H
#define CORE_H
#include "Icore.h"
#include "server2/tcpnet.h"
#include "mysql/CMySql.h"
#include <iostream>
#include <map>
#include <vector>
#include <fstream>  // 添加头文件以使用文件操作
#include <string>   // 添加头文件以使用string类型
#include <ctime>    // 添加头文件以获取时间信息
#include <algorithm>  // 包含头文件以使用 std::find
#include "packdef.h"
#include <QTimer>
#include <math.h>
enum PlayerState{Normal,Ghost};
struct Player_Information{
    int player_UserId;
    QString playerName;
    float x;
    float y;
    Player_Information(int id ,QString name,float px, float py):player_UserId(id),playerName(name),x(px),y(py){}
};
struct PlayerExpInfo {
    int level;
    long long exp;
    long long startExp;
};
struct PlayerAttributes {
    std::string name;
    int health;
    int attackPower;
    int defense;
    float critRate;
    float critDamage;
    float attackRange;
    float experience;
    int level;
    float positionX;
    float positionY;
};
class core:public ICore
{
private:
    core();
    ~core();
public:
    virtual bool open();
    virtual void close();
    Q_INVOKABLE virtual void dealData(QTcpSocket* sock,char*szbuf);
public:
    void Sendmessage_Request(QTcpSocket* sock,char* szbuf);
    void Register_Request(QTcpSocket* sock,char* szbuf);
    void Login_Request(QTcpSocket* sock,char* szbuf);
    void Initialize_Request(QTcpSocket* sock,char* szbuf);
    void Item_Request(QTcpSocket* sock,char* szbuf);
    void Location_Request(QTcpSocket* sock,char* szbuf);
    void Save_Request(QTcpSocket* sock,char* szbuf);
    void Dazuo_Request(QTcpSocket* sock,char* szbuf);
    void HandleAttack(QTcpSocket* sock,char* szbuf);
    void InitializeBag_Request(QTcpSocket* sock,char* szbuf);
    void PlayerList_Request(QTcpSocket* sock,char* szbuf);
public:
    void ItemInDB();
    int getHealth(int userId);
    int calculateDamage(int attackedId,int targetId,int clientDamage);
    PlayerAttributes getPlayerAttributesFromDB(int userId);
    bool updateHealthInDB(int userId, int newHealth);
    bool updatePlayerInDB(int userId, const PlayerAttributes &attr);
public:
    bool LevelUp(int& lvl,long long& userExp,int player_id);
public:
    //单例模式--不支持线程安全
    //饿汉模式，支持线程安全 高效
    static ICore * getKernel(){
        return m_pcore;
    }
private:
    QTimer *dazuoTimer;
    INet *m_pTCPNet;
    CMySql *m_pSQL;
    static core *m_pcore;
    char m_szSystemPath[MAXSIZE];
    //std::map<long long,uploadFileInfo *> m_mapFileToFileInfo;
    std::vector<Player_Information>players;
    ofstream outFile;

    std::unordered_map<int, QTimer*> m_mapDazuoTimer;
    std::unordered_map<int, std::shared_ptr<PlayerExpInfo>> m_mapPlayerInfo;
private:
    long long getEXP(int level){
        return 100 * level + 50 * pow(level - 1, 3);
    }
    double getHP(int level) {
        return 100 + 80 * level + 10 * pow(level - 1, 2);
    }

    double getATK(int level) {
        return 10 + 6 * level + 0.5 * pow(level - 1, 2);
    }

    double getDEF(int level) {
        return 5 + 5 * level + 0.8 * pow(level - 1, 1.5);
    }
};

#endif // CORE_H
