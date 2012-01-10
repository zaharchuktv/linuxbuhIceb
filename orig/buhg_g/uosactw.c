/* $Id: uosactw.c,v 1.13 2011-02-21 07:35:58 sasa Exp $ */
/*14.12.2010    17.02.1997      Белых А.И.      uosactw.c
Распечатка акта приема передачи основных средств
*/
#include <stdlib.h>
#include        <errno.h>
#include        "buhg_g.h"
#include <unistd.h>

extern char     *organ;/*Наименование организации*/
extern SQL_baza bd;

void uosactw(const char *data_dok, //Дата документа
const char *ndk, //номер документа
GtkWidget *wpredok)
{
char            bros[512];
char            str[1024];
FILE            *ff1;
char            imaf1[112];
int            in;
int             pdr;
double		pbs;
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
float		kof;
short		dv,mv,gv;
short		god;
char		pasp[512];
char		naim[512];
char		model[512];
short		tipz;
char		osnov[512];
char		kpos[32];
char		datpr[16];
char		kprr[32];
char		zavnom[512];
int		kkk=0;
char		godchar[5];
char		mesqc[32];
char		kodol[32];
double bsby=0.,izby=0.;
short d,m,g;
SQLCURSOR cur;
SQLCURSOR curr;
iceb_u_rsdat(&d,&m,&g,data_dok,1);

char kodedrpou[20];
memset(kodedrpou,'\0',sizeof(kodedrpou));

/*Узнаём код */
sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row1,&cur,wpredok) == 1)
 strncpy(kodedrpou,row1[0],sizeof(kodedrpou)-1);

sprintf(godchar,"%d",g);
/*Читаем шапку документа*/
sprintf(strsql,"select tipz,kontr,osnov,kodop,kodol from Uosdok where datd='%d.%02d.%02d' and \
nomd='%s'",g,m,d,ndk);

if(iceb_sql_readkey(strsql,&row1,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s %s %d.%d.%d%s",
  gettext("Не найден документ"),ndk,d,m,g,
  gettext("г."));
  iceb_menu_soob(strsql,wpredok);
  return;
 }
else
 {
  tipz=atoi(row1[0]);
  strncpy(kpos,row1[1],sizeof(kpos)-1);
  strncpy(osnov,row1[2],sizeof(osnov)-1);
  strncpy(kprr,row1[3],sizeof(kprr)-1);
  strncpy(kodol,row1[4],sizeof(kodol)-1);
 }  


if(tipz == 1)
  sprintf(imaf1,"acp%d.lst",getpid());
if(tipz == 2)
  sprintf(imaf1,"acr%d.lst",getpid());

errno=0;
if((ff1 = fopen(imaf1,"r")) == NULL)
 if(errno != ENOENT)
   {
    iceb_er_op_fil(imaf1,"",errno,wpredok);
    return;
   }

if(errno != ENOENT) /*Файл сущесвует*/
 {
  fclose(ff1);
  if(iceb_menu_danet(gettext("Распечатка уже есть! Переформировать ?"),2,wpredok) == 2)
   {
    class iceb_u_spisok imafil;
    class iceb_u_spisok naimf;
    imafil.plus(imaf1);
    naimf.plus(gettext("Распечатка акта приёмки"));
    iceb_rabfil(&imafil,&naimf,"",0,wpredok);
    return;
   }
 }

sprintf(datpr,"%d.%d.%d",d,m,g);


sprintf(strsql,"select innom,podr,bsby from Uosdok1 where datd='%d.%02d.%02d' and \
nomd='%s'",g,m,d,ndk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),wpredok);
  return;
 }

SQL_str row_alx;
class SQLCURSOR cur_alx;
const char *imaf_alx={"uosact.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return;
 }



if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }

kkk=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  kkk++;
  memset(str,'\0',sizeof(str));
  strncpy(str,row_alx[0],sizeof(str)-1);
  switch(kkk)
   {
    case 1 :
     iceb_u_bstab(str,organ,0,40,1);
     break;

   case 3 :
     iceb_u_bstab(str,kodedrpou,29,49,1);
     break;

    case 6 :
     memset(mesqc,'\0',sizeof(mesqc));
     iceb_mesc(m,1,mesqc);
     sprintf(strsql,"%02d %s %04dр.",d,mesqc,g);

     iceb_u_bstab(str,strsql,123,150,1);
//     iceb_u_bstab(str,godchar,141,146,1);
     break;

    case 14 :
     iceb_u_bstab(str,ndk,130,141,1);
     iceb_u_bstab(str,datpr,142,153,1);
     /*  iceb_u_bstab(str,us3.kol,155,160,1);*/
     iceb_u_bstab(str,kodol,155,170,1);
     iceb_u_bstab(str,kprr,172,179,1);
     break;

    case 28 :
      while(cur.read_cursor(&row) != 0)
       {
        
        in=atol(row[0]);
        pdr=atoi(row[1]);
        pbs=atof(row[2]);
        
        class bsizw_data bal_st;
        bsizw(in,pdr,d,m,g,&bal_st,NULL,wpredok);


        bsby=bal_st.sbsby+bal_st.bsby;
        izby=bal_st.sizby+bal_st.izby+bal_st.iz1by;

        class poiinpdw_data rekin;
        /*Читаем переменные данные*/
        poiinpdw(in,m,g,&rekin,wpredok);

        /*Читаем поправочный коэффициент*/
        kof=0.;
        sprintf(strsql,"select kof from Uosgrup1 where kod='%s'",rekin.hnaby.ravno());

        if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
         {
          sprintf(strsql,"%s %s !",gettext("Не найден код группы"),rekin.hnaby.ravno());
          iceb_menu_soob(strsql,wpredok);
         }
        else
         kof=atof(row1[0]);

        god=0;
        memset(pasp,'\0',sizeof(pasp));
        dv=mv=gv=0;
        memset(zavnom,'\0',sizeof(zavnom));
        memset(naim,'\0',sizeof(naim));
        memset(model,'\0',sizeof(model));
        
        /*Читаем постоянную часть*/   
        sprintf(strsql,"select god,pasp,datvv,zavnom,naim,model \
from Uosin where innom=%d",in);

        if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
         {
          sprintf(strsql,"%s %d !",gettext("Не найден инвентарный номер"),in);
          iceb_menu_soob(strsql,wpredok);
         }
        else
         {
          god=atoi(row1[0]);
          strncpy(pasp,row1[1],sizeof(pasp)-1);
          iceb_u_rsdat(&dv,&mv,&gv,row1[2],2);
          strncpy(zavnom,row1[3],sizeof(zavnom)-1);
          strncpy(naim,row1[4],sizeof(naim)-1);
          strncpy(model,row1[5],sizeof(model)-1);
         }

        if(tipz == 1)
         fprintf(ff1," %*.*s %5.5d %*.*s %*.*s %5.5s %9.9s %9.9g %10d\
 %*.*s %*.*s %*.*s %8.8g %8.8g %6.6g %5.5s %7.7s %11.11g %4d %02d.%04d\
 %-*.*s\n",
         iceb_u_kolbait(4,kpos),iceb_u_kolbait(4,kpos),kpos,
         pdr,
         iceb_u_kolbait(5,rekin.shetu.ravno()),iceb_u_kolbait(5,rekin.shetu.ravno()),rekin.shetu.ravno(),
         iceb_u_kolbait(9,rekin.hau.ravno()),iceb_u_kolbait(9,rekin.hau.ravno()),rekin.hau.ravno(),
         " "," ",
         pbs,
         in,
         iceb_u_kolbait(10,zavnom),iceb_u_kolbait(10,zavnom),zavnom,
         iceb_u_kolbait(17,rekin.hzt.ravno()),iceb_u_kolbait(17,rekin.hzt.ravno()),rekin.hzt.ravno(),
         iceb_u_kolbait(6,rekin.hnaby.ravno()),iceb_u_kolbait(6,rekin.hnaby.ravno()),rekin.hnaby.ravno(),
         kof,0.,
         rekin.popkf," "," ",izby,god,mv,gv,
         iceb_u_kolbait(14,pasp),iceb_u_kolbait(14,pasp),pasp);

        if(tipz == 2)
         fprintf(ff1," %4.4d %*.*s %5.5s %9.9s %*.*s %*.*s %9.9g %10d\
 %*.*s %*.*s %*.*s %8.8g %8.8g %6.6g %5.5s %7.7s %11.11g %4d %02d.%04d\
 %-*.*s\n",
         pdr,
         iceb_u_kolbait(5,kpos),iceb_u_kolbait(5,kpos),kpos,
         " "," ",
         iceb_u_kolbait(5,rekin.shetu.ravno()),iceb_u_kolbait(5,rekin.shetu.ravno()),rekin.shetu.ravno(),
         iceb_u_kolbait(9,rekin.hau.ravno()),iceb_u_kolbait(9,rekin.hau.ravno()),rekin.hau.ravno(),
         pbs,in,
         iceb_u_kolbait(10,zavnom),iceb_u_kolbait(10,zavnom),zavnom,
         iceb_u_kolbait(17,rekin.hzt.ravno()),iceb_u_kolbait(17,rekin.hzt.ravno()),rekin.hzt.ravno(),
         iceb_u_kolbait(6,rekin.hnaby.ravno()),iceb_u_kolbait(6,rekin.hnaby.ravno()),rekin.hnaby.ravno(),
         kof,0.,
         rekin.popkf," "," ",izby*(-1),god,mv,gv,
         iceb_u_kolbait(14,pasp),iceb_u_kolbait(14,pasp),pasp);
       }
     break;

    case 29 :
     iceb_u_bstab(str,osnov,34,74,1);
     break;

    case 33 :
     memset(bros,'\0',sizeof(bros));
     sprintf(bros,"%s %s",naim,model);
     iceb_u_bstab(str,bros,62,140,1);
     break;

    case 31 :
     iceb_u_bstab(str,godchar,22,28,1);
     break;

    case 41 :
     fprintf(ff1,"\x1b\x6C%c",1); /*Установка левого поля*/
     fprintf(ff1,"\f\n");
     continue;

    case 61 :
     memset(mesqc,'\0',sizeof(mesqc));
     iceb_mesc(m,1,mesqc);
     sprintf(strsql,"%02d %s %04dр.",d,mesqc,g);

     iceb_u_bstab(str,strsql,69,90,1);

     break;
   }

  fprintf(ff1,"%s",str);

 }

iceb_podpis(ff1,wpredok);

fclose(ff1);
class iceb_u_spisok imafil;
class iceb_u_spisok naimf;
imafil.plus(imaf1);
naimf.plus(gettext("Распечатка акта приёмки"));
iceb_ustpeh(imaf1,0,wpredok);
iceb_rabfil(&imafil,&naimf,"",0,wpredok);

return;

}
