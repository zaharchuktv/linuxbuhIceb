/*$Id: l_saldo_sh.c,v 1.33 2011-02-21 07:35:53 sasa Exp $*/
/*12.11.2009	30.12.2003	Белых А.И.	l_saldo_sh.c
Работа со списком сальдо по счетам
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include  "l_saldo_sh.h"

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
  COL_SHET,
  COL_DEBET,
  COL_KREDIT,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

class  saldo_sh_data
 {
  public:

  saldo_sh_poi poisk;

  class iceb_u_str god_tv;
  class iceb_u_str shet_tv;
  
  iceb_u_str godv;
  iceb_u_str shetv;

  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *knopka[KOL_F_KL];
//  GtkWidget *knopkav[KOL_VFK];
//  GtkWidget *entry[KOLENTER];
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *vwindow;
  GtkWidget *pwindow;
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  
  //Конструктор
  saldo_sh_data()
   {
    snanomer=0;
    kl_shift=0;
    vwindow=pwindow=window=treeview=NULL;
    god_tv.plus("");
    shet_tv.plus("");
    
   }      
 };


void saldo_sh_create_list (class saldo_sh_data *data);
gboolean   saldo_sh_key_press(GtkWidget *widget,GdkEventKey *event,class saldo_sh_data *data);
void saldo_sh_vibor(GtkTreeSelection *selection,class saldo_sh_data *data);
void saldo_sh_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class saldo_sh_data *data);
void  saldo_sh_knopka(GtkWidget *widget,class saldo_sh_data *data);
void saldo_sh_add_columns (GtkTreeView *treeview);
int l_saldo_sh_v(class iceb_u_str *god_k,class iceb_u_str *shet_k,GtkWidget *wpredok);
int l_saldo_sh_p(class saldo_sh_poi *rkp,GtkWidget *wpredok);
int saldo_sh_udzap(class saldo_sh_data *data);
int  saldo_sh_prov_row(SQL_str row,class saldo_sh_data *data);
void saldo_sh_rasp(class saldo_sh_data *data);

extern char *organ;
extern SQL_baza	bd;
extern char *name_system;

void l_saldo_sh(GtkWidget *wpredok)
{
saldo_sh_data data;
char bros[512];
GdkColor color;

data.poisk.clear_zero();

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Сальдо"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(saldo_sh_key_press),&data);
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
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(saldo_sh_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[SFK2]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(saldo_sh_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK3]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(saldo_sh_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(saldo_sh_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(saldo_sh_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(saldo_sh_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

saldo_sh_create_list (&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

printf("l_saldo_sh end\n");


}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void saldo_sh_create_list (class saldo_sh_data *data)
{
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;
//GdkColor color;

//printf("saldo_sh_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(saldo_sh_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(saldo_sh_vibor),data);

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


sprintf(strsql,"select gs,ns,deb,kre,ktoi,vrem from Saldo where kkk=0 order by gs asc,ns asc");

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
iceb_u_str s5;
iceb_u_str s6;

data->kolzap=0;
double debet=0.;
double kredit=0.;

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(saldo_sh_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->god_tv.ravno(),row[0],0) == 0 && iceb_u_SRAV(data->shet_tv.ravno(),row[1],0) == 0)
    data->snanomer=data->kolzap;

  //Год
  s1.new_plus(iceb_u_toutf(row[0]));
  
  //Счет
  s2.new_plus(iceb_u_toutf(row[1]));

  //Дебет
  s3.new_plus(row[2]);
  debet+=atof(row[2]);

  //Кредит
  s4.new_plus(iceb_u_toutf(row[3]));
  kredit+=atof(row[3]);

  //Дата и время записи
  s5.new_plus(iceb_u_toutf(iceb_u_vremzap(row[5])));

  //Кто записал
  s6.new_plus(iceb_kszap(row[4],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_GOD,s1.ravno(),
  COL_SHET,s2.ravno(),
  COL_DEBET,s3.ravno(),
  COL_KREDIT,s4.ravno(),
  COL_DATA_VREM,s5.ravno(),
  COL_KTO,s6.ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

data->god_tv.new_plus("");
data->shet_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

saldo_sh_add_columns (GTK_TREE_VIEW (data->treeview));


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

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Стартовые сальдо по счетам"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

sprintf(strsql,"%s: %.2f %s: %.2f",
gettext("Дебет"),debet,
gettext("Кредит"),kredit);

zagolov.ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

if(data->poisk.metka_poi == 1)
 {
//  printf("Формирование заголовка с реквизитами поиска.\n");
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.god.ravno(),gettext("Год"));
  iceb_str_poisk(&zagolov,data->poisk.shet.ravno(),gettext("Счет"));
  iceb_str_poisk(&zagolov,data->poisk.debet.ravno(),gettext("Дебет"));
  iceb_str_poisk(&zagolov,data->poisk.kredit.ravno(),gettext("Кредит"));

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

void saldo_sh_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

printf("saldo_sh_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
//  g_object_set(G_OBJECT(renderer),"foreground","red",NULL);

//g_object_set(G_OBJECT(renderer),"background","black",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
					       -1,gettext("Год"), renderer,
					       "text", COL_GOD,
					       NULL);

renderer = gtk_cell_renderer_text_new ();
//g_object_set(G_OBJECT(renderer),"foreground","white",NULL);
//g_object_set(G_OBJECT(renderer),"background","black",NULL);


gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
					       -1,gettext("Счет"), renderer,
					       "text", COL_SHET,
					       NULL);
renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дебет"), renderer,
"text", COL_DEBET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кредит"), renderer,
"text", COL_KREDIT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);
printf("saldo_sh_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void saldo_sh_vibor(GtkTreeSelection *selection,class saldo_sh_data *data)
{
printf("saldo_sh_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *god;
gchar *shet;
gint  nomer;


gtk_tree_model_get(model,&iter,0,&god,1,&shet,NUM_COLUMNS,&nomer,-1);

data->godv.new_plus(iceb_u_fromutf(god));
data->shetv.new_plus(iceb_u_fromutf(shet));
data->snanomer=nomer;

g_free(god);
g_free(shet);

//printf("%s %s %d\n",data->godv.ravno(),data->shetv.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  saldo_sh_knopka(GtkWidget *widget,class saldo_sh_data *data)
{
//char    bros[512];
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:

    if(l_saldo_sh_v(&data->god_tv,&data->shet_tv,data->window) == 0)
      saldo_sh_create_list(data);
    return;  

  case SFK2:
    if(l_saldo_sh_v(&data->godv,&data->shetv,data->window) == 0)
      saldo_sh_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(saldo_sh_udzap(data) == 0)
      saldo_sh_create_list(data);
    return;  


  case FK4:
    l_saldo_sh_p(&data->poisk,data->window);
    saldo_sh_create_list(data);
    return;  

  case FK5:
//    gtk_widget_hide(data->window);
//    saldo_sh_rasspis(data);
//    gtk_widget_show(data->window);
    saldo_sh_rasp(data);
    return;  

    
  case FK10:
//    printf("saldo_sh_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   saldo_sh_key_press(GtkWidget *widget,GdkEventKey *event,class saldo_sh_data *data)
{
iceb_u_str repl;
printf("saldo_sh_key_press keyval=%d state=%d\n",
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
    printf("saldo_sh_key_press-Нажата клавиша Shift\n");

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
void saldo_sh_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class saldo_sh_data *data)
{
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
}

/*****************************/
/*Удаление записи            */
/*****************************/

int saldo_sh_udzap(class saldo_sh_data *data)
{
if(iceb_pvglkni(1,data->godv.ravno_atoi(),data->window) != 0)
 return(1);

if(iceb_pbsh(1,data->godv.ravno_atoi(),data->shetv.ravno(),"","",data->window) != 0)
 return(1);
 
iceb_u_str sp;
sp.plus(gettext("Удалить запись ? Вы уверены ?"));
if(iceb_menu_danet(&sp,2,data->window) == 2)
 return(1);

char strsql[512];

sprintf(strsql,"delete from Saldo where kkk=0 and gs=%d and ns='%s'",
data->godv.ravno_atoi(),data->shetv.ravno());

iceb_sql_zapis(strsql,0,0,data->window);

return(0);


}
/****************************/
/*Проверка записей          */
/*****************************/

int  saldo_sh_prov_row(SQL_str row,class saldo_sh_data *data)
{
if(data->poisk.metka_poi == 0)
 return(0);

if(data->poisk.god.getdlinna() > 1)
 if(atoi(data->poisk.god.ravno()) != atoi(row[0]))
   return(1);

if(iceb_u_proverka(data->poisk.shet.ravno(),row[1],0,0) != 0)
 return(1);
 
if(data->poisk.debet.getdlinna() > 1)
 if(iceb_u_atof(data->poisk.debet.ravno()) != iceb_u_atof(row[2]))
   return(1);

if(data->poisk.kredit.getdlinna() > 1)
 if(iceb_u_atof(data->poisk.kredit.ravno()) != iceb_u_atof(row[3]))
   return(1);
   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void saldo_sh_rasp(class saldo_sh_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select gs,ns,deb,kre,ktoi,vrem from Saldo where kkk=0 order by gs asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"saldo%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список стартовых сальдо по счетам"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }


iceb_u_zagolov(gettext("Список стартовых сальдо по счетам"),0,0,0,0,0,0,organ,ff);

if(data->poisk.shet.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт"),data->poisk.shet.ravno());
if(data->poisk.god.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Год"),data->poisk.god.ravno());
if(data->poisk.debet.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Дебет"),data->poisk.debet.ravno());
if(data->poisk.kredit.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Кредит"),data->poisk.kredit.ravno());

fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Год |Счет  | Дебет    | Кредит   |Дата и время запис.| Кто записал\n"));


fprintf(ff,"\
------------------------------------------------------------------------\n");

double deb=0.,kre=0.;

iceb_u_str s5;
iceb_u_str s6;


cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(saldo_sh_prov_row(row,data) != 0)
    continue;

  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[5]));

  //Кто записал
  s6.new_plus(iceb_kszap(row[4],1,data->window));

  fprintf(ff,"%s %-*s %10.2f %10.2f %s %s\n",
  row[0],
  iceb_u_kolbait(6,row[1]),row[1],
  atof(row[2]),atof(row[3]),s5.ravno(),s6.ravno());

  deb+=atof(row[2]);
  kre+=atof(row[3]);

 }
fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,"%*s: %10.2f %10.2f\n",
iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),deb,kre);
fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);

iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
