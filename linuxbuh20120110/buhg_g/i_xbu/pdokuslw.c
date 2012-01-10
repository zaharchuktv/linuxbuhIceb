/*$Id:*/
/*22.01.2009	11.01.2009	Белых А.И.	Белых А.И.
Просмотр документа в подсистеме "Учёт услуг
*/
#include <stdlib.h>
#include	<pwd.h>
#include	<time.h>
#include <errno.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include <unistd.h>

extern double	okrcn;  /*Округление цены*/
extern SQL_baza bd;

int pdokusl(const char *datadok,
int podr,
const char *nomdok,
int tipz,
GtkWidget *wpredok)
{
short dd,md,gd;
char strsql[512];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
class iceb_u_str naim("");
FILE *ff;
char imaf[56];
iceb_u_rsdat(&dd,&md,&gd,datadok,1);

sprintf(strsql,"select * from Usldokum where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
gd,md,dd,podr,nomdok,tipz);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s\n%d.%d.%d %d %s %d",gettext("Не найден документ!"),
  dd,md,gd,podr,nomdok,tipz);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 
 }
int mnds=atoi(row[7]);

sprintf(imaf,"pdokusl%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }

fprintf(ff,"%s\n",gettext("Учёт услуг"));
if(tipz == 1)
 fprintf(ff,"%s\n",gettext("Приход"));
if(tipz == 2)
 fprintf(ff,"%s\n",gettext("Расход"));
 
fprintf(ff,"%s:%d.%d.%d %s:%s\n",
gettext("Дата документа"),dd,md,gd,
gettext("Номер документа"),nomdok);



naim.new_plus("");
sprintf(strsql,"select naik from Uslpodr where kod=%d",podr);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%d %s\n",gettext("Подразделение"),podr,naim.ravno());

naim.new_plus("");
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[3]);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Контрагент"),row[3],naim.ravno());

naim.new_plus("");
if(tipz == 1)
  sprintf(strsql,"select naik from Usloper1 where kod='%s'",row[6]);
if(tipz == 2)
  sprintf(strsql,"select naik from Usloper2 where kod='%s'",row[6]);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Код операции"),row[6],naim.ravno());

fprintf(ff,"%s:%s\n",gettext("Записал"),iceb_kszap(row[21],1,wpredok));
fprintf(ff,"%s:%s\n",gettext("Дата записи"),iceb_u_vremzap(row[22]));


if(atoi(row[27]) == 0)
 fprintf(ff,"%s\n",gettext("Документ неоплачен"));
if(atoi(row[27]) == 1)
 fprintf(ff,"%s\n",gettext("Документ оплачен"));

naim.new_plus("");       
sprintf(strsql,"select naik from Foroplat where kod='%s'",row[8]);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 naim.new_plus(row1[0]);
fprintf(ff,"%s:%s %s\n",gettext("Форма оплаты"),row[8],naim.ravno());

if(row[9][0] != '0')
 fprintf(ff,"%s:%s\n",gettext("Дата оплаты"),iceb_u_datzap(row[9]));


if(row[10][0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Условие продажи"),row[10]);

double suma_kor=atof(row[11]);

if(row[16][0] != '\0')
   fprintf(ff,"%s:%s\n",gettext("Номер налоговой накладной"),row[16]);

if(row[24][0] != '0');
 fprintf(ff,"%s:%s\n",gettext("Дата получения налоговой накладной"),iceb_u_datzap(row[24]));
if(row[17][0] != '0')
 fprintf(ff,"%s:%s\n",gettext("Дата выписки налоговой накладной"),iceb_u_datzap(row[17]));

if(row[18][0] != '\0')
 {
  fprintf(ff,"%s:%s ",gettext("Доверенность"),row[18]);
  fprintf(ff,"%s:%s\n",gettext("Дата"),iceb_u_datzap(row[19]));
 }
if(row[20][0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Через кого"),row[20]);

if(row[23][0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Основание"),row[23]);

double suma_nds=atof(row[25]);
float pnds=atof(row[26]);

fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
fprintf(ff,"\
Код   |  Наименование услуги         |Счёт уч.|Eд.изм.|Количество|  Цена    | Сумма    |\n");
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");

sprintf(strsql,"select metka,kodzap,kolih,cena,ei,shetu,ktoi,vrem,dnaim from Usldokum1 where \
datd='%04d-%02d-%02d' and podr=%d and tp=%d and nomd='%s'",
gd,md,dd,podr,tipz,nomdok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok );
 }
char naimu[112];
int metka=0;
double kolih=0.;
double cena=0.;
double suma=0.;
double itogo=0.;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  metka=atoi(row[0]);
  kolih=atof(row[2]);
  cena=atof(row[3]);
  cena=iceb_u_okrug(cena,okrcn);
  suma=kolih*cena;
  itogo+=suma;
    
  /*Читаем наименование услуги*/
  memset(naimu,'\0',sizeof(naimu));
  
  if(metka == 0)
   sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[1]));

  if(metka == 1)
   sprintf(strsql,"select naius from Uslugi where kodus=%d",atoi(row[1]));

  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    if(metka == 0)
     sprintf(strsql,"%s %s !\n",gettext("Не найден код материалла"),row[1]);
    if(metka == 1)
     sprintf(strsql,"%s %s !\n",gettext("Не найден код услуги"),row[1]);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    strncpy(naimu,row1[0],sizeof(naimu)-1);
    if(row[7][0] != '\0')
     {
      strcat(naimu," ");
      strcat(naimu,row[7]);
     }    
   }

  fprintf(ff,"%6s %-*.*s %-*.*s %-*.*s %10.10g %10.10g %10.2f %s %s\n",
  row[1],
  iceb_u_kolbait(30,naimu),
  iceb_u_kolbait(30,naimu),
  naimu,
  iceb_u_kolbait(8,row[5]),
  iceb_u_kolbait(8,row[5]),
  row[5],
  iceb_u_kolbait(7,row[4]),
  iceb_u_kolbait(7,row[4]),
  row[4],
  kolih,cena,suma,
  iceb_u_vremzap(row[7]),
  iceb_kszap(row[6],1,wpredok));
  

  if(iceb_u_strlen(naimu) > 30)
   fprintf(ff,"%6s %s\n","",iceb_u_adrsimv(30,naimu));
 }
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(76,gettext("Итого")),gettext("Итого"),itogo);

if(suma_kor < 0.)
  fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(76,gettext("Скидка")),gettext("Скидка"),suma_kor);
if(suma_kor > 0.)
  fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(76,gettext("Надбавка")),gettext("Надбавка"),suma_kor);

double oplata=itogo-suma_kor;

if(mnds == 0) 
 {
  double nds=0.;

  if(suma_nds != 0.)
   nds=suma_nds;
  else
    nds=oplata*pnds/100.;

  oplata=oplata+nds;
  fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(76,gettext("НДС")),gettext("НДС"),nds);
 }    
if(itogo != oplata)
 fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(76,gettext("Итого")),gettext("Итого"),oplata);
iceb_podpis(ff,wpredok);
fclose(ff);

iceb_prosf(imaf,wpredok);
sleep(1); /*для того чтобы не удалила файл перед просмотром*/
unlink(imaf);

return(0);
}
