/*$Id: l_tk_saldo.c,v 1.18 2011-02-21 07:36:20 sasa Exp $*/
/*04.04.2005	31.03.2005	Белых А.И.	l_tk_saldo.c
Работа со списком сальдо по клиентам в подсистеме диспетчеризация такси
*/
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "taxi.h"
#include "l_tk_saldo.h"
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

enum
{
 COL_GOD,
 COL_KOD,
 COL_FIO,
 COL_SALDO,
 COL_VREM,
 COL_KTO,
 NUM_COLUMNS
};


class  tk_saldo_data
 {
  public:
  
  class tk_saldo_rek_data poi;
  
  iceb_u_str  kodv;
  int         godv;
  int         nomervsp; //Номер записи в списке

  GtkWidget *label_kolstr;
  GtkWidget *labelpoi;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     metkapoi;    //0-без поиска 1-с поиском
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  int       voz;  
  
  iceb_u_str kodkl;
  
  //Конструктор
  tk_saldo_data()
   {
    metkapoi=0;
    voz=0;
    snanomer=-2;
    kl_shift=0;
    window=treeview=NULL;
   }      
 };


void tk_saldo_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class tk_saldo_data *data);
gboolean   tk_saldo_key_press(GtkWidget *widget,GdkEventKey *event,class tk_saldo_data *data);
void  tk_saldo_knopka(GtkWidget *widget,class tk_saldo_data *data);
void tk_saldo_add_columns(GtkTreeView *treeview);
void tk_saldo_vibor(GtkTreeSelection *selection,class tk_saldo_data *data);
void tk_saldo_udzap(class tk_saldo_data *data);
int l_tk_saldo_v(iceb_u_str *,GtkWidget*);
int l_tk_saldo_p(class klient_rek_data *data,GtkWidget*);
void	klient_ras(class tk_saldo_data *data);
int   tk_saldo_prov_row(SQL_str row,class tk_saldo_data *data);
void tk_saldo_create_list (class tk_saldo_data *data);
//int tk_saldo_vvod_proc(char *kodkl,GtkWidget *wpredok);

int l_tk_saldo_v(int god,iceb_u_str *kodkl,GtkWidget *wpredok);
int l_tk_saldo_p(class tk_saldo_rek_data *datap,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;

int  l_tk_saldo(GtkWidget *wpredok)
{
tk_saldo_data data;
char strsql[300];
GdkColor color;
SQLCURSOR cur;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(strsql,"%s %s",name_system,iceb_u_toutf("Список сальдо по клиентам"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(tk_saldo_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Стартовые сальдо по счетам"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.labelpoi=gtk_label_new ("");
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.labelpoi,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.labelpoi,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(tk_saldo_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[SFK2]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(tk_saldo_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK3]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(tk_saldo_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(tk_saldo_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);


sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(tk_saldo_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(tk_saldo_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

tk_saldo_create_list(&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);


gtk_main();
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

printf("l_tk_saldo end\n");

return(data.voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void tk_saldo_create_list (class tk_saldo_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[300];
int  kolstr=0;
SQL_str row;
SQL_str row1;

printf("tk_saldo_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(tk_saldo_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(tk_saldo_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1,
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select * from Taxiklsal order by god,kod asc");

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
  
    if(tk_saldo_prov_row(row,data) != 0)
      continue;


//  if(iceb_u_SRAV(data->vvod.god.ravno(),row[0],0) == 0 && iceb_u_SRAV(data->vvod.shet.ravno(),row[1],0) == 0)
//    data->snanomer=data->kolzap;

  //год
  ss[COL_GOD].new_plus(iceb_u_toutf(row[0]));

  //Код клиента
  ss[COL_KOD].new_plus(iceb_u_toutf(row[1]));

  //Фамилия
  sprintf(strsql,"select fio from Taxiklient where kod='%s'",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1 )
    ss[COL_FIO].new_plus(iceb_u_toutf(row1[0]));
  else    
    ss[COL_FIO].new_plus("");

  //сальдо
  ss[COL_SALDO].new_plus(row[2]);

  //Дата и время записи
  ss[COL_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[4])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_u_kszap(row[3],0));

    
  gtk_list_store_append (model, &iter);


   gtk_list_store_set (model, &iter,
   COL_GOD,ss[COL_GOD].ravno(),
   COL_KOD,ss[COL_KOD].ravno(),
   COL_FIO,ss[COL_FIO].ravno(),
   COL_SALDO,ss[COL_SALDO].ravno(),
   COL_VREM,ss[COL_VREM].ravno(),
   COL_KTO,ss[COL_KTO].ravno(),
   NUM_COLUMNS,data->kolzap,
   -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

tk_saldo_add_columns (GTK_TREE_VIEW (data->treeview));


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

if(data->snanomer > -2)
 {
  if(data->snanomer  > data->kolzap)
     data->snanomer= data->kolzap;
      
  printf("poz=%d\n",data->snanomer);

  sprintf(strsql,"%d",data->snanomer);
  GtkTreePath *path=gtk_tree_path_new_from_string(strsql);

//  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,TRUE,0,0);
  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

  gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
  gtk_tree_path_free(path);
  data->snanomer=-2;
 }
else
 {
  if(data->kolzap > 0)
   {
    GtkTreePath *path=gtk_tree_path_new_from_string("0");

    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

    gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
  //  gtk_tree_view_row_activated(GTK_TREE_VIEW (data->treeview),path,NULL);

    gtk_tree_path_free(path);
    data->snanomer=-2;
   }
 }

iceb_u_str stroka;
iceb_u_str zagolov;

sprintf(strsql,"%s  %s:%d",
"Список сальдо по клиентам",
gettext("Количество записей"),
data->kolzap);

zagolov.plus(strsql);


gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());


gtk_widget_show(data->label_kolstr);

if(data->metkapoi == 1)
 {
  iceb_u_str spis;
  sprintf(strsql,"%s !!!",gettext("Поиск"));
  spis.plus(strsql);
  iceb_str_poisk(&spis,data->poi.god.ravno(),"Год");
  iceb_str_poisk(&spis,data->poi.kod.ravno(),"Код клиента");
  iceb_str_poisk(&spis,data->poi.saldo.ravno(),"Сальдо");

  gtk_label_set_text(GTK_LABEL(data->labelpoi),spis.ravno_toutf());
      
  gtk_widget_show(data->labelpoi); //Показываем

 }
else
 {
  gtk_widget_hide(data->labelpoi); //Показываем
 }





gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

printf("tk_saldo_create_list end\n");

}

/*****************/
/*Создаем колонки*/
/*****************/

void tk_saldo_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

printf("tk_saldo_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,iceb_u_toutf("Год"), renderer,"text", COL_GOD,NULL);

renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,iceb_u_toutf("Код клиента"), renderer,"text", COL_KOD,NULL);
renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,iceb_u_toutf("Фамилия клиента"), renderer,"text", COL_FIO,NULL);
renderer = gtk_cell_renderer_text_new ();


gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,iceb_u_toutf("Сальдо"), renderer,"text", COL_SALDO,NULL);
renderer = gtk_cell_renderer_text_new ();


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,"text", COL_KTO,NULL);

printf("tk_saldo_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void tk_saldo_vibor(GtkTreeSelection *selection,class tk_saldo_data *data)
{
printf("tk_saldo_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *god;
gchar *kod;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_GOD,&god,COL_KOD,&kod,NUM_COLUMNS,&nomer,-1);

data->kodv.new_plus(kod);
data->godv=atoi(god);
data->nomervsp=nomer;

g_free(kod);
g_free(god);

//printf("%s %d\n",data->kodv.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  tk_saldo_knopka(GtkWidget *widget,class tk_saldo_data *data)
{
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("tk_saldo_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {
  case FK2:
    repl.new_plus("");
    if( l_tk_saldo_v(0,&repl,data->window) == 0)
      tk_saldo_create_list(data);
    
    return;  

  case SFK2:

    if( l_tk_saldo_v(data->godv,&data->kodv,data->window) == 0)
      tk_saldo_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    tk_saldo_udzap(data);
    return;  

  case FK4:
    data->metkapoi=0;
    if(l_tk_saldo_p(&data->poi,data->window) == 0)
      data->metkapoi=1;
    tk_saldo_create_list(data);
    return;  

  case FK5:
    klient_ras(data);
    return;  

    
  case FK10:
//    printf("tk_saldo_knopka F10\n");
    gtk_widget_destroy(data->window);
    data->voz=0;
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   tk_saldo_key_press(GtkWidget *widget,GdkEventKey *event,class tk_saldo_data *data)
{
iceb_u_str repl;
printf("tk_saldo_key_press keyval=%d state=%d\n",
event->keyval,event->state);

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

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);

  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(TRUE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("tk_saldo_key_press-Нажата клавиша Shift\n");

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
void tk_saldo_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class tk_saldo_data *data)
{
printf("tk_saldo_v_row корректировка\n");
//l_tk_saldo_v(data);
data->snanomer=data->nomervsp;
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
}


/****************************/
/*Удалить запись*/
/****************************/

void tk_saldo_udzap(class tk_saldo_data *data)
{

char strsql[300];

iceb_u_str repl;

repl.plus(gettext("Удалить запись ? Вы уверены ?"));
if(iceb_menu_danet(&repl,2,data->window) == 2)
  return;


sprintf(strsql,"delete from Taxiklsal where god=%d and kod='%s'",
data->godv,data->kodv.ravno_filtr());
printf("tk_saldo_v_udzap %s\n",strsql);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
  return;

tk_saldo_create_list(data);

}
/****************************************/
/*Распечатка списка клиентов            */
/****************************************/

void	klient_ras(class tk_saldo_data *data)
{
char		strsql[300];
SQL_str		row;
SQL_str		row1;
int		kolstr=0;
FILE		*ff;
char		imaf[40];
SQLCURSOR cur;
SQLCURSOR cur1;
//printf("rasklient\n");
sprintf(strsql,"select * from Taxiklsal order by god,kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
sprintf(imaf,"tks%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

iceb_u_startfil(ff);
//fprintf(ff,"\x0F"); //Ужатый режим
iceb_u_zagolov("Сальдо по клиентам",0,0,0,0,0,0,"",ff);
fprintf(ff,"\
-----------------------------------------------------\n\
 Год| Код | Фамилия Имя Отчество         |  Сальдо  | \n\
-----------------------------------------------------\n");

char fio[200];

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
   if(tk_saldo_prov_row(row,data) != 0)
      continue;
  memset(fio,'\0',sizeof(fio));
  
  sprintf(strsql,"select fio from Taxiklient where kod='%s'",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1 )
    strncpy(fio,row1[0],sizeof(fio)-1);
    
  fprintf(ff,"%4s %-5s %-30.30s %10s\n",row[0],row[1],fio,row[2]);

 }
fprintf(ff,"\
-----------------------------------------------------\n");

//fprintf(ff,"\x12"); //Нормальные буквы
iceb_podpis(ff,data->window);
fclose(ff);

iceb_u_spisok fil;
iceb_u_spisok nazv;

fil.plus(imaf);
nazv.plus(gettext("Список клиентов"));

//printf("rasklient-fine\n");

iceb_rabfil(&fil,&nazv,"",0,data->window);


}
/**********************************/
/*Проверка записи на условия поиска*/
/************************************/

int   tk_saldo_prov_row(SQL_str row,class tk_saldo_data *data)
{

if(data->metkapoi == 0)
 return(0);

if(iceb_u_proverka(data->poi.kod.ravno(),row[1],0,0) != 0)
 return(1);

if(data->poi.god.getdlinna() > 1)
 if(atoi(row[0]) != data->poi.god.ravno_atoi())
  return(1);

if(data->poi.saldo.getdlinna() > 1)
 if(atof(row[2]) != data->poi.saldo.ravno_atof())
  return(1);

   
return(0);
}
