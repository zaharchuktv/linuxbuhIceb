/*$Id: ras_sheks.c,v 1.18 2011-02-21 07:36:21 sasa Exp $*/
/*05.09.2005	22.12.2004	Белых А.И.	ras_sheks.c
Распечанка чеков на кассовом регистраторе
Возвращаем 0- если чек распечатан
*/
#include  <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "i_rest.h"

class ras_sheks_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  
  iceb_u_int *gods;
  iceb_u_spisok *nomdoks;
  
  ras_sheks_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   ras_sheks_r_key_press(GtkWidget *widget,GdkEventKey *event,class ras_sheks_r_data *data);
gint ras_sheks_r1(class ras_sheks_r_data *data);
gint ras_sheks_r2(class ras_sheks_r_data *data);
void  ras_sheks_r_v_knopka(GtkWidget *widget,class ras_sheks_r_data *data);


extern KASSA		kasr;
extern SQL_baza	bd;
extern char *name_system;
extern iceb_u_str shrift_ravnohir;
extern int     nomer_kas;  //номер кассы
extern class iceb_get_dev_data config_dev;
extern char *organ;
extern class KASSA kasr;

int  ras_sheks(iceb_u_int *gods,iceb_u_spisok *nomdoks,GtkWidget *wpredok)
{

if(iceb_menu_danet(gettext("Распечатать чек ? Вы уверены ?"),2,wpredok) == 2)
 return(1);

char strsql[500];
class ras_sheks_r_data data;
iceb_u_str repl;
data.gods=gods;
data.nomdoks=nomdoks;

//int		Model=0;

//Model = iceb_model(kasr.handle);



data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Печатаем чек"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(ras_sheks_r_key_press),&data);

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



sprintf(strsql,"%s:%d\n",gettext("Касса"),nomer_kas);
repl.new_plus(strsql);

/******************
if (Model & M_MARIA) 
   repl.plus(gettext("Печатаем чек на кассовом регистраторе"));
else if (Model & M_AMC100F) 
   repl.plus(gettext("Нажмите клавиши ФЦ-ВВ для печати чека или СБ для отмены"));
**********************/

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(ras_sheks_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

//if(config_dev.metka_kas_reg == 1) //кассовый регистратор подключен
if(kasr.handle > 0) //кассовый регистратор подключен и сервер запушен
 gtk_idle_add((GtkFunction)ras_sheks_r1,&data);
else
 gtk_idle_add((GtkFunction)ras_sheks_r2,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ras_sheks_r_v_knopka(GtkWidget *widget,class ras_sheks_r_data *data)
{
printf("ras_sheks_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   ras_sheks_r_key_press(GtkWidget *widget,GdkEventKey *event,class ras_sheks_r_data *data)
{
 printf("ras_sheks_r_key_press\n");
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

gint ras_sheks_r1(class ras_sheks_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1000];
iceb_u_str repl;
iceb_clock sss(data->window);

int koldok=data->nomdoks->kolih();
SQL_str row;
SQLCURSOR cur;
SQL_str row1;
SQLCURSOR cur1;
float procent_sk;
int kolstr;
float kolstr1=0.;
char naim[200];
double suma;
double itogo=0.;
double cena;
double kolih;
short vidnal;

for(int i=0; i < koldok; i++)
 {
  //читаем шапку документа
  sprintf(strsql,"select datd,ps from Restdok where god=%d and nomd='%s'",
  data->gods->ravno(i),data->nomdoks->ravno(i));
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    repl.new_plus(gettext("Не найден счёт"));
    repl.plus(data->nomdoks->ravno(i));
    repl.plus(" ");
    repl.plus(data->gods->ravno(i));
    iceb_menu_soob(&repl,data->window);
    continue;
   }
  procent_sk=atof(row[1]);
  sprintf(strsql,"%s=%f\n",gettext("Процента скидки"),procent_sk);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  
  sprintf(strsql,"select mz,kodz,kolih,cena,nds from Restdok1 where datd='%s' \
and nomd='%s'",row[0],data->nomdoks->ravno(i));

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  sprintf(strsql,"%d %s\n",data->gods->ravno(i),data->nomdoks->ravno(i));
  
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  kolstr1=0.;
  while(cur.read_cursor(&row) != 0)
   {
    iceb_pbar(data->bar,kolstr,++kolstr1);    


    if(atoi(row[0]) == 0) //товар
     sprintf(strsql,"select naimat from Material where kodm=%s",row[1]);
    else        //услуга
     sprintf(strsql,"select naius from Uslugi where kodus=%s",row[1]);

    memset(naim,'\0',sizeof(naim));

    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     strncpy(naim,row1[0],sizeof(naim)-1);

    sprintf(strsql,"%s %s\n",row[1],naim);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    
    cena=atof(row[3]);
    cena=cena-cena*procent_sk/100.;
    cena=iceb_u_okrug(cena,0.01);
    
    kolih=atof(row[2]);
    kolih=iceb_u_okrug(kolih,0.001);
    
    suma=cena*kolih;
    suma=iceb_u_okrug(suma,0.01);
    itogo+=suma;
    
    vidnal=atoi(row[4]);
    
    sprintf(strsql,"FSTART: ADD_CHECK\n\
%s\n\
%.2f\n\
%.2f\n\
%.6g\n\
%d\n\
FEND:\n",
     naim,suma,cena,kolih,vidnal);  
     if (iceb_cmd(&kasr, strsql,data->window)) 
      {
       data->voz=1;
       break;
      }
   }
 }

sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%s:\n\
FEND:\n",
kasr.prodavec.ravno(),kasr.parol,1,gettext("К оплате"));  

iceb_cmd(&kasr, strsql,data->window);

sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%.2f\n\
FEND:\n",
kasr.prodavec.ravno(),kasr.parol,2,itogo);  

iceb_cmd(&kasr, strsql,data->window);

short vozvr=0;
//    if(tipz == 1)
//     vozvr=1;
    /*************************
    sum_tara - Тара           |
    sum_pchek- Платежный чек  |=> Безналичная форма оплаты
    sum_cred - Кредит         |
    --------------------
    sum_nal  - Наличные

    Если сумма безналичной оплаты поркрывает общий результат операций по 
    чеку, то все движение средств представляет собой безналичную оплату.
    В противном случае недостающая до покрытия часть средств считается
    наличной.
    *************************/

double sum_nal=itogo;
double sum_tara=0.,sum_pchek=0.,sum_cred=0.;
int skl=1;
sprintf(strsql,"FSTART: MARIA_PRCHECK\n\
%s\n\
%s\n\
%d\n\
%d\n\
%.2f\n\
%.2f\n\
%.2f\n\
%.2f\n\
%.2f\n\
FEND:\n",
kasr.prodavec.ravno(),kasr.parol,skl,vozvr,itogo,sum_tara,sum_pchek,sum_cred,sum_nal);


if(iceb_cmd(&kasr, strsql,data->window) != 0) 
 data->voz=1;

data->voz=0;

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Печать закончена"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);
}
/***************************/
/*Распечатать на принтере*/
/**************************/
gint ras_sheks_r2(class ras_sheks_r_data *data)
{
time_t vremn;
struct tm *bf;

time(&vremn);
bf=localtime(&vremn);

char strsql[1000];
iceb_u_str repl;
iceb_clock sss(data->window);

int koldok=data->nomdoks->kolih();
SQL_str row;
SQLCURSOR cur;
SQL_str row1;
SQLCURSOR cur1;
float procent_sk;
int kolstr;
float kolstr1=0.;
char naim[200];
double suma;
double itogo=0.;
double cena;
double kolih;
short vidnal;

FILE *ffuz;
char imaf[30];

sprintf(imaf,"shet%d.lst",getpid());

if((ffuz = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return(FALSE);
 }



fprintf(ffuz,"%s\n\n",organ);

//читаем код клиента
sprintf(strsql,"select kodkl from Restdok where god=%d and nomd='%s'",
data->gods->ravno(0),data->nomdoks->ravno(0));
if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
 fprintf(ffuz,"%s:%s\n",gettext("Код клиента"),row[0]);

fprintf(ffuz,"%s %d.%d.%d%s\n%s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ffuz,"\
-----------------------\n");

fprintf(ffuz,"\
Код |Наименование     |\n\
Ко-тво| Цена | Сумма  |\n");

fprintf(ffuz,"\
-----------------------\n");

for(int i=0; i < koldok; i++)
 {
  //читаем шапку документа
  sprintf(strsql,"select datd,ps from Restdok where god=%d and nomd='%s'",
  data->gods->ravno(i),data->nomdoks->ravno(i));
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    repl.new_plus(gettext("Не найден счёт"));
    repl.plus(data->nomdoks->ravno(i));
    repl.plus(" ");
    repl.plus(data->gods->ravno(i));
    iceb_menu_soob(&repl,data->window);
    continue;
   }
  procent_sk=atof(row[1]);

  sprintf(strsql,"%s=%f\n",gettext("Процента скидки"),procent_sk);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    
  sprintf(strsql,"select mz,kodz,kolih,cena,nds from Restdok1 where datd='%s' \
and nomd='%s'",row[0],data->nomdoks->ravno(i));

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  sprintf(strsql,"%d %s\n",data->gods->ravno(i),data->nomdoks->ravno(i));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  kolstr1=0.;
  while(cur.read_cursor(&row) != 0)
   {
//    sprintf(strsql,"%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
//    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    
    iceb_pbar(data->bar,kolstr,++kolstr1);    


    if(atoi(row[0]) == 0) //товар
     sprintf(strsql,"select naimat from Material where kodm=%s",row[1]);
    else        //услуга
     sprintf(strsql,"select naius from Uslugi where kodus=%s",row[1]);

    memset(naim,'\0',sizeof(naim));

    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     strncpy(naim,row1[0],sizeof(naim)-1);

    sprintf(strsql,"%s %s\n",row[1],naim);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    
    cena=atof(row[3]);
    cena=cena-cena*procent_sk/100.;
    cena=iceb_u_okrug(cena,0.01);

//    sprintf(strsql,"cena=%f\n",cena);    
//    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    kolih=atof(row[2]);
    kolih=iceb_u_okrug(kolih,0.001);
    
    suma=cena*kolih;
    suma=iceb_u_okrug(suma,0.01);
    itogo+=suma;
    
    vidnal=atoi(row[4]);
    
    fprintf(ffuz,"%-4s %-18.18s\n",row[1],naim);  

    if(strlen(naim) > 18)
     fprintf(ffuz,"%s\n",&naim[18]);

    fprintf(ffuz,"%6.10g %6.2f %8.2f\n",kolih,cena,suma);  
    fprintf(ffuz,"\
-----------------------\n");



   }
 }

fprintf(ffuz,"%13s:%8.2f\n",gettext("К оплате"),itogo);
iceb_podpis(ffuz,data->window);
fclose(ffuz);

iceb_print(imaf,data->window);
unlink(imaf);


data->voz=0;
data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Печать закончена"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);
}

