/*$Id: l_spec1.c,v 1.30 2011-02-21 07:35:53 sasa Exp $*/
/*12.11.2009	15.05.2005	Белых А.И.	l_spec1.c
Ввод и корректировка спецификаций на изделия
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
//#include <ctype.h>
#include "buhg_g.h"

enum
{
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 FK7,
 FK10,
 KOL_F_KL
};

enum
{
 COL_METKA,
 COL_KOD,
 COL_NAIM,
 COL_EI,
 COL_KOLIH,
 COL_KOMENT,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  l_spec1_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *treeview;
  GtkWidget *window;
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       voz;
  
  //Реквизиты выбранной записи
  iceb_u_str kod;
      
  iceb_u_spisok imaf;

  int       kod_v; //Код только что введенного материалла и для поиска материалла по коду материалла
  iceb_u_str naim_poi; //Поиск по наименованию

  iceb_u_str kod_izdel; //Код изделия которое просмотриваем    
  iceb_u_str naim_izdel; //Наименование изделия, которое просмотриваем
  int uroven; //Уровень глубины просмотра
  
  l_spec1_data()
   {
    clear();
    kod_v=0;
    naim_izdel.plus("");
   }
  void clear()
   {
    voz=0;
    kl_shift=0;
    snanomer=0;
    treeview=NULL;
    naim_poi.new_plus("");
    kod_v=0;
   }
 };

gboolean   l_spec1_key_press(GtkWidget *widget,GdkEventKey *event,class l_spec1_data *data);
void l_spec1_vibor(GtkTreeSelection *selection,class l_spec1_data *data);
void l_spec1_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_spec1_data *data);
void  l_spec1_knopka(GtkWidget *widget,class l_spec1_data *data);
void l_spec1_add_columns (GtkTreeView *treeview);
void l_spec1_create_list (class l_spec1_data *data);


int specprov(char *kodiz,iceb_u_str *naim,iceb_u_str *poisk,int metka_naim,GtkWidget *wpredok);
void spec1_rasp(int kod_izdel,GtkWidget *wpredok);


extern SQL_baza  bd;
extern char      *name_system;

void l_spec1(const char *kod_izdel, //Код изделия список которого просмотриваем
int uroven, //Уровень глубины просмотра
GtkWidget *wpredok)
{
class  l_spec1_data data;
char strsql[512];
SQL_str row;
SQLCURSOR cur;

data.kod_izdel.new_plus(kod_izdel);
data.uroven=uroven+1;

//Узнаём наименование изделия
sprintf(strsql,"select naimat from Material where kodm=%d",data.kod_izdel.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.naim_izdel.new_plus(row[0]);
else
 {
  sprintf(strsql,"select naius from Uslugi where kodus=%d",data.kod_izdel.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   data.naim_izdel.new_plus(row[0]);
 }
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,
gettext("Работа со спецификацией на изделие"));

gtk_window_set_title (GTK_WINDOW (data.window),iceb_u_toutf(strsql));
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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_spec1_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");
GdkColor color;
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);


GtkTooltips *tooltips[KOL_F_KL];

//Кнопки
sprintf(strsql,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_spec1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод нового материалла"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"SF2 %s",gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_spec1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_spec1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"SF3 %s",gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(l_spec1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удалить все записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(strsql,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_spec1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_spec1_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка спецификации изделия"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F7 %s",gettext("Ввести"));
data.knopka[FK7]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(l_spec1_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Ввод новой услуги"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_spec1_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);

gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_spec1_create_list(&data);

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
void l_spec1_create_list (class l_spec1_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR curr;
char strsql[512];
int  kolstr=0;
SQL_str row;
//GdkColor color;

//printf("l_spec1_create_list %d\n",data->snanomer);



data->kl_shift=0; //0-отжата 1-нажата  

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_spec1_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_spec1_vibor),data);

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
G_TYPE_INT);

sprintf(strsql,"select kodd,kz,kolih,koment,ei,ktoz,vrem from Specif where kodi=%d",data->kod_izdel.ravno_atoi());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
iceb_u_str naim;
int kolstr1;
SQL_str row1;
SQLCURSOR cur1;
float kolstr2=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr2);
  
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  if(specprov(row[0],&naim,&data->naim_poi,1,data->window) != 0)
   continue;
  


//  if(iceb_u_SRAV(data->vvod.god.ravno(),row[0],0) == 0 && iceb_u_SRAV(data->vvod.shet.ravno(),row[1],0) == 0)
//    data->snanomer=data->kolzap;
  if(data->kod_v == atoi(row[0]))
   data->snanomer=data->kolzap;

  //Метка записи   
  ss[COL_METKA].new_plus("");
  if(row[1][0] == '0')
    ss[COL_METKA].new_plus("M");
  if(row[1][0] == '1')
    ss[COL_METKA].new_plus("U");
   
  /*Проверяем не является ли входящий материал изделием*/
  sprintf(strsql,"select kodi from Specif where kodi=%s limit 1",row[0]);
  kolstr1=sql_readkey(&bd,strsql,&row1,&cur1);

  if(kolstr1 != 0)
   ss[COL_METKA].new_plus("*");

  //Код записи
  ss[COL_KOD].new_plus(iceb_u_toutf(row[0]));

  //Наименование записи
  ss[COL_NAIM].new_plus(naim.ravno_toutf());
   
  //Единица измерения записи
  ss[COL_EI].new_plus(iceb_u_toutf(row[4]));

  //Количество 
  sprintf(strsql,"%.10g",atof(row[2]));
  ss[COL_KOLIH].new_plus(strsql);


  //Коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[3]));
  

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[6])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[5],0,data->window));
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKA,ss[COL_METKA].ravno(),
  COL_KOD,ss[COL_KOD].ravno(),
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_EI,ss[COL_EI].ravno(),
  COL_KOLIH,ss[COL_KOLIH].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

data->kod_v=0;

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_spec1_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

ss[0].new_plus(gettext("Работа со спецификацией на изделие"));
ss[0].ps_plus(gettext("Изделие"));
ss[0].plus(":");
ss[0].plus(data->kod_izdel.ravno());
ss[0].plus(" ");
ss[0].plus(data->naim_izdel.ravno());

sprintf(strsql,"%s:%d %s:%d",
gettext("Количество записей"),data->kolzap,
gettext("Уровень"),data->uroven);

ss[0].ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),ss[0].ravno_toutf());

gtk_widget_show(data->label_kolstr);

if(data->naim_poi.getdlinna() > 1)
 {
  sprintf(strsql,"%s:%s",gettext("Поиск"),data->naim_poi.ravno());
  gtk_label_set_text(GTK_LABEL(data->label_poisk),iceb_u_toutf(strsql));
  gtk_widget_show(data->label_poisk);
  
 }

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_spec1_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

//printf("l_spec1_add_columns\n");


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
"M", renderer,"text", COL_METKA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Код"), renderer,"text", COL_KOD,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Единица измерения"), renderer,"text", COL_EI,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Количество"), renderer,"text", COL_KOLIH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

printf("l_spec1_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void l_spec1_vibor(GtkTreeSelection *selection,class l_spec1_data *data)
{
printf("l_spec1_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_KOD,&kod,NUM_COLUMNS,&nomer,-1);

data->kod.new_plus(iceb_u_fromutf(kod));

data->snanomer=nomer;

g_free(kod);

//printf("%s %s %d\n",data->kodmat.ravno(),data->nomkar.ravno(),data->snanomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_spec1_knopka(GtkWidget *widget,class l_spec1_data *data)
{
char strsql[512];
iceb_u_str kod;
kod.plus("");
char naim[512];
memset(naim,'\0',sizeof(naim));

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_spec1_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {


  case FK2:
    if(l_spec_v(data->kod_izdel.ravno(),&kod,0,data->window) == 0)
     {
      data->kod_v=kod.ravno_atoi();
      l_spec1_create_list(data);
     }     
    return;  

  case SFK2:
    if(data->kolzap == 0)
     return;    

    if(l_spec_v(data->kod_izdel.ravno(),&data->kod,0,data->window) == 0)
     {
      data->kod_v=kod.ravno_atoi();
      l_spec1_create_list(data);
     }     

    return;
    
  case FK3:
    if(data->kolzap == 0)
     return;    

    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
     return;

    sprintf(strsql,"delete from Specif where kodi=%d and kodd=%d",
    data->kod_izdel.ravno_atoi(),data->kod.ravno_atoi());
    
    if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
    l_spec1_create_list(data);
    
    return;  

  case SFK3:
    if(data->kolzap == 0)
     return;    
    if(iceb_menu_danet(gettext("Удалить все записи ? Вы уверены ?"),2,data->window) == 2)
     return;

    sprintf(strsql,"delete from Specif where kodi=%d",data->kod_izdel.ravno_atoi());
    if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);

    l_spec1_create_list(data);
     
    return;  

    
  
  case FK4:
    data->naim_poi.new_plus("");
    
    if(iceb_menu_vvod1(gettext("Введите наименование"),naim,sizeof(naim),data->window) == 0)
      data->naim_poi.new_plus(naim);
    l_spec1_create_list(data);
    return;  
  
  case FK5:
    if(data->kolzap == 0)
     return;    
    spec1_rasp(data->kod_izdel.ravno_atoi(),data->window);

    return;  

  
  case FK7:
    if(l_spec_v(data->kod_izdel.ravno(),&kod,1,data->window) == 0)
     {
      data->kod_v=kod.ravno_atoi();
      l_spec1_create_list(data);
     }     
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_spec1_key_press(GtkWidget *widget,GdkEventKey *event,class l_spec1_data *data)
{
iceb_u_str repl;
printf("l_spec1_key_press keyval=%d state=%d\n",
event->keyval,event->state);

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

  case GDK_F7:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
    return(TRUE);


  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("l_spec1_key_press-Нажата клавиша Shift\n");

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
void l_spec1_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_spec1_data *data)
{
printf("l_spec1_v_row\n");
//gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
l_spec1(data->kod.ravno(),data->uroven,data->window);
l_spec1_create_list(data);

}
/*******************************/
/*Распечатка спецификации на изделие*/
/**************************************/

void spec1_rasp(int kod_izdel,GtkWidget *wpredok)
{
SQL_str         row,row1;
char		strsql[512];
int		kolnuz;
char		naim[512];
char		imaf[20];
char		bros[512];
FILE            *ff;
int		kodizr;
int		suz,zsuz;
int		kolstr;
int		kodd;

zsuz=0;
kodizr=kod_izdel;

sprintf(imaf,"spec%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
 
/*Определяем входящие узлы*/

class iceb_razuz_data data;
data.kod_izd=kod_izdel;
kolnuz=iceb_razuz(&data,wpredok);

/*
printw("Разузловка завершена.\n");
refresh();
*/
SQLCURSOR cur;

SQLCURSOR cur1;

naz:;


memset(naim,'\0',sizeof(naim));
for(int metka_zap=0; metka_zap < 2; metka_zap++)
 {
  if(metka_zap == 0)
    sprintf(strsql,"select naimat from Material where kodm=%d",kodizr);
  if(metka_zap == 1)
    sprintf(strsql,"select naius from Uslugi where kodus=%d",kodizr);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    if(metka_zap == 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден код записи"),kodizr);
      iceb_menu_soob(strsql,wpredok);
     }
   }  
  else
   { 
    strncpy(naim,row1[0],sizeof(naim)-1);
    break;
   }
 }
 
if(kod_izdel == kodizr)
 {
  fprintf(ff,"%s: %d %s\n\
--------------------------------------------------------------------\n",
  gettext("Спецификация на изделие"),
  kodizr,naim);
 }
else
 {
  fprintf(ff,"\n%s: %d %s\n\
--------------------------------------------------------------------\n",
  gettext("Спецификация на узел"), 
  kodizr,naim);
 }

sprintf(strsql,"select kodd,kolih,ei,kz from Specif where kodi=%d",kodizr);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0 )
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи в изделии !"),wpredok);
  fclose(ff);
  unlink(imaf);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  kodd=atoi(row[0]);
  memset(naim,'\0',sizeof(naim));
  if(row[3][0] == '0')
   sprintf(strsql,"select naimat from Material where kodm=%s",row[0]);
  if(row[3][0] == '1')
   sprintf(strsql,"select naius from Uslugi where kodus=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    if(row[3][0] == '0')
      sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodd);
    if(row[3][0] == '1')
      sprintf(strsql,"%s %d !",gettext("Не найден код услуги"),kodd);
    iceb_menu_soob(strsql,wpredok);
   }  
  else
   {
    strncpy(naim,row1[0],sizeof(naim)-1);
   }
  memset(bros,'\0',sizeof(bros));
  if(data.kod_uz.find(kodd) != -1)
     strcpy(bros,gettext("Узел"));

  fprintf(ff,"%-4s %-*s %*s %*s %10.10g\n",
  row[0],
  iceb_u_kolbait(40,naim),naim,
  iceb_u_kolbait(6,row[2]),row[2],
  iceb_u_kolbait(5,bros),bros,
  atof(row[1]));

 }

for(suz=zsuz ; suz < kolnuz ; suz++)
 {
  kodizr=data.kod_uz.ravno(suz);
  zsuz++;
  goto naz;
 }

iceb_podpis(ff,wpredok);

fclose(ff);

iceb_u_spisok imaff;
iceb_u_spisok naimf;

imaff.plus(imaf);
naimf.plus(gettext("Спецификация на изделие"));

iceb_ustpeh(imaff.ravno(0),3,wpredok);

iceb_rabfil(&imaff,&naimf,"",0,wpredok);

}
