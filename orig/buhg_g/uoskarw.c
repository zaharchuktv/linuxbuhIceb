/* $Id: uoskarw.c,v 1.11 2011-02-21 07:35:58 sasa Exp $ */
/*17.11.2009    28.05.1997      Белых А.И.      uoskarw.c
Распечатка инветарной карточки
*/
#include <stdlib.h>
#include        <ctype.h>
#include        <errno.h>
#include <unistd.h>
#include        "buhg_g.h"

extern char      *organ;/*Наименование организации*/

void uoskarw(int in,double bs,double iz,GtkWidget *wpredok)
{
char            bros[512];
char            str[1024];
FILE            *ff,*ff1;
char            imaf[112],imaf1[112];
short           i;
double		pk;
float		kof;
SQL_str         row;
SQLCURSOR curr;
char		strsql[512];
char		datdok[32];
char		nomdok[32];
class bsizw_data bal_st;


short		d,m,g;

class iceb_u_str hau;
class iceb_u_str hzt;
class iceb_u_str hna;
class iceb_u_str shetu;
class iceb_u_str popkf;

sprintf(strsql,"select * from Uosinp where innom=%d  order by god desc,mes desc limit 1",in);


if(iceb_sql_readkey(strsql,&row,&curr,wpredok) <= 0)
 {
  iceb_menu_soob("Не найдено записей в таблице Uosinp!",wpredok);
  return; 
 }
hzt.new_plus(row[4]);
hna.new_plus(row[6]);
shetu.new_plus(row[3]);
popkf.new_plus(row[7]);
hau.new_plus(row[5]);

sprintf(imaf1,"in%d.lst",in);

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
  if(iceb_menu_danet(gettext("Распечатка уже есть. Переформировать ?"),2,wpredok) == 2)
   {
    class iceb_u_spisok imafil;
    class iceb_u_spisok naimf;
    imafil.plus(imaf1);
    naimf.plus(gettext("Распечатка карточки"));
    iceb_rabfil(&imafil,&naimf,"",0,wpredok);

    return;
   }
 }
if(iceb_alxout("uoskar.alx",wpredok) != 0)
 return;
if((ff = fopen("uoskar.alx","r")) == NULL)
 {
  iceb_er_op_fil("uoskar.alx","",errno,wpredok);
  return;
 }

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff);
iceb_u_bstab(str,organ,0,40,1);
fprintf(ff1,"%s",str);
for(i=0; i< 10;i++)
 {
  memset(str,'\0',sizeof(str));
  fgets(str,sizeof(str),ff);
  fprintf(ff1,"%s",str);
 }

memset(datdok,'\0',sizeof(datdok));
memset(nomdok,'\0',sizeof(nomdok));
class iceb_u_str podr("");
//Определяем последний документ по основному средству
sprintf(strsql,"select datd,nomd,podr from Uosdok1 where innom=%d \
order by datd,tipz desc limit 1",in);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  sprintf(datdok,"%02d.%02d.%d",d,m,g);
  strncpy(nomdok,row[1],sizeof(nomdok)-1);
  podr.new_plus(row[2]);
 }

class iceb_u_str naim("");
class iceb_u_str pasp("");
class iceb_u_str model("");
class iceb_u_str zavnom("");
class iceb_u_str godv("");
/*Читаем карточку*/
sprintf(strsql,"select * from Uosin where innom=%d",in);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
 {
  naim.new_plus(row[2]);
  pasp.new_plus(row[4]);
  model.new_plus(row[5]);
  zavnom.new_plus(row[6]);
  godv.new_plus(row[1]);
 }

  
memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff);
iceb_u_bstab(str,datdok,158,168,1);
iceb_u_bstab(str,nomdok,169,178,1);
fprintf(ff1,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff);
fprintf(ff1,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff);
iceb_u_bstab(str,naim.ravno(),40,100,1);
fprintf(ff1,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff);
iceb_u_bstab(str,pasp.ravno(),40,100,1);
fprintf(ff1,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff);
iceb_u_bstab(str,model.ravno(),40,100,1);
fprintf(ff1,"%s",str);


for(i=0; i< 21;i++)
 {
  memset(str,'\0',sizeof(str));
  fgets(str,sizeof(str),ff);
  fprintf(ff1,"%s",str);
 }

sprintf(strsql,"select kof from Uosgrup where kod='%s'",hna.ravno());
kof=0.;
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найдено группу"),hna.ravno());
  iceb_menu_soob(strsql,wpredok);
 }
else
 kof=atof(row[0]); 

pk=popkf.ravno_atof();

//if(iceb_u_proverka(spgnunpa,hna.ravno(),0,1) == 0)
if(uosprovgrw(hna.ravno(),wpredok) == 1)
 {
  sprintf(strsql,"Поправочный коэффициент для группы %s",hna.ravno());
  if(iceb_poldan(strsql,bros,"uosnast.alx",wpredok) != 0)
   {
    class iceb_u_str repl;
    repl.plus("Не найден ");
    repl.plus(strsql);
    repl.plus("!");
    iceb_menu_soob(&repl,wpredok);
    pk=0.;
   }
  else
   pk=iceb_u_atof(bros);
 }

/*             1   2   3   4       5    6       7    8     9    10   11
 12  13   14  15   19   20   21    22     23  24  25   26*/

fprintf(ff1," %5d %*s %*s %10.10g %*s %*.*s %7.7g %4.4g %7s %6.6g %4.4s\
 %*s %10s %*s %7s %10d %*s %*s %10.10g %d %10s %5s %7.7g\n",
podr.ravno_atoi(),
iceb_u_kolbait(5,shetu.ravno()),shetu.ravno(),
iceb_u_kolbait(6,hau.ravno()),hau.ravno(),
bs,
iceb_u_kolbait(11,hzt.ravno()),hzt.ravno(),
iceb_u_kolbait(4,hna.ravno()),iceb_u_kolbait(4,hna.ravno()),hna.ravno(),
0.,
kof,
" ",
pk,
" ",
iceb_u_kolbait(5,hna.ravno()),hna.ravno(),
datdok,
iceb_u_kolbait(7,nomdok),nomdok,
" ",
in,
iceb_u_kolbait(10,zavnom.ravno()),zavnom.ravno(),
iceb_u_kolbait(10,pasp.ravno()),pasp.ravno(),
iz,
godv.ravno_atoi(),
" "," ",0.);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff);
fprintf(ff1,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff);
fprintf(ff1,"\f\n");
memset(str,'\0',sizeof(str));
while(fgets(str,sizeof(str),ff) != NULL)
  fprintf(ff1,"%s",str);


fclose(ff);

fclose(ff1);
unlink("uoskar.alx");


class iceb_u_spisok imafil;
class iceb_u_spisok naimf;
imafil.plus(imaf1);
naimf.plus(gettext("Распечатка карточки"));

iceb_ustpeh(imaf1,0,wpredok);

iceb_rabfil(&imafil,&naimf,"",0,wpredok);

}
