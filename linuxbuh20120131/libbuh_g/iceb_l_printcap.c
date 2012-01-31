/*$Id: iceb_l_printcap.c,v 1.16 2011-02-21 07:36:07 sasa Exp $*/
/*19.03.2007	15.04.2004	Белых А.И.	iceb_l_printcap.c
Просмотр списка принтеров
*/
#include 	<stdlib.h>
#include 	<unistd.h>
#include 	<errno.h>
#include 	<dirent.h>
#include        <sys/stat.h>
#include       "iceb_libbuh.h"

enum
 {
  VIBOR,
  VIHOD,
  KOLSTRM
 };


class  printcap_data
 {
  public:

  iceb_u_str imaprv;

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
  
  iceb_u_spisok imapr;
  iceb_u_spisok naimpr;
      
  //Конструктор
  printcap_data()
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
 COL_KOL1,
 COL_KOL2,
 NUM_COLUMNS
};

gboolean   printcap_key_press(GtkWidget *widget,GdkEventKey *event,class printcap_data *data);
void printcap_vibor(GtkTreeSelection *selection,class printcap_data *data);
void printcap_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class printcap_data *data);
void  printcap_knopka(GtkWidget *widget,class printcap_data *data);
void printcap_add_columns (GtkTreeView *treeview);
void printcap_create_list (class printcap_data *data);

int rprintcap(iceb_u_spisok *imapr,iceb_u_spisok *naimpr);

extern char *organ;

void   iceb_l_printcap(GtkWidget *wpredok)
{

printcap_data data;
char bros[512];
//GdkColor color;

if(rprintcap(&data.imapr,&data.naimpr) != 0)
  return;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Выбор принтера."));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(printcap_key_press),&data);
//gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new(gettext("Выбор принтера."));
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
gtk_widget_show (data.sw);

//Кнопки
GtkTooltips *tooltips[KOLSTRM];

sprintf(bros,"F2 %s",gettext("Выбор"));
data.knopka[VIBOR]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[VIBOR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[VIBOR]), "clicked",GTK_SIGNAL_FUNC(printcap_knopka),&data);
tooltips[VIBOR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[VIBOR],data.knopka[VIBOR],gettext("Выбор отмеченной записи."),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[VIBOR]),(gpointer)VIBOR);
gtk_widget_show(data.knopka[VIBOR]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[VIHOD]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[VIHOD],TRUE,TRUE, 0);
tooltips[VIHOD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[VIHOD],data.knopka[VIHOD],gettext("Завершение работы в этом окне."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[VIHOD]), "clicked",GTK_SIGNAL_FUNC(printcap_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[VIHOD]),(gpointer)VIHOD);
gtk_widget_show(data.knopka[VIHOD]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[VIHOD]);

printcap_create_list (&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void printcap_create_list (class printcap_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
char bros[512];

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(printcap_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(printcap_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

//iceb_u_str rekv[NUM_COLUMNS];

data->kolzap=0;

printf("iceb_l_printcap-Вставляем в список для просмотра на экране.\n");
int kolstrok=data->imapr.kolih();
data->kolzap=0;
for(int i=0; i < kolstrok ; i++)
 {
  printf("имя-%s наименование-%s\n",data->imapr.ravno(i),data->naimpr.ravno(i));
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOL1,data->imapr.ravno_toutf(i),
  COL_KOL2,data->naimpr.ravno_toutf(i),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }


gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

printcap_add_columns(GTK_TREE_VIEW (data->treeview));

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[VIBOR]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[VIBOR]),TRUE);//Доступна
 }

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

gtk_widget_show (data->treeview);


iceb_u_str zagolov;
zagolov.plus(gettext("Выбор принтера."));

sprintf(bros," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(bros);

char *print=getenv("PRINTER");
char prints[100];
if(print == NULL)
 strcpy(prints,gettext("По умалчанию"));
else
 strcpy(prints,print);
 
sprintf(bros," %s:%s",gettext("Принтер"),prints);
zagolov.ps_plus(bros);


gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

}

/*****************/
/*Создаем колонки*/
/*****************/

void printcap_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

//printf("printcap_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
//  g_object_set(G_OBJECT(renderer),"foreground","red",NULL);

//g_object_set(G_OBJECT(renderer),"background","black",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Принтер"), renderer,"text", COL_KOL1,NULL);

renderer = gtk_cell_renderer_text_new ();
//g_object_set(G_OBJECT(renderer),"foreground","white",NULL);
//g_object_set(G_OBJECT(renderer),"background","black",NULL);


gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Коментарий"), renderer,"text", COL_KOL2,NULL);

renderer = gtk_cell_renderer_text_new ();

//printf("printcap_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void printcap_vibor(GtkTreeSelection *selection,class printcap_data *data)
{
//printf("printcap_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *imaf;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_KOL1,&imaf,NUM_COLUMNS,&nomer,-1);

data->imaprv.new_plus(iceb_u_fromutf(imaf));
data->snanomer=nomer;

g_free(imaf);

//printf("%s %d\n",data->imaprv.ravno(),nomer);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   printcap_key_press(GtkWidget *widget,GdkEventKey *event,class printcap_data *data)
{
iceb_u_str repl;
printf("saldo_sh_key_press keyval=%d state=%d\n",
event->keyval,event->state);

switch(event->keyval)
 {
  case GDK_F2:
    if(data->kolzap == 0)
      return(TRUE);
      
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[VIBOR]),"clicked");
    return(TRUE);
  
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
void  printcap_knopka(GtkWidget *widget,class printcap_data *data)
{
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("printcap_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {

  case VIBOR:
    if(setenv("PRINTER",data->imaprv.ravno(),1) != 0)
      printf("iceb_printcap_knop_press-Ошибка переустановки переменной !\n");
    gtk_widget_destroy(data->window);
    return;  

  case VIHOD:
    gtk_widget_destroy(data->window);
    return;  

    
 }
}

/****************************/
/*Выбор строки*/
/**********************/
void printcap_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class printcap_data *data)
{
printf("printcap_v_row\n");
printf("printcap_v_row корректировка\n");

gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[VIBOR]),"clicked");
}

/****************************/
/*чтение файла с настройками*/
/****************************/
int rprintcap(iceb_u_spisok *imapr,iceb_u_spisok *naimpr)
{
FILE *ff;
char imafil[30];
char stroka[1000];
errno=0;

strcpy(imafil,"/etc/printcap");

if((ff = fopen(imafil,"r")) == NULL)
 if(errno != ENOENT)
  {
   iceb_u_str soob;
   sprintf(stroka,"%s %d !",gettext("Ошибка открытия файла"),errno);
   soob.plus_ps(stroka);
   soob.plus(imafil);
   iceb_menu_soob(&soob,NULL);
   return(1);
  }

if(errno == ENOENT) /*Файл не существует*/
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("Не найдено файл"));
  SOOB.plus(": ");
  SOOB.plus(imafil);
  iceb_menu_soob(&SOOB,NULL);
  return(1);
 }
char bros[512];

const short DLINNA=300;
gchar *strokalist[2];
strokalist[0]=new gchar[DLINNA];
strokalist[1]=new gchar[DLINNA];
printf("iceb_l_printcap-Читаем файл %s\n",imafil);

while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  if(stroka[0] == '#' || stroka[0] == ' ' || stroka[0] == '\t' ||  stroka[0] == '\n')
   continue;
  printf("Строка из файла-%s",stroka);
  if(iceb_u_polen(stroka,bros,sizeof(bros),1,':') == 0)
   {
    memset(strokalist[0],'\0',DLINNA);
    memset(strokalist[1],'\0',DLINNA);
    if(iceb_u_polen(bros,strokalist[0],DLINNA,1,'|') == 0)
      iceb_u_polen(bros,strokalist[1],DLINNA,2,'|');
    else
      strncpy(strokalist[0],bros,DLINNA-1);

    if(strokalist[1][0] == '.')
      continue;
    printf("Взяли из строки имя-%s наименование-%s\n",strokalist[0],strokalist[1]);
    imapr->plus(strokalist[0]);
    naimpr->plus(strokalist[1]);
   }
 }

fclose(ff);
return(0);

}
