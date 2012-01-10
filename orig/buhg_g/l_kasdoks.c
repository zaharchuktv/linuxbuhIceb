/*$Id: l_kasdoks.c,v 1.27 2011-02-21 07:35:52 sasa Exp $*/
/*14.09.2009	24.01.2006	Белых А.И.	l_kasdoks.c
Работа со списком кассовых ордеров
*/
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "buhg_g.h"
#include "l_kasdoks.h"
enum
{
 FK2,
 SFK2,
 FK3,
 SFK3,
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
 COL_METKAZ,
 COL_KASSA,
 COL_DATAD,
 COL_NOMDOK,
 COL_SHET,
 COL_KODOP,
 COL_KONTR,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  kasdoks_data
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

  class kasdoks_rek poi;
  iceb_u_str zapros;

  short ds,ms,gs; //Стартовая дата просмотра
  
  short dn,mn,gn;
  short dk,mk,gk;
  
  iceb_u_str datav;
  iceb_u_str nomdokv;
  iceb_u_str kassav;
  int tipzv;  

  //Конструктор
  kasdoks_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    dn=mn=gn=dk=mk=gk=0;
   }      
 };

gboolean   kasdoks_key_press(GtkWidget *widget,GdkEventKey *event,class kasdoks_data *data);
void kasdoks_vibor(GtkTreeSelection *selection,class kasdoks_data *data);
void kasdoks_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class kasdoks_data *data);
void  kasdoks_knopka(GtkWidget *widget,class kasdoks_data *data);
void kasdoks_add_columns (GtkTreeView *treeview);
void kasdoks_create_list(class kasdoks_data *data);
int  kasdoks_prov_row(SQL_str row,class kasdoks_data *data);

int l_kasdoks_p(class kasdoks_rek *datap,GtkWidget *wpredok);
void l_kasdoks_eks(class kasdoks_data *data);

extern SQL_baza	bd;
extern char *name_system;
extern char     *organ;
 
void l_kasdoks(short ds,short ms,short gs, //Дата начала просмотра
GtkWidget *wpredok)
{
kasdoks_data data;
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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(kasdoks_key_press),&data);
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
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(kasdoks_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Включение/выключение показа только не подтверждённых документов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"SF2 %s",gettext("Метка"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(kasdoks_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Снятие метки неподтверждённого документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 *");
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(kasdoks_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Включение/выключение показа документов без проводок"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"SF3 %s",gettext("Метка"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(kasdoks_knopka),&data);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Снятие метки документа без проводок"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(kasdoks_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Просмотр"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(kasdoks_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Просмотр выбранного документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

if(ds != 0)
 {

  sprintf(bros,"F6 %s",gettext("Увеличить"));
  data.knopka[FK6]=gtk_button_new_with_label(bros);
  gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(kasdoks_knopka),&data);
  tooltips[FK6]=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Увеличить дату просмотра"),NULL);
  gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
  gtk_widget_show(data.knopka[FK6]);

  sprintf(bros,"F7 %s",gettext("Уменьшить"));
  data.knopka[FK7]=gtk_button_new_with_label(bros);
  gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(kasdoks_knopka),&data);
  tooltips[FK7]=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Уменьшить дату просмотра"),NULL);
  gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
  gtk_widget_show(data.knopka[FK7]);

 }

sprintf(bros,"F8 %s",gettext("Експорт"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(kasdoks_knopka),&data);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Экспорт кассовых ордеров в файл"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);
 
sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(kasdoks_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

kasdoks_create_list(&data);
gtk_widget_show(data.window);

//if(metka == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


printf("l_kasdoks end\n");

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));






}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  kasdoks_knopka(GtkWidget *widget,class kasdoks_data *data)
{
short dd,md,gd;
char strsql[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    if(data->poi.metka_pros == 1)
      data->poi.metka_pros=0;
    else
      data->poi.metka_pros=1;  
     
    kasdoks_create_list(data);
    return;  

  case SFK2:
    if(iceb_menu_danet(gettext("Снять отметку неподтвеждённого документа ? Вы уверены ?"),2,data->window) == 2)
     return;     
    sprintf(strsql,"update Kasord \
set \
podt=1 \
where datd='%s' and kassa=%d and nomd='%s' and tp=%d",
    data->datav.ravno_sqldata(),data->kassav.ravno_atoi(),data->nomdokv.ravno(),data->tipzv);

    iceb_sql_zapis(strsql,0,0,data->window);

    kasdoks_create_list(data);
    return;  

  case FK3:
    if(data->poi.metka_pros == 2)
      data->poi.metka_pros=0;
    else
      data->poi.metka_pros=2;
    kasdoks_create_list(data);
    return;  

  case SFK3:

    if(iceb_menu_danet(gettext("Снять отметку документа без проводок ? Вы уверены ?"),2,data->window) == 2)
     return;     
    sprintf(strsql,"update Kasord \
set \
prov=1 \
where datd='%s' and kassa=%d and nomd='%s' and tp=%d",
    data->datav.ravno_sqldata(),data->kassav.ravno_atoi(),data->nomdokv.ravno(),data->tipzv);

    iceb_sql_zapis(strsql,0,0,data->window);
    kasdoks_create_list(data);
    return;  

  case FK4:

    l_kasdoks_p(&data->poi,data->window);
    
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
    kasdoks_create_list(data);
    return;  

  case FK5:
    iceb_u_rsdat(&dd,&md,&gd,data->datav.ravno(),1);
    l_kasdok(data->datav.ravno(),data->tipzv,data->nomdokv.ravno(),data->kassav.ravno(),data->window);

    kasdoks_create_list(data);
    return;  

  case FK6:
    if(data->ds == 0)
     return;
    iceb_u_dpm(&data->dn,&data->mn,&data->gn,1);
    data->ds=data->dk=data->dn;
    data->ms=data->mk=data->mn;
    data->gs=data->gk=data->gn;
    kasdoks_create_list(data);
    return;  

  case FK7:
    if(data->ds == 0)
     return;
    iceb_u_dpm(&data->dn,&data->mn,&data->gn,2);
    data->ds=data->dk=data->dn;
    data->ms=data->mk=data->mn;
    data->gs=data->gk=data->gn;
    kasdoks_create_list(data);
    return;  

  case FK8:
    l_kasdoks_eks(data);
    return;  

    
  case FK10:
//    printf("kasdoks_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kasdoks_key_press(GtkWidget *widget,GdkEventKey *event,class kasdoks_data *data)
{
iceb_u_str repl;
//printf("kasdoks_key_press keyval=%d state=%d\n",event->keyval,event->state);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

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
    printf("kasdoks_key_press-Нажата клавиша Shift\n");

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
void kasdoks_create_list (class kasdoks_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
SQL_str row;
SQL_str row1;
char strsql[512];
char bros[500];
int  kolstr=0;
iceb_u_str zagolov;

//printf("kasdoks_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(kasdoks_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(kasdoks_vibor),data);

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

sprintf(bros,"kassa,tp,datd,nomd,shetk,kodop,prov,podt,fio,vrem,ktoi,god,nb,osnov,dopol,dokum");
sprintf(strsql,"select %s \
from Kasord where datd >= '%d-01-01' order by datd,nomd asc",
bros,data->gn);

if(data->dn != 0)
  sprintf(strsql,"select %s \
from Kasord where datd >= '%d-%02d-%02d' \
order by datd,nomd asc",bros,data->gn,data->mn,data->dn);

if(data->dn != 0 && data->dk != 0)
  sprintf(strsql,"select %s from Kasord \
where datd >= '%d-%02d-%02d' and datd <= '%d-%02d-%02d' \
order by datd,nomd asc",bros,data->gn,data->mn,data->dn,data->gk,data->mk,data->dk);

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
  
   if(kasdoks_prov_row(row,data) != 0)
    continue;

//  if(iceb_u_SRAV(data->kodtv.ravno(),row[0],0) == 0)
//   data->snanomer=data->kolzap;

  //Метка записи
  if(row[1][0] == '1')
   ss[COL_METKAZ].new_plus("+");
  else
   ss[COL_METKAZ].new_plus("-");

  if(row[7][0] == '0')
   ss[COL_METKAZ].plus("?");
  if(row[6][0] == '0')
   ss[COL_METKAZ].plus("*");

  //Дата документа
  ss[COL_DATAD].new_plus(iceb_u_sqldata(row[2]));
  
  //Номер документа
  ss[COL_NOMDOK].new_plus(iceb_u_toutf(row[3]));

  //Контрагент

  ss[COL_KONTR].new_plus("");

  if(row[8][0] != '\0')
   ss[COL_KONTR].new_plus(iceb_u_toutf(row[8]));
  else
   {
    /*Если в ордере 1 контрагент то берем его*/
    sprintf(strsql,"select kontr from Kasord1 where kassa=%s and \
god=%s and tp=%s and nomd='%s'",row[0],row[11],row[1],row[3]);
//    printw("%s\n",strsql);
 //   OSTANOV();
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row1[0]);
      if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
        ss[COL_KONTR].new_plus(iceb_u_toutf(row1[0]));
     }
   }

  //Код операции
  ss[COL_KODOP].new_plus(iceb_u_toutf(row[5]));

  //код кассы
  ss[COL_KASSA].new_plus(iceb_u_toutf(row[0]));

  //код кассы
  ss[COL_SHET].new_plus(iceb_u_toutf(row[4]));


  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[9])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[10],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKAZ,ss[COL_METKAZ].ravno(),
  COL_KASSA,ss[COL_KASSA].ravno(),
  COL_DATAD,ss[COL_DATAD].ravno(),
  COL_SHET,ss[COL_SHET].ravno(),
  COL_NOMDOK,ss[COL_NOMDOK].ravno(),
  COL_KONTR,ss[COL_KONTR].ravno(),
  COL_KODOP,ss[COL_KODOP].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

kasdoks_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str strpoi;  
strpoi.plus("");

if(data->poi.metka_pros == 1)
 strpoi.new_plus(gettext("Просмотр только неподтверждённых документов"));
if(data->poi.metka_pros == 2)
 strpoi.new_plus(gettext("Просмотр только документов без проводок"));

if(data->poi.metka_poi == 1 )
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
  iceb_str_poisk(&strpoi,data->poi.kassa.ravno(),gettext("Код кассы"));
  iceb_str_poisk(&strpoi,data->poi.kodop.ravno(),gettext("Код операции"));
  iceb_str_poisk(&strpoi,data->poi.shet.ravno(),gettext("Счёт"));
  iceb_str_poisk(&strpoi,data->poi.nomer_bl.ravno(),gettext("Номер бланка"));
  if(data->poi.pr_ras.ravno_pr() == 1)
    strpoi.ps_plus(gettext("Только приходы"));
  if(data->poi.pr_ras.ravno_pr() == 2)
    strpoi.ps_plus(gettext("Только расходы"));
    
 }

if(strpoi.getdlinna() > 1)
 {
  gtk_label_set_text(GTK_LABEL(data->label_poisk),strpoi.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
  gtk_widget_hide(data->label_poisk); 

zagolov.new_plus(gettext("Список документов"));
if(data->ds != 0)
 {
  sprintf(strsql," %d.%d.%d",data->ds,data->ms,data->gs);
  zagolov.plus(strsql);
 }
sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void kasdoks_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

//printf("kasdoks_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Метка"), renderer,"text", COL_METKAZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Касса"), renderer,"text",COL_KASSA,NULL);

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
-1,gettext("Счёт"), renderer,"text", COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

//printf("kasdoks_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void kasdoks_vibor(GtkTreeSelection *selection,class kasdoks_data *data)
{
printf("kasdoks_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *datad;
gchar *nomdok;
gchar *kassa;
gchar *tipz;

gint  nomer;


gtk_tree_model_get(model,&iter,COL_DATAD,&datad,COL_NOMDOK,&nomdok,COL_KASSA,&kassa,
COL_METKAZ,&tipz,NUM_COLUMNS,&nomer,-1);

data->datav.new_plus(iceb_u_fromutf(datad));
data->nomdokv.new_plus(iceb_u_fromutf(nomdok));
data->kassav.new_plus(iceb_u_fromutf(kassa));

if(tipz[0] == '+')
 data->tipzv=1;
else
 data->tipzv=2;
 
data->snanomer=nomer;

g_free(datad);
g_free(nomdok);
g_free(kassa);
g_free(tipz);

//printf("kasdoks_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void kasdoks_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class kasdoks_data *data)
{
//просмотр выбранного документа
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");


}
/****************************/
/*Проверка записей          */
/*****************************/

int  kasdoks_prov_row(SQL_str row,class kasdoks_data *data)
{
short pro=atoi(row[6]);

if(data->poi.metka_pros == 2 && pro == 1)
   return(1);
  
int pod=atoi(row[7]);
if(data->poi.metka_pros == 1 && pod == 1)
   return(1);

if(data->poi.metka_poi == 0)
 return(0);

//sprintf(bros,"kassa,tp,datd,nomd,shetk,kodop,prov,podt,fio,vrem,ktoi,god");
  

if(iceb_u_proverka(data->poi.kodop.ravno(),row[5],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->poi.shet.ravno(),row[4],0,0) != 0)
 return(1);


if(iceb_u_proverka(data->poi.kassa.ravno(),row[0],0,0) != 0)
 return(1);


if(data->poi.pr_ras.ravno_pr() != 0 && data->poi.pr_ras.ravno_pr() != atoi(row[1]))
 return(1);

if(data->poi.nomdok.getdlinna() > 1)
 if(iceb_u_SRAV(data->poi.nomdok.ravno(),row[3],1) != 0)
   return(1);

if(data->poi.nomer_bl.getdlinna() > 1)
 if(iceb_u_strstrm(row[12],data->poi.nomer_bl.ravno()) == 0)
   return(1);



  
return(0);
}
/*********************************/
/*Экспорт кассовых ордеров*/
/*****************************/
void l_kasdoks_eks(class kasdoks_data *data)
{

static char imaf[56];
char strsql[512];
sprintf(strsql,"%s\n%s",
gettext("Экспорт кассовых ордеров в файл"),
gettext("Введите имя файла"));

if(iceb_menu_vvod1(strsql,imaf,sizeof(imaf),data->window) != 0)
 return;
class SQLCURSOR cur;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }


FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

float kolstr1=0;
SQL_str row;
short d,m,g;
SQLCURSOR cur1;
SQLCURSOR cur2;
SQL_str row1;
SQL_str row2;
char fio[512];
int kolstr2;
char ikod[32];

while(cur.read_cursor(&row) != 0)
 {

  iceb_pbar(gdite.bar,kolstr,++kolstr1);

   if(kasdoks_prov_row(row,data) != 0)
    continue;

  
  fprintf(ff,"\nKDOK|");
  
  if(atoi(row[1]) == 1) //приходный кассовый ордер
    fprintf(ff,"+|");
  
  if(atoi(row[1]) == 2) //расходный кассовый ордер
    fprintf(ff,"-|");
  iceb_u_rsdat(&d,&m,&g,row[2],2);
    
  fprintf(ff,"%s|%d.%d.%d|%s|%s|%s|%s|%s|%s|%s|%s|\n",
  row[0],
  d,m,g,
  row[3],
  row[4],
  row[5],
  row[13],
  row[14],
  row[8],
  row[15],
  row[12]);
  
  //читаем содержимое кассового ордера
  sprintf(strsql,"select kontr,suma from Kasord1 where \
  kassa=%s and god=%d and tp=%s and nomd='%s'",
  row[0],g,row[1],row[3]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return;
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    memset(fio,'\0',sizeof(fio));
    memset(ikod,'\0',sizeof(ikod));
    //читаем наименование контрагента
    sprintf(strsql,"select naikon,kod from Kontragent where kodkon='%s'",row1[0]);
    if(iceb_sql_readkey(strsql,&row2,&cur2,data->window) == 1)
     {
      strncpy(fio,row2[0],sizeof(fio)-1);
      strncpy(ikod,row2[1],sizeof(ikod)-1);
     }    
    fprintf(ff,"KZAP|%s|%s|%s|%s|\n",
    row1[0],row1[1],ikod,fio);
    
   }  
 }
fclose(ff);

class iceb_u_spisok imafs;
class iceb_u_spisok naimf;
imafs.plus(imaf);
naimf.plus(gettext("Экспорт кассовых ордеров в файл"));

iceb_rabfil(&imafs,&naimf,"",0,data->window);

}


