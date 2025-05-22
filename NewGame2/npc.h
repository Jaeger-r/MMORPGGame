#ifndef NPC_H
#define NPC_H

#include "player.h"
#include "TCPNet.h"
#include "tou.h"
#include "packdef.h"
#include "core.h"
class NPC:public QObject
{
    Q_OBJECT
public:
    enum NpcType { Attackable,Unattackable };
    explicit NPC(QObject* parent = nullptr);
    //NPC(const QString& id, int health, int x, int y, NpcType type);
    virtual ~NPC() {
        delete rectItem;
        delete idText;
        delete dialogBox;
        delete dialogText;
        delete InteractArea;
    }
    virtual void createNPC(QGraphicsScene*scenes,int X,int Y,QString npcName,NpcType npcType);
    virtual void onPlayerEnter(Player* player,QGraphicsScene*scene,ICore*m_core) = 0;
    virtual void onPlayerLeave(Player* player,QGraphicsScene*scene,ICore*m_core) = 0;
public:
    void setIsInRange(bool isInRange){
        isPlayerInRange = isInRange;
    }
public:
    QGraphicsRectItem* getGraphicsItem();             // 获取图形项
    QGraphicsTextItem* getTextItem();                 // 获取文本项
    QGraphicsRectItem* getInteractArea();
    void setGraphicsItem(QGraphicsRectItem *item);    // 设置图形项
    void setTextItem(QGraphicsTextItem *item);        // 设置文本项
    void updateHealth(int delta);                     // 更新血量
    int getHealth();                                  // 获取当前血量
    QString getId();                                  // 获取 ID
    NpcType getType();                                // 获取 NPC 类型
    QPointF getNpcPosition();
    int getX();
    int getY();
    bool getIsInRange(){
        return isPlayerInRange;
    }
public:
    void showDialog(QGraphicsScene* scene,const QString &message);
    void hideDialog();
private:
    QGraphicsRectItem *rectItem;     //npc本体
    QGraphicsTextItem *idText;       //npc名字
    QGraphicsRectItem *dialogBox;    //对话框背景
    QGraphicsTextItem *dialogText;   //对话内容
    QGraphicsRectItem *InteractArea; //NPC交互范围

    Player* player;

    int x;                           //npcx坐标
    int y;                           //npcy坐标
    int health;                      //npc血量
    QString npcId;                   //npcID
    NpcType npctype;                 //npc类型
    bool isPlayerInRange;      // 是否在范围内
};


#endif // NPC_H


