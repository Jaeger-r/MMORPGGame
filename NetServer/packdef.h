#ifndef PACKDEF_H
#define PACKDEF_H
#include "tou.h"
#include <iostream>
#include <vector>

#define _default_protocol_base  10
#define _default_protocol_chat_rq                    _default_protocol_base + 1
#define _default_protocol_chat_rs                    _default_protocol_base + 2
#define _default_protocol_location_rq                _default_protocol_base + 3
#define _default_protocol_location_rs                _default_protocol_base + 4
#define _default_protocol_login_rq                   _default_protocol_base + 5
#define _default_protocol_login_rs                   _default_protocol_base + 6
#define _default_protocol_register_rq                _default_protocol_base + 7
#define _default_protocol_register_rs                _default_protocol_base + 8
#define _default_protocol_initialize_rq              _default_protocol_base + 9
#define _default_protocol_initialize_rs              _default_protocol_base + 10
#define _default_protocol_item_rq                    _default_protocol_base + 11
#define _default_protocol_item_rs                    _default_protocol_base + 12
#define _default_protocol_chathistory_rq             _default_protocol_base + 13
#define _default_protocol_chathistory_rs             _default_protocol_base + 14
#define _default_protocol_save_rq                    _default_protocol_base + 15
#define _default_protocol_save_rs                    _default_protocol_base + 16
#define _default_protocol_dazuo_rq                   _default_protocol_base + 17
#define _default_protocol_dazuo_rs                   _default_protocol_base + 18
#define _default_protocol_levelup_rq                 _default_protocol_base + 19
#define _default_protocol_levelup_rs                 _default_protocol_base + 20
#define _default_protocol_attack_rq                  _default_protocol_base + 21
#define _default_protocol_attack_rs                  _default_protocol_base + 22
#define _default_protocol_revive_rq                  _default_protocol_base + 23
#define _default_protocol_revive_rs                  _default_protocol_base + 24
#define _default_protocol_initbag_rq                 _default_protocol_base + 25
#define _default_protocol_initbag_rs                 _default_protocol_base + 26
#define _default_protocol_playerlist_rq              _default_protocol_base + 27
#define _default_protocol_playerlist_rs              _default_protocol_base + 28

//#define UDPSERVER
#define MAXSIZE 1024
#define NAMESIZE 50

#define _register_success_         0
#define _register_err_             1
#define _register_password_error   2

#define _login_usernoexist 0
#define _login_passworderr 1
#define _login_success     2

#define _initialize_success 0
#define _initialize_error   1
#define _initialize_fail    2

#define _item_success 0
#define _item_error   1
#define _item_fail    2

#define _save_success_ 0
#define _save_fail_    1
#define _save_error_   2

struct STRU_BASE{
    char m_nType;//包类型
};
/**
 * @brief 注册
 */

struct STRU_REGISTER_RQ:public STRU_BASE{
    STRU_REGISTER_RQ(){
        m_nType=_default_protocol_register_rq;
    }
    char player_Name[NAMESIZE];
    char player_Password[NAMESIZE];
    long long player_tel;
};

struct STRU_REGISTER_RS:public STRU_BASE{
    STRU_REGISTER_RS(){
        m_nType=_default_protocol_register_rs;
    }
    char player_Result;
};
/**
 * @brief 登录
 */
struct STRU_LOGIN_RQ:public STRU_BASE{
    STRU_LOGIN_RQ(){
        m_nType=_default_protocol_login_rq;
    }
    char player_Name[NAMESIZE];
    char player_Password[NAMESIZE];
};
struct STRU_LOGIN_RS:public STRU_BASE{
    STRU_LOGIN_RS(){
        m_nType=_default_protocol_login_rs;
    }
    int player_UserId;
    int player_Result;
};
/**
 * @brief 聊天
 */
struct STRU_CHAT_RQ:public STRU_BASE{
    STRU_CHAT_RQ(){
        m_nType = _default_protocol_chat_rq;
    }
    char player_Name[NAMESIZE];
    char szbuf[MAXSIZE];
};
struct STRU_CHAT_RS:public STRU_BASE{
    STRU_CHAT_RS(){
        m_nType = _default_protocol_chat_rs;
    }
    int player_UserId;
    char player_Name[NAMESIZE];
    char szbuf[MAXSIZE];
};
/**
 * @brief 玩家属性初始化
 */
struct STRU_INITIALIZE_RQ:public STRU_BASE{
    STRU_INITIALIZE_RQ(){
        m_nType = _default_protocol_initialize_rq;
    }
    int player_UserId;
};
struct STRU_INITIALIZE_RS:public STRU_BASE{
    STRU_INITIALIZE_RS(){
        m_nType = _default_protocol_initialize_rs;
    }
    char player_Name[NAMESIZE];
    int health;                   //生命值
    int attackPower;              //攻击力
    int attackRange;              //攻击范围
    long long experience;         //经验
    int level;                    //等级
    int defence;                  //防御力
    float critical_rate;          //暴击率
    float critical_damage;        //暴击伤害
    float x;                        //玩家坐标X
    float y;                        //玩家坐标Y
    char Initialize_Result;       //初始化结果
    int player_UserId;
};
/**
 * @brief 玩家背包初始化
 */
struct Item_Infomation{
    int item_id;
    char item_Name[50];
    int item_type;
    char item_icon[255];
    float item_attack;
    int item_levelrequirement;
    int item_rarity;
    std::string item_decoration;
};

struct STRU_ITEM_RQ:public STRU_BASE{
    STRU_ITEM_RQ(){
        m_nType = _default_protocol_item_rq;
    }
    int player_UserId;
};
struct STRU_ITEM_RS:public STRU_BASE{
    STRU_ITEM_RS(){
        m_nType = _default_protocol_item_rs;
    }
    std::vector<Item_Infomation> Bag_Infomation;
    int player_UserId;
    char Item_Result;
};
/**
 * @brief 实时发送玩家坐标
 */
struct PlayerLocation{
    int player_UserId;
    char player_Name[NAMESIZE];
    float x;
    float y;
};
struct STRU_LOCATION_RQ:public STRU_BASE{
    STRU_LOCATION_RQ(){
        m_nType = _default_protocol_location_rq;
    }
    int player_UserId;
    char player_Name[NAMESIZE];
    float x;
    float y;
};
struct STRU_LOCATION_RS:public STRU_BASE{
    STRU_LOCATION_RS(){
        m_nType = _default_protocol_location_rs;
    }
    int playerCount;
    PlayerLocation players[50];
};
/**
 * @brief 定时向服务器申请保存
 */
struct STRU_SAVE_RQ:public STRU_BASE{
    STRU_SAVE_RQ(){
        m_nType = _default_protocol_save_rq;
    }
    int player_UserId;            //玩家ID
    int health;                   //生命值
    int attackPower;              //攻击力
    int attackRange;              //攻击范围
    long long experience;         //经验
    int level;                    //等级
    int defence;                  //防御力
    float critical_rate;          //暴击率
    float critical_damage;        //暴击伤害
};
struct STRU_SAVE_RS:public STRU_BASE{
    STRU_SAVE_RS(){
        m_nType = _default_protocol_save_rs;
    }
    char Save_Result;
};
/**
 * @brief 打坐申请
 */
struct STRU_DAZUO_RQ:public STRU_BASE{
    STRU_DAZUO_RQ(){
        m_nType = _default_protocol_dazuo_rq;
    }
    int player_UserId;
    bool isDazuo;
};
struct STRU_DAZUO_RS:public STRU_BASE{
    STRU_DAZUO_RS(){
        m_nType = _default_protocol_dazuo_rs;
    }
    long long exp;
    int userlevel;
    long long addedExp;
    long long MaxExp;
    bool dazuoConfirm;
};
/**
 * @brief 升级协议
 */
struct STRU_LEVELUP_RQ:public STRU_BASE{
    STRU_LEVELUP_RQ(){
        m_nType = _default_protocol_levelup_rq;
    }
    int player_UserId;
};
struct STRU_LEVELUP_RS:public STRU_BASE{
    STRU_LEVELUP_RS(){
        m_nType = _default_protocol_levelup_rs;
    }
    long long MaxExp;
    int userlevel;
};
/**
 * @brief 攻击包
 */
struct STRU_ATTACK_RQ :public STRU_BASE{
    STRU_ATTACK_RQ(){
        m_nType = _default_protocol_attack_rq;
    }
    int attackerId;  // 攻击者ID
    int targetId;    // 被攻击玩家ID
    float damage;    // 客户端计算的伤害值（用于校验）
};

struct STRU_ATTACK_RS:public STRU_BASE {
    STRU_ATTACK_RS(){
        m_nType = _default_protocol_attack_rs;
    }
    int targetId;
    int currentHealth; // 更新后的血量
    bool isDead;       // 是否死亡
};
/**
 * @brief 重生协议
 */
struct STRU_REVIVE_RQ:public STRU_BASE{
    STRU_REVIVE_RQ(){
        m_nType = _default_protocol_revive_rq;
    }
};
struct STRU_REVIVE_RS:public STRU_BASE{
    STRU_REVIVE_RS(){
        m_nType = _default_protocol_revive_rs;
    }
    int playerId;
    int newhealth;
};
/**
 * @brief 初始化背包
 */
 struct STRU_ITEM {
     int item_id;
     int item_count;
 };
struct STRU_INITBAG_RQ:public STRU_BASE{
    STRU_INITBAG_RQ(){
        m_nType = _default_protocol_initbag_rq;
    }
    int playerId;
};
struct STRU_INITBAG_RS:public STRU_BASE{
    STRU_INITBAG_RS(){
        m_nType = _default_protocol_initbag_rs;
    }
    int itemCount;
    STRU_ITEM playerBag[MAX_BAG_ITEM_NUM];
};
/**
 * @brief 获取玩家列表
 */
struct STRU_PLAYERLIST_RQ:public STRU_BASE{
    STRU_PLAYERLIST_RQ(){
        m_nType = _default_protocol_playerlist_rq;
    }
    int playerId;
};
struct STRU_PLAYERLIST_RS:public STRU_BASE{
    STRU_PLAYERLIST_RS(){
        m_nType = _default_protocol_playerlist_rs;
    }

};
#endif // PACKDEF_H

