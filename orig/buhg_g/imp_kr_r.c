/*$Id: imp_kr_r.c,v 1.11 2011-02-21 07:35:52 sasa Exp $*/
/*29.09.2009	09.10.2006	Белых А.И.	imp_kr_r.c
импорт начислений/удержаний из подсистемы Учёт командировочных расходов
*/
#include  <stdlib.h>
#include  <math.h>
#include  <errno.h>
#include  <unistd.h>
//#include        <sys/stat.h>
#include "buhg_g.h"
#include "imp_kr.h"

class imp_kr_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
    
  class imp_kr_rek *rk;
        
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчет завершен 1-нет
  imp_kr_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   imp_kr_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_kr_r_data *data);
gint imp_kr_r1(class imp_kr_r_data *data);
void  imp_kr_r_v_knopka(GtkWidget *widget,class imp_kr_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern double okrcn;
extern iceb_u_str shrift_ravnohir;

int imp_kr_r(class imp_kr_rek *rek,
GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class imp_kr_r_data data;

data.rk=rek;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Импорт документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(imp_kr_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Импорт документов"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(imp_kr_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)imp_kr_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  imp_kr_r_v_knopka(GtkWidget *widget,class imp_kr_r_data *data)
{
printf("imp_kr_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   imp_kr_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_kr_r_data *data)
{
 printf("imp_kr_r_key_press\n");
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

gint imp_kr_r1(class imp_kr_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_clock sss(data->window);


short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);

 }

char prist_k_tn[32];

if(iceb_poldan("Приставка к табельному номеру",prist_k_tn,"zarnast.alx",data->window) != 0)
 {
  iceb_menu_soob(gettext("Не найдена \"Приставка к табельному номеру\" !"),data->window);
  
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int dlina_prist=strlen(prist_k_tn);


SQL_str row,row1;
SQLCURSOR cur,cur1;
int kolstr=0;

sprintf(strsql,"select nomd,datd,kont from Ukrdok where \
datao >= '%04d-%02d-%02d' and datao <= '%04d-%02d-%02d'",
gn,mn,dn,gk,mk,dk);


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
//Смотрим есть ли счёт в начислении
char shet_nah[20];
memset(shet_nah,'\0',sizeof(shet_nah));

sprintf(strsql,"select shet from Nash where kod=%d",data->rk->kod_nah_ud);
if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
 strncpy(shet_nah,row1[0],sizeof(shet_nah)); 

char tabnom[20];
short dnr,mnr,gnr;
short dkr,mkr,gkr;
char shetkar[30];
int podrkar;
int kateg;
int sovm;
int zvan;
char lgot[50];
char dolg[512];
class ZARP     zp;



int nz=0;
int vernuli;
double suma=0;

char shet[32];

strncpy(shet,shet_nah,sizeof(shet)-1);
int tabn;
int kolstr2=0;
float kolstr1=0.;

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s\n",row[0],row[1],row[2]);

  iceb_pbar(data->bar,kolstr,++kolstr1);    
    
  if(iceb_u_strstrm(row[2],prist_k_tn) != 1)
   continue;
   
  memset(tabnom,'\0',sizeof(tabnom));
  strncpy(tabnom,&row[2][dlina_prist],sizeof(tabnom));
  tabn=atol(tabnom);
  
  sprintf(strsql,"tabnom=%s\n",tabnom);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  
  //Проверяем есть ли такой табельный номер  
  sprintf(strsql,"select datn,datk,shet,podr,kodg,kateg,\
sovm,zvan,lgoti,dolg from Kartb where tabn=%d",tabn);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    sprintf(strsql,"%s %d !\n",gettext("Не найден табельный номер"),tabn);
    iceb_menu_soob(strsql,data->window);
    continue;
   }

  iceb_u_rsdat(&dnr,&mnr,&gnr,row1[0],2);
  iceb_u_rsdat(&dkr,&mkr,&gkr,row1[1],2);
  strncpy(shetkar,row1[2],sizeof(shetkar)-1);

  podrkar=atoi(row1[3]);
  kateg=atoi(row1[5]);
  sovm=atoi(row1[6]);
  zvan=atoi(row1[7]);
  memset(lgot,'\0',sizeof(lgot));
  strncpy(lgot,row1[8],sizeof(lgot)-1);
  memset(dolg,'\0',sizeof(dolg));
  strncpy(dolg,row1[9],sizeof(dolg)-1);

  if(shet_nah[0] == '\0')
   strncpy(shet,shetkar,sizeof(shet)-1);
   

  sprintf(strsql,"select kodr,cena from Ukrdok1 where \
datd='%s' and nomd='%s'",row[1],row[0]);
//  printw("strsql=%s\n",strsql);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  if(kolstr2 == 0)
   continue;

  while(cur1.read_cursor(&row1) != 0)
   {

    if(iceb_u_proverka(data->rk->kod_zat.ravno(),row1[0],0,0) != 0)
     continue;
//    printw("%s %s\n",row1[0],row1[1]);
    suma=atof(row1[1]);
    if(data->rk->prn == 2)
     suma*=-1;    
    nz=0;

    zp.tabnom=tabn;
    zp.prn=data->rk->prn;
    zp.knu=data->rk->kod_nah_ud;
    zp.dz=0;   //Если день равен нулю, то удаление старой записи перед вставкой новой не происходит
    zp.mz=data->rk->mz;
    zp.gz=data->rk->gz;
    zp.mesn=data->rk->mz; zp.godn=data->rk->gz;
    zp.nomz=0;
    zp.podr=podrkar;
    strcpy(zp.shet,shet);

povtor:;

    if((vernuli=zapzarpw(&zp,suma,data->rk->denz.ravno_atoi(),data->rk->mz,data->rk->gz,0,shet,"",nz++,podrkar,"",data->window)) == 1)
      goto povtor;

    if(vernuli != 0)
     continue;
     
    zapzarn1w(tabn,data->rk->prn,data->rk->kod_nah_ud,0,0,0,shet,data->window);

    zarsocw(data->rk->mz,data->rk->gz,tabn,NULL,data->window);
    zaravprw(tabn,data->rk->mz,data->rk->gz,NULL,data->window);

    //Проверяем есть ли запись карточки по этой дате и если нет делаем запись
    sprintf(strsql,"select god from Zarn where tabn=%d and god=%d and mes=%d",tabn,data->rk->gz,data->rk->mz);
    if(iceb_sql_readkey(strsql,data->window) == 0)  
      zapzarnw(data->rk->mz,data->rk->gz,podrkar,tabn,kateg,sovm,zvan,shetkar,lgot,dnr,mnr,gnr,dkr,mkr,gkr,0,dolg,data->window);
    
   }

 }




data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);



//repl.new_plus(gettext("Загрузка завершена"));
//repl.ps_plus(gettext("Количество документов"));
//repl.plus(":");
//repl.plus(koldok);

//iceb_menu_soob(&repl,data->window);


data->voz=0;

return(FALSE);
}
