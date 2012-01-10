/*$Id: saldokasw_r.c,v 1.6 2011-02-21 07:35:57 sasa Exp $*/
/*27.02.2009	27.02.2009	Белых А.И.	saldokasw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"

class saldokasw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  short god;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  saldokasw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   saldokasw_r_key_press(GtkWidget *widget,GdkEventKey *event,class saldokasw_r_data *data);
gint saldokasw_r1(class saldokasw_r_data *data);
void  saldokasw_r_v_knopka(GtkWidget *widget,class saldokasw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

int saldokasw_r(short gods,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class saldokasw_r_data data;

data.god=gods;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,iceb_u_toutf("Перенос сальдо для подсистемы \"Учёт кассовых ордеров\""));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(saldokasw_r_key_press),&data);

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

repl.plus("Перенос сальдо для подсистемы \"Учёт кассовых ордеров\"");
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(saldokasw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)saldokasw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  saldokasw_r_v_knopka(GtkWidget *widget,class saldokasw_r_data *data)
{
printf("saldokasw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   saldokasw_r_key_press(GtkWidget *widget,GdkEventKey *event,class saldokasw_r_data *data)
{
 printf("saldokasw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}


gint saldokasw_r1(class saldokasw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;






short dn=1;
short mn=1;
short gn=data->god-1;
short dk=31;
short mk=12;
short gk=data->god-1;


int kolstr=0;
SQL_str row1;

class SQLCURSOR cur1;

class iceb_u_spisok kas_kodcn; //Список код кассы-код целевого назначения
class iceb_u_double mscn; //массив сумм сальдо целевого назначения


//Читаем сальдо по кодам целевого назначения
sprintf(strsql,"select kod,ks,saldo from Kascnsl where god=%d",gn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s",row[1],row[0]);
  if((nomer=kas_kodcn.find(strsql)) < 0)
   kas_kodcn.plus(strsql);

  mscn.plus(atof(row[2]),nomer);
 }


//читаем документы

sprintf(strsql,"select kassa,tp,datd,nomd,kodop from Kasord where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by datd,tp asc",
gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
float kolstr1=0;
double suma=0.,sumapod=0.;
short d,m,g;
short tipz;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  tipz=atoi(row[1]);
  //читаем код целевого назначения этой операции
  if(row[1][0] == '1')
    sprintf(strsql,"select kcn from Kasop1 where kod='%s'",row[4]);
  if(row[1][0] == '2')
    sprintf(strsql,"select kcn from Kasop2 where kod='%s'",row[4]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(atoi(row1[0]) == 0)
     continue;
   }
  else 
   continue;
  sprintf(strsql,"%s|%s",row[0],row1[0]);  
  if((nomer=kas_kodcn.find(strsql)) < 0)
    kas_kodcn.plus(strsql);
    
  iceb_u_rsdat(&d,&m,&g,row[2],2);
  
  sumkasorw(row[0],tipz,row[3],g,&suma,&sumapod,data->window);
  
  sumapod*=-1; 
  mscn.plus(sumapod,nomer);

 }

//удаляем записи если они есть

sprintf(strsql,"delete from Kascnsl where god=%d",data->god);
iceb_sql_zapis(strsql,0,0,data->window);

char kassa[10];
char kodcn[10];
time_t vrem;
time(&vrem);

for(int ii=0; ii < kas_kodcn.kolih() ; ii++)
 {
  iceb_u_polen(kas_kodcn.ravno(ii),kassa,sizeof(kassa),1,'|');
  iceb_u_polen(kas_kodcn.ravno(ii),kodcn,sizeof(kodcn),2,'|');

  sprintf(strsql,"Kacca:%s Код:%s Сумма:%.2f\n",kassa,kodcn,mscn.ravno(ii));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
   
  sprintf(strsql,"insert into Kascnsl values (%d,%s,%s,%.2f,%d,%ld)",
  data->god,kodcn,kassa,mscn.ravno(ii),iceb_getuid(data->window),vrem);
  iceb_sql_zapis(strsql,0,0,data->window);

   
 }

iceb_printw(iceb_u_toutf("Перенос сальдо закончен\n"),data->buffer,data->view);





gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}









