/*$Id: zar_spsoclgot_r.c,v 1.16 2011-04-14 16:09:36 sasa Exp $*/
/*05.12.2010	20.01.2007	Белых А.И.	zar_spsoclgot_r.c
Расчёт списка имеющих льготы на соц.отчисления.
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"

class zar_spsoclgot_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  const char *data_d;
  class iceb_u_spisok *imaf;
  class iceb_u_spisok *naimf;
    
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_spsoclgot_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_spsoclgot_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_spsoclgot_r_data *data);
gint zar_spsoclgot_r1(class zar_spsoclgot_r_data *data);
void  zar_spsoclgot_r_v_knopka(GtkWidget *widget,class zar_spsoclgot_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

extern short    *kodnvpen; /*Коды не входящие в расчет пенсионного отчисления с работника*/
extern short	kodpenf; /*Код пенсионного фонда*/
extern short	kodbezf;  /*Код фонда занятости*/
extern short	kodfsons;   /*Код фонда страхования от несчасного случая*/
extern short	kodsoc;   /*Код фонда социального страхования*/
extern short    *kodbl;  /*Код начисления больничного*/
extern short	*kodbzrnv; //Коды не входящие в расчет отчислений на безработицу
extern short    kodbzr;  /*Код отчисления на безработицу*/
extern short    *kodsocstrnv; //Коды не входящие в расчет соцстраха
extern short    kodpen;  /*Код пенсионных отчислений*/
extern short    *kodmp;   /*Коды материальной помощи*/

int zar_spsoclgot_r(const char *data_d,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_spsoclgot_r_data data;

data.data_d=data_d;
data.imaf=imaf;
data.naimf=naimf;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать список имеющих льготы на соц. отчисления"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_spsoclgot_r_key_press),&data);

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

repl.plus(gettext("Распечатать список имеющих льготы на соц. отчисления"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_spsoclgot_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_spsoclgot_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_spsoclgot_r_v_knopka(GtkWidget *widget,class zar_spsoclgot_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_spsoclgot_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_spsoclgot_r_data *data)
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

gint zar_spsoclgot_r1(class zar_spsoclgot_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);
SQL_str row_alx;
class SQLCURSOR cur_alx;


short mr,gr;
iceb_u_rsdat1(&mr,&gr,data->data_d);

//чтение настроек
class zar_read_tn1h nastr;
zar_read_tn1w(1,mr,gr,&nastr,NULL,data->window);

char   		kodnvpenf[300]; //Коды не входящие в расчет пенсионного отчисления
char   		kodnvbezr[300]; //Коды не входящие в расчет безработицы
char   		kodnvtravm[300]; //Коды не входящие в расчет фодда страхования от несчастного случая
char   		kodnvsoc[300]; //Коды не входящие в расчет фонда соц. страхование

SQLCURSOR curr;
SQL_str row;
memset(kodnvpenf,'\0',sizeof(kodnvpenf));
sprintf(strsql,"select kodn from Zarsoc where kod=%d",kodpenf);
if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)
 {  
  strncpy(kodnvpenf,row[0],sizeof(kodnvpenf)-1);
 }

memset(kodnvbezr,'\0',sizeof(kodnvbezr));
sprintf(strsql,"select kodn from Zarsoc where kod=%d",kodbezf);
if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)
 {  
  strncpy(kodnvbezr,row[0],sizeof(kodnvbezr)-1);
 }

memset(kodnvtravm,'\0',sizeof(kodnvtravm));
sprintf(strsql,"select kodn from Zarsoc where kod=%d",kodfsons);
if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)
 {  
  strncpy(kodnvtravm,row[0],sizeof(kodnvtravm)-1);
 }
memset(kodnvsoc,'\0',sizeof(kodnvsoc));
sprintf(strsql,"select kodn from Zarsoc where kod=%d",kodsoc);
if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)
 {  
  strncpy(kodnvsoc,row[0],sizeof(kodnvsoc)-1);
 }
char imaf[56];
sprintf(imaf,"socs%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_zagolov(gettext("Распечатка списка имеющих льготы на соц. отчисления"),0,0,0,0,0,0,organ,ff);

fprintf(ff,"%s %02d.%d%s\n",
gettext("Дата"),mr,gr,
gettext("г."));

fprintf(ff,"%s => %.2f\n",
gettext("Список имеющих сумму начисления"),nastr.max_sum_for_soc);

sprintf(strsql,"%s => %.2f\n",
gettext("Список имеющих сумму начисления"),nastr.max_sum_for_soc);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


fprintf(ff,"%s:%s\n",gettext("Коды не входящие в расчет на фонд зарплаты"),kodnvpenf);

fprintf(ff,"\
-----------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Т/н |       Фамилия             |   Сумма  |Пенсионный|Безработи.| В расчет |Соц.страх |\n\
     |                           |          |с фонда з.|с фонда з.| травмат- |с фонда з.|\n\
     |                           |          |с работни.|с работни.|   изма   |с работни.|\n"));
fprintf(ff,"\
-----------------------------------------------------------------------------------------\n");
int kolpens=0;
int kolpensr=0;
int kolbezr1=0;
int kolbezr2=0;
int koltravm=0;
int kolsocf=0;
int kolsocr=0;

SQLCURSOR cur;
SQLCURSOR cur1;
int	kolstr=0;
float kolstr1=0;
int kolstr2=0;

sprintf(strsql,"select tabn from Zarn where god=%d and mes=%d",gr,mr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int tabn;
char fio[512];
double nah=0.;
double nahr=0.;
double sumao=0.;
SQL_str	row1;
int kol=0;
double suma=0.;
short  knah=0;
double itogo=0.,itogoo=0.;
double itogobezr=0.;
double itogobezrsr=0.;
double nahbezr=0.; //безработица с фонда зарплаты
double nahbezrsr=0.; //Безработица с работника
double nahtravm=0.; //В фонд травматизма с фонда зарплати
double nahsocf=0.; //Социальное страхование с фонда зарплаты
double nahsocr=0.; //Социальное страхование с работника
double itogotravm=0.;
double itogosocf=0.;
double itogosocr=0.;
double itogopensr=0.;
short metkabezr=0;
short metkapens=0;
short prn=0;
short mesn,godn;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
//  strzag(LINES-1,0,kolstr,++kolstr1);
  tabn=atoi(row[0]);

  memset(fio,'\0',sizeof(fio));
  sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    strncpy(fio,row1[0],sizeof(fio)-1);    

  sprintf(strsql,"select prn,knah,suma,mesn,godn from Zarp where tabn=%d and \
datz >= '%04d-%d-01' and  datz <= '%04d-%d-31' and \
suma <> 0.",tabn,gr,mr,gr,mr);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  if(kolstr2 == 0)
    continue;

  nahtravm=nahbezr=nahbezrsr=nah=nahr=sumao=0.;
  metkapens=metkabezr=0;
  nahsocf=nahsocr=0.;
  double suma_boln_ntm=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    
    prn=atoi(row1[0]);
    knah=atoi(row1[1]);
    suma=atof(row1[2]);
    mesn=atoi(row1[3]);
    godn=atoi(row1[4]);     

            
    if(prn == 1)
     {     
      sumao+=suma;
      //Если больничный не за месяц расчета то он не входит в сумму для определения граничной суммы      
      if(mesn != mr || godn != gr)
       if(provkodw(kodbl,knah) >= 0)
         suma_boln_ntm+=suma;        

      if(provkodw(kodnvpen,knah) < 0)
         nahr+=suma;

      if(iceb_u_proverka(kodnvpenf,row1[1],0,1) != 0)
        nah+=suma;
      if(iceb_u_proverka(kodnvbezr,row1[1],0,1) != 0)
        nahbezr+=suma;
      if(iceb_u_proverka(kodnvtravm,row1[1],0,1) != 0)
        nahtravm+=suma;
      if(provkodw(kodbzrnv,knah) < 0)
        nahbezrsr+=suma;

      if(provkodw(kodsocstrnv,knah) < 0)
        nahsocr+=suma;
      if(iceb_u_proverka(kodnvsoc,row1[1],0,1) != 0)
        nahsocf+=suma;

      }
    if(prn == 2)
     {
      if(kodpen == knah)
       metkapens=1;
      if(kodbzr == knah)
       metkabezr=1;
       
     }
   }
  nah-=suma_boln_ntm;
  nahr-=suma_boln_ntm;
       

  if(nah <= nastr.max_sum_for_soc && nahr <= nastr.max_sum_for_soc && nahbezr <= nastr.max_sum_for_soc &&
  nahbezrsr <= nastr.max_sum_for_soc && nahtravm <= nastr.max_sum_for_soc && nahsocr <= nastr.max_sum_for_soc &&
  nahsocf <= nastr.max_sum_for_soc)
    continue;

  if(metkabezr == 0)
    nahbezrsr=0.;
    
  sprintf(strsql,"%5d %-*.*s %10.2f\n",tabn,iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,nah);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  fprintf(ff,"%5d %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  tabn,iceb_u_kolbait(27,fio),iceb_u_kolbait(27,fio),fio,sumao,nah,nahbezr,nahtravm,nahsocf);

  fprintf(ff,"%44s %10.2f %10.2f %10s %10.2f\n",
  " ",nahr,nahbezrsr," ",nahsocr);
 

  if(nah > nastr.max_sum_for_soc)
   {
    kolpens++;
    itogo+=nah;
   }
  if(nahr > nastr.max_sum_for_soc)
   {
    itogopensr+=nahr;
    kolpensr++;
   }
  itogoo+=sumao;
 
  if(nahbezr > nastr.max_sum_for_soc)
   {
    itogobezr+=nahbezr;
    kolbezr1++;
   }
  if(nahbezrsr > nastr.max_sum_for_soc)
   {
    itogobezrsr+=nahbezrsr;
    kolbezr2++;
   }
  if(nahtravm > nastr.max_sum_for_soc)
   {
    itogotravm+=nahtravm;
    koltravm++;
   }
  if(nahsocf > nastr.max_sum_for_soc)
   {
    itogosocf+=nahsocf;
    kolsocf++;    
   }
  if(nahsocr > nastr.max_sum_for_soc)
   {
    itogosocr+=nahsocr;
    kolsocr++;    
   }  
  kol++;
 }

fprintf(ff,"\
-----------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(33,gettext("Итого")),gettext("Итого"),itogoo,itogo,itogobezr,itogotravm,itogosocf);

fprintf(ff,"%44s %10.2f %10.2f %10s %10.2f\n",
" ",itogopensr,itogobezrsr," ",itogosocr);

fprintf(ff,"\n%s: %.2f-%.2f=%.2f\n",
gettext("В пенсионный фонд (с фонда зарплаты)"),
itogo,nastr.max_sum_for_soc*kolpens,itogo-nastr.max_sum_for_soc*kolpens);

fprintf(ff,"%s: %.2f-%.2f=%.2f\n",
gettext("В пенсионный фонд (с работника)"),itogopensr,nastr.max_sum_for_soc*kolpensr,itogopensr-nastr.max_sum_for_soc*kolpensr);

fprintf(ff,"%s: %.2f-%.2f=%.2f\n",
gettext("В фонд безработицы (с фонда зарплаты)"),
itogobezr,nastr.max_sum_for_soc*kolbezr1,itogobezr-nastr.max_sum_for_soc*kolbezr1);

fprintf(ff,"%s: %.2f-%.2f=%.2f\n",
gettext("В фонд безработицы (с работника)"),
itogobezrsr,nastr.max_sum_for_soc*kolbezr2,itogobezrsr-nastr.max_sum_for_soc*kolbezr2);

fprintf(ff,"%s: %.2f-%.2f=%.2f\n",
gettext("В фонд травматизма"),itogotravm,nastr.max_sum_for_soc*koltravm,itogotravm-nastr.max_sum_for_soc*koltravm);

fprintf(ff,"%s: %.2f-%.2f=%.2f\n",
gettext("Соц. страхование (с фонда зарплаты)"),itogosocf,nastr.max_sum_for_soc*kolsocf,itogosocf-nastr.max_sum_for_soc*kolsocf);

fprintf(ff,"%s: %.2f-%.2f=%.2f\n\n",
gettext("Соц. страхование (с работника)"),itogosocr,nastr.max_sum_for_soc*kolsocr,itogosocr-nastr.max_sum_for_soc*kolsocr);


//Читаем файл пенсионеров

sprintf(strsql,"select str from Alx where fil='zarpens.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob("Не найдены настройки zarpens.alx",data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


fprintf(ff,"%s.\n",
gettext("Список работающих пенсионеров"));

sprintf(strsql,"%s.\n",
gettext("Список работающих пенсионеров"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


fprintf(ff,"%s.\n",gettext("Розсчёт для фонда безробіття"));
fprintf(ff,"\
--------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Т/н |       Фамилия                          |   Сумма  |В расчет  |В расчет  |\n\
     |                                        |          |на фонд з.|с работник|\n"));

fprintf(ff,"\
--------------------------------------------------------------------------------\n");

nahbezr=0.;
itogobezr=itogobezrsr=0.;
itogoo=0.;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
//  printw("%s",row_alx[0]);
  tabn=atoi(row_alx[0]);
  if(tabn == 0)
    continue;

  memset(fio,'\0',sizeof(fio));
  sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);

  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    strncpy(fio,row1[0],sizeof(fio)-1);    
  else
    continue;

  sprintf(strsql,"select prn,knah,suma from Zarp where tabn=%d and \
datz >= '%04d-%d-01' and  datz <= '%04d-%d-31' and \
suma <> 0.",tabn,gr,mr,gr,mr);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  if(kolstr == 0)
    continue;

  nahbezr=nahbezrsr=sumao=0.;
  metkapens=metkabezr=0;
  while(cur.read_cursor(&row) != 0)
   {
    prn=atoi(row[0]);
    knah=atoi(row[1]);
    suma=atof(row[2]);

    if(prn == 1)
     {     
      sumao+=suma;
      if(iceb_u_proverka(kodnvbezr,row[1],0,1) != 0)
        nahbezr+=suma;
      if(provkodw(kodbzrnv,knah) < 0)
        nahbezrsr+=suma;
     }
    if(prn == 2)
     {
      if(kodpen == knah)
       metkapens=1;
      if(kodbzr == knah)
       metkabezr=1;
       
     }
   }
  if(nahbezr > nastr.max_sum_for_soc)
    nahbezr=nastr.max_sum_for_soc;
  if(nahbezrsr > nastr.max_sum_for_soc)
    nahbezrsr=nastr.max_sum_for_soc;
  if(metkabezr == 0)
    nahbezrsr=0.;

  sprintf(strsql,"%5d %-*.*s %10.2f\n",tabn,iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,sumao);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  fprintf(ff,"%5d %-*.*s %10.2f %10.2f %10.2f\n",
  tabn,iceb_u_kolbait(40,fio),iceb_u_kolbait(40,fio),fio,sumao,nahbezr,nahbezrsr);
  itogoo+=sumao;
  itogobezr+=nahbezr;
  itogobezrsr+=nahbezrsr;

 }
fprintf(ff,"\
--------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(46,gettext("Итого")),gettext("Итого"),itogoo,itogobezr,itogobezrsr);

//Читаем файл инвалидов
sprintf(strsql,"select str from Alx where fil='zarinv.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob("Не найдены настройки zarinv.alx",data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


fprintf(ff,"%s.\n",
gettext("Список работающих инвалидов"));

sprintf(strsql,"%s.\n",
gettext("Список работающих инвалидов"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(ff,"\
---------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Т/н |       Фамилия                |   Сумма  | В расчет | В расчет | В расчет |\n\
     |                              |          |пенс.фонда|безработиц|безработиц|\n\
     |                              |          |на фонд з.|на фонд з.|с работник|\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------\n");


itogoo=itogo=0.;
nahbezr=0.;
itogobezr=itogobezrsr=0.;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  tabn=atoi(row_alx[0]);
  if(tabn == 0)
    continue;

  memset(fio,'\0',sizeof(fio));
  sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);

  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    strncpy(fio,row1[0],sizeof(fio)-1);    
  else
    continue;
    
  sprintf(strsql,"select prn,knah,suma from Zarp where tabn=%d and \
datz >= '%04d-%d-01' and  datz <= '%04d-%d-31' and \
suma <> 0.",tabn,gr,mr,gr,mr);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  if(kolstr == 0)
    continue;

  nahbezr=nahbezrsr=nah=sumao=0.;
  metkapens=metkabezr=0;
  while(cur.read_cursor(&row) != 0)
   {
    prn=atoi(row[0]);
    knah=atoi(row[1]);
    suma=atof(row[2]);

    if(prn == 1)
     {     
      sumao+=suma;
      if(iceb_u_proverka(kodnvpenf,row[1],0,1) != 0)
        nah+=suma;
      if(iceb_u_proverka(kodnvbezr,row[1],0,1) != 0)
        nahbezr+=suma;
      if(provkodw(kodbzrnv,knah) < 0)
        nahbezrsr+=suma;
     }
    if(prn == 2)
     {
      if(kodpen == knah)
       metkapens=1;
      if(kodbzr == knah)
       metkabezr=1;
       
     }

   }

  if(nah > nastr.max_sum_for_soc)
    nah=nastr.max_sum_for_soc;
  if(nahbezr > nastr.max_sum_for_soc)
    nahbezr=nastr.max_sum_for_soc;
  if(nahbezrsr > nastr.max_sum_for_soc)
    nahbezrsr=nastr.max_sum_for_soc;
  if(metkabezr == 0)
    nahbezrsr=0.;
    
  sprintf(strsql,"%5d %-*.*s %10.2f\n",tabn,iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,nah);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  
  fprintf(ff,"%5d %-*.*s %10.2f %10.2f %10.2f %10.2f\n",
  tabn,iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,sumao,nah,nahbezr,nahbezrsr);
  itogo+=sumao;
  itogoo+=nah;
  itogobezr+=nahbezr;
  itogobezrsr+=nahbezrsr;
 }
fprintf(ff,"\
---------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(36,gettext("Итого")),gettext("Итого"),itogo,itogoo,itogobezr,itogobezrsr);


iceb_podpis(ff,data->window);

fclose(ff);

data->imaf->plus(imaf);
data->naimf->plus(gettext("Список имеющих льготы ны соц. страхование"));

iceb_ustpeh(imaf,3,data->window);


data->kon_ras=0;
data->voz=0;
gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);

}
