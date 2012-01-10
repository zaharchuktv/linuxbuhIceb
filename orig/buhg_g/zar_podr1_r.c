/*$Id: zar_podr1_r.c,v 1.13 2011-02-21 07:35:59 sasa Exp $*/
/*20.11.2009	29.01.2007	Белых А.И.	zar_podr1_r.c
Расчёт свода по подразделениям
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_podr.h"

class zar_podr1_r_data
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

  zar_podr1_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_podr1_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_podr1_r_data *data);
gint zar_podr1_r1(class zar_podr1_r_data *data);
void  zar_podr1_r_v_knopka(GtkWidget *widget,class zar_podr1_r_data *data);

void rasmaspodr(FILE *kaw,double *naon,short *nan,short kkan,short konn,short metka,double *sn,short *ka,short *koll,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;


int zar_podr1_r(class zar_podr_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_podr1_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать отчёты по подразделениям (форма 1)"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_podr1_r_key_press),&data);

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

repl.plus(gettext("Распечатать отчёты по подразделениям (форма 1)"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_podr1_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_podr1_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_podr1_r_v_knopka(GtkWidget *widget,class zar_podr1_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_podr1_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_podr1_r_data *data)
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

gint zar_podr1_r1(class zar_podr1_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short		kolnah=0;
short		kolud=0;
short		kolpodr=0;
SQL_str		row;
int		i=0;
SQLCURSOR 	curr;
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

/*Определяем количество начислений*/

sprintf(strsql,"select kod from Nash");
class SQLCURSOR cur;
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

short nahisl[kolnah];
i=0;
while(cur.read_cursor(&row) != 0)
  nahisl[i++]=atoi(row[0]);

//Определяем количество удержаний

sprintf(strsql,"select kod from Uder");
if((kolud=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolud == 0)
 {
  iceb_menu_soob(gettext("Не введены удержания !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

short uder[kolud];
i=0;
while(cur.read_cursor(&row) != 0)
  uder[i++]=atoi(row[0]);

/*Определяем количество подразделений*/

sprintf(strsql,"select kod from Podr");
if((kolpodr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolpodr == 0)
 {
  iceb_menu_soob(gettext("Не ввели подразделения !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

short kolludpodr[kolpodr]; //Количество людей в подразделении
memset(&kolludpodr,'\0',sizeof(kolludpodr));

short podraz[kolpodr];
i=0;
while(cur.read_cursor(&row) != 0)
  podraz[i++]=atoi(row[0]);

double masivsum[kolpodr*kolnah];
memset(&masivsum,'\0',sizeof(masivsum));

double masivsumud[kolpodr*kolud];
memset(&masivsumud,'\0',sizeof(masivsumud));

short d=1;
short m=mn;
short g=gn;
int kolstr=0;
float kolstr1=0;
int kolstr2=0;
int nompodr=0;
int nomnah=0;
int kpodr=0;
int knah=0;
SQL_str row1;
double suma=0.;
short prn=0;
int tabn;
int nomer;
double sal=0.;
double saldb=0.;
double  suman=0.;
double  sumau=0.;
double	saldo=0.;
double	dolgi=0.;

class iceb_u_int TABNOM; //Массив табельных номеров
class iceb_u_int KODPODR; //Массив подразделений в которых находятся табельные номера
class iceb_u_double SALDO; //Массив сальдо по табельным номерам


while(iceb_u_sravmydat(d,m,g,1,mk,gk) <= 0)
 {
  
  sprintf(strsql,"%s %d.%d%s\n",gettext("Дата"),m,g,gettext("г."));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  
  sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d",g,m);
//  printw("%s\n",strsql);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  if(kolstr == 0)
    continue;

  kolstr1=0;
  while(cur.read_cursor(&row) != 0)
   {
    
    iceb_pbar(data->bar,kolstr,++kolstr1);    

    if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
      continue;
    if(iceb_u_proverka(data->rk->podr.ravno(),row[1],0,0) != 0)
      continue;
    if(iceb_u_proverka(data->rk->kod_kat.ravno(),row[2],0,0) != 0)
      continue;

    tabn=atoi(row[0]);

    if((nomer=TABNOM.find(tabn)) < 0);
      TABNOM.plus(tabn,nomer);

    kpodr=atoi(row[1]);

    if(nomer < 0)
     {
      KODPODR.plus(kpodr,nomer);
      sal=zarsaldw(1,mn,gn,tabn,&saldb,data->window);
      SALDO.plus(sal,nomer);
      saldo+=sal;
     }

 
    //Определям номер по порядку подразделения
    
    for(nompodr=0 ; nompodr < kolpodr ; nompodr++)
     if(podraz[nompodr] == kpodr)
	break;
    
    /*Если нет подразделения в списке читаем дальше*/
    if(nompodr == kolpodr)
     {
      if(kpodr == 0)
       {
        sprintf(strsql,"%s %s %s !\n",gettext("Табельный номер"),
        row[0],"В карточку не введён код подразделения");
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
        iceb_menu_soob(strsql,data->window);
       }
      else
       {
        sprintf(strsql,"%s %s\n",gettext("Табельный номер"),row[0]);
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        sprintf(strsql,"%s %d !\n",gettext("Не найден код подразделения"),kpodr);
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        iceb_menu_soob(strsql,data->window);
       }

      continue;
     }

    
    sprintf(strsql,"select prn,knah,suma,shet from Zarp where \
datz >= '%d-%d-01' and datz <= '%d-%d-31' and tabn=%s and suma <> 0.",
    g,m,g,m,row[0]);

//    printw("%s\n",strsql);
    SQLCURSOR cur1;
    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
      continue;
     }
    if(nomer < 0) //Если только что добавили то номер меньше ноля
      nomer=TABNOM.find(tabn);

    if(kolstr2 > 0)
       kolludpodr[nompodr]++;

    while(cur1.read_cursor(&row1) != 0)
     {
//      printw("%s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
      if(row[0][0] == '1')
       if(iceb_u_proverka(data->rk->kod_nah.ravno(),row1[1],0,0) != 0)
         continue;
      if(row[0][0] == '2')
       if(iceb_u_proverka(data->rk->kod_ud.ravno(),row1[1],0,0) != 0)
         continue;
      if(iceb_u_proverka(data->rk->shet.ravno(),row1[3],0,0) != 0)
         continue;

      prn=atoi(row1[0]);
      knah=atoi(row1[1]);
      suma=atof(row1[2]);

      SALDO.plus(suma,nomer);

      if(prn == 1)      
       {
        for(nomnah=0 ; nomnah < kolnah ; nomnah++)
         if(nahisl[nomnah] == knah)
          break;
        masivsum[kolpodr*nomnah+nompodr]+=suma;
       }

      if(prn == 2)      
       {
        for(nomnah=0 ; nomnah < kolud ; nomnah++)
         if(uder[nomnah] == knah)
          break;

        masivsumud[kolpodr*nomnah+nompodr]+=suma;
       }
     }
//    OSTANOV();
   }
  iceb_u_dpm(&d,&m,&g,3); //Увеличиваем на месяц
 }



/*********************************************/
char  imaf[30];
FILE  *ff;

d=1;
iceb_u_dpm(&d,&mk,&gk,5);

//Распечатываем свод начислений по подразделениям
sprintf(imaf,"npd%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_u_zagolov(gettext("Свод начислений по подразделениям"),1,mn,gn,d,mk,gk,organ,ff);

suman=0.;

rasmaspodr(ff,masivsum,nahisl,kolpodr,kolnah,1,&suman,podraz,kolludpodr,data->window);

iceb_podpis(ff,data->window);
fclose(ff);

/****************************************/

//Распечатываем свод удержаний по подразделениям
char  imafud[30];
sprintf(imafud,"upd%d.lst",getpid());

if((ff = fopen(imafud,"w")) == NULL)
 {
  iceb_er_op_fil(imafud,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_zagolov(gettext("Свод удержаний по подразделениям"),1,mn,gn,d,mk,gk,organ,ff);

sumau=0.;
rasmaspodr(ff,masivsumud,uder,kolpodr,kolud,2,&sumau,podraz,kolludpodr,data->window);

iceb_podpis(ff,data->window);
fclose(ff);

/***********************************/

//Распечатываем свод удержаний по подразделениям
char  imafnahud[30];
sprintf(imafnahud,"nupd%d.lst",getpid());

if((ff = fopen(imafnahud,"w")) == NULL)
 {
  iceb_er_op_fil(imafnahud,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_u_zagolov(gettext("Свод начислений и удержаний по подразделениям"),1,mn,gn,d,mk,gk,organ,ff);

suman=sumau=0.;

rasmaspodr(ff,masivsum,nahisl,kolpodr,kolnah,1,&suman,podraz,kolludpodr,data->window);
rasmaspodr(ff,masivsumud,uder,kolpodr,kolud,2,&sumau,podraz,kolludpodr,data->window);
//Распечатываем сумму к выдаче по подразделениям
double sumakvd=0.;
double sumakvdi=0.;
nomer=TABNOM.kolih();
char naim[512];

for(i=0; i < kolpodr; i++)
 {
  sumakvd=0.;
  dolgi=0.;
  for(int nomertab=0; nomertab < nomer; nomertab++)
   {
    if(podraz[i] != KODPODR.ravno(nomertab))
     continue;
    sal=SALDO.ravno(nomertab);
    if(sal < 0.)
     dolgi+=sal;
    else
     sumakvd+=sal;
   }
  if(sumakvd == 0.)
    continue;
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naik from Podr where kod=%d",podraz[i]);
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);
  fprintf(ff,"%2d %-*.*s %9.2f\n",podraz[i],iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,sumakvd);
  sumakvdi+=sumakvd;
 }
fprintf(ff,"%*s:%9.2f\n\n",iceb_u_kolbait(33,gettext("Итого")),gettext("Итого"),sumakvdi);

dolgi=0.;
//Определяем сумму долгов
nomer=TABNOM.kolih();
for(i=0; i < nomer; i++)
 {
  sal=SALDO.ravno(i);
  if(sal < 0.)
   dolgi+=sal;
 }


sprintf(strsql,"\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n",
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),saldo,
iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),suman,
iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),sumau,
iceb_u_kolbait(20,gettext("Итого долгов")),gettext("Итого долгов"),dolgi,
iceb_u_kolbait(20,gettext("К выдаче")),gettext("К выдаче"),suman+sumau-dolgi+saldo,
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),suman+sumau+saldo);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(ff,"\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n",
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),saldo,
iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),suman,
iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),sumau,
iceb_u_kolbait(20,gettext("Итого долгов")),gettext("Итого долгов"),dolgi,
iceb_u_kolbait(20,gettext("К выдаче")),gettext("К выдаче"),suman+sumau-dolgi+saldo,
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),suman+sumau+saldo);


iceb_podpis(ff,data->window);
fclose(ff);


data->rk->imaf.plus(imafnahud);
data->rk->imaf.plus(imaf);
data->rk->imaf.plus(imafud);

data->rk->naimf.plus(gettext("Свод начислений и удержаний по подразделениям"));
data->rk->naimf.plus(gettext("Свод начислений по подразделениям"));
data->rk->naimf.plus(gettext("Свод удержаний по подразделениям"));

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
/***************/
/*Шапка таблицы*/
/***************/
void sappkpodr(short kk, //1-начисления  2-удержания
short kka,  //Количество категорий
char st[],
short pn,  //Позиция начала
short pk,  //Позиция конца
short konu, //Количество начислений или удержаний
FILE *kaw,double *nao,short *ka,
int kolznak,
GtkWidget *wpredok)
{
int             i,i1,i2;
char            nai[512];
double          br2;
SQL_str         row;
char		strsql[512];
char		polkah[30];
SQLCURSOR curr;
memset(polkah,'\0',sizeof(polkah));

memset(polkah,'-',kolznak+1);


/*Формируем строку подчеркивания*/
/*Полка над наименованием*/
strcpy(st,"-----------------------------------");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
     br2+=nao[i1];
   }
  if(br2 != 0. )
    strcat(st,polkah);
//    strcat(st,"-------------");
  else
   i2++;
 }
/*Полка над ИТОГО*/
strcat(st,polkah);
//strcat(st,"-------------");

fprintf(kaw,"\n%s\n",st);

if(kk == 1)
 fprintf(kaw,"%*.*s|%-*s",
 iceb_u_kolbait(3,gettext("Код")),
 iceb_u_kolbait(3,gettext("Код")),
 gettext("Код"),
 iceb_u_kolbait(30,gettext("Начисления")),gettext("Начисления"));

if(kk == 2)
  fprintf(kaw,"%*.*s|%-*s",
  iceb_u_kolbait(3,gettext("Код")),
  iceb_u_kolbait(3,gettext("Код")),
  gettext("Код"),
  iceb_u_kolbait(30,gettext("Удержания")),gettext("Удержания"));

i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
    br2+=nao[i1];
   }
  if(br2 == 0.)
   {
    i2++;
    continue;
   }
  memset(nai,'\0',sizeof(nai));
  sprintf(strsql,"select naik from Podr where kod=%d",ka[i]);
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
   {
    sprintf(strsql,"Не найден код подразделения %d",ka[i]);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   strncpy(nai,row[0],sizeof(nai)-1);

  fprintf(kaw,"| %-*.*s",kolznak-1,kolznak-1,nai);
 }

fprintf(kaw,"| %-*.*s|\n",kolznak-1,kolznak-1,gettext(" Итого"));

fprintf(kaw,"   |%-30s"," ");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
    br2+=nao[i1];

  if(br2 != 0.)
    fprintf(kaw,"| %-*d",kolznak-1,ka[i]);
  else
   i2++;
 }
fprintf(kaw,"| %-*s|\n",kolznak-1," ");

fprintf(kaw,"%s\n",st);

}

/******************************/
/*Программа распечатки массива*/
/******************************/

void rasmaspodr(FILE *kaw,
double *naon, //Массив параметров*/
short *nan,  //Массив с перечнем параметров
short kkan,  //Количество категорий
short konn,  //Количество параметров
short metka, //1-начисления 2-удержания
double *sn, //Сумма по массиву
short *ka, //Массив со списком подразделений
short *koll, //Массив количества людей по подразделениям
GtkWidget *wpredok)
{
int		ii,i1,i2,i,skn,korr;
short           kkol; /*Количество колонок в распечатке*/
double		br1,br2;
int		khel;
char		st[300];
SQL_str         row;
char		nai[80];
char		strsql[512];
SQLCURSOR curr;
/*
При ужатом режиме 234 знака при 10 знаках на дюйм
                  275 знаков при 12 знаках на дюйм
(275-35)/13=18.46
*/

#define		KOLZNAK 9 //Количество знаков в колонке вместе с дробной частью

//kkol=17;
kkol=(275-35)/(KOLZNAK+1)-1;
korr=0;
*sn=0;
for(ii=0 ; ii < kkan; ii += kkol)
 {
//printw("ii-%d kkan-%d korr-%d\n",ii,kkan,korr);
  /*Проверяем если все равно нулю то печатаем следующюю группу столбиков*/
  i2=0; br2=0;
  for(i=ii; i<ii+kkol+i2 && i < kkan;i++)
   {
    br1=0.;
    for(skn=i;skn< kkan*(konn-1)+ii+kkol && skn<kkan*konn; skn+=kkan)
      br1+=naon[skn];
    if(br1 == 0.)
     i2++;
    br2+=br1;
   }
  if(br2 == 0.)
    continue;
  memset(st,'\0',sizeof(st));

  sappkpodr(metka,kkan,st,ii,(short)(ii+kkol),konn,kaw,naon,ka,KOLZNAK,wpredok);

  br1=0.;
  for(i=0 ; i<konn;i++)
   {
    /*Проверяем если по горизонтали сумма нулевая то строчку не печатаем*/
/*  printw("\ni-%d kkan-%d\n",i,kkan);*/
    br1=0.;
     for(skn=0;skn< kkan ; skn++)
      {
/*     printw("skn-%d naon[%d]=%.2f\n",skn,skn+i*kkan,naon[skn+i*kkan]);*/
       br1+=naon[skn+i*kkan];
      }
/*  printw("\n\n");
    OSTANOV();   */
    if(br1 == 0.)
      continue;
    memset(nai,'\0',sizeof(nai));
    if(metka == 1)
      sprintf(strsql,"select naik from Nash where kod=%d",nan[i]);
    if(metka == 2)
      sprintf(strsql,"select naik from Uder where kod=%d",nan[i]);
    if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
     {
      if(metka == 1) 
       sprintf(strsql,"%s %d !",gettext("Не найден код начисления"),nan[i]);
      if(metka == 2) 
        sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),nan[i]);
      iceb_menu_soob(strsql,wpredok);
     }
    else
      strncpy(nai,row[0],sizeof(nai)-1);

    fprintf(kaw,"%3d %-*.*s",nan[i],iceb_u_kolbait(30,nai),iceb_u_kolbait(30,nai),nai);

    /*Предыдущая информация*/
    br1=0.;
    if(ii > 0 )
     {
      for(skn=i*kkan; skn<i*kkan+ii && skn < kkan*konn ;skn++)
       {
	/*printw(" skn=%d",skn);*/
	 br1+=naon[skn];
       }
      /*printw("br1=%.2f\n",br1);
      refresh();*/
     }
    i2=0;
    for(skn=i*kkan+ii; skn<i*kkan+ii+kkol+i2 && skn < i*kkan+kkan ;skn++)
     {

      /*Если колонка нулевая не печатать*/
      br2=0.;
      for(i1=skn-(i*kkan); i1< kkan*konn ; i1+=kkan)
	 br2+=naon[i1];

      if(br2 != 0.)
       {
	/*printw("skn-%d (%d) br2=%.2f %d  \n",skn,i*kkan+ii+kkk+i2,br2,kkan*konn);*/
	fprintf(kaw," %*.2f",KOLZNAK,naon[skn]);
       }
      else
       {
	i2++;
       }
      br1+=naon[skn];
     }
    fprintf(kaw," %*.2f\n",KOLZNAK,br1);
    /*printw("%3d %-30s %.2f\n",zr2.tn,nai,br1);*/
   }
  fprintf(kaw,"%s\n",st);

  fprintf(kaw,"%-*s",iceb_u_kolbait(34,gettext(" И т о г о")),gettext(" И т о г о"));

  i2=0;
  for(i=ii; i<ii+kkol+i2 && i < kkan ;i++)
   {
    br1=0.;
    for(skn=i;skn < kkan*(konn-1)+ii+kkol+i2 && skn< kkan*konn ; skn+=kkan)
      br1+=naon[skn];

    if(br1 != 0.)
       fprintf(kaw," %*.2f",KOLZNAK,br1);
    else
      i2++;
    *sn=*sn+br1;
   }
  fprintf(kaw," %*.2f\n",KOLZNAK,*sn);

  fprintf(kaw,"%s\n",st);

  /*Распечатываем количество человек*/
  fprintf(kaw,"%-*s",iceb_u_kolbait(34,gettext(" Количество работающих ")),gettext(" Количество работающих "));
  /*Предыдущая информация по людям*/
  khel=0;

  for(i=0; i<ii && i<kkan;i++)
    khel+=koll[i];

  i2=0;
  for(i=ii; i<ii+kkol+i2 && i<kkan;i++)
   {
      /*Если колонка нулевая не печатать*/
      br2=0.;
      for(i1=i; i1< kkan*konn ; i1+=kkan)
	 br2+=naon[i1];

      if(br2 != 0.)
       {
	fprintf(kaw," %*d",KOLZNAK,koll[i]);
	khel+=koll[i];
       }
      else
	i2++;
   }
  fprintf(kaw," %*d\n\n",KOLZNAK,khel);
  if(i == kkan)
   break;
  /*ii+=korr*kkol+i3;*/
  ii+=i2;
 }

}
