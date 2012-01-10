/*$Id: zar_kateg_r.c,v 1.15 2011-02-21 07:35:59 sasa Exp $*/
/*19.11.2009	08.11.2006	Белых А.И.	zar_kateg_r.c
Расчёт свода начислений/удержаний по категориям
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_kateg.h"

class zar_kateg_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_kateg_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_kateg_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_kateg_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_kateg_r_data *data);
gint zar_kateg_r1(class zar_kateg_r_data *data);
void  zar_kateg_r_v_knopka(GtkWidget *widget,class zar_kateg_r_data *data);

int kategw(class zar_kateg_rek *data,FILE *kaw,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *bar,GtkWidget *wpredok);
int kategshw(class zar_kateg_rek *data,FILE *kaw,FILE *ffhoz,FILE *ffbu,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *bar,GtkWidget *wpredok);
void komplras(class zar_kateg_r_data *data,char imaf[],char *imafhoz,char *imafbu);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_kateg_r(class zar_kateg_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_kateg_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать свод начислений и удержаний по категориям"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_kateg_r_key_press),&data);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_kateg_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_kateg_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_kateg_r_v_knopka(GtkWidget *widget,class zar_kateg_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_kateg_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_kateg_r_data *data)
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

gint zar_kateg_r1(class zar_kateg_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
class iceb_clock sss(data->window);

printf("metka_ras=%d metka_vr=%d\n",data->rk->metka_ras,data->rk->metka_vr);

char imafhoz[56];
char imafbu[56];
char imaf[56];

memset(imafhoz,'\0',sizeof(imafhoz));
memset(imafhoz,'\0',sizeof(imafhoz));
memset(imaf,'\0',sizeof(imaf));

if(data->rk->metka_ras == 1)
 {
  sprintf(imaf,"kategk%d.lst",getpid());
  sprintf(imafhoz,"kategh%d.lst",getpid());
  sprintf(imafbu,"kategb%d.lst",getpid());
  komplras(data,imaf,imafhoz,imafbu);
 }
else
 {
  sprintf(imaf,"kateg%d.lst",getpid());
  FILE *kaw;
  if((kaw = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  if(data->rk->metka_vr == 0)
    kategw(data->rk,kaw,data->view,data->buffer,data->bar,data->window);
  if(data->rk->metka_vr == 1)
   {
    FILE *ffhoz=NULL;
    FILE *ffbu=NULL;
    if(shetb != NULL)
     {
      sprintf(imafhoz,"kategh%d.lst",getpid());
      if((ffhoz = fopen(imafhoz,"w")) == NULL)
       {
        iceb_er_op_fil(imafhoz,"",errno,data->window);
        sss.clear_data();
        gtk_widget_destroy(data->window);
        return(FALSE);
       }

      sprintf(imafbu,"kategb%d.lst",getpid());
      if((ffbu = fopen(imafbu,"w")) == NULL)
       {
        iceb_er_op_fil(imafbu,"",errno,data->window);
        sss.clear_data();
        gtk_widget_destroy(data->window);
        return(FALSE);
       }
     }

     kategshw(data->rk,kaw,ffhoz,ffbu,data->view,data->buffer,data->bar,data->window);
    
    if(shetb != NULL)
     {
      fclose(ffhoz);

      fclose(ffbu);
     }
   }

  fclose(kaw);
 }


data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт начислений и удержаний по категориям"));

if(shetb != NULL && data->rk->metka_ras == 1)
 {
  data->rk->imaf.plus(imafhoz);
  data->rk->naimf.plus(gettext("Расчёт начислений и удержаний по категориям (хозрасчёт)"));
  data->rk->imaf.plus(imafbu);
  data->rk->naimf.plus(gettext("Расчёт начислений и удержаний по категориям (бюджет)"));

 }

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
/**********************************************************/
/*Комплексний расчет, данные для расчета беруться из файла*/
/**********************************************************/
void komplras(class zar_kateg_r_data *data,
char imaf[],char *imafhoz,char *imafbu)
{
char		strsql[1024];
char		bros[1024];
short		metka;
FILE		*kaw=NULL,*ffhoz=NULL,*ffbu=NULL;
char		kompl[1024];
char		komplz[1024];

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
const char *imaf_alx={"zarpodrk.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,NULL);
  return;
 }


if((kaw = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }


if(shetb != NULL)
 {
  if((ffhoz = fopen(imafhoz,"w")) == NULL)
   {
    iceb_er_op_fil(imafhoz,"",errno,data->window);
    return;
   }

  if((ffbu = fopen(imafbu,"w")) == NULL)
   {
    iceb_er_op_fil(imafbu,"",errno,data->window);
    return;
   }
 }
data->rk->clear_nast();


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
      
      sprintf(strsql,"\n%s\n",komplz);       
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

      if(data->rk->metka_vr == 0)
       kategw(data->rk,kaw,data->view,data->buffer,data->bar,data->window);

      if(data->rk->metka_vr == 1)
       kategshw(data->rk,kaw,ffhoz,ffbu,data->view,data->buffer,data->bar,data->window);

      
      data->rk->clear_nast();
     }
    metka++;    
    strcpy(komplz,kompl);
   }

  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') == 0) 
   if(iceb_u_SRAV("Подразделения",bros,0) == 0)
     iceb_u_polen(row_alx[0],&data->rk->podr,2,'|');

  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') == 0) 
   if(iceb_u_SRAV("Категории",bros,0) == 0)
    iceb_u_polen(row_alx[0],&data->rk->kod_kat,2,'|');

  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') == 0) 
   if(iceb_u_SRAV("Начисления",bros,0) == 0)
    iceb_u_polen(row_alx[0],&data->rk->kod_nah,2,'|');

  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') == 0) 
   if(iceb_u_SRAV("Удержания",bros,0) == 0)
    iceb_u_polen(row_alx[0],&data->rk->kod_ud,2,'|');

 }
if(metka > 1)
 fprintf(kaw,"\f");

sprintf(strsql,"\n%s\n",komplz);       
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(kaw,"%s\n",komplz);

if(data->rk->metka_vr == 0)
 kategw(data->rk,kaw,data->view,data->buffer,data->bar,data->window);

if(data->rk->metka_vr == 1)
 kategshw(data->rk,kaw,ffhoz,ffbu,data->view,data->buffer,data->bar,data->window);


fclose(kaw);

if(shetb != NULL)
 {
  fclose(ffhoz);

  fclose(ffbu);
 }
}
