/*$Id: vibshbnw.c,v 1.8 2011-02-21 07:35:58 sasa Exp $*/
/*29.09.2009	17.11.2001	Белых А.И.	vibshbnw.c
Выбор счета с которого будет осуществляться оплата
Если вернули 0-счет выбран
             1-нет
*/
#include        <errno.h>
#include        <ctype.h>
#include <unistd.h>
#include        "buhg_g.h"

extern SQL_baza bd;

int vibshbnw(char *kontr,char *tabl,GtkWidget *wpredok)
{
char		bros[1024];
char		shet[1024]; //Номер счета
char		naimbn[1024]; //Наименование банка
iceb_u_spisok SHETA;
iceb_u_spisok KONTR; //Код контрагента
char		imafil[112];

if(iceb_u_SRAV(tabl,"Pltp",0) == 0 || iceb_u_SRAV(tabl,"Tpltp",0) == 0)
  sprintf(imafil,"%s","avtprodok.alx");
if(iceb_u_SRAV(tabl,"Pltt",0) == 0 || iceb_u_SRAV(tabl,"Tpltt",0) == 0)
  sprintf(imafil,"%s","avtprodokt.alx");
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imafil);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imafil);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(row_alx[0][0] == '#' || row_alx[0][0] == ' ' || row_alx[0][0] == '\t' ||
  row_alx[0][0] == '\n')
   continue;
  
  if(iceb_u_polen(row_alx[0],shet,sizeof(shet),1,'|') == 0)
   {
    if(isdigit(shet[0]) == 0) //Значит символ не число
      continue;
    if(iceb_u_polen(row_alx[0],bros,sizeof(bros),3,'|') != 0)
      continue;
    KONTR.plus(bros);

    iceb_u_polen(row_alx[0],naimbn,sizeof(naimbn),4,'|');

    sprintf(bros,"%-15s|%s",shet,naimbn);    
    SHETA.plus(bros);


   }

 }

if(SHETA.kolih() != 0)
 {
/*****
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);
  int voz=dirmasiv(&SHETA, -1, -1,0,gettext("Выберите нужный счет"),1);
  if(voz == -1)
    return(1);
  strcpy(kontr,KONTR.ravno(voz));
***********/
  iceb_u_str titl;
  iceb_u_str zagolovok;
  iceb_u_spisok punkt_m;

  titl.plus(gettext("Выбор"));


  zagolovok.plus(gettext("Выберите нужное."));

  for(int ii=0; ii < SHETA.kolih() ; ii++)
    punkt_m.plus(SHETA.ravno(ii));


  int nomer=0;
  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);
  if(nomer == -1)
   return(1);
    
  strcpy(kontr,KONTR.ravno(nomer));

 }
else
 {
  iceb_menu_soob(gettext("В файле нет ни одного счёта !"),wpredok);
  return(1);
 }
return(0);
}
