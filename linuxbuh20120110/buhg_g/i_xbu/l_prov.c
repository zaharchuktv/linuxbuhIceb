/*$Id: l_prov.c,v 1.54 2011-01-13 13:49:50 sasa Exp $*/
/*16.04.2010	07.01.2004	Белых А.И.	l_prov.c
Программа работы с найденными проводками
*/
#include  <stdlib.h>
#include  <math.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include  "l_prov.h"

enum
{
 COL_DATA,
 COL_SHET,
 COL_SHETK,
 COL_KONTR,
 COL_METKA,
 COL_NOMDOK,
 COL_DEBET,
 COL_KREDIT,
 COL_KODOP,
 COL_KOMENT,  
 COL_KEKV,
 COL_DATA_VREM,
 COL_KTO,  
 COL_VAL,
 COL_VREM,
 COL_PODR,
 COL_TIPZ,
 COL_DATADOK, 
 NUM_COLUMNS
};

void  prov_knopka(GtkWidget *widget,class prov_data *data);
void prov_vibor(GtkTreeSelection *selection,class prov_data *data);
gboolean   prov_key_press(GtkWidget *widget,GdkEventKey *event,class prov_data *data);
void prov_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class prov_data *data);
void prov_add_columns(GtkTreeView *treeview);
void prov_udzap(class prov_data *data,int);
int l_prov_kr(class prov_data *data);
int l_prov_krnbs(class prov_data *data);
int  prov_kor(class prov_data *data);
void r_poiprov(class prov_poi_data *rekv_m_poiprov,GtkWidget *wpredok);
int pdokkow(const char *datadok,int kassa,int tipz,const char *nomdok,GtkWidget *wpredok);
int pdokmuw(const char *datadok,int tipz,int sklad,const char *nomdok,GtkWidget *wpredok);
int pdokpdw(const char *metka,const char *datadok,const char *nomdok,GtkWidget *wpredok);
int pdokukr(const char *datadok,const char *nomdok,GtkWidget *wpredok);
int pdokuosw(const char *datadok,const char *nomdok,int tipz,GtkWidget *wpredok);
int pdokusl(const char *datadok,int podr,const char *nomdok,int tipz,GtkWidget *wpredok);
int pdokzp(const char *datadok,const char *nomdok,GtkWidget *wpredok);


void l_prov(class prov_poi_data *rekv_m_poiprov,GtkWidget *wpredok)
{

prov_data data;
char bros[312];

//Делаем ссылку на структуру в которой остались реквизиты от предыдущей работы
//data.rek_poi=&rekv_m_poiprov;
data.rek_poi=rekv_m_poiprov;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Просмотр проводок"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(prov_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Просмотр проводок"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize (GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_widget_show (data.sw);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",gettext("Корректировать"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(prov_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(prov_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));

data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(prov_knopka),&data);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удаление проводки выполненной из другой подсистемы"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(prov_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Документ"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(prov_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Просмотр документа к которому привязана проводка"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(prov_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

prov_create_list(&data);

gtk_widget_show(data.window);
gtk_window_maximize(GTK_WINDOW(data.window));

//gtk_window_fullscreen(GTK_WINDOW(data.window));

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

}

/***********************************/
/*Создаем список для просмотра */
/***********************************/
void prov_create_list (class prov_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;
//GdkColor color;

//printf("saldo_sh_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(prov_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(prov_vibor),data);

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
G_TYPE_INT, 
G_TYPE_ULONG, 
G_TYPE_INT, 
G_TYPE_INT, 
G_TYPE_STRING, 
G_TYPE_INT);

if((kolstr=cur.make_cursor(&bd,data->rek_poi->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->rek_poi->zapros.ravno(),data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str rekv[NUM_COLUMNS];

data->kolzap=0;
double debet=0.;
double kredit=0.;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);    
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(prov_prov_row(row,data->rek_poi) != 0)
    continue;



  //Дата
  rekv[COL_DATA].new_plus(iceb_u_sqldata(row[1]));

  //Счет
  rekv[COL_SHET].new_plus(row[2]);

  //Счет корреспондент
  rekv[COL_SHETK].new_plus(row[3]);

  //Код контрагента
  rekv[COL_KONTR].new_plus(row[4]);

  //Метка
  rekv[COL_METKA].new_plus(row[5]);

  //Номер документа
  rekv[COL_NOMDOK].new_plus(row[6]);

  //Дебет
  rekv[COL_DEBET].new_plus(row[9]);
  debet+=atof(row[9]);
  
  //Кредит
  rekv[COL_KREDIT].new_plus(row[10]);
  kredit+=atof(row[10]);
  
  //Код операции
  rekv[COL_KODOP].new_plus(row[8]);

  //Коментарий
  rekv[COL_KOMENT].new_plus(row[13]);

  //Дата и время записи
  rekv[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[12])));

  //Кто записал
  rekv[COL_KTO].new_plus(iceb_kszap(row[11],0,data->window));

  //Дата документа
  
  rekv[COL_DATADOK].new_plus("");
  if(row[14][0] != '0')
    rekv[COL_DATADOK].new_plus(iceb_u_sqldata(row[14]));
  
  //КЭКЗ
  if(atoi(row[16]) > 0)
   rekv[COL_KEKV].new_plus(row[16]);
  else
   rekv[COL_KEKV].new_plus("");
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATA,rekv[COL_DATA].ravno(),
  COL_KEKV,rekv[COL_KEKV].ravno(),
  COL_SHET,rekv[COL_SHET].ravno_toutf(),
  COL_SHETK,rekv[COL_SHETK].ravno_toutf(),
  COL_KONTR,rekv[COL_KONTR].ravno_toutf(),
  COL_METKA,rekv[COL_METKA].ravno_toutf(),
  COL_NOMDOK,rekv[COL_NOMDOK].ravno_toutf(),
  COL_DEBET,rekv[COL_DEBET].ravno(),
  COL_KREDIT,rekv[COL_KREDIT].ravno(),
  COL_KODOP,rekv[COL_KODOP].ravno_toutf(),
  COL_KOMENT,rekv[COL_KOMENT].ravno_toutf(),
  COL_DATA_VREM,rekv[COL_DATA_VREM].ravno(),
  COL_KTO,rekv[COL_KTO].ravno(),
  COL_VAL,atoi(row[0]),
  COL_VREM,atol(row[12]),
  COL_PODR,atoi(row[7]),
  COL_TIPZ,atoi(row[15]),
  COL_DATADOK,rekv[COL_DATADOK].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

prov_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
 }

data->rek_vibr.clear_zero();

gtk_widget_show (data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Просмотр проводок"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

sprintf(strsql,"%s: %.2f %s: %.2f %s: %.2f",
gettext("Дебет"),debet,
gettext("Кредит"),kredit,
gettext("Разница"),debet-kredit);

zagolov.ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());


gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);
}

/*****************/
/*Создаем колонки*/
/*****************/

void prov_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;

//printf("prov_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата"),renderer,"text",COL_DATA,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_DATA);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Счет"),renderer,"text",COL_SHET,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_SHET); /*Рзарешение на сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Счет"),renderer,"text",COL_SHETK,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_SHETK);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Кон-нт"),renderer,"text",COL_KONTR,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KONTR);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Метка"),renderer,"text",COL_METKA,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_METKA);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Ном. док."),renderer,"text",COL_NOMDOK,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NOMDOK);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Дата д-та"),renderer,"text",COL_DATADOK,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATADOK);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Дебет"),renderer,"text",COL_DEBET,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DEBET);
gtk_tree_view_column_set_alignment(column,1);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Кредит"),renderer,"text",COL_KREDIT,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KREDIT);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Код операции"),renderer,"text",COL_KODOP,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KODOP);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("КЭКР"),renderer,"text",COL_KEKV,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KEKV);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Коментарий"),renderer,"text",COL_KOMENT,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOMENT);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата и время записи"),renderer,"text",COL_DATA_VREM,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_VREM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Кто записал"),renderer,"text",COL_KTO,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_KTO);
gtk_tree_view_append_column (treeview, column);

//printf("prov_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void prov_vibor(GtkTreeSelection *selection,class prov_data *data)
{
//printf("prov_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;


gint     val;
gchar    *datap;
gchar    *datadok;
gchar    *shet;
gchar    *shetk;
gchar    *kontr;
gchar    *debet;
gchar    *kredit;
gchar    *kodop;
gchar    *nomdok;
gchar    *koment;
gchar    *metka;
unsigned long  vremz;
gint nomer;
gchar    *ktoz;
gchar *kekv;
gint podr;
gint tipz;

gtk_tree_model_get(model,&iter,
COL_DATA,&datap,
COL_DATADOK,&datadok,
COL_SHET,&shet,
COL_SHETK,&shetk,
COL_KONTR,&kontr,
COL_METKA,&metka,
COL_NOMDOK,&nomdok,
COL_DEBET,&debet,
COL_KREDIT,&kredit,
COL_KODOP,&kodop,
COL_KOMENT,&koment,
COL_VAL,&val,
COL_VREM,&vremz,
COL_KTO,&ktoz,
COL_KEKV,&kekv,
COL_PODR,&podr,
COL_TIPZ,&tipz,
NUM_COLUMNS,&nomer,-1);

data->rek_vibr.datap.new_plus(datap);
data->rek_vibr.shet.new_plus(iceb_u_fromutf(shet));
data->rek_vibr.shetk.new_plus(iceb_u_fromutf(shetk));
data->rek_vibr.kontr.new_plus(iceb_u_fromutf(kontr));
data->rek_vibr.metka.new_plus(iceb_u_fromutf(metka));
data->rek_vibr.nomdok.new_plus(iceb_u_fromutf(nomdok));
data->rek_vibr.debet.new_plus(debet);
data->rek_vibr.kredit.new_plus(kredit);
data->rek_vibr.kodop.new_plus(iceb_u_fromutf(kodop));
data->rek_vibr.koment.new_plus(iceb_u_fromutf(koment));
data->rek_vibr.val=val;
data->rek_vibr.vremz=vremz;
data->rek_vibr.ktoz=atoi(ktoz);
data->rek_vibr.podr=podr;
data->rek_vibr.tipz=tipz;
data->rek_vibr.datadok.new_plus(datadok);
data->snanomer=nomer;

if(atoi(kekv) > 0)
 data->rek_vibr.kekv.new_plus(kekv);
else
 data->rek_vibr.kekv.new_plus("");

g_free(datap);
g_free(datadok);
g_free(shet);
g_free(shetk);
g_free(kontr);
g_free(debet);
g_free(kredit );
g_free(kodop );
g_free(nomdok);
g_free(koment);
g_free(metka);
g_free(ktoz);
g_free(kekv);

/************
printf("prov_vibor %s %s %s\n",
data->rek_vibr.datap.ravno(),
data->rek_vibr.shet.ravno(),
data->rek_vibr.shetk.ravno());
***********/

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  prov_knopka(GtkWidget *widget,class prov_data *data)
{
//char    bros[512];
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    if(data->kolzap == 0)
      return;
    if(prov_kor(data) == 0)
      prov_create_list(data);
    return;  


  case FK3:
    if(data->kolzap == 0)
      return;
    prov_udzap(data,0);
    return;  

  case SFK3:
    if(data->kolzap == 0)
      return;
    prov_udzap(data,1);
    return;  


  case FK5:
//    gtk_widget_hide(data->window);
//    prov_rasspis(data);
//    gtk_widget_show(data->window);
    r_poiprov(data->rek_poi,data->window);
    return;  

  case FK6:
     if(iceb_u_SRAV(gettext("КО"),data->rek_vibr.metka.ravno(),0) == 0)
       pdokkow(data->rek_vibr.datadok.ravno(),data->rek_vibr.podr,data->rek_vibr.tipz,data->rek_vibr.nomdok.ravno(),data->window);
     if(iceb_u_SRAV(gettext("МУ"),data->rek_vibr.metka.ravno(),0) == 0)
       pdokmuw(data->rek_vibr.datadok.ravno(),data->rek_vibr.tipz,data->rek_vibr.podr,data->rek_vibr.nomdok.ravno(),data->window);
     if(iceb_u_SRAV(gettext("ПЛТ"),data->rek_vibr.metka.ravno(),0) == 0)
       pdokpdw(data->rek_vibr.metka.ravno(),data->rek_vibr.datadok.ravno(),data->rek_vibr.nomdok.ravno(),data->window);
     if(iceb_u_SRAV(gettext("ТРЕ"),data->rek_vibr.metka.ravno(),0) == 0)
       pdokpdw(data->rek_vibr.metka.ravno(),data->rek_vibr.datadok.ravno(),data->rek_vibr.nomdok.ravno(),data->window);
     if(iceb_u_SRAV(gettext("УКР"),data->rek_vibr.metka.ravno(),0) == 0)
       pdokukr(data->rek_vibr.datadok.ravno(),data->rek_vibr.nomdok.ravno(),data->window);
     if(iceb_u_SRAV(gettext("УОС"),data->rek_vibr.metka.ravno(),0) == 0)
       pdokuosw(data->rek_vibr.datadok.ravno(),data->rek_vibr.nomdok.ravno(),data->rek_vibr.tipz,data->window);
     if(iceb_u_SRAV(gettext("УСЛ"),data->rek_vibr.metka.ravno(),0) == 0)
       pdokusl(data->rek_vibr.datadok.ravno(),data->rek_vibr.podr,data->rek_vibr.nomdok.ravno(),data->rek_vibr.tipz,data->window);
     if(iceb_u_SRAV(gettext("ЗП"),data->rek_vibr.metka.ravno(),0) == 0)
       pdokzp(data->rek_vibr.datadok.ravno(),data->rek_vibr.nomdok.ravno(),data->window);
    return;  

    
  case FK10:
//    printf("prov_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   prov_key_press(GtkWidget *widget,GdkEventKey *event,class prov_data *data)
{
iceb_u_str repl;
//printf("prov_key_press keyval=%d state=%d\n",
//event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);
   
  case GDK_F3:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK3]),"clicked");
    data->kl_shift=0;
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
//    printf("prov_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
//    printf("prov_key_press-Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void prov_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class prov_data *data)
{
//printf("prov_v_row корректировка\n");
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
}

/*************************/
/*Удаление проводки*/
/*************************/

void prov_udzap(class prov_data *data,int metka_ud) //0-c проверкой метки проводки 1-без
{
short d,m,g;

iceb_u_rsdat(&d,&m,&g,data->rek_vibr.datap.ravno(),1);
if(iceb_pvglkni(m,g,data->window) != 0)
 return;

if(metka_ud == 0)
 if(provvkp(data->rek_vibr.metka.ravno(),data->window) != 0)
  return; 


if(iceb_menu_danet(gettext("Удалить ? Вы уверены ?"),2,data->window) != 1)
 return;

if(metka_ud == 1)
 if(iceb_parol(0,data->window) != 0)
   return;

//printf("prov_udzap  Удаляем.\n");

if(data->rek_vibr.val == -1)        
 iceb_udprov(data->rek_vibr.val,g,m,d,data->rek_vibr.shet.ravno_filtr(),data->rek_vibr.shetk.ravno_filtr(),
 data->rek_vibr.vremz,atof(data->rek_vibr.debet.ravno()),atof(data->rek_vibr.kredit.ravno()),data->rek_vibr.koment.ravno_filtr(),1,data->window);
else
 iceb_udprov(data->rek_vibr.val,g,m,d,data->rek_vibr.shet.ravno_filtr(),data->rek_vibr.shetk.ravno_filtr(),
 data->rek_vibr.vremz,atof(data->rek_vibr.debet.ravno()),atof(data->rek_vibr.kredit.ravno()),data->rek_vibr.koment.ravno_filtr(),2,data->window);


prov_create_list(data);

}
/*****************************/
/*Коректировка               */
/*****************************/
int  prov_kor(class prov_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;


if(data->rek_vibr.val == 0)
 {

  //Ищем код контрагента во встречной проводке

  short d,m,g;
  iceb_u_rsdat(&d,&m,&g,data->rek_vibr.datap.ravno(),1);

  sprintf(strsql,"select kodkon from Prov where \
  val=%d and datp='%04d-%d-%d' and sh='%s' and shk='%s' and kto='%s' \
  and deb=%.2f and kre=%.2f and vrem=%ld and nomd='%s' and oper='%s' and \
  komen = '%s'",
  data->rek_vibr.val,g,m,d,
  data->rek_vibr.shetk.ravno_filtr(),
  data->rek_vibr.shet.ravno_filtr(),
  data->rek_vibr.metka.ravno_filtr(),
  atof(data->rek_vibr.kredit.ravno()),
  atof(data->rek_vibr.debet.ravno()),
  data->rek_vibr.vremz,
  data->rek_vibr.nomdok.ravno_filtr(),
  data->rek_vibr.kodop.ravno_filtr(),
  data->rek_vibr.koment.ravno_filtr());

//  printf("prov_kor-%s\n",strsql);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    iceb_u_str SOOB;
    SOOB.plus(gettext("Не найдено парную проводку !"));
    iceb_menu_soob(&SOOB,data->window);
    return(1);
   }
  data->rek_vibr.kontrk.new_plus(row[0]);
//  printf("prov_kor-%s\n",data->rek_vibr.kontrk.ravno());
  
  return(l_prov_kr(data));
 }
if(data->rek_vibr.val == -1)
  return(l_prov_krnbs(data));

return(1);
}
/**********************************/
/*Проверка проводок               */
/**********************************/

int  prov_prov_row(SQL_str row,class prov_poi_data *data)
{

if(data->koment.getdlinna() > 1)
 if(iceb_u_strstrm(row[13],data->koment.ravno()) == 0)
   return(2);


//if(data->metka.getdlinna() > 1)
// if(iceb_u_SRAV(data->metka.ravno(),row[5],0) != 0)
if(iceb_u_proverka(data->metka.ravno(),row[5],0,0) != 0)
  return(9);

if(data->nomdok.getdlinna() > 1)
 if(iceb_u_SRAV(data->nomdok.ravno(),row[6],0) != 0)
  return(10);

if(iceb_u_proverka(data->kodop.ravno(),row[8],0,0) != 0)
   return(11);

if(iceb_u_proverka(data->kekv.ravno(),row[16],0,0) != 0)
   return(17);



if(data->debet.getdlinna() > 1)
 {

  if(data->debet.ravno()[0] == '+' && atof(row[9]) == 0.)
    return(14);

  if(fabs(atof(data->debet.ravno())) > 0.009 && 
  fabs(atof(data->debet.ravno())-atof(row[9])) > 0.009)
    return(12); 
  
 }

if(data->kredit.getdlinna() > 1)
 {
  if(fabs(atof(data->kredit.ravno())) > 0.009 && 
  fabs(atof(data->kredit.ravno())-atof(row[10])) > 0.009)
   return(13); 


  if(data->kredit.ravno()[0] == '+' && atof(row[10]) == 0.)
   return(15);
 }

if(data->grupak.getdlinna() > 1)
 {
  char strsql[512];
  SQL_str row1;
  SQLCURSOR cur1;  
  if(row[4][0] == '\0')
    return(16);
  sprintf(strsql,"select grup from Kontragent where kodkon='%s'",
  row[4]);
//  printw("%s\n",strsql);
//  OSTANOV();

  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    if(iceb_u_proverka(data->grupak.ravno(),row1[0],0,0) != 0)
      return(16);
   }      
 }

return(0);
}
