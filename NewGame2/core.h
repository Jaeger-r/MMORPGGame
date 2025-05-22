#ifndef CORE_H
#define CORE_H
#include "Icore.h"
#include "TCPNet.h"
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>
#include "packdef.h"
#include <QObject>
using namespace std;

class core : public QObject , public ICore
{
    Q_OBJECT
private:
    core();
    ~core();
public:
    virtual bool open();
    virtual void close();
    virtual bool sendData(QTcpSocket*socket,const char *szbuf,int nlen);
    virtual void dealData(const char *szbuf);
    virtual QTcpSocket* getSocket() {return m_pTCPNet->getSocket();}
public:

public:
    static ICore * getKernel() {
        return m_pcore;
    }
signals:
    void signal_register(STRU_REGISTER_RS*);
    void signal_login(STRU_LOGIN_RS*);
    void signal_initialize(STRU_INITIALIZE_RS*);
    void signal_chat(STRU_CHAT_RS*);
    void signal_location(STRU_LOCATION_RS*);
    void signal_save(STRU_SAVE_RS*);
    void signal_dazuo(STRU_DAZUO_RS*);
    void signal_levelup(STRU_LEVELUP_RS*);
    void signal_initbag(STRU_INITBAG_RS*);
    //wzx
    void signal_attack(STRU_ATTACK_RS *);
    void signal_revive(STRU_REVIVE_RS *);
private:
    INet *m_pTCPNet;
    static core *m_pcore;
    char m_szSystemPath[MAXSIZE];
    //std::map<long long,uploadFileInfo *> m_mapFileToFileInfo;
    ofstream outFile;
};

#endif // CORE_H
