/*$Id: iceb_razuz1.c,v 1.11 2011-02-21 07:36:07 sasa Exp $*/
/*06.11.2007	18.01.2000	Белых А.И.	iceb_razuz1.c
Определение входимости первичных деталей в изделие
Возвращаем количество наименований деталей в изделии
*/
#include        <stdlib.h>
#include        "iceb_libbuh.h"

extern SQL_baza	bd;
#if 0
int		iceb_razuz1(int kodiz, //Код изделия которое нужно обработать
int koluz,      //Количество узлов
iceb_u_int *kodu,      //Массив кодов узлов
iceb_u_double *kolu,   //Массив количества узлов
iceb_u_int *kodd,     //Массив кодов записей
iceb_u_double *kold,  //Массив количества деталей
iceb_u_int *metka_zap, //Список меток записей (0-материал или 1-услуга)
GtkWidget *wpredok)
#endif
int iceb_razuz1(class iceb_razuz_data *data,GtkWidget *wpredok)
{
SQL_str         row;
char		strsql[512];
int		kolstr,kolstr1;
int		kodizo;
int		koldd;
int		koddet;
int		poz,i;
double		koliz;

/*
printw("\nrazuz1-Код изделия: %d\n",data->kod_izd);
refresh();
*/

poz=koldd=0;
kodizo=data->kod_izd;
koliz=1.;
SQLCURSOR cur1;
SQLCURSOR cur;
int koluz=data->kolih_uz.kolih();

//naz:;
for(;;)
 {
  sprintf(strsql,"select kodd,kolih,kz,ei from Specif where kodi=%d",kodizo);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return(0);
   }

  if(kolstr == 0 )
   {
    printf("iceb_razuz1-Нет ни одной записи в изделии !!!\n%s\n",strsql);
    return(0);
   }


  while(cur.read_cursor(&row) != 0)
   {
  /*
    printw("%s %s\n",row[0],row[1]);
    refresh();
  */
    /*Проверяем не является ли входящий материал изделием*/
    sprintf(strsql,"select kodi from Specif where kodi=%s limit 1",row[0]);
    if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
      continue;
     }

    koddet=0;
    if(kolstr1 == 0)
      koddet=atoi(row[0]);
    else
      continue;

    /*Проверяем есть ли деталь в массиве*/
    if((i=data->kod_det.find(koddet)) == -1)
     {
      koldd++;
      data->kod_det.plus(koddet,-1);
    //  metka_zap->plus(atoi(row[2]),i);
     }    
    
    sprintf(strsql,"%d|%s",koddet,row[3]);
    if((i=data->kod_det_ei.find(strsql)) < 0)
     {
      data->kod_det_ei.plus(strsql); 
      data->metka_mu.plus(atoi(row[2]),-1);
     }
    data->kolih_det_ei.plus(atof(row[1])*koliz,i);
    //kold->plus(atof(row[1])*koliz,i);
    
   }

  /*
  printw("poz=%d koluz=%d\n",poz,koluz);
  refresh();
  */
  /*Обрабатываем узлы*/
  if(poz < koluz)
   {
    kodizo=data->kod_uz.ravno(poz);
    koliz=data->kolih_uz.ravno(poz);
  /*
    printw("Узел:%dколичество %d\n",kodizo,koliz);
    refresh();
  */
    poz++;
  //  goto naz;
   }
  else
   break;
 }

return(koldd);

}
