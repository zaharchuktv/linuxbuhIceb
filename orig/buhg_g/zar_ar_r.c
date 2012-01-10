/*$Id: zar_ar_r.c,v 1.16 2011-04-14 16:09:36 sasa Exp $*/
/*10.03.2009	20.03.2006	Белых А.И.	zar_ar_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_ar.h"

class zar_ar_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_ar_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_ar_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_ar_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_ar_r_data *data);
gint zar_ar_r1(class zar_ar_r_data *data);
void  zar_ar_r_v_knopka(GtkWidget *widget,class zar_ar_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
//extern float dnei,hasov; //Количество рабочих дней и часов в определённом месяце. Читает программа redkalw.c перед началом расчёта начислений/удержаний

int zar_ar_r(class zar_ar_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_ar_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Автоматизированный расчёт зарплаты по всем карточкам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_ar_r_key_press),&data);

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

repl.plus(gettext("Автоматизированный расчёт зарплаты по всем карточкам"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_ar_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_ar_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_ar_r_v_knopka(GtkWidget *widget,class zar_ar_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_ar_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_ar_r_data *data)
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

gint zar_ar_r1(class zar_ar_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
short dk,mk,gk;
short dn,mn,gn;
short mr,gr;

iceb_u_rsdat1(&mr,&gr,data->rk->datar.ravno());


dk=1; mk=mr; gk=gr;
iceb_u_dpm(&dk,&mk,&gk,4); //Уменьшаем на месяц тоесть все уволенные до этого месяца в расчет не идут

zarreadnw(1,mr,gr,data->rk->metka_r,NULL,data->window); /*читаем текущие настройки*/

sprintf(strsql,"select tabn,fio,podr,sovm,kodg,kateg,zvan,\
shet,lgoti,datn,datk,dolg from Kartb where datk='0000-00-00' \
or datk > '%04d-%d-%d'",gk,mk,dk);

int kolstr=0;
class SQLCURSOR cur,cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
 
if(kolstr <= 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
SQL_str row,row1;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->podr.ravno(),row[2],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
    continue;

  sprintf(strsql,"%5s %s\n",row[0],row[1]);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  //Проверяем если карточка заблокирована то ее пропускаем
  sprintf(strsql,"select blok from Zarn where god=%d and mes=%d and \
tabn=%s",gr,mr,row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   if(atoi(row1[0]) != 0)
    {
     sprintf(strsql,"%s !\n",gettext("Карточка заблокирована"));
     iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
     continue;
    }

  int tabn=atoi(row[0]);
  int kpodr=atoi(row[2]);
  int sovm=atoi(row[3]);
  int kateg=atoi(row[5]);
  int zvan=atoi(row[6]);
  char shet[32];
  strncpy(shet,row[7],sizeof(shet)-1);
  char lgot[50];
  strncpy(lgot,row[8],sizeof(lgot)-1);
  
  /*Проверяем есть ли записи начислений, если их нет совсем
    проверяем есть ли набор начислений и удержаний на этот
    табельный номер
    Если набор есть загружаем его и считаем
    Если нет смотрим следующий табельный номер
   */
  if(data->rk->metka_r < 4)
   {  
    //Проверяем записана ли стандартная настройка
    provzzagw(mr,gr,tabn,3,kpodr,data->window);
      
    rasshetzw(tabn,mr,gr,kpodr,data->rk->metka_r,data->rk->nah_only.ravno(),data->rk->uder_only.ravno(),data->rk->ff,data->window); /*Расчет*/

    //Проверяем есть ли промежуточная запись
    sprintf(strsql,"select god from Zarn where tabn=%d and god=%d \
and mes=%d",tabn,gr,mr);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) < 1)
     {
      /*Проверяем есть ли начисления или удержания*/
      sprintf(strsql,"select tabn from Zarp where tabn=%d \
and suma <> 0. limit 1",tabn);  

      if(iceb_sql_readkey(strsql,data->window) > 0)
       {
        iceb_u_rsdat(&dn,&mn,&gn,row[9],2);
        iceb_u_rsdat(&dk,&mk,&gk,row[10],2);
        zapzarnw(mr,gr,kpodr,tabn,kateg,sovm,zvan,shet,lgot,dn,mn,gn,dk,mk,gk,0,row[11],data->window);
       }
     }
   }
  if(data->rk->metka_r < 5)
    zarsocw(mr,gr,tabn,NULL,data->window);
  
  zaravprw(tabn,mr,gr,NULL,data->window);

  //Проверяем сделаны ли все проводки
  double sald=0.;
  double nah,nahprov,uder,uderprov,nahb,uderb,uderbprov;
  nahudw(gr,mr,tabn,&nah,&nahprov,&uder,&uderprov,&nahb,&uderb,&uderbprov,0,sald,0,"",data->window);

  sprintf(strsql,"%d-%d",mr,tabn);
  
  double sumd,sumk;
  
  sumprzw(mr,gr,strsql,0,&sumd,&sumk,data->window);
  int prov;

  if(fabs((sumd+sumk)-(uderprov*(-1)+nahprov)) > 0.009 )
    prov=0;
  else
    prov=1;

  sprintf(strsql,"update Zarn set prov=%d where tabn=%d and god=%d \
and mes=%d",prov,tabn,gr,mr);

  iceb_sql_zapis(strsql,0,0,data->window);

    
 }


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}


