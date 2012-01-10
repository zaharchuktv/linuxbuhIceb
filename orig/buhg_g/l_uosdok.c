/*$Id: l_uosdok.c,v 1.26 2011-06-07 08:52:26 sasa Exp $*/
/*15.06.2010	30.10.2007	Белых А.И.	l_uosdok.c
Работа с документом в подсистеме "Учёт основных средств"
Если вернули 0-документ не удалён
             1-произошли измения, требующие перечитать список документов
               например - документ удалён или изменился номер или дата документа
*/
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>
#include "buhg_g.h"
#include <unistd.h>

enum
{
 SFK1,
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 FK6,
 FK7,
 SFK7,
 FK8,
 FK9,
 FK10,
 FK11,
 KOL_F_KL
};

enum
{
 COL_INNOM,
 COL_NAIM,
 COL_BSNU,
 COL_IZNU,
 COL_BSBU,
 COL_IZBU,
 COL_SHSP,
 COL_CENA,
 COL_PODTV,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  uosdok_data
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
  class iceb_u_str nomd;
  float pnds; //Процент НДС на момент создания документа
  
  int tipz;
  int tipz_p;
  class iceb_u_str kontr;
  class iceb_u_str kod_op;
  class iceb_u_str nomd_p;
  int podr;
  int podr_p;
  int mat_ot;
  int mat_ot_p;
  short lnds;
  short metkaprov; 
  short vido;
  
  /*Выбранный инвентарный номер*/
  class iceb_u_str innom_v;
  
  /*заголовок меню*/
  class iceb_u_str hapka;
    
  //Конструктор
  uosdok_data()
   {
    voz=0;
    clear_rek_dok();
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    pnds=0.;
    tipz=0;    
    metkaprov=0;
    vido=0;
    
   }      
  void clear_rek_dok()
   {
    lnds=0;
    kontr.new_plus("");
    kod_op.new_plus("");
    podr=0;
    mat_ot=0;
      
   }

 };

gboolean   uosdok_key_press(GtkWidget *widget,GdkEventKey *event,class uosdok_data *data);
void uosdok_vibor(GtkTreeSelection *selection,class uosdok_data *data);
void uosdok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class uosdok_data *data);
void  uosdok_knopka(GtkWidget *widget,class uosdok_data *data);
void uosdok_add_columns (GtkTreeView *treeview);
void uosdok_create_list(class uosdok_data *data);

int l_uosdok_read_rhd(class  uosdok_data *data,GtkWidget *wpredok);
int l_uosdok_vin(class  uosdok_data *data);

int uoskart1(const char *datadok,const char *nomdok,int innom,double *bsnu,double *iznu,double *bsbu,double *izbu,GtkWidget *wpredok);
int uoszvdok(short dd,short md,short gd,int tipz,int podt,int innom,const char *nomd,int podr,int mat_ot,int kol,double bs,double iz,float kof_ind,const char *kod_op,double bsby,double izby,double cena,const char *shetsp,GtkWidget *wpredok);
int l_uosdok_v(int innom,double *bsnu,double *iznu,double *bsbu,double *izbu,class iceb_u_str *shetsp,double *cena,int tipz,int metka_m,int,time_t,GtkWidget *wpredok);
int l_uosdok_kz(class  uosdok_data *data);
int pudokw(int podd,short d,short m,short g,const char *ndk,short tz,GtkWidget *wpredok);
int uduoszdw(int in,short d,short m,short g,const char *ndk,GtkWidget *wpredok);
int uduosdokw(short d,short m,short g,const char *ndk,GtkWidget *wpredok);
int l_uosdok_ud(int knop,class  uosdok_data *data);
void l_prov_uos(const char *data_dok,const char *nomdok,GtkWidget *wpredok);
void l_uosdok_f5p(class  uosdok_data *data);
void l_uosdok_f5r(class  uosdok_data *data);
void rasactuosw(const char *data_dok,const char *nomdok,GtkWidget *wpredok);
void uosactw(const char *data_dok,const char *ndk,GtkWidget *wpredok);
void uosremw(const char *data_dok,const char *ndk,GtkWidget *wpredok);
void rasdokuosw(const char *data_dok,int podr,const char *nomdok,int tipz,GtkWidget *wpredok);
int rasnalnuosw(const char *data_dok,int podr,const char *nomdok,char *imaf,GtkWidget *wpredok);
void uossosw(const char *data_dok,const char *ndk,GtkWidget *wpredok);
void uossasw(const char *data_dok,const char *ndk,GtkWidget *wpredok);
void l_uosdok_podtv(int innom,short tipz,const char *data_dok,const char *nomdok,short met,short mo,int podr,GtkWidget *wpredok);
void poduosdw(short d,short m,short g,const char *ndk,GtkWidget *wpredok);
void opluosw(short d,short m,short g,int tipz,const char *nn,const char *kodop,GtkWidget *wpredok);
void l_uosdok_f10(class uosdok_data *data);

extern SQL_baza	bd;
extern char *name_system;
//extern char *spgnupa; //Список групп налогового учета с пообъектной амортизацией

int l_uosdok(const char *data_dok, //Дата документа
const char *nomd, //Номер документа
GtkWidget *wpredok)
{
class  uosdok_data data;
iceb_u_str string;
char bros[512];
data.data_dok.new_plus(data_dok);
data.nomd.plus(nomd);
printf("%s\n",__FUNCTION__);

/*Читаем реквизиты документа*/
l_uosdok_read_rhd(&data,wpredok);

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uosdok_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);


data.label_rek_dok=gtk_label_new (data.hapka.ravno_toutf());
gtk_box_pack_start (GTK_BOX (vbox2),data.label_rek_dok,FALSE, FALSE, 0);
gtk_widget_show(data.label_rek_dok);

data.label_red=gtk_label_new ("");
gtk_box_pack_start (GTK_BOX (vbox2),data.label_red,FALSE, FALSE, 0);

GdkColor color;
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_red,GTK_STATE_NORMAL,&color);


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

sprintf(bros,"%sF1 %s",RFK,gettext("Шапка"));
data.knopka[SFK1]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK1]), "clicked",GTK_SIGNAL_FUNC(uosdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK1],TRUE,TRUE, 0);
tooltips[SFK1]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK1],data.knopka[SFK1],gettext("Просмотр шапки документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK1]),(gpointer)SFK1);
gtk_widget_show(data.knopka[SFK1]);

sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(uosdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(uosdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(uosdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(uosdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удалить документ"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Проводки"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(uosdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Переход в режим работы с проводками для этого документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(uosdok_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка документов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);


sprintf(bros,"F6 %s",gettext("Карточка"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(uosdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Просмотр карточки инвентарного номера"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Подтверждение"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(uosdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Подтверждение записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"%sF7 %s",RFK,gettext("Подтверждение"));
data.knopka[SFK7]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK7]), "clicked",GTK_SIGNAL_FUNC(uosdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK7],TRUE,TRUE, 0);
tooltips[SFK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK7],data.knopka[SFK7],gettext("Подтверждение всех записей в документе"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK7]),(gpointer)SFK7);
gtk_widget_show(data.knopka[SFK7]);

sprintf(bros,"F8 %s",gettext("Блокирование"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(uosdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Устанавливать/снимать блокировку дат"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);

sprintf(bros,"F9 %s",gettext("Подтверждение"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(uosdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Снять подтверждение со всего документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(uosdok_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);
if(data.vido == 1)
 {
  sprintf(bros,"F11 %s",gettext("Парный док."));
  data.knopka[FK11]=gtk_button_new_with_label(bros);
  gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK11],TRUE,TRUE, 0);
  tooltips[FK11]=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips[FK11],data.knopka[FK11],gettext("Перейти в парный документ"),NULL);
  gtk_signal_connect(GTK_OBJECT(data.knopka[FK11]), "clicked",GTK_SIGNAL_FUNC(uosdok_knopka),&data);
  gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK11]),(gpointer)FK11);
  gtk_widget_show(data.knopka[FK11]);
 }
 
gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

uosdok_create_list(&data);
gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));


gtk_main();


//printf("l_uosdok end\n");

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosdok_knopka(GtkWidget *widget,class uosdok_data *data)
{
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("uosdok_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый шифт так как после запуска нового меню он не сбрасывается
class iceb_u_str kod("");
short d=0,m=0,g=0;

class iceb_u_str podr("");
class iceb_u_str nomdok("");
class iceb_u_str mat_ol("");

switch (knop)
 {
  case SFK1:
    if(uosvhd(&data->data_dok,&data->nomd,&kod,data->tipz,data->window) == 0)
     {
      /*Читаем реквизиты документа*/
      l_uosdok_read_rhd(data,data->window);
      uosdok_create_list(data);
      data->voz=1;
     }
    return;  

  case FK2:

    l_uosdok_vin(data);
    uosdok_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
     return;
    if(l_uosdok_kz(data) == 0)
      uosdok_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
     return;

    if(l_uosdok_ud((int)knop,data) == 0)
      uosdok_create_list(data);

    return;  

  case SFK3:

    if(l_uosdok_ud((int)knop,data) != 0)
     return;
     
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    if(data->metkaprov == 1)
     {
      iceb_menu_soob(gettext("Для этой операции проводки не делаются !"),data->window);
      return;
     }

    if(data->vido == 1 && data->tipz == 1)
     {
      iceb_menu_soob(gettext("Для внутренних операций проводки делаются только\nдля расходных документов !"),data->window);
      return;
     }    
  

    l_prov_uos(data->data_dok.ravno(),data->nomd.ravno(),data->window);
      
    return;  

  case FK5:
    if(data->kolzap == 0)
     return;
    if(data->tipz == 1)
      l_uosdok_f5p(data);
    if(data->tipz == 2)
      l_uosdok_f5r(data);

    return;  


  case FK6:
    if(data->kolzap == 0)
     return;
    iceb_u_rsdat(&d,&m,&g,data->data_dok.ravno(),1);
    uoskart2(data->innom_v.ravno_atoi(),d,m,g,data->window);    
    return;  

  case FK7:
    if(data->kolzap == 0)
     return;

    l_uosdok_podtv(data->innom_v.ravno_atoi(),data->tipz,data->data_dok.ravno(),data->nomd.ravno(),0,0,data->podr,data->window);
    if(data->vido == 1)
      l_uosdok_podtv(data->innom_v.ravno_atoi(),data->tipz_p,data->data_dok.ravno(),data->nomd_p.ravno(),1,0,data->podr_p,data->window);
    

    uosdok_create_list(data);
    return;  

  case SFK7:
    if(data->kolzap == 0)
     return;

    l_uosdok_podtv(0,data->tipz,data->data_dok.ravno(),data->nomd.ravno(),0,0,data->podr,data->window);
    if(data->vido == 1)
      l_uosdok_podtv(0,data->tipz_p,data->data_dok.ravno(),data->nomd_p.ravno(),1,0,data->podr_p,data->window);
    
    uosdok_create_list(data);
    return;  

  case FK8:
    iceb_f_redfil("bluos.alx",0,data->window);
    return;  

  case FK9:
    l_uosdok_podtv(0,data->tipz,data->data_dok.ravno(),data->nomd.ravno(),0,1,data->podr,data->window);
    if(data->vido == 1)
      l_uosdok_podtv(0,data->tipz_p,data->data_dok.ravno(),data->nomd_p.ravno(),1,1,data->podr_p,data->window);
    uosdok_create_list(data);
    return;  
    
  case FK10:
    l_uosdok_f10(data);
    gtk_widget_destroy(data->window);
    return;

  case FK11: /*Перейти в парный документ*/
    if(data->vido != 1)
     return;
    if(iceb_menu_danet(gettext("Перейти в парный документ ? Вы уверены ?"),2,data->window) == 2)
     return;
    data->nomd.new_plus(data->nomd_p.ravno());
  
    l_uosdok_read_rhd(data,data->window);
    uosdok_create_list(data);

    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosdok_key_press(GtkWidget *widget,GdkEventKey *event,class uosdok_data *data)
{
iceb_u_str repl;
//printf("uosdok_key_press keyval=%d state=%d\n",event->keyval,event->state);

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
    //Ввод материалов
/*********
    if( l_uosdok_prblok(0,data->md,data->gd,data->blokpid,data->window) != 0)
     return(TRUE);
    vuslw(data->dd,data->md,data->gd,data->tipz,data->podr,data->nomd.ravno(),0,data->pnds,data->window);
    deletdokumu(&data->imaf);
    uosdok_create_list(data);
****************/
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
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK7]),"clicked");
    return(TRUE);

  case GDK_F8:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK8]),"clicked");
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
    printf("uosdok_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша ! Код=%d\n",event->keyval);

    break;
 }

return(TRUE);
}
/*****************/
/*Создаем колонки*/
/*****************/

void uosdok_add_columns(class uosdok_data *data)
{
GtkCellRenderer *renderer;

//printf("uosdok_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
"M", renderer,"text", COL_PODTV,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Инв.ном."), renderer,"text", COL_INNOM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Бал. стоимость\n(нал. учёт)"), renderer,"text", COL_BSNU,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Износ\n(нал. учёт)"), renderer,"text", COL_IZNU,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Бал. стоимость\n(бух. учёт)"), renderer,"text", COL_BSBU,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Износ\n(бух. учёт)"), renderer,"text", COL_IZBU,NULL);

if(data->vido == 0)
 {
  renderer = gtk_cell_renderer_text_new ();

  if(data->tipz == 1)
   gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
   gettext("Счёт приобретения"), renderer,"text", COL_SHSP,NULL);

  if(data->tipz == 2)
   gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
   gettext("Счёт списания"), renderer,"text", COL_SHSP,NULL);
 } 

if(data->tipz == 2 && data->vido == 0)
 {
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
  gettext("Цена"), renderer,"text", COL_CENA,NULL);
 }

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

//printf("uosdok_add_columns end\n");

}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void uosdok_create_list (class uosdok_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row,row1;

//printf("uosdok_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(uosdok_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(uosdok_vibor),data);

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

sprintf(strsql,"select podt,innom,bs,iz,bsby,izby,cena,shs,ktoz,vrem from \
Uosdok1 where datd='%s' and nomd='%s' and tipz=%d order by innom asc",
data->data_dok.ravno_sqldata(),data->nomd.ravno(),data->tipz);
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
double i_bsnu=0.,i_iznu=0.,i_bsbu=0.,i_izbu=0.;
double bsnu=0.,iznu=0.,bsbu=0.,izbu=0.;

while(cur.read_cursor(&row) != 0)
 {
  
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  bsnu=atof(row[2]);
  iznu=atof(row[3]);
  bsbu=atof(row[4]);
  izbu=atof(row[5]);
      
  i_bsnu+=bsnu;
  i_iznu+=iznu;
  i_bsbu+=bsbu;
  i_izbu+=izbu;

  //Метка записи
  ss[COL_PODTV].new_plus("");
  if(row[0][0] == '1')
   ss[COL_PODTV].new_plus("**");
   
  //Инвентарный номер
  ss[COL_INNOM].new_plus(row[1]);
  
  //Узнаём наименование основного средства

  ss[COL_NAIM].new_plus("");
  sprintf(strsql,"select naim from Uosin where innom=%d",atoi(row[1]));
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    ss[COL_NAIM].new_plus(iceb_u_toutf(row1[0]));

  //балансовая стоимость для налогового учёта
  sprintf(strsql,"%.2f",bsnu);
  ss[COL_BSNU].new_plus(strsql);

  //износ для налогового учёта
  sprintf(strsql,"%.2f",iznu);
  ss[COL_IZNU].new_plus(strsql);

  //балансовая стоимость для бухгалтерского учёта
  sprintf(strsql,"%.2f",bsbu);
  ss[COL_BSBU].new_plus(strsql);
  

  //износ для бухгалтерского учёта
  sprintf(strsql,"%.2f",izbu);
  ss[COL_IZBU].new_plus(strsql);
  
  /*Счёт списания/приобретения*/
  ss[COL_SHSP].new_plus(row[7]);  
  /*Цена*/
  ss[COL_CENA].new_plus(row[6]);
  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[9])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[8],0,data->window));

  

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_PODTV,ss[COL_PODTV].ravno(),
  COL_INNOM,ss[COL_INNOM].ravno(),
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_BSNU,ss[COL_BSNU].ravno(),
  COL_IZNU,ss[COL_IZNU].ravno(),
  COL_BSBU,ss[COL_BSBU].ravno(),
  COL_IZBU,ss[COL_IZBU].ravno(),
  COL_CENA,ss[COL_CENA].ravno(),
  COL_SHSP,ss[COL_SHSP].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

uosdok_add_columns (data);

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK7]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK7]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),TRUE);//Доступна
 }
gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

class iceb_u_str zagol;
zagol.plus(data->hapka.ravno());

if(data->kolzap > 0)
 {
  sprintf(strsql,"%s:%d %s:%.2f/%.2f %s:%.2f/%.2f",
  gettext("Количество записей"),
  data->kolzap,
  gettext("Нал.учёт"),
  i_bsnu,i_iznu,
  gettext("Бух.учёт"),
  i_bsbu,i_izbu);
  zagol.ps_plus(strsql);
 }
 
gtk_label_set_text(GTK_LABEL(data->label_rek_dok),zagol.ravno_toutf());

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));
}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void uosdok_vibor(GtkTreeSelection *selection,class uosdok_data *data)
{
//printf("uosdok_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *innom;
gint  nomer;

gtk_tree_model_get(model,&iter,
COL_INNOM,&innom,
NUM_COLUMNS,&nomer,-1);

data->innom_v.new_plus(innom);
data->snanomer=nomer;

g_free(innom);
//printf("uosdok_vibor end\n");

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void uosdok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class uosdok_data *data)
{
//Корректировка записи
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");


}

/******************************/
/*Чтение реквизитов шапки документа*/
/************************************/
int l_uosdok_read_rhd(class uosdok_data *data,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select * from Uosdok where datd='%s' and \
nomd='%s'",data->data_dok.ravno_sqldata(),data->nomd.ravno());

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  
  sprintf(strsql,"%s %s %s !",
  gettext("Не найден документ"),
  data->nomd.ravno(),
  data->data_dok.ravno());
  
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }


data->tipz=atoi(row[1]);
if(data->tipz == 1)
 data->tipz_p=2;
else
 data->tipz_p=1;
 
data->pnds=atof(row[23]);

class iceb_u_str nomdp;
nomdp.new_plus(row[8]);

data->kontr.new_plus(row[3]);
data->kod_op.new_plus(row[2]);
data->podr=atoi(row[5]);
data->mat_ot=atoi(row[6]);
data->podr_p=atoi(row[7]);
data->nomd_p.new_plus(row[8]);
data->mat_ot_p=atoi(row[9]);
data->lnds=atoi(row[22]);

/*Узнаём наименование контрагента*/
class iceb_u_str naim_kontr("");
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kontr.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_kontr.new_plus(row[0]);

/*Узнаём наименование операции*/
class iceb_u_str naim_kod_op("");
if(data->tipz == 1)
 sprintf(strsql,"select naik,vido,prov from Uospri where kod='%s'",data->kod_op.ravno());

if(data->tipz == 2)
 sprintf(strsql,"select naik,vido,prov from Uosras where kod='%s'",data->kod_op.ravno());

data->metkaprov=0;
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  naim_kod_op.new_plus(row[0]);
  data->vido=atoi(row[1]);
  data->metkaprov=atoi(row[2]);
 }
/*Узнаём фамилию материально-ответственного*/
class iceb_u_str fio_mat_ot("");
sprintf(strsql,"select naik from Uosol where kod=%d",data->mat_ot);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 fio_mat_ot.new_plus(row[0]);

/*Узнаём наименование подразделения*/
class iceb_u_str naim_podr("");
sprintf(strsql,"select naik from Uospod where kod=%d",data->podr);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_podr.new_plus(row[0]);

sprintf(strsql,"%s %s %s:%s\n",
gettext("Дата документа"),
data->data_dok.ravno(),
gettext("Номер"), 
data->nomd.ravno());

data->hapka.new_plus(strsql);

sprintf(strsql,"%s:%s/%s\n",
gettext("Контрагент"),
data->kontr.ravno(),naim_kontr.ravno());

data->hapka.plus(strsql);

sprintf(strsql,"%s:%d/%s\n",
gettext("Подразделение"),
data->podr,naim_podr.ravno());

data->hapka.plus(strsql);

sprintf(strsql,"%s:%d/%s\n",
gettext("Материально-ответственный"),
data->mat_ot,fio_mat_ot.ravno());

data->hapka.plus(strsql);

sprintf(strsql,"%s:%s/%s ",
gettext("Операция"),
data->kod_op.ravno(),naim_kod_op.ravno());

data->hapka.plus(strsql);

if(data->tipz == 1)
 sprintf(strsql,"(%s)",gettext("Приход"));
if(data->tipz == 2)
 sprintf(strsql,"(%s)",gettext("Расход"));

data->hapka.plus(strsql);

if(nomdp.getdlinna() > 1)
 {
  sprintf(strsql,"\n%s:%s",gettext("Номер парного документа"),nomdp.ravno());
  data->hapka.plus(strsql);
 }

return(0);
}

/*****************************/
/*Ввод инвентарного номера*/
/***************************/

int l_uosdok_vin(class  uosdok_data *data)
{
//printf("l_uosdok_vin-start\n");

char strsql[512];

/*Проверяем дату*/
if(iceb_pbpds(data->data_dok.ravno(),data->window) != 0)
 {
  sprintf(strsql,gettext("Дата %s г. заблокирована!"),data->data_dok.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }


int metka_v=1;
if(data->tipz == 1 && data->vido == 0)
   metka_v=0;
class iceb_u_str innom("");   

if(uosgetinn(&innom,metka_v,data->window) != 0)
 return(1);

short dd=0,md=0,gd=0;
iceb_u_rsdat(&dd,&md,&gd,data->data_dok.ravno(),1);

SQL_str row;
class SQLCURSOR cur;
if(data->tipz == 1 && data->vido == 0) /*Ввод в документ нового инвентарного номера*/
 {
  if(iceb_provinnom(innom.ravno(),data->window) != 0)
   return(1);
   
  /*Проверяем есть ли уже такой инвентарный номер*/
  sprintf(strsql,"select naim from Uosin where innom=%d",innom.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   {
    sprintf(strsql,"%s\n%d %s",gettext("Такой инвентарный номер уже есть!"),
    innom.ravno_atoi(),row[0]);
    iceb_menu_soob(strsql,data->window);
    return(1);   
   }
  
  double bsnu=0.,iznu=0.,bsbu=0.,izbu=0.;
  
  if(uoskart1(data->data_dok.ravno(),data->nomd.ravno(),innom.ravno_atoi(),&bsnu,&iznu,&bsbu,&izbu,data->window) != 0)
   return(1);

  return(uoszvdok(dd,md,gd,data->tipz,0,innom.ravno_atoi(),data->nomd.ravno(),data->podr,data->mat_ot,
  1,bsnu,iznu,0.,data->kod_op.ravno(),bsbu,izbu,0.,"",data->window));

 }


/*Ввод в документ существующего инвентарного номера*/

/*Проверяем есть ли уже такой инвентарный номер*/
sprintf(strsql,"select naim from Uosin where innom=%d",innom.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s :%d!",gettext("Ненайден инвентарный номер"),innom.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);   
 }
int podr=0;
int kod_mo=0;

if(poiinw(innom.ravno_atoi(),data->data_dok.ravno(),&podr,&kod_mo,data->window) != 0)
 {
  sprintf(strsql,"%s %s!",gettext("Инвентарный номер не числится на дату"),data->data_dok.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

/*Если документ расходный или приходный с операцией изменения стоимости*/

if(data->tipz == 2 || (data->tipz == 1 && data->vido == 2))
 {
  if(data->podr != podr)
   {
    
    sprintf(strsql,"%s %d",gettext("Этот инвентарный номер находится в подразделении"),podr);
    iceb_menu_soob(strsql,data->window);
    return(1);
   }

  if(data->mat_ot != kod_mo)
   {
    sprintf(strsql,"%s %d",gettext("Этот инвентарный номер числится за мат. ответственным"),kod_mo);
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }

class poiinpdw_data rek_in; 
/*Читаем переменные данные*/
if(poiinpdw(innom.ravno_atoi(),md,gd,&rek_in,data->window) != 0)
 {
  iceb_menu_soob(gettext("Ненайдены переменные данные!"),data->window);
  return(1);
 }

class bsizw_data bal_st;
double bs=0.;
double iz=0.;
double bsby=0.;
double izby=0.;
int kol=0;
if(data->tipz == 2 && data->vido != 2)
 {
//  if(iceb_u_proverka(spgnupa,rek_in.hna.ravno(),0,0) == 0)
  if(uosprovgrw(rek_in.hna.ravno(),data->window) == 0)
   {
    sprintf(strsql,"select innom from Uosamor where \
god=%d and mes=%d and innom=%d",gd,md,innom.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 0)
     { 
      
      sprintf(strsql,"%s %d.%d%s\n%s",
      gettext("Не расчитана амортизация за"),md,gd,
      gettext("г."),
      gettext("Рассчитать ?"));
      
      if(iceb_menu_danet(strsql,2,data->window) == 1)
       {
        int kvrt=0; 
        if(iceb_u_SRAV(rek_in.hna.ravno(),"1",0) == 0)
         {
          if(md < 4)
           kvrt=1;
          if(md >= 4 && md < 7)
           kvrt=2;
          if(md >= 7 && md < 10)
           kvrt=3;
          if(md >= 10 && md <= 12)
           kvrt=4;
         }
        amortw(innom.ravno_atoi(),md,gd,1,kvrt,"",NULL,data->window);
       }
     }
   }



  kol=-1;
  short dr=1;
  short gr=gd;
  short mr=0;  
  if(md <= 12)
   mr=12;
  if(md <= 9)
   mr=9;
  if(md <= 6)
   mr=6;
  if(md <= 3)
   mr=3;

  iceb_u_dpm(&dr,&mr,&gr,3);//на начало следующего квартала

  //Для налогового учета берем все до конца квартала
  bsizw(innom.ravno_atoi(),podr,dr,mr,gr,&bal_st,NULL,data->window);
  bs=bal_st.bs+bal_st.sbs;
  iz=bal_st.iz+bal_st.siz+bal_st.iz1;

  dr=1; mr=md; gr=gd;
  iceb_u_dpm(&dr,&mr,&gr,3);//на начало следующего месяца
  //Для бухгалтерского учета берем все до конца месяца

  bsizw(innom.ravno_atoi(),podr,dr,mr,gr,&bal_st,NULL,data->window);
  bsby=bal_st.bsby+bal_st.sbsby;
  izby=bal_st.izby+bal_st.sizby+bal_st.iz1by;

 }


if(data->tipz == 1 && data->vido != 2)
 {
  kol=1;
   bsizw(innom.ravno_atoi(),data->podr,dd,md,gd,&bal_st,NULL,data->window);

   bs=bal_st.bs+bal_st.sbs;
   iz=bal_st.iz+bal_st.iz1+bal_st.siz;
   bsby=bal_st.bsby+bal_st.sbsby;
   izby+=bal_st.izby+bal_st.iz1by+bal_st.sizby;
 }

int metka_m=0;
if(data->tipz == 1 && data->vido == 0)
 metka_m=1;
if(data->tipz == 2 && data->vido == 0)
 metka_m=2;
 
class iceb_u_str shetsp("");
double cena=0.;
if(data->vido == 2) /*Для операций изменения стоимости*/
 {
  kol=0;
 }
 
if(l_uosdok_v(innom.ravno_atoi(),&bs,&iz,&bsby,&izby,&shetsp,&cena,data->tipz,metka_m,0,0,data->window) != 0)
 return(1);
  

if(uoszvdok(dd,md,gd,data->tipz,0,innom.ravno_atoi(),data->nomd.ravno(),data->podr,data->mat_ot,
kol,bs,iz,0.,data->kod_op.ravno(),bsby,izby,cena,shetsp.ravno(),data->window) != 0)
 return(1);


if(data->vido == 1)
 {
  printf("Пишем в парный документ\n");
  uoszvdok(dd,md,gd,data->tipz_p,0,innom.ravno_atoi(),data->nomd_p.ravno(),data->podr_p,data->mat_ot_p,
  kol*(-1),bs,iz,0.,data->kod_op.ravno(),bsby,izby,cena,shetsp.ravno(),data->window);
  printf("записали\n");
 }

return(0);
}

/*********************************/
/*Корректировать запись*/
/****************************/
int l_uosdok_kz(class  uosdok_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select bs,iz,bsby,izby,cena,shs,ktoz,vrem from Uosdok1 where \
datd='%s' and nomd='%s' and innom=%d",
data->data_dok.ravno_sqldata(),
data->nomd.ravno(),
data->innom_v.ravno_atoi());

if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  iceb_menu_soob(gettext("Не найдена запись для корректировки!"),data->window);
  return(1);
 }

double bs=atof(row[0]);
double iz=atof(row[1]);
double bsby=atof(row[2]);
double izby=atof(row[3]);
double cena=atof(row[4]);
class iceb_u_str shetsp;
shetsp.plus(row[5]);
int ktoz=atoi(row[6]);
time_t vrem=atol(row[7]);


int metka_m=0;
if(data->tipz == 1 && data->vido == 0)
 metka_m=1;
if(data->tipz == 2 && data->vido == 0)
 metka_m=2;


if(l_uosdok_v(data->innom_v.ravno_atoi(),&bs,&iz,&bsby,&izby,&shetsp,&cena,data->tipz,metka_m,ktoz,vrem,data->window) != 0)
 return(1);


/*Проверяем дату*/
if(iceb_pbpds(data->data_dok.ravno(),data->window) != 0)
 {
  sprintf(strsql,gettext("Дата %s г. заблокирована!"),data->data_dok.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

sprintf(strsql,"update Uosdok1 \
set \
bs=%.2f,\
iz=%.2f,\
bsby=%.2f,\
izby=%.2f,\
cena=%.2f,\
ktoz=%d,\
vrem=%ld,\
shs='%s' \
where datd='%s' and nomd='%s' and innom=%d",
bs,
iz,
bsby,
izby,
cena,
iceb_getuid(data->window),
time(NULL),
shetsp.ravno(),
data->data_dok.ravno_sqldata(),
data->nomd.ravno(),data->innom_v.ravno_atoi());

iceb_sql_zapis(strsql,0,0,data->window);

return(0);
}
/**********************************/
/*Удаление*/
/***********************************/
int l_uosdok_ud(int knop,class  uosdok_data *data)
{
char strsql[512];

if(iceb_pbpds(data->data_dok.ravno(),data->window) != 0)
 {
  sprintf(strsql,gettext("Дата %s г. заблокирована!"),data->data_dok.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,data->data_dok.ravno(),1);

/*Проверка возможности удаления*/
if(knop == FK3 )
 {

  if(data->tipz == 1)
   if(puzapw(data->innom_v.ravno_atoi(),data->podr,dd,md,gd,data->nomd.ravno(),data->window) != 0)
    {
     iceb_menu_soob(gettext("Невозможно удаление записи, был расход!"),data->window);
     return(1);
    }

  if(data->tipz == 2 && data->vido == 1)
   if(puzapw(data->innom_v.ravno_atoi(),data->podr_p,dd,md,gd,data->nomd_p.ravno(),data->window) != 0)
    {
     iceb_menu_soob(gettext("Невозможно удаление записи, в парном документе был расход!"),data->window);
     return(1);
    }
 }

if(knop == SFK3)
 {
  if(data->tipz == 1)
   if(pudokw(data->podr,dd,md,gd,data->nomd.ravno(),data->tipz,data->window) != 0)
    {
     iceb_menu_soob(gettext("Невозможно удаление документа!"),data->window);
     return(1);
    }
  if(data->tipz == 2 && data->vido == 1)
   {       
    if(pudokw(data->podr_p,dd,md,gd,data->nomd_p.ravno(),1,data->window) != 0)
     {
      iceb_menu_soob(gettext("Невозможно удаление парного документа!"),data->window);
      return(1);
     }
   }
 }

if(knop == FK3)
 if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
   return(1);
if(knop == SFK3)
 if(iceb_menu_danet(gettext("Удалить документ ? Вы уверены ?"),2,data->window) == 2)
   return(1);

if(knop == SFK3)
 {

  /*Удаляем проводки*/
  if(iceb_udprgr(gettext("УОС"),dd,md,gd,data->nomd.ravno(),data->podr,data->tipz,data->window) != 0)
   return(1);
   
  if(data->vido == 1)
    if(iceb_udprgr(gettext("УОС"),dd,md,gd,data->nomd.ravno(),data->podr_p,data->tipz_p,data->window) != 0)
     return(1);

  /*Удаляем записи в документе*/
  uduoszdw(0,dd,md,gd,data->nomd.ravno(),data->window);
  if(data->vido == 1)
    uduoszdw(0,dd,md,gd,data->nomd_p.ravno(),data->window);

  /*Удаляем заголовок документа*/
  uduosdokw(dd,md,gd,data->nomd.ravno(),data->window);
  if(data->vido == 1)
    uduosdokw(dd,md,gd,data->nomd_p.ravno(),data->window);
 }

if(knop == FK3)
 {
  uduoszdw(data->innom_v.ravno_atoi(),dd,md,gd,data->nomd.ravno(),data->window);
  if(data->vido == 1)
    uduoszdw(data->innom_v.ravno_atoi(),dd,md,gd,data->nomd_p.ravno(),data->window);
 }
return(0);
}
/***********************/
/*Акты приёма-передачи*/
/************************/

void l_uosdok_app(class  uosdok_data *data)
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Распечатка акта приёма-передачи"));//0
punkt_m.plus(gettext("Распечатка акта приёма-передачи (Типовая форма N03-1"));//2


int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);
switch(nomer)
 {
  case 0:
   rasactuosw(data->data_dok.ravno(),data->nomd.ravno(),data->window);
   break;

  case 1:
   uosactw(data->data_dok.ravno(),data->nomd.ravno(),data->window);
   break; 
 }
}

/*********************/
/*Распечатка приходных документов*/
/*********************************/

void l_uosdok_f5p(class  uosdok_data *data)
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Распечатка акта приёма-передачи"));//0
punkt_m.plus(gettext("Распечатка акта ремонта/дооборудования"));//1
punkt_m.plus(gettext("Распечатка накладной"));//2


int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);
switch(nomer)
 {
  case 0:
   l_uosdok_app(data);
   break;

  case 1:
   uosremw(data->data_dok.ravno(),data->nomd.ravno(),data->window);
   break; 
   
  case 2:
   rasdokuosw(data->data_dok.ravno(),data->podr,data->nomd.ravno(),data->tipz,data->window);
   break;
 }
}
/*********************/
/*Распечатка расходных документов*/
/*********************************/

void l_uosdok_f5r(class  uosdok_data *data)
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Распечатка акта приёма-передачи"));//0
punkt_m.plus(gettext("Распечатка акта ремонта/дооборудования"));//1
punkt_m.plus(gettext("Распечатка накладной"));//2
punkt_m.plus(gettext("Распечатка налоговой накладной"));//3
punkt_m.plus(gettext("Распечатка акта списания основного средства"));//4
punkt_m.plus(gettext("Распечатка акта списания транспортного средства"));//5


int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);
char imaf[56];
class iceb_u_spisok imafil;
class iceb_u_spisok naimf;
switch(nomer)
 {
  case 0:
   l_uosdok_app(data);
   break;

  case 1:
   uosremw(data->data_dok.ravno(),data->nomd.ravno(),data->window);
   break; 
  
  case 2:
   rasdokuosw(data->data_dok.ravno(),data->podr,data->nomd.ravno(),data->tipz,data->window);
   break;

  case 3:
    
    sprintf(imaf,"nn%d.lst",getpid());
    if(rasnalnuosw(data->data_dok.ravno(),data->podr,data->nomd.ravno(),imaf,data->window) != 0)
     break;
    imafil.plus(imaf);
    naimf.plus(gettext("Распечатка налоговой накладной"));    
    iceb_rabfil(&imafil,&naimf,"",0,data->window);

    break;

  case 4:
    uossosw(data->data_dok.ravno(),data->nomd.ravno(),data->window);
    break;

  case 5:
    uossasw(data->data_dok.ravno(),data->nomd.ravno(),data->window);
 }
}
/***************/
/*Подтверждение*/
/****************/


void l_uosdok_podtv(int innom,short tipz,const char *data_dok,
const char *nomdok,
short met, //0-с запросом 1- без
short mo, //0-установить подтверждение 1-снять подтв.
int podr,
GtkWidget *wpredok)
{
char		strsql[512];


if(met == 0)
 {
  if(innom == 0)
   {
    if(mo == 0)
      if(iceb_menu_danet(gettext("Подтвердить весь документ ? Вы уверены ?"),2,wpredok) == 2)
       return;
    if(mo == 1)
      if(iceb_menu_danet(gettext("Снять подтверждение со всего документа ? Вы уверены ?"),2,wpredok) == 2)
       return;
   }
  else
   if(iceb_menu_danet(gettext("Подтвердить запись в документе ? Вы уверены ?"),2,wpredok) == 2)
       return;
 }
short d,m,g;
iceb_u_rsdat(&d,&m,&g,data_dok,1);
/*Подтвердить*/

if(mo == 0)
 {
  if(innom != 0)
    sprintf(strsql,"update Uosdok1 \
set \
podt=1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tipz=%d and \
innom=%d",
    g,m,d,podr,nomdok,tipz,innom);
  else
    sprintf(strsql,"update Uosdok1 \
set \
podt=1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tipz=%d",
    g,m,d,podr,nomdok,tipz);
 }

/*Снять подтверждение*/

if(mo == 1)
 {
  if(innom != 0)
    sprintf(strsql,"update Uosdok1 \
set \
podt=0 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tipz=%d and \
innom=%d",
    g,m,d,podr,nomdok,tipz,innom);
  else
    sprintf(strsql,"update Uosdok1 \
set \
podt=0 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tipz=%d",
    g,m,d,podr,nomdok,tipz);
 }

/*
printw("\n%s\n",strsql);
OSTANOV();
*/

iceb_sql_zapis(strsql,1,0,wpredok);



}
/*************************************/
/*Выход из документа*/
/*************************/
void l_uosdok_f10(class uosdok_data *data)
{
short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,data->data_dok.ravno(),1);

 poduosdw(dd,md,gd,data->nomd.ravno(),data->window);
 opluosw(dd,md,gd,data->tipz,data->nomd.ravno(),data->kod_op.ravno(),data->window);

 if(data->vido == 1)
  {
   poduosdw(dd,md,gd,data->nomd_p.ravno(),data->window);
   opluosw(dd,md,gd,data->tipz_p,data->nomd_p.ravno(),data->kod_op.ravno(),data->window);
  }
 if(data->metkaprov == 1)
   return;

 if(data->vido != 0 && data->tipz == 1)
   return;

 if(data->tipz == 2 || (data->tipz == 1 && 
 iceb_u_SRAV(data->kod_op.ravno(),gettext("ОСТ"),0) != 0))
  {
   class iceb_u_spisok spshet;
   class iceb_u_double sumsh;
   class iceb_u_double sumshp;
   class iceb_u_str shet_suma;       

   prpuosw(dd,md,gd,data->nomd.ravno(),data->podr,&spshet,&sumsh,&sumshp,1,&shet_suma,data->window);
   prpuosw(dd,md,gd,data->nomd.ravno(),data->podr,&spshet,&sumsh,&sumshp,0,&shet_suma,data->window);
  }

}



