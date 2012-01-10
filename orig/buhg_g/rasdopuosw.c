/*$Id: rasdopuosw.c,v 1.2 2011-09-07 11:44:20 sasa Exp $*/
/*11.08.2011	11.08.2011	Белых А.И.	rasdopuosw.c
Распечатка дополнительной информации по инвентарному номеру
*/
#include <stdlib.h>
#include <errno.h>
#include "buhg_g.h"
#include        <unistd.h>

extern short	startgoduos;
extern SQL_baza bd;

void rasdopuosw(short d,short m,short g,int innom,GtkWidget *wpredok)
{
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[512];
FILE *ff;
char imaf[64];
class iceb_u_str naim("");
sprintf(strsql,"select * from Uosin where innom=%d",innom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найдена карточка инвентарного номера"),innom);
  iceb_menu_soob(strsql,wpredok);
  return;
 }
sprintf(imaf,"%s%d.lst",__FUNCTION__,getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

iceb_u_zagolov(gettext("Карточка инвентарного номера"),0,0,0,d,m,g,iceb_get_pnk("00",0,wpredok),ff);
fprintf(ff,"%s:%d\n",gettext("Инвентарный номер"),innom);
fprintf(ff,"---------------------------------------------------------\n");

fprintf(ff,"%s%s\n",gettext("Наименование................."),row[2]);//0
fprintf(ff,"%s%s\n",gettext("Завод изготовитель..........."),row[3]);//1
fprintf(ff,"%s%s\n",gettext("Паспорт/чертеж N............."),row[4]);//2
fprintf(ff,"%s%s\n",gettext("Модель/тип/марка............."),row[5]);//3
fprintf(ff,"%s%s\n",gettext("Год выпуска.................."),row[1]);//4
fprintf(ff,"%s%s\n",gettext("Заводской номер.............."),row[6]);//5
fprintf(ff,"%s%s\n",gettext("Дата ввода в эксплуатацию...."),iceb_u_datzap(row[7]));//6

class poiinpdw_data per_dan;

poiinpdw(innom,m,g,&per_dan,wpredok);
sprintf(strsql,"select nais from Plansh where ns='%s'",per_dan.shetu.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim.new_plus(row[0]);
fprintf(ff,"%s%s %s\n",gettext("Счет учета..................."),per_dan.shetu.ravno(),naim.ravno());//7

naim.new_plus("");
sprintf(strsql,"select naik from Uoshz where kod='%s'",per_dan.hzt.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim.new_plus(row[0]);


fprintf(ff,"%s%s %s\n",gettext("Шифр затрат амортотчислений.."),per_dan.hzt.ravno(),naim.ravno());//8

naim.new_plus("");
sprintf(strsql,"select naik from Uoshau where kod='%s'",per_dan.hau.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim.new_plus(row[0]);

fprintf(ff,"%s%s %s\n",gettext("Шифр аналитического учета...."),per_dan.hau.ravno(),naim.ravno());//9

naim.new_plus("");
sprintf(strsql,"select naik from Uosgrup where kod='%s'",per_dan.hna.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim.new_plus(row[0]);

fprintf(ff,"%s%s %s\n",gettext("Шифр н-мы амор-ний (нал.уч).."),per_dan.hna.ravno(),naim.ravno());//10
fprintf(ff,"%s%.10g/%.10g\n",gettext("Поправочный коэффициент(н/б)."),per_dan.popkf,per_dan.popkfby);//11

class iceb_u_str so_txt("");
if(per_dan.soso == 0)
 so_txt.new_plus(gettext("Бух.учет + Налоговый учет +"));
if(per_dan.soso == 1)
 so_txt.new_plus(gettext("Бух.учет - Налоговый учет -"));
if(per_dan.soso == 2)
 so_txt.new_plus(gettext("Бух.учет + Налоговый учет -"));
if(per_dan.soso == 3)
 so_txt.new_plus(gettext("Бух.учет - Налоговый учет +"));




fprintf(ff,"%s%s\n",gettext("Состояние объекта............"),so_txt.ravno());//12
fprintf(ff,"%s%s\n",gettext("Номерной знак................"),per_dan.nomz.ravno());//13

naim.new_plus("");
sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",per_dan.hnaby.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim.new_plus(row[0]);
fprintf(ff,"%s%s %s\n",gettext("Шифр н-мы амор-ний (бух.уч).."),per_dan.hnaby.ravno(),naim.ravno());//14

int podr=0,kodotl=0;
poiinw(innom,d,m,g,&podr,&kodotl,wpredok);

class bsizw_data bal_st;



bsizw(innom,podr,d,m,g,&bal_st,NULL,wpredok);

fprintf(ff,"\n%30s %*s  %s\n","",iceb_u_kolbait(22,gettext("Налоговый учёт")),gettext("Налоговый учёт"),gettext("Бухгалтерский учет"));
char bros[512];
sprintf(bros,"%s 1.%d",gettext("Балансовая стоимость на"),startgoduos);
fprintf(ff,"%*s:%10.2f %10.2f %10.2f %10.2f\n",iceb_u_kolbait(31,bros),bros,bal_st.sbs,bal_st.siz,bal_st.sbsby,bal_st.sizby);
  
fprintf(ff,"%*s:%10.2f %10.2f %10.2f %10.2f\n",iceb_u_kolbait(31,gettext("Изменение за период")),gettext("Изменение за период"),bal_st.bs,bal_st.iz,bal_st.bsby,bal_st.izby);
fprintf(ff,"%*s:%10s %10.2f %10s %10.2f\n",iceb_u_kolbait(31,gettext("Амортизация")),gettext("Амортизация")," ",bal_st.iz1," ",bal_st.iz1by);

sprintf(bros,"%s %02d.%d",gettext("Балансовая стоимость на"),m,g);

fprintf(ff,"%*s:%10.2f %10.2f %10.2f %10.2f\n",iceb_u_kolbait(31,bros),bros,bal_st.sbs+bal_st.bs,bal_st.siz+bal_st.iz+bal_st.iz1,bal_st.sbsby+bal_st.bsby,bal_st.sizby+bal_st.izby+bal_st.iz1by);

fprintf(ff,"%*s %10.2f %10s %10.2f",iceb_u_kolbait(42,gettext("Остаток")),gettext("Остаток"),(bal_st.sbs+bal_st.bs)-(bal_st.siz+bal_st.iz+bal_st.iz1)," ",(bal_st.sbsby+bal_st.bsby)-(bal_st.sizby+bal_st.izby+bal_st.iz1by));








sprintf(strsql,"select zapis from Uosin1 where innom=%d order by nomz asc",innom);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  fprintf(ff,"\n%s\n",gettext("Дополнительная информация"));
  fprintf(ff,"---------------------------------------------------------\n");
  while(cur.read_cursor(&row) != 0)
    fprintf(ff,"%s\n",row[0]);
 }
while(cur.read_cursor(&row) != 0)
  fprintf(ff,"%s\n",row[0]);

/*Распечатка драг металлов если они есть*/
sprintf(strsql,"select * from Uosindm where innom=%d",innom);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr > 0)
 {
  fprintf(ff,"\n%s\n",gettext("Список драгоценных металлов"));
  fprintf(ff,"---------------------------------------------------------\n");
  while(cur.read_cursor(&row) != 0)
   {
    naim.new_plus("");
    sprintf(strsql,"select naik from Uosdm where kod=%s",row[1]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
     naim.new_plus(row1[0]);

    fprintf(ff,"%2s %-*.*s %*s %10.10g\n",
    row[1],
    iceb_u_kolbait(30,naim.ravno()),
    iceb_u_kolbait(30,naim.ravno()),
    naim.ravno(),
    iceb_u_kolbait(6,row[2]),
    row[2],
    atof(row[3]));
   }
 }


iceb_podpis(ff,wpredok);
fclose(ff);

class iceb_u_spisok imafs;
class iceb_u_spisok naimf;

imafs.plus(imaf);
naimf.plus(gettext("Дополнительная информация к инвентарному номеру"));


iceb_ustpeh(imaf,3,wpredok);
iceb_rabfil(&imafs,&naimf,"",0,wpredok);



}
