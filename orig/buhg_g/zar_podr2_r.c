/*$Id: zar_podr2_r.c,v 1.11 2011-02-21 07:35:59 sasa Exp $*/
/*20.11.2009	29.01.2007	Белых А.И.	zar_podr2_r.c
Расчёт свода по подразделениям
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_podr.h"

class zar_podr2_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_podr_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_podr2_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_podr2_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_podr2_r_data *data);
gint zar_podr2_r1(class zar_podr2_r_data *data);
void  zar_podr2_r_v_knopka(GtkWidget *widget,class zar_podr2_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

extern short	*obud; /*Обязательные удержания*/

int zar_podr2_r(class zar_podr_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_podr2_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать отчёты по подразделениям (форма 2)"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_podr2_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

repl.plus(gettext("Распечатать отчёты по подразделениям (форма 2)"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),imabaz,organ);
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno_toutf());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),450,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
//PangoFontDescription *font_pango=pango_font_description_from_string("Nimbus Mono L, Bold 12");
PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_modify_font(GTK_WIDGET(data.view),font_pango);
pango_font_description_free(font_pango);

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
//gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_ALWAYS,GTK_POLICY_ALWAYS);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


data.bar=gtk_progress_bar_new();

gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_CONTINUOUS);

gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_LEFT_TO_RIGHT);

gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_podr2_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_podr2_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_podr2_r_v_knopka(GtkWidget *widget,class zar_podr2_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_podr2_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_podr2_r_data *data)
{
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}

/******************************************/
/******************************************/

gint zar_podr2_r1(class zar_podr2_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short mn,gn;
short mk,gk;

iceb_u_rsdat1(&mn,&gn,data->rk->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->rk->datan.ravno());
if(mk == 0)
 {
  mk=mn;
  gk=gn;
 }
sprintf(strsql,"%s %d.%d %s %d.%d\n",gettext("Период с"),mn,gn,
gettext("по"),mk,gk);


short		kolnah;  /*Количество начислений*/
short		koluder;   /*Количество удержаний*/
double		inah;    /*Итого начислено*/
double		iuder;    /*Итого удержано с учетом сальдо*/
double		iuderb;    /*Итого удержано без учета сальдо*/
double		kvid;    /*К выдаче с учетом сальдо*/
double		kvidb;   /*К выдаче без учета сальдо*/
double		saldont;  /*Сальдо на начало периода по табельному номеру*/
double		inaht;    /*Итого начислено по табельному номеру*/
double		iudert;    /*Итого удержано по табельному номеру*/
double		iudertb;    /*Итого удержано по табельному номеру без учета сальдо*/
double		kvidt;    /*К выдаче по табельному номеру c учетом сальдо*/
double		kvidtb;    /*К выдаче по табельному номеру без учета сальдо*/
double		inahp;    /*Итого начисленное по подразделению*/
double		dolg,dolgb;
int		i,i1;
char		strok[1024];
char		shet[32];
SQL_str         row,row1;
short		dkm;
int		kolstr;
float kolstr1=0.;
int kolstr2;
long		tabn,tabn1;
int		kpodr; /*Подразделение в котором начислена сумма*/
int             podro; /*Основное подразделение*/
short		prn;
double		sum;
short		knah;
double		koff;
char		naim[512];
double		bb;
double		sumaub;
double		saldb;
short		metkaou; /*0-обязательное 1-не обяз. удержание*/
double sum1;
double bb1=0.;
int		klst=0;
SQLCURSOR curr;


/*Определяем количество начислений и удержаний*/

sprintf(strsql,"select kod from Nash");
class SQLCURSOR cur;
class SQLCURSOR cur1;
if((kolnah=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolnah == 0)
 {
  iceb_menu_soob(gettext("Не ввели начисления !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short nah[kolnah];

i=0;
while(cur.read_cursor(&row) != 0)
  nah[i++]=atoi(row[0]);

double snah[kolnah];
memset(snah,'\0',sizeof(snah));


sprintf(strsql,"select kod from Uder");
if((koluder=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(koluder == 0)
 {
  iceb_menu_soob(gettext("Не введены удержания !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short uder[koluder];
i=0;
while(cur.read_cursor(&row) != 0)
  uder[i++]=atoi(row[0]);

double suder[koluder];
double suderb[koluder];
memset(suder,'\0',sizeof(suder));
memset(suderb,'\0',sizeof(suderb));


/*Определяем начисления/счет удержания/счет */

sprintf(strsql,"select distinct prn,knah,shet from Zarp \
where datz >= '%d-%d-01' and datz <= '%d-%d-31' and suma <> 0.",
gn,mn,gn,mn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
//Создаем классы
class iceb_u_double NASH_SUM;
class iceb_u_double UDER_SUM;
class iceb_u_double UDER_SUMB;
UDER_SUM.make_class(kolstr);
UDER_SUMB.make_class(kolstr);
NASH_SUM.make_class(kolstr);

class iceb_u_spisok NASH_SH; //Начисления/счет
class iceb_u_spisok UDER_SH; //Удержания/счет

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s,%s",row[1],row[2]);
  if(row[0][0] == '1')
     NASH_SH.plus(strsql);
  if(row[0][0] == '2')
     UDER_SH.plus(strsql);
 }
//UDER_SH.print_masiv_char(strsql); 

/*******************************************************/
/*                 Выполняем расчет                    */
/*******************************************************/

sprintf(strsql,"select distinct podr,tabn from Zarp where \
datz >= '%d-%d-01' and datz <= '%d-%d-31' and suma <> 0.",
gn,mn,gn,mn);
//printw("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


char imaf[56];
char imafsh[50];
sprintf(imaf,"zpd%d.lst",getpid());
sprintf(imafsh,"zpds%d.lst",getpid());

FILE *ff;
FILE *ffsh;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if((ffsh = fopen(imafsh,"w")) == NULL)
 {
  iceb_er_op_fil(imafsh,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_dpm(&dkm,&mn,&gn,5);


iceb_u_zagolov(gettext("Свод начислений и удержаний по подразделениям"),1,mn,gn,dkm,mn,gn,organ,ff);

if(data->rk->podr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код подразделения"),data->rk->podr.ravno());
  fprintf(ffsh,"%s:%s\n",gettext("Код подразделеня"),data->rk->podr.ravno());
  iceb_printcod(ff,"Podr","kod","naik",0,data->rk->podr.ravno(),&klst);
  iceb_printcod(ffsh,"Podr","kod","naik",0,data->rk->podr.ravno(),&klst);
 }
else
 {
  fprintf(ff,"%s\n",gettext("По всем подразделениям"));
  fprintf(ffsh,"%s\n",gettext("По всем подразделениям"));
 }
if(data->rk->kod_kat.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код категории"),data->rk->kod_kat.ravno());
  fprintf(ffsh,"%s:%s\n",gettext("Код категории"),data->rk->kod_kat.ravno());
 }

if(data->rk->tabnom.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Табельный номер"),data->rk->tabnom.ravno());
  fprintf(ffsh,"%s:%s\n",gettext("Табельный номер"),data->rk->tabnom.ravno());
 }

if(data->rk->kod_nah.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код начисления"),data->rk->kod_nah.ravno());
  fprintf(ffsh,"%s:%s\n",gettext("Код начисления"),data->rk->kod_nah.ravno());
 }
if(data->rk->kod_ud.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код начисления"),data->rk->kod_ud.ravno());
  fprintf(ffsh,"%s:%s\n",gettext("Код начисления"),data->rk->kod_ud.ravno());
 }

dolg=dolgb=inah=iuder=iuderb=kvid=kvidb=0.;
inahp=saldont=inaht=iudert=iudertb=kvidt=kvidtb=0.;
tabn1=tabn=0;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
//  strzag(LINES-1,0,kolstr,++kolstr1);

  if(iceb_u_proverka(data->rk->podr.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[1],0,0) != 0)
    continue;
//  printw("%-4s %s\n",row[1],row[0]);

  kpodr=atoi(row[0]);
  tabn=atol(row[1]);

  /*Узнаем какая категория и основное подразделение*/
  sprintf(strsql,"select kateg,podr from Zarn where god=%d and mes=%d \
and tabn=%ld",gn,mn,tabn);

  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
   {
    sprintf(strsql,"Не найдена запись настройки за %d.%d для %ld !",mn,gn,tabn);
    iceb_menu_soob(strsql,data->window);
    continue;
   }

  podro=atoi(row1[1]);
  if(kpodr == 0)
   kpodr=podro;

  if(iceb_u_proverka(data->rk->kod_kat.ravno(),row1[0],0,0) != 0)
    continue;
   


  /*Определяем сальдо на начало периода*/
  saldont=zarsaldw(1,mn,gn,tabn,&saldb,data->window);

  sprintf(strsql,"select prn,knah,suma,podr,godn,mesn,datz,shet \
from Zarp where datz >= '%d-%d-01' and datz <= '%d-%d-31' and \
tabn=%ld and suma <> 0.",gn,mn,gn,mn,tabn);
//  printw("%s\n",strsql);
  
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  if(kolstr2 == 0)
    continue;

  /*Определяем коэффициент начисления*/
  inahp=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
//    printw("%s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
    
    prn=atoi(row1[0]);
    sum=atof(row1[2]);
    
    if(prn == 1)
     {
      if(iceb_u_proverka(data->rk->kod_nah.ravno(),row1[1],0,0) != 0)
       continue;
      inaht+=sum;
      if(kpodr == atoi(row1[3]))
       inahp+=sum;

     }
   }  
  koff=0.;
  if(inahp != 0.)
    koff=inaht/inahp;
/*
  printw("%ld koff=%.2f/%.2f=%f\n",tabn,inaht,inahp,koff);
  refresh();
*/
  cur1.poz_cursor(0);
  while(cur1.read_cursor(&row1) != 0)
   {
    prn=atoi(row1[0]);
    knah=atoi(row1[1]);
    sum=atof(row1[2]);
    
    if(prn == 1)
     {
      if(iceb_u_proverka(data->rk->kod_nah.ravno(),row1[1],0,0) != 0)
       continue;

      if(kpodr != atoi(row1[3]))
       continue;
        
      for(i=0; i < kolnah ; i++)
       if(knah == nah[i])
        {
         snah[i]+=sum;
         break;
        }
      sprintf(strsql,"%s,%s",row1[1],row1[7]);
      if((i1=NASH_SH.find(strsql)) >= 0)
        NASH_SUM.plus(sum,i1);
     }

    if(prn == 2)
     {
      if(iceb_u_proverka(data->rk->kod_ud.ravno(),row[1],0,0) != 0)
       continue;
      /*Усли удержание не относится к обязательным удержаниям то
       проверяем код подразделения
       */
      metkaou=0;      
      if(obud != NULL)
       {
        for(i1=1; i1<= obud[0]; i1++)
         if(obud[i1] == knah)
          {
           break;
          }
         if(i1 > obud[0]) /*Код не обязательное удержание*/
          {
           if(kpodr != atoi(row1[3]))
            continue;
           metkaou=1;
          }         
        }
       
      sprintf(strsql,"%s,%s",row1[1],row1[7]);
      int i22;
      i22=UDER_SH.find(strsql);

      iudert+=sum;
      for(i=0; i < koluder ; i++)
       if(knah == uder[i])
        {
         if(metkaou == 0)
          {
           if(koff != 0.)
            {
              suder[i]+=sum/koff;
              UDER_SUM.plus(sum/koff,i22);
            }
           else
            {
             class iceb_u_spisok repl;
             sprintf(strsql,"%s %ld\n",
             gettext("Табельный номер"),tabn);
             
             repl.plus(strsql);
             
             sprintf(strsql,"%s %d !\n (%d/%.2f/%.2f)\n",
             "Нет ни одного не обязательного удержания для подразделения",
             kpodr,knah,inaht,inahp);

             repl.plus(strsql);
             iceb_menu_soob(&repl,data->window);
            }
          }
         else
          {
           suder[i]+=sum;
           UDER_SUM.plus(sum,i22);
          }
         if(atoi(row1[4]) == gn && atoi(row1[5]) == mn)
          {
           iudertb+=sum;
           if(metkaou == 0 && koff != 0.)
            {
             suderb[i]+=sum/koff;
             UDER_SUMB.plus(sum/koff,i22);
            }
           else
            {
             suderb[i]+=sum;
             UDER_SUMB.plus(sum,i22);
            }
          }
         else
          {
           if(obud != NULL)
            for(i1=1; i1<= obud[0]; i1++)
             if(obud[i1] == knah)
              {
               iudertb+=sum;
               if(metkaou == 0 && koff != 0.)
                {
                 UDER_SUMB.plus(sum/koff,i22);
                 suderb[i]+=sum/koff;
                }
               else
                {
                 suderb[i]+=sum;
                 UDER_SUMB.plus(sum,i22);
                }
               break;

              }
          }

         break;        
        }
      }    
   }

  
  if(koff != 0.)
   {
    kvidt=(saldont+inaht+iudert)/koff;
    bb=prbperw(mn,gn,bf->tm_mon+1,bf->tm_year+1900,tabn,&sumaub,0,data->window);
    kvidtb=(inaht+iudertb+bb)/koff;
   }

  if(kvidt > 0.)
    kvid+=kvidt;
  else
   dolg+=kvidt;

  if(kvidtb > 0.)
    kvidb+=kvidtb;
  else
   dolgb+=kvidtb;

  inahp=saldont=inaht=iudert=iudertb=kvidt=kvidtb=0.;


 }


/******************************************/
/*       Распечатываем                    */
/******************************************/
fprintf(ff,"\
----------------------------------------------\n");
fprintf(ff,gettext("Код|  Наименование начисления     |   Сумма  |\n"));
fprintf(ff,"\
----------------------------------------------\n");
inah=0.;
for(i=0 ; i < kolnah; i++)
 {
  if(snah[i] == 0.)
   continue;
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naik from Nash where kod=%d",nah[i]);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код начисления"),nah[i]);
    iceb_menu_soob(strsql,data->window);    
   }
  else
   strncpy(naim,row1[0],sizeof(naim)-1);
  fprintf(ff,"%3d %-*.*s %10.2f\n",nah[i],iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,snah[i]);
  inah+=snah[i];
 }


fprintf(ff,"\
----------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(34,gettext("Итого")),gettext("Итого"),inah);


fprintf(ff,"\
---------------------------------------------------------\n");
fprintf(ff,gettext("Код|  Наименование удержания      |   Сумма  |Сумма б.с.|\n"));
fprintf(ff,"\
---------------------------------------------------------\n");
iuderb=iuder=0.;

for(i=0 ; i < koluder; i++)
 {

  if(suder[i] == 0.)
   continue;

  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naik from Uder where kod=%d",uder[i]);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),uder[i]);
    iceb_menu_soob(strsql,data->window);    
   }
  else
   strncpy(naim,row1[0],sizeof(naim)-1);

  fprintf(ff,"%3d %-*.*s %10.2f %10.2f\n",uder[i],iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,suder[i],suderb[i]);
  iuder+=suder[i];
  iuderb+=suderb[i];

 }

fprintf(ff,"\
---------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f\n",iceb_u_kolbait(34,gettext("Итого")),gettext("Итого"),iuder,iuderb);

fprintf(ff,"\n%*s:%10.2f\n",iceb_u_kolbait(34,gettext("К выдаче с учетом сальдо")),gettext("К выдаче с учетом сальдо"),kvid);
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(34,gettext("К выдаче без учета сальдо")),gettext("К выдаче без учета сальдо"),inah+iuderb);
/*
fprintf(ff,"%34s:%10.2f\n",gettext("Долги с учетом сальдо"),dolg);
fprintf(ff,"%34s:%10.2f\n",gettext("Долги без учета сальдо"),dolgb);
*/
sprintf(strsql,"\
%-*s:%10.2f\n\
%-*s:%10.2f\n\
%-*s:%10.2f\n\
%-*s:%10.2f\n\
%-*s:%10.2f\n\
%-*s:%10.2f\n",
iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),inah,
iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),iuder,
iceb_u_kolbait(20,gettext("Итого долгов")),gettext("Итого долгов"),dolg,
iceb_u_kolbait(20,gettext("К выдаче")),gettext("К выдаче"),kvid,
iceb_u_kolbait(20,gettext("К выдаче без учета сальдо")),gettext("К выдаче без учета сальдо"),inah+iuderb,
iceb_u_kolbait(20,gettext("Долги без учета сальдо")),gettext("Долги без учета сальдо"),dolgb);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

iceb_u_zagolov(gettext("Свод начислений и удержаний по подразделениям"),1,mn,gn,dkm,mn,gn,organ,ffsh);

/*Распечатка начилений/счет*/
fprintf(ffsh,"\n%s:\n",gettext("Начисления"));
fprintf(ffsh,"\
----------------------------------------------------\n");
fprintf(ffsh,gettext("\
Код|  Наименование начисления     |Счёт |   Сумма  |\n"));
fprintf(ffsh,"\
----------------------------------------------------\n");

bb=0.;
for(i=0; i < NASH_SH.kolih();i++)
 {
  if((sum=NASH_SUM.ravno(i)) == 0.)
   continue;
  
  iceb_u_pole(NASH_SH.ravno(i),strok,1,',');
  iceb_u_pole(NASH_SH.ravno(i),shet,2,',');
  sprintf(strsql,"select naik from Nash where kod=%s",strok);
  memset(naim,'\0',sizeof(naim));
     
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код начисления"),strok);
    iceb_menu_soob(strsql,data->window);
   }
  else
    strncpy(naim,row1[0],sizeof(naim)-1);
  fprintf(ffsh,"%3s %-*s %5s %10.2f\n",strok,iceb_u_kolbait(30,naim),naim,shet,sum);
  bb+=sum;
 }

fprintf(ffsh,"\
----------------------------------------------------\n");

fprintf(ffsh,"%3s %*s: %10.2f\n%s:\n"," ",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"),
bb,gettext("Удержания"));

fprintf(ffsh,"\
---------------------------------------------------------------\n");
fprintf(ffsh,gettext("\
Код|  Наименование удержания      |Счёт |   Сумма  |Сумма б.с.|\n"));
fprintf(ffsh,"\
---------------------------------------------------------------\n");

i=1;
bb1=bb=0.;

//while(UDER_SH.pol_masiv_char(i++,bros) == 0)
for(i=0; i < UDER_SH.kolih() ; i++)
 {
  sum=UDER_SUM.ravno(i);
  sum1=UDER_SUMB.ravno(i);
  if(sum == 0. && sum1 == 0.)
    continue;
       
//  printw("bros=%s\n",bros);
  
  iceb_u_pole(UDER_SH.ravno(i),strok,1,',');
  iceb_u_pole(UDER_SH.ravno(i),shet,2,',');
  sprintf(strsql,"select naik from Uder where kod=%s",strok);
  memset(naim,'\0',sizeof(naim));
     
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код удержания"),strok);
    iceb_menu_soob(strsql,data->window);
   }
  else
    strncpy(naim,row1[0],sizeof(naim)-1);
  fprintf(ffsh,"%3s %-*s %5s %10.2f %10.2f\n",strok,iceb_u_kolbait(30,naim),naim,shet,sum,sum1);
  bb+=sum;
  bb1+=sum1;
 }

fprintf(ffsh,"\
---------------------------------------------------------------\n");

fprintf(ffsh,"%3s %*s: %10.2f %10.2f\n"," ",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"),bb,bb1);

fprintf(ffsh,"\n%*s:%10.2f\n",iceb_u_kolbait(34,gettext("К выдаче с учетом сальдо")),gettext("К выдаче с учетом сальдо"),kvid);
fprintf(ffsh,"%*s:%10.2f\n",iceb_u_kolbait(34,gettext("К выдаче без учета сальдо")),gettext("К выдаче без учета сальдо"),inah+iuderb);

iceb_podpis(ff,data->window);
iceb_podpis(ffsh,data->window);
fclose(ff);
fclose(ffsh);


data->rk->imaf.plus(imaf);
data->rk->imaf.plus(imafsh);
data->rk->naimf.plus(gettext("Свод начислений и удержаний по подразделениям"));
data->rk->naimf.plus(gettext("Свод начислений и удержаний по подразделениям и счетам"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),0,data->window);




data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
