/*$Id: prov_vv_r.c,v 1.6 2011-02-21 07:36:21 sasa Exp $*/
/*07.01.2007	07.01.2007	Белых А.И.	prov_vv_r.c
Проверка возможности входа/выхода в подразделение
*/
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include "i_rest.h"


class prov_vv_r_data
 {
  public:
  
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  int    kolstr;  //Количество строк в курсоре

  int podr;
  int nomer_tur;
  char kod_kart[50];
  int vhod_vihod; //1-вход 2-выход

  short voz;
  
  prov_vv_r_data()
   {
    voz=0;
    memset(kod_kart,'\0',sizeof(kod_kart));
   }
   
 };

gboolean   prov_vv_r_key_press(GtkWidget *widget,GdkEventKey *event,class prov_vv_r_data *data);
gint prov_vv_r1(class prov_vv_r_data *data);
void  prov_vv_r_v_knopka(GtkWidget *widget,class prov_vv_r_data *data);

void  itog_skl(char *sklz,double *itogskl,FILE *ff,GtkWidget *view,GtkTextBuffer *buffer);
void  itog_podr(char *podrz,double *itogop,FILE *ff,GtkWidget *view,GtkTextBuffer *buffer);
void prov_vv_sap(class prov_vv_r_data *data,int,FILE *ff);
void  prov_vv_r_razuz(iceb_u_spisok *,iceb_u_double *,iceb_u_spisok *,iceb_u_double *,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;
extern char *organ;
extern uid_t kod_operatora;

extern char *host;
extern char *imabaz;
extern char *parol;

void  prov_vv_r(int podr,int nomer_tur,const char *kod_kart,
int vhod_vihod, //1-вход 2-выход
GtkWidget *wpredok)
{

iceb_u_str soob;
char strsql[300];
class prov_vv_r_data data;
data.podr=podr;
data.nomer_tur=nomer_tur;
data.vhod_vihod=vhod_vihod;
strncpy(data.kod_kart,kod_kart,sizeof(data.kod_kart)-1);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_default_size (GTK_WINDOW  (data.window),500,-1);

if(data.vhod_vihod == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Проверка возможности входа"));
if(data.vhod_vihod == 2)
  sprintf(strsql,"%s %s",name_system,gettext("Проверка возможности выхода"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(prov_vv_r_key_press),&data);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));
gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


soob.new_plus(strsql);

if(data.vhod_vihod == 1)
  sprintf(strsql,"%s",gettext("Проверка возможности входа"));
if(data.vhod_vihod == 2)
  sprintf(strsql,"%s",gettext("Проверка возможности выхода"));


soob.new_plus(strsql);

soob.ps_plus(gettext("Код клиента"));
soob.plus(":");
soob.plus(data.kod_kart);

soob.ps_plus(gettext("Код подразделения"));
soob.plus(":");
soob.plus(data.podr);

GtkWidget *label=gtk_label_new(soob.ravno_toutf());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);


data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),400,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
PangoFontDescription *font_pango=pango_font_description_from_string("Nimbus Mono L, Bold 15");
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(prov_vv_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна


gtk_idle_add((GtkFunction)prov_vv_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));




}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  prov_vv_r_v_knopka(GtkWidget *widget,class prov_vv_r_data *data)
{
// printf("prov_vv_r_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   prov_vv_r_key_press(GtkWidget *widget,GdkEventKey *event,class prov_vv_r_data *data)
{
// printf("prov_vv_r_key_press\n");

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

gint prov_vv_r1(class prov_vv_r_data *data)
{
char imaf[50];
sprintf(imaf,"prov_vv%d.tmp",getpid());
char strsql[1000];

sprintf(strsql,"i_kontr_vv %s %s %s %d %d %s > %s",
imabaz,host,parol,data->vhod_vihod,data->nomer_tur,data->kod_kart,imaf);

system(strsql);

FILE *ff;

if((ff = fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_label_set_text(GTK_LABEL(data->label),gettext("Проверка закончена"));
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);

   return(FALSE);
 }
unlink(imaf);

while(fgets(strsql,sizeof(strsql),ff) != NULL)
 {
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
 }


fclose(ff);



gtk_label_set_text(GTK_LABEL(data->label),gettext("Проверка закончена"));
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);

return(FALSE);

}
