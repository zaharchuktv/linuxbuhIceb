/*$Id: iceb_l_sklad.c,v 1.35 2011-02-21 07:36:07 sasa Exp $*/
/*11.11.2008	10.05.2004	Белых А.И.	iceb_l_sklad.c
Работа со списком складов
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include  "iceb_libbuh.h"
#include  "iceb_l_sklad.h"

enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK6,
  FK7,
  FK10,
  SFK2,
  KOL_F_KL
};

class  sklad_data
 {
  public:

  sklad_rek poisk;

  iceb_u_str kodv; //Склад отмеченный в списке
  iceb_u_str naimv;
  iceb_u_str sklad_tv; //Склад только что введённый
  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
//  GtkWidget *entry[KOLENTER];
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  short     metka_poi;  //0-без поиска 1-с поиском
  int       metka_voz;  //0-выбрали 1-нет  
  short     metka_rr;   //0-работа со списком 1-выбор
  short metka_sort; //0-без сортировки 1-сортировка по наименованиям
  short metka_isp_skl; //0-все склады 1-только используемые 2-только не используемые
  class iceb_u_str zapros;
  
  //Конструктор
  sklad_data()
   {
    sklad_tv.plus("");
    snanomer=0;
    metka_rr=metka_voz=kl_shift=metka_poi=0;
    window=treeview=NULL;
    metka_sort=0;
    metka_isp_skl=0;
   }      
 };

enum
{
  COL_KOD,
  COL_METKA_ISP,
  COL_NAIM,
  COL_FIO,
  COL_DOLG,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

gboolean   sklad_key_press(GtkWidget *widget,GdkEventKey *event,class sklad_data *data);
void sklad_vibor(GtkTreeSelection *selection,class sklad_data *data);
void sklad_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class sklad_data *data);
void  sklad_knopka(GtkWidget *widget,class sklad_data *data);
void sklad_add_columns (GtkTreeView *treeview);
int iceb_l_sklad_v(class iceb_u_str*,GtkWidget*);
int iceb_l_sklad_p(class sklad_rek *data,GtkWidget*);
void sklad_udzap(class sklad_data *data);
int  sklad_prov_row(SQL_str row,class sklad_data *data);
void sklad_rasp(class sklad_data *data);
int  sklad_pvu(int,const char *kod,GtkWidget *wpredok);

extern char *organ;

int   iceb_l_sklad(int metka_rr, //0-ввод и корек. 1-выбор
iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok)
{
sklad_data data;
char bros[512];
GdkColor color;

data.poisk.clear_zero();
data.metka_rr=metka_rr;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Список складов"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(sklad_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список складов"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(sklad_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[SFK2]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(sklad_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK3]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(sklad_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(sklad_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(sklad_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Сортировка"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(sklad_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Включить/выключить сортировку по наименованиям"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Использование"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(sklad_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Включить/выключить показ только используемых складов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(sklad_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

sklad_create_list(&data);

gtk_widget_show(data.window);
if(metka_rr == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));

gtk_main();

if(data.metka_voz == 0)
 {
  kod->new_plus(data.kodv.ravno());
  naim->new_plus(data.naimv.ravno());
 }

//printf("l_sklad end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void sklad_create_list (class sklad_data *data)
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

data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(sklad_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(sklad_vibor),data);

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
G_TYPE_INT);

data->zapros.new_plus("select * from Sklad");

if(data->metka_isp_skl == 1)
 data->zapros.plus(" where mi=0");
if(data->metka_isp_skl == 2)
 data->zapros.plus(" where mi=1");

if(data->metka_sort == 0)
 data->zapros.plus(" order by kod asc");
else
 data->zapros.plus(" order by naik asc");
//printf("%s\n",data->zapros.ravno());

if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(sklad_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->sklad_tv.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  //Код
  ss[COL_KOD].new_plus(iceb_u_toutf(row[0]));
  
  //Наименование
  ss[COL_NAIM].new_plus(iceb_u_toutf(row[1]));

  //Фамилия
  ss[COL_FIO].new_plus(iceb_u_toutf(row[2]));

  //Должность
  ss[COL_DOLG].new_plus(iceb_u_toutf(row[3]));

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[5])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[4],0,data->window));

  //Метка использования склада
  if(row[6][0] == '1')
    ss[COL_METKA_ISP].new_plus("*");
  else   
    ss[COL_METKA_ISP].new_plus("");
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,ss[COL_KOD].ravno(),
  COL_METKA_ISP,ss[COL_METKA_ISP].ravno(),
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_FIO,ss[COL_FIO].ravno(),
  COL_DOLG,ss[COL_DOLG].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->sklad_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

sklad_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список складов"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

//printf("data->metka_poi=%d\n",data->metka_poi);
zagolov.new_plus("");

if(data->metka_isp_skl == 1)
 zagolov.plus(gettext("Просмотр только складов, которые используются !"));
if(data->metka_isp_skl == 2)
 zagolov.plus(gettext("Просмотр только складов, которые неиспользуются !"));

if(data->metka_poi == 1)
 {
//  printf("Формирование заголовка с реквизитами поиска.\n");
  if(zagolov.getdlinna() > 1)
   zagolov.plus("\n");
     
  zagolov.plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.kod.ravno(),gettext("Код"));
  iceb_str_poisk(&zagolov,data->poisk.naim.ravno(),gettext("Наименование"));
  iceb_str_poisk(&zagolov,data->poisk.fmol.ravno(),gettext("Фамилия"));
  iceb_str_poisk(&zagolov,data->poisk.dolg.ravno(),gettext("Должность"));
  
 }

if(zagolov.getdlinna() > 1)
 {
  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno_toutf());
  gtk_widget_show(data->label_poisk);

 }
else
 gtk_widget_hide(data->label_poisk); 
 
gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void sklad_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

//printf("sklad_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
//  g_object_set(G_OBJECT(renderer),"foreground","red",NULL);

//g_object_set(G_OBJECT(renderer),"background","black",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"M", renderer,"text", COL_METKA_ISP,NULL);

renderer = gtk_cell_renderer_text_new ();
//g_object_set(G_OBJECT(renderer),"foreground","white",NULL);
//g_object_set(G_OBJECT(renderer),"background","black",NULL);


gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Фамилия"), renderer,"text", COL_FIO,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Должность"), renderer,"text", COL_DOLG,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);
//printf("sklad_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void sklad_vibor(GtkTreeSelection *selection,class sklad_data *data)
{
//printf("sklad_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gint  nomer;


gtk_tree_model_get(model,&iter,0,&kod,1,&naim,NUM_COLUMNS,&nomer,-1);

data->kodv.new_plus(iceb_u_fromutf(kod));
data->naimv.new_plus(iceb_u_fromutf(naim));
data->snanomer=nomer;

g_free(kod);
g_free(naim);

//printf("%s %s %d\n",data->kodv.ravno(),data->naimv.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  sklad_knopka(GtkWidget *widget,class sklad_data *data)
{
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("sklad_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:

    iceb_l_sklad_v(&data->sklad_tv,data->window);
    sklad_create_list(data);
     
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    iceb_l_sklad_v(&data->kodv,data->window);
    sklad_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    sklad_udzap(data);
    sklad_create_list(data);
    return;  


  case FK4:
    if(iceb_l_sklad_p(&data->poisk,data->window) == 0)
     data->metka_poi=1;
    else 
     data->metka_poi=0;
     
    sklad_create_list(data);
    return;  

  case FK5:
    sklad_rasp(data);
    return;  

  case FK6:
    data->metka_sort++;
    if(data->metka_sort > 1)
     data->metka_sort=0;
    sklad_create_list(data);
     
    return;  

  case FK7:
    data->metka_isp_skl++;
    if(data->metka_isp_skl > 2)
     data->metka_isp_skl=0;
    sklad_create_list(data);
    return;  

    
  case FK10:
//    printf("sklad_knopka F10\n");
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   sklad_key_press(GtkWidget *widget,GdkEventKey *event,class sklad_data *data)
{
iceb_u_str repl;
//printf("sklad_key_press keyval=%d state=%d\n",event->keyval,event->state);
data->kl_shift=0;
switch(event->keyval)
 {

  case GDK_F2:

    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");

    return(TRUE);
   
  case GDK_F3:
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
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
  case GDK_F7:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
    return(TRUE);

  
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("sklad_key_press-Нажата клавиша Shift\n");

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
void sklad_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class sklad_data *data)
{
//printf("sklad_v_row\n");
//printf("sklad_v_row корректировка\n");
if(data->metka_rr == 0)
 {
  gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
  return;
 }

gtk_widget_destroy(data->window);

data->metka_voz=0;

}

/*****************************/
/*Удаление записи            */
/*****************************/

void sklad_udzap(class sklad_data *data)
{

iceb_u_str sp;
sp.plus(gettext("Удалить запись ? Вы уверены ?"));
if(iceb_menu_danet(&sp,2,data->window) == 2)
 return;

if(sklad_pvu(1,data->kodv.ravno(),data->window) != 0)
 return;

char strsql[512];

sprintf(strsql,"delete from Sklad where kod=%s",
data->kodv.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;




}
/****************************/
/*Проверка записей          */
/*****************************/

int  sklad_prov_row(SQL_str row,class sklad_data *data)
{
if(data->metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.kod.ravno(),row[0],0,0) != 0)
 return(1);


//Поиск образца в строке
if(data->poisk.naim.ravno()[0] != '\0' && iceb_u_strstrm(row[1],data->poisk.naim.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->poisk.fmol.getdlinna() > 1 && iceb_u_strstrm(row[2],data->poisk.fmol.ravno()) == 0)
 return(1);


//Поиск образца в строке
if(data->poisk.dolg.getdlinna() > 1 && iceb_u_strstrm(row[3],data->poisk.dolg.ravno()) == 0)
 return(1);

   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void sklad_rasp(class sklad_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;


if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"sklad%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список складов"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }
iceb_u_startfil(ff);
fprintf(ff,"\x1B\x4D"); /*12-знаков*/

iceb_u_zagolov(gettext("Список складов"),0,0,0,0,0,0,organ,ff);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Код |Наименование        |Фамилия мат.ответст.|Должность мат.ответ.|Дата и время запис.| Кто записал\n"));

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;


cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(sklad_prov_row(row,data) != 0)
    continue;

  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[4]));

  //Кто записал
  s6.new_plus(iceb_kszap(row[3],1,data->window));

  fprintf(ff,"%-4s %-*.*s %-*.*s %-*.*s %s %s\n",
  row[0],
  iceb_u_kolbait(20,row[1]),iceb_u_kolbait(20,row[1]),row[1],
  iceb_u_kolbait(20,row[2]),iceb_u_kolbait(20,row[2]),row[2],
  iceb_u_kolbait(20,row[3]),iceb_u_kolbait(20,row[3]),row[3],
  s5.ravno(),s6.ravno());


 }
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fprintf(ff,"\x1B\x50"); /*10-знаков*/

fclose(ff);

iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
/*****************************/
/*Проверить возможность удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int  sklad_pvu(int met,const char *kod,GtkWidget *wpredok)
{
char strsql[512];
int kolstr;
SQLCURSOR cur;

sprintf(strsql,"select sklad from Kart where sklad=%d  limit 1",atoi(kod));

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr != 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Kart",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }    


sprintf(strsql,"select sklad from Dokummat where sklad=%d  limit 1",atoi(kod));

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
   
   
if(kolstr != 0)
 {
  if(met != 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Dokummat",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
   }
  return(1);
 }    

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select skl from Uplouot where skl=%s limit 1",kod);
  
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 }
else   
 {
  if(kolstr != 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Uplouot",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }    
 }

return(0);

}
