#pragma once
#include "/usr/local/mysql-8.0.35-macos13-arm64/include/mysql.h"
#include <list>
#include <string>
using namespace std;

class CMySql
{
public:
    CMySql(void);
    ~CMySql(void);
public:
    bool  ConnectMySql(const char *host,const char *user,const char *pass,const char *db);
    void  DisConnect();
    bool  SelectMySql(const char* szSql,int nColumn,list<string>& lstStr);
   
    bool  UpdateMySql(const char* szSql);
 
private:
    MYSQL *sock;   
	MYSQL_RES *results;   
	MYSQL_ROW record; 
   
};

