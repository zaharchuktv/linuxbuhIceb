/*$Id: l_uosspdok.c,v 1.20 2011-09-07 11:44:20 sasa Exp $*/
/*16.12.2009	30.10.2007	Белых А.И.	l_uosspdok.c
Просмотр списка документов
*/

#include <stdlib.h>
#include <unistd.h>
#include "buhg_g.h"
#include "l_uosspdok.h"
enum
{
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 FK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_METKAZ,
 COL_DATAD,
 COL_NOMDOK,
 COL_KONTR,
 COL_KODOP,
 COL_PODR,
 COL_MAT_OT,
 COL_NOMPD,
 COL_NOMNN,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  uosspdok_data
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

  class uosspdok_rek poi;
  iceb_u_str zapros;

  short ds,ms,gs; //Стартовая дата просмотра
  
  short dn,mn,gn;
  short dk,mk,gk;
  
  iceb_u_str datav;
  iceb_u_str nomdokv;

  //Конструктор
  uosspdok_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    dn=mn=gn=dk=mk=gk=0;
   }      
 };

gboolean   uosspdok_key_press(GtkWidget *widget,GdkEventKey *event,class uosspdok_data *data);
void uosspdok_vibor(GtkTreeSelection *selection,class uosspdok_data *data);
void uosspdok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class uosspdok_data *data);
void  uosspdok_knopka(GtkWidget *widget,class uosspdok_data *data);
void uosspdok_add_columns (GtkTreeView *treeview);
void uosspdok_create_list(class uosspdok_data *data);
int  uosspdok_prov_row(SQL_str row,class uosspdok_data *data);

int l_uosspdok_p(class uosspdok_rek *datap,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;
extern char     *organ;
extern short startgoduos;
 
void l_uosspdok(short ds,short ms,short gs, //Дата начала просмотра
GtkWidget *wpredok)
{
class uosspdok_data data;
char bros[512];
GdkColor color;


data.ds=data.dn=ds;
data.ms=data.mn=ms;
data.gs=data.gn=gs;
if(data.gn == 0)
 data.gn=startgoduos;
  
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Список документов"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uosspdok_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список документов"));


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


sprintf(bros,"F2 ?");
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(uosspdok_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Включение/выключение показа только не подтверждённых документов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"SF2 %s",gettext("Метка"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(uosspdok_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Снятие метки неподтверждённого документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 *");
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(uosspdok_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Включение/выключение показа документов без проводок"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"SF3 %s",gettext("Метка"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(uosspdok_knopka),&data);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Снятие метки документа без проводок"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(uosspdok_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Просмотр"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(uosspdok_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Просмотр выбранного документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F9 %s",gettext("Оплата"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(uosspdok_knopka),&data);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Переключение режимов просмотра (только оплаченыые, только не оплаченные, все)"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(uosspdok_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

uosspdok_create_list(&data);
gtk_widget_show(data.window);

//if(metka == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


//printf("l_uosspdok end\n");

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosspdok_knopka(GtkWidget *widget,class uosspdok_data *data)
{
char strsql[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
data->kl_shift=0; //Сбрасываем нажатый сшифт 

switch (knop)
 {
  case FK2:
    if(data->poi.metka_pros == 1)
      data->poi.metka_pros=0;
    else
      data->poi.metka_pros=1;  
     
    uosspdok_create_list(data);
    return;  

  case SFK2:
    if(iceb_menu_danet(gettext("Снять отметку неподтвеждённого документа ? Вы уверены ?"),2,data->window) == 2)
     return;     
    
    sprintf(strsql,"update Uosdok set podt=1 where datd='%s' and nomd='%s'",
    data->datav.ravno_sqldata(),data->nomdokv.ravno());

    iceb_sql_zapis(strsql,0,0,data->window);

    uosspdok_create_list(data);
    return;  

  case FK3:
    if(data->poi.metka_pros == 2)
      data->poi.metka_pros=0;
    else
      data->poi.metka_pros=2;
    uosspdok_create_list(data);
    return;  

  case SFK3:
    if(iceb_menu_danet(gettext("Снять отметку документа без проводок ? Вы уверены ?"),2,data->window) == 2)
     return;     
    
    sprintf(strsql,"update Uosdok set prov=0 where datd='%s' and nomd='%s'",
    data->datav.ravno_sqldata(),data->nomdokv.ravno());

    iceb_sql_zapis(strsql,0,0,data->window);
    uosspdok_create_list(data);
    return;  

  case FK4:
    l_uosspdok_p(&data->poi,data->window);
    
    if(data->poi.metka_poi == 1)
     {
      if(data->poi.datan.getdlinna() > 1)
       iceb_rsdatp(&data->dn,&data->mn,&data->gn,data->poi.datan.ravno(),\
       &data->dk,&data->mk,&data->gk,data->poi.datak.ravno(),data->window);
     }
    else
     {
      data->dn=data->ds;
      data->mn=data->ms;
      data->gn=data->gs;
      
     }
    uosspdok_create_list(data);
    return;  

  case FK5:
    l_uosdok(data->datav.ravno(),data->nomdokv.ravno(),data->window);
    /*выставляются метки на документе нужно обязательно перечитать*/
    uosspdok_create_list(data);
    return;  

  case FK9:
    data->poi.metka_opl++;
    if(data->poi.metka_opl > 2)
     data->poi.metka_opl=0;    
    uosspdok_create_list(data);
    return;  
    
  case FK10:
//    printf("uosspdok_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosspdok_key_press(GtkWidget *widget,GdkEventKey *event,class uosspdok_data *data)
{
iceb_u_str repl;
//printf("uosspdok_key_press keyval=%d state=%d\n",event->keyval,event->state);


switch(event->keyval)
 {
  case GDK_F2:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
    return(TRUE);

  case GDK_F3:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK3]),"clicked");
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
    printf("uosspdok_key_press-Нажата клавиша Shift\n");

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
void uosspdok_create_list (class uosspdok_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
char bros[500];
int  kolstr=0;
SQL_str row;
iceb_u_str zagolov;

//printf("uosspdok_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(uosspdok_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(uosspdok_vibor),data);

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

sprintf(bros,"select datd,tipz,kodop,kontr,nomd,podr,nomdv,podt,prov,ktoz,vrem,kodol,mo,nomnn from Uosdok");

sprintf(strsql,"%s where datd >= '%04d-01-01' order by datd asc",bros,data->gn);

if(data->dn != 0)
  sprintf(strsql,"%s where datd >= '%04d-%02d-%02d' order by datd asc",bros,data->gn,data->mn,data->dn);

if(data->dn != 0 && data->dk != 0)
  sprintf(strsql,"%s where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' \
order by datd asc",bros,data->gn,data->mn,data->dn,data->gk,data->mk,data->dk);

data->zapros.new_plus(strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;

float kolstr1=0.;
iceb_refresh();
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
   if(uosspdok_prov_row(row,data) != 0)
    continue;

  //Метка записи
  if(row[1][0] == '1')
   ss[COL_METKAZ].new_plus("+");
  else
   ss[COL_METKAZ].new_plus("-");

  if(row[7][0] == '0')
   ss[COL_METKAZ].plus("?");
 
  if(row[8][0] == '1')
   ss[COL_METKAZ].plus("*");
  if(row[12][0] == '0')
   ss[COL_METKAZ].plus("$");

  //Дата документа
  ss[COL_DATAD].new_plus(iceb_u_sqldata(row[0]));
  
  //Номер документа
  ss[COL_NOMDOK].new_plus(iceb_u_toutf(row[4]));

  //Контрагент
  ss[COL_KONTR].new_plus(iceb_u_toutf(row[3]));

  //Код операции
  ss[COL_KODOP].new_plus(iceb_u_toutf(row[2]));

  //код подразделения
  ss[COL_PODR].new_plus(iceb_u_toutf(row[5]));

  //Номер накладной парного документа
  ss[COL_NOMPD].new_plus(iceb_u_toutf(row[6]));

  //Номер накладной парного документа
  ss[COL_NOMNN].new_plus(iceb_u_toutf(row[13]));

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[10])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[9],0,data->window));


 /*Код материально-ответственного*/
  ss[COL_MAT_OT].new_plus(iceb_u_toutf(row[11]));
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKAZ,ss[COL_METKAZ].ravno(),
  COL_DATAD,ss[COL_DATAD].ravno(),
  COL_NOMDOK,ss[COL_NOMDOK].ravno(),
  COL_KONTR,ss[COL_KONTR].ravno(),
  COL_KODOP,ss[COL_KODOP].ravno(),
  COL_PODR,ss[COL_PODR].ravno(),
  COL_MAT_OT,ss[COL_MAT_OT].ravno(),
  COL_NOMPD,ss[COL_NOMPD].ravno(),
  COL_NOMNN,ss[COL_NOMNN].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

uosspdok_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
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

iceb_u_str strpoi;  
strpoi.plus("");

if(data->poi.metka_pros == 1)
 strpoi.new_plus(gettext("Просмотр только неподтверждённых документов"));
if(data->poi.metka_pros == 2)
 strpoi.new_plus(gettext("Просмотр только документов без проводок"));
 
if(data->poi.metka_poi == 1 || data->poi.metka_opl != 0)
 {

// printf("Формирование заголовка с реквизитами поиска.\n");
  if(data->poi.metka_pros != 0)      
   strpoi.ps_plus(gettext("Поиск"));
  else
   strpoi.new_plus(gettext("Поиск"));
  strpoi.plus(" !!!");

  iceb_str_poisk(&strpoi,data->poi.nomdok.ravno(),gettext("Номер документа"));
  iceb_str_poisk(&strpoi,data->poi.datan.ravno(),gettext("Дата начала"));
  iceb_str_poisk(&strpoi,data->poi.datak.ravno(),gettext("Дата конца"));
  iceb_str_poisk(&strpoi,data->poi.kontr.ravno(),gettext("Код контрагента"));
  iceb_str_poisk(&strpoi,data->poi.kodop.ravno(),gettext("Код операции"));
  iceb_str_poisk(&strpoi,data->poi.kodpod.ravno(),gettext("Код подразделения"));
  iceb_str_poisk(&strpoi,data->poi.mat_ot.ravno(),gettext("Материально-ответственный"));
  if(data->poi.pr_ras.ravno_pr() == 1)
    strpoi.ps_plus(gettext("Только приходы"));
  if(data->poi.pr_ras.ravno_pr() == 2)
    strpoi.ps_plus(gettext("Только расходы"));

  if(data->poi.metka_opl == 1)
   strpoi.ps_plus(gettext("Только оплаченные"));
  if(data->poi.metka_opl == 2)
   strpoi.ps_plus(gettext("Только неоплаченные"));
    
 }

if(strpoi.getdlinna() > 1)
 {
  gtk_label_set_text(GTK_LABEL(data->label_poisk),strpoi.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
  gtk_widget_hide(data->label_poisk); 

zagolov.new_plus(gettext("Список документов"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void uosspdok_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

//printf("uosspdok_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Метка"), renderer,"text", COL_METKAZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата док-та"), renderer,"text",COL_DATAD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Номер документа"), renderer,"text", COL_NOMDOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Контрагент"), renderer,"text", COL_KONTR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Операция"), renderer,"text", COL_KODOP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Подразделение"), renderer,"text", COL_PODR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Мат.о."), renderer,"text", COL_MAT_OT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Номер пар.док."), renderer,"text", COL_NOMPD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Номер нал.нак."), renderer,"text", COL_NOMNN,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

//printf("uosspdok_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void uosspdok_vibor(GtkTreeSelection *selection,class uosspdok_data *data)
{
//printf("uosspdok_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *datad;
gchar *nomdok;

gint  nomer;


gtk_tree_model_get(model,&iter,COL_DATAD,&datad,COL_NOMDOK,&nomdok,NUM_COLUMNS,&nomer,-1);

data->datav.new_plus(datad);
data->nomdokv.new_plus(iceb_u_fromutf(nomdok));
 
data->snanomer=nomer;

g_free(datad);
g_free(nomdok);

//printf("uosspdok_vibor-%s %s %d\n",data->datav.ravno(),data->nomdokv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void uosspdok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class uosspdok_data *data)
{
//просмотр выбранного документа
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");


}
/****************************/
/*Проверка записей          */
/*****************************/

int  uosspdok_prov_row(SQL_str row,class uosspdok_data *data)
{

int metka_opl=atoi(row[12]);

if(data->poi.metka_opl == 1 && metka_opl != 1)
  return(1);
if(data->poi.metka_opl == 2 && metka_opl != 0)
  return(1);

short pro=atoi(row[8]);

if(data->poi.metka_pros == 2 && pro == 0)
   return(1);
  
int pod=atoi(row[7]);
if(data->poi.metka_pros == 1 && pod == 1)
   return(1);


if(data->poi.metka_poi == 0)
 return(0);
 
if(iceb_u_proverka(data->poi.kontr.ravno(),row[3],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->poi.kodop.ravno(),row[2],0,0) != 0)
 return(1);


if(iceb_u_proverka(data->poi.kodpod.ravno(),row[5],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->poi.mat_ot.ravno(),row[11],0,0) != 0)
 return(1);

if(data->poi.pr_ras.ravno_pr() != 0 && data->poi.pr_ras.ravno_pr() != atoi(row[1]))
 return(1);

if(data->poi.nomdok.getdlinna() > 1)
 if(iceb_u_SRAV(data->poi.nomdok.ravno(),row[4],1) != 0)
  if(iceb_u_SRAV(data->poi.nomdok.ravno(),row[6],1) != 0)
   return(1);



return(0);
}
