#include "player.h"

Player::Player()
    : playerName("none"),
    health(0),
    attackPower(0),
    attackRange(0),
    experience(0),
    level(0),
    defence(0),
    critical_rate(0.0f),
    critical_damage(0.0f),
    x(0.0f),
    y(0.0f),
    player_dir(Player::s),
    player_id(0),
    rectItem(nullptr),
    idText(nullptr),
    arrow(nullptr) {}

Player::~Player() {}

void Player::createPlayer(QGraphicsScene* scenes, QString name, int hp, int atkPower, int atkRange,
                          long long exp, int lvl, int def, float critRate, float critDmg,
                          float posX, float posY, PlayerDirection dir, int id) {
    playerName = name;
    health = hp;
    attackPower = atkPower;
    attackRange = atkRange;
    experience = exp;
    level = lvl;
    defence = def;
    critical_rate = critRate;
    critical_damage = critDmg;
    x = posX;
    y = posY;
    player_dir = dir;
    player_id = id;
    // 创建小人并添加到场景
    rectItem = new QGraphicsEllipseItem(0, 0, PLAYER_SIZE, PLAYER_SIZE);
    rectItem->setPos(PLAYER_CHUSHILOCATION_X,PLAYER_CHUSHILOCATION_Y);
    rectItem->setBrush(QBrush(Qt::cyan));  // 设置填充颜色
    rectItem->setPen(QPen(Qt::black, 2));  // 设置边框
    scenes->addItem(rectItem);

    // 创建显示玩家ID的文本项
    idText = new QGraphicsTextItem(playerName);
    idText->setDefaultTextColor(Qt::cyan);
    idText->setPos(PLAYER_CHUSHILOCATION_X, PLAYER_CHUSHILOCATION_Y - PLAYER_SIZE - 10); // 初始位置
    scenes->addItem(idText);

    // 创建箭头
    QPolygonF arrowShape;
    arrowShape << QPointF(0, -10)   // 顶点
               << QPointF(-5, 0)    // 左下
               << QPointF(5, 0);    // 右下

    arrow = new QGraphicsPolygonItem(arrowShape);
    arrow->setBrush(QBrush(Qt::yellow));  // 设置箭头颜色
    arrow->setPos(rectItem->pos());
    scenes->addItem(arrow);

    // 设置箭头初始位置
    updateArrowPosition();
    updateTextPosition();
}

void Player::updateExperience(long long exp)
{
    experience += exp;
}
void Player::updateHealth(int delta) {
    health += delta;
    if(health <= 0) {
        health = 0;
    }
}
void Player::updateLevel(int lvl)
{
    level += lvl;
}
void Player::updateArrowPosition() {
    if (!rectItem || !arrow) return;

    QPointF playerPos = rectItem->pos();         // 获取玩家方块的位置
    QPointF arrowPos;                            // 箭头的新位置
    qreal rotation = 0;                          // 箭头的旋转角度

    switch (player_dir) {
    case Player::n: // 北
        arrowPos = playerPos + QPointF(PLAYER_SIZE / 2, -10);
        rotation = 0;
        break;
    case Player::ne: // 东北
        arrowPos = playerPos + QPointF(PLAYER_SIZE + 5, -5);
        rotation = 45;
        break;
    case Player::e: // 东
        arrowPos = playerPos + QPointF(PLAYER_SIZE + 10, PLAYER_SIZE / 2);
        rotation = 90;
        break;
    case Player::se: // 东南
        arrowPos = playerPos + QPointF(PLAYER_SIZE + 5, PLAYER_SIZE + 5);
        rotation = 135;
        break;
    case Player::s: // 南
        arrowPos = playerPos + QPointF(PLAYER_SIZE / 2, PLAYER_SIZE + 10);
        rotation = 180;
        break;
    case Player::sw: // 西南
        arrowPos = playerPos + QPointF(-5, PLAYER_SIZE + 5);
        rotation = 225;
        break;
    case Player::w: // 西
        arrowPos = playerPos + QPointF(-10, PLAYER_SIZE / 2);
        rotation = 270;
        break;
    case Player::nw: // 西北
        arrowPos = playerPos + QPointF(-5, -5);
        rotation = 315;
        break;
    }

    arrow->setPos(arrowPos);       // 更新箭头位置
    arrow->setRotation(rotation); // 更新箭头方向
}

void Player::updateTextPosition()
{
    if (!rectItem || !idText) return;
    QPointF playerPos = rectItem->pos();

    getTextItem()->setPos(playerPos.x(), playerPos.y() - PLAYER_TEXTPOSITION);
}

//get
QGraphicsEllipseItem* Player::getGraphicsItem(){
    return rectItem;
}
QGraphicsTextItem* Player::getTextItem() {
    return idText;
}
QGraphicsPolygonItem* Player::getArrorLocation() {
    return arrow;
}
int Player::getId(){
    return player_id;
}
int Player::getHealth() {
    return health;
}
int Player::getAttackPower() {
    return attackPower;
}
int Player::getAttackRange() {
    return attackRange;
}
float Player::getX() {
    return x;
}
float Player::getY() {
    return y;
}
QString Player::getName() {
    return playerName;
}

long long Player::getExperience(){
    return experience;
}

int Player::getLevel(){
    return level;
}

int Player::getDefence(){
    return defence;
}

float Player::getCritical_Rate(){
    return critical_rate;
}

float Player::getCritical_Damage(){
    return critical_damage;
}
Player::PlayerDirection Player::getDirection() {
    return player_dir;
}

//set
void Player::setId(int id){
    player_id = id;
}
void Player::setName(QString name){
    playerName = name;
}
void Player::setExperience(long long exp){
    experience = exp;
}
void Player::setLevel(int lev){
    level = lev;
}
void Player::setDefence(int def){
    defence = def;
}
void Player::setCritical_Rate(float crit_rate){
    critical_rate = crit_rate;
}
void Player::setCritical_Damage(float crit_damage){
    critical_damage = crit_damage;
}
void Player::setHealth(int Health) {
    health = Health;
}
void Player::setAttackPower(int AttackPower) {
    attackPower = AttackPower;
}
void Player::setAttackRange(int AttackRange) {
    attackRange = AttackRange;
}
void Player::setX(float X) {
    x = X;
}
void Player::setY(float Y) {
    y = Y;
}
void Player::setDirection(PlayerDirection player_Dir) {
    player_dir = player_Dir;
}
void Player::setGraphicsItem(QGraphicsEllipseItem *item) {
    rectItem = item;
}
void Player::setTextItem(QGraphicsTextItem *item) {
    idText = item;
}

