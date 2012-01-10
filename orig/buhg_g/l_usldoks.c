/*$Id: l_usldoks.c,v 1.32 2011-02-21 07:35:54 sasa Exp $*/
/*17.05.2010	07.08.2005	Белых А.И.	l_usldoks.c
Просмотр списка документов на услуги
*/
#include <stdlib.h>
#include <unistd.h>
#include "buhg_g.h"
#include "l_usldoks.h"
#include "zar_eks_ko.h"
enum
{
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 FK6,
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
 COL_NOMNALNAK,
 COL_NOMPD,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  usldoks_data
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

  class usldoks_rek poi;
  iceb_u_str zapros;

  short ds,ms,gs; //Стартовая дата просмотра
  
  short dn,mn,gn;
  short dk,mk,gk;
  
  iceb_u_str datav;
  iceb_u_str nomdokv;
  iceb_u_str podrv;
  int tipzv;  

  //Конструктор
  usldoks_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    dn=mn=gn=dk=mk=gk=0;
   }      
 };

gboolean   usldoks_key_press(GtkWidget *widget,GdkEventKey *event,class usldoks_data *data);
void usldoks_vibor(GtkTreeSelection *selection,class usldoks_data *data);
void usldoks_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class usldoks_data *data);
void  usldoks_knopka(GtkWidget *widget,class usldoks_data *data);
void usldoks_add_columns (GtkTreeView *treeview);
void usldoks_create_list(class usldoks_data *data);
int  usldoks_prov_row(SQL_str row,class usldoks_data *data);

int l_usldoks_p(class usldoks_rek *datap,GtkWidget *wpredok);
void l_usldoks_eko(class  usldoks_data *data);

extern SQL_baza	bd;
extern char *name_system;
extern char     *organ;
 
void l_usldoks(short ds,short ms,short gs, //Дата начала просмотра
GtkWidget *wpredok)
{
usldoks_data data;
char bros[512];
GdkColor color;

data.ds=data.dn=ds;
data.ms=data.mn=ms;
data.gs=data.gn=gs;

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(usldoks_key_press),&data);
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
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(usldoks_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Включение/выключение показа только не подтверждённых документов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"SF2 %s",gettext("Метка"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(usldoks_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Снятие метки неподтверждённого документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 *");
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(usldoks_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Включение/выключение показа документов без проводок"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"SF3 %s",gettext("Метка"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(usldoks_knopka),&data);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Снятие метки документа без проводок"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(usldoks_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Просмотр"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(usldoks_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Просмотр выбранного документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Экспорт"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(usldoks_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Экспорт кассовых ордеров в подсистему \"Учёт кассовых ордеров\""),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F9 %s",gettext("Оплата"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(usldoks_knopka),&data);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Переключение режимов просмотра (только оплаченыые, только не оплаченные, все)"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(usldoks_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

usldoks_create_list(&data);
gtk_widget_show(data.window);

//if(metka == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();



if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  usldoks_knopka(GtkWidget *widget,class usldoks_data *data)
{
short dd,md,gd;
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
     
    usldoks_create_list(data);
    return;  

  case SFK2:
    if(iceb_menu_danet(gettext("Снять отметку неподтвеждённого документа ? Вы уверены ?"),2,data->window) == 2)
     return;     
    sprintf(strsql,"update Usldokum \
set \
pod=1 \
where datd='%s' and podr=%d and nomd='%s' and tp=%d",
    data->datav.ravno_sqldata(),data->podrv.ravno_atoi(),data->nomdokv.ravno(),data->tipzv);

    iceb_sql_zapis(strsql,0,0,data->window);

    usldoks_create_list(data);
    return;  

  case FK3:
    if(data->poi.metka_pros == 2)
      data->poi.metka_pros=0;
    else
      data->poi.metka_pros=2;
    usldoks_create_list(data);
    return;  

  case SFK3:
    if(iceb_menu_danet(gettext("Снять отметку документа без проводок ? Вы уверены ?"),2,data->window) == 2)
     return;     
    sprintf(strsql,"update Usldokum \
set \
pro=1 \
where datd='%s' and podr=%d and nomd='%s' and tp=%d",
    data->datav.ravno_sqldata(),data->podrv.ravno_atoi(),data->nomdokv.ravno(),data->tipzv);

    iceb_sql_zapis(strsql,0,0,data->window);
    usldoks_create_list(data);
    return;  

  case FK4:
    l_usldoks_p(&data->poi,data->window);
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
    usldoks_create_list(data);
    return;  

  case FK5:
    iceb_u_rsdat(&dd,&md,&gd,data->datav.ravno(),1);
    l_usldok(dd,md,gd,data->tipzv,data->podrv.ravno_atoi(),data->nomdokv.ravno(),data->window);
    /*При выходе из документа устанавливаются метки подтверждения и выполнения проводок*/
    usldoks_create_list(data);
    return;  


  case FK6:
    l_usldoks_eko(data);
    return;  

  case FK9:
    data->poi.metka_opl++;
    if(data->poi.metka_opl > 2)
     data->poi.metka_opl=0;    
    usldoks_create_list(data);
    return;  

    
  case FK10:
//    printf("usldoks_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   usldoks_key_press(GtkWidget *widget,GdkEventKey *event,class usldoks_data *data)
{
iceb_u_str repl;
//printf("usldoks_key_press keyval=%d state=%d\n",event->keyval,event->state);


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

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
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
void usldoks_create_list (class usldoks_data *data)
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

data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(usldoks_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(usldoks_vibor),data);

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

sprintf(bros,"tp,datd,podr,kontr,nomd,nomnn,kodop,pod,pro,ktoi,nomdp,vrem,dover,nds,sumkor,sumnds,pn,mo from Usldokum");

sprintf(strsql,"select %s where datd >= '%d-01-01' order by datd asc",
bros,data->gn);

if(data->dn != 0)
  sprintf(strsql,"select %s where datd >= '%d-%02d-%02d' order by datd asc",
  bros,data->gn,data->mn,data->dn);

if(data->dn != 0 && data->dk != 0)
  sprintf(strsql,"select %s where datd >= '%d-%02d-%02d' and datd <= '%d-%02d-%02d' \
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
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
   if(usldoks_prov_row(row,data) != 0)
    continue;

  //Метка записи
  ss[COL_METKAZ].new_plus("");
  if(row[0][0] == '1')
   ss[COL_METKAZ].new_plus("+");
  else
   ss[COL_METKAZ].new_plus("-");

  if(row[7][0] == '0')
   ss[COL_METKAZ].plus("?");
  if(row[8][0] == '0')
   ss[COL_METKAZ].plus("*");
  if(row[17][0] == '0')
   ss[COL_METKAZ].plus("$");

  //Дата документа
  ss[COL_DATAD].new_plus(iceb_u_sqldata(row[1]));
  
  //Номер документа
  ss[COL_NOMDOK].new_plus(iceb_u_toutf(row[4]));

  //Контрагент
  ss[COL_KONTR].new_plus(iceb_u_toutf(row[3]));

  //Код операции
  ss[COL_KODOP].new_plus(iceb_u_toutf(row[6]));

  //код подразделения
  ss[COL_PODR].new_plus(iceb_u_toutf(row[2]));

  //Номер налоговой накладной
  ss[COL_NOMNALNAK].new_plus(iceb_u_toutf(row[5]));

  //Номер накладной парного документа
  ss[COL_NOMPD].new_plus(iceb_u_toutf(row[10]));

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[11])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[9],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKAZ,ss[COL_METKAZ].ravno(),
  COL_DATAD,ss[COL_DATAD].ravno(),
  COL_NOMDOK,ss[COL_NOMDOK].ravno(),
  COL_KONTR,ss[COL_KONTR].ravno(),
  COL_KODOP,ss[COL_KODOP].ravno(),
  COL_PODR,ss[COL_PODR].ravno(),
  COL_NOMNALNAK,ss[COL_NOMNALNAK].ravno(),
  COL_NOMPD,ss[COL_NOMPD].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

usldoks_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
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
  iceb_str_poisk(&strpoi,data->poi.naim_kontr.ravno(),gettext("Наименование контрагента"));
  iceb_str_poisk(&strpoi,data->poi.kodop.ravno(),gettext("Код операции"));
  iceb_str_poisk(&strpoi,data->poi.kodpod.ravno(),gettext("Код подразделения"));
  iceb_str_poisk(&strpoi,data->poi.nalnak.ravno(),gettext("Номер налоговой накладной"));
  iceb_str_poisk(&strpoi,data->poi.dover.ravno(),gettext("Доверенность"));

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

void usldoks_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


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
gettext("Номер нал.нак."), renderer,"text", COL_NOMNALNAK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Номер пар.док."), renderer,"text", COL_NOMPD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);


}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void usldoks_vibor(GtkTreeSelection *selection,class usldoks_data *data)
{
printf("usldoks_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *datad;
gchar *nomdok;
gchar *podr;
gchar *tipz;

gint  nomer;


gtk_tree_model_get(model,&iter,COL_DATAD,&datad,COL_NOMDOK,&nomdok,COL_PODR,&podr,
COL_METKAZ,&tipz,NUM_COLUMNS,&nomer,-1);

data->datav.new_plus(iceb_u_fromutf(datad));
data->nomdokv.new_plus(iceb_u_fromutf(nomdok));
data->podrv.new_plus(iceb_u_fromutf(podr));

if(tipz[0] == '+')
 data->tipzv=1;
else
 data->tipzv=2;
 
data->snanomer=nomer;

g_free(datad);
g_free(nomdok);
g_free(podr);
g_free(tipz);

//printf("usldoks_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void usldoks_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class usldoks_data *data)
{
//просмотр выбранного документа
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");


}
/****************************/
/*Проверка записей          */
/*****************************/

int  usldoks_prov_row(SQL_str row,class usldoks_data *data)
{

short pro=atoi(row[8]);
short metka_opl=atoi(row[17]);

if(data->poi.metka_opl == 1 && metka_opl != 1)
  return(1);
if(data->poi.metka_opl == 2 && metka_opl != 0)
  return(1);

if(data->poi.metka_pros == 2 && pro == 1)
   return(1);
  
int pod=atoi(row[7]);
if(data->poi.metka_pros == 1 && pod == 1)
   return(1);

if(data->poi.metka_poi == 0)
 return(0);
 
if(iceb_u_proverka(data->poi.kontr.ravno(),row[3],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->poi.kodop.ravno(),row[6],0,0) != 0)
 return(1);


if(iceb_u_proverka(data->poi.kodpod.ravno(),row[2],0,0) != 0)
 return(1);


if(data->poi.pr_ras.ravno_pr() != 0 && data->poi.pr_ras.ravno_pr() != atoi(row[0]))
 return(1);

if(data->poi.nomdok.getdlinna() > 1)
 if(iceb_u_SRAV(data->poi.nomdok.ravno(),row[4],1) != 0)
  if(iceb_u_SRAV(data->poi.nomdok.ravno(),row[11],1) != 0)
   return(1);


if(data->poi.naim_kontr.getdlinna() > 1)
 {
  SQLCURSOR curr;
  char strsql[512];
  SQL_str row1;
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[3]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1 )
   if(iceb_u_strstrm(row1[0],data->poi.naim_kontr.ravno()) == 0)
     return(1);
   
 }

if(iceb_u_proverka(data->poi.nalnak.ravno(),row[5],4,0) != 0)
  return(1);

if(iceb_u_proverka(data->poi.dover.ravno(),row[12],4,0) != 0)
  return(1);
  
return(0);
}



/******************************/
/*Экспорт кассовых ордеров*/
/****************************/
void l_usldoks_eko(class  usldoks_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

SQL_str row;
class SQLCURSOR cur;
char strsql[512];
char shetk[32];
char naimop[512];
class zar_eks_ko_rek rek;

  
if(eks_ko_v(&rek,data->window) != 0)
 return;
//Проверяем код операции
if(rek.prn == 1)
 sprintf(strsql,"select naik,shetkor from Kasop1 where kod='%s'",rek.kodop.ravno());
if(rek.prn == 2)
 sprintf(strsql,"select naik,shetkor from Kasop2 where kod='%s'",rek.kodop.ravno());

if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),rek.kodop.ravno());
  iceb_menu_soob(strsql,data->window);
  return;
 }

memset(naimop,'\0',sizeof(naimop));
strncpy(naimop,row[0],sizeof(naimop)-1);
strncpy(shetk,row[1],sizeof(shetk)-1);
short dko=0,mko=0,gko=0;
//Проверяем дату
iceb_u_rsdat(&dko,&mko,&gko,rek.data.ravno(),1);

short vt=0;
iceb_poldan("Код группы возвратная тара",strsql,"matnast.alx",data->window);
vt=atoi(strsql);

class iceb_u_spisok kod_kontr;
class iceb_u_double suma_kontr;



class iceb_u_str nomd("");

double suma=0.;
double sumabn=0.;
double sumkor=0.;
short d,m,g;
int lnds=0;
int kolih_kasord=0;
double suma_nds=0.;
double suma_dok=0.;
float pnds=0.;
int tipz_usl=0;
if(rek.prn == 1)
 tipz_usl=2;
else
 tipz_usl=1;
int nomer_kontr=0;
/************
tp,datd,podr,kontr,nomd,nomnn,kodop,pod,pro,ktoi,nomdp,vrem,dover,nds,sumkor,sumnds,pn
***************/
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(rek.prn == atoi(row[0])) /*Если приходный кассовый ордер то нам нужны расходные документы и наооборот*/
   continue;
  
  if(usldoks_prov_row(row,data) != 0)
    continue;
  
  iceb_u_rsdat(&d,&m,&g,row[1],2);

  pnds=atof(row[16]);

  /*Узнаем НДС документа*/
  lnds=atoi(row[13]);

  suma_nds=0.;
  if(lnds == 0)
   {
    suma_nds=atof(row[15]);
   }

  sumzap1uw(d,m,g,row[4],atoi(row[3]),&suma,&sumabn,lnds,tipz_usl,data->window);

  if(lnds == 0 && suma_nds == 0.)
    suma_nds=(suma+sumkor)*pnds/100.;
  
/*  printw("suma=%f sumabn=%f sumkor=%f\n",suma,sumabn,sumkor);*/
  suma_dok=suma+sumkor+suma_nds+sumabn;
    


  if(rek.metka_r == 0)
   {
    kolih_kasord++;
    iceb_lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
    iceb_nomnak(gko,rek.kassa.ravno(),&nomd,rek.prn,2,1,data->window);
    zaphkorw(0,rek.kassa.ravno(),rek.prn,dko,mko,gko,nomd.ravno(),shetk,rek.kodop.ravno(),naimop,"","","","","",0,0,0,"","",data->window);
    printf("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());

    zapzkorw(rek.kassa.ravno(),rek.prn,dko,mko,gko,nomd.ravno(),row[4],suma_dok,"","",data->window);
   }
  else
   {
    if((nomer_kontr=kod_kontr.find_r(row[3])) < 0)
      kod_kontr.plus(row[3]);

    suma_kontr.plus(suma_dok,nomer_kontr);


   }  

 }

if(rek.metka_r == 1)
 {
  kolih_kasord++;
  iceb_lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
  iceb_nomnak(gko,rek.kassa.ravno(),&nomd,rek.prn,2,1,data->window);
  zaphkorw(0,rek.kassa.ravno(),rek.prn,dko,mko,gko,nomd.ravno(),shetk,rek.kodop.ravno(),naimop,"","","","","",0,0,0,"","",data->window);
  printf("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());

  for(nomer_kontr=0; nomer_kontr < kod_kontr.kolih(); nomer_kontr++)
   {
    
    zapzkorw(rek.kassa.ravno(),rek.prn,dko,mko,gko,nomd.ravno(),kod_kontr.ravno(nomer_kontr),suma_kontr.ravno(nomer_kontr),"","",data->window);
   }
 }

sprintf(strsql,"%s:%d",gettext("Количество экспортированых кассовых ордеров"),kolih_kasord);
iceb_menu_soob(strsql,data->window);

}




