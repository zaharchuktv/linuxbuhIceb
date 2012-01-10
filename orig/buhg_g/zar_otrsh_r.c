/*$Id: zar_otrsh_r.c,v 1.10 2011-02-21 07:35:59 sasa Exp $*/
/*20.11.2009	21.11.2006	Белых А.И.	zar_otrsh_r.c
Расчёт свода отработанного времени по категориям
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_otrsh.h"

class zar_otrsh_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_otrsh_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_otrsh_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_otrsh_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_otrsh_r_data *data);
gint zar_otrsh_r1(class zar_otrsh_r_data *data);
void  zar_otrsh_r_v_knopka(GtkWidget *widget,class zar_otrsh_r_data *data);

int otrshw(class zar_otrsh_rek *data,FILE *kaw,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);
void komplrasov(class zar_otrsh_rek *data,char imaf[],GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_otrsh_r(class zar_otrsh_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_otrsh_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчет свода отработанного времени по категориям"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_otrsh_r_key_press),&data);

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

repl.plus(gettext("Расчет свода отработанного времени по категориям"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_otrsh_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_otrsh_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_otrsh_r_v_knopka(GtkWidget *widget,class zar_otrsh_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_otrsh_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_otrsh_r_data *data)
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

gint zar_otrsh_r1(class zar_otrsh_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
class iceb_clock sss(data->window);


char imaf[56];

if(data->rk->metka_ras == 1)
 {
  sprintf(imaf,"ovk%d.lst",getpid());
  komplrasov(data->rk,imaf,data->view,data->buffer,data->window);
 }
else
 {
  sprintf(imaf,"ov%d.lst",getpid());
  FILE *kaw=NULL;
  if((kaw = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"",errno,data->window);
    data->kon_ras=0;
    return(FALSE);
   }

  otrshw(data->rk,kaw,data->view,data->buffer,data->window);
  iceb_podpis(kaw,data->window);
  fclose(kaw);
 }


data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчет свода отработанного времени по категориям"));

iceb_ustpeh(imaf,0,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/**********************************************************/
/*Комплексний расчет, данные для расчета беруться из файла*/
/**********************************************************/
void komplrasov(class zar_otrsh_rek *data,char imaf[],
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
char		bros[512];
short		metka;
FILE		*kaw;
char		kompl[512];
char		komplz[512];

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zarpodrv.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return;
 }


if((kaw = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

data->clear_nast();

metka=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],kompl,sizeof(kompl),1,':') == 0)
   {
    if(metka != 0)
     {
      if(metka > 1)
       fprintf(kaw,"\f");
      fprintf(kaw,"%s\n",komplz);       
      sprintf(bros,"\n%s\n",komplz);       
      iceb_printw(iceb_u_toutf(bros),buffer,view);

      otrshw(data,kaw,view,buffer,wpredok);

      data->clear_nast();
     }
    metka++;    
    strcpy(komplz,kompl);
   }

  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') == 0) 
   if(iceb_u_SRAV("Подразделения",bros,0) == 0)
    iceb_u_polen(row_alx[0],&data->podr,2,'|');

  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') == 0) 
   if(iceb_u_SRAV("Категории",bros,0) == 0)
    iceb_u_polen(row_alx[0],&data->kod_kat,2,'|');

  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') == 0) 
   if(iceb_u_SRAV("Коды табеля",bros,0) == 0)
    iceb_u_polen(row_alx[0],&data->kod_tab,2,'|');

  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') == 0) 
   if(iceb_u_SRAV("Табельные номера",bros,0) == 0)
    iceb_u_polen(row_alx[0],&data->tabnom,2,'|');

 }
if(metka > 1)
 fprintf(kaw,"\f");

sprintf(bros,"\n%s\n",komplz);       
iceb_printw(iceb_u_toutf(bros),buffer,view);

fprintf(kaw,"%s\n",komplz);

otrshw(data,kaw,view,buffer,wpredok);

iceb_podpis(kaw,wpredok);
fclose(kaw);

}
