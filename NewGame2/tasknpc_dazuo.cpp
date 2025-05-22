#include "tasknpc_dazuo.h"

TaskNPC_Dazuo::TaskNPC_Dazuo(QObject* parent)
    : NPC(parent), currentPlayer(nullptr)
{

}

void TaskNPC_Dazuo::createNPC(QGraphicsScene*scenes,int X,int Y,QString npcName,NpcType npcType) {
    NPC::createNPC(scenes,X,Y,npcName,npcType);
}

void TaskNPC_Dazuo::onPlayerEnter(Player *player,QGraphicsScene*scene,ICore*m_core)
{
    JaegerDebug();
    if (meditating) return;

    meditating = true;
    showDialog(scene,"你好，我是张文帝，来这里打坐吧");

    STRU_DAZUO_RQ sdq;
    sdq.player_UserId = player->getId();
    sdq.isDazuo = true;
    m_core->sendData(m_core->getSocket(),(char*)&sdq,sizeof(sdq));
}

void TaskNPC_Dazuo::onPlayerLeave(Player *player,QGraphicsScene*scene,ICore*m_core)
{
    if (!meditating) return;

    hideDialog();
    meditating = false;

    STRU_DAZUO_RQ sdq;
    sdq.player_UserId = player->getId();
    sdq.isDazuo = false;
    m_core->sendData(m_core->getSocket(),(char*)&sdq,sizeof(sdq));
}

