/*$Id: impotvrw_r.c,v 1.5 2011-02-21 07:35:52 sasa Exp $*/
/*17.03.2009	17.03.2009	Белых А.И.	impotvrw_r.c
импорт отработанного времени
*/
#include  <stdlib.h>
#include  <math.h>
#include  <errno.h>
#include  <unistd.h>
#include        <sys/stat.h>
#include "buhg_g.h"

class impotvrw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
    
  class impotvrw_rek *rk;
  const char *imaf_imp;
          
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчет завершен 1-нет
  impotvrw_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   impotvrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class impotvrw_r_data *data);
gint impotvrw_r1(class impotvrw_r_data *data);
void  impotvrw_r_v_knopka(GtkWidget *widget,class impotvrw_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern double okrcn;
extern iceb_u_str shrift_ravnohir;

int impotvrw_r(const char *imaf_imp,
GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class impotvrw_r_data data;

data.imaf_imp=imaf_imp;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Импорт отработанного времени"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(impotvrw_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Импорт отработанного времени"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(impotvrw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)impotvrw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  impotvrw_r_v_knopka(GtkWidget *widget,class impotvrw_r_data *data)
{
printf("impotvrw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   impotvrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class impotvrw_r_data *data)
{
 printf("impotvrw_r_key_press\n");
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

gint impotvrw_r1(class impotvrw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_clock sss(data->window);
struct stat work;
char strok[1024];
char bros[1024];
SQL_str row;
class SQLCURSOR cur;
if(stat(data->imaf_imp,&work) != 0)
 {
  
  sprintf(strsql,"%s %s !",gettext("Нет найден файл"),data->imaf_imp);
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }


FILE *ff=NULL;

if((ff = fopen(data->imaf_imp,"r")) == NULL)
 {
  iceb_er_op_fil(data->imaf_imp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafprom[50];
FILE *ffprom=NULL;

sprintf(imafprom,"prom%d.tmp",getpid());
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  iceb_er_op_fil(imafprom,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
float		mkolrd=0.; //Количество рабочих дней в месяце
float		mkolrh=0.;  //Количество рабочих часов в месяце
float kolrd=0.,kolrh=0.,kolkd=0.;
int kodtb=0;
short mes=0,god=0;
int razmer=0;
int i=0;
int tabnom=0;
while(fgets(strok,sizeof(strok),ff) != NULL)
 {
  i=strlen(strok);
  razmer+=i;
  if(strok[i-2] == '\r') //Возврат каретки меняем на пробел
   strok[i-2]=' ';

//  printw("%s",strok);
//  strzag(LINES-1,0,work.st_size,razmer);
  iceb_printw(iceb_u_toutf(strok),data->buffer,data->view);
  iceb_pbar(data->bar,work.st_size,razmer);    

  if(iceb_u_polen(strok,bros,sizeof(bros),1,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
  if(bros[0] == '"') /*Для Госстандарта*/
   bros[0]='0';
  if(bros[0] == '\0')
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
  if((tabnom=atoi(bros)) == 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
    
  sprintf(strsql,"select tabn from Kartb where tabn=%d",tabnom);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) <= 0)
   {

    sprintf(bros,"%s:%d",gettext("Не найден табельный номер"),tabnom);
    iceb_menu_soob(bros,data->window);

    fprintf(ffprom,"%s",strok);
    fprintf(ffprom,"%s:%d\n",\
    gettext("Не найден табельный номер"),tabnom);
    continue;
   }

  if(iceb_u_polen(strok,bros,sizeof(bros),2,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
  kodtb=atoi(bros);

  sprintf(strsql,"select kod from Tabel where kod=%d",kodtb);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) <= 0)
   {

    sprintf(bros,"%s:%d",gettext("Не найден код табеля"),kodtb);
    iceb_menu_soob(bros,data->window);

    fprintf(ffprom,"%s",strok);
    fprintf(ffprom,"%s:%d\n",\
    gettext("Не найден код табеля"),kodtb);
    continue;
   }

  sprintf(strsql,"select god from Ztab where tabn=%d and god=%d and \
mes=%d and kodt=%d and nomz=%d",tabnom,god,mes,kodtb,0);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
   {
    iceb_menu_soob(gettext("Такая запись уже есть !"),data->window);

    fprintf(ffprom,"%s",strok);
    fprintf(ffprom,"%s\n",gettext("Такая запись уже есть !"));
    continue;
   }

  //Проверяем блокировку карточки
  sprintf(strsql,"select blok from Zarn where god=%d and mes=%d and \
tabn=%d",god,mes,tabnom);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   if(atoi(row[0]) != 0)
    {
     sprintf(bros,"%s !",gettext("Карточка заблокирована"));
     iceb_menu_soob(bros,data->window);
     
     fprintf(ffprom,"%s",strok);
     fprintf(ffprom,"%s !\n",gettext("Карточка заблокирована"));
     continue;
    }
   

  if(iceb_u_polen(strok,bros,sizeof(bros),3,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
  kolrd=atof(bros);

  if(iceb_u_polen(strok,bros,sizeof(bros),4,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
  kolrh=atof(bros);

  if(iceb_u_polen(strok,bros,sizeof(bros),5,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
  kolkd=atof(bros);

  if(iceb_u_polen(strok,bros,sizeof(bros),6,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
  mkolrd=atof(bros);

  if(iceb_u_polen(strok,bros,sizeof(bros),7,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
  mkolrh=atof(bros);

  zaptabelw(0,mes,god,tabnom,kodtb,kolrd,\
  kolrh,kolkd,0,0,0,0,0,0,mkolrd,mkolrh,"",0,0,data->window);

 }

fclose(ff);
fclose(ffprom);
unlink(data->imaf_imp);

rename(imafprom,data->imaf_imp);

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
