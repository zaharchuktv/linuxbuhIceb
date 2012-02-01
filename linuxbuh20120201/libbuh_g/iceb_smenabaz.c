/*$Id: iceb_smenabaz.c,v 1.15 2011-02-21 07:36:08 sasa Exp $*/
/*18.04.2010	12.04.2004	Белых А.И.	iceb_smenabaz.c
Смена базы данных
Если вернули 0-базу сменили
             -1-нет
*/
#include <pwd.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include "iceb_libbuh.h"


extern SQL_baza	bd;
extern char *host;
extern char *parol;
extern char *imabaz;
extern char *organ;

//int iceb_smenabaz(char **host,char *parol,
//char **imabaz,char **organ,int mopen,GtkWidget *wpredok) //0-открыть и закрыть 1-закрыть и открыть
int iceb_smenabaz(int mopen,GtkWidget *wpredok) //0-открыть и закрыть 1-закрыть и открыть
{
iceb_u_str imabaz_v;
iceb_u_str host_v;
imabaz_v.plus("");
host_v.plus(host);

FILE *ff;
if((ff = fopen(".spisokbaz.alx","r")) == NULL)
 {
  if(errno != ENOENT)
   {
    iceb_er_op_fil(".spisokbaz.alx","",errno,wpredok);
    return(-1);
   }

  if(errno == ENOENT) /*Файл не существует*/
   {
/*********
    if(iceb_smenabaz_mysql(&imabaz_v,0,wpredok) != 0)
     if(iceb_smenabaz_m(&imabaz_v,&host_v) != 0)
       return(-1);
****************/
     if(iceb_l_smenabaz(&imabaz_v,0,wpredok) != 0)
      return(-1);
    
   }
 }
else
 {
  char bros[512];
  char stroka[1024];
  iceb_u_spisok baza;
  iceb_u_spisok naimb;
  iceb_u_spisok sp_host;
  while(fgets(stroka,sizeof(stroka),ff) != NULL)
   {
  //  printw("%s\n",stroka);
  //  refresh();
    if(stroka[0] == '#')
      continue;
    if(iceb_u_polen(stroka,bros,sizeof(bros),1,'|') != 0)
      continue;
    baza.plus(bros);
    
    iceb_u_polen(stroka,bros,sizeof(bros),2,'|');
    naimb.plus(bros);


    iceb_u_polen(stroka,bros,sizeof(bros),3,'|');
    sp_host.plus(bros);    
   }

  fclose(ff);

  class iceb_u_spisok naim_kol;

  naim_kol.plus(gettext("База"));
  naim_kol.plus(gettext("Наименование"));
  iceb_u_str naim_menu;
  naim_menu.plus(gettext("Выбор базы данных"));
  int nomer;  
  if((nomer=iceb_l_spisokm(&baza,&naimb,&naim_kol,&naim_menu,0,wpredok)) < 0)
    return(1);

  imabaz_v.new_plus(baza.ravno(nomer));
  host_v.new_plus(sp_host.ravno(nomer));
 }


struct  passwd  *ktor; /*Кто работает*/
ktor=getpwuid(getuid());
/*********
char *host_open;
if(host_v.getdlinna() <= 1)
  host_open=*host;
else
 {
  host_open=(char*)host_v.ravno();
 }
*************/
class iceb_u_str host_open;
if(host_v.getdlinna() <= 1)
  host_open.new_plus(host);
else
 {
  host_open.new_plus(host_v.ravno());
 }

//printf("\nhost=%s parol=%s imabaz=%s\n",host_open.ravno(),parol,imabaz_v.ravno());

if(mopen == 0)
 if(sql_openbaz(&bd,imabaz_v.ravno(),host_open.ravno(),ktor->pw_name,parol) != 0)
  {
   iceb_eropbaz(imabaz_v.ravno(),ktor->pw_uid,ktor->pw_name,1);
   return(-1);
  }

if(mopen == 1)
 {
  sql_closebaz(&bd);

  if(sql_openbaz(&bd,imabaz_v.ravno(),host_open.ravno(),ktor->pw_name,parol) != 0)
   {
    iceb_eropbaz(imabaz_v.ravno(),ktor->pw_uid,ktor->pw_name,1);

    if(sql_openbaz(&bd,imabaz,host,ktor->pw_name,parol) != 0)
     {
    /*  printw("%s\n",sql_error(&bd));*/
    /*  OSTANOV();*/
     }

    return(-1);
   }

 }


if(host_v.getdlinna() > 1)
 {
  delete [] host;
  host=new char[strlen(host_v.ravno())+1];
  strcpy(host,host_v.ravno());
 }

delete [] imabaz;

imabaz=new char[strlen(imabaz_v.ravno())+1];
strcpy(imabaz,imabaz_v.ravno());
/*Читаем после присвоения имени базы*/
iceb_start_rf(); /*Чтение и выполнение запросов к базе данных*/

/*Читаем наименование организации*/
if(organ != NULL)
 {
  free(organ);
  organ=NULL;
 }
char strsql[512];
SQL_str row;
SQLCURSOR cur;
sprintf(strsql,"select naikon from Kontragent where kodkon='00'");

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_vkk00(NULL);
/**************
  iceb_u_str repl;
  repl.plus(gettext("Не найден код контрагента 00 !"));
  iceb_menu_soob(&repl,wpredok);
  
  int i=1+strlen(gettext("Не найден код контрагента 00 !"));
  organ=new char[i];
  strcpy(organ,gettext("Не найден код контрагента 00 !"));
***************/    
 }
else
 {
  strcpy(strsql,row[0]);   
  int i=strlen(strsql)+1;
  if((organ=(char *)malloc(i*sizeof(char))) == NULL)
       printf("Не могу выделить пямять для organ !\n");

  strcpy(organ,strsql);
 }
if(mopen == 0)
 sql_closebaz(&bd);


return(0);
}
