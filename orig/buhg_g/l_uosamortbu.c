/*$Id: l_uosamortbu.c,v 1.15 2011-02-21 07:35:53 sasa Exp $*/
/*27.06.2010	17.07.2005	Белых А.И.	l_uosamortbu.c
Работа с записями амортизационных отчислений налогового учёта
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"
enum
{
 FK2,
 SFK2,
 FK3,
 FK4,
 FK5,
 FK6,
 FK7,
 FK8,
 FK10,
 KOL_F_KL
};

enum
{
 COL_DATA,
 COL_SUMA,
 COL_PODR,
 COL_HZT,
 COL_HNA,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  l_uosamortbu_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_bsiz;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей

  iceb_u_str zapros;

  int innom;
  class iceb_u_str naim;
  int podr;
  class iceb_u_str hzt;
  class iceb_u_str hna;
  short dr,mr,gr; /*Дата расчёта остатка*/
    
  class iceb_u_str data_v; //Выбранная дата 
  class iceb_u_str data_tv; /*Дата только что введённая*/
  class iceb_u_str imafprot; /*Имя файла с протоколом расчёта балансовой стоимости*/
  class iceb_u_str imafprot_am; /*Имя файла с протоколом расчёта амортизации*/
  //Конструктор
  l_uosamortbu_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    
   }      
 };

gboolean   l_uosamortbu_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosamortbu_data *data);
void l_uosamortbu_vibor(GtkTreeSelection *selection,class l_uosamortbu_data *data);
void l_uosamortbu_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class l_uosamortbu_data *data);
void l_uosamortbu_knopka(GtkWidget *widget,class l_uosamortbu_data *data);
void l_uosamortbu_add_columns (GtkTreeView *treeview);
void l_uosamortbu_create_list(class l_uosamortbu_data *data);

int l_uosamortbu_v(int innom,int,const char*,const char *hzt,const char *hna,GtkWidget *wpredok);
void l_uosamortbu_udzap(class l_uosamortbu_data *data);
void l_uosamortbu_rasp(class l_uosamortbu_data *data);
void l_uosamortbu_bsiz(class l_uosamortbu_data *data);
void l_uosamortbu_ra(class l_uosamortbu_data *data);
void l_uosamortbu_sd(class l_uosamortbu_data *data);
void l_uosamortbu_ppr(class l_uosamortbu_data *data);

extern SQL_baza	bd;
extern char *name_system;
extern char     *organ;
extern short	startgoduos; /*Стартовый год*/
extern iceb_u_str shrift_ravnohir;
extern char	*spgnu; //Список групп налогового учета
extern short	metkabo; //Если 1 то организация бюджетная
 
void l_uosamortbu(int innom,int podr,const char *hzt,const char *hna,GtkWidget *wpredok)
{
class l_uosamortbu_data data;
char bros[512];
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

data.innom=innom;
data.podr=podr;
data.hzt.new_plus(hzt);
data.hna.new_plus(hna);
data.naim.plus("");
iceb_u_poltekdat(&data.dr,&data.mr,&data.gr);
data.dr=1;

sprintf(strsql,"bsizbu%d.lst",getpid());
data.imafprot.new_plus(strsql);
unlink(data.imafprot.ravno());

sprintf(strsql,"bsizbua%d.lst",getpid());
data.imafprot_am.new_plus(strsql);
unlink(data.imafprot_am.ravno());

/*узнаём наименование*/
sprintf(strsql,"select naim from Uosin where innom=%d",innom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.naim.new_plus(row[0]);
  
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


sprintf(bros,"%s %s",name_system,gettext("Амортизационные отчисления налогового учёта"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uosamortbu_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 1);
gtk_widget_show(hbox);

class iceb_u_str zagol;
zagol.plus(gettext("Амортизационные отчисления бух.учёта"));
zagol.ps_plus(gettext("Инвентарный номер"));
zagol.plus(":");
zagol.plus(data.innom);
zagol.plus("/");
zagol.plus(data.naim.ravno());

data.label_kolstr=gtk_label_new(zagol.ravno_toutf());


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 1);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);




data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),-1,200);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start(GTK_BOX (vbox2), data.sw, TRUE, TRUE, 1);

data.label_bsiz=gtk_label_new("");
gtk_box_pack_start(GTK_BOX (vbox2), data.label_bsiz, FALSE, FALSE, 1);
PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_modify_font(GTK_WIDGET(data.label_bsiz),font_pango);
pango_font_description_free(font_pango);

gtk_widget_show(data.label_bsiz);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];


sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_uosamortbu_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_uosamortbu_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_uosamortbu_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Расчёт"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_uosamortbu_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Расчёт амортизации"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_uosamortbu_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Документы"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(l_uosamortbu_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Получить список документов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Протокол"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(l_uosamortbu_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Просмотр протокола определения балансовой стоимости"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"F8 %s",gettext("Смена даты"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(l_uosamortbu_knopka),&data);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Сменить дату на которую рассчитать остаточную стоимость"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_uosamortbu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_uosamortbu_create_list(&data);
gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));


gtk_main();

unlink(data.imafprot.ravno());
unlink(data.imafprot_am.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_uosamortbu_knopka(GtkWidget *widget,class l_uosamortbu_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("uosamortbu_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    if(l_uosamortbu_v(data->innom,data->podr,"",data->hzt.ravno(),data->hna.ravno(),data->window) == 0)
     l_uosamortbu_create_list(data);
    return;  

  case SFK2:
    if(l_uosamortbu_v(data->innom,data->podr,data->data_v.ravno(),data->hzt.ravno(),data->hna.ravno(),data->window) == 0)
      l_uosamortbu_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    l_uosamortbu_udzap(data);
    l_uosamortbu_create_list(data);
    return;  


  case FK4:
    l_uosamortbu_ra(data);
    l_uosamortbu_create_list(data);
    return;  

  case FK5:
    l_uosamortbu_rasp(data);
    return;  

  case FK6:
    uosrsdw(data->innom,data->window);
    return;  

  case FK7:
    l_uosamortbu_ppr(data);
    return;  

  case FK8:
    l_uosamortbu_sd(data);
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uosamortbu_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosamortbu_data *data)
{

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


  case GDK_F7:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
    return(TRUE);

  case GDK_F8:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK8]),"clicked");
    return(TRUE);



  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("uosamortbu_key_press-Нажата клавиша Shift\n");

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
void l_uosamortbu_create_list (class l_uosamortbu_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row;
iceb_u_str zagolov;

//printf("uosamortbu_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_uosamortbu_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_uosamortbu_vibor),data);

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
G_TYPE_INT);

sprintf(strsql,"select * from Uosamor1 where innom=%d and god >= %d \
order by god,mes asc",data->innom,startgoduos);

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;

short mes=0,god=0;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  

  mes=atoi(row[1]);
  god=atoi(row[2]);
  sprintf(strsql,"%02d.%d",mes,god);
  if(iceb_u_SRAV(strsql,data->data_tv.ravno(),0) == 0)
   data->snanomer=data->kolzap;
  
  //ДАТА  
  ss[COL_DATA].new_plus(strsql);

  //подразделение
  ss[COL_PODR].new_plus(iceb_u_toutf(row[3]));

  //ШИФР затрат
  ss[COL_HZT].new_plus(iceb_u_toutf(row[4]));
  //Шифр нормы аморт-отчислений
  ss[COL_HNA].new_plus(iceb_u_toutf(row[5]));
  //Сумма
  ss[COL_SUMA].new_plus(atof(row[6]));
  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[8])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[7],0,data->window));

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATA,ss[COL_DATA].ravno(),
  COL_SUMA,ss[COL_SUMA].ravno(),
  COL_PODR,ss[COL_PODR].ravno(),
  COL_HZT,ss[COL_HZT].ravno(),
  COL_HNA,ss[COL_HNA].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->data_tv.new_plus("");
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_uosamortbu_add_columns (GTK_TREE_VIEW (data->treeview));


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



zagolov.new_plus(gettext("Амортизационные отчисления бух.учёта"));
zagolov.ps_plus(gettext("Инвентарный номер"));
zagolov.plus(":");
zagolov.plus(data->innom);
zagolov.plus("/");
zagolov.plus(data->naim.ravno());

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

l_uosamortbu_bsiz(data);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void l_uosamortbu_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

printf("uosamortbu_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата"), renderer,"text", COL_DATA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Подразделение"), renderer,"text", COL_PODR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Шифр затрат аморт-отчислений"), renderer,"text", COL_HZT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Группа"), renderer,"text", COL_HNA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

printf("uosamortbu_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void l_uosamortbu_vibor(GtkTreeSelection *selection,class l_uosamortbu_data *data)
{
//printf("uosamortbu_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *data_v;
gint  nomer;

gtk_tree_model_get(model,&iter,COL_DATA,&data_v,NUM_COLUMNS,&nomer,-1);

data->data_v.new_plus(data_v);
data->snanomer=nomer;

g_free(data_v);

//printf("uosamortbu_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void l_uosamortbu_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_uosamortbu_data *data)
{

gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");

}
/*****************************/
/*Удаление записи            */
/*****************************/

void l_uosamortbu_udzap(class l_uosamortbu_data *data)
{


char strsql[512];

if(iceb_pbpds(data->data_v.ravno(),data->window) != 0)
  return;

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
 return;

short m1=0,g1=0;


iceb_u_rsdat1(&m1,&g1,data->data_v.ravno());

sprintf(strsql,"delete from Uosamor1 where innom=%d and god=%d \
and mes=%d",data->innom,g1,m1);

iceb_sql_zapis(strsql,0,0,data->window);






}
/**********************************/
/*Распечатать список*/
/****************************/

void l_uosamortbu_rasp(class l_uosamortbu_data *data)
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

sprintf(strsql,"uosamortbu%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Амортизационные отчисления бух.учёта"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_u_zagolov(gettext("Амортизационные отчисления бух.учёта"),0,0,0,0,0,0,organ,ff);
fprintf(ff,"%s:%d %s\n",gettext("Инвентарный номер"),data->innom,data->naim.ravno());

fprintf(ff,"\
----------------------------------------------------------------------------\n");

fprintf(ff,"\
 Дата  |  Сумма   |Подр.|Шифр затрат|Группа|Дата и время запис.| Кто записал\n");
  
fprintf(ff,"\
----------------------------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;

double itogo=0.;
while(cur.read_cursor(&row) != 0)
 {

  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[8]));

  //Кто записал
  s6.new_plus(iceb_kszap(row[7],1,data->window));

  fprintf(ff,"%02d.%d %10.2f %5s %11s %6s %s %s\n",
  atoi(row[1]),atoi(row[2]),atof(row[6]),row[3],row[4],row[5],s5.ravno(),s6.ravno());
  itogo+=atof(row[6]);
 }
fprintf(ff,"\
----------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),itogo);

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
/**********************************/
/*Расчёт балансовой стоимоси и износа*/
/***************************************/
void l_uosamortbu_bsiz(class l_uosamortbu_data *data)
{
short dt=data->dr,mt=data->mr,gt=data->gr;
double bs=0.,iz=0.;
double bs1=0.,iz1=0.,iz11=0.;

if(dt == 0)
 {
  iceb_u_poltekdat(&dt,&mt,&gt);
  dt=1;
 }
FILE *ff=NULL;

if((ff = fopen(data->imafprot.ravno(),"w")) == NULL)
 {
  iceb_er_op_fil(data->imafprot.ravno(),"",errno,data->window);
  return;
 }

char strsql[512];
class bsizw_data bal_st;
//  bsiz(in,podd,dt,mt,gt,&bs,&iz,&bs1,&iz1,&iz11,&bsby,&izby,&bs1by,&iz1by,&iz11by,ff);

bsizw(data->innom,data->podr,dt,mt,gt,&bal_st,ff,data->window);

bs=bal_st.sbsby;
iz=bal_st.sizby;
bs1=bal_st.bsby;
iz1=bal_st.izby;
iz11=bal_st.iz1by;

fclose(ff);
iceb_ustpeh(data->imafprot.ravno(),3,data->window);

class iceb_u_str strok;

sprintf(strsql,"%s   %s",gettext("Бал.стоимость"),gettext("Износ"));
strok.plus(strsql);

sprintf(strsql,"%10.2f %10.2f",bs,iz);
strok.ps_plus(strsql);

sprintf(strsql,"%10.2f %10.2f",bs1,iz1);
strok.ps_plus(strsql);

sprintf(strsql,"%s:%9.2f",gettext("Амортизация"),iz11);
strok.ps_plus(strsql);

sprintf(strsql,"%10.2f %10.2f",bs+bs1,iz+iz1+iz11);
strok.ps_plus(strsql);

sprintf(strsql,"%*s:%9.2f",iceb_u_kolbait(11,gettext("Остаток")),gettext("Остаток"),(bs+bs1)-(iz+iz1+iz11));
strok.ps_plus(strsql);

sprintf(strsql,"%*s:%02d.%02d.%d",iceb_u_kolbait(11,gettext("Дата остатка")),gettext("Дата остатка"),dt,mt,gt);
strok.ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_bsiz),strok.ravno_toutf());

}

/***************************/
/*Расчёт амортизации*/
/***************************/

void l_uosamortbu_ra(class l_uosamortbu_data *data)
{
char strsql[512];    
char bros[512];
memset(bros,'\0',sizeof(bros));
short kvrt=0,m1=0,g1=0;
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);    

sprintf(bros,"%d.%d",mt,gt);
sprintf(strsql,"%s (%s)",gettext("Введите дату"),gettext("м.г"));

if(iceb_menu_vvod1(strsql,bros,8,data->window) != 0)
 return;
  
if(iceb_u_rsdat1(&m1,&g1,bros) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
  return;
 }

if(kvrt == 1)
  m1=1;
if(kvrt == 2)
  m1=4;
if(kvrt == 3)
  m1=7;
if(kvrt == 4)
 m1=10;
if(metkabo == 1)
  m1=1;
  

for(int i=m1 ; i < m1+3; i++)
 if(iceb_pbpds(i,g1,data->window) != 0)
  return;


FILE *ff=NULL;

if((ff = fopen(data->imafprot_am.ravno(),"w")) == NULL)
 {
  iceb_er_op_fil(data->imafprot_am.ravno(),"",errno,data->window);
  return;
 }

amortbyw(data->innom,m1,g1,1,data->hna.ravno(),ff,data->window);
fclose(ff);
iceb_ustpeh(data->imafprot_am.ravno(),3,data->window);
}
/*************************/
/*Смена даты расчёта остаточной стоимости*/
/****************************************/
void l_uosamortbu_sd(class l_uosamortbu_data *data)
{

class iceb_u_str datar;

if(iceb_menu_vvod1(gettext("Введите дату на которую расчитать остаточную стоимость (д.м.г)"),&datar,11,data->window) != 0)
 return;

if(datar.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
  return;
 }

iceb_u_rsdat(&data->dr,&data->mr,&data->gr,datar.ravno(),1);
/*Узнаём где числится на эту дату*/
int podraz=0,kodol=0;
if(poiinw(data->innom,data->dr,data->mr,data->gr,&podraz,&kodol,data->window) == 0)
 data->podr=podraz;
 
l_uosamortbu_bsiz(data);

}
/********************************/
/*Просмотр протоколов расчёта*/
/********************************/

void l_uosamortbu_ppr(class l_uosamortbu_data *data)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Просмотр протокола расчёта балансовой стоимости и износа"));//0
punkt_m.plus(gettext("Просмотр протокола расчёта амортизации"));//1


int nomer=0;
while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      iceb_prosf(data->imafprot.ravno(),data->window);
      return;

    case 1:
      iceb_prosf(data->imafprot_am.ravno(),data->window);
      return;
   }
 }
}

