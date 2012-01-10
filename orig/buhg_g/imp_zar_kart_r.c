/*$Id: imp_zar_kart_r.c,v 1.10 2011-02-21 07:35:52 sasa Exp $*/
/*02.09.2007	24.10.2006	Белых А.И.	imp_zar_kart_r.c
импорт карточек из файла
*/
#include  <stdlib.h>
#include  <math.h>
#include  <errno.h>
#include  <unistd.h>
#include        <sys/stat.h>
#include "buhg_g.h"

class imp_zar_kart_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
    
  class iceb_u_str imafz;
        
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчет завершен 1-нет
  imp_zar_kart_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   imp_zar_kart_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_zar_kart_r_data *data);
gint imp_zar_kart_r1(class imp_zar_kart_r_data *data);
void  imp_zar_kart_r_v_knopka(GtkWidget *widget,class imp_zar_kart_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern double okrcn;
extern iceb_u_str shrift_ravnohir;

int imp_zar_kart_r(const char *imaf,
GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class imp_zar_kart_r_data data;

data.imafz.new_plus(imaf);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
sprintf(strsql,"%s %s",name_system,gettext("Импорт карточек из файла"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(imp_zar_kart_r_key_press),&data);

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

GtkWidget *label=NULL;

label=gtk_label_new(gettext("Импорт карточек из файла"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(imp_zar_kart_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)imp_zar_kart_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  imp_zar_kart_r_v_knopka(GtkWidget *widget,class imp_zar_kart_r_data *data)
{
printf("imp_zar_kart_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   imp_zar_kart_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_zar_kart_r_data *data)
{
 printf("imp_zar_kart_r_key_press\n");
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

gint imp_zar_kart_r1(class imp_zar_kart_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_clock sss(data->window);
struct stat work;
//class ZARP     zp;





if(stat(data->imafz.ravno(),&work) != 0)
 {
  
  sprintf(strsql,"%s %s !",gettext("Не найден файл"),data->imafz.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }



FILE *ff;

if((ff = fopen(data->imafz.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->imafz.ravno(),"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafprom[50];
FILE *ffprom;
sprintf(imafprom,"prom%d.tmp",getpid());
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  iceb_er_op_fil(imafprom,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char tabnom[20];
char fam[512];
char imq[40];
char oth[40];
char inn[20];
char fio[512];

int  kolkart=0;
float razmer=0.;
char strok[1024];
int i=0;
SQL_str row;
class SQLCURSOR cur;

while(fgets(strok,sizeof(strok),ff) != NULL)
 {
  i=strlen(strok);
  razmer+=i;
  if(strok[i-2] == '\r') //Возврат каретки меняем на пробел
   strok[i-2]=' ';

  //printw("%s",strok);
  iceb_printw(iceb_u_toutf(strok),data->buffer,data->view);
 
  iceb_pbar(data->bar,work.st_size,razmer);    
  //strzag(LINES-1,0,work.st_size,razmer);

  if(iceb_u_polen(strok,tabnom,sizeof(tabnom),1,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }

  if(atoi(tabnom) == 0)
   {
    iceb_menu_soob(gettext("Табельный номер не может быть равен нолю !"),data->window);
    fprintf(ffprom,"%s",strok);
    continue;
   }

  //Проверяем на отсутсвие такого табельного номера в базе данных
  sprintf(strsql,"select fio from Kartb where tabn=%s",tabnom);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
   {
    class iceb_u_str repl;
    
    sprintf(strsql,gettext("Табельный номер %s уже введен !"),tabnom);
    repl.plus(strsql);
    repl.ps_plus(row[0]);
    iceb_menu_soob(&repl,data->window);
    fprintf(ffprom,"%s\n%s\n",strok,strsql);
    continue;
   } 

  if(iceb_u_polen(strok,fam,sizeof(fam),2,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }

  if(iceb_u_polen(strok,imq,sizeof(imq),3,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }

  if(iceb_u_polen(strok,oth,sizeof(oth),4,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }

  if(iceb_u_polen(strok,inn,sizeof(inn),5,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }

  //Проверяем на отсутствие индивидуального налогового номера
  if(inn[0] != 0)
   {
    sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",inn);
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
     {
      class iceb_u_str repl;
      repl.plus(gettext("Такой идентификационный код уже введён !"));

      sprintf(strsql,"%s %s",row[0],row[1]);
      repl.ps_plus(strsql);

      fprintf(ffprom,"%s\n%s:%s\n",strok,
      gettext("Такой индентификационный код уже введен !"),
      strsql);
      continue;
     } 
   }

  sprintf(fio,"%s %s %s",fam,imq,oth);
  sqlfiltr(fio,sizeof(fio));
    
  sprintf(strsql,"insert into Kartb set tabn=%s, fio='%s', inn='%s'",tabnom,fio,inn);
  
  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
    continue;
  kolkart++;  
 }

fclose(ff);
fclose(ffprom);
unlink(data->imafz.ravno());

rename(imafprom,data->imafz.ravno());


class iceb_u_str repl;

repl.new_plus(gettext("Загрузка завершена"));
repl.ps_plus(gettext("Количество загруженых карточек"));
repl.plus(":");
repl.plus(kolkart);

iceb_menu_soob(&repl,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);
}
