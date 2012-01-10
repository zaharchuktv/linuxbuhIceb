/*$Id: l_upldok.c,v 1.12 2011-02-21 07:35:54 sasa Exp $*/
/*24.03.2008	11.03.2008	Белых А.И.	l_upldok.c
Работа с документом в подсистеме "Учёт путевых листов"
*/
#include <stdlib.h>
#include "buhg_g.h"

enum
{
 SFK1,
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK6,
 FK7,
 FK8,
 FK10,
 KOL_F_KL
};

enum
{
 COL_KOD_TOP,
 COL_KOLIH,
 COL_TIPZ,
 COL_KOMENT,
 COL_DATA_VREM,
 COL_KTO,  
 COL_NOMZAP_V_DOK,
 COL_NOMZAP,
 NUM_COLUMNS
};

class  upldok_data
 {
  public:
  GtkWidget *label_red;
  GtkWidget *label_rek_dok;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей
  int voz;
  
  //реквизиты документа с которым работаем
  class iceb_u_str data_dok; /*Дата документа в символьном виде*/
  class iceb_u_str nomd;     /*Номер документа*/
  class iceb_u_str kod_pod;  /*код подразделения*/
    
  class iceb_u_str kod_vod;
  class iceb_u_str kod_avt;
  class iceb_u_str nz;  /*Номер документа в один и тотже день*/
  
  class iceb_u_str nomzap_v_dok_v;
  class iceb_u_str nomzap_v_dok_tv;
  class iceb_u_str nomzap_v;  
  double zat_top_f; /*Затраты топлива фактические*/    
  /*заголовок меню*/
  class iceb_u_str hapka;
  class iceb_u_spisok kod_top_ost;    /*Список кодов топлива на остатке*/
  class iceb_u_double kolih_top_ost;  /*Список количества топлива на остатке*/
  class iceb_u_spisok kod_top_pol;    /*Список кодов топлива плученного*/
  class iceb_u_double kolih_top_pol;  /*Список количества топлива полученного*/
  double kolih_ost; /*Количество топлива на остатке на начало документа*/
  short tipz;  
  //Конструктор
  upldok_data()
   {
    tipz=0;
    voz=0;
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    kolih_ost=0.;
    nomzap_v_dok_tv.plus("");
   }      

 };

gboolean   upldok_key_press(GtkWidget *widget,GdkEventKey *event,class upldok_data *data);
void upldok_vibor(GtkTreeSelection *selection,class upldok_data *data);
void upldok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class upldok_data *data);
void  upldok_knopka(GtkWidget *widget,class upldok_data *data);
void upldok_add_columns (GtkTreeView *treeview);
void upldok_create_list(class upldok_data *data);

int l_upldok_read_rhd(class  upldok_data *data,GtkWidget *wpredok);
int l_upldok_vn(short dd,short md,short gd,char *nomdok,GtkWidget *wpredok);
int l_upldok_uz(class upldok_data *data);
int l_upldok_ud(class upldok_data *data);
void l_prov_ukr(char *data_dok,char *nomdok,GtkWidget *wpredok);
void ukrku(short gd,char *nomd,GtkWidget *wpredok);
int l_upldok_vzp(const char *data_dok,int kod_pod,const char *nomdok,int nomzap,int metka_r, GtkWidget *wpredok);
void l_uplst(const char *data_dok,const char *nomdok,int kod_pod,GtkWidget *wpredok);
void uplast(short dd,short md,short gd,int nom_zap,const char *nomd,const char *podr,const char *kodavt,const char *kodvod,double sumasp,class iceb_u_spisok *KTC,class iceb_u_double *OST,class iceb_u_spisok *KTCPL,class iceb_u_double *OSTPL,GtkWidget *wpredok);
void potspv(short dd,short md,short gd,const char *podr,const char *nomd,const char *kodavt,const char *kodvod,int nom_zap,GtkWidget *wpredok);
int l_upldok_vzr(const char *data_dok,int kod_pod,const char *nomdok,int nomzap,int metka_r,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;
extern short startgodupl; //Стартовый год для "Учета путевых листов"

int l_upldok(const char *data_dok, //Дата документа
const char *nomd, //Номер документа
const char *kod_pod, /*код подразделения*/
GtkWidget *wpredok)
{
class  upldok_data data;
iceb_u_str string;
char bros[500];

data.data_dok.new_plus(data_dok);
data.nomd.plus(nomd);
data.kod_pod.plus(kod_pod);


/*Читаем реквизиты документа*/
l_upldok_read_rhd(&data,wpredok);

/*Читаем после чтения реквизитов документов*/
data.kolih_ost=ostvaw(data.data_dok.ravno(),data.kod_vod.ravno(),data.kod_avt.ravno(),data.nz.ravno_atoi(),&data.kod_top_ost,&data.kolih_top_ost,wpredok);


data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);

sprintf(bros,"%s %s",name_system,gettext("Работа с документом"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(upldok_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
GtkWidget *hbox_hap = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE,1);
gtk_widget_show(hbox);


data.label_rek_dok=gtk_label_new (data.hapka.ravno_toutf());
//gtk_box_pack_start (GTK_BOX (vbox2),data.label_rek_dok,FALSE, FALSE,1);

gtk_box_pack_start (GTK_BOX (vbox2),hbox_hap,FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox_hap),data.label_rek_dok,FALSE, FALSE,1);

class iceb_u_str spis_top_ost;
spis_top_ost.plus(gettext("Остаток топлива"));
spis_top_ost.plus(":\n");

for(int ii=0; ii < data.kod_top_ost.kolih(); ii++)
 {
  sprintf(bros,"%6s:%10.3f\n",data.kod_top_ost.ravno(ii),data.kolih_top_ost.ravno(ii));
  spis_top_ost.plus(bros);
 }
GtkWidget *label=NULL;
label=gtk_label_new(spis_top_ost.ravno_toutf());

gtk_box_pack_end (GTK_BOX (hbox_hap),label,FALSE, FALSE,1);

gtk_widget_show_all(hbox_hap);

data.label_red=gtk_label_new ("");
gtk_box_pack_start (GTK_BOX (vbox2),data.label_red,FALSE, FALSE,1);

GdkColor color;
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_red,GTK_STATE_NORMAL,&color);


gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),-1,200);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE,1);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE,1);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"%sF1 %s",RFK,gettext("Шапка"));
data.knopka[SFK1]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK1]), "clicked",GTK_SIGNAL_FUNC(upldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK1],TRUE,TRUE,1);
tooltips[SFK1]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK1],data.knopka[SFK1],gettext("Просмотр шапки документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK1]),(gpointer)SFK1);
gtk_widget_show(data.knopka[SFK1]);

sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(upldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE,1);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи на получение топлива"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(upldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE,1);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(upldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE,1);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(upldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE,1);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удалить документ"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Объекти"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(upldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE,1);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Переход в режим списания топлива по объектам и счетам"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F6 %s",gettext("Списание"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(upldok_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Автоматическое списание топлива"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Перенос"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(upldok_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Перенос остатка топлива с предыдушего водителя"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"F8 %s",gettext("Списание"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(upldok_knopka),&data);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Ввод записи на списание топлива"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE,1);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(upldok_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);

 
gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

upldok_create_list(&data);
gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));


gtk_main();


//printf("l_upldok end\n");

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  upldok_knopka(GtkWidget *widget,class upldok_data *data)
{
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("upldok_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый шифт так как после запуска нового меню он не сбрасывается
short dd=0,md=0,gd=0;
iceb_u_rsdat(&dd,&md,&gd,data->data_dok.ravno(),1);

switch (knop)
 {
  case SFK1:

    if(l_upldok_v(&data->data_dok,&data->kod_vod,&data->nomd,data->window) == 0)
     {
      /*Читаем реквизиты документа*/
      l_upldok_read_rhd(data,data->window);
      upldok_create_list(data);
      data->voz=1;
     }
    return;  

  case FK2:
    if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
     return;

    if(l_upldok_vzp(data->data_dok.ravno(),data->kod_pod.ravno_atoi(),data->nomd.ravno(),0,0,data->window) == 0)
      upldok_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
     return;

    if(data->tipz == 2) /*Расход*/
     if(l_upldok_vzr(data->data_dok.ravno(),data->kod_pod.ravno_atoi(),data->nomd.ravno(),data->nomzap_v_dok_tv.ravno_atoi(),1,data->window) == 0)
       upldok_create_list(data);

    if(data->tipz == 1) /*приход*/
     if(l_upldok_vzp(data->data_dok.ravno(),data->kod_pod.ravno_atoi(),data->nomd.ravno(),data->nomzap_v_dok_tv.ravno_atoi(),1,data->window) == 0)
       upldok_create_list(data);

    return;  

    return;  

  case FK3:
    if(data->kolzap == 0)
     return;
    if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
     return;

    if(l_upldok_uz(data) == 0)
      upldok_create_list(data);

    return;  

  case SFK3:
    if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
     return;

    if(l_upldok_ud(data) != 0)
     return;

    data->voz=1;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    if(data->kolzap == 0)
     return;
    l_uplst(data->data_dok.ravno(),data->nomd.ravno(),data->kod_pod.ravno_atoi(),data->window);
    return;  

  case FK6:
    if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
     return;

    uplast(dd,md,gd,data->nz.ravno_atoi(),data->nomd.ravno(),data->kod_pod.ravno(),data->kod_avt.ravno(),
    data->kod_vod.ravno(),data->zat_top_f,&data->kod_top_ost,&data->kolih_top_ost,
    &data->kod_top_pol,&data->kolih_top_pol,data->window);

    upldok_create_list(data);

    return;

  case FK7:
    if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
     return;

    potspv(dd,md,gd,data->kod_pod.ravno(),data->nomd.ravno(),data->kod_avt.ravno(),data->kod_vod.ravno(),data->nz.ravno_atoi(),data->window);

    return;

  case FK8:
    if(l_upldok_vzr(data->data_dok.ravno(),data->kod_pod.ravno_atoi(),data->nomd.ravno(),0,0,data->window) == 0)
     upldok_create_list(data);
    return;


  case FK10:
    provpstw(dd,md,gd,data->nomd.ravno(),data->kod_pod.ravno_atoi(),data->window);

    gtk_widget_destroy(data->window);
    return;


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   upldok_key_press(GtkWidget *widget,GdkEventKey *event,class upldok_data *data)
{
//printf("upldok_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F1:
//    if(data->kl_shift == 0)
//      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK1]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK1]),"clicked");
    return(TRUE);

  case GDK_plus: //это почемуто не работает Нажата клавиша плюс на основной клавиатуре
  case GDK_KP_Add: //Нажата клавиша "+" на дополнительной клавиатуре
  case GDK_F2:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
    return(TRUE);

  case GDK_minus: //Нажата клавиша минус на основной клавиатуре
  case GDK_KP_Subtract: //Нажата клавиша "-" на дополнительной клавиатуре
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

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  case GDK_F7:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
    return(TRUE);

  case GDK_F8:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK8]),"clicked");
    return(TRUE);


  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("upldok_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  default:
    printf("Не выбрана клавиша ! Код=%d\n",event->keyval);

    break;
 }

return(TRUE);
}
/*****************/
/*Создаем колонки*/
/*****************/

void upldok_add_columns(class upldok_data *data)
{
GtkCellRenderer *renderer;

//printf("upldok_add_columns\n");
renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
"M", renderer,"text", COL_TIPZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Код топлива"), renderer,"text", COL_KOD_TOP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Количество"), renderer,"text", COL_KOLIH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Коментар"), renderer,"text", COL_KOMENT,NULL);



renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

//printf("upldok_add_columns end\n");

}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void upldok_create_list (class upldok_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row;

//printf("upldok_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(upldok_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(upldok_vibor),data);

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

sprintf(strsql,"select nzap,tz,kodtp,kolih,kom,ktoz,vrem from Upldok1 where datd='%s' and kp=%d and nomd='%s' order by tz,kodtp asc",
data->data_dok.ravno_sqldata(),data->kod_pod.ravno_atoi(),data->nomd.ravno());

/*printf("strsql=%s\n",strsql);*/

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

class iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;

float kolstr1=0.;
double itogo_sp=0.;
double ost=data->kolih_ost;
double kolih=0.;
data->kod_top_pol.free_class();
data->kolih_top_pol.free_class();
int nomer_top=0;
while(cur.read_cursor(&row) != 0)
 {
/*  printf("%s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6]);*/
  
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  //Номер записи в документе
  ss[COL_NOMZAP_V_DOK].new_plus(row[0]);
   
  //Код топлива
  ss[COL_KOD_TOP].new_plus(iceb_u_toutf(row[2]));
  //Количество  
  ss[COL_KOLIH].new_plus(row[3]);
  kolih=atof(row[3]);

  //Тип записи
  if(row[1][0] == '1')
   {
    ss[COL_TIPZ].new_plus("+");
    if((nomer_top=data->kod_top_pol.find(row[2])) < 0)
     data->kod_top_pol.plus(row[2]);
    data->kolih_top_pol.plus(kolih,nomer_top);    
   }
  else
   {
    ss[COL_TIPZ].new_plus("-");
    itogo_sp+=atof(row[3]);
    kolih*=-1;
   }  
  
  ost+=kolih;
  //Коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[4]));

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[6])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[5],0,data->window));


  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD_TOP,ss[COL_KOD_TOP].ravno(),
  COL_KOLIH,ss[COL_KOLIH].ravno(),
  COL_TIPZ,ss[COL_TIPZ].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_KOLIH,ss[COL_KOLIH].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  COL_NOMZAP_V_DOK,ss[COL_NOMZAP_V_DOK].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->nomzap_v_dok_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

upldok_add_columns (data);

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
 }
gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

class iceb_u_str zagol;
zagol.plus(data->hapka.ravno());

if(data->kolzap > 0)
 {
  sprintf(strsql,"%s:%d ",
  gettext("Количество записей"),
  data->kolzap);

  zagol.ps_plus(strsql);
  
  sprintf(strsql,"%s:%.10g ",
  gettext("Списано топлива"),
  itogo_sp);

  zagol.plus(strsql);

  sprintf(strsql,"%s:%.10g",
  gettext("Остаток"),
  ost);
    
  zagol.plus(strsql);

 }
 
gtk_label_set_text(GTK_LABEL(data->label_rek_dok),zagol.ravno_toutf());

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));
}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void upldok_vibor(GtkTreeSelection *selection,class upldok_data *data)
{
//printf("upldok_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *nomzap_v_dok;
gchar *tipz;
gint  nomer;

gtk_tree_model_get(model,&iter,
COL_NOMZAP_V_DOK,&nomzap_v_dok,
COL_TIPZ,&tipz,
NUM_COLUMNS,&nomer,-1);

data->nomzap_v_dok_v.new_plus(nomzap_v_dok);
if(tipz[0] == '+')
 data->tipz=1;
else
 data->tipz=2;
data->snanomer=nomer;

g_free(nomzap_v_dok);
g_free(tipz);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void upldok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class upldok_data *data)
{
//Корректировка записи
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");


}

/******************************/
/*Чтение реквизитов шапки документа*/
/************************************/
int l_upldok_read_rhd(class  upldok_data *data,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select * from Upldok where god=%d and kp=%d and \
nomd='%s'",data->data_dok.ravno_god(),data->kod_pod.ravno_atoi(),data->nomd.ravno());

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  
  sprintf(strsql,"%s %s %s !",
  gettext("Не найден документ"),
  data->nomd.ravno(),
  data->data_dok.ravno());
  
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

data->kod_vod.new_plus(row[6]);
data->kod_avt.new_plus(row[5]);
data->nz.new_plus(row[33]);

data->zat_top_f=atof(row[10])+atof(row[23])+atof(row[28])+atof(row[31]);

class iceb_u_str naim_pod("");
sprintf(strsql,"select naik from Uplpodr where kod=%d",data->kod_pod.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_pod.new_plus(row[0]);

class iceb_u_str naim_vod("");
sprintf(strsql,"select naik from Uplouot where kod=%d",data->kod_vod.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_vod.new_plus(row[0]);

class iceb_u_str naim_avt("");
sprintf(strsql,"select naik from Uplavt where kod=%d",data->kod_avt.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_avt.new_plus(row[0]);

sprintf(strsql,"%s %s %s:%s\n",
gettext("Дата документа"),
data->data_dok.ravno(),
gettext("Номер"), 
data->nomd.ravno());

data->hapka.new_plus(strsql);

sprintf(strsql,"%s:%s/%s\n",
gettext("Подразделение"),
data->kod_pod.ravno(),naim_pod.ravno());

data->hapka.plus(strsql);


sprintf(strsql,"%s:%d/%s\n",
gettext("Водитель"),
data->kod_vod.ravno_atoi(),naim_vod.ravno());

data->hapka.plus(strsql);

sprintf(strsql,"%s:%d/%s\n",
gettext("Автомобиль"),
data->kod_avt.ravno_atoi(),naim_avt.ravno());

data->hapka.plus(strsql);

sprintf(strsql,"%s:%.10g",
gettext("Затраты топлива"),
data->zat_top_f);

data->hapka.plus(strsql);


return(0);
}
/***********************/
/*Удаление записи в документе*/
/****************************/
int l_upldok_uz(class upldok_data *data)
{

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
   return(1);    

char strsql[512];

sprintf(strsql,"delete from Upldok1 where datd='%s' and kp=%d and nomd='%s' and nzap=%d",
data->data_dok.ravno_sqldata(),data->kod_pod.ravno_atoi(),data->nomd.ravno(),data->nomzap_v_dok_v.ravno_atoi());

iceb_sql_zapis(strsql,1,0,data->window);

return(0);
}
/***************************/
/*Удаление всего документа*/
/**************************/
int l_upldok_ud(class upldok_data *data)
{


if(iceb_menu_danet(gettext("Удалить документ ? Вы уверены ?"),2,data->window) == 2)
  return(1);    

char strsql[512];

sprintf(strsql,"delete from Upldok2 where datd='%s' and kp=%d and nomd='%s'",
data->data_dok.ravno_sqldata(),data->kod_pod.ravno_atoi(),data->nomd.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
  return(1);

sprintf(strsql,"delete from Upldok1 where datd='%s' and kp=%d and nomd='%s'",
data->data_dok.ravno_sqldata(),data->kod_pod.ravno_atoi(),data->nomd.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
  return(1);

sprintf(strsql,"delete from Upldok where god=%d and kp=%d and nomd='%s'",
data->data_dok.ravno_god(),data->kod_pod.ravno_atoi(),data->nomd.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
  return(1);

 

return(0);
}
/*******************************/
/*Запись расходов из массива   */
/*******************************/

double	uplast1(short dd,short md,short gd, //Дата
int nom_zap, //порядковый номер документа в один и тотже день
const char *nomd, //Номер документа
const char *podr, //Подразделение
const char *kodavt, //Код автомобиля
const char *kodvod, //Код водителя
double sumasp, //Сумма которую надо списать
class iceb_u_spisok *KTC,class iceb_u_double *OST,
GtkWidget *wpredok)
{
double          kolih=0.;
double		kolihz=0.;
time_t		vrem;
char		kodtp[20];

//printw("uplast1\n");
//refresh();

time(&vrem);

int kolzap=KTC->kolih();
int i;


for(i=0; i<kolzap;i++)
 {
  if(sumasp < 0.0009)
   return(0.);

  strcpy(kodtp,KTC->ravno(i));

  kolih=OST->ravno(i);
  
  if(kolih > sumasp)
   {
    kolihz=sumasp;
    sumasp=0.;
   }
  else
   {
    kolihz=kolih;
    sumasp-=kolih;
   }
//  printw("kolihz=%f sumasp=%f\n",kolihz,sumasp);
//OSTANOV();


  if(kolihz > 0.)
   {
    upl_zapvdokw(dd,md,gd,atoi(podr),nomd,atoi(kodavt),atoi(kodvod),0,2,kodtp,kolihz,
    "",0,nom_zap,wpredok);
  }
 }

return(sumasp);

}

/****************************************/
/*Автоматическое списание топлива       */
/****************************************/

void	uplast(short dd,short md,short gd, //Дата
int nom_zap, //порядковый номер документа в один и тотже день
const char *nomd, //Номер документа
const char *podr, //Подразделение
const char *kodavt, //Код автомобиля
const char *kodvod, //Код водителя
double sumasp, //Сумма которую надо списать
class iceb_u_spisok *KTC,class iceb_u_double *OST,
class iceb_u_spisok *KTCPL,class iceb_u_double *OSTPL,
GtkWidget *wpredok)
{
char		strsql[512];
double   spisano=0;

//Удаляем все расходы в документе
sprintf(strsql,"delete from Upldok1 where datd='%d-%d-%d' and kp=%d and nomd='%s' and tz=2",
gd,md,dd,atoi(podr),nomd);

if(iceb_sql_zapis(strsql,0,0,wpredok) != 0)
  return;

spisano=uplast1(dd,md,gd,nom_zap,nomd,podr,kodavt,kodvod,sumasp,KTC,OST,wpredok);
uplast1(dd,md,gd,nom_zap,nomd,podr,kodavt,kodvod,spisano,KTCPL,OSTPL,wpredok);

}

/*************************************************/
/*Перенос остатков топлива с предыдущего водителя*/
/*************************************************/

void potspv(short dd,short md,short gd,
const char *podr,
const char *nomd,
const char *kodavt,
const char *kodvod,
int nom_zap,
GtkWidget *wpredok)
{
char		strsql[512];
SQL_str		row;
SQLCURSOR curr;
char		kodvodp[20];

memset(kodvodp,'\0',sizeof(kodvodp));

//Определяем самый последний документ по данному автомобилю
if(nom_zap == 0)
 sprintf(strsql,"select kv from Upldok1 where \
datd < '%d-%d-%d' and ka=%s order by datd desc limit 1",
 gd,md,dd,kodavt);
else
 sprintf(strsql,"select kv from Upldok1 where \
datd = '%d-%d-%d' and ka=%s and nz < %d order by nz desc limit 1",
 gd,md,dd,kodavt,nom_zap);

if(iceb_sql_readkey(strsql,&row,&curr,wpredok) > 0)
 {
  strncpy(kodvodp,row[0],sizeof(kodvodp)-1); 
 }
else
 {
  //Читаем может есть сальдо
  sprintf(strsql,"select kv from Uplsal where god=%d and ka=%s",startgodupl,kodavt);
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
    strncpy(kodvodp,row[0],sizeof(kodvodp)-1); 

 }
if(iceb_u_SRAV(kodvod,kodvodp,0) == 0)
  return;
if(kodvodp[0] == '\0')
  return;

//Создаем список остатков топлива
class iceb_u_spisok KTC;
class iceb_u_double OST;

ostvaw(dd,md,gd,kodvodp,kodavt,nom_zap,&KTC,&OST,wpredok);

int kolsp=KTC.kolih();
if(kolsp == 0)
  return;
  
double kolih=0.;
char   kodtp[20];
char koment[512];

memset(koment,'\0',sizeof(koment));
strncpy(koment,gettext("Перенос остатка"),sizeof(koment)-1);

time_t vrem;
time(&vrem);

for(int i=0; i < kolsp;i++)
 {

  strcpy(kodtp,KTC.ravno(i));

  kolih=OST.ravno(i);

  upl_zapvdokw(dd,md,gd,atoi(podr),nomd,atoi(kodavt),atoi(kodvod),atoi(kodvodp),1,kodtp,kolih,
  koment,atoi(kodavt),nom_zap,wpredok);

 }



}
