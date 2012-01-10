/*$Id: peh1w.c,v 1.11 2011-04-14 16:09:36 sasa Exp $*/
/*10.03.2010	06.09.2006	Белых А.И.	peh1w.c
Распечатка корешка
*/
#include <stdlib.h>
#include        "buhg_g.h"

extern char *organ;
extern short	pehfam; //0-фамилия в корешке мелко 1-крупно
extern SQL_baza bd;

void    peh1w(const char *np,//Наименование подразделения
const char *zap,long tabb,const char *inn,short mr,short gr,
const char *dolg,int kategor,
FILE *kaw,FILE *kawk1,FILE *kawk2,FILE *ffkor2s,
GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
char		nait[512];
char		str[1024];
class iceb_u_str naim_kateg("");
SQLCURSOR curr;

sprintf(strsql,"select naik from Kateg where kod=%d",kategor);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
  naim_kateg.new_plus(row[0]);

sprintf(str,"\
------------------------------------------------------------");

strfilw(str,str,1,kaw);
strfilw(str,str,1,ffkor2s);

if(kawk1 != NULL)
 fprintf(kawk1,"\n%s\n",str);
if(kawk2 != NULL)
 fprintf(kawk2,"\n%s\n",str);

sprintf(str,"%s",organ);
strfilw(str,str,0,kaw);
strfilw(str,str,0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",str);
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str);

sprintf(str,"%s",np);
strfilw(str,str,0,kaw);
strfilw(str,str,0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",str);
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str);


sprintf(str,"%s %d.%d%s %s:%s",
gettext("Дата"),mr,gr,
gettext("г."),gettext("Фамилия"),zap);

strfilw(str,str,0,kaw);
strfilw(str,str,0,ffkor2s);

if(pehfam == 1)
 sprintf(str,"%s %d.%d%s %s:\x12%s\x0F",
 gettext("Дата"),mr,gr,
 gettext("г."),gettext("Фамилия"),zap);
else
 sprintf(str,"%s %d.%d%s %s:%s",
 gettext("Дата"),mr,gr,
 gettext("г."),gettext("Фамилия"),zap);

if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",str);
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str);

sprintf(str,"%s:%s",
gettext("Должность"),dolg);

strfilw(str,str,0,kaw);
strfilw(str,str,0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",str);
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str);

sprintf(str,"%s:%d %s",gettext("Категория"),kategor,naim_kateg.ravno());

strfilw(str,str,0,kaw);
strfilw(str,str,0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",str);
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str);

sprintf(str,"%s %ld %s %s",
gettext("Табельный номер"),tabb,
gettext("Иден.ном."),inn);

strfilw(str,str,0,kaw);
strfilw(str,str,0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",str);
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str);

int tarif=0; 
double okld=0.;
int kollgot=0;
class iceb_u_str kod_lgot("");
float lgota=zarlgotw(tabb,mr,gr,&kollgot,&kod_lgot,NULL,wpredok);

if((okld=okladw(tabb,mr,gr,&tarif,wpredok)) != 0.)
 {
  sprintf(str,"%s:%.2f %s:%.2f%%/%d",gettext("Оклад"),okld,gettext("Льгота"),lgota,kollgot);
  if(tarif == 1)
    sprintf(str,"%s:%.2f %s:%.2f%%/%d",gettext("Тариф"),okld,gettext("Льгота"),lgota,kollgot);
  strfilw(str,str,0,kaw);
  strfilw(str,str,0,ffkor2s);
  if(kawk1 != NULL)
   fprintf(kawk1,"%s\n",str);
  if(kawk2 != NULL)
   fprintf(kawk2,"%s\n",str);
 }  
else
 {
  sprintf(str,"%s:%.2f%%/%d",gettext("Льгота"),lgota,kollgot);
  if(tarif == 1)
    sprintf(str,"%s:%.2f%%/%d",gettext("Льгота"),lgota,kollgot);
  strfilw(str,str,0,kaw);
  strfilw(str,str,0,ffkor2s);
  if(kawk1 != NULL)
   fprintf(kawk1,"%s\n",str);
  if(kawk2 != NULL)
   fprintf(kawk2,"%s\n",str);
 }

/*Читаем табель*/
sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where \
god=%d and mes=%d and tabn=%ld",gr,mr,tabb);
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  /*Читаем наименование табеля*/
  memset(nait,'\0',sizeof(nait));
  sprintf(strsql,"select naik from Tabel where kod=%s",
  row[0]);

  memset(nait,'\0',sizeof(nait)-1); 

  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
   {
    sprintf(strsql,"%s %s !\n",gettext("Не найден код табеля"),row[0]);
    iceb_menu_soob(strsql,wpredok);
    continue;
   }
  else
    strncpy(nait,row1[0],sizeof(nait)-1);

   sprintf(str,"%-*.*s: %2.fд. %3.fч. %2.fк.д.",
   iceb_u_kolbait(40,nait),iceb_u_kolbait(40,nait),nait,
   atof(row[1]),atof(row[2]),atof(row[3]));
   strfilw(str,str,0,kaw);
   strfilw(str,str,0,ffkor2s);
   if(kawk1 != NULL)
    fprintf(kawk1,"%s\n",str);
   if(kawk2 != NULL)
    fprintf(kawk2,"%s\n",str);

 }
}
