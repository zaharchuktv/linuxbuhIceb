/*$Id: l_nalog.c,v 1.5 2011-02-21 07:35:53 sasa Exp $*/
/*14.10.2010	14.10.2010	Белых А.И.	l_nalog.c
Работа со списком категорий
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze

enum
{
  COL_DNDN,
  COL_PROC,
  COL_KOMENT,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK2,
  FK3,
  FK5,
  FK10,
  SFK2,
  KOL_F_KL
};

class  l_nalog_data
 {
  public:

  class iceb_u_str dndn; /*Дата начала действия налога*/
  
  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  
  int nomnal;
  //Конструктор
  l_nalog_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
   }      
 };

gboolean   l_nalog_key_press(GtkWidget *widget,GdkEventKey *event,class l_nalog_data *data);
void l_nalog_vibor(GtkTreeSelection *selection,class l_nalog_data *data);
void l_nalog_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_nalog_data *data);
void  l_nalog_knopka(GtkWidget *widget,class l_nalog_data *data);
void l_nalog_add_columns (GtkTreeView *treeview);
void l_nalog_udzap(class l_nalog_data *data);
void l_nalog_rasp(class l_nalog_data *data);
void l_nalog_create_list (class l_nalog_data *data);

int l_nalog_v(int nomnal,const char *dndn,GtkWidget *wpredok);
int l_nalog_p(class l_nalog_rek *rek_poi,GtkWidget *wpredok);


extern char *organ;
extern SQL_baza  bd;
extern char      *name_system;

int l_nalog(int nomnal,GtkWidget *wpredok)
{
l_nalog_data data;
char bros[312];

data.nomnal=nomnal;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);

if(nomnal == 0)
 sprintf(bros,"%s %s",name_system,gettext("Подоходный налог"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_nalog_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

if(nomnal == 0)
  data.label_kolstr=gtk_label_new (gettext("Подоходный налог"));
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

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_nalog_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_nalog_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_nalog_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);


sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_nalog_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_nalog_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_nalog_create_list(&data);

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
void l_nalog_create_list (class l_nalog_data *data)
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

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_nalog_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_nalog_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select * from Nalog where vn=%d order by dndn desc",data->nomnal);

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
  


  //дата
  
  ss[COL_DNDN].new_plus(iceb_u_datzap(row[1]));
  
  //процент
  ss[COL_PROC].new_plus(row[2]);

  //коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[3]));
  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[5])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[4],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DNDN,ss[COL_DNDN].ravno(),
  COL_PROC,ss[COL_PROC].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_nalog_add_columns (GTK_TREE_VIEW (data->treeview));


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
if(data->nomnal == 0)
 zagolov.plus(gettext("Подоходный налог"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());
/************************
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
 gtk_widget_hide(data->label_poisk); 
*************/
gtk_widget_show(data->label_kolstr);

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_nalog_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата"),renderer,"text",COL_DNDN,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DNDN);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Процент"),renderer,"text",COL_PROC,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_PROC);
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

void l_nalog_vibor(GtkTreeSelection *selection,class l_nalog_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *dndn;
gint nomer;

gtk_tree_model_get(model,&iter,COL_DNDN,&dndn,NUM_COLUMNS,&nomer,-1);

data->dndn.new_plus(dndn);
data->snanomer=nomer;

g_free(dndn);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_nalog_knopka(GtkWidget *widget,class l_nalog_data *data)
{
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_nalog_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    if( l_nalog_v(data->nomnal,"",data->window) == 0)
      l_nalog_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if( l_nalog_v(data->nomnal,data->dndn.ravno(),data->window) == 0)
      l_nalog_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_nalog_udzap(data);
    l_nalog_create_list(data);
    return;  
  


  case FK5:
    l_nalog_rasp(data);
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

gboolean   l_nalog_key_press(GtkWidget *widget,GdkEventKey *event,class l_nalog_data *data)
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
//    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void l_nalog_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_nalog_data *data)
{
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");

}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_nalog_udzap(class l_nalog_data *data)
{


char strsql[512];

sprintf(strsql,"delete from Nalog where vn=%d and dndn='%s'",data->nomnal,data->dndn.ravno_sqldata());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_nalog_rasp(class l_nalog_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from Nalog where vn=%d order by dndn desc",data->nomnal);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"nalog%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список категорий работников"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }
if(data->nomnal == 0)
  iceb_u_zagolov(gettext("Подоходный налог"),0,0,0,0,0,0,organ,ff);

/**************
if(data->metka_poi == 1)
 {
  if(data->poisk.kod.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код"),data->poisk.kod.ravno());
  if(data->poisk.naim.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Наименование"),data->poisk.naim.ravno());
 }
***************/
fprintf(ff,"\
------------------------------------------------------------------------------\n");

fprintf(ff,"\
   Дата   | Процент  |  Коментарий        |Дата и время запис.| Кто записал\n");

fprintf(ff,"\
------------------------------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;


cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {

  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[5]));

  //Кто записал
  s6.new_plus(iceb_kszap(row[4],1,data->window));

  fprintf(ff,"%s %10.2f %-*.*s %s %s\n",
  iceb_u_datzap(row[1]),
  atof(row[2]),
  iceb_u_kolbait(20,row[3]),iceb_u_kolbait(20,row[3]),row[3],
  s5.ravno(),s6.ravno());

  for(int ii=20; ii < iceb_u_strlen(row[3]); ii+=20)
   fprintf(ff,"%4s %-*.*s\n",
   "",
   iceb_u_kolbait(20,iceb_u_adrsimv(ii,row[3])),
   iceb_u_kolbait(20,iceb_u_adrsimv(ii,row[3])),
   iceb_u_adrsimv(ii,row[3]));
  

 }
fprintf(ff,"\
------------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
