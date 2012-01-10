/*$Id: iceb_sql_flag.c,v 1.19 2011-02-21 07:36:08 sasa Exp $*/
/*25.08.2006	16.08.2004	Белых А.И.	iceb_sql_flag.c
Установка и снятие флага в базе данных
*/
#include "iceb_libbuh.h"

extern SQL_baza	bd;

//Конструктор
iceb_sql_flag::iceb_sql_flag(void)
{
flag.new_plus("");
metka=0;
}
//Конструктор
iceb_sql_flag::iceb_sql_flag(const char *flag_str)
{
flag.new_plus(flag_str);
metka=0;
}
//Деструктор
iceb_sql_flag::~iceb_sql_flag()
{
flag_off();
}

//Выставить флаг
int iceb_sql_flag::flag_on(const char *str_flag)
 {
  flag.new_plus(str_flag);
  return(flag_on());
 }

//Выставить флаг текущий 
int iceb_sql_flag::flag_on()
{

int voz=0;
if(flag.getdlinna() <= 1)
 return(voz);
//printf("iceb_sql_gflag-Выстaвляем флаг=%s\n",flag.ravno());
if((voz=sql_flag(&bd,flag.ravno(),0,0)) == 0)
  metka=1;
return(voz);

}

//Снять флаг
int iceb_sql_flag::flag_off()
{
if(metka == 0)
  return(0);
//printf("iceb_sql_flag-Снимаем флаг=%s\n",flag.ravno());
int voz=sql_flag(&bd,flag.ravno(),0,1);
flag.new_plus("");
return(voz);
}
