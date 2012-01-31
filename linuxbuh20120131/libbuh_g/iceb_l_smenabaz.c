/*$Id: iceb_l_smenabaz.c,v 1.7 2011-02-21 07:36:07 sasa Exp $*/
/*14.05.2010	16.04.2010	Белых А.И.	iceb_l_smenabaz.c
Выбор базы 
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include  "iceb_libbuh.h"

enum
{
  COL_IMABAZ,
  COL_NAIM_KONTR,
  NUM_COLUMNS
};

enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK6,
  FK10,
  KOL_F_KL
};

class  iceb_l_smenabaz_data
 {
  public:

  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  short     metka_poi;
  //0-без поиска 1-с поиском
  int       metka_voz;  //0-выбрали 1-нет  

  class iceb_u_spisok imabaz;
  class iceb_u_spisok imakontr;
  class iceb_u_str imabazv; /*Выбранное имя базы из списка*/
  /****Реквизиты поиска*/
  class iceb_u_str imabaz_poi;
  class iceb_u_str imakontr_poi;
    
  //Конструктор
  iceb_l_smenabaz_data()
   {
    snanomer=0;
    metka_voz=kl_shift=metka_poi=0;
    window=treeview=NULL;
    imabazv.plus("");
    imabaz_poi.plus("");
    imakontr_poi.plus("");
   }      
 };

gboolean   iceb_l_smenabaz_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_smenabaz_data *data);
void iceb_l_smenabaz_vibor(GtkTreeSelection *selection,class iceb_l_smenabaz_data *data);
void iceb_l_smenabaz_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class iceb_l_smenabaz_data *data);
void  iceb_l_smenabaz_knopka(GtkWidget *widget,class iceb_l_smenabaz_data *data);
void iceb_l_smenabaz_add_columns (GtkTreeView *treeview);
int  iceb_l_smenabaz_prov_row(int nomer,class iceb_l_smenabaz_data *data);
void iceb_l_smenabaz_rasp(class iceb_l_smenabaz_data *data);
void iceb_l_smenabaz_create_list (class iceb_l_smenabaz_data *data);


int iceb_l_smenabaz_sb(int metka,class iceb_u_spisok *imabaz,class iceb_u_spisok *naimkontr,int *nom_baz,GtkWidget *wpredok);
void iceb_optimbazw(const char *imabaz,GtkWidget *wpredok);
int iceb_l_smenabaz_p(class iceb_u_str *imabaz,class iceb_u_str *imakontr,GtkWidget *wpredok);
int iceb_l_smenabaz_v(GtkWidget *wpredok);

extern SQL_baza  bd;
extern char      *name_system;
extern char *host;
extern char *parol;
extern char *imabaz;
extern class iceb_u_str kodirovka_iceb;


int   iceb_l_smenabaz(class iceb_u_str *imabazv,
int metka, /*0-показать список баз если база только одна 1-взять имя базы*/
GtkWidget *wpredok)
{

iceb_l_smenabaz_data data;
char bros[512];
GdkColor color;
int kodvoz=0;
printf("%s\n",__FUNCTION__);

if((kodvoz=iceb_l_smenabaz_sb(metka,&data.imabaz,&data.imakontr,&data.snanomer,wpredok)) <= 0)
 {
  if(kodvoz == 0)
   imabazv->new_plus(data.imabaz.ravno(0));
  return(kodvoz);
 }

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Список баз доступных оператору"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_l_smenabaz_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список баз доступных оператору"));
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

sprintf(bros,"F2 %s",gettext("Выбор"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(iceb_l_smenabaz_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Выбор отмеченной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"F3 %s",gettext("Хост"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(iceb_l_smenabaz_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Сменить хост на базу"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(iceb_l_smenabaz_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(iceb_l_smenabaz_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Оптимизировать"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(iceb_l_smenabaz_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Оптимизировать базу данных"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(iceb_l_smenabaz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK2]);

iceb_l_smenabaz_create_list(&data);

gtk_widget_show(data.window);
//  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();

if(data.metka_voz == 0)
 {
  imabazv->new_plus(data.imabazv.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void iceb_l_smenabaz_create_list (class iceb_l_smenabaz_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
char strsql[1024];
int  kolstr=0;

class iceb_u_str imaendbaz("");
//GdkColor color;

data->kl_shift=0; //0-отжата 1-нажата  

iceb_u_poldan("NAMEBAZ",&imaendbaz,".iceb.dat");

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(iceb_l_smenabaz_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(iceb_l_smenabaz_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);



model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);



iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
float kolstr1=0. ;
kolstr=data->imabaz.kolih();
for(int nom=0; nom < kolstr; nom++)
 {
//  printf("%s %s\n",data->imabaz.ravno(nom),data->imakontr.ravno(nom));
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  if(iceb_l_smenabaz_prov_row(nom,data) != 0)
    continue;



  //Имя базы
  ss[COL_IMABAZ].new_plus(iceb_u_toutf(data->imabaz.ravno(nom)));
  
  //Наименование контрагента
  ss[COL_NAIM_KONTR].new_plus(iceb_u_toutf(data->imakontr.ravno(nom)));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_IMABAZ,ss[COL_IMABAZ].ravno(),
  COL_NAIM_KONTR,ss[COL_NAIM_KONTR].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

iceb_l_smenabaz_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список баз доступных для оператора"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
sprintf(strsql,"%s:%s",gettext("Хост на базу"),host);
zagolov.ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

if(data->metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->imabaz_poi.ravno(),gettext("База"));
  iceb_str_poisk(&zagolov,data->imakontr_poi.ravno(),gettext("Наименование контрагента"));
  

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

void iceb_l_smenabaz_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("База"),renderer,"text",COL_IMABAZ,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_IMABAZ);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Контрагент"),renderer,"text",COL_NAIM_KONTR,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NAIM_KONTR);
gtk_tree_view_append_column (treeview, column);


}

/****************************/
/*Выбор строки*/
/**********************/

void iceb_l_smenabaz_vibor(GtkTreeSelection *selection,class iceb_l_smenabaz_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *imabaz;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_IMABAZ,&imabaz,NUM_COLUMNS,&nomer,-1);

data->imabazv.new_plus(iceb_u_fromutf(imabaz));
data->snanomer=nomer;

g_free(imabaz);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_l_smenabaz_knopka(GtkWidget *widget,class iceb_l_smenabaz_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
struct  passwd  *ktor; /*Кто работает*/

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("iceb_l_smenabaz_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->metka_voz=0;
    gtk_widget_destroy(data->window);
 
    return;  


  case FK3:
    if(iceb_l_smenabaz_v(data->window) != 0)
     return;

    ktor=getpwuid(getuid());

    sql_closebaz(&bd);
    if(sql_openbaz(&bd,"",host,ktor->pw_name,parol) != 0)
      iceb_eropbaz("",ktor->pw_uid,ktor->pw_name,0);

    iceb_sql_readkey("select VERSION()",&row,&cur,NULL);
    if(atof(row[0]) >= 4.1)
     {
      if(kodirovka_iceb.getdlinna() <= 1)
        sprintf(strsql,"SET NAMES koi8u");
      else
        sprintf(strsql,"SET NAMES %s",kodirovka_iceb.ravno_udsimv("-"));

      iceb_sql_zapis(strsql,1,0,data->window);
     }

    iceb_l_smenabaz_sb(0,&data->imabaz,&data->imakontr,&data->snanomer,data->window);
    iceb_l_smenabaz_create_list(data);
//    sql_closebaz(&bd);
    return;  
  


  case FK4:
    if(iceb_l_smenabaz_p(&data->imabaz_poi,&data->imakontr_poi,data->window) == 0)
     data->metka_poi=1;
    else
     data->metka_poi=0;

    iceb_l_smenabaz_create_list(data);

    return;  

  case FK5:
    iceb_l_smenabaz_rasp(data);
    return;  

  case FK6:
    iceb_optimbazw(data->imabazv.ravno(),data->window);
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

gboolean   iceb_l_smenabaz_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_smenabaz_data *data)
{

switch(event->keyval)
 {

  case GDK_F2:

    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

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
void iceb_l_smenabaz_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class iceb_l_smenabaz_data *data)
{
//printf("iceb_l_smenabaz_v_row\n");
//printf("iceb_l_smenabaz_v_row корректировка\n");

data->metka_voz=0;

gtk_widget_destroy(data->window);


}

/****************************/
/*Проверка записей          */
/*****************************/

int  iceb_l_smenabaz_prov_row(int nomer,class iceb_l_smenabaz_data *data)
{
if(data->metka_poi == 0)
 return(0);

if(iceb_u_proverka(data->imabaz_poi.ravno(),data->imabaz.ravno(nomer),4,0) != 0)
 return(1);


if(iceb_u_proverka(data->imakontr_poi.ravno(),data->imakontr.ravno(nomer),4,0) != 0)
 return(1);

return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void iceb_l_smenabaz_rasp(class iceb_l_smenabaz_data *data)
{
char strsql[312];
FILE *ff;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=data->imabaz.kolih();
int metka_imabaz=0;

sprintf(strsql,"spb%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список баз доступных оператору"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_u_zagolov(gettext("Список баз доступных оператору"),0,0,0,0,0,0,"",ff);

if(data->metka_poi == 1)
 {
  if(data->imabaz_poi.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("База"),data->imabaz_poi.ravno());
  if(data->imakontr_poi.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Контрагент"),data->imakontr_poi.ravno());
 }

fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,"\
   База   |       Контрагент\n");

fprintf(ff,"\
------------------------------------------------------------------------\n");



for(int nom=0; nom < kolstr; nom++)
 {
  if(iceb_l_smenabaz_prov_row(nom,data) != 0)
    continue;


  fprintf(ff,"%-10s %ss\n",data->imabaz.ravno(nom),data->imakontr.ravno(nom));

  

 }
fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);

iceb_ustpeh(imaf.ravno(0),3,data->window);

if(imabaz == NULL)
 {
  metka_imabaz=1;
  imabaz=new char[data->imabazv.getdlinna()];
  strcpy(imabaz,data->imabazv.ravno());
 }

iceb_rabfil(&imaf,&naimot,"",0,data->window);
if(metka_imabaz == 1)
 {
  delete [] imabaz;
  imabaz=NULL;
 }
}
/************************************/
/*Получить список баз*/
/*******************************/
int iceb_l_smenabaz_sb(int metka,/*0-показать список баз если база только одна 1-взять имя базы*/
class iceb_u_spisok *imabaz,class iceb_u_spisok *naimkontr,int *nom_baz,GtkWidget *wpredok)
{
char strsql[312];
class SQLCURSOR cur,cur1;
SQL_str row,row1;
class iceb_u_str imaendbaz("");
int kolstr;

naimkontr->free_class();
imabaz->free_class();

iceb_u_poldan("NAMEBAZ",&imaendbaz,".iceb.dat");

if((kolstr=cur.make_cursor(&bd,"SHOW DATABASES")) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной базы!"),wpredok);
  return(1);
 }
class iceb_u_spisok baza;
class iceb_u_spisok naimb;
*nom_baz=0;
int kodv=0;
int kol_db=0; /*количество доступных баз*/
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select naikon from %s.Kontragent where kodkon='00'",row[0]);
  if((kodv=sql_readkey(&bd,strsql,&row1,&cur1)) < 0)
   continue;     

  if(kodv == 1)
    naimkontr->plus(row1[0]);
  else
    naimkontr->plus("");
  imabaz->plus(row[0]);

  if(iceb_u_proverka(imaendbaz.ravno(),row[0],0,1) == 0) 
   *nom_baz=kol_db;
  kol_db++;
 }

if(kol_db == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной базы, к которой вам разрешён доступ!"),wpredok);
  return(-1);
 }

if(kol_db == 1 && metka == 1)
 {

  return(0);
 }

return(kol_db);

}

