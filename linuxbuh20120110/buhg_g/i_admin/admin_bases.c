/*$Id: admin_bases.c,v 1.14 2011-02-21 07:35:50 sasa Exp $*/
/*09.11.2010	05.02.2009	Белых А.И.	admin_bases.c
Просмотр списка баз
*/
#include <glib.h>
//zs
//#include  "buhg_g.h"
#include  "../headers/buhg_g.h"
//ze

enum
 {
  FK3,
  FK5,
  FK6,
  FK10,
  KOL_F_KL  
 };

enum
 {
  COL_BAZA,
  COL_KODIROVKA,
  COL_NAIM_ORG,
  NUM_COLUMNS
 };

class admin_bases_data
 {
  public:
  
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *sw;
  GtkWidget *label;
  GtkWidget *knopka[KOL_F_KL];
    
  int        kl_shift;
  class iceb_u_str host;
  class iceb_u_str imabaz;
  class iceb_u_str login;

  int snanomer;  
  int kolzap;
  int voz;
  int metka_r;
  admin_bases_data() //Конструктор
   {
    kl_shift=0;
    treeview=NULL;
    snanomer=kolzap=0;
    metka_r=0;
    voz=0;
   }
   
 };

gboolean   admin_bases_key_press(GtkWidget *widget,GdkEventKey *event,class admin_bases_data *data);
static void admin_bases_add_columns (GtkTreeView *treeview);
void admin_bases_add_f10(GtkTreeView *treeview);
void admin_bases_create_list(class admin_bases_data *data);
void  admin_bases_knopka(GtkWidget *widget,class admin_bases_data *data);
void admin_bases_vibor(GtkTreeSelection *selection,class admin_bases_data *data);
int   admin_alx(const char *imabazv,GtkWidget *wpredok);
void admin_bases_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class admin_bases_data *data);

int admin_del_alx(const char *put_na_kat,GtkWidget *wpredok);
int admin_icebuser(const char *imabazv,GtkWidget *wpredok);

extern char *name_system;
extern SQL_baza bd;
extern class iceb_u_str kzkb;
extern char *imabaz;

int admin_bases(int metka_r, /*0-просмотр 1-просмотр с выбором*/
class iceb_u_str *vimabaz,GtkWidget *wpredok)
{
class admin_bases_data data;
data.metka_r=metka_r;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

char bros[512];
sprintf(bros,"%s %s",name_system,gettext("Список баз данных"));

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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(admin_bases_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);


GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(vbox1);
gtk_widget_show(vbox2);
gtk_widget_show(hbox);

GtkWidget *label=gtk_label_new(gettext("Список баз данных"));
gtk_box_pack_start((GtkBox*)vbox2,label,FALSE,FALSE,0);
gtk_widget_show(label);


data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//printf("Кнопки\n");

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(admin_bases_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F5 %s",gettext("Настройка"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(admin_bases_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Просмотр списка файлов настройки"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Операторы"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(admin_bases_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Просмотр списка операторов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);


sprintf(bros,"F10 %s",iceb_u_toutf("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK10], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(admin_bases_knopka),&data);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],iceb_u_toutf("Завершение работы"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);

gtk_widget_grab_focus(data.knopka[FK10]);

admin_bases_create_list(&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);

//gtk_widget_hide (data.label_poisk);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
if(data.voz == 0)
 vimabaz->new_plus(data.imabaz.ravno());
 
return(data.voz);


}

/*****************/
/*Создание списка*/
/*****************/
void admin_bases_create_list(class admin_bases_data *data)
{
char strsql[512];
GtkListStore *model=NULL;
GtkTreeIter iter;
//printf("admin_bases_create_list\n");


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);


g_signal_connect(data->treeview,"row_activated",G_CALLBACK(admin_bases_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(admin_bases_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


sprintf(strsql,"SET NAMES %s",kzkb.ravno());
iceb_sql_zapis(strsql,1,1,data->window);


model = gtk_list_store_new (NUM_COLUMNS+1,
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_INT);

sprintf(strsql,"SHOW DATABASES");
class SQLCURSOR cur;
SQL_str row;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

//Определяем количество файлов
//int kolfil=data->imaf->kolih();
class iceb_u_str naim_org("");
class iceb_u_str naim_org_v;
class SQLCURSOR cur1;
SQL_str row1;
class iceb_u_str kodirovka;
int voz=0;
data->kolzap=0;
while(cur.read_cursor(&row) != 0)
 {
//  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(iceb_u_SRAV("mysql",row[0],0) == 0)
   continue;
   
  /*Устанавливаем базу данных для того чтобы прочесть какая у неё кодировка по умолчанию*/
  sprintf(strsql,"USE %s",row[0]);
  sql_zap(&bd,strsql);

  /*читаем кодировку базы данных*/  
  kodirovka.new_plus("");
  if(sql_readkey(&bd,"SELECT @@character_set_database",&row1,&cur1) > 0)
   kodirovka.new_plus(row1[0]);
  /*Узнаём наименование организации если это iceB база*/

  naim_org.new_plus("");

  sprintf(strsql,"select naikon from %s.Kontragent where kodkon='00'",row[0]);
  if((voz=sql_readkey(&bd,strsql,&row1,&cur1)) > 0)
   naim_org.new_plus(row1[0]);

  if(voz < 0)
  //zs
  //naim_org_v.new_plus(iceb_u_toutf("Это не iceB база данных"));
  naim_org_v.new_plus(iceb_u_toutf("Это не linuxbuh.ru база данных"));
   //ze
  if(voz == 0)
   naim_org_v.new_plus(iceb_u_toutf("Не введён контрагент с кодом 00"));

  if(voz > 0)
   {   
    naim_org_v.new_plus("");
//    if(iceb_u_SRAV(kodirovka.ravno(),"koi8",1) == 0)
    if(g_utf8_validate(naim_org.ravno(),-1,NULL) != TRUE)
     naim_org_v.new_plus(naim_org.ravno_toutf());
    else    
     naim_org_v.new_plus(naim_org.ravno());
   }
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_BAZA,row[0],
  COL_KODIROVKA,kodirovka.ravno(),
  COL_NAIM_ORG,naim_org_v.ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);
  data->kolzap++;
 }
/*возвращаем базу данных по умолчанию*/
iceb_sql_zapis("USE mysql",1,1,data->window);

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

admin_bases_add_columns (GTK_TREE_VIEW (data->treeview));


if(kolstr == 0)
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

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

}

/**********************************/
/*Новая программа создания колонок*/
/**********************************/

static void admin_bases_add_columns (GtkTreeView *treeview)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  /* ********** */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes (gettext("База"),
						     renderer,
						     "text",
						     COL_BAZA,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_BAZA);
  gtk_tree_view_append_column (treeview, column);


  /* ********** */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes (gettext("Кодировка"),
						     renderer,
						     "text",
						     COL_KODIROVKA,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_KODIROVKA);
  gtk_tree_view_append_column (treeview, column);

  /* ********** */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes (gettext("Организация"),
						     renderer,
						     "text",
						     COL_NAIM_ORG,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_NAIM_ORG);
  gtk_tree_view_append_column (treeview, column);

}



/****************************/
/*Выбор строки*/
/**********************/

void admin_bases_vibor(GtkTreeSelection *selection,class admin_bases_data *data)
{
//printf("admin_basesn_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *imabaz;
gint nomer;

gtk_tree_model_get(model,&iter,COL_BAZA,&imabaz,NUM_COLUMNS,&nomer,-1);

data->imabaz.new_plus(imabaz);
data->snanomer=nomer;
g_free(imabaz);

}
/****************************/
/*Выбор строки*/
/**********************/
void admin_bases_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class admin_bases_data *data)
{
if(data->metka_r == 1)
 {
  data->voz=0;

  gtk_widget_destroy(data->window);
 }

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  admin_bases_knopka(GtkWidget *widget,class admin_bases_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("admin_basesn_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается
char strsql[512];
class iceb_u_str put_na_alx("");

switch (knop)
 {

  case FK3: /*Удалить запись в таблице*/
    if(iceb_u_SRAV("mysql",data->imabaz.ravno(),0) == 0  || iceb_u_SRAV("information_schema",data->imabaz.ravno(),0) == 0) 
     {
      iceb_menu_soob("Эту базу удалять нельзя!",data->window);
      return;
     }
    if(iceb_menu_danet(gettext("Удалить базу? Вы уверены?"),2,data->window) == 2)
     return;
    if(iceb_parol(1,data->window) != 0)
     return;
    sprintf(strsql,"drop database %s",data->imabaz.ravno());
    iceb_sql_zapis(strsql,1,1,data->window);    
    admin_bases_create_list(data);

    return;  

  case FK5: /*Работа с файлом настройки*/
    if(imabaz != NULL)
     delete [] imabaz;   
    imabaz=new char[data->imabaz.getdlinna()];
    strcpy(imabaz,data->imabaz.ravno());

//    sprintf(strsql,"USE %s",imabaz);

//    iceb_sql_zapis(strsql,1,0,data->window);

//    iceb_f_redfil("nastsys.alx",0,data->window);
    admin_alx(imabaz,data->window);

    iceb_sql_zapis("USE mysql",1,1,data->window);

    return;  

  case FK6: /*Работа со списком операторов*/
    if(imabaz != NULL)
     delete [] imabaz;   
    imabaz=new char[data->imabaz.getdlinna()];
    strcpy(imabaz,data->imabaz.ravno());
    admin_icebuser(data->imabaz.ravno(),data->window);
    return;
       
  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   admin_bases_key_press(GtkWidget *widget,GdkEventKey *event,class admin_bases_data *data)
{
//printf("admin_basesn_key_press keyval=%d state=%d\n",
//event->keyval,event->state);

switch(event->keyval)
 {

   
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
//    printf("kontr_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
//    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
