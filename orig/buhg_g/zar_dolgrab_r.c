/*$Id: zar_dolgrab_r.c,v 1.11 2011-02-21 07:35:59 sasa Exp $*/
/*19.11.2009	05.12.2006	Белых А.И.	zar_dolgrab_r.c
Расчёт свода начислений/удержаний по категориям
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"

class zar_dolgrab_r_data
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

  zar_dolgrab_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_dolgrab_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_dolgrab_r_data *data);
gint zar_dolgrab_r1(class zar_dolgrab_r_data *data);
void  zar_dolgrab_r_v_knopka(GtkWidget *widget,class zar_dolgrab_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_dolgrab_r(const char *data_d,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_dolgrab_r_data data;

data.data_d=data_d;
data.imaf=imaf;
data.naimf=naimf;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать долги работников за месяц"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_dolgrab_r_key_press),&data);

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

repl.plus(gettext("Распечатать долги работников за месяц"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_dolgrab_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_dolgrab_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_dolgrab_r_v_knopka(GtkWidget *widget,class zar_dolgrab_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_dolgrab_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_dolgrab_r_data *data)
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

gint zar_dolgrab_r1(class zar_dolgrab_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short mr,gr;
iceb_u_rsdat1(&mr,&gr,data->data_d);


char imaf[54];
FILE *kaw;

sprintf(imaf,"spidol%d.lst",getpid());

if((kaw = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return(FALSE);
 }

iceb_u_zagolov(gettext("Список должников"),0,0,0,0,mr,gr,organ,kaw);



fprintf(kaw,"\
----------------------------------------------------\n");
sprintf(strsql,"select tabn from Zarn where god=%d and mes=%d",
gr,mr);
int kolstr=0;
SQLCURSOR cur;
SQLCURSOR curr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  return(FALSE);
 }

class SQLCURSOR cur1;

int kol=0;
double itogb=0.,itognb=0.,itogo=0.,sum=0.;
float kolstr1=0;
int tabb=0;
SQL_str row,row1;
double sald=0.,saldb=0.;
int kolstr2=0;
double sumnb,sumb;
double sm=0.;
char fio[512];
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  tabb=atoi(row[0]);

  sum=0.;
  /*Читаем сальдо*/
  sum=sald=zarsaldw(1,mr,gr,tabb,&saldb,data->window);

  sprintf(strsql,"select suma,prn,shet from Zarp where datz >= '%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%d",
  gr,mr,gr,mr,tabb);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  sumnb=sald-saldb;
  sumb=saldb;

  while(cur1.read_cursor(&row1) != 0)
   {
    sm=atof(row1[0]);
    sum+=sm;
    if(shetbu != NULL && shetb != NULL)
     {
      if(row1[1][0] == '1')
       {
       if(iceb_u_proverka(shetb,row1[2],0,0) == 0)
        sumb+=sm;
       else
        sumnb+=sm;
       }
      if(row1[1][0] == '2')
       {
       if(iceb_u_proverka(shetbu,row1[2],0,0) == 0)
        sumb+=sm;
       else
        sumnb+=sm;
       }
     }
   }
   
  memset(fio,'\0',sizeof(fio));
  if(sum < -0.009 && shetbu == NULL) 
   {
    memset(fio,'\0',sizeof(fio));
   
    sprintf(strsql,"select fio from Kartb where tabn=%d",tabb);
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
      strncpy(fio,row1[0],sizeof(fio)-1);
    sprintf(strsql,"%-5d %-*s %15s\n",tabb,iceb_u_kolbait(30,fio),fio,iceb_u_prnbr(sum));
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    fprintf(kaw,"%-5d %-*s %15s\n",tabb,iceb_u_kolbait(30,fio),fio,iceb_u_prnbr(sum));
    itogo+=sum;
    kol++;
   }
  if(shetbu != NULL && (sumb < -0.009 || sumnb < -0.009 ))
   {
    memset(fio,'\0',sizeof(fio));
    sprintf(strsql,"select fio from Kartb where tabn=%d",tabb);

    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
      strncpy(fio,row1[0],sizeof(fio)-1);
    if( sumb < -0.009)
     {
      sprintf(strsql,"%-5d %-*s %15s %s\n",tabb,iceb_u_kolbait(30,fio),fio,iceb_u_prnbr(sumb),gettext("Бюджет"));
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
      fprintf(kaw,"%-5d %-*s %15s %s\n",tabb,iceb_u_kolbait(30,fio),fio,iceb_u_prnbr(sumb),gettext("Бюджет"));
      itogb+=sumb;
     }
    if( sumnb < -0.009)
     {
      sprintf(strsql,"%-5d %-*s %15s\n",tabb,iceb_u_kolbait(30,fio),fio,iceb_u_prnbr(sumnb));
      
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
      
      fprintf(kaw,"%-5d %-*s %15s\n",tabb,iceb_u_kolbait(30,fio),fio,iceb_u_prnbr(sumnb));
      itognb+=sumnb;
     }
    kol++;

   }
 }

if(kol == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного должника !"),data->window);
 }
else
 {
  sprintf(strsql,"%s:%d\n",gettext("Количество должников"),kol);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  fprintf(kaw,"\
----------------------------------------------------\n");
  if(shetbu == NULL)
   {
    sprintf(strsql,"%5s %*s:%15s\n"," ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(itogo));
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    fprintf(kaw,"%5s %*s:%15s\n"," ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(itogo));
   }
  else
   {
    if(itogb != 0.)
     {
      sprintf(strsql,"%5s %*s:%15s\n"," ",iceb_u_kolbait(30,gettext("Итого бюджет")),gettext("Итого бюджет"),iceb_u_prnbr(itogb));
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
      fprintf(kaw,"%5s %*s:%15s\n"," ",iceb_u_kolbait(30,gettext("Итого бюджет")),gettext("Итого бюджет"),iceb_u_prnbr(itogb));
     }
    if(itognb != 0.)
     {
      sprintf(strsql,"%5s %*s:%15s\n"," ",iceb_u_kolbait(30,gettext("Итого хозрасчёт")),gettext("Итого хозрасчёт"),iceb_u_prnbr(itognb));
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
      fprintf(kaw,"%5s %*s:%15s\n"," ",iceb_u_kolbait(30,gettext("Итого хозрасчёт")),gettext("Итого хозрасчёт"),iceb_u_prnbr(itognb));
     }
   }
  fprintf(kaw,"%s:%d\n",gettext("Количество должников"),kol);


 }
iceb_podpis(kaw,data->window);
fclose(kaw);

if(kol != 0)
 {
  data->voz=0;
  data->imaf->plus(imaf);
  data->naimf->plus(gettext("Список должников"));
  iceb_ustpeh(imaf,0,data->window);
 }
else
 {

  data->voz=1;
  unlink(imaf);
 }






data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);

}
