/*$Id: zar_srninu_r.c,v 1.9 2011-02-21 07:36:00 sasa Exp $*/
/*20.11.2009	29.11.2006	Белых А.И.	zar_srninu_r.c
Расчёт свода начислений/удержаний по категориям
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_srninu.h"

class zar_srninu_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_srninu_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_srninu_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_srninu_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_srninu_r_data *data);
gint zar_srninu_r1(class zar_srninu_r_data *data);
void  zar_srninu_r_v_knopka(GtkWidget *widget,class zar_srninu_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_srninu_r(class zar_srninu_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_srninu_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать список работников не имеющих начисления/удержания"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_srninu_r_key_press),&data);

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

repl.plus(gettext("Распечатать список работников не имеющих начисления/удержания"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_srninu_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_srninu_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_srninu_r_v_knopka(GtkWidget *widget,class zar_srninu_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_srninu_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_srninu_r_data *data)
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

gint zar_srninu_r1(class zar_srninu_r_data *data)
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
if(mk == 0)
 {
  mk=mn;
  gk=gn;
 }


sprintf(strsql,"select tabn,fio,podr from Kartb order by tabn asc");
class SQLCURSOR cur;
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
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);  
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf[56];
sprintf(imaf,"sn%d.lst",getpid());
FILE *kaw;
if((kaw = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short dk=0;
iceb_u_dpm(&dk,&mk,&gk,5);
iceb_u_startfil(kaw);
iceb_u_zagolov(gettext("Список работников неимеющих начисление/удержание"),1,mn,gn,dk,mk,gk,organ,kaw);

int klst=0;
if(data->rk->podr.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Код подраздедения"),data->rk->podr.ravno());
  iceb_printcod(kaw,"Podr","kod","naik",0,data->rk->podr.ravno(),&klst);
 }
else
 fprintf(kaw,"%s\n",gettext("По всем подразделениям"));

if(data->rk->kod_nah.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Код начисления"),data->rk->kod_nah.ravno());
  iceb_printcod(kaw,"Nash","kod","naik",0,data->rk->kod_nah.ravno(),&klst);
 }
if(data->rk->kod_ud.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Код удержания"),data->rk->kod_ud.ravno());
  iceb_printcod(kaw,"Uder","kod","naik",0,data->rk->kod_ud.ravno(),&klst);
 }

fprintf(kaw,"-----------------------------------------------------------------------\n");


int kolp=0;
sprintf(strsql,"----------------------------------------------\n");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
float kolstr1=0;
class SQLCURSOR cur1;
SQL_str row,row1;
int tabn;
char fio[512];
int kolstr2=0;
int metka=0;
int prn=0;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->podr.ravno(),row[2],0,0) != 0)
    continue;
  
  tabn=atol(row[0]);
  memset(fio,'\0',sizeof(fio));
  strncpy(fio,row[1],sizeof(fio)-1);
  
  sprintf(strsql,"select prn,knah from Zarp where \
datz >= '%d-%02d-01' and datz <= '%d-%02d-%02d' and tabn=%d",
  gn,mn,gk,mk,dk,tabn);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr2 == 0)
    continue;

  metka=0;
  while(cur1.read_cursor(&row1) != 0)
   {

    prn=atoi(row1[0]);

    if(prn == 1 && data->rk->kod_nah.ravno()[0] == '-')
       continue;
    if(prn == 1 && data->rk->kod_nah.ravno()[0] != '\0')
     if(iceb_u_proverka(data->rk->kod_nah.ravno(),row1[1],0,0) == 0)
   //  if(iceb_u_pole1(nahis,row1[1],',',0,&i1) == 0 || SRAV(nahis,row1[1],0) == 0)
      {
       metka++;        
       continue;
      }

    if(prn == 2 && data->rk->kod_ud.ravno()[0] == '-')
       continue;
       
    if(prn == 2 && data->rk->kod_ud.ravno()[0] != '\0')
     if(iceb_u_proverka(data->rk->kod_ud.ravno(),row1[1],0,0) == 0)
      {
       metka++;        
       continue;
      }
   }

  if(metka == 0)
   {
    sprintf(strsql,"%-6d %s\n",tabn,fio);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    fprintf(kaw,"%-7d %s\n",tabn,fio);
    kolp++;
   }
 }

fprintf(kaw,"-----------------------------------------------------------------------\n");
fprintf(kaw,"%s: %d",
gettext("Количество работников"),kolp);

sprintf(strsql,"----------------------------------------------\n");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s: %d",
gettext("Количество работников"),kolp);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

iceb_podpis(kaw,data->window);
fclose(kaw);

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Список работников неимеющих начисление/удержание"));
iceb_ustpeh(imaf,3,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
