/*$Id: l_vmat.c,v 1.24 2011-02-21 07:36:21 sasa Exp $*/
/*22.08.2006	26.02.2004	Белых А.И.	l_vmat.c
Выбор материалла или услуг из списка 
Если вернули 0-выбрали
             1-нет
*/

#include <stdlib.h>
#include <unistd.h>
//#include <gtimer.h>
#include <glib.h>
#include "i_rest.h"

enum
{
  FK2,
  FK_VVERH,
  FK_VNIZ,
  FK10,
  KOL_F_KL
};

enum
{
 COL_KOD,
 COL_NAIM,
 COL_CENA,
 COL_EI,
 NUM_COLUMNS
};
 
class  vmat_data
 {
  public:
  int        kodgr;  //Код группы для которой ищем записи
  iceb_u_str naigr;  //наименование группы
  
  iceb_u_str kod; //выбранный код записи

  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  gint       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  int       voz;     //0-код выбран 1-нет  
  int       metkarr; //0-работа со списком товаров 1-списком услуг
  
  short metka_released;
  
  //Конструктор
  vmat_data()
   {
    voz=0;
    kod.new_plus("");
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    kodgr=0;
    naigr.plus("");
    metka_released=0;
   }      
 };

void vmat_create_list (class vmat_data *data);
void vmat_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class vmat_data *data);
gboolean   vmat_key_press(GtkWidget *widget,GdkEventKey *event,class vmat_data *data);
void  vmat_knopka(GtkWidget *widget,class vmat_data *data);
void vmat_add_columns(GtkTreeView *treeview);
void vmat_vibor(GtkTreeSelection *selection,class vmat_data *data);

void l_vmat_setstr(class vmat_data *data);
void  l_vmat_knopka_released(GtkWidget *widget,class vmat_data *data);
void  l_vmat_knopka_vniz_press(GtkWidget *widget,class vmat_data *data);
void  l_vmat_knopka_vverh_press(GtkWidget *widget,class vmat_data *data);

extern SQL_baza	bd;
extern char *name_system;

int l_vmat(int *kodv,//выбранный код
int kodgr, //Код группы для которой ищем записи
int metkarr, //0-работа со списком товаров 1-списком услуг
GtkWidget *wpredok)
{
vmat_data data;
char strsql[300];
//GdkColor color;
SQLCURSOR cur;
SQL_str   row;
printf("l_vmat %d\n",kodgr);

data.kodgr=kodgr;
data.metkarr=metkarr;
if(metkarr == 0)
  sprintf(strsql,"select naik from Grup where kod=%d",kodgr);
if(metkarr == 1)
  sprintf(strsql,"select naik from Uslgrup where kod=%d",kodgr);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 data.naigr.new_plus(row[0]);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

gtk_window_set_default_size (GTK_WINDOW  (data.window),600,600);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);

if(metkarr == 0)
  sprintf(strsql,"%s %s",name_system,gettext("Список товаров"));
if(metkarr == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Список услуг"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(vmat_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Выберите нужную запись"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Выбор"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(vmat_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Выбор отмеченной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

GtkWidget *arrow=gtk_arrow_new(GTK_ARROW_UP,GTK_SHADOW_OUT);

//sprintf(strsql,"%s",gettext("Вверх"));
//data.knopka[FK_VVERH]=gtk_button_new_with_label(strsql);
data.knopka[FK_VVERH]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data.knopka[FK_VVERH]),arrow);

gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK_VVERH], TRUE, TRUE, 0);
//gtk_signal_connect(GTK_OBJECT(data.knopka[FK_VVERH]), "clicked",GTK_SIGNAL_FUNC(vmat_knopka),&data);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK_VVERH]), "pressed",GTK_SIGNAL_FUNC(l_vmat_knopka_vverh_press),&data);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK_VVERH]), "released",GTK_SIGNAL_FUNC(l_vmat_knopka_released),&data);
//tooltips[FK_VVERH]=gtk_tooltips_new();
//gtk_tooltips_set_tip(tooltips[FK_VVERH],data.knopka[FK_VVERH],gettext("Выбор отмеченной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK_VVERH]),(gpointer)FK_VVERH);
gtk_widget_show(data.knopka[FK_VVERH]);
gtk_widget_show(arrow);

arrow=gtk_arrow_new(GTK_ARROW_DOWN,GTK_SHADOW_OUT);

//sprintf(strsql,"%s",gettext("Вниз"));
//data.knopka[FK_VNIZ]=gtk_button_new_with_label(strsql);

data.knopka[FK_VNIZ]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data.knopka[FK_VNIZ]),arrow);

gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK_VNIZ], TRUE, TRUE, 0);
//gtk_signal_connect(GTK_OBJECT(data.knopka[FK_VNIZ]), "clicked",GTK_SIGNAL_FUNC(vmat_knopka),&data);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK_VNIZ]), "pressed",GTK_SIGNAL_FUNC(l_vmat_knopka_vniz_press),&data);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK_VNIZ]), "released",GTK_SIGNAL_FUNC(l_vmat_knopka_released),&data);
//tooltips[FK_VNIZ]=gtk_tooltips_new();
//gtk_tooltips_set_tip(tooltips[FK_VNIZ],data.knopka[FK_VNIZ],gettext("Выбор отмеченной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK_VNIZ]),(gpointer)FK_VNIZ);
gtk_widget_show(data.knopka[FK_VNIZ]);
gtk_widget_show(arrow);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(vmat_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

vmat_create_list (&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);

gtk_main();

printf("l_vmat end\n");
*kodv=atoi(data.kod.ravno());
return(data.voz);
}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void vmat_create_list (class vmat_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
char strsql[400];
int  kolstr=0;
SQLCURSOR cur;
SQL_str   row;

printf("vmat_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(vmat_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(vmat_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);



model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


//gtk_list_store_clear(model);
if(data->metkarr == 0)
  sprintf(strsql,"select kodm,naimat,ei,cenapr from Material where kodgr=%d \
  and cenapr != 0. and ei != '' order by naimat asc",data->kodgr);
if(data->metkarr == 1)
  sprintf(strsql,"select kodus,naius,ei,cena from Uslugi where kodgr=%d \
  and cena != 0. and ei != '' order by naius asc",data->kodgr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str s1;
iceb_u_str s2;
iceb_u_str s3;
iceb_u_str s4;

float kolstr1=0.;
data->kolzap=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  

  //код
  s1.new_plus(row[0]);
  
  //наименование
  s2.new_plus(iceb_u_toutf(row[1]));

  //единица измерения
  s3.new_plus(iceb_u_toutf(row[2]));


  //цена
  sprintf(strsql,"%.2f",atof(row[3]));  
  s4.new_plus(strsql);

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,s1.ravno(),
  COL_NAIM,s2.ravno(),
  COL_EI,s3.ravno(),
  COL_CENA,s4.ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

vmat_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK_VVERH]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK_VNIZ]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK_VVERH]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK_VNIZ]),TRUE);//Доступна
 }


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

if(data->kolzap > 0)
  l_vmat_setstr(data); //стать на нужную строку

iceb_u_str stroka;
iceb_u_str zagolov;

sprintf(strsql,"%s:%d %s",gettext("Группа"),data->kodgr,data->naigr.ravno());
zagolov.plus_ps(strsql);
zagolov.plus(gettext("Выберите нужную запись"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());


gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void vmat_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

printf("vmat_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
//  g_object_set(G_OBJECT(renderer),"foreground","red",NULL);

//g_object_set(G_OBJECT(renderer),"background","black",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
					       -1,gettext("Код"), renderer,
					       "text", COL_KOD,
					       NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,\
gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,\
gettext("Цена"), renderer,"text", COL_CENA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,\
gettext("Е/и"), renderer,"text", COL_EI,NULL);



printf("vmat_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void vmat_vibor(GtkTreeSelection *selection,class vmat_data *data)
{
printf("vmat_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gint  nomer;


gtk_tree_model_get(model,&iter,0,&kod,NUM_COLUMNS,&nomer,-1);


data->kod.new_plus(kod);
data->snanomer=nomer;

g_free(kod);

//printf(" %s %d\n",data->kod.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  vmat_knopka(GtkWidget *widget,class vmat_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("vmat_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {
  case FK2:
    if(data->kolzap == 0)
      return;
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK_VVERH:
    if(data->kolzap == 0)
      return;

    data->snanomer--;
    l_vmat_setstr(data); //стать на нужную строку

    return;  

  case FK_VNIZ:
    if(data->kolzap == 0)
      return;
    
    data->snanomer++;
    l_vmat_setstr(data); //стать на нужную строку
    return;  

  case FK10:
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vmat_key_press(GtkWidget *widget,GdkEventKey *event,class vmat_data *data)
{
iceb_u_str repl;
printf("vmat_key_press keyval=%d state=%d\n",
event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);
   
  
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(TRUE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("vmat_key_press-Нажата клавиша Shift\n");

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
void vmat_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class vmat_data *data)
{
printf("vmat_v_row\n");
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
}
/***********************************/
/*Стат на нужную строку*/
/************************************/
void l_vmat_setstr(class vmat_data *data)
{
  if(data->snanomer  > data->kolzap-1)
     data->snanomer= data->kolzap-1;
  if(data->snanomer < 0)
     data->snanomer=0;
      
char strsql[50];
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
void  l_vmat_knopka_vniz_press(GtkWidget *widget,class vmat_data *data)
{
printf("кнопка вниз нажата\n");
data->metka_released=1;
while(data->metka_released == 1)
 {
  data->snanomer++;
  printf("вниз-%d\n",data->snanomer);

  l_vmat_setstr(data); //стать на нужную строку

  g_usleep(SLEEP_STR);

  iceb_refresh();  //обязательно после sleep
 }


}
/*************************/
/*Кнопка вверх нажата*/
/**********************************/

void  l_vmat_knopka_vverh_press(GtkWidget *widget,class vmat_data *data)
{
printf("кнопка вверх нажата\n");
data->metka_released=1;
while(data->metka_released == 1)
 {
  data->snanomer--;
  printf("вверх-%d\n",data->snanomer);

  l_vmat_setstr(data); //стать на нужную строку


  g_usleep(SLEEP_STR);

  iceb_refresh();  //обязательно после sleep
 }


}
/*************************/
/*Кнопка вниз/вверх отжата*/
/**********************************/
void  l_vmat_knopka_released(GtkWidget *widget,class vmat_data *data)
{
printf("кнопка отжата\n");
data->metka_released=0;
}

