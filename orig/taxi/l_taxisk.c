/*$Id: l_taxisk.c,v 1.6 2011-02-21 07:36:20 sasa Exp $*/
/*29.05.2009	29.05.2009	Белых А.И.	l_taxisk.c
Работа со списком сальдо по клиентам
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include  "taxi.h"
#include  "l_taxisk.h"

enum
{
  COL_KODKL,
  COL_FIO,
  COL_SALDO,
  COL_ADRES,
  COL_TELEF,
  COL_DENROG,
  COL_KOMENT,
  COL_POL,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK2,
  FK3,
  SFK3,
  FK4,
  FK5,
  FK6,
  FK10,
  SFK2,
  KOL_F_KL
};

class  l_taxisk_data
 {
  public:

  l_taxisk_rek poisk;

  iceb_u_str kodklv;
  iceb_u_str fiov;
  iceb_u_str kodkl_tv; //только что введённая единица измерения
  
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
  short     metka_rr;   //0-работа со списком 1-выбор
  short metka_saldo; /*0-без сальдо 1-расчёт сальдо*/

  //Конструктор
  l_taxisk_data()
   {
    snanomer=0;
    metka_rr=metka_voz=kl_shift=0;
    window=treeview=NULL;
    kodkl_tv.plus("");
    metka_saldo=0;
   }      
 };

gboolean   l_taxisk_key_press(GtkWidget *widget,GdkEventKey *event,class l_taxisk_data *data);
void l_taxisk_vibor(GtkTreeSelection *selection,class l_taxisk_data *data);
void l_taxisk_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_taxisk_data *data);
void  l_taxisk_knopka(GtkWidget *widget,class l_taxisk_data *data);
void l_taxisk_add_columns(GtkTreeView *treeview,short metka_saldo);
void l_taxisk_udzap(class l_taxisk_data *data);
int  l_taxisk_prov_row(SQL_str row,class l_taxisk_data *data);
void l_taxisk_rasp(class l_taxisk_data *data);
int  l_taxisk_pvu(int,const char *kod,GtkWidget *wpredok);
void l_taxisk_create_list (class l_taxisk_data *data);

int l_taxisk_v(class iceb_u_str *kod_gr,GtkWidget *wpredok);
int l_taxisk_p(class l_taxisk_rek *rek_poi,GtkWidget *wpredok);
void l_gruppdok_unk(class l_taxisk_data *data);


extern char *organ;
extern SQL_baza  bd;
extern char      *name_system;

int   l_taxisk(int metka_rr, //0-ввод и корек. 1-выбор
iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok)
{
l_taxisk_data data;
char bros[300];
GdkColor color;

data.poisk.clear_data();
data.metka_rr=metka_rr;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,iceb_u_toutf("Список клиентов"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_taxisk_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (iceb_u_toutf("Список сальдо по клиетам"));
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

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_taxisk_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_taxisk_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_taxisk_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(l_taxisk_knopka),&data);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удалить неиспользуемые коды"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_taxisk_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_taxisk_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Сальдо"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(l_taxisk_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Включить/выключить расчёт сальдо"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_taxisk_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_taxisk_create_list(&data);

gtk_widget_show(data.window);
if(metka_rr == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();

if(data.metka_voz == 0)
 {
  kod->new_plus(data.kodklv.ravno());
  naim->new_plus(data.fiov.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_taxisk_create_list (class l_taxisk_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[300];
int  kolstr=0;
SQL_str row;
//GdkColor color;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_taxisk_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_taxisk_vibor),data);

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
G_TYPE_INT);


sprintf(strsql,"select * from Taxiklient order by kod asc");

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
  
  if(l_taxisk_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->kodkl_tv.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  ss[COL_SALDO].new_plus("");
  if(data->metka_saldo != 0)
   {
    ss[COL_SALDO].new_plus(taxi_saldo(row[0]));
   }
  //Код
  ss[COL_KODKL].new_plus(iceb_u_toutf(row[0]));
  
  //ФАМИЛИЯ
  ss[COL_FIO].new_plus(iceb_u_toutf(row[1]));

  /*Адрес*/
  ss[COL_ADRES].new_plus(iceb_u_toutf(row[2]));
  /*Телефон*/
  ss[COL_TELEF].new_plus(iceb_u_toutf(row[3]));
  /*Пол*/
  if(atoi(row[11]) == 0)
   ss[COL_POL].new_plus(gettext("Мужчина"));
  else  
   ss[COL_POL].new_plus(gettext("Женщина"));
  /*Коментарий*/
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[13]));
  
  /*Дата рождения*/
  if(row[12][0] != '0')
    ss[COL_DENROG].new_plus(iceb_u_sqldata(row[12]));
  else
    ss[COL_DENROG].new_plus("");
      
  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[5])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_u_kszap(row[4],0));

    
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KODKL,ss[COL_KODKL].ravno(),
  COL_FIO,ss[COL_FIO].ravno(),
  COL_SALDO,ss[COL_SALDO].ravno(),
  COL_ADRES,ss[COL_ADRES].ravno(),
  COL_TELEF,ss[COL_TELEF].ravno(),
  COL_DENROG,ss[COL_DENROG].ravno(),
  COL_POL,ss[COL_POL].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kodkl_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_taxisk_add_columns (GTK_TREE_VIEW (data->treeview),data->metka_saldo);


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus("Список клиентов");

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

if(data->poisk.metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.kodkl.ravno(),gettext("Код"));
  iceb_str_poisk(&zagolov,data->poisk.fio.ravno(),gettext("Фамилия"));
  iceb_str_poisk(&zagolov,data->poisk.adres.ravno(),gettext("Адрес"));
  iceb_str_poisk(&zagolov,data->poisk.telef.ravno(),gettext("Телефон"));
  iceb_str_poisk(&zagolov,data->poisk.denrog.ravno(),gettext("Дата рождения"));
  iceb_str_poisk(&zagolov,data->poisk.koment.ravno(),gettext("Коментарий"));
  if(data->poisk.pol == 1)
     iceb_str_poisk(&zagolov,gettext("Мужчина"),gettext("Пол"));
  if(data->poisk.pol == 2)
     iceb_str_poisk(&zagolov,gettext("Женщина"),gettext("Пол"));


  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_taxisk_add_columns(GtkTreeView *treeview,short metka_saldo)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Код"),renderer,"text",COL_KODKL,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KODKL);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Фамилия"),renderer,"text",COL_FIO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_FIO);
gtk_tree_view_append_column (treeview, column);

if(metka_saldo != 0)
 {
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes (gettext("Сальдо"),renderer,"text",COL_SALDO,NULL);
  gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
  gtk_tree_view_column_set_sort_column_id (column, COL_SALDO);
  gtk_tree_view_append_column (treeview, column);
 }

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Адрес"),renderer,"text",COL_ADRES,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_ADRES);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Телефон"),renderer,"text",COL_TELEF,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_TELEF);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Пол"),renderer,"text",COL_POL,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_POL);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата рождения"),renderer,"text",COL_DENROG,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DENROG);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Коментарий"),renderer,"text",COL_KOMENT,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOMENT);
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

void l_taxisk_vibor(GtkTreeSelection *selection,class l_taxisk_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_KODKL,&kod,COL_FIO,&naim,NUM_COLUMNS,&nomer,-1);

data->kodklv.new_plus(iceb_u_fromutf(kod));
data->fiov.new_plus(iceb_u_fromutf(naim));
data->snanomer=nomer;

g_free(kod);
g_free(naim);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_taxisk_knopka(GtkWidget *widget,class l_taxisk_data *data)
{
//char    bros[300];
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_taxisk_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->kodkl_tv.new_plus("");
    if(l_taxisk_v(&data->kodkl_tv,data->window) == 0)
      l_taxisk_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;

    if(l_taxisk_v(&data->kodklv,data->window) == 0)
      l_taxisk_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_taxisk_udzap(data);
    l_taxisk_create_list(data);
    return;  
  
  case SFK3:
    if(data->kolzap == 0)
      return;

    if(iceb_menu_danet(gettext("Удалить неиспользуемые коды ? Вы уверены ?"),2,data->window) != 1)
     return;

    l_gruppdok_unk(data);
    l_taxisk_create_list(data);
    return;  


  case FK4:
    l_taxisk_p(&data->poisk,data->window);
    l_taxisk_create_list(data);
    return;  

  case FK5:
    l_taxisk_rasp(data);
    return;  

  case FK6:
    data->metka_saldo++;
    if(data->metka_saldo > 1)
     data->metka_saldo=0;
    l_taxisk_create_list(data);
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

gboolean   l_taxisk_key_press(GtkWidget *widget,GdkEventKey *event,class l_taxisk_data *data)
{
iceb_u_str repl;

switch(event->keyval)
 {

  case GDK_F2:

    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");

    return(TRUE);
   
  case GDK_F3:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK3]),"clicked");
    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
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
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void l_taxisk_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_taxisk_data *data)
{
//printf("l_taxisk_v_row\n");
//printf("l_taxisk_v_row корректировка\n");
if(data->metka_rr == 0)
 {
  gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
  return;
 }

data->metka_voz=0;

gtk_widget_destroy(data->window);


}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_taxisk_udzap(class l_taxisk_data *data)
{

if(l_taxisk_pvu(1,data->kodklv.ravno(),data->window) != 0)
 return;

char strsql[300];

sprintf(strsql,"delete from Taxiklient where kod='%s'",data->kodklv.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/****************************/
/*Проверка записей          */
/*****************************/

int  l_taxisk_prov_row(SQL_str row,class l_taxisk_data *data)
{
if(data->poisk.metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.kodkl.ravno(),row[0],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->poisk.fio.ravno(),row[1],4,0) != 0)
 return(1);
if(iceb_u_proverka(data->poisk.adres.ravno(),row[2],4,0) != 0)
 return(1);
if(iceb_u_proverka(data->poisk.telef.ravno(),row[3],4,0) != 0)
 return(1);
if(iceb_u_proverka(data->poisk.koment.ravno(),row[13],4,0) != 0)
 return(1);

if(data->poisk.pol == 1)
 if(atoi(row[11]) != 0)
  return(1);

if(data->poisk.pol == 2)
 if(atoi(row[11]) != 1)
  return(1);
if(data->poisk.denrog.getdlinna() > 1)
 {
  class iceb_u_str denrog;
  denrog.plus(iceb_u_sqldata(row[12])); 
  if(iceb_u_sravmydat(data->poisk.denrog.ravno(),denrog.ravno()) != 0)
   return(1);

 }
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_taxisk_rasp(class l_taxisk_data *data)
{
char strsql[300];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from Taxiklient order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"kateg%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список категорий работников"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }
iceb_u_startfil(ff);
fprintf(ff,"\x1B\x4D"); /*12-знаков*/

iceb_u_zagolov("Список клиентов",0,0,0,0,0,0,organ,ff);

if(data->poisk.metka_poi == 1)
 {
  if(data->poisk.kodkl.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код"),data->poisk.kodkl.ravno());
  if(data->poisk.fio.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Фамилия"),data->poisk.fio.ravno());
  if(data->poisk.adres.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Адрес"),data->poisk.adres.ravno());
  if(data->poisk.telef.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Телефон"),data->poisk.telef.ravno());
  if(data->poisk.denrog.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Дата рождения"),data->poisk.denrog.ravno());
  if(data->poisk.koment.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Коментарий"),data->poisk.koment.ravno());
  
  if(data->poisk.pol == 1)
   fprintf(ff,"%s:%s\n",gettext("Пол"),gettext("Мужчина"));
  if(data->poisk.pol == 2)
   fprintf(ff,"%s:%s\n",gettext("Пол"),gettext("Женщина"));

 }

fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,"\
  Код  |Фамилия             |  Сальдо  |Дата и время запис.| Кто записал\n");
/*
1234567 12345678901234567890 1234567890
*/
fprintf(ff,"\
------------------------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;


cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(l_taxisk_prov_row(row,data) != 0)
    continue;

  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[5]));

  //Кто записал
  s6.new_plus(iceb_u_kszap(row[4],1));

  fprintf(ff,"%-7s %-20.20s %10.2f %s %s\n",
  row[0],row[1],taxi_saldo(row[0]),s5.ravno(),s6.ravno());

  if(strlen(row[1]) > 20)
   fprintf(ff,"%4s %s\n","",&row[1][20]);
  

 }
fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);

iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
/*****************************/
/*Проверить возможность удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int  l_taxisk_pvu(int met,const char *kod,GtkWidget *wpredok)
{
char strsql[300];

sprintf(strsql,"select kodk from Taxizak where kodk='%s' limit 1",kod);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Taxizak",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }    


return(0);

}
/**********************************/
/*Удаление неиспользуемых кодов групп*/
/**************************************/
void l_gruppdok_unk(class l_taxisk_data *data)
{
SQL_str row;
SQLCURSOR cur;

char strsql[500];
sprintf(strsql,"select kod from Taxiklient");
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
int koludzap=0;

while(cur.read_cursor(&row) != 0)
 {
  if(l_taxisk_pvu(0,row[0],data->window) != 0)
   continue;
  sprintf(strsql,"delete from Taxiklient where kod='%s'",row[0]);
  if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
   break;
  koludzap++;
  
 }
sprintf(strsql,"%s:%d",gettext("Количество удалённых записей"),koludzap);
iceb_menu_soob(strsql,data->window);

}

