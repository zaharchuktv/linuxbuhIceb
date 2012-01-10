/*$Id: l_xdkdoks.c,v 1.27 2011-02-21 07:35:54 sasa Exp $*/
/*08.03.2010	06.04.2006	Белых А.И.	l_xdkdoks.c
Работа со списком платёжных документов
*/
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include "buhg_g.h"
#include "l_xdkdoks.h"
enum
{
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 FK8,
 FK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_METKAZ,
 COL_DATAD,
 COL_KODOP,
 COL_NOMDOK,
 COL_KONTR,
 COL_NAIM_KONTR,
 COL_SUMA,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  xdkdoks_data
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

  class xdkdoks_rek poi;
  iceb_u_str zapros;

  short ds,ms,gs; //Стартовая дата просмотра
  char  tablica[30];
    
  short dn,mn,gn;
  short dk,mk,gk;
  
  iceb_u_str datav;
  iceb_u_str nomdokv;

  //Конструктор
  xdkdoks_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    dn=mn=gn=dk=mk=gk=0;
   }      
 };

gboolean   xdkdoks_key_press(GtkWidget *widget,GdkEventKey *event,class xdkdoks_data *data);
void xdkdoks_vibor(GtkTreeSelection *selection,class xdkdoks_data *data);
void xdkdoks_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class xdkdoks_data *data);
void  xdkdoks_knopka(GtkWidget *widget,class xdkdoks_data *data);
void xdkdoks_add_columns (GtkTreeView *treeview);
void xdkdoks_create_list(class xdkdoks_data *data);
int  xdkdoks_prov_row(SQL_str row,class xdkdoks_data *data);

int l_xdkdoks_p(class xdkdoks_rek *datap,char *tablica,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;
extern char     *organ;
extern short startgodd;
 
void l_xdkdoks(short ds,short ms,short gs, //Дата начала просмотра
const char *tablica,
GtkWidget *wpredok)
{
xdkdoks_data data;
char bros[312];
GdkColor color;

data.ds=data.dn=ds;
data.ms=data.mn=ms;
if(gs != 0)
 data.gs=data.gn=gs;
else
 {
    
  data.gn=data.gs=data.gn=startgodd;
  if(startgodd == 0)
   {
    short dt,mt,gt;
    iceb_u_poltekdat(&dt,&mt,&gt);
    data.gn=data.gs=data.gn=gt;
    
   }
  data.mn=data.ms=1;
  data.dn=data.ds=1;
 }
strcpy(data.tablica,tablica);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);

if(iceb_u_SRAV(data.tablica,"Pltp",0) == 0)
 sprintf(bros,"%s %s",name_system,gettext("Список платёжных поручений"));
else
 sprintf(bros,"%s %s",name_system,gettext("Список платёжных требований"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(xdkdoks_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);


if(iceb_u_SRAV(data.tablica,"Pltp",0) == 0)
  data.label_kolstr=gtk_label_new (gettext("Список платёжных поручений"));
else
  data.label_kolstr=gtk_label_new (gettext("Список платёжных требований"));

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
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(xdkdoks_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Включение/выключение показа только не подтверждённых документов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"SF2 %s",gettext("Метка"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(xdkdoks_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Снятие метки неподтверждённого документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 *");
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(xdkdoks_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Включение/выключение показа документов без проводок"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"SF3 %s",gettext("Метка"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(xdkdoks_knopka),&data);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Снятие метки документа без проводок"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(xdkdoks_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Просмотр"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(xdkdoks_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Просмотр выбранного документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F8 #");
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(xdkdoks_knopka),&data);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Включить/выключить показ только документов помеченных для передчи в банк"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);

sprintf(bros,"F9 %s",gettext("В банк"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(xdkdoks_knopka),&data);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Уcтановить/снять метку для передчи в банк"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(xdkdoks_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

xdkdoks_create_list(&data);
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
void  xdkdoks_knopka(GtkWidget *widget,class xdkdoks_data *data)
{
short dd,md,gd;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
data->kl_shift=0; //Сбрасываем нажатый сшифт

switch (knop)
 {
  case FK2:
    if(data->poi.metka_pros == 1)
      data->poi.metka_pros=0;
    else
      data->poi.metka_pros=1;  
     
    xdkdoks_create_list(data);
    return;  

  case SFK2:
    if(iceb_menu_danet(gettext("Снять отметку неподтвеждённого документа ? Вы уверены ?"),2,data->window) == 2)
     return;     

    sprintf(strsql,"update %s set \
podt=1 \
where datd='%s' and nomd='%s'",
    data->tablica,data->datav.ravno_sqldata(),data->nomdokv.ravno());

    iceb_sql_zapis(strsql,0,0,data->window);

    xdkdoks_create_list(data);
    return;  

  case FK3:
    if(data->poi.metka_pros == 2)
      data->poi.metka_pros=0;
    else
      data->poi.metka_pros=2;
    xdkdoks_create_list(data);
    return;  

  case SFK3:

    if(iceb_menu_danet(gettext("Снять отметку документа без проводок ? Вы уверены ?"),2,data->window) == 2)
     return;     

    sprintf(strsql,"update %s \
set \
prov=0 \
where datd='%s' and nomd='%s'",
    data->tablica,data->datav.ravno_sqldata(),data->nomdokv.ravno());

    iceb_sql_zapis(strsql,0,0,data->window);
    xdkdoks_create_list(data);
    return;  

  case FK4:

    l_xdkdoks_p(&data->poi,data->tablica,data->window);
    
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
    xdkdoks_create_list(data);
    return;  

  case FK5:
    iceb_u_rsdat(&dd,&md,&gd,data->datav.ravno(),1);

    readpdokw(data->tablica,gd,data->nomdokv.ravno(),data->window);

    xdkdok(data->tablica,data->window);
    /*При выходе из документа устанавливаются метки на документ*/
    xdkdoks_create_list(data);
    return;  

  case FK8:
    data->poi.metka_pp++;

    if(data->poi.metka_pp > 1)
     data->poi.metka_pp=0;    

    xdkdoks_create_list(data);
    return;  

   case FK9: /*Установить/снять метку для передачи в банк*/
      sprintf(strsql,"select vidpl from %s where datd='%s' and nomd='%s'",
      data->tablica,data->datav.ravno_sqldata(),data->nomdokv.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
       {
        iceb_menu_soob(gettext("Ненашли запись!"),data->window);
        return;
       }
      if(atoi(row[0]) == 1)
       sprintf(strsql,"update %s set vidpl='' where datd='%s' and nomd='%s'",
       data->tablica,data->datav.ravno_sqldata(),data->nomdokv.ravno());
      else
       sprintf(strsql,"update %s set vidpl='1' where datd='%s' and nomd='%s'",
       data->tablica,data->datav.ravno_sqldata(),data->nomdokv.ravno());

      iceb_sql_zapis(strsql,1,0,data->window);

      xdkdoks_create_list(data);

      return;  
  
  case FK10:
//    printf("xdkdoks_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   xdkdoks_key_press(GtkWidget *widget,GdkEventKey *event,class xdkdoks_data *data)
{
iceb_u_str repl;
//printf("xdkdoks_key_press keyval=%d state=%d\n",event->keyval,event->state);


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

  case GDK_F8:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK8]),"clicked");
    return(TRUE);

  case GDK_F9:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK9]),"clicked");
    return(TRUE);

  case GDK_F11:
//    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK11]),"clicked");
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("xdkdoks_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
//    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void xdkdoks_create_list (class xdkdoks_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQL_str row;
char strsql[512];
char bros[512];
int  kolstr=0;
iceb_u_str zagolov;

//printf("xdkdoks_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(xdkdoks_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(xdkdoks_vibor),data);

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

sprintf(bros,"datd,nomd,oper,suma,polu,podt,prov,ktoz,vrem,vidpl");

sprintf(strsql,"select %s \
from %s where datd >= '%d-01-01' order by datd asc",
bros,data->tablica,data->gn);

if(data->dn != 0)
  sprintf(strsql,"select %s \
from %s where datd >= '%d-%02d-%02d' \
order by datd asc",bros,data->tablica,data->gn,data->mn,data->dn);

if(data->dn != 0 && data->dk != 0)
  sprintf(strsql,"select %s from %s \
where datd >= '%d-%02d-%02d' and datd <= '%d-%02d-%02d' \
order by datd asc",bros,data->tablica,data->gn,data->mn,data->dn,data->gk,data->mk,data->dk);

data->zapros.new_plus(strsql);
//printf("%s\n",strsql);

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
  
   if(xdkdoks_prov_row(row,data) != 0)
    continue;

  //Метка записи
  if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
   ss[COL_METKAZ].new_plus("-");
  else
   ss[COL_METKAZ].new_plus("+");

  if(row[5][0] == '0')
   ss[COL_METKAZ].plus("?");

  if(row[6][0] == '1')
   ss[COL_METKAZ].plus("*");

  if(row[9][0] == '1')
   ss[COL_METKAZ].plus("#");
  
  //Дата документа
  ss[COL_DATAD].new_plus(iceb_u_sqldata(row[0]));
  
  //Номер документа
  ss[COL_NOMDOK].new_plus(iceb_u_toutf(row[1]));

  //Контрагент
  iceb_u_polen(row[4],strsql,sizeof(strsql),1,'#');
  ss[COL_KONTR].new_plus(iceb_u_toutf(strsql));
  
  //Наименование контрагента
  iceb_u_polen(row[4],strsql,sizeof(strsql),2,'#');
  ss[COL_NAIM_KONTR].new_plus(iceb_u_toutf(strsql));

  //Код операции
  ss[COL_KODOP].new_plus(iceb_u_toutf(row[2]));

  //Сумма по документа
  sprintf(strsql,"%10.2f",atof(row[3]));
  ss[COL_SUMA].new_plus(strsql);
  
  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[8])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[7],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKAZ,ss[COL_METKAZ].ravno(),
  COL_DATAD,ss[COL_DATAD].ravno(),
  COL_NOMDOK,ss[COL_NOMDOK].ravno(),
  COL_KONTR,ss[COL_KONTR].ravno(),
  COL_NAIM_KONTR,ss[COL_NAIM_KONTR].ravno(),
  COL_SUMA,ss[COL_SUMA].ravno(),
  COL_KODOP,ss[COL_KODOP].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

xdkdoks_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),TRUE);//Доступна
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
if(data->poi.metka_pp == 1)
 strpoi.new_plus(gettext("Просмотр только помеченных для передачи в банк"));

if(data->poi.metka_poi == 1 )
 {

// printf("Формирование заголовка с реквизитами поиска.\n");

  if(strpoi.getdlinna() > 1)      
   strpoi.ps_plus(gettext("Поиск"));
  else
   strpoi.new_plus(gettext("Поиск"));
  strpoi.plus(" !!!");

  iceb_str_poisk(&strpoi,data->poi.nomdok.ravno(),gettext("Номер документа"));
  iceb_str_poisk(&strpoi,data->poi.datan.ravno(),gettext("Дата начала"));
  iceb_str_poisk(&strpoi,data->poi.datak.ravno(),gettext("Дата конца"));
  iceb_str_poisk(&strpoi,data->poi.kodop.ravno(),gettext("Код операции"));
  iceb_str_poisk(&strpoi,data->poi.suma.ravno(),gettext("Сумма"));
    
 }

if(strpoi.getdlinna() > 1)
 {
  gtk_label_set_text(GTK_LABEL(data->label_poisk),strpoi.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
  gtk_widget_hide(data->label_poisk); 


if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
  zagolov.new_plus(gettext("Список платёжных поручений"));
else
  zagolov.new_plus(gettext("Список платёжных требований"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
zagolov.plus(" ");
zagolov.plus(gettext("Год"));
zagolov.plus(":");
zagolov.plus(data->gs);

sprintf(strsql,"?-%s *-%s #-%s",
gettext("не подтверждён"),
gettext("не сделаны проводки"),
gettext("передать в банк"));

zagolov.ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void xdkdoks_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

//printf("xdkdoks_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Метка"), renderer,"text", COL_METKAZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата док-та"), renderer,"text",COL_DATAD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Ном.док."), renderer,"text", COL_NOMDOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Операция"), renderer,"text", COL_KODOP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Контрагент"), renderer,"text", COL_KONTR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Наименование контрагента"), renderer,"text", COL_NAIM_KONTR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

//printf("xdkdoks_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void xdkdoks_vibor(GtkTreeSelection *selection,class xdkdoks_data *data)
{
printf("xdkdoks_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *datad;
gchar *nomdok;

gint  nomer;


gtk_tree_model_get(model,&iter,COL_DATAD,&datad,COL_NOMDOK,&nomdok,NUM_COLUMNS,&nomer,-1);

data->datav.new_plus(iceb_u_fromutf(datad));
data->nomdokv.new_plus(iceb_u_fromutf(nomdok));

data->snanomer=nomer;

g_free(datad);
g_free(nomdok);

//printf("xdkdoks_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void xdkdoks_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class xdkdoks_data *data)
{
//просмотр выбранного документа
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");


}
/****************************/
/*Проверка записей          */
/*****************************/

int  xdkdoks_prov_row(SQL_str row,class xdkdoks_data *data)
{
short pro=atoi(row[6]);

if(data->poi.metka_pp == 1 && atoi(row[9]) != 1)
 return(1);
 
if(data->poi.metka_pros == 2 && pro == 0)
   return(1);
  
int pod=atoi(row[5]);
if(data->poi.metka_pros == 1 && pod == 1)
   return(1);

if(data->poi.metka_poi == 0)
 return(0);

//sprintf(bros,"kassa,tp,datd,nomd,shetk,kodop,prov,podt,fio,vrem,ktoi,god");
  

if(iceb_u_proverka(data->poi.kodop.ravno(),row[2],0,0) != 0)
 return(1);




if(data->poi.nomdok.getdlinna() > 1)
 if(iceb_u_SRAV(data->poi.nomdok.ravno(),row[1],1) != 0)
   return(1);

if(data->poi.suma.getdlinna() > 1)
 if(fabs(data->poi.suma.ravno_atof()-atof(row[3])) > 0.009)
  return(1);


  
return(0);
}
