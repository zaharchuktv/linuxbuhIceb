/*$Id: iceb_razuz.c,v 1.10 2011-02-21 07:36:07 sasa Exp $*/
/*06.11.2007	18.01.2000	Белых А.И.	iceb_razuz.c
Разузлование - определение какие узлы и в каком количестве
входят в изделие
Возвращает количество входящих узлов

Определяем все узлы изделия первого уровня.
Если они есть создаем массивы и заполняем их.
Дальше работаем в цикле.

Берем первый узел и определяем есть ли в нем входящие узлы.

Если нет проверяем следующий узел.
Если есть проверяем есть ли они в списке узлов
Если есть увеличиваем количество
Если нет добавляем в список узлов
Берем для проверки следующий узел


*/
#include        <stdlib.h>
#include        "iceb_libbuh.h"

extern SQL_baza	bd;
#if 0
int		iceb_razuz(int kodiz, //Код изделия которое нужно обработать
iceb_u_int *kodu, //Массив кодов узлов
iceb_u_double *kolu, //Массив количества узлов
GtkWidget *wpredok)
#endif
int iceb_razuz(class iceb_razuz_data *data,GtkWidget *wpredok)
{
SQL_str         row;
char		strsql[512];
int		koluz=0;
int		kodviz;
int		kolstr,kolstr1;
int		zsuz,i;
int		kodizo;
double          kolviz=1.;

/*
printw("\nrazuz-Код изделия: %d\n",data->kod_izd);
refresh();
*/

zsuz=koluz=0;
kodizo=data->kod_izd;
SQLCURSOR cur;
SQLCURSOR cur1;

//naz:;
for(;;)
 {
  sprintf(strsql,"select kodd,kolih from Specif where kodi=%d",kodizo);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return(0);
   }

  if(kolstr == 0 )
   {
    /*В изделии нет ни одной записи*/
    printf("\niceb_razuzw-Нет ни одной записи в изделии !!!\n%s\n%d\n",strsql,kodizo);
    return(0);
   }


  while(cur.read_cursor(&row) != 0)
   {

  //  printw("%s %s %d %d\n",row[0],row[1],zsuz,koluz);
  //  OSTANOV();

    /*Проверяем не является ли входящий материал изделием*/
    sprintf(strsql,"select distinct kodi from Specif where kodi=%s",row[0]);
    if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
      continue;
     }
    kodviz=0;
    if(kolstr1 != 0)
      kodviz=atoi(row[0]);
    else
      continue;

    if(kodviz == data->kod_izd)
     {
      iceb_u_str repl;
      repl.plus("razuzw-Ошибка в спецификации !");
      sprintf(strsql,"Изделие входит само в себя в узле %d. Это не возможно !",kodizo);
      repl.ps_plus(strsql);
      iceb_menu_soob(&repl,wpredok);
      continue;
     }

    double kkk=kolviz*atof(row[1]);

    /*Проверяем есть ли он в массиве*/
    if((i=data->kod_uz.find(kodviz)) != -1)
     {
      data->kolih_uz.plus(kkk,i);
     }
    else
     {
      koluz++;

  //    printw("Новый узел %d\n",kodviz);

      data->kod_uz.plus(kodviz,-1);
      data->kolih_uz.plus(kkk,-1);

     }


   }

   
  /*Обрабатываем узлы*/
  if(zsuz < koluz)
   {
    kodizo=data->kod_uz.ravno(zsuz);
    kolviz=data->kolih_uz.ravno(zsuz);
    zsuz++;
//    goto naz;
   }
  else
   break;
 }
return(koluz);

}
