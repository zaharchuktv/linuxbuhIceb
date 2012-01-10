/*$Id: xudrasdokw.c,v 1.15 2011-07-06 08:12:30 sasa Exp $*/
/*22.03.2010	02.04.2009	Белых А.И.	xudrasdokw.c
Распечатка доверенности
*/
#include <stdlib.h>
#include <errno.h>
#include "buhg_g.h"
#include <unistd.h>

extern SQL_baza bd;

int xudrasdokw(class iceb_u_str *datdov,class iceb_u_str *nomdov,
class iceb_u_spisok *imafo,
class iceb_u_spisok *naimf,
GtkWidget *wpredok)
{
FILE *ff;
char imaf[56];
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str komu("");
class iceb_u_str post("");
class iceb_u_str datad("");
class iceb_u_str nomnar("");
class iceb_u_str datanar("");
class iceb_u_str inn("");
class iceb_u_str nspnds("");
class iceb_u_str telef("");
class iceb_u_str adres("");
class iceb_u_str mfo("");
class iceb_u_str nomshet("");
class iceb_u_str naim_bank("");
class iceb_u_str naim_00("");
class iceb_u_str dolg("");
class iceb_u_str dokum("");
class iceb_u_str seriq("");
class iceb_u_str nomerdok("");
class iceb_u_str datavid("");
class iceb_u_str vidan("");
class iceb_u_str kod_edrp("");
/*Читаем шапку документа*/
sprintf(strsql,"select * from Uddok where datd='%s' and nomd='%s'",
datdov->ravno_sqldata(),nomdov->ravno());

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  komu.new_plus(row[3]);
  post.new_plus(row[4]);

  datad.new_plus(iceb_u_datzap(row[2]));
  nomnar.new_plus(row[8]);
  datanar.new_plus(iceb_u_datzap(row[7]));
  dokum.new_plus(row[5]);
  seriq.new_plus(row[9]);
  nomerdok.new_plus(row[10]);
  datavid.new_plus(iceb_u_datzap(row[11]));
  vidan.new_plus(row[12]);
  dolg.new_plus(row[13]);
 }
class iceb_u_str kod00;
iceb_poldan("Код контрагента с нужными банковскими реквизитами",&kod00,"nastud.alx",wpredok);
if(kod00.getdlinna() <= 1)
 kod00.new_plus("00");
sprintf(strsql,"select * from Kontragent where kodkon='%s'",kod00.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  naim_00.new_plus(row[1]);
  if(row[16][0] != '\0')
   naim_00.new_plus(row[16]);
  kod_edrp.new_plus(row[5]);
  inn.new_plus(row[8]);
  nspnds.new_plus(row[9]);
  adres.new_plus(row[3]);
  mfo.new_plus(row[6]);
  nomshet.new_plus(row[7]);
  naim_bank.new_plus(row[2]);
  if(iceb_u_polen(row[10],&telef,1,' ') != 0)
   telef.new_plus(row[10]);
 }
sprintf(imaf,"dov%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }

char stroka[1024];


//fprintf(ff,"IПH - %s  NCB - %s тел.%-15s \n",inn.ravno(),nspnds.ravno(),telef.ravno());  
fprintf(ff,"Одержувач:%-*s Типова форма N M-2\n",45,naim_00.ravno());

if(iceb_u_strlen(naim_00.ravno()) > 45)
 fprintf(ff,"%s\n",iceb_u_adrsimv(45,naim_00.ravno()));
 
fprintf(ff,"Індивідуальний податковий номер:%-*s       Затверджена наказом Мінстату України\n",
iceb_u_kolbait(17,inn.ravno()),inn.ravno());

fprintf(ff,"Номер свідоцтва платника ПДВ:%-*s 21.06.96р. N192 \n",
iceb_u_kolbait(26,nspnds.ravno()),nspnds.ravno());

fprintf(ff,"Телефон одержувача:%-*s\n",iceb_u_kolbait(10,telef.ravno()),telef.ravno());
int dlinna_str=50;
sprintf(stroka,"Адреса одержувача: %s",adres.ravno());

for(int ii=0; ii < iceb_u_strlen(stroka); ii+=dlinna_str)
 {
  fprintf(ff,"%-*.*s\n",
  iceb_u_kolbait(dlinna_str,iceb_u_adrsimv(ii,stroka)),
  iceb_u_kolbait(dlinna_str,iceb_u_adrsimv(ii,stroka)),
  iceb_u_adrsimv(ii,stroka));
 }
fprintf(ff,"Ідентифікаційний код ЄДРПОУ:%-*s          Код за УКУД\n",
iceb_u_kolbait(20,kod_edrp.ravno()),kod_edrp.ravno());

fprintf(ff,"Платник:%s\n",naim_00.ravno());

sprintf(stroka,"Адреса платника: %s",adres.ravno());

for(int ii=0; ii < iceb_u_strlen(stroka); ii+=dlinna_str)
 fprintf(ff,"%.*s\n",
  iceb_u_kolbait(dlinna_str,iceb_u_adrsimv(ii,stroka)),
  iceb_u_adrsimv(ii,stroka));

fprintf(ff,"Счёт:%s MФО:%s\n",nomshet.ravno(),mfo.ravno());
fprintf(ff,"Банк:%-*.*s Довіреність дійсна до %s р.\n\n",
iceb_u_kolbait(50,naim_bank.ravno()),iceb_u_kolbait(50,naim_bank.ravno()),naim_bank.ravno(),
datad.ravno());


fprintf(ff,"\x1B\x50"); /*10-знаков*/

fprintf(ff,"\x0E"); /*Включение режима удвоенной ширины*/
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff,"          ДОВІРЕНІСТЬ N%s",nomdov->ravno());
fprintf(ff,"\x14"); /*Выключение режима удвоенной ширины*/
fprintf(ff,"\n");
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта
fprintf(ff,"\x1B\x50"); /*10-знаков*/

fprintf(ff,"                      Дата видачі %s р.\n\n",datdov->ravno());

fprintf(ff,"Видано");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %s %s\n",dolg.ravno(),komu.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"\nДокумент що засвідчує особу");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %s\n",dokum.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта


fprintf(ff,"серія");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %-*s",iceb_u_kolbait(5,seriq.ravno()),seriq.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff," N");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %-*s",iceb_u_kolbait(20,nomerdok.ravno()),nomerdok.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff," від");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %s р.\n",datavid.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"\nВиданий");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %s\n",vidan.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"\nНа отримання від");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %s\n",post.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"цінностей за");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %s %s\n",nomnar.ravno(),datanar.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"\x1B\x4D"); //12 знаков на дюйм
fprintf(ff,"\f\
Перелік цінностей, які належить отримати:\n");
fprintf(ff,"\
------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 N |         Найменування цінностей         |Од.вим.|     Кількість (прописом)     |\n");
fprintf(ff,"\
------------------------------------------------------------------------------------\n");
/*
123 1234567890123456789012345678901234567890 1234567 123456789012345678901234567890
*/
/*Читаем записи в документе*/
sprintf(strsql,"select zapis,ei,kol from Uddok1 where datd='%s' and nomd='%s'",
datdov->ravno_sqldata(),nomdov->ravno());
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
char kolih[2048];

int nomzap=0;
int kol_str=0;
while(cur.read_cursor(&row) != 0)
 {
  memset(kolih,'\0',sizeof(kolih));
  strncpy(kolih,iceb_u_nofwords(atof(row[2])),sizeof(kolih)-1);
//  printf("%s-%s\n",__FUNCTION__,kolih);
  kol_str++;
  fprintf(ff,"%3d|%-*.*s|%-*.*s|%-*.*s|\n",
  ++nomzap,
  iceb_u_kolbait(40,row[0]),iceb_u_kolbait(40,row[0]),row[0],
  iceb_u_kolbait(7,row[1]),iceb_u_kolbait(7,row[1]),row[1],
  iceb_u_kolbait(30,kolih),
  iceb_u_kolbait(30,kolih),
  kolih);

  /**********1 дополнительная строка*********************/  
  if(iceb_u_strlen(row[0]) > 40 && iceb_u_strlen(kolih) <= 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-*.*s|%-7.7s|%-30.30s|\n",
    "",
    iceb_u_kolbait(40,iceb_u_adrsimv(40,row[0])),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,row[0])),
    iceb_u_adrsimv(40,row[0]),
    "","");
   }
  if(iceb_u_strlen(row[0]) > 40 && iceb_u_strlen(kolih) > 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-*.*s|%-7.7s|%-*.*s|\n","",
    iceb_u_kolbait(40,iceb_u_adrsimv(40,row[0])),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,row[0])),
    iceb_u_adrsimv(40,row[0]),
    "",
    iceb_u_kolbait(30,iceb_u_adrsimv(30,kolih)),
    iceb_u_kolbait(30,iceb_u_adrsimv(30,kolih)),
    iceb_u_adrsimv(30,kolih));
   }

  if(iceb_u_strlen(row[0]) <= 40 && iceb_u_strlen(kolih) > 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-40.40s|%-7.7s|%-*.*s|\n","","","",
    iceb_u_kolbait(30,iceb_u_adrsimv(30,kolih)),
    iceb_u_kolbait(30,iceb_u_adrsimv(30,kolih)),
    iceb_u_adrsimv(30,kolih));
   }

  /**********2 дополнительная строка*********************/  
  if(iceb_u_strlen(row[0]) > 80 && iceb_u_strlen(kolih) <= 60)
   {
    kol_str++;
    fprintf(ff,"%3s|%-*.*s|%-7.7s|%-30.30s|\n",
    "",
    iceb_u_kolbait(40,iceb_u_adrsimv(80,row[0])),
    iceb_u_kolbait(40,iceb_u_adrsimv(80,row[0])),
    iceb_u_adrsimv(80,row[0]),
    "","");
   }
  if(iceb_u_strlen(row[0]) > 40 && iceb_u_strlen(kolih) > 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-*.*s|%-7.7s|%-*.*s|\n",
    "",
    iceb_u_kolbait(40,iceb_u_adrsimv(80,row[0])),
    iceb_u_kolbait(40,iceb_u_adrsimv(80,row[0])),
    iceb_u_adrsimv(80,row[0]),
    "",
    iceb_u_kolbait(30,iceb_u_adrsimv(60,kolih)),
    iceb_u_kolbait(30,iceb_u_adrsimv(60,kolih)),
    iceb_u_adrsimv(60,kolih));
   }

  if(iceb_u_strlen(row[0]) <= 40 && iceb_u_strlen(kolih) > 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-40.40s|%-7.7s|%-*.*s|\n",
    "",
    "",
    "",
    iceb_u_kolbait(30,iceb_u_adrsimv(60,kolih)),
    iceb_u_kolbait(30,iceb_u_adrsimv(60,kolih)),
    iceb_u_adrsimv(60,kolih));
   }

  kolstr++;
  fprintf(ff,"\
------------------------------------------------------------------------------------\n");
 }
for(; kol_str < 10; kol_str+=2)
 {
  fprintf(ff,"%3s|%-40.40s|%-7.7s|%-30.30s|\n","","","","");
  fprintf(ff,"\
------------------------------------------------------------------------------------\n");
 }
fprintf(ff,"\n\
          Підпис_________________________________________________засвідчую\n\
                 (зразок підпису особи, що одержала довіреність)\n");
fprintf(ff,"\n\
          Керівник підприємства\n\n");
fprintf(ff,"\
          Головний бухгалтер\n\n");

fprintf(ff,"\
          Місце печатки\n");


fclose(ff);


imafo->plus(imaf);
naimf->plus(gettext("Распечатка доверенности"));
iceb_ustpeh(imaf,1,wpredok);


return(0);
}
