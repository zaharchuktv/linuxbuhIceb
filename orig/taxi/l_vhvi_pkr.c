/*$Id: l_vhvi_pkr.c,v 1.11 2011-02-21 07:36:21 sasa Exp $*/
/*15.09.2007	15.09.2007	Белых А.И.	l_vhvi_pkr.c
Просмотр найденных записей входов/выходов по конкретной карточке клиента
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "i_rest.h"

enum
{
  FK5,
  FK10,
  KOL_F_KL
};

enum
{
 COL_VV,
 COL_DATA,
 COL_VREMZ,
 COL_PODR,
 COL_KODKART,
 COL_NAIMKL,
 NUM_COLUMNS
};


class  l_vhvi_data
 {
  public:
  

  //Реквизиты выбранной записи
  
  iceb_u_str data_vrem; //дата и время выбранной записи
  int podr;
  int         nomervsp; //Номер записи в списке

  GtkWidget *label_kolstr;
  GtkWidget *labelpoi;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надо стать
  int       kolzap;     //Количество записей
  int       voz;  
  
  iceb_u_str zapros;
  class iceb_u_str kodkart;  

  //Конструктор
  l_vhvi_data()
   {
    voz=0;
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    data_vrem.plus("");
   }      
 };

void  l_vhvi_knopka(GtkWidget *widget,class l_vhvi_data *data);
gboolean   l_vhvi_key_press(GtkWidget *widget,GdkEventKey *event,class l_vhvi_data *data);
void l_vhvi_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class l_vhvi_data *data);
void l_vhvi_vibor(GtkTreeSelection *selection,class l_vhvi_data *data);
void l_vhvi_add_columns(GtkTreeView *treeview);
void l_vhvi_create_list (class l_vhvi_data *data);
void	l_vhvi_pkras(class l_vhvi_data *data);
int l_vhvi_p(class l_vhvi_zap *datap,GtkWidget *wpredok);

int l_vhvi_v(char *data_vrem,int podr,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;

int  l_vhvi_pkr(const char *kodkart,GtkWidget *wpredok)
{
class  l_vhvi_data data;
GdkColor color;
char strsql[300];

data.kodkart.new_plus(kodkart);
 
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(strsql,"%s %s",name_system,gettext("Движение по карточке"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_vhvi_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Движение по карточке"));
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


sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_vhvi_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);
 
 sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_vhvi_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_vhvi_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

//printf("l_l_vhvi end\n");

return(data.voz);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_vhvi_knopka(GtkWidget *widget,class l_vhvi_data *data)
{
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_vhvi_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {

  case FK5:
    l_vhvi_pkras(data);
    return;  

    
  case FK10:
    printf("l_vhvi_knopka F10\n");
    gtk_widget_destroy(data->window);
    data->voz=0;
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_vhvi_key_press(GtkWidget *widget,GdkEventKey *event,class l_vhvi_data *data)
{
iceb_u_str repl;
printf("l_vhvi_key_press keyval=%d state=%d\n",
event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(TRUE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("l_vhvi_key_press-Нажата клавиша Shift\n");

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
void l_vhvi_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_vhvi_data *data)
{
printf("l_vhvi_v_row корректировка\n");
//l_l_vhvi_v(data);
data->snanomer=data->nomervsp;
//gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
}
/****************************/
/*Выбор строки*/
/**********************/

void l_vhvi_vibor(GtkTreeSelection *selection,class l_vhvi_data *data)
{
printf("klientr_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *dataz;
gchar *vrem;

gint  nomer;
gint podr;

char strok[1024];
short d,m,g;
short has,min,sek;


gtk_tree_model_get(model,&iter,COL_DATA,&dataz,COL_VREMZ,&vrem,COL_PODR,&podr,NUM_COLUMNS,&nomer,-1);

iceb_u_rsdat(&d,&m,&g,dataz,1);
iceb_u_rstime(&has,&min,&sek,vrem);

sprintf(strok,"%d-%d-%d %d:%d:%d",g,m,d,has,min,sek);

data->data_vrem.new_plus(strok);
data->snanomer=data->nomervsp=nomer;
data->podr=podr;

g_free(dataz);
g_free(vrem);

//printf("%s %d\n",data->kodv.ravno(),nomer);

}


/****************************************/
/*Распечатка списка          */
/****************************************/

void	l_vhvi_pkras(class l_vhvi_data *data)
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

if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
sprintf(imaf,"vv%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }
iceb_u_startfil(ff);

//fprintf(ff,"\x0F"); //Ужатый режим
iceb_u_zagolov(gettext("Движение по карточке"),0,0,0,0,0,0,"",ff);



fprintf(ff,"\
-----------------------------------------------------------------------------\n\
+/-|   Дата   | Время  | Подразделение |Код карточки|Наименование\n\
-----------------------------------------------------------------------------\n");
//  1234567890 12345678 123456789012345 123456789012                     
short d,m,g;
char vremq[20];
char naim_podr[50];
char fio[100];
char kod_naim_podr[100];
char vv[10];
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  memset(vv,'\0',sizeof(vv));
  if(row[3][0] == '1')
   vv[0]='+';
  if(row[3][0] == '2')
   vv[0]='-';
  if(atoi(row[3]) == -1)
   vv[0]='#';
      
  iceb_u_polen(row[0],strsql,sizeof(strsql),1,' ');
  iceb_u_rsdat(&d,&m,&g,strsql,2);
 
  iceb_u_polen(row[0],vremq,sizeof(vremq),2,' ');

  memset(naim_podr,'\0',sizeof(naim_podr));
  sprintf(strsql,"select naik from Restpod where kod=%s",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strncpy(naim_podr,row1[0],sizeof(naim_podr)-1);

  memset(fio,'\0',sizeof(fio));
  sprintf(strsql,"select fio from Taxiklient where kod='%s'",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strncpy(fio,row1[0],sizeof(fio)-1);

  sprintf(kod_naim_podr,"%s %s",row[2],naim_podr);

  fprintf(ff,"%3s %02d.%02d.%d %-8s %-15.15s %-12.12s %s\n",
  vv,d,m,g,vremq,kod_naim_podr,row[1],fio);


 }
fprintf(ff,"\
-----------------------------------------------------------------------------\n");

//fprintf(ff,"\x12"); //Нормальные буквы
iceb_podpis(ff,data->window);
fclose(ff);

iceb_u_spisok fil;
iceb_u_spisok nazv;

fil.plus(imaf);
nazv.plus(gettext("Движение по карточке"));

//printf("rasklient-fine\n");

iceb_rabfil(&fil,&nazv,"",0,data->window);


}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_vhvi_create_list (class l_vhvi_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[300];
int  kolstr=0;
SQL_str row;
SQL_str row1;

printf("l_vhvi_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_vhvi_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_vhvi_vibor),data);

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

sprintf(strsql,"select * from Restvv where kk='%s' order by dv desc",data->kodkart.ravno());

data->zapros.new_plus(strsql);

iceb_refresh();

//printf("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);
printf("kolstr=%d\n",kolstr);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
short d,m,g;
char has_min_sek[10];
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  iceb_u_polen(row[0],strsql,sizeof(strsql),1,' ');
  iceb_u_rsdat(&d,&m,&g,strsql,2);

  iceb_u_polen(row[0],has_min_sek,sizeof(has_min_sek),2,' ');

  //Дата
  sprintf(strsql,"%02d.%02d.%d",d,m,g);
  ss[COL_DATA].new_plus(strsql);

  //Время
  ss[COL_VREMZ].new_plus(has_min_sek);
  
  //Код карточки
  ss[COL_KODKART].new_plus(row[1]);

  //Подразделение

  ss[COL_PODR].new_plus(row[2]);
  sprintf(strsql,"select naik from Restpod where kod=%s",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    ss[COL_PODR].plus(" ");
    ss[COL_PODR].plus(iceb_u_toutf(row1[0]));

   }

  //Вход-выход
  if(row[3][0] == '1')
   ss[COL_VV].new_plus("+");
  if(row[3][0] == '2')
   ss[COL_VV].new_plus("-");
  if(atoi(row[3]) == -1)
   ss[COL_VV].new_plus("#");

  //Читаем наименование клиента
  ss[COL_NAIMKL].new_plus("");

  sprintf(strsql,"select fio from Taxiklient where kod='%s'",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     ss[COL_NAIMKL].new_plus(iceb_u_toutf(row1[0]));

  gtk_list_store_append (model, &iter);


  gtk_list_store_set (model, &iter,
  COL_DATA,ss[COL_DATA].ravno(),
  COL_VREMZ,ss[COL_VREMZ].ravno(),
  COL_PODR,ss[COL_PODR].ravno(),
  COL_KODKART,ss[COL_KODKART].ravno(),
  COL_VV,ss[COL_VV].ravno(),
  COL_NAIMKL,ss[COL_NAIMKL].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_vhvi_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

if(data->kolzap > 0)
 {
  GtkTreePath *path=gtk_tree_path_new_from_string("0");

  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

  gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
  //  gtk_tree_view_row_activated(GTK_TREE_VIEW (data->treeview),path,NULL);

  gtk_tree_path_free(path);
 }



iceb_u_str stroka;
iceb_u_str zagolov;

sprintf(strsql,"%s %s:%d",
gettext("Движение по подразделениям"),
gettext("Количество записей"),
data->kolzap);

zagolov.plus(strsql);


gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());


gtk_widget_show(data->label_kolstr);

iceb_u_str spis;
sprintf(strsql,"%s !!!",gettext("Поиск"));
spis.plus(strsql);
iceb_str_poisk(&spis,data->kodkart.ravno(),gettext("Код карточки"));

gtk_label_set_text(GTK_LABEL(data->labelpoi),spis.ravno_toutf());
    
gtk_widget_show(data->labelpoi); //Показываем


gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

printf("l_vhvi_create_list end\n");

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_vhvi_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

printf("l_vhvi_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"+/-", renderer,"text", COL_VV,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата"), renderer,"text", COL_DATA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Время"), renderer,"text", COL_VREMZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Подразделение"), renderer,"text", COL_PODR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Код карточки"), renderer,"text", COL_KODKART,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Фамилия"), renderer,"text", COL_NAIMKL,NULL);


printf("l_vhvi_add_columns end\n");

}
