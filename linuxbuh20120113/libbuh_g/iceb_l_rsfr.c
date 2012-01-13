/*$Id: iceb_l_rsfr.c,v 1.23 2011-02-21 07:36:07 sasa Exp $*/
/*22.05.2008	08.04.2004	Белых А.И.	iceb_l_rsfr.c
Работа с файлами распечаток
*/
#include 	<stdlib.h>
#include 	<unistd.h>
#include 	<errno.h>
#include 	<dirent.h>
#include        <sys/stat.h>
#include       "iceb_libbuh.h"

#define   LST "lst"

enum
 {
  REDAKTOR,
  PEHAT,
  DELETE,
  PONP,
  OONP,
  VIHOD,
  KOLSTRM
 };


class  rsfr_l_data
 {
  public:

  iceb_u_str imafv;

  GtkWidget *label_kolstr;
//  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *vwindow;
  GtkWidget *pwindow;
  GtkWidget *knopka[KOLSTRM];
  short     kl_shift; //0-отжата 1-нажата  
  short     metkazapisi; //0-новая запись 1-корректировка
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  short     metka_poi;  //0-без поиска 1-с поиском
  
  class SYSTEM_NAST sysnast;

  //Конструктор
  rsfr_l_data()
   {
    snanomer=0;
    kl_shift=metkazapisi=metka_poi=0;
    vwindow=pwindow=window=treeview=NULL;

    
   }      
 };

extern SQL_baza	bd;
extern char *name_system;


enum
{
  COL_IMAF,
  COL_RAZMER,
  COL_DATAVR,
  NUM_COLUMNS
};

gboolean   rsfr_l_key_press(GtkWidget *widget,GdkEventKey *event,class rsfr_l_data *data);
void rsfr_l_vibor(GtkTreeSelection *selection,class rsfr_l_data *data);
void rsfr_l_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class rsfr_l_data *data);
void  rsfr_l_knopka(GtkWidget *widget,class rsfr_l_data *data);
void rsfr_l_add_columns (GtkTreeView *treeview);
int  rsfr_l_prov_row(SQL_str row,class rsfr_l_data *data);
void rsfr_l_create_list (class rsfr_l_data *data);
//void rsfr_l_rasp(class rsfr_l_data *data);

extern char *organ;

void   iceb_l_rsfr(GtkWidget *wpredok)
{
rsfr_l_data data;
char bros[512];
//GdkColor color;

iceb_rsn(&data.sysnast,wpredok);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Файлы распечаток."));

gtk_window_set_title (GTK_WINDOW (data.window),bros);
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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rsfr_l_key_press),&data);
//gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new(gettext("Файлы распечаток."));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

/***************
data.label_poisk=gtk_label_new ("");
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);
**************/
data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
//GtkTooltips *tooltips[KOLSTRM];

sprintf(bros,"%s",gettext("Просмотр и редактирование файла."));
data.knopka[REDAKTOR]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[REDAKTOR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[REDAKTOR]), "clicked",GTK_SIGNAL_FUNC(rsfr_l_knopka),&data);
//tooltips[REDAKTOR]=gtk_tooltips_new();
//gtk_tooltips_set_tip(tooltips[REDAKTOR],data.knopka[REDAKTOR],gettext("Ввод новой записи."),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[REDAKTOR]),(gpointer)REDAKTOR);
gtk_widget_show(data.knopka[REDAKTOR]);

sprintf(bros,"%s",gettext("Печать на системном принтере."));
data.knopka[PEHAT]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[PEHAT],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[PEHAT]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[PEHAT]), "clicked",GTK_SIGNAL_FUNC(rsfr_l_knopka),&data);
//tooltips[PEHAT]=gtk_tooltips_new();
//gtk_tooltips_set_tip(tooltips[PEHAT],data.knopka[PEHAT],gettext("Корректировка выбранной запси."),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PEHAT]),(gpointer)PEHAT);
gtk_widget_show(data.knopka[PEHAT]);


sprintf(bros,"%s",gettext("Удалить файл."));
data.knopka[DELETE]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[DELETE],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[DELETE]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[DELETE]), "clicked",GTK_SIGNAL_FUNC(rsfr_l_knopka),&data);
//tooltips[DELETE]=gtk_tooltips_new();
//gtk_tooltips_set_tip(tooltips[DELETE],data.knopka[DELETE],gettext("Удаление выбранной запси."),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[DELETE]),(gpointer)DELETE);
gtk_widget_show(data.knopka[DELETE]);

sprintf(bros,"%s",gettext("Просмотр очереди на печать."));
data.knopka[PONP]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[PONP]), "clicked",GTK_SIGNAL_FUNC(rsfr_l_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[PONP],TRUE,TRUE, 0);
//tooltips[PONP]=gtk_tooltips_new();
//gtk_tooltips_set_tip(tooltips[PONP],data.knopka[PONP],gettext("Поиск нужных записей."),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PONP]),(gpointer)PONP);
gtk_widget_show(data.knopka[PONP]);

sprintf(bros,"%s",gettext("Очистить очередь на печать."));
data.knopka[OONP]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[OONP]), "clicked",GTK_SIGNAL_FUNC(rsfr_l_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[OONP],TRUE,TRUE, 0);
//tooltips[OONP]=gtk_tooltips_new();
//gtk_tooltips_set_tip(tooltips[OONP],data.knopka[OONP],gettext("Поиск нужных записей."),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[OONP]),(gpointer)OONP);
gtk_widget_show(data.knopka[OONP]);

sprintf(bros,"%s",gettext("Выход"));
data.knopka[VIHOD]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[VIHOD],TRUE,TRUE, 0);
//tooltips[VIHOD]=gtk_tooltips_new();
//gtk_tooltips_set_tip(tooltips[VIHOD],data.knopka[VIHOD],gettext("Завершение работы в этом окне."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[VIHOD]), "clicked",GTK_SIGNAL_FUNC(rsfr_l_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[VIHOD]),(gpointer)VIHOD);
gtk_widget_show(data.knopka[VIHOD]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[VIHOD]);

rsfr_l_create_list (&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);


gtk_main();

//printf("l_rsfr_l end\n");


}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void rsfr_l_create_list (class rsfr_l_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
DIR             *dirp;
struct dirent *dppp;
struct stat work;

if((dirp = opendir(".")) == NULL) 
 {
  iceb_er_op_fil(".","Не могу открыть каталог",errno,data->window);
  return;
 }

if((dppp = readdir(dirp)) == NULL) 
 {
  iceb_er_op_fil(".","Не могу прочесть каталог",errno,data->window);
  return;
 }



gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(rsfr_l_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(rsfr_l_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

iceb_u_str rekv[NUM_COLUMNS];

data->kolzap=0;
int kolpol;
char rash[30];
char bros[512];

while( dppp != NULL )
 {

  if((kolpol=iceb_u_pole2(dppp->d_name,'.')) != 0)
   {     
    iceb_u_polen(dppp->d_name,rash,sizeof(rash),kolpol,'.');
    if(iceb_u_SRAV(LST,rash,0) == 0)
     {      
      if(stat(dppp->d_name,&work))
        continue;

/**************
      printf("%-12.12s %6ld %02d:%02d %02d.%02d.%d\n",
      dppp->d_name,
      work.st_size,bf->tm_hour,
      bf->tm_min,bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
******************/

      rekv[0].new_plus(dppp->d_name);
      
      sprintf(bros,"%d",(int)work.st_size);
      rekv[1].new_plus(bros);

      //Дата и время записи
      rekv[2].new_plus(iceb_u_vremzap(work.st_mtime));
      
      gtk_list_store_append (model, &iter);

      gtk_list_store_set (model, &iter,
      COL_IMAF,rekv[0].ravno_toutf(),
      COL_RAZMER,rekv[1].ravno(),
      COL_DATAVR,rekv[2].ravno(),
      NUM_COLUMNS,data->kolzap,
      -1);

      data->kolzap++;
     }
   }
  dppp = readdir(dirp);
 }

closedir(dirp);
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

rsfr_l_add_columns(GTK_TREE_VIEW (data->treeview));

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[REDAKTOR]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[PEHAT]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[DELETE]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[REDAKTOR]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[PEHAT]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[DELETE]),TRUE);//Доступна
 }

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);


iceb_u_str zagolov;
zagolov.plus(gettext("Файлы распечаток."));

sprintf(bros," %s:%d",gettext("Количество файлов"),data->kolzap);
zagolov.plus(bros);

sprintf(bros,"%s: %s",gettext("Программа печати"),data->sysnast.prog_peh_sys.ravno());
zagolov.ps_plus(bros);

sprintf(bros,"%s: %s",gettext("Программа редактирования"),data->sysnast.redaktor.ravno());
zagolov.ps_plus(bros);

sprintf(bros,"%s: %s",gettext("Программа снятия с очереди на печать"),
data->sysnast.clear_peh.ravno());
zagolov.ps_plus(bros);

sprintf(bros,"%s: %s",gettext("Программа просмотра очереди на печать"),
data->sysnast.prosop.ravno());
zagolov.ps_plus(bros);
/***********
char strsql[512];

char *name_p=getenv("PRINTER");
if(name_p == NULL)
 strcpy(strsql,gettext("По умолчанию"));
else
 strcpy(strsql,name_p);

sprintf(bros,"%s: %s",gettext("Принтер"),strsql);
zagolov.ps_plus(bros);
****************/
gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

}

/*****************/
/*Создаем колонки*/
/*****************/

void rsfr_l_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

printf("rsfr_l_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
//  g_object_set(G_OBJECT(renderer),"foreground","red",NULL);

//g_object_set(G_OBJECT(renderer),"background","black",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Имя файла"), renderer,"text", COL_IMAF,NULL);

renderer = gtk_cell_renderer_text_new ();
//g_object_set(G_OBJECT(renderer),"foreground","white",NULL);
//g_object_set(G_OBJECT(renderer),"background","black",NULL);


gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Размер"), renderer,"text", COL_RAZMER,NULL);

renderer = gtk_cell_renderer_text_new ();

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATAVR,NULL);

printf("rsfr_l_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void rsfr_l_vibor(GtkTreeSelection *selection,class rsfr_l_data *data)
{
//printf("rsfr_l_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *imaf;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_IMAF,&imaf,NUM_COLUMNS,&nomer,-1);

data->imafv.new_plus(iceb_u_fromutf(imaf));
data->snanomer=nomer;

g_free(imaf);

//printf("%s %d\n",data->imafv.ravno(),data->nomervsp);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rsfr_l_key_press(GtkWidget *widget,GdkEventKey *event,class rsfr_l_data *data)
{
iceb_u_str repl;
printf("saldo_sh_key_press keyval=%d state=%d\n",
event->keyval,event->state);

switch(event->keyval)
 {
  
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[VIHOD]),"clicked");
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

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rsfr_l_knopka(GtkWidget *widget,class rsfr_l_data *data)
{
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("rsfr_l_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {

  case REDAKTOR:

    repl.new_plus(data->sysnast.redaktor.ravno());
    repl.plus(" ");
    repl.plus(data->imafv.ravno());

    printf("rsfr_l_knopka-%s\n",repl.ravno());

    system(repl.ravno());    

    return;  

  case PEHAT:
    if(iceb_menu_danet(gettext("Распечатать? Вы уверены?"),2,data->window) == 2)
      return;
    iceb_print(data->imafv.ravno(),1,"","",data->window);
    return;  

  case DELETE:
    if(iceb_menu_danet(gettext("Удалить ? Вы уверены ?"),2,data->window) == 2)
      return;

    unlink(data->imafv.ravno());

    rsfr_l_create_list(data);

    return;  

  case PONP:
    iceb_ponp(data->sysnast.prosop.ravno(),data->sysnast.clear_peh.ravno(),data->window);
    return;  
      
    unlink(data->imafv.ravno());
    rsfr_l_create_list(data);
    return;  

  case OONP:
    iceb_uionp(data->window);
    return;  

  case VIHOD:
    gtk_widget_destroy(data->window);
    return;  

    
 }
}

/****************************/
/*Выбор строки*/
/**********************/
void rsfr_l_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class rsfr_l_data *data)
{
printf("rsfr_l_v_row\n");
printf("rsfr_l_v_row корректировка\n");

gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[REDAKTOR]),"clicked");
}
