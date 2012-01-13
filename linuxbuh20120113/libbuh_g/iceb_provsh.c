/* $Id: iceb_provsh.c,v 1.11 2011-02-21 07:36:07 sasa Exp $ */
/*03.01.2010	20.07.1998	Белых А.И.	iceb_provsh.c
Проверка счета для автоматических проводок
Проверка на наличие в плане счетов такого счета и
если счет с развернутым сальдо то проверка на наличие
кода контрагента в списке счета
Если вернули 0 все впорядке
             1 - не найден счет в плане счетов
             2 - не определена организация
             3 - отказались искать организацию
             4 - счет имеет субсчета
             5 - счет не балансовий
*/

#include        <ctype.h>
#include        <time.h>
#include        "iceb_libbuh.h"
#include        <unistd.h>

extern SQL_baza	bd;

int iceb_provsh(class iceb_u_str *kor,const char *shp4,
struct OPSHET *shetv,
int metka, /*************
           Передается:
           1 автоматическая вставка кода контрагента в список счета
              и общий список если его там нет. Применяется только из
              подсистемы расчет зарплаты для первичного формирования
              списков контрагентов
           2 для других подсистем
           ************/
int metkanbs, //0-не балансовый счет не возможен 1-возможен
GtkWidget *wpredok)
{
char		nai[80];
short		i;
char		strsql[1024];
SQL_str         row;
char		bros[312];


memset(nai,'\0',sizeof(nai));
shetv->saldo=0;
if((i=iceb_prsh(shp4,shetv,wpredok)) == 0)
 {
  sprintf(strsql,gettext("Нет счета %s в плане счетов !"),shp4);
  iceb_menu_soob(strsql,wpredok); 
  return(1);
 }
if(i == 2)
 {
  sprintf(strsql,gettext("Счет %s имеет субсчета. Введите нужный субсчет."),shp4);
  iceb_menu_soob(strsql,wpredok); 
  return(4);
 }
if(metkanbs == 0  && shetv->stat == 1)
 {
  sprintf(strsql,gettext("Счет %s внебалансовый !"),shp4);
  iceb_menu_soob(strsql,wpredok); 
  return(5);
 }

if(shetv->saldo == 3)
 {
  if(kor->ravno()[0] != '\0')
   {
    sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",shp4,kor->ravno());
    if(iceb_sql_readkey(strsql,wpredok) != 1)
     {
      if(metka != 0) //Автоматическая вставка кода контрагента
       {
        int metka_prov_kont=0;
        //Проверяем есть ли в общем списке
        sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kor->ravno());
        if((metka_prov_kont=iceb_sql_readkey(strsql,wpredok)) != 1)
         {
          if(metka == 2) /*Добавлять в общий список нельзя*/
           {
            sprintf(strsql,"%s %s",kor->ravno(),gettext("Не найден код контрагента в общем списке!"));
            iceb_menu_soob(strsql,wpredok);
            return(3);
           }

         }

        sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) values ('%s','%s',%d,%ld)",
        shp4,kor->ravno(),iceb_getuid(wpredok),time(NULL));

        iceb_sql_zapis(strsql,1,0,wpredok);

        if(metka_prov_kont == 1) /*Есть в общем списке*/
         return(0);

        if(metka == 1)
         {
          //Узнаем фамилию
          class iceb_u_str pristavka;
          iceb_poldan("Приставка к табельному номеру",&pristavka,"zarnast.alx",wpredok);
          class iceb_u_str tabn;
          tabn.plus(iceb_u_adrsimv(iceb_u_strlen(pristavka.ravno()),kor->ravno()));

          class iceb_u_str nai("");


          class iceb_u_str adres("");

          class iceb_u_str telef("");

          class iceb_u_str inn("");
/*************          
          int kk=0;
          for(i=0; i < (signed)strlen(kor);i++)
            if(isdigit(kor[i]) != 0)
             tabn[kk++]=kor[i];
**************/
          SQLCURSOR curr;
          sprintf(strsql,"select fio,adres,telef,inn from Kartb where tabn=%s",tabn.ravno());
          if(sql_readkey(&bd,strsql,&row,&curr) == 1)
           {
            nai.new_plus(row[0]);

            adres.new_plus(row[1]);

            telef.new_plus(row[2]);
         
            inn.new_plus(row[3]);
           }

          sprintf(strsql,"replace into Kontragent \
(kodkon,naikon,adres,innn,telef,ktoz,vrem) \
values ('%s','%s','%s','%s','%s',%d,%ld)",kor->ravno(),nai.ravno_filtr(),adres.ravno_filtr(),inn.ravno_filtr(),telef.ravno_filtr(),iceb_getuid(wpredok),time(NULL));
          if(sql_zap(&bd,strsql) != 0)
           {
            sprintf(bros,"iceb_provsh-2-%s",gettext("Ошибка записи !"));
            iceb_msql_error(&bd,bros,strsql,wpredok);
           }
          return(0);
         }        

       }

      iceb_u_str repl;
      repl.plus(gettext("Не нашли код контрагента"));
      repl.plus(" ");
      repl.plus(kor->ravno());
      repl.plus(gettext("в списке счёта"));
      repl.plus(" ");
      repl.plus(shp4);
      repl.plus(" !");

      repl.ps_plus(gettext("Будете просматривать список контрагентов ?"));

      if(iceb_menu_danet(&repl,2,wpredok) == 1)
       {
        kor->new_plus("");
        iceb_u_str kontr;
        kontr.plus("");
        iceb_u_str nai;
        nai.plus("");
        if(iceb_l_kontrsh(1,shp4,&kontr,&nai,wpredok) != 0)
         {
          return(2);
         }
        kor->new_plus(kontr.ravno());
       }
      else
       {
        return(3);
       }
     }
   }
  else
   {

    iceb_u_str repl;

    sprintf(strsql,gettext("Не задан код контрагента для счета %s !"),shp4);
    repl.plus(strsql);
    repl.ps_plus(gettext("Будете просматривать список контрагентов ?"));
    
    if(iceb_menu_danet(&repl,2,wpredok) == 1)
     {
      kor->new_plus("");
      iceb_u_str kontr;
      kontr.plus("");
      iceb_u_str nai;
      nai.plus("");
      if(iceb_l_kontrsh(1,shp4,&kontr,&nai,wpredok) != 0)
       {
        return(2);
       }
      kor->new_plus(kontr.ravno());
     }
    else
     {
      return(3);
     }
   }
 }
else
  kor->new_plus("");
return(0);
}
