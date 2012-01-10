/*$Id: zar_dolgor_r.c,v 1.15 2011-04-14 16:09:36 sasa Exp $*/
/*19.11.2009	04.12.2006	Белых А.И.	zar_dolgor_r.c
Расчёт долгов предприятия перед работниками
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_dolgor.h"
#include "zarp1.h"

class zar_dolgor_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_dolgor_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_dolgor_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_dolgor_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_dolgor_r_data *data);
gint zar_dolgor_r1(class zar_dolgor_r_data *data);
void  zar_dolgor_r_v_knopka(GtkWidget *widget,class zar_dolgor_r_data *data);

double prbper1w(short mz,short gz,short dk,short mk,short gk,int tabb,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern short	*obud; /*Обязательные удержания*/
extern class iceb_u_str kodpn_all;
extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/

extern int kodf_esv; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_bol; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_inv; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_dog; /*Код фонда удержания единого социального взноса*/

int zar_dolgor_r(class zar_dolgor_rek *datark,GtkWidget *wpredok)
{
char strsql[524];
iceb_u_str repl;
class zar_dolgor_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать свод начислений и удержаний по категориям"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_dolgor_r_key_press),&data);

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

repl.plus(gettext("Распечатать свод начислений и удержаний по категориям"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_dolgor_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_dolgor_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_dolgor_r_v_knopka(GtkWidget *widget,class zar_dolgor_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_dolgor_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_dolgor_r_data *data)
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
/****************************************/
/*Шапка файла*/
/******************************************/
void rasdolgi2_s(const char *data_nah,
short dd,short md,short gd,
const char *tabnom,
const char *podr,
FILE *ff)
{

struct  tm      *bf;
time_t tmm;
time(&tmm);
bf=localtime(&tmm);

fprintf(ff,"%s\n\
%s %s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %02d:%02d\n",
organ,
gettext("Долг по зарплате за"),
data_nah,
gettext("на"),
dd,md,gd,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(tabnom[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),tabnom);

if(podr[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Подразделение"),podr);
 
}

/******************************************/
/******************************************/

gint zar_dolgor_r1(class zar_dolgor_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);

short mes_nah,god_nah;
short dd,md,gd;

if(iceb_u_rsdat1(&mes_nah,&god_nah,data->rk->datan.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Не верно введена дата начисления !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(iceb_u_rsdat(&dd,&md,&gd,data->rk->datad.ravno(),0) != 0)
 {
  iceb_menu_soob(gettext("Не верно введена дата долга !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf[64];
sprintf(imaf,"golg2%d.lst",getpid());
char imaf_soc[64];
sprintf(imaf_soc,"golg2s%d.lst",getpid());
char imaf_prot[64];
sprintf(imaf_prot,"golgp2%d.lst",getpid());

FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

FILE *ff_prot;
if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_prot,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

FILE *ff_soc;
if((ff_soc = fopen(imaf_soc,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_soc,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

rasdolgi2_s(data->rk->datan.ravno(),dd,md,gd,data->rk->tabnom.ravno(),data->rk->podr.ravno(),ff);
rasdolgi2_s(data->rk->datan.ravno(),dd,md,gd,data->rk->tabnom.ravno(),data->rk->podr.ravno(),ff_soc);
rasdolgi2_s(data->rk->datan.ravno(),dd,md,gd,data->rk->tabnom.ravno(),data->rk->podr.ravno(),ff_prot);

zarrnesvw(ff_prot,data->window);

int kolstr=0;
float kolstr1=0;
int kolstr2=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
sprintf(strsql,"select tabn,podr from Zarn where god=%d and mes=%d \
order by podr,tabn asc",god_nah,mes_nah);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr ==  0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

fprintf(ff,"\
----------------------------------------------------------------------\n");
fprintf(ff,gettext(" Т/н | Фамилия  , Імя , Отчество    |Начислено | Удержано |  Долг    |\n"));
fprintf(ff,"\
----------------------------------------------------------------------\n");
fprintf(ff_soc,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_soc,"\
 Т/н |       Призвище               |Нараховано|Обов'язков|До видачі |   ЄСВ               |  ЄСВ з лікарняних   |   ЄСВ з інвалідів   |  ЄСВ з договорів    |Прибуткови|\n\
     |                              |          |і утриман.|          |Нарахуван.|Утримання |Нарахуван.|Утримання |Нарахуван.|Утримання |Нарахуван.|Утримання |й податок |\n");
/*********
                                         0          1          2          3         4           5          6           7         8          9         10        11
                                                               
12345 123456789012345678901234567890 1234567890 1234567890 1234567890
**********/
fprintf(ff_soc,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
const int RAZMER_MAS=12;
double suma_str[RAZMER_MAS];
double suma_str_mes[RAZMER_MAS];
double suma_str_dolg[RAZMER_MAS];
double isuma_str[RAZMER_MAS];
double isuma_str_dolg[RAZMER_MAS];

memset(isuma_str,'\0',sizeof(isuma_str));
memset(isuma_str_dolg,'\0',sizeof(isuma_str_dolg));
class iceb_u_str fio("");
int tabn=0;
int knah=0;
double sumanu=0.;
double i_suma_uder=0.;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->podr.ravno(),row[1],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
    continue;
  tabn=atoi(row[0]);  
  fio.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    fio.new_plus(row1[0]);

  fprintf(ff_prot,"\n%d %s\n",tabn,fio.ravno());

  /*Узнаём какая сумма к выплате и суммы всех отчислений*/
  memset(suma_str,'\0',sizeof(suma_str));
  memset(suma_str_dolg,'\0',sizeof(suma_str_dolg));

  
  sprintf(strsql,"select prn,knah,suma,datz,godn,mesn from Zarp where \
 datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and tabn=%s and suma <> 0.",
  god_nah,mes_nah,1,god_nah,mes_nah,31,row[0]);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  memset(suma_str_mes,'\0',sizeof(suma_str_mes));    

  while(cur1.read_cursor(&row1) != 0)
   {
    knah=atoi(row1[1]);
    sumanu=atof(row1[2]);
    if(atoi(row1[0]) == 1)
     {
      suma_str_mes[0]+=sumanu;
     }
    else
     {
      if(knah == kod_esv)
       suma_str_mes[4]+=sumanu;
      if(knah == kod_esv_bol)
       suma_str_mes[6]+=sumanu;
      if(knah == kod_esv_inv)
       suma_str_mes[8]+=sumanu;
      if(knah == kod_esv_dog)
       suma_str_mes[10]+=sumanu;

//      if(knah == kodpn)
      if(iceb_u_proverka(kodpn_all.ravno(),knah,0,1) == 0)
       suma_str_mes[11]+=sumanu;

      if(provkodw(obud,knah) >= 0)
       {
        suma_str_mes[1]+=sumanu;
        continue;  
       }        
     }
   }
  suma_str_mes[2]=suma_str_mes[0]+suma_str_mes[1];    
  /*определяем сумму всех удержаний в счёт этого месяца*/
  double suma_uder=prbper1w(mes_nah,god_nah,dd,md,gd,tabn,data->window);
  suma_str_dolg[2]+=suma_str_mes[0]+suma_uder;
  
  double koef=0.;
  if(suma_str_mes[2]+suma_uder != 0.)
    koef=suma_str_mes[2]/(suma_str_mes[0]+suma_uder);

  fprintf(ff_prot,"Коэффициент: %f=%.2f/(%.2f %.2f)\n",
  koef,suma_str_mes[2],suma_str_mes[0],suma_uder);

  if(data->rk->metka_vse == 1 && fabs(suma_str_mes[0]+suma_uder) < 0.009 )
    continue;
  i_suma_uder+=suma_uder;
  
  /*Смотрим начисления на фонд зарплаты*/
  sprintf(strsql,"select kodz,sumap from Zarsocz where datz='%04d-%02d-01' and tabn=%s",god_nah,mes_nah,row[0]);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  int kod_fonda=0;

  while(cur1.read_cursor(&row1) != 0)
   {
    kod_fonda=atoi(row1[0]);
    sumanu=atof(row1[1]);

    if(kod_fonda == kodf_esv)
     suma_str_mes[3]+=sumanu;    
    if(kod_fonda == kodf_esv_bol)
     suma_str_mes[5]+=sumanu;    
    if(kod_fonda == kodf_esv_inv)
     suma_str_mes[7]+=sumanu;    
    if(kod_fonda == kodf_esv_dog)
     suma_str_mes[9]+=sumanu;    

   }

  for(int nomm=0; nomm < RAZMER_MAS; nomm++)
    suma_str[nomm]+=suma_str_mes[nomm];

  /*Делаем расчёт налогов на долг*/
  if(koef != 0.)
   for(int nomm=3; nomm < RAZMER_MAS; nomm++)
     suma_str_dolg[nomm]+=suma_str_mes[nomm]*koef;

  fprintf(ff,"%-5d %-*.*s %10.2f %10.2f %10.2f\n",
  tabn,
  iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno(),
  suma_str_mes[0],suma_uder,suma_str_mes[0]+suma_uder);

  fprintf(ff_soc,"%5s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  row[0],
  iceb_u_kolbait(30,fio.ravno()),
  iceb_u_kolbait(30,fio.ravno()),
  fio.ravno(),
  suma_str[0],
  suma_str[1],
  suma_str[2],
  suma_str[3],
  suma_str[4],
  suma_str[5],
  suma_str[6],
  suma_str[7],
  suma_str[8],
  suma_str[9],
  suma_str[10],
  suma_str[11]);

  fprintf(ff_soc,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  iceb_u_kolbait(58,gettext("Долг")),
  gettext("Долг"),
  suma_str_dolg[2],
  suma_str_dolg[3],
  suma_str_dolg[4],
  suma_str_dolg[5],
  suma_str_dolg[6],
  suma_str_dolg[7],
  suma_str_dolg[8],
  suma_str_dolg[9],
  suma_str_dolg[10],
  suma_str_dolg[11]);



  for(int nomm=0; nomm < RAZMER_MAS; nomm++)
   {
    isuma_str[nomm]+=suma_str[nomm];
    isuma_str_dolg[nomm]+=suma_str_dolg[nomm];
   }  

 }

fprintf(ff,"\
----------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(36,gettext("Итого")),
gettext("Итого"),
isuma_str[0],i_suma_uder,isuma_str[0]+i_suma_uder);

fprintf(ff_soc,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_soc,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(36,gettext("Итого начислено")),
gettext("Итого начислено"),
isuma_str[0],
isuma_str[1],
isuma_str[2],
isuma_str[3],
isuma_str[4],
isuma_str[5],
isuma_str[6],
isuma_str[7],
isuma_str[8],
isuma_str[9],
isuma_str[10],
isuma_str[11]);

fprintf(ff_soc,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_soc,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(58,gettext("Итого долгов")),
gettext("Итого долгов"),
isuma_str_dolg[2],
isuma_str_dolg[3],
isuma_str_dolg[4],
isuma_str_dolg[5],
isuma_str_dolg[6],
isuma_str_dolg[7],
isuma_str_dolg[8],
isuma_str_dolg[9],
isuma_str_dolg[10],
isuma_str_dolg[11]);
 



iceb_podpis(ff,data->window);
fclose(ff);
iceb_podpis(ff_soc,data->window);
fclose(ff_soc);
iceb_podpis(ff_prot,data->window);
fclose(ff_prot);

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Распечатка долгов"));


data->rk->imaf.plus(imaf_soc);
data->rk->naimf.plus(gettext("Распечатка долгов по соц. фондов"));

data->rk->imaf.plus(imaf_prot);
data->rk->naimf.plus(gettext("Протокол хода расчёта"));

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
