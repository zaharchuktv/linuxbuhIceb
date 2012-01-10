/* $Id: rasnalnuosw.c,v 1.11 2011-06-07 08:52:26 sasa Exp $ */
/*06.06.2011    22.01.1994      Белых А.И.      rasnalnuosw.c
Распечатка налоговых накладных для Учета основных средств
*/
#include <stdlib.h>
#include	"buhg_g.h"
void rasnalnuosw_old(const char *data_dok,int podr,const char *nomdok,char *imaf,GtkWidget *wpredok);
int rasnalnuosw21122010(const char *data_dok,int podr,const char *nomdok,char *imaf,GtkWidget *wpredok);

extern double          okrcn;  /*Округление цены*/
extern double okrg1;
extern SQL_baza bd;

int rasnalnuosw(const char *data_dok,int podr,const char *nomdok,char *imaf,GtkWidget *wpredok)
{
if(iceb_u_sravmydat(data_dok,"10.1.2011") < 0)
 {
  rasnalnuosw_old(data_dok,podr,nomdok,imaf,wpredok);
  return(0);
 }
else
 return(rasnalnuosw21122010(data_dok,podr,nomdok,imaf,wpredok));
}
/***********************************************************************/
int rasnalnuosw21122010(const char *data_dok,int podr,const char *nomdok,char *imaf,GtkWidget *wpredok)
{
short dd,md,gd;
SQL_str         row,row1;
SQLCURSOR curr;
int		kolstr;
char		strsql[512];
FILE            *f1;
short           i;
double          itogo;
char		kpos[32];
char		osnov[512];
char		uspr[512];
short		tipz;
char		nomnn[32]; /*Номер налоговой накладной*/
char		naim[512];
double		cena;
char		ei[32];
char		datop[16];		
char		kodforop[32];
short		dnn,mnn,gnn; //Дата налоговой накладной
char kodop[56];
float pnds=0.;
short tipnn=0;

memset(datop,'\0',sizeof(datop));
iceb_u_rsdat(&dd,&md,&gd,data_dok,1);

/*Читаем шапку документа*/
sprintf(strsql,"select kontr,tipz,osnov,forop,nomnn,datpnn,kodop,nds,pn \
from Uosdok where datd='%04d-%02d-%02d' and podr=%d and nomd='%s'",
gd,md,dd,podr,nomdok);

if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
  sprintf(strsql,"Не найден документ! Дата:%d.%d.%d Документ:%s Подразделение:%d",
  dd,md,gd,nomdok,podr);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
memset(osnov,'\0',sizeof(osnov));
memset(uspr,'\0',sizeof(uspr));
memset(kodforop,'\0',sizeof(kodforop));

strncpy(kpos,row[0],sizeof(kpos)-1);
tipz=atoi(row[1]);
strncpy(osnov,row[2],sizeof(osnov)-1);
if(row[3] != NULL)
 strncpy(kodforop,row[3],sizeof(kodforop)-1);
strncpy(nomnn,row[4],sizeof(nomnn)-1);
iceb_u_rsdat(&dnn,&mnn,&gnn,row[5],2);
strncpy(kodop,row[6],sizeof(kodop));
tipnn=atoi(row[7]);
pnds=atof(row[8]);

int nomer_str=0;
//Печатаем шапку документа
if(iceb_nalnaks(imaf,&nomer_str,dd,md,gd,dnn,mnn,gnn,nomnn,osnov,uspr,kodforop,kpos,tipz,0,&f1,wpredok) != 0)
 return(1);
nomer_str+=iceb_insfil("nalnakst.alx",f1,wpredok);
//iceb_nalnakh(&nomer_str,f1);  


sprintf(strsql,"select innom,cena from Uosdok1 \
where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tipz=%d \
order by innom asc",gd,md,dd,podr,nomdok,tipz);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }


itogo=0.;
i=0;
strcpy(ei,gettext("Шт"));
char razdel[8];
strcpy(razdel," I");
while(cur.read_cursor(&row) != 0)
 {
  i++;

  cena=atof(row[1]);
  cena=iceb_u_okrug(cena,okrcn);

  if(i == 2)
   {
    memset(datop,'\0',sizeof(datop));
    memset(razdel,'\0',sizeof(razdel));
   }
  //Читаем наименование
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naim from Uosin where innom=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) > 0)
    strncpy(naim,row1[0],sizeof(naim)-1);

  /*печатаем строку*/
  iceb_nalnak_str(0,tipnn,razdel,datop,naim,ei,1.,cena,cena,f1);
  itogo+=cena;


 }

//Печатаем концовку налоговой накладной
iceb_nalnake(tipz,tipnn,itogo,0.,"",0.,pnds,"uosnast.alx",kodop,okrg1,0.,f1,wpredok);
iceb_ustpeh(imaf,3,wpredok);

return(0);
}

/***********************************************************************/
void rasnalnuosw_old(const char *data_dok,int podr,const char *nomdok,char *imaf,GtkWidget *wpredok)
{
short dd,md,gd;
SQL_str         row,row1;
SQLCURSOR curr;
int		kolstr;
char		strsql[512];
FILE            *f1,*f2;
short           i;
double          itogo;
int		kor ;
char		kpos[32];
char		osnov[512];
char		uspr[512];
short		tipz;
char		nomnn[32]; /*Номер налоговой накладной*/
char		naim[512];
double		cena;
char		ei[32];
char		datop[16];		
char		kodforop[32];
short		dnn,mnn,gnn; //Дата налоговой накладной
char kodop[56];
float pnds=0.;
short tipnn=0;

memset(datop,'\0',sizeof(datop));
iceb_u_rsdat(&dd,&md,&gd,data_dok,1);

/*Читаем шапку документа*/
sprintf(strsql,"select kontr,tipz,osnov,forop,nomnn,datpnn,kodop,nds,pn \
from Uosdok where datd='%04d-%02d-%02d' and podr=%d and nomd='%s'",
gd,md,dd,podr,nomdok);

if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
  sprintf(strsql,"Не найден документ! Дата:%d.%d.%d Документ:%s Подразделение:%d",
  dd,md,gd,nomdok,podr);
  iceb_menu_soob(strsql,wpredok);
  return;
 }
memset(osnov,'\0',sizeof(osnov));
memset(uspr,'\0',sizeof(uspr));
memset(kodforop,'\0',sizeof(kodforop));

strncpy(kpos,row[0],sizeof(kpos)-1);
tipz=atoi(row[1]);
strncpy(osnov,row[2],sizeof(osnov)-1);
if(row[3] != NULL)
 strncpy(kodforop,row[3],sizeof(kodforop)-1);
strncpy(nomnn,row[4],sizeof(nomnn)-1);
iceb_u_rsdat(&dnn,&mnn,&gnn,row[5],2);
strncpy(kodop,row[6],sizeof(kodop));
tipnn=atoi(row[7]);
pnds=atof(row[8]);

int nomer_str=0;
//Печатаем шапку документа
iceb_rasnaln1(imaf,&nomer_str,&kor,dd,md,gd,dnn,mnn,gnn,nomnn,osnov,uspr,kodforop,kpos,tipz,&f1,&f2,wpredok);


sprintf(strsql,"select innom,cena from Uosdok1 \
where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tipz=%d \
order by innom asc",gd,md,dd,podr,nomdok,tipz);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }


itogo=0.;
i=0;
strcpy(ei,gettext("Шт"));
char razdel[8];
strcpy(razdel," I");
while(cur.read_cursor(&row) != 0)
 {
  i++;

  cena=atof(row[1]);
  cena=iceb_u_okrug(cena,okrcn);

  if(i == 2)
   {
    memset(datop,'\0',sizeof(datop));
    memset(razdel,'\0',sizeof(razdel));
   }
  //Читаем наименование
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naim from Uosin where innom=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) > 0)
    strncpy(naim,row1[0],sizeof(naim)-1);
    
  fprintf(f1,"|%-4s|%-10s|%-*.*s|%-*.*s|%9.9g|%10s",
  razdel,datop,
  iceb_u_kolbait(40,naim),iceb_u_kolbait(40,naim),naim,
  iceb_u_kolbait(7,ei),iceb_u_kolbait(7,ei),ei,
  1.,iceb_prcn(cena));

  if(tipnn == 0)
    fprintf(f1,"|%9s|%9s|%9s|%9s|%11s|\n",
     iceb_prcn(cena)," "," "," "," ");

  if(tipnn == 1)
    fprintf(f1,"|%9s|%9s|%9s|%9s|%11s|\n",
    " ",iceb_prcn(cena)," "," "," ");

  if(tipnn == 2)
    fprintf(f1,"|%9s|%9s|%9s|%9s|%11s|\n",
    " "," ",iceb_prcn(cena)," "," ");

  if(tipnn == 3)
    fprintf(f1,"|%9s|%9s|%9s|%9s|%11s|\n",
    " "," "," ",iceb_prcn(cena)," ");

  if(iceb_u_strlen(naim) > 40)
   fprintf(f1,"|%4s|%-10s|%-*.*s|%-7.7s|%9.9s|%10s|%9s|%9s|%9s|%9s|%11s|\n",
    " "," ",
    iceb_u_kolbait(40,iceb_u_adrsimv(40,naim)),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,naim)),
    iceb_u_adrsimv(40,naim),
    " "," "," "," "," "," "," "," ");

  itogo+=cena;


 }

//Печатаем концовку налоговой накладной
//iceb_rasnaln2(nomer_str,kor,tipnn,itogo,0.,"",0.,pnds,"uosnast.alx",kodop,f1,f2);
iceb_rasnaln2(nomer_str,kor,tipnn,itogo,0.,"",0.,pnds,"uosnast.alx",kodop,okrg1,0.,f1,f2,wpredok);


}
