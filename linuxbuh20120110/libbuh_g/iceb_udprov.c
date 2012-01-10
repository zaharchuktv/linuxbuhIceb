/*$Id: iceb_udprov.c,v 1.11 2011-02-21 07:36:08 sasa Exp $*/
/*11.11.2008	27.11.1997	Белых А.И.	iceb_udprov.c
Уделение проводки (пары)
Если вернули 0 удаление прошло успешно
             1 - нет
*/
#include        "iceb_libbuh.h"

extern SQL_baza	bd;

int     	iceb_udprov1(int val,short god,short mes,short den,const char*sh,const char*shk,time_t vrem,
double deb,double kre,const char*koment,short kp,GtkWidget *wpredok);



int     	iceb_udprov(int val, //0-балансовый счет -1 не балансовый
const char *datap, //Дата проводки
const char *sh,   //Счет
const char *shk,  //Счет корреспондент
time_t vrem,   //Время записи
double deb,  //Дебет
double kre,  //Кредит
const char *koment, //Коментарий
short kp, //Количество проводок
GtkWidget *wpredok)
{
short god,mes,den;
iceb_u_rsdat(&den,&mes,&god,datap,1);

return(iceb_udprov1(val,god,mes,den,sh,shk,vrem,deb,kre,koment,kp,wpredok));
}

int     	iceb_udprov(int val, //0-балансовый счет -1 не балансовый
short god,short mes,short den, //Дата проводки
const char *sh,   //Счет
const char *shk,  //Счет корреспондент
time_t vrem,   //Время записи
double deb,  //Дебет
double kre,  //Кредит
const char *koment, //Коментарий
short kp, //Количество проводок
GtkWidget *wpredok)
{
return(iceb_udprov1(val,god,mes,den,sh,shk,vrem,deb,kre,koment,kp,wpredok));
}

/*********************************************************/
/*********************************************************/

int iceb_udprov1(int val, //0-балансовый счет -1 не балансовый
short god,short mes,short den, //Дата проводки
const char *sh,   //Счет
const char *shk,  //Счет корреспондент
time_t vrem,   //Время записи
double deb,  //Дебет
double kre,  //Кредит
const char *koment, //Коментарий
short kp, //Количество проводок
GtkWidget *wpredok)
{
char		strsql[1024];
char		strok[1024];
/*
printw("udprov-%d %d.%d.%d %s %s %ld %.2f %.2f\n",
val,god,mes,den,sh,shk,vrem,deb,kre);
OSTANOV();
*/
if(iceb_pbsh(mes,god,sh,shk,gettext("Невозможно удалить проводку!"),wpredok) != 0)
 return(1);

deb=iceb_u_okrug(deb,0.01);
kre=iceb_u_okrug(kre,0.01);

sprintf(strsql,"delete from Prov where val=%d and datp='%04d-%02d-%02d' \
and sh='%s' and shk='%s' and vrem=%ld and deb=%.2f and kre=%.2f and \
komen='%s'",
val,god,mes,den,sh,shk,vrem,deb,kre,koment);

//printf("iceb_udprov-%s\n",strsql);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),wpredok);
   return(1);
  }
 else
  {
   sprintf(strok,"1 %s-%s",__FUNCTION__,gettext("Ошибка удаления записи !"));
   iceb_msql_error(&bd,strok,strsql,wpredok);
   return(1);
  }
 }
if(kp == 1 || val == -1)
  return(0);

sprintf(strsql,"delete from Prov where val=%d and datp='%04d-%02d-%02d' \
and sh='%s' and shk='%s' and vrem=%ld and deb=%.2f and kre=%.2f and \
komen='%s'",
val,god,mes,den,shk,sh,vrem,kre,deb,koment);

if(sql_zap(&bd,strsql) != 0)
 {
  sprintf(strok,"2 %s-%s",__FUNCTION__,gettext("Ошибка удаления записи !"));
  iceb_msql_error(&bd,strok,strsql,wpredok);
  return(1);
 }
return(0);   
}
