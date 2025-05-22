#include "CMySql.h"

#include <qDebug>

CMySql::CMySql(void)
{
   
    sock = new MYSQL;
    mysql_init(sock );
    mysql_set_character_set(sock,"utf8");
}


CMySql::~CMySql(void)
{
    if(sock)
    {
        delete sock;
        sock = NULL;
    }
    
}

void CMySql::DisConnect()
{
    mysql_close(sock);
}

bool CMySql::ConnectMySql(const char *host,const char *user,const char *pass,const char *db)
{
  
   
	if (!mysql_real_connect(sock, host, user, pass, db, 0, NULL, 0))
	{
        qDebug() << "MySQL Connection Failed: " << mysql_error(sock);
		return false;
	}
	
    return true;
}

bool CMySql::SelectMySql(const char* szSql,int nColumn,list<string>& lstStr)
{

    if(mysql_query(sock,szSql))return false;

	results=mysql_store_result(sock);
    if(NULL == results)return false;
 
	while (record = mysql_fetch_row(results))
	{
       
		 for(int i = 0;i < nColumn;i++)
         { 
			 if(NULL == record[i])
			 {
                 record[i] = (char*)"null";
			 }
             lstStr.push_back(record[i]);
         }
    

	}

    return true;
}

 bool  CMySql::UpdateMySql(const char* szSql)
 {
    if(!szSql)return false;

    if(mysql_query(sock,szSql))return false;

    return true;
 }


