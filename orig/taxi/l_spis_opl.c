/*$Id: l_spis_opl.c,v 1.23 2011-02-21 07:36:20 sasa Exp $*/
/*22.08.2006	25.01.2006	Белых А.И.	Белых А.И.	l_spis_opl.c
просмотр таблицы оплат 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include "i_rest.h"
#include "l_spis_opl.h"

#define VNOS_NAL "Нал."
#define VNOS_BNAL "Б.нал."
#define SPISANIE "Списание"

enum
{
 FK5,
 FK3,
 FK10,
 KOL_F_KL
};

enum
{
 COL_KODKL,
 COL_KASSA,
 COL_DATAZ,
 COL_VREMZ,
 COL_METKAZ,
 COL_SUMA,
 COL_NOMDOK,
 COL_DATAD,
 COL_PODR,
 COL_KOMENT,
 COL_KTO,
 NUM_COLUMNS
};

class  spis_opl_data
 {
  public:
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
  short     metka_f3;
  
  class l_spis_opl_rek *poi;
  iceb_u_str zapros;

  short dn,mn,gn;
  short dk,mk,gk;
  double suma_zap; //Сумма всех найденных записей  
  
  short dd,md,gd; //Дата документа

  //Реквизиты выбранной записи
  class iceb_u_str kodkl_v;
  class iceb_u_str nk_v;
  class iceb_u_str datz_v;
  class iceb_u_str vrem_v;
  class iceb_u_str kodz_v;
  class iceb_u_str suma_v;
  class iceb_u_str nomd_v;
  class iceb_u_str datd_v;
  class iceb_u_str podr_v;
  
    
  
  //Конструктор
  spis_opl_data()
   {
    metka_f3=0;
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    dn=mn=gn=dk=mk=gk=0;
    suma_zap=0.;
   }      
 };

gboolean   spis_opl_key_press(GtkWidget *widget,GdkEventKey *event,class spis_opl_data *data);
void spis_opl_vibor(GtkTreeSelection *selection,class spis_opl_data *data);
void spis_opl_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class spis_opl_data *data);
void  spis_opl_knopka(GtkWidget *widget,class spis_opl_data *data);
void spis_opl_add_columns (GtkTreeView *treeview);
void spis_opl_create_list(class spis_opl_data *data);
int  spis_opl_prov_row(SQL_str row,class spis_opl_data *data);

void spis_opl_rasp(class spis_opl_data *data);

extern SQL_baza	bd;
extern char *name_system;
extern char     *organ;
 
int l_spis_opl(class l_spis_opl_rek *datap,int metka_f3,GtkWidget *wpredok)
{
spis_opl_data data;
char bros[300];
GdkColor color;

data.poi=datap;
data.metka_f3=metka_f3;

iceb_u_rsdat(&data.dd,&data.md,&data.gd,data.poi->datad.ravno(),1);

if(data.poi->datan.getdlinna() > 1)
  iceb_rsdatp(&data.dn,&data.mn,&data.gn,data.poi->datan.ravno(),&data.dk,&data.mk,&data.gk,data.poi->datak.ravno(),wpredok);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Список оплат"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(spis_opl_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список оплат"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");
/************
PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_modify_font(GTK_WIDGET(data.label_ost),font_pango);
pango_font_description_free(font_pango);
***************/

gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),-1,200);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(spis_opl_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбарнную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);


sprintf(bros,"F5 %s",gettext("Печатать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(spis_opl_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);



sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(spis_opl_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

spis_opl_create_list(&data);
gtk_widget_show(data.window);

//if(metka == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
if(data.metka_f3 == 0)
 gtk_widget_hide(data.knopka[FK3]);

gtk_main();


printf("l_spis_opl end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  spis_opl_knopka(GtkWidget *widget,class spis_opl_data *data)
{
char strsql[500];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("spis_opl_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {

  case FK3:
    if(data->kolzap <= 0)
     return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
      return;
  
    sprintf(strsql,"delete from Restkas where kodkl='%s' and nk=%d and datz='%s' and vrem='%s' and \
kodz=%d and suma=%s and nomd='%s' and datd='%s' and podr=%d",
    data->kodkl_v.ravno(),
    data->nk_v.ravno_atoi(),
    data->datz_v.ravno_sqldata(),
    data->vrem_v.ravno(),
    data->kodz_v.ravno_atoi(),
    data->suma_v.ravno(),
    data->nomd_v.ravno(),
    data->datd_v.ravno_sqldata(),
    data->podr_v.ravno_atoi());
    
    printf("%s\n",strsql);
    
    iceb_sql_zapis(strsql,0,0,data->window);

    spis_opl_create_list(data);
          
    return;  



  case FK5:
   spis_opl_rasp(data);
    return;  

    
  case FK10:
//    printf("spis_opl_knopka F10\n");
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   spis_opl_key_press(GtkWidget *widget,GdkEventKey *event,class spis_opl_data *data)
{
iceb_u_str repl;
//printf("spis_opl_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
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
    printf("spis_opl_key_press-Нажата клавиша Shift\n");

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
void spis_opl_create_list (class spis_opl_data *data)
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

printf("spis_opl_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(spis_opl_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(spis_opl_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_INT);

sprintf(strsql,"select * from Restkas");
if(data->poi->datan.getdlinna() > 1)
sprintf(strsql,"select * from Restkas where \
(datz = '%04d-%02d-%02d' and vrem >= '%s') or (datz = '%04d-%02d-%02d' and vrem <= '%s') \
or (datz > '%04d-%02d-%02d' and datz < '%04d-%02d-%02d')",
data->gn,data->mn,data->dn,data->poi->vremn.ravno_time(),
data->gk,data->mk,data->dk,data->poi->vremk.ravno_time_end(),
data->gn,data->mn,data->dn,
data->gk,data->mk,data->dk);

strcat(strsql," order by datz asc");

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
data->suma_zap=0.;
double suma=0.;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  if(spis_opl_prov_row(row,data) != 0)
    continue;


  //Код клиента
  ss[COL_KODKL].new_plus(iceb_u_toutf(row[0]));

  //Номер кассы
  if(row[1][0] != '0')
    ss[COL_KASSA].new_plus(row[1]);
  else
    ss[COL_KASSA].new_plus("");

  //Дата записи
  iceb_u_rsdat(&d,&m,&g,row[2],2);
  sprintf(strsql,"%02d.%02d.%04d",d,m,g);  
  ss[COL_DATAZ].new_plus(strsql);

  //Время записи
  ss[COL_VREMZ].new_plus(row[3]);

  //Сумма
  ss[COL_SUMA].new_plus(row[5]);
  suma=atof(row[5]);
  data->suma_zap+=suma;

  
  //Метка
  if(row[4][0] == '0')
    ss[COL_METKAZ].new_plus(iceb_u_toutf(VNOS_NAL));
  if(row[4][0] == '1')
    ss[COL_METKAZ].new_plus(iceb_u_toutf(VNOS_BNAL));
  if(row[4][0] == '2')
    ss[COL_METKAZ].new_plus(iceb_u_toutf(SPISANIE));

  //Номер документа
  ss[COL_NOMDOK].new_plus(row[6]);

  //Дата документа
  iceb_u_rsdat(&d,&m,&g,row[7],2);
  sprintf(strsql,"%02d.%02d.%04d",d,m,g);  
  if(d != 0)
   ss[COL_DATAD].new_plus(strsql);
  else
   ss[COL_DATAD].new_plus("");
  
  //Подразделение
  if(row[8][0] != '0')
   ss[COL_PODR].new_plus(row[8]);
  else
   ss[COL_PODR].new_plus("");

  //Кто записал
  ss[COL_KTO].new_plus(iceb_u_kszap(row[9],0));

  //Коментарий 
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[10]));

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KODKL,ss[COL_KODKL].ravno(),
  COL_KASSA,ss[COL_KASSA].ravno(),
  COL_DATAZ,ss[COL_DATAZ].ravno(),
  COL_VREMZ,ss[COL_VREMZ].ravno(),
  COL_METKAZ,ss[COL_METKAZ].ravno(),
  COL_SUMA,ss[COL_SUMA].ravno(),
  COL_NOMDOK,ss[COL_NOMDOK].ravno(),
  COL_DATAD,ss[COL_DATAD].ravno(),
  COL_PODR,ss[COL_PODR].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

spis_opl_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  if(data->metka_f3 > 0)
   gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  if(data->metka_f3 > 0)
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

if(data->kolzap > 0)
 {
  if(data->snanomer  > data->kolzap-1)
     data->snanomer= data->kolzap-1;
  if(data->snanomer  < 0)
     data->snanomer=0;
      
  printf("poz=%d\n",data->snanomer);

  sprintf(strsql,"%d",data->snanomer);
  GtkTreePath *path=gtk_tree_path_new_from_string(strsql);

//  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,TRUE,0,0);
  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

  gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
  gtk_tree_path_free(path);
//  data->snanomer=-2;
 }

// printf("Формирование заголовка с реквизитами поиска.\n");
iceb_u_str strpoi;  

strpoi.new_plus(gettext("Поиск"));
strpoi.plus(" !!!");

iceb_str_poisk(&strpoi,data->poi->datan.ravno(),gettext("Дата начала"));
iceb_str_poisk(&strpoi,data->poi->vremn.ravno(),gettext("Время начала"));
iceb_str_poisk(&strpoi,data->poi->datak.ravno(),gettext("Дата конца"));
iceb_str_poisk(&strpoi,data->poi->vremk.ravno(),gettext("Время конца"));
iceb_str_poisk(&strpoi,data->poi->kodkl.ravno(),gettext("Код клиента"));
iceb_str_poisk(&strpoi,data->poi->kassa.ravno(),gettext("Касса"));
iceb_str_poisk(&strpoi,data->poi->suma.ravno(),gettext("Сумма"));
iceb_str_poisk(&strpoi,data->poi->nomdok.ravno(),gettext("Номер документа"));
iceb_str_poisk(&strpoi,data->poi->datad.ravno(),gettext("Дата док-та"));
iceb_str_poisk(&strpoi,data->poi->podr.ravno(),gettext("Подразделение"));
iceb_str_poisk(&strpoi,data->poi->koment.ravno(),gettext("Коментарий"));
if(data->poi->metka_nal == 1)
  strpoi.ps_plus(gettext("Оплату наличкой - не показывать"));
if(data->poi->metka_bnal == 1)
  strpoi.ps_plus(gettext("Безналичную оплату - не показывать"));
if(data->poi->metka_spis == 1)
  strpoi.ps_plus(gettext("Списание - не показывать"));
gtk_label_set_text(GTK_LABEL(data->label_poisk),strpoi.ravno_toutf());
gtk_widget_show(data->label_poisk);

iceb_u_str zagolov;
zagolov.new_plus(gettext("Список оплат"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

sprintf(strsql," %s:%.2f",gettext("Сумма всех записей"),data->suma_zap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void spis_opl_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

printf("spis_opl_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код клиента"), renderer,"text", COL_KODKL,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Касса"), renderer,"text", COL_KASSA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата"), renderer,"text", COL_DATAZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Время"), renderer,"text", COL_VREMZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Форма оплати"), renderer,"text", COL_METKAZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Ном.док."), renderer,"text", COL_NOMDOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата док-та"), renderer,"text", COL_DATAD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Подразделение"), renderer,"text", COL_PODR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,"text", COL_KTO,NULL);

printf("spis_opl_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void spis_opl_vibor(GtkTreeSelection *selection,class spis_opl_data *data)
{
printf("spis_opl_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;

if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gint  nomer;
gchar *kodkl_v;
gchar *nk_v;
gchar *datz_v;
gchar *vrem_v;
gchar *kodz_v;
gchar *suma_v;
gchar *nomd_v;
gchar *datd_v;
gchar *podr_v;


gtk_tree_model_get(model,&iter,
COL_KODKL,&kodkl_v,
COL_KASSA,&nk_v,
COL_DATAZ,&datz_v,
COL_VREMZ,&vrem_v,
COL_METKAZ,&kodz_v,
COL_SUMA,&suma_v,
COL_NOMDOK,&nomd_v,
COL_DATAD,&datd_v,
COL_PODR,&podr_v,
NUM_COLUMNS,&nomer,-1);

data->kodkl_v.new_plus(iceb_u_fromutf(kodkl_v));
data->nk_v.new_plus(iceb_u_fromutf(nk_v));
data->datz_v.new_plus(iceb_u_fromutf(datz_v));
data->vrem_v.new_plus(iceb_u_fromutf(vrem_v));
/************
  //Метка
  if(row[4][0] == '0')
    ss[COL_METKAZ].new_plus(iceb_u_toutf(VNOS_NAL));
  if(row[4][0] == '1')
    ss[COL_METKAZ].new_plus(iceb_u_toutf(VNOS_BNAL));
  if(row[4][0] == '2')
    ss[COL_METKAZ].new_plus(iceb_u_toutf(SPISANIE));
*************/
if(iceb_u_SRAV(iceb_u_fromutf(kodz_v),VNOS_NAL,0) == 0)
  data->kodz_v.new_plus("0");
if(iceb_u_SRAV(iceb_u_fromutf(kodz_v),VNOS_BNAL,0) == 0)
  data->kodz_v.new_plus("1");
if(iceb_u_SRAV(iceb_u_fromutf(kodz_v),SPISANIE,0) == 0)
  data->kodz_v.new_plus("2");
  
data->suma_v.new_plus(iceb_u_fromutf(suma_v));
data->nomd_v.new_plus(iceb_u_fromutf(nomd_v));
data->datd_v.new_plus(iceb_u_fromutf(datd_v));
data->podr_v.new_plus(iceb_u_fromutf(podr_v));

data->snanomer=nomer;
 
g_free(kodkl_v);
g_free(nk_v);
g_free(datz_v);
g_free(vrem_v);
g_free(kodz_v);
g_free(suma_v);
g_free(nomd_v);
g_free(datd_v);
g_free(podr_v);

//printf("spis_opl_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);
}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void spis_opl_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class spis_opl_data *data)
{
//printf("spis_opl_v_row\n");
//printf("spis_opl_v_row корректировка\n");
/******************
if(data->metka_rr == 0)
 {
  gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
  return;
 }
gtk_widget_destroy(data->window);

data->metka_voz=0;
**********************/

}
/**********************************/
/*Распечатать список*/
/****************************/

void spis_opl_rasp(class spis_opl_data *data)
{
char strsql[300];
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

sprintf(strsql,"spis_opl%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список оплат"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }
iceb_u_startfil(ff);

fprintf(ff,"\x1B\x4D"); /*12-знаков*/
fprintf(ff,"\x12");//Ужатый режим

iceb_u_zagolov(gettext("Список оплат"),data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,organ,ff);

if(data->poi->datan.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Дата начала"),data->poi->datan.ravno());

if(data->poi->vremn.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Время начала"),data->poi->vremn.ravno());

if(data->poi->datak.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Дата конца"),data->poi->datak.ravno());

if(data->poi->vremk.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Время конца"),data->poi->vremk.ravno());

if(data->poi->kodkl.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код клиента"),data->poi->kodkl.ravno());

if(data->poi->kassa.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Касса"),data->poi->kassa.ravno());

if(data->poi->suma.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Сумма"),data->poi->suma.ravno());

if(data->poi->nomdok.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Номер документа"),data->poi->nomdok.ravno());

if(data->poi->datad.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Дата документа"),data->poi->datad.ravno());

if(data->poi->podr.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Подразделение"),data->poi->podr.ravno());

if(data->poi->koment.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Коментарий"),data->poi->koment.ravno());

if(data->poi->metka_nal == 1)
 fprintf(ff,"%s\n",gettext("Оплату наличными - не показывать"));

if(data->poi->metka_bnal == 1)
 fprintf(ff,"%s\n",gettext("Безналичную оплату - не показывать"));
 

if(data->poi->metka_spis == 1)
 fprintf(ff,"%s\n",gettext("Списание - не показывать"));

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
Код клиента|Касса|   Дата   | Время  |Ф-ма о.|  Сумма   |Номер док-тa|Дата д-та |Под-ние|Коментарий|Кто записал|\n");
/*
12345678901 12345 1234567890 12345678 1234567 1234567890 123456789012 1234567890 1234567 1234567890
*/  
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;

short dz,mz,gz;
short dd,md,gd;
double itogo=0.;
char data_dok[20];
char podr[20];
char kassa[20];

while(cur.read_cursor(&row) != 0)
 {
  if(spis_opl_prov_row(row,data) != 0)
    continue;

  iceb_u_rsdat(&dz,&mz,&gz,row[2],2);
  iceb_u_rsdat(&dd,&md,&gd,row[7],2);

  memset(data_dok,'\0',sizeof(data_dok));

  if(dd != 0)
   sprintf(data_dok,"%02d.%02d.%d",dd,md,gd);

  //Метка
  if(row[4][0] == '0')
    s5.new_plus(VNOS_NAL);
  if(row[4][1] == '1')
      s5.new_plus(VNOS_BNAL);
  if(row[4][2] == '2')
    s5.new_plus(SPISANIE);

  itogo+=atof(row[5]);

  memset(podr,'\0',sizeof(podr));
  if(row[8][0] != '0')
   strncpy(podr,row[8],sizeof(podr)-1);
  memset(kassa,'\0',sizeof(kassa));
  if(row[1][0] != '0')
   strncpy(kassa,row[1],sizeof(kassa)-1);
  //Кто записал
  s6.new_plus(iceb_u_kszap(row[9],1));

  fprintf(ff,"%-11s %-5s %02d.%02d.%04d %-8s %-7.7s %10.2f %-12s %-10s %-7s %-10s %s\n",
  row[0],kassa,dz,mz,gz,row[3],s5.ravno(),atof(row[5]),row[6],data_dok,podr,row[10],s6.ravno());

 }
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%45s:%10.2f\n",gettext("Итого"),itogo);

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);

iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
/**********************************/
/*Проверка на условия поиска*/
/***************************/

int  spis_opl_prov_row(SQL_str row,class spis_opl_data *data)
{
//Не показывать наличную оплату
if(data->poi->metka_nal == 1)
 if(atoi(row[4]) == 0)
  return(1);

//Не показывать безналичную оплату
if(data->poi->metka_bnal == 1)
 if(atoi(row[4]) == 1)
  return(1);

//Не показывать списание
if(data->poi->metka_spis == 1)
 if(atoi(row[4]) == 2)
  return(1);

//Поиск образца в строке
if(data->poi->koment.getdlinna() > 1)
 if(iceb_u_strstrm(row[10],data->poi->koment.ravno()) == 0)
   return(1);

if(iceb_u_proverka(data->poi->podr.ravno(),row[8],0,0) != 0)
 return(1);
if(iceb_u_proverka(data->poi->kodkl.ravno(),row[0],0,0) != 0)
 return(1);
if(iceb_u_proverka(data->poi->kassa.ravno(),row[1],0,0) != 0)
 return(1);
if(iceb_u_proverka(data->poi->nomdok.ravno(),row[6],0,0) != 0)
 return(1);

if(data->dd != 0)
 {
  short dd=0,md=0,gd=0;
  iceb_u_rsdat(&dd,&md,&gd,row[7],2);
  if(iceb_u_sravmydat(dd,md,gd,data->dd,data->md,data->gd) != 0)
   return(1);
 }
if(fabs(data->poi->suma.ravno_atof()) > 0.009)
 {
  if(fabs(data->poi->suma.ravno_atof()-atof(row[5])) > 0.009)
   return(1);
 }
return(0);
}
