#include "otherplayers.h"

OtherPlayers::OtherPlayers() : rectItem(nullptr), idText(nullptr) {}
OtherPlayers::~OtherPlayers() {}

QGraphicsEllipseItem *OtherPlayers::getGraphicsItem()
{
    return rectItem;
}

QGraphicsTextItem *OtherPlayers::getTextItem()
{
    return idText;
}

void OtherPlayers::updateTextPosition()
{
    if (!rectItem || !idText) {
        qDebug() << "Warning: rectItem or idText is nullptr!";
        return;
    }
    QPointF otherplayerPos = rectItem->pos();

    getTextItem()->setPos(otherplayerPos.x(), otherplayerPos.y() - PLAYER_TEXTPOSITION);
}

void OtherPlayers::createPlayer(QGraphicsScene *scenes, int id,QString name, float X, float Y)
{
    player_id = id;
    playerName = name;
    x = X;
    y = Y;

    rectItem = new QGraphicsEllipseItem(0, 0, PLAYER_SIZE, PLAYER_SIZE);
    rectItem->setPos(X,Y);
    rectItem->setBrush(QBrush(Qt::red));  // 设置填充颜色
    rectItem->setPen(QPen(Qt::black, 2));  // 设置边框
    scenes->addItem(rectItem);

    // 创建显示玩家ID的文本项
    idText = new QGraphicsTextItem(name);
    idText->setDefaultTextColor(Qt::black);
    idText->setPos(X, Y - PLAYER_SIZE - 10); // 初始位置
    scenes->addItem(idText);

    updateTextPosition();
}

void OtherPlayers::updateLocation(float x, float y){
    setX(x);
    setY(y);
    rectItem->setPos(x,y);
    updateTextPosition();
}

void OtherPlayers::setHealth(int health){
    this->health=health;
}
void OtherPlayers::setId(int id){
    player_id = id;
}
void OtherPlayers::setName(QString name){
    playerName = name;
}
void OtherPlayers::setX(float X) {
    x = X;
}
void OtherPlayers::setY(float Y) {
    y = Y;
}
void OtherPlayers::setGraphicsItem(QGraphicsEllipseItem *item){
    rectItem = item;
}

void OtherPlayers::setTextItem(QGraphicsTextItem *item) {
    idText = item;
}

int OtherPlayers::getId(){
    return player_id;
}
QString OtherPlayers::getName() {
    return playerName;
}
float OtherPlayers::getX() {
    return x;
}
float OtherPlayers::getY() {
    return y;
}
int OtherPlayers::getHealth() {
    return health;
}

