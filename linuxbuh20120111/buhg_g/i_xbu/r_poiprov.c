/*$Id: r_poiprov.c,v 1.26 2011-02-21 07:35:56 sasa Exp $*/
/*12.06.2010	19.01.2004	Белых А.И.	r_poiprov.c
Распечатка найденных проводок из режима поиска проводок
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "prov_poi.h"

class r_poiprov_data
 {
  public:
  prov_poi_data *rek_poi;
  SQLCURSOR cur;
  FILE *ff;
  FILE *ffd;
  FILE *ffs;
  iceb_u_spisok imaf; //Имена файлов с отчетами
  iceb_u_spisok naim; //Наименование отчетов
  GtkWidget *window;
  GtkWidget *bar;
  GtkTextBuffer *buffer;
  gfloat kolstr1; //Количество прочитанных строк
  int    kolstr;  //Количество строк в курсоре
  

  double dbt,krt;
  double dbtden,krtden;
  double dbtmes,krtmes;
  iceb_u_spisok SHET;
  iceb_u_double DEB;
  iceb_u_double KRE;
  short         denz,mesz;
    
  r_poiprov_data() //Конструкор
   {
    ff=NULL;
    kolstr1=0.;
    denz=mesz=0;
    dbt=krt=0.;
    dbtden=krtden=0.;
    dbtmes=krtmes=0.;
   }
 };
gint   r_poiprov1(class r_poiprov_data *data);
void	rapsap(FILE *ff1);
void raspitog(double dbt,double krt,int metka,FILE *ff1);

extern char *name_system;
extern SQL_baza	bd;
extern char *organ;

void r_poiprov(class prov_poi_data *rekv_m_poiprov,GtkWidget *wpredok)
{
char strsql[512];
r_poiprov_data data;

data.rek_poi=rekv_m_poiprov;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/
gtk_window_set_deletable(GTK_WINDOW(data.window),FALSE); /*Выключить кнопку close в рамке окна*/

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

sprintf(strsql,"%s %s",name_system,gettext("Распечатка проводок"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

//gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

GtkWidget *label=gtk_label_new(gettext("Ждите !!!"));
gtk_widget_set_size_request(GTK_WIDGET(label),300,-1);
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

label=gtk_label_new(gettext("Распечатка проводок"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.bar=gtk_progress_bar_new();

gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_CONTINUOUS);

gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_LEFT_TO_RIGHT);

gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна
gtk_widget_show_all(data.window);

gtk_idle_add((GtkFunction)r_poiprov1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


iceb_rabfil(&data.imaf,&data.naim,"",0,wpredok);

}
/**************************************/
/*Расчет                              */
/***************************************/

gint   r_poiprov1(class r_poiprov_data *data)
{
char strsql[1024];
SQL_str row,row1;


if((data->kolstr=data->cur.make_cursor(&bd,data->rek_poi->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->rek_poi->zapros.ravno(),data->window);
  return(FALSE);
 }

sprintf(strsql,"prov%d.lst",getpid());

data->imaf.plus(strsql);
data->naim.plus(gettext("Распечатка проводок"));

if((data->ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return(FALSE);
 }

sprintf(strsql,"provd%d.lst",getpid());

data->imaf.plus(strsql);
data->naim.plus(gettext("Распечатка проводок c дневными итогами"));

if((data->ffd = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return(FALSE);
 }

sprintf(strsql,"provs%d.lst",getpid());

data->imaf.plus(strsql);
data->naim.plus(gettext("Свод по счетам корреспондентам"));

if((data->ffs = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return(FALSE);
 }

short dn,mn,gn;
short dk,mk,gk;
iceb_u_rsdat(&dn,&mn,&gn,data->rek_poi->datan.ravno(),1);
iceb_u_rsdat(&dk,&mk,&gk,data->rek_poi->datan.ravno(),1);

iceb_u_zagolov(gettext("Распечатка проводок"),dn,mn,gn,dk,mk,gk,organ,data->ff);
iceb_u_zagolov(gettext("Распечатка проводок"),dn,mn,gn,dk,mk,gk,organ,data->ffd);
iceb_u_zagolov(gettext("Свод по счетам корреспондентам"),dn,mn,gn,dk,mk,gk,organ,data->ffs);


if(data->rek_poi->datanz.getdlinna() > 1)
 {
  fprintf(data->ff,"%s:%s => %s\n",gettext("Поиск по дате записи"),data->rek_poi->datanz.ravno(),data->rek_poi->datakz.ravno());
  fprintf(data->ffd,"%s:%s => %s\n",gettext("Поиск по дате записи"),data->rek_poi->datanz.ravno(),data->rek_poi->datakz.ravno());
  fprintf(data->ffs,"%s:%s => %s\n",gettext("Поиск по дате записи"),data->rek_poi->datanz.ravno(),data->rek_poi->datakz.ravno());
 }

fprintf(data->ff,"%s:\n",gettext("Реквизиты поиска"));
fprintf(data->ffd,"%s:\n",gettext("Реквизиты поиска"));
fprintf(data->ffs,"%s:\n",gettext("Реквизиты поиска"));

if(data->rek_poi->shet.getdlinna() > 1)
 {
  fprintf(data->ff,"%s %s\n",gettext("Счет"),data->rek_poi->shet.ravno());
  fprintf(data->ffd,"%s %s\n",gettext("Счет"),data->rek_poi->shet.ravno());
  fprintf(data->ffs,"%s %s\n",gettext("Счет"),data->rek_poi->shet.ravno());
 }
if(data->rek_poi->shetk.getdlinna() > 1)
 {
  fprintf(data->ff,"%s %s\n",gettext("Счет корреспондент"),data->rek_poi->shetk.ravno());
  fprintf(data->ffd,"%s %s\n",gettext("Счет корреспондент"),data->rek_poi->shetk.ravno());
  fprintf(data->ffs,"%s %s\n",gettext("Счет корреспондент"),data->rek_poi->shetk.ravno());
 }
if(data->rek_poi->kontr.getdlinna() > 1)
 {
  fprintf(data->ff,"%s %s\n",gettext("Код контрагента"),data->rek_poi->kontr.ravno());
  fprintf(data->ffd,"%s %s\n",gettext("Код контрагента"),data->rek_poi->kontr.ravno());
  fprintf(data->ffs,"%s %s\n",gettext("Код контрагента"),data->rek_poi->kontr.ravno());
 }
if(data->rek_poi->koment.getdlinna() > 1)
 {
  fprintf(data->ff,"%s %s\n",gettext("Коментарий"),data->rek_poi->koment.ravno());
  fprintf(data->ffd,"%s %s\n",gettext("Коментарий"),data->rek_poi->koment.ravno());
  fprintf(data->ffs,"%s %s\n",gettext("Коментарий"),data->rek_poi->koment.ravno());
 }
if(data->rek_poi->debet.getdlinna() > 1)
 {
  fprintf(data->ff,"%s %s\n",gettext("Дебет"),data->rek_poi->debet.ravno());
  fprintf(data->ffd,"%s %s\n",gettext("Дебет"),data->rek_poi->debet.ravno());
  fprintf(data->ffs,"%s %s\n",gettext("Дебет"),data->rek_poi->debet.ravno());
 }
if(data->rek_poi->kredit.getdlinna() > 1)
 {
  fprintf(data->ff,"%s %s\n",gettext("Кредит"),data->rek_poi->kredit.ravno());
  fprintf(data->ffd,"%s %s\n",gettext("Кредит"),data->rek_poi->kredit.ravno());
  fprintf(data->ffs,"%s %s\n",gettext("Кредит"),data->rek_poi->kredit.ravno());
 }

if(data->rek_poi->metka.getdlinna() > 1)
 {
  fprintf(data->ff,"%s %s\n",gettext("Метка проводки"),data->rek_poi->metka.ravno());
  fprintf(data->ffd,"%s %s\n",gettext("Метка проводки"),data->rek_poi->metka.ravno());
  fprintf(data->ffs,"%s %s\n",gettext("Метка проводки"),data->rek_poi->metka.ravno());
 }
if(data->rek_poi->nomdok.getdlinna() > 1)
 {
  fprintf(data->ff,"%s %s\n",gettext("Номер документа"),data->rek_poi->nomdok.ravno());
  fprintf(data->ffd,"%s %s\n",gettext("Номер документа"),data->rek_poi->nomdok.ravno());
  fprintf(data->ffs,"%s %s\n",gettext("Номер документа"),data->rek_poi->nomdok.ravno());
 }
if(data->rek_poi->kodop.getdlinna() > 1)
 {
  fprintf(data->ff,"%s %s\n",gettext("Код операции"),data->rek_poi->kodop.ravno());
  fprintf(data->ffd,"%s %s\n",gettext("Код операции"),data->rek_poi->kodop.ravno());
  fprintf(data->ffs,"%s %s\n",gettext("Код операции"),data->rek_poi->kodop.ravno());
 }  
if(data->rek_poi->grupak.getdlinna() > 1)
 {
  fprintf(data->ff,"%s %s\n",gettext("Группа"),data->rek_poi->grupak.ravno());
  fprintf(data->ffd,"%s %s\n",gettext("Группа"),data->rek_poi->grupak.ravno());
  fprintf(data->ffs,"%s %s\n",gettext("Группа"),data->rek_poi->grupak.ravno());
 }  
if(data->rek_poi->kekv.getdlinna() > 1)
 {
  fprintf(data->ff,"%s:%s\n",gettext("Код экономической классификации затрат"),data->rek_poi->kekv.ravno());
  fprintf(data->ffd,"%s:%s\n",gettext("Код экономической классификации затрат"),data->rek_poi->kekv.ravno());
  fprintf(data->ffs,"%s:%s\n",gettext("Код экономической классификации затрат"),data->rek_poi->kekv.ravno());
 }  

rapsap(data->ff);
rapsap(data->ffd);
 

char naikon[512];
short d,m,g;
double debet,kredit;
int nomer=0;
int i=0;
char koment[512];
SQLCURSOR cur1;
while(data->cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

  if(prov_prov_row(row,data->rek_poi) != 0)
    continue;

  iceb_u_rsdat(&d,&m,&g,row[1],2);
  debet=atof(row[9]);
  kredit=atof(row[10]);

  sprintf(strsql,"%s|%s",row[2],row[3]);
  if((nomer=data->SHET.find(strsql)) <0)
    data->SHET.plus(strsql);

  data->DEB.plus(debet,nomer);
  data->KRE.plus(kredit,nomer);

  if(data->denz != d)
   {
    if(data->denz != 0)
      raspitog(data->dbtden,data->krtden,1,data->ffd);
    
    data->dbtden=data->krtden=0.;
    data->denz=d;
   }
  if(data->mesz != m)
   {
    if(data->mesz != 0)
      raspitog(data->dbtmes,data->krtmes,2,data->ffd);
    
    data->dbtmes=data->krtmes=0.;
    data->mesz=m;
   }

  memset(naikon,'\0',sizeof(naikon));;
  
  if(row[4][0] != '\0')
   {  
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
    row[4]);
    if((i=sql_readkey(&bd,strsql,&row1,&cur1)) != 1)
     {
      sprintf(strsql,"%02d.%02d.%d %-5s %-5s %-5s %-3s %-4s %10.2f %10.2f %s%-10.10s\n\
Error %d\n",      
      d,m,g,row[2],row[3],row[4],row[5],row[6],
      debet,kredit,row[8],row[13],i);
      iceb_u_str repl;
      repl.plus(strsql);
      iceb_menu_soob(&repl,data->window);
     }
    else
     strncpy(naikon,row1[0],sizeof(naikon)-1);
   }       

  sozkom(koment,sizeof(koment),row[13],row[5],row[8]);

  fprintf(data->ff,"%02d.%02d.%4d %-*s %-*s %-*s %-3s%4s %-*s %10s",
  d,m,g,
  iceb_u_kolbait(6,row[2]),row[2],
  iceb_u_kolbait(6,row[3]),row[3],
  iceb_u_kolbait(5,row[4]),row[4],
  row[5],
  row[11],
  iceb_u_kolbait(10,row[6]),row[6],
  iceb_u_prnbr(debet));

  fprintf(data->ff," %10s %s %-*s %s\n",
  iceb_u_prnbr(kredit),
  row[8],
  iceb_u_kolbait(32,koment),koment,
  naikon);

  fprintf(data->ffd,"%02d.%02d.%4d %-*s %-*s %-*s %-3s%4s %-*s %10s",
  d,m,g,
  iceb_u_kolbait(6,row[2]),row[2],
  iceb_u_kolbait(6,row[3]),row[3],
  iceb_u_kolbait(5,row[4]),row[4],
  row[5],
  row[11],
  iceb_u_kolbait(10,row[6]),row[6],
  iceb_u_prnbr(debet));

  fprintf(data->ffd," %10s %s %-*s %s\n",
  iceb_u_prnbr(kredit),
  row[8],
  iceb_u_kolbait(32,koment),koment,
  naikon);

  if(data->rek_poi->datanz.getdlinna() > 1) //По дате записи
   {
    time_t tmm=atoi(row[12]);
    struct tm *bf;
    
    bf=localtime(&tmm);

    fprintf(data->ff,"%s: %d.%d.%d%s %s: %d:%d:%d\n",
    gettext("Дата записи"),
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
    gettext("г."),
    gettext("Время"),
    bf->tm_hour,bf->tm_min,bf->tm_sec);

    fprintf(data->ffd,"%s: %d.%d.%d%s %s: %d:%d:%d\n",
    gettext("Дата записи"),
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
    gettext("г."),
    gettext("Время"),
    bf->tm_hour,bf->tm_min,bf->tm_sec);

   }
  data->dbt+=debet;
  data->krt+=kredit;
  data->dbtden+=debet;
  data->krtden+=kredit;
  data->dbtmes+=debet;
  data->krtmes+=kredit;

 }

raspitog(data->dbt,data->krt,0,data->ff);
raspitog(data->dbtden,data->krtden,1,data->ffd);
raspitog(data->dbtmes,data->krtmes,2,data->ffd);
raspitog(data->dbt,data->krt,0,data->ffd);

iceb_podpis(data->ff,data->window);
iceb_podpis(data->ffd,data->window);

fclose(data->ff);
fclose(data->ffd);


/*******************************/
//Распечатка свода по счетам

fprintf(data->ffs,"\n");
fprintf(data->ffs,"\
----------------------------------------\n");
fprintf(data->ffs,gettext("\
  Счет  |Счет кор|  Дебет   |  Кредит  |\n"));
fprintf(data->ffs,"\
----------------------------------------\n");

int kolelem=data->SHET.kolih();
char shet[32];
char shetk[32];

for(nomer=0; nomer < kolelem; nomer++)
 {
  strcpy(strsql,data->SHET.ravno(nomer));
  iceb_u_pole(strsql,shet,1,'|');
  iceb_u_pole(strsql,shetk,2,'|');
  
  fprintf(data->ffs,"%-*s %-*s %10.2f %10.2f\n",
  iceb_u_kolbait(8,shet),shet,
  iceb_u_kolbait(8,shetk),shetk,
  data->DEB.ravno(nomer),
  data->KRE.ravno(nomer));
  
 }
fprintf(data->ffs,"\
----------------------------------------\n");
fprintf(data->ffs,"%*s %10.2f %10.2f\n",
iceb_u_kolbait(17,gettext("Итого")),gettext("Итого"),
data->dbt,data->krt);


iceb_podpis(data->ffs,data->window);
fclose(data->ffs);

for(int nom=0; nom < data->imaf.kolih(); nom++)
 iceb_ustpeh(data->imaf.ravno(nom),1,data->window);

gtk_widget_destroy(data->window);

return(FALSE);

}
/**************/
/*Выдача итога*/
/**************/
void raspitog(double dbt,double krt,int metka,FILE *ff1)
{
double	brr;

fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------------\n");

if(metka == 0)
  fprintf(ff1,"%*s:",iceb_u_kolbait(46,gettext("Итого")),gettext("Итого"));
if(metka == 1)
  fprintf(ff1,"%*s:",iceb_u_kolbait(46,gettext("Итого за день")),gettext("Итого за день"));
if(metka == 2)
  fprintf(ff1,"%*s:",iceb_u_kolbait(46,gettext("Итого за месяц")),gettext("Итого за месяц"));

fprintf(ff1,"%13s",iceb_u_prnbr(dbt));
fprintf(ff1," %10s\n",iceb_u_prnbr(krt));

if(dbt >= krt)
 {
  brr=dbt-krt;
  fprintf(ff1,"\
%50s%10s\n"," ",iceb_u_prnbr(brr));
 }
else
 {
  brr=krt-dbt;
  fprintf(ff1,"\
%50s%10s %10s\n"," "," ",iceb_u_prnbr(brr));
 }
fprintf(ff1,"\n");

}
/*******/
/*Шапка*/
/*******/
void	rapsap(FILE *ff1)
{

fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,gettext("  Дата    | Счет.|Сч.к-т|Конт.| Кто   |Номер док.|   Дебет  |  Кредит  |      К о м е н т а р и й       |Наименование контрагента|\n"));

fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------------\n");

}
/************************************/
/*Формирование комментария           */
/************************************/

void	sozkom(char *koment,
int razmk, //размер комментрария
char *komentprov, //Коментарий из проводки
char *kto, //Из какой подсистемы сделана проводка
char *kodop) //Код операции
{
char   strsql[512];
SQL_str row1;

memset(koment,'\0',razmk);
strncpy(koment,komentprov,razmk-1);
if(koment[0] != '\0')
  return;
  
if(koment[0] == '\0' && kto[0] == '\0' )
  return;
SQLCURSOR cur1;
memset(strsql,'\0',sizeof(strsql));
if(iceb_u_SRAV(kto,gettext("МУ"),0) == 0)
 {
  sprintf(strsql,"select naik from Prihod where kod='%s'",kodop);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    strncpy(koment,row1[0],razmk-1);
  else
   {
    sprintf(strsql,"select naik from Rashod where kod='%s'",kodop);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      strncpy(koment,row1[0],razmk-1);
   }
 }

if(iceb_u_SRAV(kto,gettext("ПЛТ"),0) == 0)
 {
  sprintf(strsql,"select naik from Opltp where kod='%s'",kodop);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    strncpy(koment,row1[0],razmk-1);
 }

if(iceb_u_SRAV(kto,gettext("КО"),0) == 0)
 {
  sprintf(strsql,"select naik from Кasop1 where kod='%s'",kodop);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    strncpy(koment,row1[0],razmk-1);
  else
   {
    sprintf(strsql,"select naik from Kasop2 where kod='%s'",kodop);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      strncpy(koment,row1[0],razmk-1);
   }
 }

if(iceb_u_SRAV(kto,gettext("УСЛ"),0) == 0)
 {
  sprintf(strsql,"select naik from Usloper1 where kod='%s'",kodop);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    strncpy(koment,row1[0],razmk-1);
  else
   {
    sprintf(strsql,"select naik from usloper2 where kod='%s'",kodop);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      strncpy(koment,row1[0],razmk-1);
   }
 }
   

}
