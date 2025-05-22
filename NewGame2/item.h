#ifndef ITEM_H
#define ITEM_H
#include "tou.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

enum class ItemType { Weapon, Armor, Consumable, Misc };
///weapon 1,Armor 2,Consumable 3,Misc 4
///consumable: Health 1, Mana 2, Buff 3, Debuff 4
/**
    INSERT INTO items (item_name, item_type, rarity, level_requirement, description, icon_path)
    VALUES ('长剑', 2, 1, 5, '一把普通的长剑，适合新手使用。', 'icons/armor/sword.bmp');
    SET @last_id = LAST_INSERT_ID();
    INSERT INTO weapons (item_id, attack, durability,critical_rate,critical_damage)
    VALUES (@last_id, 50, 100, 0.01,2.0);

    INSERT INTO items (item_name, item_type, rarity, level_requirement, description, icon_path)
    VALUES ('木盾', 2, 1, 5, '一把普通的木盾，适合新手使用。', 'icons/armor/woodshield.bmp');
    SET @last_id = LAST_INSERT_ID();
    INSERT INTO armors (item_id, defence, durability)
    VALUES (@last_id, 50, 100);

    INSERT INTO items (item_name, item_type, rarity, level_requirement, description, icon_path)
    VALUES ('苹果', 3, 1, 5, '一只刚从树上掉下来的苹果，可以恢复你的健康和美好心情。', 'icons/Consumable/apple.bmp');
    SET @last_id = LAST_INSERT_ID();
    INSERT INTO consumable (item_id, effect_type, effect_value, duration, max_stack, use_limit, cooldown)
    VALUES (@last_id, 1, 10,0,100,1,5);

    INSERT INTO items (item_name, item_type, rarity, level_requirement, description, icon_path)
    VALUES ('水瓶', 3, 1, 5, '一瓶水，可以恢复你的法力。', 'icons/Consumable/waterbottle.bmp');
    SET @last_id = LAST_INSERT_ID();
    INSERT INTO consumable (item_id, effect_type, effect_value, duration, max_stack, use_limit, cooldown)
    VALUES (@last_id, 2, 10,0,100,1,5);

    INSERT INTO items (item_name, item_type, rarity, level_requirement, description, icon_path)
    VALUES ('熟牛肉', 3, 1, 5, '一块刚出锅的热乎牛肉，温暖你的胃，同时也使你更加兴奋。', 'icons/Consumable/Hotbeef.bmp');
    SET @last_id = LAST_INSERT_ID();
    INSERT INTO consumable (item_id, effect_type, effect_value, duration, max_stack, use_limit, cooldown)
    VALUES (@last_id, 3, 10,20,100,1,30);

    INSERT INTO items (item_name, item_type, rarity, level_requirement, description, icon_path)
    VALUES ('坏苹果', 3, 1, 5, '一只从树上掉下来很久的苹果，除了给你带来恶心的感觉以外，什么用也没有。', 'icons/Consumable/Badapple.bmp');
    SET @last_id = LAST_INSERT_ID();
    INSERT INTO consumable (item_id, effect_type, effect_value, duration, max_stack, use_limit, cooldown)
    VALUES (@last_id, 4, 10,10,100,1,0);

    INSERT INTO items (item_name, item_type, rarity, level_requirement, description, icon_path)
    VALUES ('蓝色布衫', 3, 1, 5, '一只从树上掉下来很久的苹果，除了给你带来恶心的感觉以外，什么用也没有。', 'icons/Consumable/Badapple.bmp');
    SET @last_id = LAST_INSERT_ID();
    INSERT INTO consumable (item_id, effect_type, effect_value, duration, max_stack, use_limit, cooldown)
    VALUES (@last_id, 4, 10,10,100,1,0);
 **/
class Item
{
public:
    Item();
    ~Item();
    // 获取基本信息
    int getId() const;
    QString getName() const;
    QString getType() const;
    QString getIconPath() const;
    int getAttack() const;
    int getDefense() const;
    int getDurability() const;
    int getMagicResistance() const;
    int getHeal() const;
    int getManaRestore() const;
    int getQuestId() const;
    int getAmount() const;
    int getLevelRequirement() const;
    QString getRarity() const;
    QString getDescription() const;

    int getQuantity() const;

    // 修改数量
    void setQuantity(int newQuantity);
    void addQuantity(int id,int amount);
    void loadItemsFromJson(const QString &filePath);

    virtual void use();
private:
    QMap<int, Item> itemsDatabase;

    int id;
    QString name;
    QString type;
    QString iconPath;
    int attack = 0;
    int defense = 0;
    int durability = 0;
    int magicResistance = 0;
    int heal = 0;
    int manaRestore = 0;
    int questId = 0;
    int amount = 0;
    int levelRequirement = 0;
    QString rarity;
    QString description;
};



#endif // ITEM_H

