/*$Id: matuddokw_r.c,v 1.11 2011-02-21 07:35:55 sasa Exp $*/
/*09.05.2005	06.05.2005	Белых А.И. 	matuddokw_r.c
Удаление документов за год
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"

class matuddokw_r_data
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

  matuddokw_r_data()
   {
    god=0;  
    voz=1;
    kon_ras=1;
   }
 };
gboolean   matuddokw_r_key_press(GtkWidget *widget,GdkEventKey *event,class matuddokw_r_data *data);
gint matuddokw_r1(class matuddokw_r_data *data);
void  matuddokw_r_v_knopka(GtkWidget *widget,class matuddokw_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern double	nds1;
extern int kol_strok_na_liste;

int matuddokw_r(short god,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class matuddokw_r_data data;

data.god=god;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Удаление документов за год"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(matuddokw_r_key_press),&data);

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

repl.plus(gettext("Удаление документов за год"));
repl.plus(" ");
repl.plus(god);
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(matuddokw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)matuddokw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  matuddokw_r_v_knopka(GtkWidget *widget,class matuddokw_r_data *data)
{
printf("matuddokw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   matuddokw_r_key_press(GtkWidget *widget,GdkEventKey *event,class matuddokw_r_data *data)
{
 printf("matuddokw_r_key_press\n");
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

gint matuddokw_r1(class matuddokw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;



int metud=iceb_menu_danet(gettext("Не подтверждённые документы удалять ?"),2,data->window);



/*Если документ не подтвержден, то пропускаем его
Если подтвержден, проверяем нет ли подтверждений следующим годом
Если есть тоже пропускаем
*/
sprintf(strsql,"select datd,nomd,sklad,pod,tip from Dokummat \
where datd >= '%04d-01-01' and datd <= '%04d-12-31' order by datd asc",
data->god,data->god);

SQLCURSOR cur;
SQLCURSOR cur1;
int kolstr;
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

sprintf(strsql,"%s:%d\n",gettext("Количество документов"),kolstr);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

float kolstr1=0;
SQL_str row;
SQL_str row1;
SQL_str row2;
short dd,md,gd;
short d,m,g;
int tipz;
int skl;
int kolstr2;
int kolstr3;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %s %s %s",row[0],row[1],row[2],row[3]);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  iceb_pbar(data->bar,kolstr,++kolstr1);
//  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(metud == 2)
   {
    if(row[3][0] == '0')
     {
      sprintf(strsql," %s !\n",gettext("Не подтвержден"));
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
      continue;
     }
   }
  else
   goto vp;
   
  sprintf(strsql,"select nomkar from Dokummat1 where datd='%s' and \
nomd='%s' and sklad=%s",row[0],row[1],row[2]);  
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr2 == 0)
   {
    goto vp;
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    sprintf(strsql,"select datdp from Zkart where sklad=%s and \
nomk=%s and datd='%s' and nomd='%s'",row[2],row1[0],row[0],row[1]);
    SQLCURSOR cur2;
    if((kolstr3=cur2.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
      continue;
     }

    if(kolstr3 == 0)
     {
      goto vp;
     }

    while(cur2.read_cursor(&row2) != 0)
     {
      iceb_u_rsdat(&d,&m,&g,row2[0],2);
      if(g > data->god)
       {
        sprintf(strsql,gettext("Документ подтвержден в %d году"),g);
        iceb_menu_soob(strsql,data->window);
        goto vp1;
       }
      } 
   }

vp:;
  sprintf(strsql," %s.\n",gettext("Удален"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


  tipz=atoi(row[4]); 
  iceb_u_rsdat(&dd,&md,&gd,row[0],2);
  skl=atoi(row[2]);

  matuddw(0,1,0,tipz,dd,md,gd,row[1],skl,0,0,data->window);
vp1:;
  
 } 


sprintf(strsql,"%s %s !\n",gettext("Оптимизация таблицы"),"Dokummat");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int		metkazap=0;

strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur);
if(iceb_u_SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  
sprintf(strsql,"VERSION:%s\n",row[0]);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Dokummat");
if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql,data->window);
 }
else
  sql_readkey(&bd,strsql);

sprintf(strsql,"%s.\n",gettext("Оптимизацию таблицы закончено"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


sprintf(strsql,"%s %s !\n",gettext("Оптимизация таблицы"),"Dokummat1");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Dokummat1");
if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql,data->window);
 }
else
  sql_readkey(&bd,strsql);

sprintf(strsql,"%s.\n",gettext("Оптимизацию таблицы закончено"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s %s !\n",gettext("Оптимизация таблицы"),"Dokummat2");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Dokummat2");
if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql,data->window);
 }
else
  sql_readkey(&bd,strsql);

sprintf(strsql,"%s.\n",gettext("Оптимизацию таблицы закончено"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s %s !\n",gettext("Оптимизация таблицы"),"Dokummat3");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Dokummat3");
if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql,data->window);
 }
else
  sql_readkey(&bd,strsql);

sprintf(strsql,"%s.\n",gettext("Оптимизацию таблицы закончено"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


sprintf(strsql,"%s %s !\n",gettext("Оптимизация таблицы"),"Kart");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Kart");
if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql,data->window);
 }
else
  sql_readkey(&bd,strsql);

sprintf(strsql,"%s.\n",gettext("Оптимизацию таблицы закончено"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s %s !\n",gettext("Оптимизация таблицы"),"Zkart");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Zkart");
if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql,data->window);
 }
else
  sql_readkey(&bd,strsql);

sprintf(strsql,"%s.\n",gettext("Оптимизацию таблицы закончено"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
