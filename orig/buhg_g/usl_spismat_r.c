/*$Id: usl_spismat_r.c,v 1.13 2011-02-21 07:35:58 sasa Exp $*/
/*16.12.2009	19.10.2004	Белых А.И.	usl_spismat_r.c

*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "buhg_g.h"
#include "usl_spismat.h"

class usl_spismat_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  short prkk;
  
  class usl_spismat_rek *rk;
    

 };

gboolean   usl_spismat_r_key_press(GtkWidget *widget,GdkEventKey *event,class usl_spismat_r_data *data);
gint usl_spismat_r1(class usl_spismat_r_data *data);
void  usl_spismat_r_v_knopka(GtkWidget *widget,class usl_spismat_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern iceb_u_str shrift_ravnohir;
 
void usl_spismat_r(class usl_spismat_rek *rek_ras,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class usl_spismat_r_data data;
data.rk=rek_ras;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Копировать записи из других документов."));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(usl_spismat_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Копировать записи из других документов."));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),450,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
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


gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(usl_spismat_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)usl_spismat_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  usl_spismat_r_v_knopka(GtkWidget *widget,class usl_spismat_r_data *data)
{
 printf("usl_spismat_r_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   usl_spismat_r_key_press(GtkWidget *widget,GdkEventKey *event,class usl_spismat_r_data *data)
{
 printf("usl_spismat_r_key_press\n");

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

gint usl_spismat_r1(class usl_spismat_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
int kolstr;
iceb_u_str repl;
int kolsz=0;

short d,m,g;
iceb_u_rsdat(&d,&m,&g,data->rk->datas.ravno(),1);

//Блокируем таблицу
class iceb_lock_tables lock("LOCK TABLES Dokummat WRITE,icebuser READ");

/*Проверяем есть ли такой номер документа*/
sprintf(strsql,"select nomd from Dokummat where \
datd >= '%04d-01-01' and datd <= '%d-12-31' and sklad=%d and nomd='%s'",
g,g,
data->rk->sklad.ravno_atoi(),
data->rk->nomdok.ravno());

if(iceb_sql_readkey(strsql,data->window) >= 1)
 {
  sprintf(strsql,gettext("С номером %s документ уже есть !"),data->rk->nomdok.ravno());
  iceb_menu_soob(strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(zap_s_mudokw(data->rk->tipz,d,m,g,data->rk->sklad.ravno_atoi(),data->rk->kontr.ravno(),
data->rk->nomdok.ravno(),"",data->rk->kodop.ravno(),0,0,"",0,0,0,0,data->window) != 0)
 {
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

//Разблокируем все таблицы
lock.unlock();

SQLCURSOR cur;
SQL_str row;

sprintf(strsql,"select kodzap,kolih,cena,ei,shetu,dnaim,shsp from Usldokum1 where datd='%04d-%02d-%02d' \
and podr=%d and nomd='%s' and tp=%d and metka=0",
data->rk->gd,data->rk->md,data->rk->dd,data->rk->podr,data->rk->nomdoku.ravno(),data->rk->tipz);

//iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
//iceb_printw("\n",data->buffer,data->view);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char osnov[80];
sprintf(osnov,"USL%02d.%02d.%d N%s",data->rk->dd,data->rk->md,data->rk->gd,data->rk->nomdoku.ravno());

//Записываем через кого (есть в документе на приход и на расход)
sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',%d,'%s')",g,data->rk->sklad.ravno_atoi(),data->rk->nomdok.ravno(),2,osnov);

//iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
//iceb_printw("\n",data->buffer,data->view);

iceb_sql_zapis(strsql,1,0,data->window);


//программа позволяет записать один и тотже материал любое количество раз
//поэтому нужно просумировать количества одинаковых кодов материалов с тем 
//чтобы списать в материальном учёте одной записью
iceb_u_int kod_mat;
iceb_u_double kolih_mat;
int kodm;
double kolih;
int nomer;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  sprintf(strsql,"%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  
  kodm=atoi(row[0]);
  kolih=atof(row[1]);
  
  if((nomer=kod_mat.find(kodm)) < 0)
   kod_mat.plus(kodm,nomer);
  kolih_mat.plus(kolih,nomer);
  kolsz++;  
 }
double ndss=data->rk->pnds;
if(data->rk->lnds != 0)
 ndss=0.;
 
for(int i=0; i < kod_mat.kolih() ; i++)
 {

  sprintf(strsql,"select cena,ei,shetu,shsp from Usldokum1 where datd='%04d-%02d-%02d' \
and podr=%d and nomd='%s' and tp=%d and metka=0 and kodzap=%d limit 1",
  data->rk->gd,data->rk->md,data->rk->dd,data->rk->podr,data->rk->nomdoku.ravno(),data->rk->tipz,kod_mat.ravno(i));
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись в документе"),data->window);
    continue;
   }

  if(data->rk->tipz == 1)
   zap_prihodw(d,m,g,data->rk->nomdok.ravno(),data->rk->sklad.ravno_atoi(),kod_mat.ravno(i),kolih_mat.ravno(i),atof(row[0]),row[1],row[2],ndss,0,0,"",row[3],data->window);
  if(data->rk->tipz == 2)
    priv_k_kartw(d,m,g,data->rk->nomdok.ravno(),data->rk->sklad.ravno_atoi(),kod_mat.ravno(i),kolih_mat.ravno(i),atof(row[0]),row[2],row[2],ndss,0,0,"",0,row[3],data->window);

 
 }
podtdok1w(d,m,g,data->rk->sklad.ravno_atoi(),data->rk->nomdok.ravno(),data->rk->tipz,d,m,g,1,data->window);
podvdokw(d,m,g,data->rk->nomdok.ravno(),data->rk->sklad.ravno_atoi(),data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен."));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

//sprintf(strsql,"%s %d !",gettext("Количество переписаних документов"),koldok);
//repl.new_plus(strsql);
sprintf(strsql,"%s %d !",gettext("Количество переписаних записей"),kolsz);
repl.new_plus(strsql);
iceb_menu_soob(&repl,data->window);

return(FALSE);
}
