/*$Id: rest_saldo_r.c,v 1.17 2011-02-21 07:36:21 sasa Exp $*/
/*11.01.2007	26.12.2004	Белых А.И.	rest_saldo_r.c
Перенос сальдо
*/
#include <stdlib.h>
#include <unistd.h>
#include "i_rest.h"

class rest_saldo_r_data
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

  rest_saldo_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rest_saldo_r_key_press(GtkWidget *widget,GdkEventKey *event,class rest_saldo_r_data *data);
gint rest_saldo_r1(class rest_saldo_r_data *data);
void  rest_saldo_r_v_knopka(GtkWidget *widget,class rest_saldo_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern iceb_u_str shrift_ravnohir;
extern char *imabaz;
extern char *organ;
extern short start_god_rest;

int rest_saldo_r(GtkWidget *wpredok)
{
char strsql[500];
sprintf(strsql,"Перенести сальдо с %dг. на %dг. ? Вы уверены ?",start_god_rest,start_god_rest+1);
if(iceb_menu_danet(strsql,2,wpredok) == 2)
 return(1);


iceb_u_str repl;
class rest_saldo_r_data data;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Перенос сальдо"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rest_saldo_r_key_press),&data);

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

repl.plus(gettext("Перенос сальдо"));
sprintf(strsql,"%s:%s %s",gettext("База данных"),imabaz,organ);
repl.ps_plus(strsql);
repl.ps_plus(gettext("Стартовый год"));
repl.plus(":");
repl.plus(start_god_rest);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(rest_saldo_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)rest_saldo_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rest_saldo_r_v_knopka(GtkWidget *widget,class rest_saldo_r_data *data)
{
printf("rest_saldo_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rest_saldo_r_key_press(GtkWidget *widget,GdkEventKey *event,class rest_saldo_r_data *data)
{
 printf("rest_saldo_r_key_press\n");
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

gint rest_saldo_r1(class rest_saldo_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1000];
iceb_u_str repl;
iceb_clock sss(data->window);




SQL_str row;
SQLCURSOR cur;

iceb_u_int spisok_kas;
iceb_u_spisok spisok_kodkl;

sprintf(strsql,"select distinct nk from Restkas where datz >= '%04d-01-01' and \
datz <= '%04d-12-31'",start_god_rest,start_god_rest);
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
 spisok_kas.plus(atoi(row[0])); 

sprintf(strsql,"select distinct kodkl from Restkas where datz >= '%04d-01-01' and \
datz <= '%04d-12-31'",start_god_rest,start_god_rest);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
 spisok_kodkl.plus(row[0]); 

iceb_u_double suma_kas;
iceb_u_double suma_kodkl;
iceb_u_double suma_kodkl_po_kase;

suma_kas.make_class(spisok_kas.kolih());
suma_kodkl.make_class(spisok_kodkl.kolih()); //Общее сальдо по клиенту
suma_kodkl_po_kase.make_class(spisok_kodkl.kolih()); //Сальдо по кассе по клиенту


sprintf(strsql,"select kodkl,nk,kodz,suma from Restkas where datz >= '%04d-01-01' and \
datz <= '%04d-12-31'",start_god_rest,start_god_rest);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int nk;
int kodz;
double suma;
int nomer;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  nk=atoi(row[1]);
  kodz=atoi(row[2]);
  suma=atof(row[3]);
  
  if(kodz == 0)
   {
    nomer=spisok_kas.find(nk);
    suma_kas.plus(suma,nomer);
   }  
  if(row[0][0] != '\0')
   {
    nomer=spisok_kodkl.find(row[0]);
    suma_kodkl.plus(suma,nomer);  
    if(kodz == 0 || kodz == 2)
     suma_kodkl_po_kase.plus(suma,nomer);    
   }
 }
//Если по налу отрицательное то его обнуляем. 
//Считаем что сначала деньги списываются с нала, а всё остальное с безнала
for(int ii=0; ii < suma_kodkl_po_kase.kolih(); ii++)
 if(suma_kodkl_po_kase.ravno(ii) < 0.)
  suma_kodkl_po_kase.new_plus(0.,ii);
  
//Удаляем стартовые сальдо если они есть
sprintf(strsql,"delete from Restkas where datz >= '%04d-01-01' and \
datz <= '%04d-12-31' and nomd='000'",start_god_rest+1,start_god_rest+1);

iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s\n",gettext("Сальдо по кассам"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

//Записываем сальдо по кассам
for(int i=0; i < spisok_kas.kolih() ; i++)
 {

  sprintf(strsql,"%-5d %.2f\n",spisok_kas.ravno(i),suma_kas.ravno(i));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  if(suma_kas.ravno(i) == 0.)
   continue;

  sprintf(strsql,"insert into Restkas (kodkl,nk,datz,vrem,kodz,suma,nomd,ktoz) \
values('%s',%d,'%04d-%02d-%02d','%s',%d,%.2f,'%s',%d)",
  "",spisok_kas.ravno(i),start_god_rest+1,1,1,"00:00:00",0,suma_kas.ravno(i),"000",iceb_getuid(data->window));

  iceb_sql_zapis(strsql,1,0,data->window);
 }

sprintf(strsql,"\n%s\n",gettext("Сальдо по клиентам"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

char naimkl[50];
//Записываем сальдо по клиентам
for(int i=0; i < spisok_kodkl.kolih() ; i++)
 {
  memset(naimkl,'\0',sizeof(naimkl)); 
  sprintf(strsql,"select fio from Taxiklient where kod='%s'",spisok_kodkl.ravno(i));
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   strncpy(naimkl,row[0],sizeof(naimkl)-1);
  
  sprintf(strsql,"%-5s %-30.30s %.2f\n",spisok_kodkl.ravno(i),naimkl,suma_kodkl.ravno(i));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


  if(suma_kodkl.ravno(i) == 0.)
   continue;

  //Записываем сальдо по безналичым деньгам   
  if(suma_kodkl.ravno(i)-suma_kodkl_po_kase.ravno(i) != 0.)
   sprintf(strsql,"insert into Restkas (kodkl,nk,datz,vrem,kodz,suma,nomd,ktoz) \
values('%s',%d,'%04d-%02d-%02d','%s',%d,%.2f,'%s',%d)",
   spisok_kodkl.ravno(i),0,start_god_rest+1,1,1,"00:00:00",1,
   suma_kodkl.ravno(i)-suma_kodkl_po_kase.ravno(i),
   "000",iceb_getuid(data->window));

  //Записываем сальдо по наличным деньгам   
  if(suma_kodkl_po_kase.ravno(i) != 0.)
   sprintf(strsql,"insert into Restkas (kodkl,nk,datz,vrem,kodz,suma,nomd,ktoz) \
values('%s',%d,'%04d-%02d-%02d','%s',%d,%.2f,'%s',%d)",
   spisok_kodkl.ravno(i),0,start_god_rest+1,1,1,"00:00:00",0,
   suma_kodkl_po_kase.ravno(i),
   "000",iceb_getuid(data->window));

  iceb_sql_zapis(strsql,1,0,data->window);
 }
 




data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;


iceb_u_str imafn("restnast.alx") ;
//iceb_imafn("restnast.alx",&imafn);

repl.new_plus(gettext("Теперь нужно установить новый стартовый год в файле настройки"));
repl.ps_plus(imafn.ravno());
iceb_menu_soob(&repl,data->window);
return(FALSE);

}
