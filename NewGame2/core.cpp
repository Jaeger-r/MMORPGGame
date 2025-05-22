#include "core.h"

core *core::m_pcore = new core;
core::core() {
    m_pTCPNet = new TCPNet;
#ifdef UDPSERVER
    m_pUDPNet = new udpnet;
#endif
}

core:: ~core(){
    delete m_pTCPNet;
#ifdef UDPSERVER
    delete m_pUDPNet;
#endif
}

bool core::open(){
    if(!m_pTCPNet->initNetWork()){
        qDebug()<<"Initialize TCPNetWork Failed！";
        return false;
    }
    else {
        qDebug()<<"Initialize TCPNetWork Success!";
    }
#ifdef UDPSERVER
    if(!m_pUDPNet->initNetWork()){
        cout<<"Initialize UDPNetWork Failed！"<<endl;
        return false;
    }
#endif
    return true;
}

void core::close(){
    m_pTCPNet->unInitNetWork();
#ifdef UDPSERVER
    m_pUDPNet->unInitNetWork("UDPover");
#endif
    outFile.close();
}

bool core::sendData(QTcpSocket*socket,const char *szbuf,int nlen)
{
    if(!m_pTCPNet->sendData(socket,szbuf,nlen)) {
        return false;
    }
    return true;
}

void core::dealData(const char *szbuf){
    switch(*szbuf){
        case _default_protocol_chat_rs:
            emit signal_chat((STRU_CHAT_RS*)szbuf);
            break;
        case _default_protocol_register_rs:
            emit signal_register((STRU_REGISTER_RS*)szbuf);
            break;
        case _default_protocol_login_rs:
            emit signal_login((STRU_LOGIN_RS*)szbuf);
            break;
        case _default_protocol_initialize_rs:
            emit signal_initialize((STRU_INITIALIZE_RS*)szbuf);
            break;
        case _default_protocol_location_rs:
            emit signal_location((STRU_LOCATION_RS*)szbuf);
            break;
        case _default_protocol_save_rs:
            emit signal_save((STRU_SAVE_RS*)szbuf);
            break;
        case _default_protocol_dazuo_rs:
            emit signal_dazuo((STRU_DAZUO_RS*)szbuf);
            break;
        case _default_protocol_levelup_rs:
            emit signal_levelup((STRU_LEVELUP_RS*)szbuf);
            break;
        case _default_protocol_attack_rs:
            emit signal_attack((STRU_ATTACK_RS*)szbuf);
            break;
        case _default_protocol_revive_rs:
            emit signal_revive((STRU_REVIVE_RS*)szbuf);
            break;
        case _default_protocol_initbag_rs:
            emit signal_initbag((STRU_INITBAG_RS*)szbuf);
            break;
    }
}

