/*$Id: iceb_lock_tables.c,v 1.8 2011-01-13 13:49:59 sasa Exp $*/
/*18.02.2009	22.02.2004 	Белых А.И.	iceb_lock_tables.c
Класс для блокировки/разблокировки таблиц
*/
#include "iceb_libbuh.h"

extern SQL_baza	bd;

iceb_lock_tables::iceb_lock_tables() //конструктор без параметров
 {
  metka=0;
 }

iceb_lock_tables::iceb_lock_tables(iceb_u_str *tabl) //конструктор с параметрами
 {
  lock(tabl);
 }

iceb_lock_tables::iceb_lock_tables(const char *tabl) //конструктор с параметрами
 {
  lock(tabl);
 }

iceb_lock_tables::~iceb_lock_tables() //деструктор
 {
  unlock();    
 }

int iceb_lock_tables::lock(const char *tabl) //блокировка таблиц
 {
  if(sql_zap(&bd,tabl) != 0)
   {
    iceb_msql_error(&bd,"Ошибка блокировки таблицы !",tabl,NULL);
    return(1);
   } 
//  printf("iceb_lock_tables-Таблицы заблокированы.\n");
  metka=1;
  return(0);    
 }

int iceb_lock_tables::lock(iceb_u_str *tabl) //блокировка таблиц
 {
  if(sql_zap(&bd,tabl->ravno()) != 0)
    return(1);
    
  metka=1;
  return(0);    
 }

int iceb_lock_tables::unlock() //разблокировка таблиц
 {
  if(metka == 0)
   return(0);

  char strsql[50];
  strcpy(strsql,"UNLOCK TABLES");

  if(sql_zap(&bd,strsql) != 0)
   {
    iceb_msql_error(&bd,"Ошибка разблокировки таблицы !",strsql,NULL);
    return(1);
   }
  metka=0;
//  printf("iceb_lock_tables-Таблицы разблокированы.\n");
  
  return(0);    

 }
