/*$Id: admin_icebuser.c,v 1.5 2011-02-21 07:35:50 sasa Exp $*/
/*14.09.2010	14.09.2010	Белых А.И.	admin_icebuser.c
Работа со списком операторов
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
//zs
//#include  "buhg_g.h"
#include  "../headers/buhg_g.h"
//ze

enum
{
  COL_LOG,
  COL_UN,
  COL_FIO,
  COL_KOMENT,
  COL_GK,
  COL_MU,
  COL_PD,
  COL_ZP,
  COL_UOS,
  COL_UU,
  COL_UKO,
  COL_UKR,
  COL_UPL,
  COL_RNN,
  COL_UD,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK2,
  FK3,
  FK5,
  FK6,
  FK10,
  KOL_F_KL
};

class  admin_icebuser_data
 {
  public:


  class iceb_u_str logv;
  class iceb_u_str logtv; //только что введённая единица измерения
  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  
  class iceb_u_str imabaz;
  int user;
  //Конструктор
  admin_icebuser_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    logv.plus("");
    logtv.plus("");
    user=0;
   }      
 };

gboolean   admin_icebuser_key_press(GtkWidget *widget,GdkEventKey *event,class admin_icebuser_data *data);
void admin_icebuser_vibor(GtkTreeSelection *selection,class admin_icebuser_data *data);
void admin_icebuser_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class admin_icebuser_data *data);
void  admin_icebuser_knopka(GtkWidget *widget,class admin_icebuser_data *data);
void admin_icebuser_add_columns (GtkTreeView *treeview);
void admin_icebuser_udzap(class admin_icebuser_data *data);
void admin_icebuser_rasp(class admin_icebuser_data *data);
void admin_icebuser_create_list (class admin_icebuser_data *data);

int admin_icebuser_v(class iceb_u_str *kod_gr,GtkWidget *wpredok);
int admin_icebuser_p(class admin_icebuser_rek *rek_poi,GtkWidget *wpredok);
int admin_icebuser_v(int user,GtkWidget *wpredok);
int admin_icebuser_v1(int user,GtkWidget *wpredok);
int admin_icebuser(int nom_op,GtkWidget *wpredok);


extern SQL_baza  bd;
extern char      *name_system;

int admin_icebuser(const char *imabazv,GtkWidget *wpredok)
{
admin_icebuser_data data;
char bros[512];
GdkColor color;
sprintf(bros,"USE %s",imabazv);

if(iceb_sql_zapis(bros,1,0,wpredok) != 0)
 return(1);
/*проверяем есть ли в базе нужная талица*/
sprintf(bros,"select un from icebuser limit 1");
if(sql_readkey(&bd,"select un from icebuser limit 1") < 0)
 {
  iceb_menu_soob(gettext("Это не linuxbuh база данных!"),wpredok);
  return(1);
 }
data.imabaz.new_plus(imabazv); 
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Список операторов"));

gtk_window_set_title (GTK_WINDOW (data.window),bros);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(admin_icebuser_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список операторов"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",gettext("Корректировка"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(admin_icebuser_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);


sprintf(bros,"F3 %s",gettext("Проверка"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(admin_icebuser_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Проверка возможности удаления"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(admin_icebuser_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Разрешения"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(admin_icebuser_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Установка разрешений"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(admin_icebuser_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

admin_icebuser_create_list(&data);

gtk_widget_show(data.window);
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void admin_icebuser_create_list (class admin_icebuser_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;
//GdkColor color;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(admin_icebuser_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(admin_icebuser_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
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
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select * from icebuser order by login asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
float kolstr1=0. ;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  


  if(iceb_u_SRAV(data->logtv.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  //Логин
  ss[COL_LOG].new_plus(iceb_u_toutf(row[0]));
  
  //Фамилия
  ss[COL_FIO].new_plus(iceb_u_toutf(row[1]));

  /*Уникальный номер оператора*/
  ss[COL_UN].new_plus(row[2]);
  /*КОМЕНТАРИЙ*/
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[3]));
  
  /*Гланая книга*/
  if(atoi(row[6]) == 0)  
   ss[COL_GK].new_plus("Y-");
  else
   ss[COL_GK].new_plus("N-");

  if(atoi(row[7]) == 0)  
   ss[COL_GK].plus("Y-");
  else
   ss[COL_GK].plus("N-");

  if(atoi(row[8]) == 0)  
   ss[COL_GK].plus("Y");
  else
   ss[COL_GK].plus("N");

  /*Материальный учёт*/
  if(atoi(row[9]) == 0)  
   ss[COL_MU].new_plus("Y-");
  else
   ss[COL_MU].new_plus("N-");

  if(atoi(row[10]) == 0)  
   ss[COL_MU].plus("Y-");
  else
   ss[COL_MU].plus("N-");

  if(atoi(row[11]) == 0)  
   ss[COL_MU].plus("Y");
  else
   ss[COL_MU].plus("N");

  /*Платёжные документы*/
  if(atoi(row[12]) == 0)  
   ss[COL_PD].new_plus("Y-");
  else
   ss[COL_PD].new_plus("N-");

  if(atoi(row[13]) == 0)  
   ss[COL_PD].plus("Y-");
  else
   ss[COL_PD].plus("N-");

  if(atoi(row[14]) == 0)  
   ss[COL_PD].plus("Y");
  else
   ss[COL_PD].plus("N");

  /*Заработная плата*/
  if(atoi(row[15]) == 0)  
   ss[COL_ZP].new_plus("Y-");
  else
   ss[COL_ZP].new_plus("N-");

  if(atoi(row[16]) == 0)  
   ss[COL_ZP].plus("Y-");
  else
   ss[COL_ZP].plus("N-");

  if(atoi(row[17]) == 0)  
   ss[COL_ZP].plus("Y");
  else
   ss[COL_ZP].plus("N");

  /*Учёт основных средств*/
  if(atoi(row[18]) == 0)  
   ss[COL_UOS].new_plus("Y-");
  else
   ss[COL_UOS].new_plus("N-");

  if(atoi(row[19]) == 0)  
   ss[COL_UOS].plus("Y-");
  else
   ss[COL_UOS].plus("N-");

  if(atoi(row[20]) == 0)  
   ss[COL_UOS].plus("Y");
  else
   ss[COL_UOS].plus("N");

  /*Учёт услуг*/
  if(atoi(row[21]) == 0)  
   ss[COL_UU].new_plus("Y-");
  else
   ss[COL_UU].new_plus("N-");

  if(atoi(row[22]) == 0)  
   ss[COL_UU].plus("Y-");
  else
   ss[COL_UU].plus("N-");

  if(atoi(row[23]) == 0)  
   ss[COL_UU].plus("Y");
  else
   ss[COL_UU].plus("N");

  /*Учёт кассовых ордеров*/
  if(atoi(row[24]) == 0)  
   ss[COL_UKO].new_plus("Y-");
  else
   ss[COL_UKO].new_plus("N-");

  if(atoi(row[25]) == 0)  
   ss[COL_UKO].plus("Y-");
  else
   ss[COL_UKO].plus("N-");

  if(atoi(row[26]) == 0)  
   ss[COL_UKO].plus("Y");
  else
   ss[COL_UKO].plus("N");

  /*Учёт командировочных расходов*/
  if(atoi(row[27]) == 0)  
   ss[COL_UKR].new_plus("Y-");
  else
   ss[COL_UKR].new_plus("N-");

  if(atoi(row[28]) == 0)  
   ss[COL_UKR].plus("Y-");
  else
   ss[COL_UKR].plus("N-");

  if(atoi(row[29]) == 0)  
   ss[COL_UKR].plus("Y");
  else
   ss[COL_UKR].plus("N");

  /*Учёт путевых листов*/
  if(atoi(row[30]) == 0)  
   ss[COL_UPL].new_plus("Y-");
  else
   ss[COL_UPL].new_plus("N-");

  if(atoi(row[31]) == 0)  
   ss[COL_UPL].plus("Y-");
  else
   ss[COL_UPL].plus("N-");

  if(atoi(row[32]) == 0)  
   ss[COL_UPL].plus("Y");
  else
   ss[COL_UPL].plus("N");

  /*Реестр счет-фактур*/
  if(atoi(row[33]) == 0)  
   ss[COL_RNN].new_plus("Y-");
  else
   ss[COL_RNN].new_plus("N-");

  if(atoi(row[34]) == 0)  
   ss[COL_RNN].plus("Y-");
  else
   ss[COL_RNN].plus("N-");

  if(atoi(row[35]) == 0)  
   ss[COL_RNN].plus("Y");
  else
   ss[COL_RNN].plus("N");

  /*Учёт доверенностей*/
  if(atoi(row[36]) == 0)  
   ss[COL_UD].new_plus("Y-");
  else
   ss[COL_UD].new_plus("N-");

  if(atoi(row[37]) == 0)  
   ss[COL_UD].plus("Y-");
  else
   ss[COL_UD].plus("N-");

  if(atoi(row[38]) == 0)  
   ss[COL_UD].plus("Y");
  else
   ss[COL_UD].plus("N");

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[5])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_u_kszap(row[4],0));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_LOG,ss[COL_LOG].ravno(),
  COL_UN,ss[COL_UN].ravno(),
  COL_FIO,ss[COL_FIO].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_GK,ss[COL_GK].ravno(),
  COL_MU,ss[COL_MU].ravno(),
  COL_PD,ss[COL_PD].ravno(),
  COL_ZP,ss[COL_ZP].ravno(),
  COL_UOS,ss[COL_UOS].ravno(),
  COL_UU,ss[COL_UU].ravno(),
  COL_UKO,ss[COL_UKO].ravno(),
  COL_UKR,ss[COL_UKR].ravno(),
  COL_UPL,ss[COL_UPL].ravno(),
  COL_RNN,ss[COL_RNN].ravno(),
  COL_UD,ss[COL_UD].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->logtv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

admin_icebuser_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список операторов"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());
/*******************************
if(data->metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.kod.ravno(),gettext("Код"));
  iceb_str_poisk(&zagolov,data->poisk.naim.ravno(),gettext("Наименование"));
  

  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
******************************/
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

}

/*****************/
/*Создаем колонки*/
/*****************/

void admin_icebuser_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Логин"),renderer,"text",COL_LOG,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_LOG);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("No",renderer,"text",COL_UN,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_UN);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Фамилия"),renderer,"text",COL_FIO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_FIO);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Коментарий"),renderer,"text",COL_KOMENT,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOMENT);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Главная\nкнига"),renderer,"text",COL_GK,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_GK);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Материальный\nучёт"),renderer,"text",COL_MU,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_MU);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Платёжные\nдокументы"),renderer,"text",COL_PD,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_PD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Заработная\nплата"),renderer,"text",COL_ZP,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_ZP);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Учёт основных\nсредств"),renderer,"text",COL_UOS,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_UOS);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Учёт\nуслуг"),renderer,"text",COL_UU,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_UU);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Учёт кассовых\nордеров"),renderer,"text",COL_UKO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_UKO);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Учёт командировочных\nрасходов"),renderer,"text",COL_UKR,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_UKR);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Учёт путевых\nлистов"),renderer,"text",COL_UPL,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_UPL);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Реестр счет-\nфактур"),renderer,"text",COL_RNN,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_RNN);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Учёт\nдоверенностей"),renderer,"text",COL_UD,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_UD);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата и время записи"),renderer,"text",COL_DATA_VREM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_VREM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Кто записал"),renderer,"text",COL_KTO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KTO);
gtk_tree_view_append_column (treeview, column);
}

/****************************/
/*Выбор строки*/
/**********************/

void admin_icebuser_vibor(GtkTreeSelection *selection,class admin_icebuser_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gint  nomer;
gchar *user;

gtk_tree_model_get(model,&iter,COL_LOG,&kod,COL_UN,&user,NUM_COLUMNS,&nomer,-1);

data->logv.new_plus(iceb_u_fromutf(kod));
data->user=atoi(user);
data->snanomer=nomer;

g_free(kod);
g_free(user);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  admin_icebuser_knopka(GtkWidget *widget,class admin_icebuser_data *data)
{
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("admin_icebuser_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
     if(admin_icebuser_v(data->user,data->window) == 0)
      admin_icebuser_create_list(data);
    return;  


  case FK3:
    if(data->kolzap == 0)
      return;
    admin_icebuser_udzap(data);
    admin_icebuser_create_list(data);
    return;  
  


  case FK5:
    admin_icebuser_rasp(data);
    return;  

  case FK6:
    if(admin_icebuser_v1(data->user,data->window) == 0)
      admin_icebuser_create_list(data);
    return;  
    
  case FK10:
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   admin_icebuser_key_press(GtkWidget *widget,GdkEventKey *event,class admin_icebuser_data *data)
{
iceb_u_str repl;

switch(event->keyval)
 {

  case GDK_F2:

    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

    return(TRUE);
   
  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    return(TRUE);


  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    return(TRUE);


  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:

    data->kl_shift=1;

    return(TRUE);

  default:
//    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void admin_icebuser_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class admin_icebuser_data *data)
{

gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");


}

/*****************************/
/*Удаление записи            */
/*****************************/

void admin_icebuser_udzap(class admin_icebuser_data *data)
{

prov_uduserw(data->logv.ravno(),data->imabaz.ravno(),1,data->window);

}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void admin_icebuser_rasp(class admin_icebuser_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
class iceb_u_spisok imaf;
class iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from icebuser order by login asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"icebusers%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список операторов"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

struct tm *bf;
time_t     vrem;
time(&vrem);
bf=localtime(&vrem);

fprintf(ff,"%s\n",gettext("Список операторов"));
fprintf(ff,"\
%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);


/*********************
if(data->metka_poi == 1)
 {
  if(data->poisk.kod.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код"),data->poisk.kod.ravno());
  if(data->poisk.naim.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Наименование"),data->poisk.naim.ravno());
 }
*********************/
fprintf(ff,"%s:%s\n",
gettext("База данных"),data->imabaz.ravno());

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 Логин |Номер|      Фамилия       |Глав.|Матер|Плат.|Зарпл|Учёт |Учёт |Учёт |Учёт |Учёт |Реест|Учёт |Коментарий\n\
       |     |                    |книга|учёт |докум| ата |основ|услуг|кассо|коман|путев|налог|довер|\n");
/*
1234567 12345 12345678901234567890 12345
*/
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;


cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  fprintf(ff,"%-7s %-5s %-*.*s|",
  row[0],
  row[2],
  iceb_u_kolbait(20,row[1]),
  iceb_u_kolbait(20,row[1]),
  row[1]);
  for(int ii=6; ii < 6+33; ii+=3)
   {
    if(atoi(row[ii]) == 0)
     fprintf(ff,"Y ");    
    else
     fprintf(ff,"N ");    

    if(atoi(row[ii+1]) == 0)
     fprintf(ff,"Y ");    
    else
     fprintf(ff,"N ");    

    if(atoi(row[ii+2]) == 0)
     fprintf(ff,"Y|");    
    else
     fprintf(ff,"N|");    
   }
  fprintf(ff,"%s\n",row[3]);
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
 }


fprintf(ff,"\
                                   | | |\n\
                                   | | ->%s\n\
                                   | ->%s\n\
                                   ->%s\n",
gettext("Разрешение на настройку подсистемы"),
gettext("Разрешение на блокировку подсистемы"),
gettext("Разрешение на работу с подсистемой"));

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);


iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
