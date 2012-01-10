/*$Id: zar_soc_nah_r.c,v 1.15 2011-08-29 07:13:44 sasa Exp $*/
/*06.05.2011	16.01.2007	Белых А.И.	zar_soc_nah_r.c
Расчёт свода отчислений в соц-фонды на фонд оплаты труда
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_soc_nah.h"

class zar_soc_nah_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_soc_nah_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_soc_nah_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_soc_nah_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_soc_nah_r_data *data);
gint zar_soc_nah_r1(class zar_soc_nah_r_data *data);
void  zar_soc_nah_r_v_knopka(GtkWidget *widget,class zar_soc_nah_r_data *data);

void  socstrh(FILE *ff);
void socstr_rek(class zar_soc_nah_rek *rk,FILE *ff1);
double	socnul(short mn,short gn,short mk,short gk,short kodsf,FILE *ff,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern short	kodpenf; /*Код пенсионного фонда*/
extern short	kodsoc;   /*Код фонда социального страхования*/
extern short	kodbezf;  /*Код фонда занятости*/
extern short	kodfsons;   /*Код фонда страхования от несчасного случая*/

extern short kodpen;  /*Код пенсионных отчислений*/
extern short kodsocstr;  /*Код отчисления на соц-страх*/
extern short kodbzr;  /*Код отчисления на безработицу*/

extern double   okrg;

extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/
extern int kod_esv_vs;

extern int kodf_esv; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_bol; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_inv; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_dog; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_vs; /*Код фонда удержания единого социального взноса*/

int zar_soc_nah_r(class zar_soc_nah_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_soc_nah_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт соц. отчислений на фонд оплаты труда"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_soc_nah_r_key_press),&data);

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

repl.plus(gettext("Расчёт соц. отчислений на начисленную зарплату"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_soc_nah_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_soc_nah_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_soc_nah_r_v_knopka(GtkWidget *widget,class zar_soc_nah_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_soc_nah_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_soc_nah_r_data *data)
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
/**********************/
/*Поиск суммы отчисления в фонд с работника*/
/********************************************/

double socstr_rud(short mn,short gn,short mk,short gk,int tabnom,int kodud,GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;

char strsql[512];
int kolstr=0;

sprintf(strsql,"select suma from Zarp where datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and tabn=%d and prn='2' and knah=%d and suma <> 0.",
gn,mn,1,gk,mk,31,tabnom,kodud);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

double suma=0.;
while(cur.read_cursor(&row) != 0)
 {
  suma+=atof(row[0]);  
 }

return(suma);
}

/******************************************/
/******************************************/

gint zar_soc_nah_r1(class zar_soc_nah_r_data *data)
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
iceb_u_rsdat1(&mk,&gk,data->rk->datak.ravno());
if(gk == 0)
 {
  mk=mn;
  gk=gn;
 }

sprintf(strsql,"%d.%d > %d.%d\n",mn,gn,mk,gk);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int kolsoc=0;
/*Определяем количество начислений и удержаний*/
sprintf(strsql,"select kod from Zarsoc");
class SQLCURSOR cur;
SQL_str row;
if((kolsoc=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

if(kolsoc == 0)
 {
  iceb_menu_soob(gettext("Не введены коды соц. отчислений на фонд оплаты труда !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"select distinct kodz,proc,shet from Zarsocz where \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31'",gn,mn,gk,mk);

if((kolsoc=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolsoc == 0)
 {
  iceb_menu_soob(gettext("Не нашли ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_spisok kodsocm;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s|%s",row[0],row[1],row[2]);
  kodsocm.plus(strsql);
 }

class iceb_u_double kodsum;
kodsum.make_class(kolsoc);
class iceb_u_double sumasn;
class iceb_u_double sumasnb;
sumasn.make_class(kolsoc);
sumasnb.make_class(kolsoc);

//double sumasn[kolsoc]; //Сумма с которой начислялось
//double sumasnb[kolsoc]; //Сумма с которой начислялось бюджет
//memset(&sumasn,'\0',sizeof(sumasn));
//memset(&sumasnb,'\0',sizeof(sumasnb));

class iceb_u_double kodsumb; //Сумма перечисления в фонд с бюджетной части
kodsumb.make_class(kolsoc);


sprintf(strsql,"select tabn,kodz,sumap,proc,datz,shet,sumas,sumapb,sumasb from \
Zarsocz where datz >= '%04d-%d-01' and datz <= '%04d-%d-31' \
order by kodz,proc,tabn asc",gn,mn,gk,mk);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не нашли ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short dkm=1;
iceb_u_dpm(&dkm,&mk,&gk,5);

FILE *ff;
char imaf[64];
sprintf(imaf,"nnfz%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
 }
FILE *ff1;
char imaf1[64];
sprintf(imaf1,"nnfzs%d.lst",getpid());

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
 }


iceb_u_zagolov(gettext("Расчёт соц. отчислений на фонд оплаты труда"),1,mn,gn,dkm,mk,gk,organ,ff);

socstr_rek(data->rk,ff);


iceb_u_zagolov(gettext("Расчёт соц. отчислений на фонд оплаты труда"),1,mn,gn,dkm,mk,gk,organ,ff1);
socstr_rek(data->rk,ff1);

fprintf(ff1,"\n\
Расшифровка колонок:\n\
1-код фонда\n\
2-общая сумма всех начислений\n\
3-сумма взятая в расчет\n\
4-сумма налога с фонда оплаты\n\
5-сумма налога для бюджета\n\
6-процент налога\n\
7-счет\n\
8-дата записи\n\
9-сумма налога с работника\n\n");



double sum1=0.,sum2=0.,sum3=0.;
int nomerstr=0;
class iceb_u_spisok FONDSHET; //Список код фонда-счет
class iceb_u_double SUMA; //Массив сумм к списку код фонда-счет
char tabn[64];
double bb=0.;
memset(tabn,'\0',sizeof(tabn));
double		sumkodz[5];
memset(&sumkodz,'\0',sizeof(sumkodz));
float kolstr1=0;
class SQLCURSOR curr;
char naimf[512];
double isumanah=0.;
double sumanah=0.;
short		kodzz=0;
short d,m,g;
SQL_str row1;
char kod[32];
int kodz=0;
int i=0;
double suma=0.;
char shet[32];
char fio[512];
char		metkasum[5];
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->kod_soc.ravno(),row[1],0,0) != 0)
    continue;

  if(data->rk->podr.getdlinna() > 1)
   {
    
    iceb_u_rsdat(&d,&m,&g,row[4],2);

    sprintf(strsql,"select podr from Zarn where god=%d and mes=%d \
and tabn=%s",g,m,row[0]);
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
      continue;    
    strncpy(kod,row1[0],sizeof(kod)-1);

    if(iceb_u_proverka(data->rk->podr.ravno(),kod,0,0) != 0)
      continue;
    
   }
  kodz=atoi(row[1]);
  if(kodz != kodzz)
   {

    if(kodzz != 0)
     {
      fprintf(ff1,"%*.*s:%8.2f %8.2f %8.2f %8.2f %24s %10.2f\n",
      iceb_u_kolbait(39,naimf),iceb_u_kolbait(39,naimf),naimf,sumkodz[0],sumkodz[1],sumkodz[2],sumkodz[3],"",sumkodz[4]);
      if((bb=socnul(mn,gn,mk,gk,kodzz,ff1,data->window)) != 0.)
       {
        fprintf(ff1,"%*s:%8.2f\n",
        iceb_u_kolbait(39,gettext("Общий итог по фонду")),gettext("Общий итог по фонду"),sumkodz[0]+bb);

       }
      
     }

    //Читаем наименование фонда
    memset(naimf,'\0',sizeof(naimf));
    sprintf(strsql,"select naik from Zarsoc where kod=%d",kodz);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
      strncpy(naimf,row1[0],sizeof(naimf)-1);
    fprintf(ff1,"%d %s\n",kodz,naimf);
    socstrh(ff1);
  //  isumanah=0.;
    memset(&sumkodz,'\0',sizeof(sumkodz));
    kodzz=kodz;
   }
  sprintf(strsql,"%s|%s|%s",row[1],row[3],row[5]);

  if((i=kodsocm.find(strsql)) < 0)
   {
    sprintf(strsql,"Не найдено %s в массиве кодов фондов/процент !\n",strsql);
    iceb_menu_soob(strsql,data->window);
    continue;
   }       
  suma=atof(row[2]);
  suma=iceb_u_okrug(suma,okrg);
  
  kodsum.plus(suma,i);
  sum2+=suma;
  sumkodz[2]+=suma;

  suma=atof(row[6]);
  suma=iceb_u_okrug(suma,okrg);
  sumasn.plus(suma,i);
  sumasnb.plus(atof(row[8]),i);
  sum1+=suma;
  sumkodz[1]+=suma;

  suma=atof(row[7]);
  suma=iceb_u_okrug(suma,okrg);
  kodsumb.plus(suma,i);
  sum3+=suma;
  sumkodz[3]+=suma;

  if(iceb_u_SRAV(tabn,row[0],0) != 0)
   {
    memset(fio,'\0',sizeof(fio));
    sprintf(strsql,"select fio,shet from Kartb where tabn=%s",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
/****************
      sprintf(strsql,"delete from Zarsocz where tabn=%s",row[0]);
      sql_zapis(strsql,1,0);
      continue;
********************/      
      
      sprintf(strsql,"%s %s !",gettext("Не найдено табельный номер"),row[0]);
      iceb_menu_soob(strsql,data->window);
      continue;      
     }
    strncpy(fio,row1[0],sizeof(fio)-1);
    strcpy(shet,row1[1]);
   }
  iceb_u_rsdat(&d,&m,&g,row[4],2);
  isumanah+=sumanah=sosnsumw(row[0],1,m,g,31,m,g,data->window);
  sumkodz[0]+=sumanah;
  
  memset(metkasum,'\0',sizeof(metkasum));
  if(fabs(sumanah-atof(row[6])) > 0.009)
     metkasum[0]='*';
  else
     metkasum[0]=' ';

  double suma_sr=0.; /*Сумма с работника*/
  
  if(kodpenf  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kodpen,data->window);
  
  if(kodsoc  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kodsocstr,data->window);
  
  if(kodbezf  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kodbzr,data->window);
  /**************/
  if(kodf_esv  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv,data->window);
  if(kodf_esv_bol  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv_bol,data->window);
  if(kodf_esv_inv  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv_inv,data->window);
  if(kodf_esv_dog  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv_dog,data->window);
  if(kodf_esv_vs  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv_vs,data->window);
  
  sumkodz[4]+=suma_sr;
    
  fprintf(ff1,"%5s %-*.*s %2s %8.2f %8s%s%8s %8s %5s%% %-6s %s %10.2f\n",
  row[0],iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,row[1],sumanah,row[6],metkasum,row[2],row[7],row[3],row[5],row[4],suma_sr);
  strncpy(tabn,row[0],sizeof(tabn)-1);

  suma=atof(row[2]);
  suma=iceb_u_okrug(suma,okrg);
  sprintf(strsql,"%d|%s",kodzz,shet);

  if((nomerstr=FONDSHET.find(strsql)) == -1)
    FONDSHET.plus(strsql);
  SUMA.plus(suma,nomerstr);
 }

fprintf(ff1,"%*.*s:%8.2f %8.2f %8.2f %8.2f %24s %10.2f\n",
iceb_u_kolbait(39,naimf),iceb_u_kolbait(39,naimf),naimf,sumkodz[0],sumkodz[1],sumkodz[2],sumkodz[3],"",sumkodz[4]);

if((bb=socnul(mn,gn,mk,gk,kodzz,ff1,data->window)) != 0.)
 {
  fprintf(ff1,"%*s:%8.2f\n",
  iceb_u_kolbait(39,gettext("Общий итог по фонду")),gettext("Общий итог по фонду"),sumkodz[0]+bb);
 }

fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,"%*s:%8.2f %8.2f %8.2f %8.2f\n",
iceb_u_kolbait(39,gettext("Общий итог по всем фондам")),gettext("Общий итог по всем фондам"),isumanah+bb,sum1,sum2,sum3);

double itogsn=0.,itogo=0.,itogob=0.;

fprintf(ff,"\n\
------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,
gettext("Код|       Наименование фонда     |Проц. |   Счет   |Сумма с/н |Сумма ит| Сумма бюджет   | Сумма небюджет |\n"));
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------\n");

fprintf(ff1,"\n\
------------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,
gettext("Код|       Наименование фонда     |Проц. |   Счет   |Сумма с/н |Сумма ит| Сумма бюджет   | Сумма небюджет |\n"));
fprintf(ff1,"\
------------------------------------------------------------------------------------------------------------\n");
/*Распечатываем массивы*/
char procent[20];

for(int i=0; i < kolsoc ; i++)
 {
  if(kodsum.ravno(i) == 0.)
    continue;
  strncpy(strsql,kodsocm.ravno(i),sizeof(strsql)-1);
  iceb_u_pole(strsql,kod,1,'|');
  iceb_u_pole(strsql,procent,2,'|');
  iceb_u_pole(strsql,shet,3,'|');
  memset(naimf,'\0',sizeof(naimf));
  sprintf(strsql,"select naik,shet from Zarsoc where kod=%s",kod);
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    strncpy(naimf,row[0],sizeof(naimf)-1);
   }
  sprintf(strsql,"%3s %-*.*s %5s%% %-*s %10.2f %8.2f %8.2f\n",
  kod,
  iceb_u_kolbait(20,naimf),iceb_u_kolbait(20,naimf),naimf,
  procent,
  iceb_u_kolbait(10,shet),shet,
  sumasn.ravno(i),kodsum.ravno(i),kodsumb.ravno(i));

  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  fprintf(ff,"%3s %-*.*s %5s%% %-*s %10.2f %8.2f %8.2f/%8.2f %8.2f/%8.2f\n",
  kod,iceb_u_kolbait(30,naimf),iceb_u_kolbait(30,naimf),naimf,
  procent,
  iceb_u_kolbait(10,shet),shet,
  sumasn.ravno(i),kodsum.ravno(i),kodsumb.ravno(i),
  sumasnb.ravno(i),
  kodsum.ravno(i)-kodsumb.ravno(i),
  sumasn.ravno(i)-sumasnb.ravno(i));


  fprintf(ff1,"%3s %-*.*s %5s%% %-*s %10.2f %8.2f %8.2f/%8.2f %8.2f/%8.2f\n",
  kod,
  iceb_u_kolbait(30,naimf),iceb_u_kolbait(30,naimf),naimf,
  procent,
  iceb_u_kolbait(10,shet),shet,
  sumasn.ravno(i),kodsum.ravno(i),kodsumb.ravno(i),
  
  sumasnb.ravno(i),
  kodsum.ravno(i)-kodsumb.ravno(i),
  sumasn.ravno(i)-sumasnb.ravno(i));

  itogo+=kodsum.ravno(i);
  itogob+=kodsumb.ravno(i);
  itogsn+=sumasn.ravno(i);
 }
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %8.2f %8.2f          %8.2f\n",
iceb_u_kolbait(52,gettext("Итого")),gettext("Итого"),itogsn,itogo,itogob,itogo-itogob);


fprintf(ff1,"\
------------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,"%*s:%10.2f %8.2f %8.2f          %8.2f\n",
iceb_u_kolbait(52,gettext("Итого")),gettext("Итого"),itogsn,itogo,itogob,itogo-itogob);

sprintf(strsql,"%*s:%10.2f %8.2f %8.2f\n",
iceb_u_kolbait(42,gettext("Итого")),gettext("Итого"),itogsn,itogo,itogob);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

iceb_podpis(ff,data->window);
iceb_podpis(ff1,data->window);

fclose(ff);
fclose(ff1);

data->rk->imaf.plus(imaf1);
data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт соц. отчислений на фонд оплаты труда"));
data->rk->naimf.plus(gettext("Общие итоги"));


for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),1,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}

/*************************************************************************/
/*Получение списка имеющих начисления и не имеющих отчислений в соц фонды*/
/*************************************************************************/

double	socnul(short mn,short gn,short mk,short gk,
short	kodsf,
FILE *ff,GtkWidget *wpredok)
{
char	strsql[512];
int	kolstr=0;
SQL_str	row,row1;
char	fio[512];
double  suma=0.;
double  isuma=0.;
SQLCURSOR curr;
sprintf(strsql,"select distinct tabn from Zarp where \
datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and prn='1'",
gn,mn,1,gk,mk,31);

SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }
fprintf(ff,"\n%s:\n",
gettext("Список работников не имеющих отчислений в соц. фонд"));

while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);

  sprintf(strsql,"select distinct kodz from Zarsocz where \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31' and tabn=%s and kodz=%d",
  gn,mn,gk,mk,row[0],kodsf);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 0)
   {
    memset(fio,'\0',sizeof(fio));
    sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     strncpy(fio,row1[0],sizeof(fio)-1);

    suma=sosnsumw(row[0],1,mn,gn,31,mk,gk,wpredok);
    isuma+=suma;
    
    if(suma != 0.)
     fprintf(ff,"%5s %-*.*s %2s %8.2f\n",
     row[0],iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio," ",suma);

   }

 }
if(isuma != 0. && isuma > suma)
 fprintf(ff,"%*s:%.2f\n",iceb_u_kolbait(40,gettext("Итого по не имеющим удержания")),gettext("Итого по не имеющим удержания"),isuma);
return(isuma); 
}
/*******************************/
/*шапка*/
/*******************************/
void  socstrh(FILE *ff)
{
fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 Т/н |Фамилия, имя, отчество        |1 |   2    |   3    |   4    |   5    |  6   |  7   |    8     |     9    |\n");
fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------\n");
}
/*************************/
/*Реквизиты расчёта*/
/*************************/
void socstr_rek(class zar_soc_nah_rek *rk,FILE *ff1)
{

if(rk->podr.getdlinna() > 1)
 {
  fprintf(ff1,"%s:%s\n",gettext("Код подразделения"),rk->podr.ravno());
  int klst=0;
  iceb_printcod(ff1,"Podr","kod","naik",0,rk->podr.ravno(),&klst);
 }
else
 fprintf(ff1,gettext("По всем подразделениям.\n"));

if(rk->tabnom.getdlinna() > 1)
 fprintf(ff1,"%s:%s\n",gettext("Табельный номер"),rk->tabnom.ravno());
if(rk->kod_soc.getdlinna() > 1)
 fprintf(ff1,"%s:%s\n",gettext("Код соц. фонда"),rk->kod_soc.ravno());

}
