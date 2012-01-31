/*$Id: plc_mu_r.c,v 1.13 2011-02-21 07:35:55 sasa Exp $*/
/*13.11.2005	18.11.2004	Белых А.И.	plc_mu_r.c
Проверка логической целосности базы материального учёта
*/
#include <errno.h>
#include <unistd.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze

class plc_mu_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
    
  int metka; //0-только проверка 1-проверка с удалением
  
  short kon_ras; //0-расчет завершен 1-нет
  iceb_u_str imaf_p;
  int kolerror;
  plc_mu_r_data()
   {
    kolerror=0;
    kon_ras=1;
   }
 };
gboolean   plc_mu_r_key_press(GtkWidget *widget,GdkEventKey *event,class plc_mu_r_data *data);
gint plc_mu_r1(class plc_mu_r_data *data);
void  plc_mu_r_v_knopka(GtkWidget *widget,class plc_mu_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

int plc_mu_r(int metka,iceb_u_str *imaf_p,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class plc_mu_r_data data;

data.metka=metka;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Проверка логической целосности базы"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(plc_mu_r_key_press),&data);

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

repl.plus(gettext("Проверка логической целосности базы"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(plc_mu_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)plc_mu_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.kolerror);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  plc_mu_r_v_knopka(GtkWidget *widget,class plc_mu_r_data *data)
{
printf("plc_mu_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   plc_mu_r_key_press(GtkWidget *widget,GdkEventKey *event,class plc_mu_r_data *data)
{
 printf("plc_mu_r_key_press\n");
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

gint plc_mu_r1(class plc_mu_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_u_str repl;
iceb_clock sss(data->window);


sprintf(strsql,"matulk%d.lst",getpid());
data->imaf_p.new_plus(strsql);
FILE *ff;

if((ff = fopen(data->imaf_p.ravno(),"w")) == NULL)
 {
  iceb_er_op_fil(data->imaf_p.ravno(),"",errno,data->window);
  return(FALSE);
 }
iceb_u_startfil(ff);


iceb_u_zagolov(gettext("Проверка логической целосности базы данных"),0,0,0,0,0,0,organ,ff);



fprintf(ff,"%s:%s %s\n",gettext("База данных"),imabaz,organ);


fprintf(ff,"\
--------------------------------------\n");
fprintf(ff,"%s\n",
gettext("  Дата    |Склад|+/-|Номер документа"));
fprintf(ff,"\
--------------------------------------\n");


sprintf(strsql,"\
--------------------------------------\n");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s\n",
gettext("  Дата    |Склад|+/-|Номер документа"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"\
--------------------------------------\n");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

SQLCURSOR cur;
SQL_str row;
SQLCURSOR cur1;
SQL_str row1;
int kolstr;
float kolstr1=0;

sprintf(strsql,"%s Dokummat1.\n",gettext("Проверка таблицы"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
fprintf(ff,"%s\n",strsql);
fprintf(ff,"%s\n",
gettext("Проверка записей в документах не имеющих шапки документа"));


sprintf(strsql,"select distinct datd,sklad,nomd,tipz,nomkar from Dokummat1 \
order by datd asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);


if(kolstr == 0)
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);

while(cur.read_cursor(&row) != 0)
 {
//  sprintf(stsql,"%s %-5s %-3s %s\n",row[0],row[1],row[3],row[2]);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select tip from Dokummat where tip=%s and \
datd='%s' and sklad=%s and nomd='%s'",row[3],row[0],row[1],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    sprintf(strsql,"%s %-5s %-3s %s\n",row[0],row[1],row[3],row[2]);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(ff,"%s %-5s %-3s %s\n",row[0],row[1],row[3],row[2]);
    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Dokummat1 where datd='%s' and sklad=%s and nomd='%s' and \
tipz=%s and nomkar=%s",row[0],row[1],row[2],row[3],row[4]);
      if(sql_zap(&bd,strsql) != 0)      
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
   }
  if(row[4][0] == '0')
   continue;
  sprintf(strsql,"select nomk from Kart where sklad=%s and \
nomk=%s",row[1],row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    sprintf(strsql,"%s %-5s %-3s %s\n",row[0],row[1],row[3],row[2]);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    sprintf(strsql,"%s %s %s %s !\n",
    gettext("Не найдена карточка"),row[4],
    gettext("на складе"),row[3]);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(ff,"%s %-5s %-3s %s\n",row[0],row[1],row[3],row[2]);
    fprintf(ff,"%s %s %s %s !",
    gettext("Не найдена карточка"),row[4],
    gettext("на складе"),row[3]);
    data->kolerror++;
   }
 }

sprintf(strsql,"%s Dokummat2.\n",gettext("Проверка таблицы"));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
fprintf(ff,"%s\n",strsql);


sprintf(strsql,"select distinct god,sklad,nomd from Dokummat2 order by god asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);


if(kolstr == 0)
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  sprintf(stsql,"%s %-5s %-3s %s\n",row[0],row[1],row[3],row[2]);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select tip from Dokummat where  \
datd >= '%s-01-01' and datd <= '%s-12-31'and sklad=%s and nomd='%s'",row[0],row[0],row[1],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    sprintf(strsql,"%10s %-5s %-3s %s\n",row[0],row[1],"",row[2]);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(ff,"%s %-5s %-3s %s\n",row[0],row[1],"",row[2]);
    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Dokummat2 where god='%s' and sklad=%s and nomd='%s'",
      row[0],row[1],row[2]);
      if(sql_zap(&bd,strsql) != 0)      
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
   }
 }

sprintf(strsql,"%s Dokummat3.\n",gettext("Проверка таблицы"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
fprintf(ff,"%s\n",strsql);


sprintf(strsql,"select distinct datd,sklad,nomd from Dokummat3 order by datd asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);


if(kolstr == 0)
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  sprintf(stsql,"%s %-5s %-3s %s\n",row[0],row[1],row[3],row[2]);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select tip from Dokummat where \
datd='%s' and sklad=%s and nomd='%s'",row[0],row[1],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    sprintf(strsql,"%s %-5s %-3s %s\n",row[0],row[1],"",row[2]);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(ff,"%s %-5s %-3s %s\n",row[0],row[1],"",row[2]);
    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Dokummat3 where datd='%s' and sklad=%s and nomd='%s'",
      row[0],row[1],row[2]);
      if(sql_zap(&bd,strsql) != 0)      
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
   }
 }

sprintf(strsql,"\n%s Zkart.\n",gettext("Проверка таблицы"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(ff,"\n%s.\n",gettext("Проверка записей в карточках"));

sprintf(strsql,"select distinct sklad,nomk,nomd,datd,tipz from Zkart \
order by datd asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  wprintw(win1,"%s %-5s %-3s %s\n",row[3],row[0],row[4],row[2]);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select tip from Dokummat where tip=%s and \
datd='%s' and sklad=%s and nomd='%s'",row[4],row[3],row[0],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    sprintf(strsql,"%s %-5s %-3s %s\n",row[3],row[0],row[4],row[2]);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    sprintf(strsql,"%s\n",gettext("Не найдена шапка документа !"));
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(ff,"%s %-5s %-3s %s\n",row[3],row[0],row[4],row[2]);
    fprintf(ff,"%s !\n",gettext("Не найдена шапка документа"));
    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Zkart where datd='%s' and sklad=%s and nomd='%s' and \
tipz=%s and nomk=%s",row[3],row[0],row[2],row[4],row[1]);
      if(sql_zap(&bd,strsql) != 0)      
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
   }

  sprintf(strsql,"select tipz from Dokummat1 where tipz=%s and \
datd='%s' and sklad=%s and nomd='%s'",row[4],row[3],row[0],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    sprintf(strsql,"%s %-5s %-3s %s\n",row[3],row[0],row[4],row[2]);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    sprintf(strsql,"%s !\n",gettext("Не найдена запись в документе"));
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(ff,"%s %-5s %-3s %s\n",row[3],row[0],row[4],row[2]);
    fprintf(ff,"%s !\n",gettext("Не найдена запись в документе"));
    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Zkart where datd='%s' and sklad=%s and nomd='%s' and \
tipz=%s and nomk=%s",row[3],row[0],row[2],row[4],row[1]);
      if(sql_zap(&bd,strsql) != 0)      
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
   }
  sprintf(strsql,"select sklad from Kart where sklad=%s and nomk=%s",
  row[0],row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    sprintf(strsql,"%s %-5s %-3s %s\n",row[3],row[0],row[4],row[2]);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    sprintf(strsql,"%s !\n",gettext("Не найдена карточка"));
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(ff,"%s %-5s %-3s %s\n",row[3],row[0],row[4],row[2]);
    fprintf(ff,"%s !\n",gettext("Не найдена карточка"));
    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Zkart where datd='%s' and sklad=%s and nomd='%s' and \
tipz=%s and nomk=%s",row[3],row[0],row[2],row[4],row[1]);
      if(sql_zap(&bd,strsql) != 0)      
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
   }
 }

sprintf(strsql,"\nПроверка таблицы Kart.\n");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"\
--------------------\n");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s\n",
gettext("Склад|Номер карточки"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"\
--------------------\n");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


fprintf(ff,"\n%s.\n",gettext("Проверка карточек без ссылок"));

fprintf(ff,"\
--------------------\n");

fprintf(ff,"%s\n",
gettext("Склад|Номер карточки"));

fprintf(ff,"\
--------------------\n");


sprintf(strsql,"select sklad,nomk from Kart \
order by sklad asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);


kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  wprintw(win1,"%-5s %s\n",row[0],row[1]);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select tipz from Dokummat1 where \
sklad=%s and nomkar=%s limit 1",row[0],row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    sprintf(strsql,"%-5s %s\n",row[0],row[1]);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(ff,"%-5s %s\n",row[0],row[1]);
    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Kart where sklad=%s and nomk=%s",row[0],row[1]); 
      if(sql_zap(&bd,strsql) != 0)      
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
   }
 }

fclose(ff);

iceb_printw_vr(vremn,data->buffer,data->view);



if(data->kolerror == 0)
 {
  iceb_menu_soob(gettext("Порядок, ошибок не обнаружено !"),data->window);
  unlink(data->imaf_p.ravno());
 }
else
 {
  sprintf(strsql,"%s: %d",gettext("Количество ошибок"),data->kolerror);
  repl.new_plus(strsql);
  iceb_menu_soob(&repl,data->window);
 }






data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

return(FALSE);

}
