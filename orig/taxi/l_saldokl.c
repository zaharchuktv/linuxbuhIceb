/*$Id: l_saldokl.c,v 1.36 2011-02-21 07:36:20 sasa Exp $*/
/*20.03.2006	13.12.2004	Белых А.И.	l_saldokl.c
Программа вычисления сальдо по клиенту и показ всех счетов
*/
#include <stdlib.h>
#include "i_rest.h"

enum
{
  FK2, //просмотр счёта
//  FK3, //оплата
//  FK4, //расчёт сдачи
  FK10, //выход
  KOL_F_KL
};

enum
{
 COL_METKA,
 COL_DATAD,
 COL_NOMD,
 COL_SUMA, 
 COL_KODPOD,
 COL_NAIMPOD,
 COL_KOMENT,
 COL_VREM,
 COL_KTO,
 NUM_COLUMNS
};

class  saldokl_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_saldo;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  
  iceb_u_str kodkl;
  int kolzap;  
  int       snanomer;   //номер записи на которую надостать или -2

  short dv,mv,gv; //дата выбранной записи
  iceb_u_str nomdv; //номер документа выбранной записи
  time_t dvk; //дата+время возврата карточки в секундах
  double saldo;  //Конечное сальдо (общее наличное+безналичное)
  double saldo_po_kas; //Сальдо по кассе (только наличка)
  double ost_na_saldo; //Остаток на сальдо пред первым неоплаченным документом
  iceb_u_spisok nomdoks; //список номеров счетов
  iceb_u_int    god_doks; //года выписки счетов

  //Конструктор
  saldokl_data()
   {
    dvk=0;
    ost_na_saldo=0.;
    saldo_po_kas=0.;
    saldo=0.;
    snanomer=0; 
    kolzap=0;
    kl_shift=0;
    window=treeview=NULL;
   }

};
void saldokl_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class saldokl_data *data);
gboolean   saldokl_key_press(GtkWidget *widget,GdkEventKey *event,class saldokl_data *data);
void  saldokl_knopka(GtkWidget *widget,class saldokl_data *data);
void saldokl_add_columns(GtkTreeView *treeview);
void saldokl_vibor(GtkTreeSelection *selection,class saldokl_data *data);
void saldokl_create_list (class saldokl_data *data);

void  l_restdok1(short god,iceb_u_str *nomdok,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;
extern iceb_u_str shrift_ravnohir;
extern int nomer_kas;
extern uid_t kod_operatora;
                     

void l_saldokl(const char *kodkl,GtkWidget *wpredok)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,1,wpredok);

class  saldokl_data data;
char strsql[500];
data.kodkl.new_plus(kodkl);

class iceb_sql_flag kl_flag;
iceb_refresh();

if(rest_flag_kl(&kl_flag,data.kodkl.ravno(),wpredok) != 0)
 return;
iceb_refresh();

data.saldo= ras_sal_kl(kodkl,&data.ost_na_saldo,&data.saldo_po_kas,wpredok);

SQL_str row;
SQLCURSOR cur;
//читаем фамилию клиента
sprintf(strsql,"select fio,dvk from Taxiklient where kod='%s'",data.kodkl.ravno());
char fio[60];
memset(fio,'\0',sizeof(fio));
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  strncpy(fio,row[0],sizeof(fio)-1);
  
  data.dvk=iceb_u_datetime_sec(row[1]);
//  printf("%s %ld\n",row[1],data.dvk);
 }


gdite.close();
/*****************
if(data.saldo > -0.01)
 {
   
  class iceb_u_str repl;
  repl.plus(gettext("Долгов нет !"));
  repl.ps_plus(gettext("Клиент"));
  repl.plus(":");
  repl.plus(data.kodkl.ravno());
  repl.plus(" ");
  repl.plus(fio);
  repl.ps_plus(gettext("Сальдо"));
  repl.plus(":");
  repl.plus(data.saldo);
  iceb_menu_soob(&repl,wpredok);
  return;
 }
****************/

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(strsql,"%s %s",name_system,gettext("Список счетов"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(saldokl_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список счетов"));
data.label_saldo=gtk_label_new ("");

PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_modify_font(data.label_saldo,font_pango);
pango_font_description_free(font_pango);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),data.label_saldo,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

gtk_widget_show(data.sw);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Просмотр"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(saldokl_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Просмотр выбранного документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

/******************
sprintf(strsql,"F3 %s",gettext("Оплата"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(saldokl_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Отметить документы как оплаченные"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"F4 %s",gettext("Расчёт"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(saldokl_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Рассчитать сдачу"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);
*********************/

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(saldokl_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

saldokl_create_list(&data);

gtk_widget_show(data.window);
//gtk_widget_show_all(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();

printf("l_saldokl end\n");

}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void saldokl_create_list (class saldokl_data *data)
{
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[500];
int  kolstr=0;
SQL_str row;
SQL_str row1;

data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(saldokl_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(saldokl_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_INT);

//sprintf(strsql,"select nomd,datd,koment,ktoi,vrem,podr from Restdok where kodkl='%s' and mo=0 order by vremz asc",data->kodkl.ravno());
sprintf(strsql,"select nomd,datd,koment,ktoi,vrem,podr,mo from Restdok where kodkl='%s' and vremz >= %ld order by vremz desc",
data->kodkl.ravno(),data->dvk);
printf("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
short d,m,g;

while(cur.read_cursor(&row) != 0)
 {

  //номер документа
  ss[COL_NOMD].new_plus(iceb_u_toutf(row[0]));
  data->nomdoks.plus(row[0]);
  
  //дата выписки счёта
  iceb_u_rsdat(&d,&m,&g,row[1],2);
  sprintf(strsql,"%02d.%02d.%d",d,m,g);
  ss[COL_DATAD].new_plus(strsql);
  data->god_doks.plus(g);
  
  //Коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(iceb_u_vremzap(row[2])));

  //Дата и время записи
  ss[COL_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[4])));
  //Кто записал
  ss[COL_KTO].new_plus(iceb_u_kszap(row[3],0));

  //код подразделения
  ss[COL_KODPOD].new_plus(iceb_u_toutf(row[5]));

  //Наименование подразделения
  ss[COL_NAIMPOD].new_plus("");
  sprintf(strsql,"select naik from Restpod where kod=%s",row[5]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    ss[COL_NAIMPOD].new_plus(iceb_u_toutf(row1[0]));
  
  //Вычисляем сумму по счёту
  sprintf(strsql,"%.2f",sumapsh(g,row[0],"",data->window));
  ss[COL_SUMA].new_plus(strsql);

  //Метка списания
  if(atoi(row[6]) == 0)
    ss[COL_METKA].new_plus("*");
  else 
    ss[COL_METKA].new_plus("");
  
  gtk_list_store_append (model, &iter);


  gtk_list_store_set (model, &iter,
  COL_METKA,   ss[COL_METKA].ravno(),
  COL_DATAD,   ss[COL_DATAD].ravno(),
  COL_NOMD,    ss[COL_NOMD].ravno(),
  COL_SUMA,    ss[COL_SUMA].ravno(),
  COL_KODPOD,  ss[COL_KODPOD].ravno(),
  COL_NAIMPOD, ss[COL_NAIMPOD].ravno(),
  COL_KOMENT,  ss[COL_KOMENT].ravno(),
  COL_VREM,    ss[COL_VREM].ravno(),
  COL_KTO,     ss[COL_KTO].ravno(),
  NUM_COLUMNS, data->kolzap,
               -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

saldokl_add_columns (GTK_TREE_VIEW (data->treeview));

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK4]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK4]),TRUE);//Доступна
 }


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str zagolov;
zagolov.plus(gettext("Список счетов"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

sprintf(strsql,"\
%-20s:%8.2f\n\
%-20s:%8.2f\n\
%-20s:%8.2f\n",
gettext("Сальдо"),
data->ost_na_saldo,
gettext("Сумма по счетам"),
(data->saldo - data->ost_na_saldo)*-1,
gettext("К оплате"),
data->saldo*-1);

gtk_label_set_text(GTK_LABEL(data->label_saldo),iceb_u_toutf(strsql));

gtk_widget_show(data->label_kolstr);
gtk_widget_show(data->label_saldo);

//printf("l_saldokl end\n");

}
/*****************/
/*Создаем колонки*/
/*****************/

void saldokl_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

//printf("saldokl_add_columns\n");

renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"M", renderer,"text", COL_METKA,NULL);

renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата док-та"), renderer,"text", COL_DATAD,NULL);

renderer = gtk_cell_renderer_text_new ();
//g_object_set(G_OBJECT(renderer),"foreground","white",NULL);
//g_object_set(G_OBJECT(renderer),"background","black",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,\
gettext("Н/д"), renderer,"text", COL_NOMD,NULL);

renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("К/п"), renderer,"text", COL_KODPOD,NULL);


renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Наименование подразделения"), renderer,"text", COL_NAIMPOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,"text", COL_KTO,NULL);

//printf("saldokl_add_columns end\n");

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  saldokl_knopka(GtkWidget *widget,class saldokl_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("saldokl_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {
  case FK2:

    l_restdok1(data->gv,&data->nomdv,data->window);
    return;  

/***************
  case FK3:
      if(ras_sheks(&data->god_doks,&data->nomdoks,data->window) == 0)
       {
        for(int i=0; i < data->nomdoks.kolih(); i++)
         otm_opl_dok(data->god_doks.ravno(i),data->nomdoks.ravno(i),0.,data->window);
        gtk_widget_destroy(data->window);
       }
    return;  

***************/
    
  case FK10:
//    printf("saldokl_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   saldokl_key_press(GtkWidget *widget,GdkEventKey *event,class saldokl_data *data)
{
printf("saldokl_key_press keyval=%d state=%d\n",
event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

//  case GDK_F3:
//    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
//    return(TRUE);

//  case GDK_F4:
//    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
//    return(TRUE);
   
  
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(TRUE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("saldokl_key_press-Нажата клавиша Shift\n");

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
void saldokl_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class saldokl_data *data)
{
printf("saldokl_v_row\n");
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
}
/****************************/
/*Выбор строки*/
/**********************/

void saldokl_vibor(GtkTreeSelection *selection,class saldokl_data *data)
{
printf("saldokl_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;
gchar *datd;
gchar *nomd;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_DATAD,&datd,COL_NOMD,&nomd,NUM_COLUMNS,&nomer,-1);

iceb_u_rsdat(&data->dv,&data->mv,&data->gv,datd,1);

data->nomdv.new_plus(iceb_u_fromutf(nomd));
data->snanomer=nomer;

g_free(datd);
g_free(nomd);

//printf("%d.%d.%d %s %d\n",data->dv,data->mv,data->gv,data->nomdokv.ravno(),nomer);

}
