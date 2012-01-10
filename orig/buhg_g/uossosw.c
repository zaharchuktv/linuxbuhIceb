/* $Id: uossosw.c,v 1.10 2011-02-21 07:35:58 sasa Exp $ */
/*17.11.2009    29.05.1997      Белых А.И.      uossosw.c
Акт списания основных средств
*/
#include <stdlib.h>
#include        <errno.h>
#include        "buhg_g.h"
#include <unistd.h>

extern char     *organ;/*Наименование организации*/
extern SQL_baza bd;

void uossosw(const char *data_dok, //дата документа
const char *ndk,  //Номер документа
GtkWidget *wpredok)
{
short d,m,g;
char            str[1024];
char            bros[512];
FILE            *ff,*ff1;
char            imaf1[32];
short           i;
int            in;
int             pdr;
double          bs,iz;
double		pbs,piz;
long		kolstr;
SQL_str         row,row1;
SQLCURSOR curr;
char strsql[512];
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

iceb_u_rsdat(&d,&m,&g,data_dok,1);


sprintf(imaf1,"aso%d.lst",getpid());
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
    naimf.plus(gettext("Распечатка акта списания основых средств"));
    iceb_rabfil(&imafil,&naimf,"",0,wpredok);
    
    return;
   }
 }
sprintf(datpr,"%d.%d.%d",d,m,g);

/*Читаем шапку документа*/
sprintf(strsql,"select tipz,kontr,osnov,kodop from Uosdok where datd='%d.%02d.%02d' and \
nomd='%s'",g,m,d,ndk);

if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
 {
  sprintf(strsql,"%s %s %d.%d.%d%s",
  gettext("Не найден документ"),ndk,d,m,g,
  gettext("г."));
  iceb_menu_soob(strsql,wpredok);
  return;
 }
tipz=atoi(row1[0]);
strncpy(kpos,row1[1],sizeof(kpos)-1);
strncpy(osnov,row1[2],sizeof(osnov)-1);
strncpy(kprr,row1[3],sizeof(kprr)-1);

sprintf(strsql,"select innom,podr,bs,iz from Uosdok1 where datd='%d.%02d.%02d' and \
nomd='%s'",g,m,d,ndk);
SQLCURSOR cur;
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


if(iceb_alxout("uossos.alx",wpredok) != 0)
 return;
if((ff = fopen("uossos.alx","r")) == NULL)
 {
  iceb_er_op_fil("uossos.alx","",errno,wpredok);
  return;
 }

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff);
iceb_u_bstab(str,organ,0,40,1);
fprintf(ff1,"%s",str);
for(i=0; i< 13;i++)
 {
  memset(str,'\0',sizeof(str));
  fgets(str,sizeof(str),ff);
  fprintf(ff1,"%s",str);
 }

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff);
iceb_u_bstab(str,ndk,131,141,1);
iceb_u_bstab(str,datpr,143,153,1);
iceb_u_bstab(str,kprr,155,164,1);
fprintf(ff1,"%s",str);

for(i=0; i< 13;i++)
 {
  memset(str,'\0',sizeof(str));
  fgets(str,sizeof(str),ff);
  fprintf(ff1,"%s",str);
 }

while(cur.read_cursor(&row) != 0)
 {
  
  in=atol(row[0]);
  pdr=atoi(row[1]);
  pbs=atof(row[2]);
  piz=atof(row[3]);
  class bsizw_data bal_st;
  
  bsizw(in,pdr,d,m,g,&bal_st,NULL,wpredok);
  iz=bal_st.iz1+bal_st.siz+bal_st.iz;
  bs=bal_st.sbs+bal_st.bs;
  
  class poiinpdw_data rekin;  
  /*Читаем переменные данные*/
  poiinpdw(in,m,g,&rekin,wpredok);

  /*Читаем поправочный коэффициент*/
  kof=0.;
  sprintf(strsql,"select kof from Uosgrup where kod='%s'",rekin.hna.ravno());

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код группы"),rekin.hna.ravno());
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
  sprintf(strsql,"select god,pasp,datvv,zavnom,naim,model from Uosin \
where innom=%d",in);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден инвентарный номер"),in);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    god=atoi(row1[0]);
    strncpy(pasp,row1[1],sizeof(pasp)-1);
    iceb_u_rsdat(&dv,&mv,&gv,row[2],2);
    strncpy(zavnom,row1[3],sizeof(zavnom)-1);
    strncpy(naim,row1[4],sizeof(naim)-1);
    strncpy(model,row1[5],sizeof(model)-1);
   }
/*               1   2   3   4   5    6        7     8
    9     10   11    12    13   14  15  16    17*/

  fprintf(ff1," %7d %5s %9s %*s %*s %10.10g %11.11g %10d\
 %*.*s %*s %6.6g %8.8g %8.8g %*s %*s %4d %02d.%d\n",
   pdr," "," ",
   iceb_u_kolbait(5,rekin.shetu.ravno()),rekin.shetu.ravno(),
   iceb_u_kolbait(9,rekin.hau.ravno()),rekin.hau.ravno(),
   pbs*(-1),piz*(-1),in,
   iceb_u_kolbait(10,zavnom),iceb_u_kolbait(10,zavnom),zavnom,
   iceb_u_kolbait(17,rekin.hzt.ravno()),rekin.hzt.ravno(),
   0.,kof,0.,
   iceb_u_kolbait(5,rekin.hna.ravno()),rekin.hna.ravno(),
   iceb_u_kolbait(7,rekin.hna.ravno()),rekin.hna.ravno(),
   god,mv,gv);

 }

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff);
fprintf(ff1,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff);
fprintf(ff1,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff);
fprintf(ff1,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff);
iceb_u_bstab(str,osnov,11,51,1);
sprintf(bros,"%s %s",naim,model);
iceb_u_bstab(str,bros,81,160,1);
fprintf(ff1,"%s",str);

for(;;)
 {
  memset(str,'\0',sizeof(str));
  fgets(str,sizeof(str),ff);
  if(str[0] == '=')
   break;
  fprintf(ff1,"%s",str);

 }
fprintf(ff1,"\f\n");
memset(str,'\0',sizeof(str));
while(fgets(str,sizeof(str),ff) != NULL)
  fprintf(ff1,"%s",str);

iceb_podpis(ff1,wpredok);

fclose(ff);
fclose(ff1);
unlink("uossos.alx");
class iceb_u_spisok imafil;
class iceb_u_spisok naimf;
imafil.plus(imaf1);
naimf.plus(gettext("Распечатка акта списания основых средств"));
iceb_ustpeh(imaf1,0,wpredok);
iceb_rabfil(&imafil,&naimf,"",0,wpredok);
}
