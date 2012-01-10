/*$Id: l_zarsocot.c,v 1.17 2011-02-21 07:35:55 sasa Exp $*/
/*14.11.2009	13.09.2006	Белых А.И.	l_zarsocot.c
Работа со списком соц-отчислений на фонд зарплаты работника
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"
enum
{
 FK2,
 SFK2,
 FK4,
 FK3,
 FK5,
 FK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_KOD,
 COL_NAIM,
 COL_SHET,
 COL_Z_SUMI,
 COL_SUMA_OB,
 COL_Z_SUMI_BU,
 COL_SUMA_BU,
 COL_PROC,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  zarsocot_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  

  class iceb_u_str zapros;
  
  int tabnom;
  short mp,gp;
  class iceb_u_str fio;  

  iceb_u_str kodtv;//Код толькочто введённой записи
  iceb_u_str kodv; //Выбранный код 
  char imaf_prot[50];
  
  //Конструктор
  zarsocot_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    fio.plus("");
    memset(imaf_prot,'\0',sizeof(imaf_prot));
   }      
 };

gboolean   zarsocot_key_press(GtkWidget *widget,GdkEventKey *event,class zarsocot_data *data);
void zarsocot_vibor(GtkTreeSelection *selection,class zarsocot_data *data);
void zarsocot_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class zarsocot_data *data);
void  zarsocot_knopka(GtkWidget *widget,class zarsocot_data *data);
void zarsocot_add_columns (GtkTreeView *treeview);
void zarsocot_create_list(class zarsocot_data *data);

void zarsocot_udzap(class zarsocot_data *data);
void zarsocot_rasp(class zarsocot_data *data);
int l_zarsocot_v(int tabnom,short mp,short gp,class iceb_u_str *kodz,const char *fio,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;
extern char     *organ;
 
void l_zarsocot(int tabnom,short mp,short gp,GtkWidget *wpredok)
{
class zarsocot_data data;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
sprintf(data.imaf_prot,"zarsoc%d.txt",getpid());

data.tabnom=tabnom;
data.mp=mp;
data.gp=gp;


sprintf(strsql,"select fio from Kartb where tabn=%d",data.tabnom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.fio.new_plus(row[0]);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(strsql,"%s %s",name_system,gettext("Соц-отчисления на фонд оплаты труда"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zarsocot_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

class iceb_u_str zagolov;
zagolov.plus(gettext("Соц-отчисления на фонд оплаты труда"));
zagolov.ps_plus(data.tabnom);
zagolov.plus(" ");
zagolov.plus(data.fio.ravno());

data.label_kolstr=gtk_label_new (zagolov.ravno_toutf());


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),-1,200);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];


sprintf(strsql,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(zarsocot_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(zarsocot_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(zarsocot_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"F4 %s",gettext("Расчёт"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(zarsocot_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Выполнить расчёт отчислений в социальные фонды"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);


sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(zarsocot_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F9 %s",gettext("Просмотр"));
data.knopka[FK9]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(zarsocot_knopka),&data);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Просмотр протокола хода расчёта"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(zarsocot_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

zarsocot_create_list(&data);
gtk_widget_show(data.window);

 gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


printf("l_zarsocot end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return;

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zarsocot_knopka(GtkWidget *widget,class zarsocot_data *data)
{
FILE *ff_prot=NULL;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zarsocot_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    if(iceb_pbpds(data->mp,data->gp,data->window) != 0)
     return;
    data->kodtv.new_plus("");
    l_zarsocot_v(data->tabnom,data->mp,data->gp,&data->kodtv,data->fio.ravno(),data->window);
    zarsocot_create_list(data);
    return;  

  case SFK2:
    if(iceb_pbpds(data->mp,data->gp,data->window) != 0)
     return;

    if(l_zarsocot_v(data->tabnom,data->mp,data->gp,&data->kodv,data->fio.ravno(),data->window) == 0)
     data->kodtv.new_plus(data->kodv.ravno());
    zarsocot_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_pbpds(data->mp,data->gp,data->window) != 0)
     return;
//    printf("data->snanomer=%d\n",data->snanomer);
    zarsocot_udzap(data);
    zarsocot_create_list(data);
    return;  


  case FK4:
    if(iceb_pbpds(data->mp,data->gp,data->window) != 0)
     return;

    if((ff_prot = fopen(data->imaf_prot,"w")) == NULL)
     {
      iceb_er_op_fil(data->imaf_prot,"",errno,data->window);
      ff_prot=NULL;
     }
    
    zarsocw(data->mp,data->gp,data->tabnom,ff_prot,data->window);
    zarsocot_create_list(data);

    if(ff_prot != NULL)
     fclose(ff_prot);

    return;  

  case FK5:
    zarsocot_rasp(data);
    return;  

  case FK9:
    iceb_prosf(data->imaf_prot,data->window);
    return;  

    
  case FK10:
    unlink(data->imaf_prot);
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zarsocot_key_press(GtkWidget *widget,GdkEventKey *event,class zarsocot_data *data)
{
iceb_u_str repl;
//printf("zarsocot_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {
  case GDK_plus: //это почемуто не работает Нажата клавиша плюс на основной клавиатуре
  case GDK_KP_Add: //Нажата клавиша "+" на дополнительной клавиатуре
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

  case GDK_F9:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK9]),"clicked");
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("zarsocot_key_press-Нажата клавиша Shift\n");

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
void zarsocot_create_list (class zarsocot_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row;
iceb_u_str zagolov;

//printf("zarsocot_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(zarsocot_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(zarsocot_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_INT);

sprintf(strsql,"select * from Zarsocz where datz='%d-%d-01' and tabn=%d",
data->gp,data->mp,data->tabnom);

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;

SQL_str row1;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  

  if(iceb_u_SRAV(data->kodtv.ravno(),row[0],0) == 0)
   data->snanomer=data->kolzap;

  //Код соц-отчисления
  ss[COL_KOD].new_plus(iceb_u_toutf(row[2]));

  //Наименование 
  ss[COL_NAIM].new_plus("");
  sprintf(strsql,"select naik from Zarsoc where kod=%s",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     ss[COL_NAIM].new_plus(iceb_u_toutf(row1[0]));

  //Счёт
  ss[COL_SHET].new_plus(iceb_u_toutf(row[3]));

  //Сумма с которой расчитывалось соц-отчисление
  ss[COL_Z_SUMI].new_plus(atof(row[4]));

  //Сумма соц отчисления
  ss[COL_SUMA_OB].new_plus(atof(row[5]));


  //Сумма с которой расчитывалось бюджетное соц-отчисление
  ss[COL_Z_SUMI_BU].new_plus(atof(row[10]));

  //Сумма соц отчисления бюджетного
  ss[COL_SUMA_BU].new_plus(atof(row[9]));

  //Процент 
  ss[COL_PROC].new_plus(atof(row[6]));

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[8])));


  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[7],0,data->window));


  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,ss[COL_KOD].ravno(),
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_SHET,ss[COL_SHET].ravno(),
  COL_Z_SUMI,ss[COL_Z_SUMI].ravno(),
  COL_SUMA_OB,ss[COL_SUMA_OB].ravno(),
  COL_Z_SUMI_BU,ss[COL_Z_SUMI_BU].ravno(),
  COL_SUMA_BU,ss[COL_SUMA_BU].ravno(),
  COL_PROC,ss[COL_PROC].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kodtv.new_plus("");
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

zarsocot_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


zagolov.new_plus(gettext("Соц-отчисления на фонд оплаты труда"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
zagolov.ps_plus(data->tabnom);
zagolov.plus(" ");
zagolov.plus(data->fio.ravno());
zagolov.plus(" ");
zagolov.plus(gettext("Дата"));
zagolov.plus(":");
zagolov.plus(data->mp);
zagolov.plus(".");
zagolov.plus(data->gp);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void zarsocot_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

printf("zarsocot_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт"), renderer,"text", COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Процент"), renderer,"text", COL_PROC,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("С какой\nсуммы"), renderer,"text", COL_Z_SUMI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Общая сумма\nсоцотчисления"), renderer,"text", COL_SUMA_OB,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("С какой\nсуммы\nбюджет"), renderer,"text", COL_Z_SUMI_BU,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Бюджетная сумма\nсоцотчисления"), renderer,"text", COL_SUMA_BU,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

printf("zarsocot_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void zarsocot_vibor(GtkTreeSelection *selection,class zarsocot_data *data)
{
printf("zarsocot_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gint  nomer;

gtk_tree_model_get(model,&iter,COL_KOD,&kod,NUM_COLUMNS,&nomer,-1);

data->kodv.new_plus(iceb_u_fromutf(kod));
data->snanomer=nomer;

g_free(kod);

//printf("zarsocot_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void zarsocot_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class zarsocot_data *data)
{
//printf("zarsocot_v_row\n");
//printf("zarsocot_v_row корректировка\n");

gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");

}
/*****************************/
/*Удаление записи            */
/*****************************/

void zarsocot_udzap(class zarsocot_data *data)
{

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
 return;

char strsql[512];

sprintf(strsql,"delete from Zarsocz where datz='%04d-%02d-01' and tabn=%d and kodz=%d",
data->gp,data->mp,data->tabnom,
data->kodv.ravno_atoi());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;

}
/**********************************/
/*Распечатать список*/
/****************************/

void zarsocot_rasp(class zarsocot_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;


if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"zarsocot%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Соц-отчисления на фонд оплаты труда"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_u_zagolov(gettext("Соц-отчисления на фонд оплаты труда"),0,0,0,0,0,0,organ,ff);
fprintf(ff,"%d %s %s:%d.%d\n",data->tabnom,data->fio.ravno(),gettext("Дата"),data->mp,data->gp);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Код |   Наименование     | Счёт  |С суммы |Соц.отч.|С б.сум.|Б.соц.от|Дата и время запис.| Кто записал\n"));
  
fprintf(ff,"\
----------------------------------------------------------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;

char naim[512];
SQL_str row1;
class SQLCURSOR cur1;
double suma_i[4];
memset(suma_i,'\0',sizeof(suma_i));

while(cur.read_cursor(&row) != 0)
 {

  //Наименование 
  memset(naim,'\0',sizeof(naim));
  
  sprintf(strsql,"select naik from Zarsoc where kod=%s",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    strncpy(naim,row1[0],sizeof(naim)-1);

  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[8]));

  //Кто записал
  s6.new_plus(iceb_kszap(row[7],1,data->window));

  fprintf(ff,"%-4s %-*.*s %-*s %8s %8s %8s %8s %s %s\n",
  row[2],
  iceb_u_kolbait(20,naim),iceb_u_kolbait(20,naim),naim,
  iceb_u_kolbait(7,row[3]),row[3],
  row[4],row[5],row[10],row[9],
  s5.ravno(),s6.ravno());
  if(iceb_u_strlen(naim) > 20)
   fprintf(ff,"%4s %s\n","",iceb_u_adrsimv(20,naim));

  suma_i[0]+=atof(row[4]);
  suma_i[1]+=atof(row[5]);

  suma_i[2]+=atof(row[10]);
  suma_i[3]+=atof(row[9]);

 }

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %8.2f %8.2f %8.2f %8.2f\n",
iceb_u_kolbait(33,gettext("Итого")),gettext("Итого"),suma_i[0],suma_i[1],suma_i[2],suma_i[3]);

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
