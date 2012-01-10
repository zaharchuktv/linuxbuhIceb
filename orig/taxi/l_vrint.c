/*$Id: l_vrint.c,v 1.21 2011-02-21 07:36:21 sasa Exp $*/
/*22.01.2007	30.11.2005	Белых А.И.	l_vrint.c
Работа со списком временных интервалов для подразделений
*/
#include <errno.h>
#include <unistd.h>
#include "i_rest.h"
#include "l_vrint.h"
enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK6,
  FK10,
  SFK2,
  KOL_F_KL
};

enum
{
 COL_DATA,
 COL_VREMZ,
 COL_CENA,
 COL_CENA_F_W,
 COL_GRUPA, 
 COL_KOMENT,
 COL_VREM,
 COL_KTO,
 NUM_COLUMNS
};


class  l_vrint_data
 {
  public:
  
  class l_vrint_zap poi;

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
  short     metkapoi;    //0-без поиска 1-с поиском
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       voz;  
  
  iceb_u_str zapros;
  int podr_sp;  //Подразделение для которого смотрим список записей  
  iceb_u_str naim_podr;
  

  //Конструктор
  l_vrint_data()
   {
    metkapoi=0; //0-новая запись 1-корректировка
    voz=0;
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    podr_sp=0;
    data_vrem.plus("");
    naim_podr.plus("");
   }      
 };

int   l_vrint_prov_row(SQL_str row,class l_vrint_data *data);
void  l_vrint_knopka(GtkWidget *widget,class l_vrint_data *data);
gboolean   l_vrint_key_press(GtkWidget *widget,GdkEventKey *event,class l_vrint_data *data);
void l_vrint_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class l_vrint_data *data);
void l_vrint_udzap(class l_vrint_data *data);
void l_vrint_vibor(GtkTreeSelection *selection,class l_vrint_data *data);
void l_vrint_add_columns(GtkTreeView *treeview);
void l_vrint_create_list (class l_vrint_data *data);
void	l_vrint_ras(class l_vrint_data *data);
int l_vrint_p(class l_vrint_zap *datap,GtkWidget *wpredok);

int l_vrint_v(const char *data_vrem,int podr,GtkWidget *wpredok);
int l_vrint_k(class iceb_u_str *dataz,class iceb_u_str *datan,GtkWidget *wpredok);
int l_vrint_kz(int podr,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;

int  l_vrint(int podr,GtkWidget *wpredok)
{
class  l_vrint_data data;
GdkColor color;
char strsql[300];
SQL_str row;
SQLCURSOR cur;

data.podr_sp=podr;
//читаем наименование подразделения
sprintf(strsql,"select naik from Restpod where kod=%d",podr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.naim_podr.new_plus(row[0]);
 
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(strsql,"%s %s",name_system,gettext("Список временных интервалов"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_vrint_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список временных интервалов"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_vrint_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[SFK2]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_vrint_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK3]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_vrint_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_vrint_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);


sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_vrint_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F6 %s",gettext("Копировать"));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(l_vrint_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Копировать записи по выбранной дате на другую дату"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);
 
 sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_vrint_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_vrint_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

printf("l_l_vrint end\n");

return(data.voz);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_vrint_knopka(GtkWidget *widget,class l_vrint_data *data)
{
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_vrint_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {
  case FK2:
    l_vrint_v("",data->podr_sp,data->window);
    l_vrint_create_list(data);
    
    return;  

  case SFK2:

    l_vrint_v(data->data_vrem.ravno(),data->podr_sp,data->window);
    l_vrint_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    l_vrint_udzap(data);
    l_vrint_create_list(data);
    return;  

  case FK4:
    data->metkapoi=l_vrint_p(&data->poi,data->window);
    l_vrint_create_list(data);
    return;  

  case FK5:
    l_vrint_ras(data);
    return;  

  case FK6:
    if(l_vrint_kz(data->podr_sp,data->window) == 0)
      l_vrint_create_list(data);
    
    return;  


    
  case FK10:
    printf("l_vrint_knopka F10\n");
    gtk_widget_destroy(data->window);
    data->voz=0;
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_vrint_key_press(GtkWidget *widget,GdkEventKey *event,class l_vrint_data *data)
{
iceb_u_str repl;
//printf("l_vrint_key_press keyval=%d state=%d\n",
//event->keyval,event->state);

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

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(TRUE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("l_vrint_key_press-Нажата клавиша Shift\n");

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
void l_vrint_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_vrint_data *data)
{
printf("l_vrint_v_row корректировка\n");
//l_l_vrint_v(data);
data->snanomer=data->nomervsp;
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
}
/****************************/
/*Выбор строки*/
/**********************/

void l_vrint_vibor(GtkTreeSelection *selection,class l_vrint_data *data)
{
//printf("klientr_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *dataz;
gchar *vrem;
gint  nomer;
char strok[1024];
short d,m,g;
short has,min,sek;


gtk_tree_model_get(model,&iter,COL_DATA,&dataz,COL_VREMZ,&vrem,NUM_COLUMNS,&nomer,-1);

iceb_u_rsdat(&d,&m,&g,dataz,1);
iceb_u_rstime(&has,&min,&sek,vrem);

sprintf(strok,"%d-%d-%d %d:%d:%d",g,m,d,has,min,sek);

data->data_vrem.new_plus(strok);
data->snanomer=data->nomervsp=nomer;

g_free(dataz);
g_free(vrem);

//printf("%s %d\n",data->kodv.ravno(),nomer);

}


/****************************/
/*Удалить запись*/
/****************************/

void l_vrint_udzap(class l_vrint_data *data)
{

char strsql[300];

iceb_u_str repl;

repl.plus(gettext("Удалить запись ? Вы уверены ?"));
if(iceb_menu_danet(&repl,2,data->window) == 2)
  return;


sprintf(strsql,"delete from Restvi where dv='%s'",data->data_vrem.ravno());
printf("l_vrint_v_udzap %s\n",strsql);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
  return;

}
/****************************************/
/*Распечатка списка          */
/****************************************/

void	l_vrint_ras(class l_vrint_data *data)
{
char		strsql[300];
SQL_str		row;
int		kolstr=0;
FILE		*ff;
char		imaf[40];
SQLCURSOR cur;
//printf("rasklient\n");

if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
sprintf(imaf,"vi%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }
iceb_u_startfil(ff);
//fprintf(ff,"\x0F"); //Ужатый режим
iceb_u_zagolov(gettext("Список временных интервалов"),0,0,0,0,0,0,"",ff);
fprintf(ff,"\
-----------------------------------------------------------------------------\n\
   Дата   | Время  | Цена |Группы с бесплатным входом|Коментарий\n\
-----------------------------------------------------------------------------\n");

short d,m,g;
char vremq[20];
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
   if(l_vrint_prov_row(row,data) != 0)
      continue;
  iceb_u_polen(row[0],strsql,sizeof(strsql),1,' ');
  iceb_u_rsdat(&d,&m,&g,strsql,2);
 
  iceb_u_polen(row[0],vremq,sizeof(vremq),2,' ');

  fprintf(ff,"%02d.%02d.%d %-8s %-6s %-26.26s %s\n",
  d,m,g,vremq,row[2],row[3],row[4]);

  if(strlen(row[2]) > 16)
   fprintf(ff,"%23s %s"," ",&row[0][16]);

 }
fprintf(ff,"\
-----------------------------------------------------------------------------\n");

//fprintf(ff,"\x12"); //Нормальные буквы
iceb_podpis(ff,data->window);
fclose(ff);

iceb_u_spisok fil;
iceb_u_spisok nazv;

fil.plus(imaf);
nazv.plus(gettext("Список временных интервалов"));

//printf("rasklient-fine\n");

iceb_rabfil(&fil,&nazv,"",0,data->window);


}
/**********************************/
/*Проверка записи на условия поиска*/
/************************************/

int   l_vrint_prov_row(SQL_str row,class l_vrint_data *data)
{
//printf("lklient_prov_row\n");
if(data->metkapoi == 0)
 return(0);

//Поиск образца в строке
if(data->poi.koment.ravno()[0] != '\0')
 if(iceb_u_strstrm(row[4],data->poi.koment.ravno()) == 0)
   return(1);

return(0);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_vrint_create_list (class l_vrint_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[300];
int  kolstr=0;
SQL_str row;

printf("l_vrint_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_vrint_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_vrint_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_INT);

sprintf(strsql,"select * from Restvi where kp=%d",data->podr_sp);

if(data->metkapoi == 1)
 {
  if(data->poi.datan.getdlinna() > 1)
   {
    char bros[100];    
    sprintf(bros," and dv >= '%s %s'",
    data->poi.datan.ravno_sqldata(),
    data->poi.vremn.ravno_time());

    strcat(strsql,bros);

    if(data->poi.datak.getdlinna() > 1)
     {
      sprintf(bros," and dv <= '%s %s'",
      data->poi.datak.ravno_sqldata(),
      data->poi.vremk.ravno_time_end());
      strcat(strsql,bros);
     }
   }
 } 
strcat(strsql," order by dv asc");
data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
short d,m,g;
char has_min_sek[10];

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
    if(l_vrint_prov_row(row,data) != 0)
      continue;

  iceb_u_polen(row[0],strsql,sizeof(strsql),1,' ');
  iceb_u_rsdat(&d,&m,&g,strsql,2);

  iceb_u_polen(row[0],has_min_sek,sizeof(has_min_sek),2,' ');

  //Дата
  sprintf(strsql,"%02d.%02d.%d",d,m,g);
  ss[COL_DATA].new_plus(strsql);

  //Время
  ss[COL_VREMZ].new_plus(has_min_sek);
  
  //Цена
  ss[COL_CENA].new_plus(row[2]);

  //Группа
  ss[COL_GRUPA].new_plus(row[3]);

  //Коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[4]));

  //Дата и время записи
  ss[COL_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[6])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_u_kszap(row[5],0));

  //Цена для женщин
  ss[COL_CENA_F_W].new_plus(row[7]);
    
  gtk_list_store_append (model, &iter);


  gtk_list_store_set (model, &iter,
  COL_DATA,ss[COL_DATA].ravno(),
  COL_VREMZ,ss[COL_VREMZ].ravno(),
  COL_CENA,ss[COL_CENA].ravno(),
  COL_CENA_F_W,ss[COL_CENA_F_W].ravno(),
  COL_GRUPA,ss[COL_GRUPA].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_VREM,ss[COL_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_vrint_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
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

sprintf(strsql,"%s %s:%d %s %s:%d",
gettext("Список временных интервалов"),
gettext("Подразделение"),data->podr_sp,
data->naim_podr.ravno(),
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
  iceb_str_poisk(&spis,data->poi.datan.ravno(),gettext("Дата начала"));
  iceb_str_poisk(&spis,data->poi.vremn.ravno(),gettext("Время начала"));
  iceb_str_poisk(&spis,data->poi.datak.ravno(),gettext("Дата конца"));
  iceb_str_poisk(&spis,data->poi.vremk.ravno(),gettext("Время конца"));
  iceb_str_poisk(&spis,data->poi.koment.ravno(),gettext("Коментарий"));

  gtk_label_set_text(GTK_LABEL(data->labelpoi),spis.ravno_toutf());
      
  gtk_widget_show(data->labelpoi); //Показываем

 }
else
 {
  gtk_widget_hide(data->labelpoi); //Показываем
 }





gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

printf("l_vrint_create_list end\n");

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_vrint_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

//printf("l_vrint_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата"), renderer,"text", COL_DATA,NULL);

renderer = gtk_cell_renderer_text_new ();


gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Время"), renderer,"text", COL_VREMZ,NULL);
renderer = gtk_cell_renderer_text_new ();

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Цена"), renderer,"text", COL_CENA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Цена (Ж)"), renderer,"text", COL_CENA_F_W,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Группа"), renderer,"text", COL_GRUPA,NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,"text", COL_KTO,NULL);

//printf("l_vrint_add_columns end\n");

}
/*****************************/
/*Копирование записей*/
/**********************/

int l_vrint_kz(int podr,GtkWidget *wpredok)
{

static class iceb_u_str dataz;
static class iceb_u_str datan;
printf("dataz=%d\n",dataz.getdlinna());

if(dataz.getdlinna() == 0)
 dataz.plus("");
if(datan.getdlinna() == 0)
 datan.plus("");
 
if(l_vrint_k(&dataz,&datan,wpredok) != 0)
 return(1);
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);

SQL_str row;
class SQLCURSOR cur;

char strsql[300];
sprintf(strsql,"select * from Restvi where kp=%d and dv >='%s 0:0:0' and dv <= '%s 23:59:59'",
podr,
dataz.ravno_sqldata(),
dataz.ravno_sqldata());
 
int kolstr=0;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 return(1);
time_t vrem;
time(&vrem);

char vrem_zap[20];
float kolstr1=0.;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  iceb_u_polen(row[0],vrem_zap,sizeof(vrem_zap),2,' ');

  sprintf(strsql,"replace into Restvi \
values ('%s %s',%d,%s,'%s','%s',%d,%ld,%s)",
  datan.ravno_sqldata(),
  vrem_zap,
  podr,
  row[2],
  row[3],
  row[4],
  iceb_getuid(wpredok),vrem,
  row[7]);

//  printf("strsql=%s\n",strsql);

  iceb_sql_zapis(strsql,1,0,wpredok);     
 }

return(0);
}

