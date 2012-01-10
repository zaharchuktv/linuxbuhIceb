/*$Id: l_restdok2.c,v 1.31 2011-02-21 07:36:20 sasa Exp $*/
/*30.10.2007	23.02.2004	Белых А.И.	l_restdok2.c
Работа с записями документа (Новый вариант программы)
Если вернули 0-перечитывать вызывающий список
             1-не перечитывать
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "i_rest.h"
#include "l_restdok2.h"
#include "../knopki/cod_tovara.xpm"
#include "../knopki/tovar.xpm"
#include "../knopki/korektir.xpm"
#include "../knopki/udalit.xpm"
#include "../knopki/print.xpm"
#include "../knopki/usluga.xpm"
#include "../knopki/oplata.xpm"
#include "../knopki/hidden.xpm"
#include "../knopki/vihod.xpm"
#include "../knopki/cod_uslugi.xpm"

void restdok2_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class restdok2_data *data);
gboolean   restdok2_key_press(GtkWidget *widget,GdkEventKey *event,class restdok2_data *data);
void  restdok2_knopka(GtkWidget *widget,class restdok2_data *data);
void restdok2_add_columns(GtkTreeView *treeview);
void restdok2_vibor(GtkTreeSelection *selection,class restdok2_data *data);
void restdok2_udzap(class restdok2_data *data);
void restdok2_create_list (class restdok2_data *data);


void     restdok2_vrp(class restdok2_data *data);
void restdok2_rskl(short dd,short md,short gd,const char *nomdok,GtkWidget *wpredok);
void restdok2_rskl1(short dd,short md,short gd,const char *nomdok,const char *skl,const char *imaf,int,const char*,GtkWidget *wpredok);
void  restdok2_sklad_get(GtkWidget *widget,class restdok2_data *data);
void  restdok2_sp_sklad(GtkWidget **windowvoditel,iceb_u_str*,int);
void  restdok2_sp_podr(GtkWidget **widget_podr,iceb_u_str *podrv);
void  restdok2_podr_get(GtkWidget *widget,class restdok2_data *data);
int restdok2_prov_opl(class restdok2_data *data);

void l_restdok2_vgr(class restdok2_data *data);
void restdo2_hbox_gr_mu(class restdok2_data *data);
void l_restdok2_znm(class restdok2_data *data);


extern short metka_crk; //0-расчёт клиентов по подразделениям 1-централизованный
extern SQL_baza	bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern char *putnansi;
extern class iceb_get_dev_data config_dev;
extern uid_t kod_operatora;
extern int  nomer_kas;  //номер кассы в настоящий момент всегда равно нолю так как работа с кассовыми регистраторами не реализована
                          //Эта переменная необходима при записи в таблицу Restkas при списании 

int  l_restdok2(short god,iceb_u_str *nomdok,
GtkWidget *wpredok)
{

restdok2_data data;
char strsql[512];
SQLCURSOR cur;
SQL_str   row;

data.gd=god;
data.nomdok.new_plus(nomdok->ravno());

//читаем реквизиты шапки документа
sprintf(strsql,"select * from Restdok where god=%d and nomd='%s'",
data.gd,data.nomdok.ravno());

if(iceb_sql_readkey(strsql,&row,&cur,NULL) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"%s %s %s %d",
  gettext("Не найден документ"),
  nomdok->ravno(),
  gettext("Год"),
  god);
  
  repl.plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return(0);
 }

iceb_u_rsdat(&data.dd,&data.md,&data.gd,row[2],2); 
data.mo=atoi(row[3]);
data.nstol.new_plus(row[4]);
data.kodkl.new_plus(row[5]);
data.fio.new_plus(row[6]);
data.koment.new_plus(row[7]);
data.podr=atoi(row[11]);
data.naimpodr.new_plus("");
data.vremz=atol(row[12]);
data.proc_sk=atof(row[14]);

//Выставляем флаг
memset(strsql,'\0',sizeof(strsql));
if(data.kodkl.getdlinna() > 1)
  sprintf(strsql,"rest|%s|%s",imabaz,data.kodkl.ravno());
iceb_sql_flag kl_flag;
if(data.mo == 0)
  rest_flag_kl(&kl_flag,data.kodkl.ravno(),wpredok);
  
sprintf(strsql,"select naik from Restpod where kod=%d",data.podr);
if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
 data.naimpodr.new_plus(row[0]);


//читаем заданные размеры окна
iceb_u_str imaf_nast("restnast.alx");
//iceb_imafn("restnast.alx",&imaf_nast);
iceb_poldan("Размеры окна в пикселах для режима выписки счетов",strsql,imaf_nast.ravno(),wpredok);
char bros[512];

int goriz;
int vert;

iceb_u_polen(strsql,bros,sizeof(bros),1,',');
goriz=atoi(bros);

iceb_u_polen(strsql,bros,sizeof(bros),2,',');
vert=atoi(bros);
int metka_klris=0;
if(iceb_poldan("В режиме выписки счетов клавиши с рисунком",strsql,imaf_nast.ravno(),wpredok) == 0)
 if(iceb_u_SRAV("Вкл",strsql,1) == 0)
  metka_klris=1; /*Клавиши с рисунком*/


data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(vert != 0 && goriz != 0)
 gtk_window_set_default_size (GTK_WINDOW  (data.window),goriz,vert);

//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(strsql,"%s %s",name_system,gettext("Просмотр документа"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 0);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(restdok2_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxv = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 0);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 0);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Стартовые сальдо по счетам"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),hboxv,FALSE, FALSE, 0);

//читаем настройку разделителя для вертикальных панелей
iceb_u_poldan("Количество пикселов для разделения вертикальных панелей",strsql,".iceb_l_restdok2.dat");
int kol_pix=atoi(strsql);

data.vert_panel=gtk_vpaned_new();
gtk_box_pack_start (GTK_BOX (vbox2),data.vert_panel,TRUE, TRUE, 0);
if(kol_pix > 0)
 gtk_paned_set_position(GTK_PANED(data.vert_panel),kol_pix);

gtk_widget_show(data.vert_panel);


//читаем настройку разделителя для горизонтальных панелей
iceb_u_poldan("Количество пикселов для разделения горизонтальных панелей",strsql,".iceb_l_restdok2.dat");
kol_pix=atoi(strsql);

data.gor_panel=gtk_hpaned_new();
//gtk_box_pack_start (GTK_BOX (vbox2),data.gor_panel,TRUE, TRUE, 0);
gtk_paned_add1(GTK_PANED(data.vert_panel),data.gor_panel);

if(kol_pix > 0)
 gtk_paned_set_position(GTK_PANED(data.gor_panel),kol_pix);

//Вставляем в горизонтальный бокс виджеты
restdo2_hbox_gr_mu(&data);


//data.label_poisk=gtk_label_new ("");
//gdk_color_parse("red",&color);
//gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

//gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
//gtk_widget_set_usize(GTK_WIDGET(data.sw),600,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_paned_add2(GTK_PANED(data.vert_panel),data.sw);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

if(config_dev.metka_screen == 0) //Нет сенсорного экрана
 {
  GtkWidget *label=gtk_label_new(gettext("Склад"));
  gtk_box_pack_start(GTK_BOX (hboxv),label,FALSE,FALSE,0);

  //Создаём список складов
  restdok2_sp_sklad(&data.windowvoditel,&data.skladv,data.podr);

  gtk_box_pack_start (GTK_BOX (hboxv), data.windowvoditel, TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT (GTK_COMBO(data.windowvoditel)->entry), "activate",GTK_SIGNAL_FUNC(restdok2_sklad_get),&data);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(data.windowvoditel)->entry),data.skladv.ravno_toutf());

  label=gtk_label_new(gettext("Подразделение"));
  gtk_box_pack_start(GTK_BOX (hboxv),label,FALSE,FALSE,0);

  //Создаём список подразделений
  restdok2_sp_podr(&data.widget_podr,&data.podrv);

  gtk_box_pack_start (GTK_BOX (hboxv), data.widget_podr, TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT (GTK_COMBO(data.widget_podr)->entry), "activate",GTK_SIGNAL_FUNC(restdok2_podr_get),&data);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(data.widget_podr)->entry),data.podrv.ravno_toutf());
 }
else
 {
  data.knopka[F_SKLAD]=gtk_button_new_with_label(gettext("Склад"));
  gtk_box_pack_start (GTK_BOX (hboxv), data.knopka[F_SKLAD], TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(data.knopka[F_SKLAD]), "clicked",GTK_SIGNAL_FUNC(restdok2_knopka),&data);

  tooltips[F_SKLAD]=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips[F_SKLAD],data.knopka[F_SKLAD],gettext("Выбор склада"),NULL);
  gtk_object_set_user_data(GTK_OBJECT(data.knopka[F_SKLAD]),(gpointer)F_SKLAD);
  gtk_widget_show(data.knopka[F_SKLAD]);

  class iceb_u_str spis_skl;
  l_restdok_spis_skl(&spis_skl,data.podr);
  strncpy(bros,spis_skl.ravno(),sizeof(bros)-1);
  
  if(bros[0] != '\0')
   {
    int kolih=iceb_u_pole2(bros,',');
    if(kolih == 0)
     sprintf(strsql,"select kod,naik from Sklad where kod=%s",bros);
    else
     {
      char bros1[50];
      iceb_u_polen(bros,bros1,sizeof(bros1),1,',');
      sprintf(strsql,"select kod,naik from Sklad where kod=%s",bros1);
     }    

    data.skladv.new_plus("");
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     {
      data.skladv.new_plus(row[0]);
      data.skladv.plus(" ");
      data.skladv.plus(row[1]);
     }
   }  
  //склад или равен нулевому байту или имеет значение
  data.label_naim_skl=gtk_label_new (data.skladv.ravno_toutf());
  gtk_box_pack_start(GTK_BOX (hboxv), data.label_naim_skl, TRUE, TRUE, 0);

  //***********услуги**********************

  data.knopka[F_PODR]=gtk_button_new_with_label(gettext("Подразделение"));
  gtk_box_pack_start (GTK_BOX (hboxv), data.knopka[F_PODR], TRUE, TRUE, 0);
//  gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[F_PODR], TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(data.knopka[F_PODR]), "clicked",GTK_SIGNAL_FUNC(restdok2_knopka),&data);

  tooltips[F_PODR]=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips[F_PODR],data.knopka[F_PODR],gettext("Выбор подразделения"),NULL);
  gtk_object_set_user_data(GTK_OBJECT(data.knopka[F_PODR]),(gpointer)F_PODR);
  gtk_widget_show(data.knopka[F_PODR]);

  memset(bros,'\0',sizeof(bros));
  iceb_u_str imafil("restnast.alx");
  //iceb_imafn("restnast.alx",&imafil);

  iceb_poldan("Список подразделений предоставляющих услуги",bros,imafil.ravno(),wpredok);

  if(bros[0] != '\0')
   {
    int kolih=iceb_u_pole2(bros,',');
    if(kolih == 0)
     sprintf(strsql,"select kod,naik from Uslpodr where kod=%s",bros);
    else
     {
      char bros1[50];
      iceb_u_polen(bros,bros1,sizeof(bros1),1,',');
      sprintf(strsql,"select kod,naik from Uslpodr where kod=%s",bros1);
     }    

    data.podrv.new_plus("");
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     {
      data.podrv.new_plus(row[0]);
      data.podrv.plus(" ");
      data.podrv.plus(row[1]);
     }
   }  
  //подразделение равно нулевому байту или имеет значение
  data.label_naim_podr=gtk_label_new (data.podrv.ravno_toutf());
  gtk_box_pack_start(GTK_BOX (hboxv), data.label_naim_podr, TRUE, TRUE, 0);
  GdkColor color;
  gdk_color_parse("red",&color);
  gtk_widget_modify_fg(data.label_naim_skl,GTK_STATE_NORMAL,&color);
  gtk_widget_modify_fg(data.label_naim_podr,GTK_STATE_NORMAL,&color);

 }
 
gtk_widget_show_all(hboxv);
gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


//sprintf(strsql,"F2 %s",gettext("Товар"));
//data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_realize(data.window);

//gtk_widget_show(data.window);
GtkStyle *style=NULL;
GdkPixmap *pixmap=NULL;
GdkBitmap *mask;
GtkWidget *pixmapWidget;

  style = gtk_widget_get_style(data.window);
if(metka_klris == 0)
 {
  sprintf(strsql,"F2 %s",gettext("Ввод"));
  data.knopka[FK2]=gtk_button_new_with_label(strsql);
 }
else
 {
  pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],(gchar **)tovar_xpm);
  pixmapWidget=gtk_pixmap_new(pixmap,mask);
  data.knopka[FK2]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[FK2]),pixmapWidget);
 }


gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(restdok2_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод нового товарa в счёт"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show_all(data.knopka[FK2]);

if(metka_klris == 0)
 {
  sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
  data.knopka[SFK2]=gtk_button_new_with_label(strsql);
 }
else
 {
  pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],(gchar **)korektir_xpm);
  pixmapWidget=gtk_pixmap_new(pixmap,mask);
  data.knopka[SFK2]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[SFK2]),pixmapWidget);
 }

gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(restdok2_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show_all(data.knopka[SFK2]);


if(metka_klris == 0)
 {
  sprintf(strsql,"F3 %s",gettext("Удалить"));
  data.knopka[FK3]=gtk_button_new_with_label(strsql);
 }
else
 {
  pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],(gchar **)udalit_xpm);
  pixmapWidget=gtk_pixmap_new(pixmap,mask);
  data.knopka[FK3]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[FK3]),pixmapWidget);
 }


gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(restdok2_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление записи или документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show_all(data.knopka[FK3]);

if(metka_klris == 0)
 {
  sprintf(strsql,"F5 %s",gettext("Печать"));
  data.knopka[FK5]=gtk_button_new_with_label(strsql);
 }
else
 {
  pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],(gchar **)print_xpm);
  pixmapWidget=gtk_pixmap_new(pixmap,mask);
  data.knopka[FK5]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[FK5]),pixmapWidget);
 }

gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(restdok2_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show_all(data.knopka[FK5]);

if(metka_crk == 0)
 {
  if(metka_klris == 0)
   {
    sprintf(strsql,"F6 %s",gettext("Оплата"));
    data.knopka[FK6]=gtk_button_new_with_label(strsql);
   }
  else
   {
    pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],(gchar **)oplata_xpm);
    pixmapWidget=gtk_pixmap_new(pixmap,mask);
    data.knopka[FK6]=gtk_button_new();
    gtk_container_add(GTK_CONTAINER(data.knopka[FK6]),pixmapWidget);
   }

  gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(restdok2_knopka),&data);
  tooltips[FK6]=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Отметить документ как оплаченный"),NULL);
  gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
  gtk_widget_show_all(data.knopka[FK6]);
 }

if(metka_klris == 0)
 {
  sprintf(strsql,"F7 %s",gettext("Послуга"));
  data.knopka[FK7]=gtk_button_new_with_label(strsql);
 }
else
 {
  pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],(gchar **)usluga_xpm);
  pixmapWidget=gtk_pixmap_new(pixmap,mask);
  data.knopka[FK7]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[FK7]),pixmapWidget);
 }

gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(restdok2_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Ввод новой услуги в счет"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show_all(data.knopka[FK7]);

if(metka_klris == 0)
 {
  sprintf(strsql,"F8 %s",gettext("Код товара"));
  data.knopka[FK8]=gtk_button_new_with_label(strsql);
 }
else
 {
  pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],(gchar **)cod_tovara_xpm);
  pixmapWidget=gtk_pixmap_new(pixmap,mask);
  data.knopka[FK8]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[FK8]),pixmapWidget);
 }

gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(restdok2_knopka),&data);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Ввод товара по коду товара"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show_all(data.knopka[FK8]);

if(metka_klris == 0)
 {
  sprintf(strsql,"F9 %s",gettext("Код услуги"));
  data.knopka[FK9]=gtk_button_new_with_label(strsql);
 }
else
 {
  pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],(gchar **)cod_uslugi_xpm);
  pixmapWidget=gtk_pixmap_new(pixmap,mask);
  data.knopka[FK9]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[FK9]),pixmapWidget);
 }
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(restdok2_knopka),&data);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Ввод услуги по коду услуги"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show_all(data.knopka[FK9]);

if(metka_klris == 0)
 {
  sprintf(strsql,"%sF9 %s",RFK,gettext("Спрятать"));
  data.knopka[SFK9]=gtk_button_new_with_label(strsql);
 }
else
 {
  pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],(gchar **)hidden_xpm);
  pixmapWidget=gtk_pixmap_new(pixmap,mask);
  data.knopka[SFK9]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[SFK9]),pixmapWidget);
 }
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK9],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK9]), "clicked",GTK_SIGNAL_FUNC(restdok2_knopka),&data);
tooltips[SFK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK9],data.knopka[SFK9],gettext("Спрятать/показать шапку документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK9]),(gpointer)SFK9);
gtk_widget_show_all(data.knopka[SFK9]);

if(metka_klris == 0)
 {
  sprintf(strsql,"F10 %s",gettext("Выход"));
  data.knopka[FK10]=gtk_button_new_with_label(strsql);
 }
else
 {
  pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],(gchar **)vihod_xpm);
  pixmapWidget=gtk_pixmap_new(pixmap,mask);
  data.knopka[FK10]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[FK10]),pixmapWidget);
 }
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(restdok2_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show_all(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

//printf("создаём списоk\n");
restdok2_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

//printf("l_restdok2 end voz=%d\n",data.voz);

return(data.voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void restdok2_create_list (class restdok2_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row;
SQL_str row1;
double suma=0.;

//printf("restdok2_create_list %d\n",data->snanomer);
//data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(restdok2_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(restdok2_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);

model = gtk_list_store_new (NUM_COLUMNS+3,
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT,
G_TYPE_INT,
G_TYPE_INT);


sprintf(strsql,"select * from Restdok1 where datd='%d-%d-%d' and \
nomd='%s' order by mz asc,skl asc",
data->gd,data->md,data->dd,data->nomdok.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
double kolih=0.;
double cena=0.;
double cena_sk=0.;
//int nomerzap=0;
char metka_ras[5];
double suma_skidki=0.;
float proc_sk=0.;
data->suma_k_oplate=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  


  if(data->kodn == atoi(row[4]))
     data->snanomer=data->kolzap;

  //Склад
  ss[COL_SKL].new_plus(iceb_u_toutf(row[2]));
  
  //Метка записи
  if(row[3][0] == '0')
   ss[COL_MZ].new_plus(iceb_u_toutf("Т"));
  else
   ss[COL_MZ].new_plus(iceb_u_toutf("П"));
  
  //Код
  ss[COL_KOD].new_plus(iceb_u_toutf(row[4]));

  //узнаём наименование
  if(atoi(row[3]) == 0) //материал
   {
    
    sprintf(strsql,"select naimat from Material where kodm=%s",
    row[4]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     ss[COL_NAIM].new_plus(iceb_u_toutf(row1[0]));
    else
     ss[COL_NAIM].new_plus(iceb_u_toutf(" "));
            
   }
  else //услуга
   {
    
    sprintf(strsql,"select naius from Uslugi where kodus=%s",
    row[4]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     ss[COL_NAIM].new_plus(iceb_u_toutf(row1[0]));
    else
     ss[COL_NAIM].new_plus(iceb_u_toutf(" "));
            
   }
  
  //единица измерения
  ss[COL_EI].new_plus(iceb_u_toutf(row[5]));

  //количество
  kolih=atof(row[6]);
  sprintf(strsql,"%.10g",kolih);
  ss[COL_KOLIH].new_plus(strsql);

  // цена
  cena=atof(row[7]);
  sprintf(strsql,"%.10g",cena);
  ss[COL_CENA].new_plus(strsql);

  // Процент скидки
  proc_sk=atof(row[16]);
  sprintf(strsql,"%.10g%%",proc_sk);
  ss[COL_PROC_SK].new_plus(strsql);

  //Цена со скидкой
  cena_sk=cena-cena*proc_sk/100.;  
  cena_sk=iceb_u_okrug(cena_sk,0.01);
  ss[COL_CENA_SK].new_plus(cena_sk);

  suma_skidki+=kolih*(cena-cena_sk);
  
  // сумма
  suma=kolih*cena_sk;
  data->suma_k_oplate+=suma;
  sprintf(strsql,"%.2f",suma);  
  ss[COL_SUMA].new_plus(strsql);

  //коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[8]));

  //МЕТКА СПИСАНИЯ
  if(atoi(row[9]) == 1)
   ss[COL_SPISANIE].new_plus(gettext("Списано"));
  else
   ss[COL_SPISANIE].new_plus("");
  
  //Дата и время записи
  ss[COL_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[11])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_u_kszap(row[10],0));
  
  memset(metka_ras,'\0',sizeof(metka_ras));
  if(row[14][0] == '1')
   metka_ras[0]='*';
   
  gtk_list_store_append (model, &iter);


   gtk_list_store_set (model, &iter,
   COL_SKL,ss[COL_SKL].ravno(),
   COL_MZ,ss[COL_MZ].ravno(),
   COL_KOD,ss[COL_KOD].ravno(),
   COL_MR,metka_ras,
   COL_NAIM,ss[COL_NAIM].ravno(),
   COL_EI,ss[COL_EI].ravno(),
   COL_KOLIH,ss[COL_KOLIH].ravno(),
   COL_CENA,ss[COL_CENA].ravno(),
   COL_CENA_SK,ss[COL_CENA_SK].ravno(),
   COL_PROC_SK,ss[COL_PROC_SK].ravno(),
   COL_SUMA,ss[COL_SUMA].ravno(),
   COL_KOMENT,ss[COL_KOMENT].ravno(),
   COL_SPISANIE,ss[COL_SPISANIE].ravno(),
   COL_VREM,ss[COL_VREM].ravno(),
   COL_KTO,ss[COL_KTO].ravno(),
   NUM_COLUMNS,data->kolzap,
   NUM_COLUMNS+1,atoi(row[3]),
   NUM_COLUMNS+2,atoi(row[13]),
   -1);

  data->kolzap++;
 }
data->kodn=0;
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

restdok2_add_columns (GTK_TREE_VIEW (data->treeview));

if(data->mo == 1)
 {
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна

    if(data->kolzap == 0)
      gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
    if(metka_crk == 0)
      gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
  if(data->kolzap == 0)
   {
//    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
    if(metka_crk == 0)
      gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
   }
  else
   {
//    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
    if(metka_crk == 0)
     gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
   }


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

if(data->kolzap > 0)
 {
  if(data->snanomer  > data->kolzap-1)
     data->snanomer= data->kolzap-1;
      
//  printf("poz=%d\n",data->snanomer);

  sprintf(strsql,"%d",data->snanomer);
  GtkTreePath *path=gtk_tree_path_new_from_string(strsql);

//  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,TRUE,0,0);
  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

  gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
  gtk_tree_path_free(path);
 }

iceb_u_str stroka;
iceb_u_str zagolov;

sprintf(strsql,"%s:%s %s %d.%d.%d %s:%d",
gettext("Счет"),
data->nomdok.ravno(),
gettext("от"),
data->dd,data->md,data->gd,
gettext("Количество записей"),
data->kolzap);

zagolov.plus(strsql);
//printf("data->vremz=%ld\n",data->vremz);
sprintf(strsql,"%s:%s",gettext("Дата заказа"),iceb_u_vremzap(data->vremz));

zagolov.ps_plus(strsql);

sprintf(strsql," %s: %d %s",
gettext("Подразделение"),
data->podr,data->naimpodr.ravno());
zagolov.plus(strsql);

if(data->nstol.getdlinna() > 1)
 {
  zagolov.ps_plus(gettext("Номер столика"));
  zagolov.plus(":");
  zagolov.plus(data->nstol.ravno());
 }

if(data->kodkl.getdlinna() > 1)
 {
  zagolov.ps_plus(gettext("Код клиента"));
  zagolov.plus(":");
  zagolov.plus(data->kodkl.ravno());
 }

if(data->fio.getdlinna() > 1)
 {
  zagolov.ps_plus(gettext("Фамилия клиента"));
  zagolov.plus(":");
  zagolov.plus(data->fio.ravno());
 }


if(data->koment.getdlinna() > 1)
 {
  zagolov.ps_plus(gettext("Коментарий"));
  zagolov.plus(":");
  zagolov.plus(data->koment.ravno());
 }


if(data->proc_sk != 0.)
 {
  zagolov.ps_plus(gettext("Процент скидки"));
  zagolov.plus(":");
  zagolov.plus(data->proc_sk);
  suma_skidki=iceb_u_okrug(suma_skidki,0.01);
 }

if(suma_skidki == 0.)
  sprintf(strsql,"%s:%.2f",gettext("Сумма к оплате"),data->suma_k_oplate);
else
  sprintf(strsql,"%s:%.2f %s:%.2f",
  gettext("Сумма к оплате"),data->suma_k_oplate,
  gettext("Сумма скидки"),suma_skidki);

zagolov.ps_plus(strsql);
zagolov.plus("  ");
if(data->mo == 0)
  zagolov.plus(gettext("Счет не оплачено"));
if(data->mo == 1)
  zagolov.plus(gettext("Счет оплачено"));
data->saldo=0.;
if(data->kodkl.getdlinna() > 1)
 {
  zagolov.ps_plus(gettext("Сальдо"));
  zagolov.plus(":");

  data->saldo=rest_sal_kl(data->kodkl.ravno(),data->window);
  zagolov.plus(data->saldo);
 }
 
gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());


gtk_widget_show(data->label_kolstr);

if(data->metka_hapki == 1)
  gtk_widget_hide(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void restdok2_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

//printf("restdok2_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("П/д"), renderer,"text", COL_SKL,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"M", renderer,"text", COL_MZ,NULL);

renderer = gtk_cell_renderer_text_new ();
//g_object_set(G_OBJECT(renderer),"foreground","white",NULL);
//g_object_set(G_OBJECT(renderer),"background","black",NULL);


gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);
renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
"*", renderer,"text", COL_MR,NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Наименование"), renderer,
"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Е/и"), renderer,
"text", COL_EI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Количество"), renderer,
"text", COL_KOLIH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Цена"), renderer,
"text", COL_CENA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("П/c"), renderer,
"text", COL_PROC_SK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Цена со скидкой"), renderer,
"text", COL_CENA_SK,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Сумма"), renderer,
"text", COL_SUMA,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Коментарий"), renderer,
"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Метка списания"), renderer,"text", COL_SPISANIE,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

//printf("restdok2_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void restdok2_vibor(GtkTreeSelection *selection,class restdok2_data *data)
{
//printf("restdok2_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *skl;
gchar *kod;
gint  mz;
gint  nomer;
gint  nomerzap;


gtk_tree_model_get(model,&iter,COL_SKL,&skl,COL_KOD,&kod,NUM_COLUMNS,&nomer,\
NUM_COLUMNS+1,&mz,NUM_COLUMNS+2,&nomerzap,-1);

data->sklv=atoi(skl);
data->kodv=atoi(kod);
data->mzv=mz;
data->snanomer=nomer;
data->nz=nomerzap;

g_free(skl);
g_free(kod);

//printf("sklv=%d %d %d %d %d\n",data->sklv,data->kodv,nomer,mz,nomerzap);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  restdok2_knopka(GtkWidget *widget,class restdok2_data *data)
{
char    strsql[300];
iceb_u_str repl;
iceb_u_str kodv;
//int kodvskl=0;
iceb_u_str kod;
SQL_str row;
SQLCURSOR cur;
time_t vremo=0;
kod.plus("");
kodv.plus("");
int kodt=0;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("restdok2_knopka knop=%d\n",knop);
//data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {
  case F_SKLAD:

      if(l_spisok(0,0,&kodv,data->podr,data->window) == 0)
       {
        sprintf(strsql,"select kod,naik from Sklad where kod=%d",kodv.ravno_atoi());

        data->skladv.new_plus("");
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
         {
          data->skladv.new_plus(row[0]);
          if(data->metka_show == 1)
            l_restdok2_vgr(data);

          data->skladv.plus(" ");
          data->skladv.plus(row[1]);
         }
        gtk_label_set_text(GTK_LABEL(data->label_naim_skl),data->skladv.ravno_toutf());
       }
      return;

  case F_PODR:

      if(l_spisok(2,0,&kodv,0,data->window) == 0)
       {
        sprintf(strsql,"select kod,naik from Uslpodr where kod=%d",kodv.ravno_atoi());

        data->podrv.new_plus("");
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
         {
          data->podrv.new_plus(row[0]);
          if(data->metka_show == 2)
            l_restdok2_vgr(data);
          data->podrv.plus(" ");
          data->podrv.plus(row[1]);
         }
        gtk_label_set_text(GTK_LABEL(data->label_naim_podr),data->podrv.ravno_toutf());
       }
      return;

  case FK2:
    if(restdok2_prov_opl(data) != 0)
      return;

    if(data->metka_show == 1)
     {
      data->metka_show=0;
      gtk_widget_hide(data->gor_panel);
      return;
     }
    gtk_label_set_text(GTK_LABEL(data->label_mat_usl),gettext("Мат"));

    data->metka_show=1;
    if(config_dev.metka_screen == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(GTK_COMBO(data->windowvoditel)->entry),"activate");
    
    data->metka_mat_usl=0;
    l_restdok2_vgr(data);
    
  
    return;  

  case SFK2:
    if(restdok2_prov_opl(data) != 0)
      return;
    if(data->kolzap == 0)
      return;
    if(l_restdok_v(data->dd,data->md,data->gd,data->nomdok.ravno(),data->sklv,data->kodv,data->podr,1,data->mzv,data->nz,data->proc_sk,data->kodkl.ravno(),data->window) == 0)
     {
      if(data->metka_kontr_ost == 1)
        l_restdok2_vmu(data);
      restdok2_create_list(data);
     }
    return;  

  case FK3:
    if(restdok2_prov_opl(data) != 0)
      return;
    restdok2_udzap(data);
    return;  


  case FK5:
    if(data->kolzap == 0)
      return;
    restdok2_vrp(data);
    return;  

  case FK6:
    if(metka_crk == 1)
      return;
    if(data->mo == 1)
      return;
    if(data->kolzap == 0)
      return;
    
    repl.new_plus(gettext("Счет оплачен ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
     return;
    
    time(&vremo); 
    
    sprintf(strsql,"update Restdok set mo=1,vremo=%ld where god=%d and nomd='%s'",
    vremo,data->gd,data->nomdok.ravno());
    printf("strsql=%s\n",strsql);
    if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
     return;

    if(data->kodkl.getdlinna() > 1)
     {    
      //Если сальдо отрицательное - значит он оплатил эту сумму делаем запись внос налички
      if(data->saldo < -0.009)
        zap_v_kas(nomer_kas,data->kodkl.ravno(),data->saldo*-1,0,\
        "",0,0,0,data->podr,kod_operatora,"");
       
      //Записываем списание суммы если в счёте введён код клиента для того чтобы можно было получить 
      //реальное сальдо по клиенту
       
        zap_v_kas(nomer_kas,data->kodkl.ravno(),data->suma_k_oplate*-1,2,\
        data->nomdok.ravno(),data->dd,data->md,data->gd,data->podr,kod_operatora,data->koment.ravno());
      }
    data->mo=1;
    data->voz=1;

    restdok2_create_list(data);

    return;  


  case FK7:
    if(data->mo == 1)
      return;

    if(data->metka_show == 2)
     {
      data->metka_show=0;
//      gtk_widget_hide(data->hbox_gr_mu);
      gtk_widget_hide(data->gor_panel);
      return;
     }
    data->metka_show=2;
  
    if(config_dev.metka_screen == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(GTK_COMBO(data->widget_podr)->entry),"activate");

    gtk_label_set_text(GTK_LABEL(data->label_mat_usl),gettext("Усл."));
    data->metka_mat_usl=1;
    l_restdok2_vgr(data);

    return;  

  case FK8:
    if(data->mo == 1)
      return;
    
    if(config_dev.metka_screen == 0)
     {
      gtk_signal_emit_by_name(GTK_OBJECT(GTK_COMBO(data->windowvoditel)->entry),"activate");
      repl.new_plus(gettext("Введите код товара"));

      if(iceb_menu_vvod1(&repl,&kod,20,data->window) != 0)    
        return;  
     }

    if(config_dev.metka_screen == 1)
     {
      if(iceb_mous_klav(gettext("Введите код товара"),&kod,10,0,0,0,data->window) != 0)
          return;
     }
    if(l_restdok_v(data->dd,data->md,data->gd,data->nomdok.ravno(),data->skladv.ravno_atoi(),kod.ravno_atoi(),data->podr,0,0,0,data->proc_sk,data->kodkl.ravno(),data->window) == 0)
      {
       if(data->metka_kontr_ost == 1)
         l_restdok2_vmu(data);
       data->kodn=kodt;
       restdok2_create_list(data);
      }
    return;  

  case FK9:
    if(data->mo == 1)
      return;


    if(config_dev.metka_screen == 0)
     {
      gtk_signal_emit_by_name(GTK_OBJECT(GTK_COMBO(data->widget_podr)->entry),"activate");
      repl.new_plus(gettext("Введите код услуги"));
    
      if(iceb_menu_vvod1(&repl,&kod,20,data->window) != 0)
        return;  
     }
    if(config_dev.metka_screen == 1)
     if(iceb_mous_klav(gettext("Введите код услуги"),&kod,10,0,0,0,data->window) != 0)
       return;

    if(l_restdok_v(data->dd,data->md,data->gd,data->nomdok.ravno(),data->skladv.ravno_atoi(),kod.ravno_atoi(),data->podr,0,1,0,data->proc_sk,data->kodkl.ravno(),data->window) == 0)
       restdok2_create_list(data);

    return;  
  
  case SFK9: //Спрятать шапку

    data->metka_hapki++;

    if(data->metka_hapki == 2)
     data->metka_hapki=0;      

    if(data->metka_hapki == 1)
      gtk_widget_hide(data->label_kolstr);
    else
      gtk_widget_show(data->label_kolstr);
      
    return;
           
  case FK10:
//    printf("restdok2_knopka F10\n");
    l_restdok2_znm(data);
    
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   restdok2_key_press(GtkWidget *widget,GdkEventKey *event,class restdok2_data *data)
{
iceb_u_str repl;
//printf("restdok2_key_press keyval=%d state=%d\n",
//event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F2:
    if(data->kl_shift == 0)   
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    if(data->kl_shift == 1)   
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
    return(TRUE);
   
  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    return(TRUE);

  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    return(TRUE);

  case GDK_F6:
    if(metka_crk == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  case GDK_F7:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
    return(TRUE);

  case GDK_F8:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK8]),"clicked");
    return(TRUE);


  case GDK_F9:
    if(data->kl_shift == 0)   
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK9]),"clicked");
    if(data->kl_shift == 1)   
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK9]),"clicked");
    return(TRUE);

  
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(TRUE);

  case ICEB_REG_L:
  case ICEB_REG_R:
  //  printf("restdok2_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
  //  printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void restdok2_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class restdok2_data *data)
{
//printf("restdok2_v_row корректировка\n");
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
}

/****************************/
/*Удалить запись*/
/****************************/

void restdok2_udzap(class restdok2_data *data)
{
char strsql[300];

iceb_u_str titl;
iceb_u_str zagol;

sprintf(strsql,"%s %s",name_system,gettext("Удалить"));
titl.plus(strsql);
zagol.plus(gettext("Выберите нужное"));
iceb_u_spisok mv;

mv.plus(gettext("Удалить выбранную запись"));
mv.plus(gettext("Удалить документ"));

int nomer=iceb_menu_mv(&titl,&zagol,&mv,0,data->window);

iceb_u_str repl;

switch(nomer)
 {
  case -1:
    return;
    
  case 0:
    if(data->kolzap == 0)
      return;
    repl.plus(gettext("Удалить запись ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return;



    sprintf(strsql,"delete from Restdok1 where datd='%d-%d-%d' and \
    nomd='%s' and skl=%d and mz=%d and kodz=%d",
    data->gd,data->md,data->dd,
    data->nomdok.ravno(),
    data->sklv,data->mzv,data->kodv);

//    printf("restdok2_v_udzap %s\n",strsql);

    if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
     return;
    
    break;
  
  case 1:

    repl.plus(gettext("Удалить документ ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return;
  
    if(restdok_uddok(data->dd,data->md,data->gd,data->nomdok.ravno(),data->window) == 0)
     {
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
      data->voz=1;
      return;
     }    
        
    break;

}

restdok2_create_list(data);

}
/********************************/
/*меню выбора режима печати */
/*****************************/
void     restdok2_vrp(class restdok2_data *data)
{

//char bros[300];

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Распечатка счёта"));//0
punkt_m.plus(gettext("Распечатка по складам"));//1


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

switch (nomer)
 {
  case 0:
    restdok_rshet(data->dd,data->md,data->gd,data->nomdok.ravno(),data->window);
    return;
  case 1:
    restdok2_rskl(data->dd,data->md,data->gd,data->nomdok.ravno(),data->window);
    restdok2_create_list(data); //после распечатки по складам выставляються метки
    return;
 }

}
/*************************************/
/*Распечатка по складам              */
/*************************************/

void restdok2_rskl(short dd,short md,short gd,
const char *nomdok,
GtkWidget *wpredok)
{

SQLCURSOR cur;
SQL_str   row;
int       kolskl;
char      strsql[400];
iceb_u_str podr;
iceb_u_str naimpodr;


podr.new_plus("");  
naimpodr.new_plus("");

//читаем шапку

sprintf(strsql,"select podr from Restdok where datd='%d-%d-%d' and \
nomd='%s'",gd,md,dd,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  podr.new_plus(row[0]);  

  sprintf(strsql,"select naik from Restpod where kod=%s",podr.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
   naimpodr.new_plus(row[0]);

 }

//Определяем склады

sprintf(strsql,"select distinct skl from Restdok1 where datd='%d-%d-%d' and \
nomd='%s' and mz=0 and mr=0 order by skl asc",gd,md,dd,nomdok);
if((kolskl=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolskl == 0)
 return;

iceb_u_spisok skladi;

while(cur.read_cursor(&row) != 0)
  skladi.plus(row[0]);

char imaf[100];

for(int i=0; i < kolskl; i++)
 {
  sprintf(imaf,"skl%s-%d.lst",skladi.ravno(i),getpid());  
  restdok2_rskl1(dd,md,gd,nomdok,skladi.ravno(i),imaf,podr.ravno_atoi(),naimpodr.ravno(),wpredok);

 }  

}
/****************************************/
/*Распечатка по конкретному складу*/
/***********************************/
void restdok2_rskl1(short dd,short md,short gd,
const char *nomdok,
const char *skl,
const char *imaf,
int podr,
const char *naimpodr,
GtkWidget *wpredok)
{
SQLCURSOR cur;
SQLCURSOR cur1;
SQL_str   row;
SQL_str   row1;
int       kolstr;
char      strsql[400];

char name_printer[512];
sprintf(strsql,"Принтер для склада %s",skl);
iceb_u_str nast("restnast.alx");

//iceb_imafn("restnast.alx",&nast);
if(iceb_poldan(strsql,name_printer,nast.ravno(),wpredok) != 0)
 {  
//  printf("restdok2_rskl1 %s \n%s\n",strsql,nast.ravno());
  iceb_u_str repl;
  repl.plus(gettext("Не найдена настройка"));
  repl.plus("!!!");
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return;
  
 }

sprintf(strsql,"select kodz,ei,kolih from Restdok1 where datd='%d-%d-%d' and \
nomd='%s' and skl=%s and mz=0 and mr=0",gd,md,dd,nomdok,skl);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 return;
 
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;

 }
iceb_u_startfil(ff);
time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem);

fprintf(ff,"\n%s N%s\n",gettext("Счет"),nomdok);

fprintf(ff,"%s:%d %s\n",gettext("Подразделение"),podr,naimpodr);
fprintf(ff,"%s %d.%d.%d%s %s:%02d:%02d\n",
gettext("Распечатано"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

char naim[512];

fprintf(ff,"\
----------------------------------------------\n");

fprintf(ff,"%s\n",gettext(" Код |Наименование             |Ед.из.|Колич.|"));
/*
12345 1234567890123456789012345 123456 123456
*/
fprintf(ff,"\
----------------------------------------------\n");

while(cur.read_cursor(&row) != 0)
 {
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naimat from Material where kodm=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    strncpy(naim,row1[0],sizeof(naim)-1);            
  
  fprintf(ff,"%-5s %-25.25s %-6.6s %6.6g\n",
  row[0],naim,row[1],atof(row[2]));
  
  if(strlen(naim) > 15)
   fprintf(ff,"%-5s %s\n","",&naim[15]);
 }
fprintf(ff,"\
----------------------------------------------\n");

iceb_podpis(kod_operatora,ff,wpredok);

fclose(ff);

if(iceb_print(imaf,1,name_printer,"",wpredok) == 0)
 {
  sprintf(strsql,"update Restdok1 set mr=1 where datd='%d-%d-%d' and \
nomd='%s' and skl=%s and mr=0 and mz=0",gd,md,dd,nomdok,skl);

  iceb_sql_zapis(strsql,1,0,wpredok);
 }

unlink(imaf);

}
/*************************************/
/*Обработчик сигнала выбора склада*/
/************************************/
void  restdok2_sklad_get(GtkWidget *widget,class restdok2_data *data)
{
char strsql[512];
SQL_str row;
char kod[512];


data->skladv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

if(iceb_u_polen(data->skladv.ravno(),kod,sizeof(kod),1,' ') == 0)
 data->skladv.new_plus(kod);
 
sprintf(strsql,"select naik from Sklad where kod=%s",data->skladv.ravno_filtr_dpp());
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"Не найден склад %s !!!",data->skladv.ravno());
  repl.plus(strsql);
  iceb_menu_soob(&repl,NULL);
  data->skladv.new_plus("");
  gtk_entry_set_text(GTK_ENTRY(widget),"");
  return; 
 }

if(data->metka_show == 1)
  l_restdok2_vgr(data);
//printf("restdok2_sklad_get:data->metka_show=%d\n",data->metka_show);
data->skladv.plus(" ");
data->skladv.plus(row[0]);
 
gtk_entry_set_text(GTK_ENTRY(widget),data->skladv.ravno_toutf());
gtk_entry_select_region(GTK_ENTRY(widget),0,strlen(data->skladv.ravno_toutf()));

}
/***********************/
/*создание списка складов*/
/**************************/

void  restdok2_sp_sklad(GtkWidget **windowvoditel,iceb_u_str *skladv,int podr)
{

GList *glist=NULL;

iceb_u_str imafil("restnast.alx");
//iceb_imafn("restnast.alx",&imafil);
char spisok[1024];
memset(spisok,'\0',sizeof(spisok));
char strsql[512];

iceb_u_str spis_skl;
l_restdok_spis_skl(&spis_skl,podr);
strncpy(spisok,spis_skl.ravno(),sizeof(spisok)-1);

int kolih=iceb_u_pole2(spisok,',');

if(kolih == 0 && spisok[0] != '\0')
  kolih=1;

char  kod[56];
iceb_u_spisok sklad;
iceb_u_str    naim;
SQLCURSOR cur;
SQL_str   row;

for(int i=1; i <= kolih; i++)
 {

  memset(kod,'\0',sizeof(kod));
  if(kolih > 1)
    iceb_u_polen(spisok,kod,sizeof(kod),i,',');
  else
   strncpy(kod,spisok,sizeof(kod)-1);
   
  sprintf(strsql,"select naik from Sklad where kod=%s",kod);
  naim.new_plus(kod);        
  naim.plus(" ");        

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    naim.plus(row[0]);        
  if(i == 1)
   skladv->new_plus(naim.ravno());
  sklad.plus(naim.ravno_toutf());

 }

*windowvoditel=gtk_combo_new();

for(int i=0 ; i < kolih; i++)
  glist=g_list_append(glist,(void*)sklad.ravno(i));

if(kolih == 0)
 {
  skladv->new_plus("");
  glist=g_list_append(glist,spisok);
 }

gtk_combo_set_popdown_strings(GTK_COMBO(*windowvoditel),glist);
gtk_combo_disable_activate(GTK_COMBO(*windowvoditel));


}
/*************************************/
/*Обработчик сигнала выбора подразделения*/
/************************************/
void  restdok2_podr_get(GtkWidget *widget,class restdok2_data *data)
{
char strsql[512];
SQL_str row;
char kod[512];

//printf("voditel_get\n");

//strcpy(kod,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

data->podrv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
if(iceb_u_polen(data->podrv.ravno(),kod,sizeof(kod),1,' ') == 0)
 data->podrv.new_plus(kod);
 
sprintf(strsql,"select naik from Uslpodr where kod=%s",data->podrv.ravno_filtr_dpp());
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"Не найденo подразделение %s !!!",data->podrv.ravno());
  repl.plus(strsql);
  iceb_menu_soob(&repl,NULL);
  data->podrv.new_plus("");
  gtk_entry_set_text(GTK_ENTRY(widget),"");
  return; 
 }
if(data->metka_show == 2)
  l_restdok2_vgr(data);

data->podrv.plus(" ");
data->podrv.plus(row[0]);
 
gtk_entry_set_text(GTK_ENTRY(widget),data->podrv.ravno_toutf());
gtk_entry_select_region(GTK_ENTRY(widget),0,strlen(data->podrv.ravno_toutf()));

}

/***********************/
/*создание списка подразделений*/
/**************************/

void  restdok2_sp_podr(GtkWidget **widget_podr,iceb_u_str *podrv)
{

GList *glist=NULL;

iceb_u_str imafil("restnast.alx");
//iceb_imafn("restnast.alx",&imafil);
char spisok[1024];
memset(spisok,'\0',sizeof(spisok));

iceb_poldan("Список подразделений предоставляющих услуги",spisok,imafil.ravno(),NULL);

int kolih=iceb_u_pole2(spisok,',');

if(kolih == 0 && spisok[0] != '\0')
  kolih=1;
//printf("kolih=%d\n",kolih);
char  kod[56];
iceb_u_spisok sklad;
iceb_u_str    naim;
char strsql[512];
SQLCURSOR cur;
SQL_str   row;

for(int i=1; i <= kolih; i++)
 {

  memset(kod,'\0',sizeof(kod));
  if(kolih > 1)
    iceb_u_polen(spisok,kod,sizeof(kod),i,',');
  else
   strncpy(kod,spisok,sizeof(kod)-1);
   
  sprintf(strsql,"select naik from Uslpodr where kod=%s",kod);
  naim.new_plus(kod);        
  naim.plus(" ");        

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    naim.plus(row[0]);        
  if(i == 1)
   podrv->new_plus(naim.ravno());
  sklad.plus(naim.ravno_toutf());

 }

*widget_podr=gtk_combo_new();

for(int i=0 ; i < kolih; i++)
  glist=g_list_append(glist,(void*)sklad.ravno(i));

if(kolih == 0)
 {
  podrv->new_plus("");
  glist=g_list_append(glist,spisok);
 }

gtk_combo_set_popdown_strings(GTK_COMBO(*widget_podr),glist);
gtk_combo_disable_activate(GTK_COMBO(*widget_podr));

}
/************************/
/*Проверка оплаты документа*/
/*****************************/
//Если вернули 0-не оплачен 1-оплачен
int restdok2_prov_opl(class restdok2_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
sprintf(strsql,"select mo from Restdok where god=%d and nomd='%s'",
data->gd,data->nomdok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  iceb_u_str repl;
  repl.plus("%s-Не найдена шапка документа !",__FUNCTION__);
  iceb_menu_soob(&repl,data->window);
  return(0);
 } 
data->mo=atoi(row[0]);
if(data->mo == 1)
 {
  restdok2_create_list(data);
  return(1);   
 }

return(0);


}

/*******************************************************************/

/*********************************************************************/

/***********************************/
/*Стат на нужную строку*/
/************************************/
void l_restdok2_setstr(class restdok2_data_sp *data)
{
//printf("l_restdok2_setstr-Стать на нужную строку\n");

if(data->snanomer  > data->kolzap-1)
   data->snanomer= data->kolzap-1;
if(data->snanomer < 0)
   data->snanomer=0;
      
char strsql[56];
sprintf(strsql,"%d",data->snanomer);
GtkTreePath *path=gtk_tree_path_new_from_string(strsql);

//  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,TRUE,0,0);
gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
gtk_tree_path_free(path);
}


/*************************/
/*Кнопка вниз нажата*/
/**********************************/
void  l_restdok2_knopka_vniz_press(GtkWidget *widget,class restdok2_data_sp *data)
{
//printf("кнопка вниз нажата\n");
data->metka_released=1;
while(data->metka_released == 1)
 {
  data->snanomer++;
//  printf("вниз-%d\n",data->snanomer);

  l_restdok2_setstr(data); //стать на нужную строку

  g_usleep(SLEEP_STR);

  iceb_refresh();  //обязательно после sleep
 }


}
/*************************/
/*Кнопка вверх нажата*/
/**********************************/

void  l_restdok2_knopka_vverh_press(GtkWidget *widget,class restdok2_data_sp *data)
{
//printf("кнопка вверх нажата\n");
data->metka_released=1;
while(data->metka_released == 1)
 {
  data->snanomer--;
//  printf("вверх-%d\n",data->snanomer);

  l_restdok2_setstr(data); //стать на нужную строку


  g_usleep(SLEEP_STR);

  iceb_refresh();  //обязательно после sleep
 }


}
/*************************/
/*Кнопка вниз/вверх отжата*/
/**********************************/
void  l_restdok2_knopka_released(GtkWidget *widget,class restdok2_data_sp *data)
{
//printf("кнопка отжата\n");
data->metka_released=0;
}
/*****************************/
/*Обработчик нажатия кнопки выбора из списка материалов и услуг */
/*****************************/
void  restdok2_mu_knopka(GtkWidget *widget,class restdok2_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("restdok2_knopka knop=%d\n",knop);
//data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается
int kod=0;

if(data->metka_mat_usl == 0)
 kod=data->skladv.ravno_atoi();
if(data->metka_mat_usl == 1)
 kod=data->podrv.ravno_atoi();
 
switch ((gint)knop)
 {
  case MU_FK_VIBOR:

//      printf("restdok2_mu_knopka:kod=%d data->kod_matus=%d\n",kod,data->kod_matus);
      
      if(l_restdok_v(data->dd,data->md,data->gd,data->nomdok.ravno(),kod,data->kod_matus,data->podr,0,data->metka_mat_usl,0,data->proc_sk,data->kodkl.ravno(),data->window) == 0)
       {
        if(data->metka_mat_usl == 0)
         if(data->metka_kontr_ost == 1)
           l_restdok2_vmu(data);
        data->kodn=data->kod_matus;
        restdok2_create_list(data);
       }

      return;

  case MU_FK_OSTATOK:

    data->metka_kontr_ost++;

    if(data->metka_kontr_ost > 1)
     data->metka_kontr_ost=0;

    l_restdok2_vmu(data);
    return;
 }
}

/********************************************************/
/*Создаём горизонтальный бокс со списками групп и материалов/услуг*/
/*******************************************************************/

void restdo2_hbox_gr_mu(class restdok2_data *data)
{
GtkWidget *hbox_gr = gtk_hbox_new (FALSE, 1); //Горизонтальный бокс для размещения групп
GtkWidget *hbox_mu = gtk_hbox_new (FALSE, 1); //Горизонтальный бокс для размещения материаллов/услуг

gtk_paned_add1(GTK_PANED(data->gor_panel),hbox_gr);
gtk_paned_add2(GTK_PANED(data->gor_panel),hbox_mu);

GtkWidget *vbox_kn1 = gtk_vbox_new (FALSE, 1); //Вертикальный бокс для кнопок выбора группы
GtkWidget *vbox_kn2 = gtk_vbox_new (FALSE, 1); //Вертикальный бокс для кнопок выбора материала/услуги


//вставляем окно для отображения списка групп
data->spisok_gr.sw = gtk_scrolled_window_new (NULL, NULL);
//gtk_widget_set_usize(GTK_WIDGET(data.sw),600,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data->spisok_gr.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data->spisok_gr.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (hbox_gr), data->spisok_gr.sw, TRUE, TRUE, 0);


//вставляем вертикальный бокс для кнопок выбора записи
gtk_box_pack_start(GTK_BOX (hbox_gr),vbox_kn1,FALSE,FALSE,0);


//вставляем окно для отображения списка материалов/услуг
data->spisok_mu.sw = gtk_scrolled_window_new (NULL, NULL);
//gtk_widget_set_usize(GTK_WIDGET(data.sw),600,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data->spisok_mu.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data->spisok_mu.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (hbox_mu), data->spisok_mu.sw, TRUE, TRUE, 0);

//вставляем вертикальный бокс для кнопок выбора материала/услуги
gtk_box_pack_start(GTK_BOX (hbox_mu),vbox_kn2,FALSE,FALSE,0);

//вставляем кнопки для выбора групп в вертикальный бокс

data->label_mat_usl=gtk_label_new ("***");
GdkColor color;
//gdk_color_parse("green",&color);
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data->label_mat_usl,GTK_STATE_NORMAL,&color);
gtk_box_pack_start (GTK_BOX (vbox_kn1),data->label_mat_usl,FALSE, FALSE, 0);

GtkWidget *arrow=gtk_arrow_new(GTK_ARROW_UP,GTK_SHADOW_OUT);

data->knopka_gr[GR_FK_VVERH]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data->knopka_gr[GR_FK_VVERH]),arrow);

gtk_box_pack_start(GTK_BOX(vbox_kn1), data->knopka_gr[GR_FK_VVERH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data->knopka_gr[GR_FK_VVERH]), "pressed",GTK_SIGNAL_FUNC(l_restdok2_knopka_vverh_press),&data->spisok_gr);
gtk_signal_connect(GTK_OBJECT(data->knopka_gr[GR_FK_VVERH]), "released",GTK_SIGNAL_FUNC(l_restdok2_knopka_released),&data->spisok_gr);
//tooltips[GR_FK_VVERH]=gtk_tooltips_new();
//gtk_tooltips_set_tip(tooltips[GR_FK_VVERH],data->knopka_gr[GR_FK_VVERH],gettext("Выбор отмеченной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data->knopka_gr[GR_FK_VVERH]),(gpointer)GR_FK_VVERH);
gtk_widget_show(data->knopka_gr[GR_FK_VVERH]);
gtk_widget_show(arrow);

arrow=gtk_arrow_new(GTK_ARROW_DOWN,GTK_SHADOW_OUT);


data->knopka_gr[GR_FK_VNIZ]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data->knopka_gr[GR_FK_VNIZ]),arrow);

gtk_box_pack_start(GTK_BOX(vbox_kn1), data->knopka_gr[GR_FK_VNIZ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data->knopka_gr[GR_FK_VNIZ]), "pressed",GTK_SIGNAL_FUNC(l_restdok2_knopka_vniz_press),&data->spisok_gr);
gtk_signal_connect(GTK_OBJECT(data->knopka_gr[GR_FK_VNIZ]), "released",GTK_SIGNAL_FUNC(l_restdok2_knopka_released),&data->spisok_gr);
//tooltips[GR_FK_VNIZ]=gtk_tooltips_new();
//gtk_tooltips_set_tip(tooltips[GR_FK_VNIZ],data->knopka_gr[GR_FK_VNIZ],gettext("Выбор отмеченной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data->knopka_gr[GR_FK_VNIZ]),(gpointer)GR_FK_VNIZ);
gtk_widget_show(data->knopka_gr[GR_FK_VNIZ]);
gtk_widget_show(arrow);



//вставляем кнопки для выбора материаллов/услуг
GtkTooltips *tooltips[KOL_MU_FK];


data->knopka_mu[MU_FK_VIBOR]=gtk_button_new_with_label(gettext("Выбор"));
gtk_box_pack_start(GTK_BOX(vbox_kn2), data->knopka_mu[MU_FK_VIBOR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data->knopka_mu[MU_FK_VIBOR]), "clicked",GTK_SIGNAL_FUNC(restdok2_mu_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(data->knopka_mu[MU_FK_VIBOR]),(gpointer)MU_FK_VIBOR);
tooltips[MU_FK_VIBOR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[MU_FK_VIBOR],data->knopka_mu[MU_FK_VIBOR],gettext("Ввод нового товара в счёт"),NULL);


arrow=gtk_arrow_new(GTK_ARROW_UP,GTK_SHADOW_OUT);

data->knopka_mu[MU_FK_VVERH]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data->knopka_mu[MU_FK_VVERH]),arrow);

gtk_box_pack_start(GTK_BOX(vbox_kn2), data->knopka_mu[MU_FK_VVERH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data->knopka_mu[MU_FK_VVERH]), "pressed",GTK_SIGNAL_FUNC(l_restdok2_knopka_vverh_press),&data->spisok_mu);
gtk_signal_connect(GTK_OBJECT(data->knopka_mu[MU_FK_VVERH]), "released",GTK_SIGNAL_FUNC(l_restdok2_knopka_released),&data->spisok_mu);
gtk_object_set_user_data(GTK_OBJECT(data->knopka_mu[MU_FK_VVERH]),(gpointer)MU_FK_VVERH);
gtk_widget_show(data->knopka_mu[MU_FK_VVERH]);
gtk_widget_show(arrow);
tooltips[MU_FK_VVERH]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[MU_FK_VVERH],data->knopka_mu[MU_FK_VVERH],gettext("Вверх на одну строку"),NULL);

arrow=gtk_arrow_new(GTK_ARROW_DOWN,GTK_SHADOW_OUT);


data->knopka_mu[MU_FK_VNIZ]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data->knopka_mu[MU_FK_VNIZ]),arrow);

gtk_box_pack_start(GTK_BOX(vbox_kn2), data->knopka_mu[MU_FK_VNIZ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data->knopka_mu[MU_FK_VNIZ]), "pressed",GTK_SIGNAL_FUNC(l_restdok2_knopka_vniz_press),&data->spisok_mu);
gtk_signal_connect(GTK_OBJECT(data->knopka_mu[MU_FK_VNIZ]), "released",GTK_SIGNAL_FUNC(l_restdok2_knopka_released),&data->spisok_mu);
gtk_object_set_user_data(GTK_OBJECT(data->knopka_mu[MU_FK_VNIZ]),(gpointer)MU_FK_VNIZ);
gtk_widget_show(data->knopka_mu[MU_FK_VNIZ]);
tooltips[MU_FK_VNIZ]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[MU_FK_VNIZ],data->knopka_mu[MU_FK_VNIZ],gettext("Вниз на одну строку"),NULL);

gtk_widget_show(arrow);

//if(data->metka_mat_usl == 0)
// {
  data->knopka_mu[MU_FK_OSTATOK]=gtk_button_new_with_label(gettext("Остаток"));
  gtk_box_pack_start(GTK_BOX(vbox_kn2), data->knopka_mu[MU_FK_OSTATOK], TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(data->knopka_mu[MU_FK_OSTATOK]), "clicked",GTK_SIGNAL_FUNC(restdok2_mu_knopka),data);
  gtk_object_set_user_data(GTK_OBJECT(data->knopka_mu[MU_FK_OSTATOK]),(gpointer)MU_FK_OSTATOK);
  tooltips[MU_FK_OSTATOK]=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips[MU_FK_OSTATOK],data->knopka_mu[MU_FK_OSTATOK],gettext("Включить/выключить показ записей с остатками"),NULL);
// }
 
}




