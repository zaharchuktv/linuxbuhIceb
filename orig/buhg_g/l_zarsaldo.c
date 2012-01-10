/*$Id: l_zarsaldo.c,v 1.16 2011-02-21 07:35:55 sasa Exp $*/
/*12.11.2009	21.06.2006	Белых А.И.	l_zarsaldo.c
Работа со списком 
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include  "buhg_g.h"

enum
{
  COL_GOD,
  COL_TAB_NOM,
  COL_SALDO,
  COL_SALDO_B,
  COL_FIO,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK10,
  SFK2,
  KOL_F_KL
};

class  l_zarsaldo_data
 {
  public:

  short god_v;
  int tabn_v;
  short god_tv; //только что введённый год
  int tabn_tv; //только что введённый табельный номер
  
  short god; //Год просмотра сальдо
  
  class iceb_u_str zapros_bd; //Запрос к базе данных

  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  short     metka_poi;  //0-без поиска 1-с поиском
  int       metka_voz;  //0-выбрали 1-нет  
  short     metka_rr;   //0-работа со списком 1-выбор
  //Конструктор
  l_zarsaldo_data()
   {
    snanomer=0;
    metka_rr=metka_voz=kl_shift=metka_poi=0;
    window=treeview=NULL;
    god_tv=tabn_tv=0;
   }      
 };

gboolean   l_zarsaldo_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsaldo_data *data);
void l_zarsaldo_vibor(GtkTreeSelection *selection,class l_zarsaldo_data *data);
void l_zarsaldo_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zarsaldo_data *data);
void  l_zarsaldo_knopka(GtkWidget *widget,class l_zarsaldo_data *data);
void l_zarsaldo_add_columns (GtkTreeView *treeview);
void l_zarsaldo_udzap(class l_zarsaldo_data *data);
void l_zarsaldo_rasp(class l_zarsaldo_data *data);
void l_zarsaldo_create_list (class l_zarsaldo_data *data);

int l_zarsaldo_v(short *godk,int *tabnk,GtkWidget *wpredok);


extern char *organ;
extern SQL_baza  bd;
extern char      *name_system;
extern short		startgodz; /*Стаpтовый год для расчета сальдо*/

void   l_zarsaldo(GtkWidget *wpredok)
{
l_zarsaldo_data data;
char bros[512];
GdkColor color;
data.god=startgodz;
if(data.god == 0)
 {
  short dt,mt,gt;
  iceb_u_poltekdat(&dt,&mt,&gt);
  data.god=gt;
 }

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Сальдо по табельным номерам"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_zarsaldo_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Сальдо по табельным номерам"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_zarsaldo_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_zarsaldo_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_zarsaldo_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);


sprintf(bros,"F4 %s",gettext("Год"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_zarsaldo_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Сменить год просмотра сальдо"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_zarsaldo_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_zarsaldo_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_zarsaldo_create_list(&data);

gtk_widget_show(data.window);
//if(metka_rr == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_zarsaldo_create_list (class l_zarsaldo_data *data)
{
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

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_zarsaldo_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_zarsaldo_vibor),data);

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


sprintf(strsql,"select * from Zsal where god=%d order by god,tabn asc",data->god);
data->zapros_bd.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
SQL_str row1;
SQLCURSOR cur1;
double isaldo=0.;
double isaldob=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  


  if(data->god_tv == atoi(row[0]) && data->tabn_tv == atoi(row[1]))
    data->snanomer=data->kolzap;

  //Год
  ss[COL_GOD].new_plus(iceb_u_toutf(row[0]));
  
  //Табельный номер
  ss[COL_TAB_NOM].new_plus(iceb_u_toutf(row[1]));

  //Сальдо общее
  ss[COL_SALDO].new_plus(iceb_u_toutf(row[2]));
  isaldo+=atof(row[2]);
  
  //Сальдо бюджетное
  ss[COL_SALDO_B].new_plus(iceb_u_toutf(row[5]));
  isaldob+=atof(row[5]);
  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[4])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[3],0,data->window));

  //Узнаём фамилию
  ss[COL_FIO].new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    ss[COL_FIO].new_plus(iceb_u_toutf(row1[0]));
     
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_GOD,ss[COL_GOD].ravno(),
  COL_TAB_NOM,ss[COL_TAB_NOM].ravno(),
  COL_SALDO,ss[COL_SALDO].ravno(),
  COL_SALDO_B,ss[COL_SALDO_B].ravno(),
  COL_FIO,ss[COL_FIO].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->god_tv=0;
data->tabn_tv=0;

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_zarsaldo_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Сальдо по табельным номерам"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

sprintf(strsql,"\n%s:%.2f %s:%.2f\n%s:%d",gettext("Общее сальдо"),isaldo,
gettext("Cальдо бюджетное"),isaldob,
gettext("Год просмотра"),data->god);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());
/*****************
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
********************/
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_zarsaldo_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;


renderer = gtk_cell_renderer_text_new ();
//  g_object_set(G_OBJECT(renderer),"foreground","red",NULL);

//g_object_set(G_OBJECT(renderer),"background","black",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Год"), renderer,"text", COL_GOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Табельный номер"), renderer,"text", COL_TAB_NOM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сальдо"), renderer,"text", COL_SALDO,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сальдо бюджетное"), renderer,"text", COL_SALDO_B,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Фамилия"), renderer,"text", COL_FIO,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

}

/****************************/
/*Выбор строки*/
/**********************/

void l_zarsaldo_vibor(GtkTreeSelection *selection,class l_zarsaldo_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *god;
gchar *tabn;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_GOD,&god,COL_TAB_NOM,&tabn,NUM_COLUMNS,&nomer,-1);

data->god_v=atoi(god);
data->tabn_v=atoi(tabn);
data->snanomer=nomer;

g_free(god);
g_free(tabn);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_zarsaldo_knopka(GtkWidget *widget,class l_zarsaldo_data *data)
{
char    bros[512];
memset(bros,'\0',sizeof(bros));
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_zarsaldo_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    l_zarsaldo_v(&data->god_tv,&data->tabn_tv,data->window);

    l_zarsaldo_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    l_zarsaldo_v(&data->god_v,&data->tabn_v,data->window);

    l_zarsaldo_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_zarsaldo_udzap(data);
    l_zarsaldo_create_list(data);
    return;  
  

  case FK4:
    if(iceb_menu_vvod1(gettext("Введите год"),bros,5,data->window) != 0)
     return;
    data->god=atoi(bros);
    l_zarsaldo_create_list(data);
    return;  

  case FK5:
    l_zarsaldo_rasp(data);
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

gboolean   l_zarsaldo_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsaldo_data *data)
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
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);

  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
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
void l_zarsaldo_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zarsaldo_data *data)
{
//printf("l_zarsaldo_v_row\n");
//printf("l_zarsaldo_v_row корректировка\n");
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

void l_zarsaldo_udzap(class l_zarsaldo_data *data)
{
if(iceb_pbpds(1,data->god,data->window) != 0)
 return;
 
if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
   return;

char strsql[512];

sprintf(strsql,"delete from Zsal where god=%d and tabn=%d",data->god,data->tabn_v);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_zarsaldo_rasp(class l_zarsaldo_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;


if((kolstr=cur.make_cursor(&bd,data->zapros_bd.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"kateg%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Сальдо по табельным номерам"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_u_zagolov(gettext("Сальдо по табельным номерам"),0,0,0,0,0,0,organ,ff);
fprintf(ff,"%s:%d\n",gettext("Год просмотра"),data->god);

/************
if(data->metka_poi == 1)
 {
  if(data->poisk.kod.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код"),data->poisk.kod.ravno());
  if(data->poisk.naim.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Наименование"),data->poisk.naim.ravno());
 }
****************/
fprintf(ff,"\
----------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Год |Таб.номер |Общ.сальдо|Б.сальдо  |Фамилия , Имя, Отчестьво      |Дата и время запис.| Кто записал\n"));

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;
char fio[512];
SQL_str row1;
SQLCURSOR cur1;
double it1=0.;
double it2=0.;
double it_p_o=0.; //положительный итог
double it_o_o=0.; //Отрицательный итог
double it_p_b=0.; //положительный итог
double it_o_b=0.; //Отрицательный итог
double suma_o;
double suma_b;
cur.poz_cursor(0);

while(cur.read_cursor(&row) != 0)
 {

  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[4]));

  //Кто записал
  s6.new_plus(iceb_kszap(row[3],1,data->window));

  memset(fio,'\0',sizeof(fio));
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    strncpy(fio,row1[0],sizeof(fio)-1);
    
  fprintf(ff,"%-4s %-10s %10.2f %10.2f %-*.*s %s %s\n",
  row[0],row[1],atof(row[2]),atof(row[5]),
  iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,
  s5.ravno(),s6.ravno());
  
  suma_o=atof(row[2]);
  suma_b=atof(row[5]);
  it1+=suma_o;
  it2+=suma_b;

  if(suma_o > 0.)
    it_p_o+=suma_o;
  else
    it_o_o+=suma_o;

  if(suma_b > 0.)
    it_p_b+=suma_b;
  else
    it_o_b+=suma_b;

 }
fprintf(ff,"\
----------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f\n",iceb_u_kolbait(15,gettext("Итого")),gettext("Итого"),it1,it2);

fprintf(ff,"\n%s\n\
%s:%.2f\n\
%s:%.2f\n",
gettext("Общее сальдо"),
gettext("Положительное"),it_p_o,
gettext("Отрицательное"),it_o_o);

fprintf(ff,"\n%s\n\
%s:%.2f\n\
%s:%.2f\n\n",
gettext("Бюджет"),
gettext("Положительное"),it_p_b,
gettext("Отрицательное"),it_o_b);


fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
