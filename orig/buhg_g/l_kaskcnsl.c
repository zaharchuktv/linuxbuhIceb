/*$Id: l_kaskcnsl.c,v 1.19 2011-02-21 07:35:52 sasa Exp $*/
/*18.06.2010	29.09.2006	Белых А.И.	l_kaskcnsl.c
Работа со списком стартовых сальдо по кодам целевых затрат
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include  "buhg_g.h"
#include  "l_kaskcnsl.h"

enum
{
  COL_GOD,
  COL_KAS,
  COL_KOD,
  COL_NAIM,
  COL_SALDO,
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

class  l_kaskcnsl_data
 {
  public:

  l_kaskcnsl_rek poisk;

  class iceb_u_str god;
  class iceb_u_str kas;
  class iceb_u_str kodv;

  class iceb_u_str kod_cn_tv; //только что введённая единица измерения
  class iceb_u_str god_tv;
  class iceb_u_str kas_tv;
  
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
  //Конструктор
  l_kaskcnsl_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    kod_cn_tv.plus("");
   }      
 };

gboolean   l_kaskcnsl_key_press(GtkWidget *widget,GdkEventKey *event,class l_kaskcnsl_data *data);
void l_kaskcnsl_vibor(GtkTreeSelection *selection,class l_kaskcnsl_data *data);
void l_kaskcnsl_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_kaskcnsl_data *data);
void  l_kaskcnsl_knopka(GtkWidget *widget,class l_kaskcnsl_data *data);
void l_kaskcnsl_add_columns (GtkTreeView *treeview);
int l_kaskcnsl_udzap(class l_kaskcnsl_data *data);
int  l_kaskcnsl_prov_row(SQL_str row,class l_kaskcnsl_data *data);
void l_kaskcnsl_rasp(class l_kaskcnsl_data *data);
void l_kaskcnsl_create_list (class l_kaskcnsl_data *data);

int l_kaskcnsl_v(class iceb_u_str *god,class iceb_u_str *kas,class iceb_u_str *kod_cn,GtkWidget *wpredok);
int l_kaskcnsl_p(class l_kaskcnsl_rek *rek_poi,GtkWidget *wpredok);


extern char *organ;
extern SQL_baza  bd;
extern char      *name_system;

void l_kaskcnsl(GtkWidget *wpredok)
{
l_kaskcnsl_data data;
char bros[512];
GdkColor color;

data.poisk.clear_data();

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Старовое сальдо по кодам целевых затрат"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_kaskcnsl_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Старовое сальдо по кодам целевых затрат"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_kaskcnsl_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_kaskcnsl_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_kaskcnsl_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_kaskcnsl_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_kaskcnsl_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_kaskcnsl_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_kaskcnsl_create_list(&data);

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
void l_kaskcnsl_create_list (class l_kaskcnsl_data *data)
{
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur,cur1;
char strsql[512];
int  kolstr=0;
SQL_str row,row1;
//GdkColor color;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_kaskcnsl_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_kaskcnsl_vibor),data);

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


sprintf(strsql,"select * from Kascnsl order by god,ks,kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(l_kaskcnsl_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->kod_cn_tv.ravno(),row[1],0) == 0 && \
     iceb_u_SRAV(data->god_tv.ravno(),row[0],0) == 0 && \
     iceb_u_SRAV(data->kas_tv.ravno(),row[2],0) == 0)
    data->snanomer=data->kolzap;

  //Год
  ss[COL_GOD].new_plus(row[0]);

  //Касса
  ss[COL_KAS].new_plus(row[2]);

  //Код
  ss[COL_KOD].new_plus(iceb_u_toutf(row[1]));
  

  //Наименование
  sprintf(strsql,"select naik from Kascn where kod=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    ss[COL_NAIM].new_plus(iceb_u_toutf(row1[0]));
  else
    ss[COL_NAIM].new_plus("");


  //Сальдо
  ss[COL_SALDO].new_plus(row[3]);


  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[5])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[4],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,ss[COL_KOD].ravno(),
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_GOD,ss[COL_GOD].ravno(),
  COL_KAS,ss[COL_KAS].ravno(),
  COL_SALDO,ss[COL_SALDO].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kod_cn_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_kaskcnsl_add_columns (GTK_TREE_VIEW (data->treeview));


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
zagolov.plus(gettext("Старовое сальдо по кодам целевых затрат"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

if(data->poisk.metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.kod.ravno(),gettext("Код"));
  iceb_str_poisk(&zagolov,data->poisk.god.ravno(),gettext("Год"));
  iceb_str_poisk(&zagolov,data->poisk.kas.ravno(),gettext("Касса"));
  

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

void l_kaskcnsl_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Год"), renderer,"text", COL_GOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Касса"), renderer,"text", COL_KAS,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сальдо"), renderer,"text", COL_SALDO,NULL);

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

void l_kaskcnsl_vibor(GtkTreeSelection *selection,class l_kaskcnsl_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *god;
gchar *kas;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_KOD,&kod,COL_KAS,&kas,COL_GOD,&god,NUM_COLUMNS,&nomer,-1);

data->kodv.new_plus(iceb_u_fromutf(kod));
data->god.new_plus(god);
data->kas.new_plus(kas);
data->snanomer=nomer;

g_free(kod);
g_free(god);
g_free(kas);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_kaskcnsl_knopka(GtkWidget *widget,class l_kaskcnsl_data *data)
{
//char    bros[512];
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_kaskcnsl_knopka knop=%d\n",knop);
data->kl_shift=0;
switch (knop)
 {
  case FK2:
    data->kod_cn_tv.new_plus("");
    if(l_kaskcnsl_v(&data->god_tv,&data->kas_tv,&data->kod_cn_tv,data->window) == 0)
      l_kaskcnsl_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(l_kaskcnsl_v(&data->god,&data->kas,&data->kodv,data->window) == 0)
      l_kaskcnsl_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(l_kaskcnsl_udzap(data) == 0)
      l_kaskcnsl_create_list(data);
    return;  
  

  case FK4:

    l_kaskcnsl_p(&data->poisk,data->window);
    l_kaskcnsl_create_list(data);
    return;  

  case FK5:
    l_kaskcnsl_rasp(data);
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

gboolean   l_kaskcnsl_key_press(GtkWidget *widget,GdkEventKey *event,class l_kaskcnsl_data *data)
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
void l_kaskcnsl_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_kaskcnsl_data *data)
{
//printf("l_kaskcnsl_v_row\n");
//printf("l_kaskcnsl_v_row корректировка\n");
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");


}

/*****************************/
/*Удаление записи            */
/*****************************/

int l_kaskcnsl_udzap(class l_kaskcnsl_data *data)
{

if(iceb_pbpds(1,data->god.ravno_atoi(),data->window) != 0)
 return(1);
if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
 return(1);

char strsql[512];

sprintf(strsql,"delete from Kascnsl where god=%s and kod=%s and ks=%s",data->god.ravno(),data->kodv.ravno(),data->kas.ravno());

iceb_sql_zapis(strsql,0,0,data->window);
return(0);


}
/****************************/
/*Проверка записей          */
/*****************************/

int  l_kaskcnsl_prov_row(SQL_str row,class l_kaskcnsl_data *data)
{
if(data->poisk.metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.kod.ravno(),row[1],0,0) != 0)
 return(1);
if(iceb_u_proverka(data->poisk.god.ravno(),row[0],0,0) != 0)
 return(1);
if(iceb_u_proverka(data->poisk.kas.ravno(),row[2],0,0) != 0)
 return(1);




   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_kaskcnsl_rasp(class l_kaskcnsl_data *data)
{
char strsql[512];
SQL_str row,row1;
FILE *ff;
SQLCURSOR cur,cur1;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from Kascnsl order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"kaskcnsl%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Старовое сальдо по кодам целевых затрат"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_u_zagolov(gettext("Старовое сальдо по кодам целевых затрат"),0,0,0,0,0,0,organ,ff);

if(data->poisk.metka_poi == 1)
 {
  if(data->poisk.kod.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код"),data->poisk.kod.ravno());
  if(data->poisk.god.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Год"),data->poisk.god.ravno());
  if(data->poisk.kas.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Касса"),data->poisk.kas.ravno());
 }

fprintf(ff,"\
-------------------------------------------------------------------------------------\n");

fprintf(ff,"\
Год |Касса|Код |Наименование        | Сальдо   |Дата и время запис.| Кто записал\n");

fprintf(ff,"\
-------------------------------------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;


cur.poz_cursor(0);
char naim[512];
double itogo=0.;
while(cur.read_cursor(&row) != 0)
 {
  if(l_kaskcnsl_prov_row(row,data) != 0)
    continue;

  //Наименование
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naik from Kascn where kod=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strncpy(naim,row1[0],sizeof(naim)-1);

  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[5]));

  //Кто записал
  s6.new_plus(iceb_kszap(row[4],1,data->window));

  fprintf(ff,"%-4s %-5s %-4s %-*.*s %*s %s %.*s\n",
  row[0],row[2],row[1],
  iceb_u_kolbait(20,naim),iceb_u_kolbait(20,naim),naim,
  iceb_u_kolbait(10,row[3]),row[3],
  s5.ravno(),
  iceb_u_kolbait(20,s6.ravno()),s6.ravno());

  if(iceb_u_strlen(naim) > 20)
   fprintf(ff,"%15s %s\n","",iceb_u_adrsimv(20,naim));
  itogo+=atof(row[3]);
 }
fprintf(ff,"\
-------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(36,gettext("Итого")),gettext("Итого"),itogo);

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);

iceb_ustpeh(imaf.ravno(0),3,data->window);

iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
