/*$Id: l_uslkart.c,v 1.24 2011-02-21 07:35:54 sasa Exp $*/
/*12.11.2009	14.11.2005	Белых А.И.	l_uslkart.c
Просмотр карточек в учёте услуг
*/
#include <math.h>
#include <pwd.h>
#include <errno.h>
#include <stdlib.h>
#include "buhg_g.h"
#include <unistd.h>
#include "l_uslkart.h"
enum
{
 FK3,
 FK4,
 FK5,
 FK10,
 KOL_F_KL
};

enum
{
 COL_PR,  //+ приход - расход
 COL_DATAP,
 COL_DATAD,
 COL_NOMD,
 COL_KODOP,
 COL_KONTR,
 COL_KOLIH,
 COL_CENA,
 COL_PODR,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};


class  uslkart_data
 {
  public:
  class uslkart_poi poi;  


  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  
  
  int metka_zap;
  int kodzap;
  iceb_u_str naim_zap;
    
  //Реквизиты выбранной записи
  short ddv,mdv,gdv;
  short dpv,mpv,gpv;
  int podrv;
  int tpv;
  iceb_u_str nomdv;
  int nzv;
  iceb_u_str kodopv;
  
  //диапазон дат для просмотра записей
  short dn,mn,gn;
  short dk,mk,gk;
  iceb_u_str zapros; //Запрос на поиск записей
  
  //Конструктор
  uslkart_data()
   {
    metka_zap=0;
    kodzap=0;
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
   }      
 };

gboolean   uslkart_key_press(GtkWidget *widget,GdkEventKey *event,class uslkart_data *data);
void uslkart_vibor(GtkTreeSelection *selection,class uslkart_data *data);
void uslkart_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class uslkart_data *data);
void  uslkart_knopka(GtkWidget *widget,class uslkart_data *data);
void uslkart_add_columns (GtkTreeView *treeview);
void uslkart_create_list(class uslkart_data *data);
int l_uslkart_p(class uslkart_poi *datap,GtkWidget *wpredok);
int  uslkart_prov_row(SQL_str row,class uslkart_data *data);
int uslkart_udzap(class uslkart_data *data);
void   raszpod(class uslkart_data *data);

extern SQL_baza	bd;
extern char *name_system;
extern short	startgodus; /*Стартовый год просмотров*/
extern char *organ;

void l_uslkart(int metka_zap,int kodzap,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
class  uslkart_data data;
data.metka_zap=metka_zap;
data.kodzap=kodzap;

data.naim_zap.plus("");

if(metka_zap == 0) //материал
 sprintf(strsql,"select naimat from Material where kodm=%d",kodzap);

if(metka_zap == 1) //услуга
 sprintf(strsql,"select naius from Uslugi where kodus=%d",kodzap);
 
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.naim_zap.new_plus(row[0]);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(strsql,"%s %s",name_system,gettext("Список подтверждённых записей"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uslkart_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

iceb_u_str soob;

soob.new_plus(gettext("Список подтверждённых записей"));
//soob.ps_plus(kodzap);
//soob.plus(" ");
//soob.plus(naim_zap.ravno());

data.label_kolstr=gtk_label_new (soob.ravno_toutf());


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");
GdkColor color;
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),600,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];


sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(uslkart_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(uslkart_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(uslkart_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(uslkart_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

uslkart_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();

printf("l_uslkart end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uslkart_knopka(GtkWidget *widget,class uslkart_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("uslkart_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {

  case FK3:
    if(data->kolzap == 0)
      return;

    uslkart_udzap(data);

    uslkart_create_list(data);

    return;  

  case FK4:
    l_uslkart_p(&data->poi,data->window);
    uslkart_create_list(data);
    return;  

  case FK5:
    raszpod(data);
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

gboolean   uslkart_key_press(GtkWidget *widget,GdkEventKey *event,class uslkart_data *data)
{
iceb_u_str repl;
//printf("uslkart_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    return(TRUE);
  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);
  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    return(TRUE);

  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("uslkart_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}

/***********************************/
/*Создаем список для просмотра */
/***********************************/
void uslkart_create_list (class uslkart_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

class iceb_clock skur(data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQL_str row;
SQLCURSOR cur1;
SQL_str row1;
char strsql[512];
int  kolstr=0;
//диапазон дат для просмотра записей

//printf("uslkart_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(uslkart_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(uslkart_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);

model = gtk_list_store_new (NUM_COLUMNS+2, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT,  
G_TYPE_INT);

data->dn=1; data->mn=1; 
data->gn=startgodus;
if(data->gn == 0)
 {   
  short dt,mt,gt;
  iceb_u_poltekdat(&dt,&mt,&gt);
  data->gn=gt;
 }
data->dk=data->mk=data->gk=0;

if(data->poi.metka_poi == 1)
 if(data->poi.datan.getdlinna() > 1) 
  {
   iceb_rsdatp(&data->dn,&data->mn,&data->gn,data->poi.datan.ravno(),&data->dk,&data->mk,&data->gk,data->poi.datak.ravno(),data->window);
  }

if(data->dk == 0)
  sprintf(strsql,"select * from Usldokum2 where metka=%d and kodzap=%d \
and datp >= '%04d-%02d-%02d' order by datp asc",
  data->metka_zap,data->kodzap,data->gn,data->mn,data->dn);
else
  sprintf(strsql,"select * from Usldokum2 where metka=%d and kodzap=%d \
and datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' order by datp asc",
  data->metka_zap,data->kodzap,data->gn,data->mn,data->dn,data->gk,data->mk,data->dk);

data->zapros.new_plus(strsql); //Запоминаем запрос чтобы можно было использовать при распечатке записей

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
float kolstr1=0.;
int nomzap=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
   if(uslkart_prov_row(row,data) != 0)
    continue;

  //приход/расход
  if(row[0][0] == '1')
    ss[COL_PR].new_plus("+");
  else
    ss[COL_PR].new_plus("-");
  
  //Дата подтверждения
  ss[COL_DATAP].new_plus(iceb_u_sqldata(row[2]));

  //Дата документа
  ss[COL_DATAD].new_plus(iceb_u_sqldata(row[1]));

  //Номер документа
  ss[COL_NOMD].new_plus(iceb_u_toutf(row[3]));

  //читаем шапку документа
  sprintf(strsql,"select kodop,kontr from Usldokum where datd='%s' and podr=%s and nomd='%s' \
and tp=%s",
  row[1],row[10],row[3],row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) > 0)
   {
    //Код операции
    ss[COL_KODOP].new_plus(iceb_u_toutf(row1[0]));

    //Код контрагента
    ss[COL_KONTR].new_plus(iceb_u_toutf(row1[1]));
   }
  else
   { 
    iceb_u_str repl;
    repl.plus(gettext("Не найден документ"));
    repl.plus(" !!!");
    repl.ps_plus(row[1]);
    repl.plus(" ");
    repl.plus(row[10]);
    repl.plus(" ");
    repl.plus(row[3]);
    repl.plus(" ");
    repl.plus(row[0]);
    iceb_menu_soob(&repl,data->window);
    
    //Код операции
    ss[COL_KODOP].new_plus(iceb_u_toutf(" "));

    //Код контрагента
    ss[COL_KONTR].new_plus(iceb_u_toutf(" "));
   }

  //количество
  sprintf(strsql,"%.10g",atof(row[6]));
  ss[COL_KOLIH].new_plus(strsql);

  //цена
  sprintf(strsql,"%.10g",atof(row[7]));
  ss[COL_CENA].new_plus(strsql);

  ss[COL_PODR].new_plus(row[10]);


  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[12])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[11],0,data->window));

  nomzap=atoi(row[13]);
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_PR,ss[COL_PR].ravno(),
  COL_DATAP,ss[COL_DATAP].ravno(),
  COL_DATAD,ss[COL_DATAD].ravno(),
  COL_NOMD,ss[COL_NOMD].ravno(),
  COL_KODOP,ss[COL_KODOP].ravno(),
  COL_KONTR,ss[COL_KONTR].ravno(),
  COL_KOLIH,ss[COL_KOLIH].ravno(),
  COL_CENA,ss[COL_CENA].ravno(),
  COL_PODR,ss[COL_PODR].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,nomzap,
  NUM_COLUMNS+1,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

uslkart_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список подтверждённых записей"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
zagolov.ps_plus(data->kodzap);
zagolov.plus(" ");
zagolov.plus(data->naim_zap.ravno());

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

if(data->poi.metka_poi == 1)
 {
//  printf("Формирование заголовка с реквизитами поиска.\n");
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");
  iceb_str_poisk(&zagolov,data->poi.nomd.ravno(),gettext("Номер документа"));
  iceb_str_poisk(&zagolov,data->poi.datan.ravno(),gettext("Дата начала"));
  iceb_str_poisk(&zagolov,data->poi.datak.ravno(),gettext("Дата конца"));
  iceb_str_poisk(&zagolov,data->poi.kontr.ravno(),gettext("Код контрагента"));
  iceb_str_poisk(&zagolov,data->poi.kodop.ravno(),gettext("Код операции"));
  iceb_str_poisk(&zagolov,data->poi.podr.ravno(),gettext("Код подразделения"));
  if(data->poi.pr_ras.ravno_pr() == 1)
    zagolov.ps_plus(gettext("Только приходы"));
  if(data->poi.pr_ras.ravno_pr() == 2)
    zagolov.ps_plus(gettext("Только расходы"));

  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

//gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void uslkart_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

printf("uslkart_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"+\\-", renderer,"text", COL_PR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата подт-ния"), renderer,"text", COL_DATAP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата док-та"), renderer,"text", COL_DATAD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Номер документа"), renderer,"text", COL_NOMD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Операция"), renderer,"text", COL_KODOP,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Контрагент"), renderer,"text", COL_KONTR,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Количество"), renderer,"text", COL_KOLIH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Цена"), renderer,"text", COL_CENA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Подразделение"), renderer,"text", COL_PODR,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);
printf("uslkart_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void uslkart_vibor(GtkTreeSelection *selection,class uslkart_data *data)
{
printf("uslkart_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *datap;
gchar *datad;
gchar *podr;
gchar *tp;
gchar *nomd;
gchar *kodop;
gint  nz;

gint  nomer;


gtk_tree_model_get(model,&iter,
COL_DATAP,&datap,
COL_DATAD,&datad,
COL_PODR,&podr,
COL_PR,&tp,
COL_NOMD,&nomd,
COL_KODOP,&kodop,
NUM_COLUMNS,&nz,
NUM_COLUMNS+1,&nomer,-1);

iceb_u_rsdat(&data->dpv,&data->mpv,&data->gpv,datap,1);
iceb_u_rsdat(&data->ddv,&data->mdv,&data->gdv,datad,1);
data->podrv=atoi(podr);
data->tpv=0;
if(tp[0] == '+')
 data->tpv=1;
if(tp[0] == '-')
 data->tpv=2;
data->nomdv.new_plus(iceb_u_fromutf(nomd));
data->nzv=nz;
data->kodopv.new_plus(iceb_u_fromutf(kodop));
data->snanomer=nomer;

g_free(datap);
g_free(datad);
g_free(podr);
g_free(tp);
g_free(nomd);
g_free(kodop);

}

/****************************/
/*Выбор строки*/
/**********************/
void uslkart_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class uslkart_data *data)
{
printf("Кликнули два раза по строке.\n");
}


/****************************/
/*Проверка записей          */
/*****************************/

int  uslkart_prov_row(SQL_str row,class uslkart_data *data)
{


if(data->poi.metka_poi == 0)
 return(0);
 
if(data->poi.kontr.getdlinna() > 1 || data->poi.kodop.getdlinna() > 1)
 {
  SQL_str row1;
  SQLCURSOR cur1;
  char strsql[512];
  //читаем шапку документа
  sprintf(strsql,"select kodop,kontr from Usldokum where datd='%s' and podr=%s and nomd='%s' \
and tp=%s",row[1],row[10],row[3],row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) > 0)
   {
    if(iceb_u_proverka(data->poi.kontr.ravno(),row1[1],0,0) != 0)
     return(1);

    if(iceb_u_proverka(data->poi.kodop.ravno(),row1[0],0,0) != 0)
     return(1);
   }

 }  


if(iceb_u_proverka(data->poi.podr.ravno(),row[10],0,0) != 0)
 return(1);


if(data->poi.pr_ras.ravno_pr() != 0 && data->poi.pr_ras.ravno_pr() != atoi(row[0]))
 return(1);

if(data->poi.nomd.getdlinna() > 1)
 if(iceb_u_SRAV(data->poi.nomd.ravno(),row[3],1) != 0)
//  if(iceb_u_SRAV(data->poi.nomd.ravno(),row[11],1) != 0)
   return(1);


return(0);
}
/********************************/
/*Удалить запись*/
/******************************/
//Если удалили возаращаем 0 
int uslkart_udzap(class uslkart_data *data)
{
char strsql[512];

if(iceb_pbpds(data->mpv,data->gpv,data->window) != 0)
  return(1);
iceb_u_str stroka;
iceb_u_spisok stroka1;

stroka.new_plus(gettext("Внимание"));
stroka.plus(" !!!");
stroka1.plus(stroka.ravno());

stroka1.plus(gettext("Документ будет помечен, как не подтвержденный и без проводок !"));
stroka1.plus(gettext("Удалить запись ? Вы уверены ?"));

if(iceb_menu_danet(&stroka1,2,data->window) == 2)
 return(1);


sprintf(strsql,"delete from Usldokum2 where podr=%d and metka=%d \
and kodzap=%d and nomd='%s' and datp='%d-%02d-%02d' and \
datd='%d-%02d-%02d' and tp=%d",
      data->podrv,data->metka_zap,data->kodzap,data->nomdv.ravno(),data->gpv,
      data->mpv,data->dpv,data->gdv,data->mdv,data->ddv,data->tpv);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR)
  {
   stroka.new_plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&stroka,data->window);
   return(1); 
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
 }


if(data->tpv == 1)
 {
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select vido from Usloper1 where kod='%s'",data->kodopv.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    stroka.new_plus(gettext("Не найден код операции"));
    stroka.plus(" ");
    stroka.plus(data->kodopv.ravno());
    stroka.plus(" !");
    iceb_menu_soob(&stroka,data->window);
   }
  else
   {
    /*Если операция внутреннего перемещения или изменения
     стоимости то проверять проводки не надо для прихода*/
    if(row[0][0] != '0')
      return(0);
   }
 }

sprintf(strsql,"update Usldokum set pod=0, pro=0 where \
datd='%d.%d.%d' and podr=%d and nomd='%s' and tp=%d",
data->gdv,data->mdv,data->ddv,data->podrv,data->nomdv.ravno(),data->tpv);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR)
  {
   stroka.new_plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&stroka,data->window);
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
 }
return(0);
}
/*********************/
/*Распечатка подтверждающих записей*/
/*********************/
void            raszpod(class uslkart_data *data)
{
short		dp,mp,gp;
short		d,m,g;
double		kolih,cena;
short		tp;
char		bros[512];
SQL_str         row,row1;
char		strsql[512];
FILE            *f1;
char            imaf[20];
double          it;
char		br1[4];
char		naim[512];
char		naimg[512];
int		kgrm=0;
char		kop[20],kor[20];
char		fio[512];
struct  passwd  *ktoz; /*Кто записал*/
SQLCURSOR curr;
SQLCURSOR cur;
int kolstr;

/*Узнаем наименование материалла*/
if(data->metka_zap == 0)
 {
  sprintf(strsql,"select kodgr,naimat  from Material where kodm=%d",data->kodzap);
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)
   {
    kgrm=atoi(row[0]);
    strncpy(naim,row[1],sizeof(naim)-1);
   }
 }
/*Узнаем наименование услуги*/
if(data->metka_zap == 1)
 {
  sprintf(strsql,"select kodgr,naius from Uslugi where kodus=%d",data->kodzap);
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)
   {
    kgrm=atoi(row[0]);
    strncpy(naim,row[1],sizeof(naim)-1);
   }
 }



/*Узнаем наименование группы материалла*/
if(data->metka_zap == 0)
 {
  sprintf(strsql,"select naik from Grup where kod=%d",kgrm);
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)
   {
    strncpy(naimg,row[0],sizeof(naimg)-1);
   }
 }

/*Узнаем наименование группы услуги*/
if(data->metka_zap == 0)
 {
  sprintf(strsql,"select naik from Uslgrup where kod=%d",kgrm);
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)
   {
    strncpy(naimg,row[0],sizeof(naimg)-1);
   }
 }

if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
if(kolstr == 0)
 return;

memset(imaf,'\0',sizeof(imaf));
sprintf(imaf,"upz%d.lst",getpid());
if((f1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

//fprintf(f1,"%s\n",organ);

if(data->metka_zap == 1)
 sprintf(strsql,"%s %d %s",
 gettext("Распечатка подтверждающих записей услуги"),data->kodzap,naim);
if(data->metka_zap == 0)
 sprintf(strsql,"%s %d %s",
 gettext("Распечатка подтверждающих записей материалла"),data->kodzap,naim);

iceb_u_zagolov(strsql,data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,organ,f1);

fprintf(f1,"%s:%d %s\n",gettext("Группа"),kgrm,naimg);

if(data->poi.nomd.getdlinna() > 1)
 fprintf(f1,"%s:%s\n",gettext("Номер документа"),data->poi.nomd.ravno());
if(data->poi.kontr.getdlinna() > 1)
 fprintf(f1,"%s:%s\n",gettext("Контрагент"),data->poi.kontr.ravno());
if(data->poi.kodop.getdlinna() > 1)
 fprintf(f1,"%s:%s\n",gettext("Код операции"),data->poi.kodop.ravno());

if(data->poi.pr_ras.ravno_pr() == 1)
 fprintf(f1,"%s.\n",gettext("Только приходы"));

if(data->poi.pr_ras.ravno_pr() == 2)
 fprintf(f1,"%s.\n",gettext("Только расходы"));

fprintf(f1,"\n\
-----------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(f1,"\
Дата подт.|Дата док. |Ном.докумен.|Операция |Код конт.|Наименование контра.|  Количество |     Цена    |Ед.изм.| Счет |Кто записал|\n");
fprintf(f1,"\
-----------------------------------------------------------------------------------------------------------------------------------\n");


it=0.;

while(cur.read_cursor(&row) != 0)
 {
  if(uslkart_prov_row(row,data) != 0)
    continue;

  /*Читаем код организации, код операции*/
  sprintf(strsql,"select tp,kodop,kontr from Usldokum \
where datd='%s' and podr=%s and nomd='%s' and tp=%s",
  row[1],row[10],row[3],row[0]);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"Не найден документ ! Дата:%s Док.:%s Подр.:%s Тип:%s",
    row[1],row[3],row[10],row[0]);
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  tp=atoi(row1[0]);
  strcpy(kop,row1[1]);
  strcpy(kor,row1[2]);    

  memset(bros,'\0',sizeof(bros));
  if(iceb_u_atof(kor) == 0. && iceb_u_pole(kor,bros,2,'-') == 0 && kor[0] == '0')
    strncpy(bros,"00",sizeof(bros)-1);
  else
    strncpy(bros,kor,sizeof(bros)-1);

  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",bros);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kor);
    iceb_menu_soob(strsql,data->window);
   }
  else
    strcpy(bros,row1[0]);

  strcpy(br1,"+");
  if(tp == 2)
   {  
    strcpy(br1,"-");
    kolih*=(-1);
   }
  iceb_u_rsdat(&dp,&mp,&gp,row[2],2);
  iceb_u_rsdat(&d,&m,&g,row[1],2);

  kolih=atof(row[6]);  
  cena=atof(row[7]);  

  it+=kolih;
  memset(fio,'\0',sizeof(fio));
  if((ktoz=getpwuid(atoi(row[11]))) != NULL)
     strncpy(fio,ktoz->pw_gecos,sizeof(fio)-1);
  else
     sprintf(fio,"%s %d !",gettext("Неизвестный логин"),atoi(row[11]));
  
  fprintf(f1,"%02d.%02d.%d|%02d.%02d.%d|%-*s|%-*s %s|%-*s|%-*.*s|\
%13.13g|%13.10g|%-7s|%-6s|%s\n",
  dp,mp,gp,
  d,m,g,
  iceb_u_kolbait(12,row[3]),row[3],
  iceb_u_kolbait(7,kop),kop,
  br1,
  iceb_u_kolbait(9,kor),kor,
  iceb_u_kolbait(20,bros),iceb_u_kolbait(20,bros),bros,
  kolih,cena,row[8],row[9],fio);
 }


if(fabs(it) < 0.0000000001)
  it=0.;
fprintf(f1,"\
-----------------------------------------------------------------------------------------------------------------------------------\n\
%*s: %13.10g\n",iceb_u_kolbait(74,gettext("Итого")),gettext("Итого"),it);

iceb_podpis(f1,data->window);

fclose(f1);

iceb_u_spisok imafil;
iceb_u_spisok naimfil;

imafil.plus(imaf);
naimfil.plus(gettext("Распечатка подтверждающих записей"));
iceb_ustpeh(imaf,3,data->window);
iceb_rabfil(&imafil,&naimfil,"",0,data->window);
}
