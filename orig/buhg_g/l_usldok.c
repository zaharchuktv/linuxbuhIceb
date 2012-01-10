/*$Id: l_usldok.c,v 1.76 2011-06-07 08:52:26 sasa Exp $*/
/*06.06.2011	15.08.2005	Белых А.И.	l_usldok.c
Работа с документом в подсистеме "Учёт услуг"
Если вернули 0-вышли нажав клавишу f10
             1-удалили документ
*/
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>
#include <unistd.h>
#include "buhg_g.h"
enum
{
 SFK1,
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 SFK5,
 FK6,
 FK7,
 SFK7,
 FK8,
 FK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_METKA,
 COL_KODZAP,
 COL_NAIM,
 COL_EI,
 COL_SHET,
 COL_KOLIH,
 COL_CENA,
 COL_SUMA,
 COL_KOLIH_POD,
 COL_DATA_VREM,
 COL_KTO,  
 COL_METKA_INT,
 COL_NZ,
 NUM_COLUMNS
};

class  usldok_data
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
  short dd,md,gd;
  int   tipz;
  int   podr;
  iceb_u_str nomd;
  float pnds; //Процент НДС на момент создания документа
  
  iceb_u_str nomdp; //Номер документа поставщика
  iceb_u_str kontr;
  iceb_u_str naim_kontr;
  iceb_u_str kodop;
  iceb_u_str naim_kodop;
  iceb_u_str naimpodr;
  short		metkaprov;
  short lnds;
  iceb_u_str naimkor; //Наименование вида снижения цены
  int blokpid; //Код оператора заблокировавшего документ  
  iceb_u_double maskor;
  double sumkor;  
  double suma;
  short pro;
  int kolihz;
  double kolihu;
  double sumaitogo;
  float procent;  //Процент дополнительного налога
  double sumandspr;
  time_t vrem_zap;
  
  class iceb_u_str naimnal; //Наименование дополнительного налога

  //реквизиты выбранной записи
  iceb_u_str kodzapv;
  int metkazapv;
  int nomzapv;
  iceb_u_str naim_zapv; //Наименование выбранной записи
  //Имена файлов
  class iceb_u_spisok imaf;
  
  //Конструктор
  usldok_data()
   {
    sumandspr=0.;
    voz=0;
    clear_rek_dok();
    metkazapv=nomzapv=0;
    kodzapv.plus("");
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    pnds=0.;
    naim_zapv.plus("");    
   }      
  void clear_rek_dok()
   {
    naimnal.new_plus("");
    procent=0.;
    sumaitogo=0.;
    kolihz=0;
    kolihu=0.;
    pro=0;
    suma=0.;  
    sumkor=0.;
    blokpid=0;
    naimkor.new_plus("");
    lnds=0;
    metkaprov=0;
    kontr.new_plus("");
    naim_kontr.new_plus("");
    kodop.new_plus("");
    naim_kodop.new_plus("");
    naimpodr.new_plus("");
    vrem_zap=0;
   }

 };

gboolean   usldok_key_press(GtkWidget *widget,GdkEventKey *event,class usldok_data *data);
void usldok_vibor(GtkTreeSelection *selection,class usldok_data *data);
void usldok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class usldok_data *data);
void  usldok_knopka(GtkWidget *widget,class usldok_data *data);
void usldok_add_columns (GtkTreeView *treeview);
void usldok_create_list(class usldok_data *data);

int l_usldok_rd(class usldok_data *data,GtkWidget *wpredok);
void l_usldok_str_rek(class usldok_data *data,iceb_u_str *sap);
int vuslw(short dd,short md,short gd,int tipz,int podr,const char *nomdok,int metka,float pnds,GtkWidget *wpredok);
int l_usldok_prblok(short mp,short md,short gd,int blokpid,GtkWidget *wpredok);
void deletdokumu(class iceb_u_spisok *imaf);
void l_prov_usl(short dd,short md,short gd,int podr,int tipz,const char *nomdok,const char *nomdokp,const char *kontr,const char *kodop,int lnds,double sumad,double sumkor,float pnds,GtkWidget *wpredok);
void podudokw(short d,short m,short g,const char nn[],int podr,int tipz,GtkWidget *wpredok);
void l_usldok_f10(class usldok_data *data);
void rasdokuw(short dg,short mg,short gg,int podr,const char *nomdok,short lnds,short vidr,FILE *ff,int tipz,int saldo,GtkWidget *wpredok);
void l_usldok_sozfu(class usldok_data *data);
void l_usldok_r1(class usldok_data *data);
void l_usldok_r2(class usldok_data *data);
int usl_shet_fakw(short dg,short mg,short gg,int podr,const char *nomdok,iceb_u_str *imafil,int tipz,GtkWidget *wpredok);
int rasnalnuw(short dd,short md,short gd,int podr,const char *nomdok,const char *imaf,short tipnn,GtkWidget *wpredok);
int usl_act_vrw(short dg,short mg,short gg,int podr,const char *nomdok,iceb_u_str *imafil,int tipz,GtkWidget *wpredok);
void  sozfusal(class usldok_data *data);
void l_usldok_pros_red2(class usldok_data *data);
void l_usldok_pros_red1(class usldok_data *data);
void l_uslkart(int metka_zap,int kodzap,GtkWidget *wpredok);
int   podzapusl(short dd,short md,short gd,int tipz,int podr,const char *nomdok,int kodzap,int metka_zap,int nom_zap,GtkWidget *wpredok);
void l_usldok_pod_dok(class usldok_data *data);
void l_usldok_snkz(class usldok_data *data);
void l_usldok_F8(class usldok_data *data);
void l_usldok_read_sap(class usldok_data *data,GtkWidget *wpredok);
int copdokuw(short dd,short md,short gd,const char *nomdok,int pod,int tipz,GtkWidget *wpredok);
int	prilogw(char mr,int podr,short god,short tipz,const char *nomd,const char *imaf,GtkWidget *wpredok);
void usl_spismat(short dd,short md,short gd,const char *nomdok,int podr,int tipz,const char *,int,float,GtkWidget *wpredok);
void perecenuw(short dd,short mm,short gg,const char *nn,int podr,int tp,float pnds,GtkWidget *wpredok);
void l_usldok_F9r(class usldok_data *data);
void l_usldok_F9p(class usldok_data *data);
int l_usldok_snn(class  usldok_data *data);

extern SQL_baza	bd;
extern char *name_system;
extern double   okrg1;
extern double	okrcn;  /*Округление цены*/
short    mvnp; /*0-внешняя 1 -внутреняя*/
//extern double   nds1; /*Н.Д.С.*/
extern char *organ;
extern char *imabaz;

int l_usldok(short dd,short md,short gd, //Дата документа
int tipz,  //1-приход 2-расход
int podr,  //Подразделение
const char *nomd, //Номер документа
GtkWidget *wpredok)
{
class  usldok_data data;
iceb_u_str string;
char bros[512];
//char strsql[512];
data.dd=dd;
data.md=md;
data.gd=gd;
data.tipz=tipz;
data.podr=podr;
data.nomd.plus(nomd);

//читаем реквизиты документа формируем имена распечаток
l_usldok_read_sap(&data,wpredok);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);

if(tipz == 1)
 sprintf(bros,"%s %s",name_system,gettext("Работа с документом на полученные услуги"));
if(tipz == 2)
 sprintf(bros,"%s %s",name_system,gettext("Работа с документом на выполненные услуги"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(usldok_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);


data.label_rek_dok=gtk_label_new ("");
gtk_box_pack_start (GTK_BOX (vbox2),data.label_rek_dok,FALSE, FALSE, 0);

data.label_red=gtk_label_new ("");
gtk_box_pack_start (GTK_BOX (vbox2),data.label_red,FALSE, FALSE, 0);

GdkColor color;
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_red,GTK_STATE_NORMAL,&color);

//формируем заголовок с реквизитами документа
l_usldok_str_rek(&data,&string);

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
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK1]), "clicked",GTK_SIGNAL_FUNC(usldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK1],TRUE,TRUE, 0);
tooltips[SFK1]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK1],data.knopka[SFK1],gettext("Просмотр шапки документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK1]),(gpointer)SFK1);
gtk_widget_show(data.knopka[SFK1]);

sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(usldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(usldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(usldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(usldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удалить документ"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Проводки"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(usldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Переход в режим работы с проводками для этого документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(usldok_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка документов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"%sF5 %s",RFK,gettext("Просмотр"));
data.knopka[SFK5]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK5]), "clicked",GTK_SIGNAL_FUNC(usldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK5],TRUE,TRUE, 0);
tooltips[SFK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK5],data.knopka[SFK5],gettext("Просмотр распечатки документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK5]),(gpointer)SFK5);
gtk_widget_show(data.knopka[SFK5]);

sprintf(bros,"F6 %s",gettext("Карточка"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(usldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Просмотр карточки материалла"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Подтверждение"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(usldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Подтверждение записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"%sF7 %s",RFK,gettext("Подтверждение"));
data.knopka[SFK7]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK7]), "clicked",GTK_SIGNAL_FUNC(usldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK7],TRUE,TRUE, 0);
tooltips[SFK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK7],data.knopka[SFK7],gettext("Подтверждение всех записей в документе"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK7]),(gpointer)SFK7);
gtk_widget_show(data.knopka[SFK7]);

sprintf(bros,"F8 %s",gettext("Меню"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(usldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Получение меню для вибора нужного режима работы"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);

sprintf(bros,"F9 %s",gettext("Меню"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(usldok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Получение меню для выбора нужного режима работы"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(usldok_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);

gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

usldok_create_list(&data);
gtk_widget_show(data.window);

//if(metka == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


//printf("l_usldok end\n");

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  usldok_knopka(GtkWidget *widget,class usldok_data *data)
{
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("usldok_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case SFK1:
    vdnuw(&data->dd,&data->md,&data->gd,data->tipz,&data->nomd,&data->podr,data->window);

    //удаляем здесь так как имена файлов после чтения шапки могут измениться
    deletdokumu(&data->imaf);
 
    //читаем реквизиты документа и формируем наименования распечаток
    l_usldok_read_sap(data,data->window);

    usldok_create_list(data);

    return;  

  case FK2:
    if( l_usldok_prblok(0,data->md,data->gd,data->blokpid,data->window) != 0)
     return;

    vuslw(data->dd,data->md,data->gd,data->tipz,data->podr,data->nomd.ravno(),1,data->pnds,data->window);
    deletdokumu(&data->imaf);
    usldok_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
     return;
    if( l_usldok_prblok(0,data->md,data->gd,data->blokpid,data->window) != 0)
     return;

    vuslw_v(data->dd,data->md,data->gd,data->tipz,data->podr,data->nomd.ravno(),data->kodzapv.ravno(),data->metkazapv,data->nomzapv,data->pnds,data->window);

    deletdokumu(&data->imaf);
    usldok_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
     return;

    if( l_usldok_prblok(0,data->md,data->gd,data->blokpid,data->window) != 0)
     return;

    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
     return;

    uduvdokw(data->tipz,data->dd,data->md,data->gd,data->nomd.ravno(),data->podr,\
    data->metkazapv,data->kodzapv.ravno_atoi(),data->nomzapv,data->window);

    deletdokumu(&data->imaf);

    usldok_create_list(data);

    return;  

  case SFK3:

    if( l_usldok_prblok(0,data->md,data->gd,data->blokpid,data->window) != 0)
     return;
      /*Смотрим заблокированы ли проводки к этому документа*/
    if(iceb_pbp(6,data->dd,data->md,data->gd,data->nomd.ravno(),data->podr,data->tipz,gettext("Удалить документ невозможно!"),data->window) != 0)
      return;  

    if(iceb_menu_danet(gettext("Удалить документ ? Вы уверены ?"),2,data->window) == 2)
     return;

    if(iceb_udprgr(gettext("УСЛ"),data->dd,data->md,data->gd,data->nomd.ravno(),data->podr,data->tipz,data->window) != 0)
     return;

    deletdokumu(&data->imaf);

    if(uduvdokw(data->tipz,data->dd,data->md,data->gd,data->nomd.ravno(),data->podr,0,0,0,data->window) != 0)
      return;
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    if(data->kolzap == 0)
     return;
    if(data->metkaprov == 1)
     {
      iceb_menu_soob(gettext("Для этой операции проводки не делаются !"),data->window);
      return;
     }

    if(mvnp != 0 && data->tipz == 1)
     {
      iceb_menu_soob(gettext("Для внутренних операций проводки делаются только\nдля расходных документов !"),data->window);
      return;
     }    
  

    l_prov_usl(data->dd,data->md,data->gd,data->podr,data->tipz,data->nomd.ravno(),
    data->nomdp.ravno(),data->kontr.ravno(),data->kodop.ravno(),data->lnds,data->suma,
    data->sumkor,data->pnds,data->window);

    //Читаем шапку для того чтобы прочитать метку выполнения проводок
    l_usldok_rd(data,data->window);
  
    usldok_create_list(data);
    return;  

  case FK5:
    if(data->kolzap == 0)
     return;

    l_usldok_sozfu(data);

    if(data->tipz == 1)
      l_usldok_r1(data);
    if(data->tipz == 2)
      l_usldok_r2(data);
    return;  

  case SFK5:
    l_usldok_sozfu(data);

    if(data->tipz == 1)
     l_usldok_pros_red1(data);
    if(data->tipz == 2)
     l_usldok_pros_red2(data);


    return;  

  case FK6:
    if(data->kolzap == 0)
     return;
    l_uslkart(data->metkazapv,data->kodzapv.ravno_atoi(),data->window);
    usldok_create_list(data);
    return;  

  case FK7:
    if(data->kolzap == 0)
     return;
    if( l_usldok_prblok(1,data->md,data->gd,data->blokpid,data->window) != 0)
     return;

    podzapusl(data->dd,data->md,data->gd,data->tipz,data->podr,data->nomd.ravno(),data->kodzapv.ravno_atoi(),
    data->metkazapv,data->nomzapv,data->window);

    usldok_create_list(data);
    return;  

  case SFK7:
    if(data->kolzap == 0)
     return;

    if( l_usldok_prblok(1,data->md,data->gd,data->blokpid,data->window) != 0)
     return;

    l_usldok_pod_dok(data);

    usldok_create_list(data);
    return;  

  case FK8:
    l_usldok_F8(data);
    return;  

  case FK9:
    if(data->tipz == 1)
     l_usldok_F9p(data);
    if(data->tipz == 2)
     l_usldok_F9r(data);
    return;  
    
  case FK10:
//    printf("usldok_knopka F10\n");
    l_usldok_f10(data);
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   usldok_key_press(GtkWidget *widget,GdkEventKey *event,class usldok_data *data)
{
iceb_u_str repl;
//printf("usldok_key_press keyval=%d state=%d\n",event->keyval,event->state);

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

    if( l_usldok_prblok(0,data->md,data->gd,data->blokpid,data->window) != 0)
     return(TRUE);
    vuslw(data->dd,data->md,data->gd,data->tipz,data->podr,data->nomd.ravno(),0,data->pnds,data->window);
    deletdokumu(&data->imaf);
    usldok_create_list(data);
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
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK5]),"clicked");
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
    printf("usldok_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша ! Код=%d\n",event->keyval);

    break;
 }

return(TRUE);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void usldok_create_list (class usldok_data *data)
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
iceb_u_str zagolov;

//printf("usldok_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(usldok_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(usldok_vibor),data);

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
G_TYPE_INT,
G_TYPE_INT,
G_TYPE_INT);

sprintf(strsql,"select metka,kodzap,kolih,cena,ei,shetu,dnaim,nz,shsp,ktoi,vrem from \
Usldokum1 where datd='%04d-%02d-%02d' and \
podr=%d and nomd='%s' and tp=%d order by metka,kodzap asc",
data->gd,data->md,data->dd,data->podr,data->nomd.ravno(),data->tipz);
//printf("strsql=%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
double kolih,cena,sum,kolih_pod=0.;
int nom_zap=0;
int metka_zap; //0-материал 1-услуга
double suma_str;
data->kolihz=0;
data->kolihu=0.;
data->suma=0.;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  data->kolihz++;
  data->kolihu+=atof(row[2]);
  cena=atof(row[3]);
  cena=iceb_u_okrug(cena,okrcn);
  suma_str=atof(row[2])*cena;
  suma_str=iceb_u_okrug(suma_str,okrg1);
  data->suma+=suma_str;


  //Метка записи

  metka_zap=atoi(row[0]);

  ss[COL_METKA].new_plus("");
  if(metka_zap == 0)
    ss[COL_METKA].new_plus("M");
  if(metka_zap == 1)
    ss[COL_METKA].new_plus("U");

  nom_zap=atoi(row[7]);  

  //Код записи
  ss[COL_KODZAP].new_plus(row[1]);
  
  //Узнаём наименование записи

  ss[COL_NAIM].new_plus("");
  if(metka_zap == 0)
   {
    /*Читаем наименование материалла*/
    sprintf(strsql,"select naimat from Material where kodm=%s",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      ss[COL_NAIM].new_plus(iceb_u_toutf(row1[0]));
      if(row[6][0] != '\0')
       {
        ss[COL_NAIM].plus("+");
        ss[COL_NAIM].plus(iceb_u_toutf(row[6]));
       }
     }
   }  

  if(metka_zap == 1)
   {
    /*Читаем наименование услуги*/
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      ss[COL_NAIM].new_plus(iceb_u_toutf(row1[0]));
      if(row[6][0] != '\0')
       {
        ss[COL_NAIM].plus("+");
        ss[COL_NAIM].plus(iceb_u_toutf(row[6]));
       }
     }
   }  

  kolih=atof(row[2]);
  cena=atof(row[3]);
  cena=iceb_u_okrug(cena,okrcn);
   
  sum=cena*kolih;
  sum=iceb_u_okrug(sum,okrg1);

  kolih_pod=readkoluw(data->dd,data->md,data->gd,data->podr,data->nomd.ravno(),data->tipz,\
   metka_zap,atoi(row[1]),nom_zap,data->window);

  //единица измерения
  ss[COL_EI].new_plus(iceb_u_toutf(row[4]));
  //счёт
  ss[COL_SHET].new_plus(iceb_u_toutf(row[5]));
  
  //Количество
  ss[COL_KOLIH].new_plus(kolih);

  //Цена
  ss[COL_CENA].new_plus(cena);

  //сумма
  ss[COL_SUMA].new_plus(sum);

  //Количество подтверждённое
  ss[COL_KOLIH_POD].new_plus(kolih_pod);
  
  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[10])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[9],0,data->window));

  

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKA,ss[COL_METKA].ravno(),
  COL_KODZAP,ss[COL_KODZAP].ravno(),
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_EI,ss[COL_EI].ravno(),
  COL_SHET,ss[COL_SHET].ravno(),
  COL_KOLIH,ss[COL_KOLIH].ravno(),
  COL_CENA,ss[COL_CENA].ravno(),
  COL_SUMA,ss[COL_SUMA].ravno(),
  COL_KOLIH_POD,ss[COL_KOLIH_POD].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  COL_METKA_INT,metka_zap,
  COL_NZ,nom_zap,
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

usldok_add_columns (GTK_TREE_VIEW (data->treeview));

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK4]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK7]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK4]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK7]),TRUE);//Доступна
 }
gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

//формируем заголовок с реквизитами документа
l_usldok_str_rek(data,&zagolov);




sprintf(strsql,"update Usldokum set \
sumkor=%.2f where datd='%04d-%02d-%02d' and \
podr=%d and nomd='%s' and tp=%d",
data->sumkor,data->gd,data->md,data->dd,data->podr,data->nomd.ravno(),data->tipz);

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);



gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));
}
/*****************/
/*Создаем колонки*/
/*****************/

void usldok_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

printf("usldok_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Метка"), renderer,"text", COL_METKA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text",COL_KODZAP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Ед.изм"), renderer,"text", COL_EI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Счёт"), renderer,"text", COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Количество"), renderer,"text", COL_KOLIH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Цена"), renderer,"text", COL_CENA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Под-но"), renderer,"text", COL_KOLIH_POD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

printf("usldok_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void usldok_vibor(GtkTreeSelection *selection,class usldok_data *data)
{
//printf("usldok_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kodzapv;
gchar *naimzapv;
gint  metkazapv;
gint  nomzapv;
gint  nomer;

gtk_tree_model_get(model,&iter,
COL_KODZAP,&kodzapv,
COL_NAIM,&naimzapv,
COL_METKA_INT,&metkazapv,
COL_NZ,&nomzapv,
NUM_COLUMNS,&nomer,-1);

data->kodzapv.new_plus(kodzapv);
data->naim_zapv.new_plus(iceb_u_fromutf(naimzapv));
data->metkazapv=metkazapv;
data->nomzapv=nomzapv;
data->snanomer=nomer;

g_free(kodzapv);
g_free(naimzapv);

//printf("%s %s\n",data->kodzapv.ravno(),data->naim_zapv.ravno());
}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void usldok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class usldok_data *data)
{
//Корректировка записи
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");


}
/************************************/
/*Чтение реквизитов документа*/
/********************************/
//Если вернули 0- шапка документа прочитана 1-нет
int l_usldok_rd(class usldok_data *data,GtkWidget *wpredok)
{
char strsql[512];
char bros[512];

SQL_str row;
SQLCURSOR cur;

/*Читаем шапку документа*/
sprintf(strsql,"select kontr,kodop,nds,sumkor,uslpr,forop,nomnn,\
datop,blokir,pro,osnov,nomdp,datpnn,dover,datdov,sherez,pn,vrem from Usldokum where \
datd='%d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
data->gd,data->md,data->dd,data->podr,data->nomd.ravno(),data->tipz);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(bros,"%s %s %d.%d.%d %d %d !",
  gettext("Не найден документ"),
  data->nomd.ravno(),
  data->dd,data->md,data->gd,
  data->podr,
  data->tipz);
  iceb_menu_soob(bros,wpredok);
  return(1);
 }
data->clear_rek_dok();

data->kontr.new_plus(row[0]);

data->kodop.new_plus(row[1]);
data->lnds=atoi(row[2]);
data->blokpid=atoi(row[8]);
data->sumkor=atof(row[3]);
data->pro=atoi(row[9]);
data->nomdp.new_plus(row[11]);
data->pnds=atof(row[16]);
data->vrem_zap=atol(row[17]);

//printf("data->pnds=%f\n",data->pnds);

/*Читаем наименование контрагента*/
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
data->kontr.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 data->naim_kontr.new_plus(row[0]);

/*Читаем наименование подразделения*/
sprintf(strsql,"select naik from Uslpodr where kod=%d",
data->podr);
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 data->naimpodr.new_plus(row[0]);

/*Читаем наименование операции*/
mvnp=0;

if(data->tipz == 1)
 sprintf(strsql,"select naik,vido,prov from Usloper1 where kod='%s'",data->kodop.ravno());

if(data->tipz == 2)
 sprintf(strsql,"select naik,vido,prov from Usloper2 where kod='%s'",data->kodop.ravno());

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  data->naim_kodop.new_plus(row[0]);
  mvnp=atoi(row[1]);
  data->metkaprov=atoi(row[2]);
 }

usldopnalw(data->tipz,data->kodop.ravno(),&data->procent,&data->naimnal);

return(0);
}
/******************************************/
/*Формирование строки с реквизитами документа*/
/*********************************************/

void l_usldok_str_rek(class usldok_data *data,iceb_u_str *sap)
{
char strsql[512];

sprintf(strsql,"%s:%02d.%02d.%d %s:%s",
gettext("Дата"),data->dd,data->md,data->gd,
gettext("Номер документа"),data->nomd.ravno()); 

sap->new_plus(strsql);

sprintf(strsql,"%s:%s %s",gettext("Контрагент"),data->kontr.ravno(),data->naim_kontr.ravno());
sap->ps_plus(strsql);

sprintf(strsql,"%s:%d %s",gettext("Подразделение"),data->podr,data->naimpodr.ravno());
sap->ps_plus(strsql);

sprintf(strsql,"%s:%s %s",gettext("Операция"),data->kodop.ravno(),data->naim_kodop.ravno());
sap->ps_plus(strsql);
if(data->tipz == 1)
 {
  sprintf(strsql," (%s)",gettext("Приход"));
  sap->plus(strsql);
 }
if(data->tipz == 2)
 {
  sprintf(strsql," (%s)",gettext("Расход"));
  sap->plus(strsql);
 }




double		bb;
double		proc;
SQL_str		row;
SQLCURSOR cur;
if(data->lnds == 0 && data->tipz == 1)
 {
  sprintf(strsql,"select sumnds from Usldokum where \
datd='%04d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
  data->gd,data->md,data->dd,data->podr,data->nomd.ravno(),data->tipz);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   data->sumandspr=atof(row[0]);
 }

 

if(data->naimkor.getdlinna() > 1)
  sap->ps_plus(data->naimkor.ravno());



if(data->maskor.kolih() != 0 && data->naimkor.getdlinna() > 1)
   data->sumkor=makkorrw(data->suma,&data->maskor);

double suma_bez_nds=data->suma+data->sumkor;
suma_bez_nds=iceb_u_okrug(suma_bez_nds,okrg1);



proc=0.;
if(data->suma != 0.)
 proc=data->sumkor*100/data->suma;


sprintf(strsql,"%s:%d/%.10g %s:%.2f",
gettext("Количество"),data->kolihz,data->kolihu,
gettext("Сумма"),data->suma);

sap->ps_plus(strsql);


if(data->sumkor != 0.)
 {
  sprintf(strsql,"(%.2f %.2f%% %.2f)",data->sumkor,proc,data->suma+data->sumkor);
  sap->plus(strsql);
 }

double sumands=0.;
if(data->lnds == 0)
 {
  double nds1=iceb_pnds(data->dd,data->md,data->gd,data->window);
  if(data->sumandspr != 0.)
   sumands=data->sumandspr;
  else
   sumands=suma_bez_nds*nds1/100.;
 }

sprintf(strsql,"+%.2f",sumands);
sap->plus(strsql);

bb=data->sumaitogo=suma_bez_nds+sumands;
data->sumaitogo=iceb_u_okrug(bb,okrg1);
sprintf(strsql,"=%.2f",data->sumaitogo);
sap->plus(strsql);

if(data->procent != 0.)
 {
//  data->sumaitogo+=sumands+data->sumaitogo*data->procent/100.;
  data->sumaitogo=sumands+suma_bez_nds+suma_bez_nds*data->procent/100.;
  bb=data->sumaitogo;
  data->sumaitogo=iceb_u_okrug(bb,okrg1);
  
  sprintf(strsql,"/+%.1f%%=%.2f",data->procent,data->sumaitogo);
  sap->plus(strsql);
 }

gtk_label_set_text(GTK_LABEL(data->label_rek_dok),sap->ravno_toutf());

gtk_widget_show(data->label_rek_dok);

sap->new_plus("");

if(data->pro == 0)
  sap->plus_v_spisok(gettext("Проводки не сделаны !"));

if(data->blokpid != 0)
 {
  struct  passwd  *ktor;
  if((ktor=getpwuid(data->blokpid)) != NULL)
    sprintf(strsql,"%s ! %d %.*s",gettext("Документ заблокировано"),
    data->blokpid,iceb_u_kolbait(20,ktor->pw_gecos),ktor->pw_gecos);
  else
     sprintf(strsql,"%s %d !",gettext("Заблокировано неизвестным логином"),data->blokpid);

  sap->plus_v_spisok(strsql);
 } 
if(sap->getdlinna() > 1)
 {
  gtk_label_set_text(GTK_LABEL(data->label_red),sap->ravno_toutf());
  gtk_widget_show(data->label_red);
 }
else
 gtk_widget_hide(data->label_red);

}
/************************************************/
/*Проверка возможности корректировать накладную*/
/************************************************/
/*Если вернули 0 - можно корректировать
	       1 - нет
*/

int l_usldok_prblok(short mp, //0-все проверять 1-только блокировку документа
short md,short gd,int blokpid,
GtkWidget *wpredok)
{
struct  passwd  *ktoz; /*Кто записал*/
int		bbb;
char		strsql[512];

if(blokpid != 0 )
 {
  bbb=blokpid;
  if((ktoz=getpwuid(bbb)) != NULL)
     sprintf(strsql,"%s %d %s !",gettext("Документ заблокирован"),blokpid,ktoz->pw_gecos);
  else
     sprintf(strsql,"%s %d !",gettext("Заблокировано неизвестным логином"),bbb);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

if(mp == 1)
 return(0);
 
if(iceb_pbpds(md,gd,wpredok) != 0)
  return(1);


return(0);
}
/*********************************/
/*Удаление всех файлов документов*/
/**********************************/
void		deletdokumu(class iceb_u_spisok *imaf)
{
for(int i=0; i < imaf->kolih() ;i++)
 unlink(imaf->ravno(i));

}
/******************************/
/*Операции перед выходом из накладной*/
/**************************************/
void l_usldok_f10(class usldok_data *data)
{

  
podudokw(data->dd,data->md,data->gd,data->nomd.ravno(),data->podr,data->tipz,data->window);

if(mvnp != 0) /*Устанавливаем метку оплаты для внутренних документов*/
 {
  char strsql[512];
  sprintf(strsql,"update Usldokum \
set \
mo=1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
  data->gd,data->md,data->dd,data->podr,data->nomd.ravno(),data->tipz);


  if(sql_zap(&bd,strsql) != 0)
   if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
    {
     iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);
    }
 }

if(data->metkaprov == 1)
 return;

if(iceb_u_SRAV(data->nomd.ravno(),"000",0) == 0)
 {
  return;
 }

/*Если операция внутренняя и накладная на приход
Проводки выполняются только для расходных накладных*/

if(mvnp != 0 && data->tipz == 1)
 {
  return;
 }

/*Проверить на выполнение проводок*/
prosprusw(3,data->podr,data->dd,data->md,data->gd,data->nomd.ravno(),data->tipz,data->lnds,
data->kodop.ravno(),data->pnds,NULL,data->window);
}
/*********************************************/
/*Распечатка расходных документов*/
/************************************************/
void l_usldok_r1(class usldok_data *data)
{



iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
char stroka[1024];

titl.plus(gettext("Распечатка документов"));


punkt_m.plus(gettext("Распечатать акт выполненых работ"));//0
punkt_m.plus(gettext("Распечатать кассовый ордер"));//1
punkt_m.plus(gettext("Улалить распечатки документов"));//2
punkt_m.plus(gettext("Распечатать налоговую накладную"));//3


memset(stroka,'\0',sizeof(stroka));
sprintf(stroka,"%s\n%s",organ,gettext("Распечатка документов"));

zagolovok.new_plus(stroka);

int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);

iceb_u_str imaf;
iceb_u_spisok imafil;
iceb_u_spisok naimfil;

switch (nomer)
 {
  case -1:
    return;
  case 0:
    iceb_pehf(data->imaf.ravno(0),0,data->window);
    break;

  case 1:
     char imafko[30];
     sprintf(imafko,"kor%d.lst",getpid());

     iceb_kasord1(imafko,"",data->dd,data->md,data->gd,"","",data->sumaitogo,"","","","",data->window);

     imafil.plus(imafko);
     naimfil.plus(gettext("Распечатка кассового ордера"));

     iceb_rabfil(&imafil,&naimfil,"",0,data->window);
    break;


    case 2:
        deletdokumu(&data->imaf);
        break;
    case 3:
      l_usldok_snn(data);
      iceb_pehf(data->imaf.ravno(1),0,data->window);
      break;

 }
}
/*********************************************/
/*Распечатка расходных документов*/
/************************************************/
void l_usldok_r2(class usldok_data *data)
{



iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
char stroka[1024];

titl.plus(gettext("Распечатка документов"));


punkt_m.plus(gettext("Распечатать акт выполненых работ"));//0
punkt_m.plus(gettext("Распечатать налоговую накладную"));//1
punkt_m.plus(gettext("Распечатать счёт"));//2
punkt_m.plus(gettext("Распечатать счёт с учётом сальдо"));//3
punkt_m.plus(gettext("Распечатать кассовый ордер"));//4
punkt_m.plus(gettext("Распечатать счёт-фактуру"));//5
punkt_m.plus(gettext("Распечатать акт выполненых работ-списания материаллов"));//6
punkt_m.plus(gettext("Улалить распечатки документов"));//7

memset(stroka,'\0',sizeof(stroka));
sprintf(stroka,"%s\n%s",organ,gettext("Распечатка документов"));

zagolovok.new_plus(stroka);

int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);

class iceb_u_str imaf_s_logotip("");

char klush_peh[1024];
iceb_u_spisok imafil;
iceb_u_spisok naimfil;
class iceb_u_str imaf("");
switch (nomer)
 {
  case -1:
    return;
  case 0:
    iceb_pehf(data->imaf.ravno(0),0,data->window);
    break;
  case 1:
    l_usldok_snn(data);
    iceb_pehf(data->imaf.ravno(1),0,data->window);
    break;
  case 2:
    iceb_pehf(data->imaf.ravno(2),0,data->window);
    break;
  case 3:

    sozfusal(data);
    iceb_pehf(data->imaf.ravno(3),0,data->window);
    break;

  case 4:
     char imafko[30];
     sprintf(imafko,"kor%d.lst",getpid());

//     iceb_kasord1(imafko,"",data->dd,data->md,data->gd,"","",data->sumaitogo,"","","",data->window);
     iceb_kasord2(imafko,"",data->dd,data->md,data->gd,"","",
     data->sumaitogo,data->naim_kontr.ravno(),"","","","",NULL,data->window);


     imafil.plus(imafko);
     naimfil.plus(gettext("Распечатка кассового ордера"));

     iceb_rabfil(&imafil,&naimfil,"",0,data->window);
    break;

  case 5:

     //читаем имя файла с логотипом
     memset(klush_peh,'\0',sizeof(klush_peh));


     iceb_poldan("Путь на файл с логотипом",&imaf_s_logotip,"nastsys.alx",data->window);

     if(imaf_s_logotip.getdlinna() > 1)
         sprintf(klush_peh,"-o logo=%s",imaf_s_logotip.ravno());

     if(usl_shet_fakw(data->dd,data->md,data->gd,data->podr,data->nomd.ravno(),&imaf,data->tipz,data->window) == 0)
        imafil.plus(imaf.ravno());

     naimfil.plus(gettext("Распечатка счёта-фактуры"));

     iceb_rabfil(&imafil,&naimfil,klush_peh,0,data->window);

    break;

   case 6: //Распечатка акта выполненных работ, списания материалов
     if(usl_act_vrw(data->dd,data->md,data->gd,data->podr,data->nomd.ravno(),&imaf,data->tipz,data->window) == 0)
      {
       imafil.plus(imaf.ravno());
       naimfil.plus(gettext("Распечатка акта выполненых работ, списания материаллов"));
       iceb_rabfil(&imafil,&naimfil,"",0,data->window);
      }
     break;

    case 7:
        deletdokumu(&data->imaf);
        break;
 }
}
/***********************************/
/*Создание файлов с документами*/
/*********************************/
void l_usldok_sozfu(class usldok_data *data)
{
FILE            *ff;

printf("%s-start\n",__FUNCTION__);

/*Создание акта выполненых работ*/
errno=0;

if((ff = fopen(data->imaf.ravno(0),"r")) == NULL)
 if(errno != ENOENT)
  {
   iceb_er_op_fil(data->imaf.ravno(0),"",errno,NULL);
   return;
  }

if(errno == ENOENT)
 {
  if((ff = fopen(data->imaf.ravno(0),"w")) == NULL)
   {
    iceb_er_op_fil(data->imaf.ravno(0),"",errno,NULL);
    return;
   }
  iceb_u_startfil(ff);
  rasdokuw(data->dd,data->md,data->gd,data->podr,data->nomd.ravno(),data->lnds,0,ff,data->tipz,0,data->window);
  fclose(ff);
 }
else
  fclose(ff);

printf("%s\n",__FUNCTION__);

/*Если операция внутреннего перемещения то налоговая накладная не нужна*/
if(mvnp != 0)
 return;


/*Создание налоговой накладной*/
/***************
if((ff = fopen(data->imaf.ravno(1),"r")) == NULL)
 {
  if(errno != ENOENT)
   {
    iceb_er_op_fil(data->imaf.ravno(1),"",errno,NULL);
    return;
   }

  if(errno == ENOENT)
   {  
    rasnalnuw(data->dd,data->md,data->gd,data->podr,data->nomd.ravno(),data->imaf.ravno(1),data->lnds,data->window);
   }
 }
else
  fclose(ff);
************************/

if(data->tipz == 1)
  return;

/*Создание счета без сальдо*/
if((ff = fopen(data->imaf.ravno(2),"r")) == NULL)
 if(errno != ENOENT)
  {
   iceb_er_op_fil(data->imaf.ravno(2),"",errno,NULL);
   return;
  }

printf("%s\n",__FUNCTION__);
if(errno == ENOENT)
 {
  if((ff = fopen(data->imaf.ravno(2),"w")) == NULL)
   {
    iceb_er_op_fil(data->imaf.ravno(2),"",errno,NULL);
    return;
   }
  iceb_u_startfil(ff);

  rasdokuw(data->dd,data->md,data->gd,data->podr,data->nomd.ravno(),data->lnds,1,ff,data->tipz,0,data->window);
  fclose(ff);
 }
else
  fclose(ff);

printf("%s-end\n",__FUNCTION__);

}
/*****************************/
/*создание налоговой накладной*/
/*******************************/
int l_usldok_snn(class  usldok_data *data)
{
FILE *ff;
if((ff = fopen(data->imaf.ravno(1),"r")) == NULL)
 {
  if(errno != ENOENT)
   {
    iceb_er_op_fil(data->imaf.ravno(1),"",errno,NULL);
    return(1);
   }

  if(errno == ENOENT)
   {  
    return(rasnalnuw(data->dd,data->md,data->gd,data->podr,data->nomd.ravno(),data->imaf.ravno(1),data->lnds,data->window));
   }
 }
else
  fclose(ff);
return(0);
}


/*************************/
/*Создание счета с сальдо*/
/*************************/
void  sozfusal(class usldok_data *data)
{
FILE	*ff;

/*Создание счета с сальдо*/
if((ff = fopen(data->imaf.ravno(3),"r")) == NULL)
 if(errno != ENOENT)
   {
    iceb_er_op_fil(data->imaf.ravno(3),"",errno,NULL);
    return;
   }

if(errno == ENOENT)
 {
  if((ff = fopen(data->imaf.ravno(3),"w")) == NULL)
   {
    iceb_er_op_fil(data->imaf.ravno(3),"",errno,NULL);
    return;
   }

  iceb_u_startfil(ff);

  rasdokuw(data->dd,data->md,data->gd,data->podr,data->nomd.ravno(),data->lnds,1,ff,data->tipz,1,data->window);
  fclose(ff);
 }
else
  fclose(ff);
}


/*************************************/
/*Просмотр с редактированием документов*/
/***************************************/
void l_usldok_pros_red1(class usldok_data *data)
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
iceb_u_str stroka;

titl.plus(gettext("Просмотр документа"));

stroka.new_plus("\n");
stroka.plus_ps(organ);
stroka.plus_ps(gettext("Просмотр документа"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Редактирование акта выполненых работ"));//0
punkt_m.plus(gettext("Редактирование налоговой накладной"));
punkt_m.plus(gettext("Просмотр акта выполненых работ"));//0
punkt_m.plus(gettext("Просмотр налоговой накладной"));

int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);


switch (nomer)
 {
  case -1:
    return;

  case 0:
    iceb_f_redfil(data->imaf.ravno(0),1,data->window);
    break;

  case 1:
    if(l_usldok_snn(data) != 0)
     break;
    iceb_f_redfil(data->imaf.ravno(1),1,data->window);
    break;

  case 2:
    iceb_prospp(data->imaf.ravno(0),data->window);
    break;

  case 3:
    if(l_usldok_snn(data) != 0)
     break;
    iceb_prospp(data->imaf.ravno(1),data->window);
    break;
 }
 
}
/*************************************/
/*Просмотр с редактированием документов*/
/***************************************/
void l_usldok_pros_red2(class usldok_data *data)
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
iceb_u_str stroka;

titl.plus(gettext("Просмотр документа"));

stroka.new_plus("\n");
stroka.plus_ps(organ);
stroka.plus_ps(gettext("Просмотр документа"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Редактирование акта выполненых работ"));//0
punkt_m.plus(gettext("Редактирование налоговой накладной"));
punkt_m.plus(gettext("Редактирование счета"));
punkt_m.plus(gettext("Редактирование счета с учётом сальдо "));
punkt_m.plus(gettext("Просмотр акта выполненых работ"));//0
punkt_m.plus(gettext("Просмотр налоговой накладной"));
punkt_m.plus(gettext("Просмотр счета"));
punkt_m.plus(gettext("Просмотр счета с учётом сальдо "));

int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);


switch (nomer)
 {
  case -1:
    return;

  case 0:
    iceb_f_redfil(data->imaf.ravno(0),1,data->window);
    break;

  case 1:
    if(l_usldok_snn(data) != 0)
     break;
    iceb_f_redfil(data->imaf.ravno(1),1,data->window);
    break;

  case 2:
    iceb_f_redfil(data->imaf.ravno(2),1,data->window);
    break;

  case 3:

    sozfusal(data);
    iceb_f_redfil(data->imaf.ravno(3),1,data->window);
    break;

  case 4:
    iceb_prospp(data->imaf.ravno(0),data->window);
    break;

  case 5:
    if(l_usldok_snn(data) != 0)
     break;
    iceb_prospp(data->imaf.ravno(1),data->window);
    break;
  case 6:
    iceb_prospp(data->imaf.ravno(2),data->window);
    break;

  case 7:
    sozfusal(data);
    iceb_prospp(data->imaf.ravno(3),data->window);
    break;
 }
 
}
/********************************/
/*Подтверждение всего документа*/
/********************************/
void l_usldok_pod_dok(class usldok_data *data)
{

if(iceb_menu_danet(gettext("Подтвердить весь документ ? Вы уверены ?"),2,data->window) == 2)
 return;
char strsql[512];

sprintf(strsql,"%d.%d.%d",data->dd,data->md,data->gd);
if(iceb_menu_vvod1(gettext("Введите дату подтверждения"),strsql,11,data->window) != 0)
 return;

short d1,m1,g1;
class iceb_clock sss(data->window);

if(iceb_u_rsdat(&d1,&m1,&g1,strsql,0) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
  return;
 }

if(iceb_u_sravmydat(d1,m1,g1,data->dd,data->md,data->gd) < 0)
 {
  iceb_menu_soob(gettext("Дата меньше чем дата в документе ! Подтверждение не возможно !"),data->window);
  return;
 }

if(iceb_pblok(m1,g1,ICEB_PS_UU,data->window) != 0)
 {
  sprintf(strsql,gettext("Документы на дату %d.%dг. заблокированы !"),m1,g1);
  iceb_menu_soob(strsql,data->window);
  return;
 }


sprintf(strsql,"select metka,kodzap,kolih,cena,ei,shetu,nz from \
Usldokum1 where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
data->gd,data->md,data->dd,data->podr,data->nomd.ravno(),data->tipz);

SQL_str row;
SQLCURSOR cur;
int kolstr;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

if(kolstr == 0)
  return;

int nz=0;
int metka;
int kodzap;
double kol1;
double kol2;
double cena;
while(cur.read_cursor(&row) != 0)
 {
  metka=atoi(row[0]);
  kodzap=atoi(row[1]);
  kol1=atof(row[2]);
  cena=atof(row[3]);
  nz=atoi(row[6]);  

  /*Читаем в карточке количество реально выданное-полученное*/
  kol2=readkoluw(data->dd,data->md,data->gd,data->podr,data->nomd.ravno(),data->tipz,metka,
  kodzap,nz,data->window);

  //Количество может быть отрицательным
  if(fabs(kol1) > fabs(kol2))
    zappoduw(data->tipz,data->dd,data->md,data->gd,d1,m1,g1,data->nomd.ravno(),metka,
    kodzap,kol1-kol2,cena,row[4],row[5],data->podr,nz,data->window);


 }

}
/***************************************/
/*Меню для клавиши F8*/
/****************************/

void l_usldok_F8(class usldok_data *data)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
iceb_u_str stroka;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Найти нужный код записи"));//0
punkt_m.plus(gettext("Установить/снять блокировку даты"));//1
if(data-> blokpid == 0)
  punkt_m.plus(gettext("Заблокировать документ"));//2
if(data->blokpid != 0)
  punkt_m.plus(gettext("Разблокировать документ"));//2

punkt_m.plus(gettext("Снять подтверждение со всего документа"));//3
punkt_m.plus(gettext("Ввести сумму или процент корректировки"));//4
punkt_m.plus(gettext("Копировать записи из других документов"));//5
punkt_m.plus(gettext("Просмотр приложения к документу"));//6
punkt_m.plus(gettext("Добавить к наименованию услуги"));//7
punkt_m.plus(gettext("Создать документ в подсистеме \"Материальный учёт.\""));//8

int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);
char strsql[512];
char dnaim[512];
memset(dnaim,'\0',sizeof(dnaim));

switch (nomer)
 {
  case -1:
    return;

  case 0:
    l_usldok_snkz(data);
    break;

  case 1:
    iceb_f_redfil("uslugbl.alx",0,NULL);
    break;

  case 2:
    printf("data->blokpid=%d /%d\n",data->blokpid,iceb_getuid(data->window));
    if(data->blokpid != 0 && data->blokpid != iceb_getuid(data->window))
     if(iceb_parol(0,data->window) != 0)
      return;

    if(data->blokpid == 0)
     data->blokpid=iceb_getuid(data->window);
    else
     data->blokpid=0;

    memset(strsql,'\0',sizeof(strsql)); 
    sprintf(strsql,"update Usldokum set blokir=%d \
where datd='%04d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
    data->blokpid,data->gd,data->md,data->dd,data->podr,data->nomd.ravno(),data->tipz);

    iceb_sql_zapis(strsql,0,0,data->window);

    usldok_create_list(data);

    break;

  case 3:
    if(data->blokpid != iceb_getuid(data->window))
    if( l_usldok_prblok(0,data->md,data->gd,data->blokpid,data->window) != 0)
     return;

    sprintf(strsql,"delete from Usldokum2 where datd='%d-%02d-%02d' \
and nomd='%s' and podr=%d and tp=%d",data->gd,data->md,data->dd,data->nomd.ravno(),data->podr,data->tipz);

    iceb_sql_zapis(strsql,0,0,data->window);

    usldok_create_list(data);

    break;

  case 4:
    
    if( l_usldok_prblok(1,data->md,data->gd,data->blokpid,data->window) != 0)
     return;
    memset(strsql,'\0',sizeof(strsql));
    if(iceb_menu_vvod1(gettext("Введите сумму или процент (%%)"),strsql,20,data->window) != 0)
     return;

    deletdokumu(&data->imaf);
    data->naimkor.new_plus("");
    data->sumkor=iceb_u_atof(strsql);
    char bros[512];
    if(iceb_u_pole(strsql,bros,1,'%') == 0)
      data->sumkor=data->suma*iceb_u_atof(bros)/100.;

    usldok_create_list(data);

    break;

  case 5:
    copdokuw(data->dd,data->md,data->gd,data->nomd.ravno(),data->podr,data->tipz,data->window);
    usldok_create_list(data);
    break;

  case 6:
    //Установить флаг работы с приложением
    char flag[300];
    sprintf(flag,"usl%s%s%d%d",imabaz,data->nomd.ravno(),data->gd,data->podr);

    if(sql_flag(&bd,flag,0,0) != 0)
     {
      iceb_menu_soob(gettext("С приложением уже работает другой оператор !"),data->window);
      break;
     }
    sprintf(strsql,"usl_prilog%d.tmp",getpid());
    prilogw('-',data->podr,data->gd,data->tipz,data->nomd.ravno(),strsql,data->window);

    iceb_vizred(strsql,data->window);

    prilogw('+',data->podr,data->gd,data->tipz,data->nomd.ravno(),strsql,data->window);

    unlink(strsql);        

    sql_flag(&bd,flag,0,1);
    deletdokumu(&data->imaf);
    break;

  case 7:
     if(data->kolzap == 0)
       break;
     if( l_usldok_prblok(1,data->md,data->gd,data->blokpid,data->window) != 0)
      return;
     iceb_u_pole(data->naim_zapv.ravno(),dnaim,2,'+');
//     strncpy(dnaim,data->dop_naimv.ravno(),sizeof(dnaim)-1);

     if(iceb_menu_vvod1(gettext("Введите дополнение к наименованию"),dnaim,sizeof(dnaim),data->window) != 0)
      return;
       

      sqlfiltr(dnaim,sizeof(dnaim));

      sprintf(strsql,"update Usldokum1 set dnaim='%s' \
where datd='%04d-%d-%d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d and nz=%d",
      dnaim,data->gd,data->md,data->dd,data->podr,data->nomd.ravno(),
      data->metkazapv,data->kodzapv.ravno_atoi(),data->tipz,data->nomzapv);
      
      printf("strsql=%s\n",strsql);
      
      iceb_sql_zapis(strsql,1,0,data->window); 
      deletdokumu(&data->imaf);
      usldok_create_list(data);
      break;

  case 8:
    usl_spismat(data->dd,data->md,data->gd,data->nomd.ravno(),data->podr,data->tipz,
    data->kontr.ravno(),data->lnds,data->pnds,data->window);
    break;

 }

}

/***************************************/
/*Меню для клавиши F9 расход*/
/****************************/

void l_usldok_F9r(class usldok_data *data)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
iceb_u_str stroka;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Переценить документ на заданный процент"));//0
punkt_m.plus(gettext("Выбрать вид корректировки суммы по документу"));//1

int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);

switch (nomer)
 {
  case -1:
    return;

  case 0:
    if( l_usldok_prblok(1,data->md,data->gd,data->blokpid,data->window) != 0)
     return;
    perecenuw(data->dd,data->md,data->gd,data->nomd.ravno(),data->podr,data->tipz,data->pnds,data->window);
    usldok_create_list(data);
    deletdokumu(&data->imaf);
    break;

  case 1: //Установить вид скидки
    if( l_usldok_prblok(1,data->md,data->gd,data->blokpid,data->window) != 0)
     return;

    data->naimkor.new_plus("");
    punkt_m.free_class();
    if(makkorw(&punkt_m,1,&data->naimkor,&data->maskor,"uslkor.alx") != 0)
     break;       

    if((nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window)) < 0)
      break;
     

    data->naimkor.new_plus(punkt_m.ravno(nomer));
    makkorw(&punkt_m,2,&data->naimkor,&data->maskor,"uslkor.alx");

    deletdokumu(&data->imaf);
    usldok_create_list(data);

    break;


 }

}
/***************************************/
/*Меню для клавиши F9 приход*/
/****************************/

void l_usldok_F9p(class usldok_data *data)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
iceb_u_str stroka;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Ввести сумму НДС для документа"));//2

int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);
char bros[512];
char strsql[512];
switch (nomer)
 {
  case -1:
    return;

  case 0: //Ввести сумму НДС для документа
    if( l_usldok_prblok(1,data->md,data->gd,data->blokpid,data->window) != 0)
     return;
    bros[0]='\0';
    if(data->sumandspr != 0.)
      sprintf(bros,"%.2f",data->sumandspr);

    if(iceb_menu_vvod1(gettext("Введите сумму НДС"),bros,20,data->window) != 0)
      break;

    data->sumandspr=atof(bros);
     
    sprintf(strsql,"update Usldokum set sumnds=%.2f where \
datd='%04d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
    data->sumandspr,data->gd,data->md,data->dd,data->podr,data->nomd.ravno(),data->tipz);

    
     iceb_sql_zapis(strsql,1,0,data->window);

    deletdokumu(&data->imaf);
    usldok_create_list(data);
    break;


 }

}


/***********************************/
/*стать на нужный код записи*/
/*******************************/

void l_usldok_snkz(class usldok_data *data)
{
char kodzap_ch[20];
if(iceb_menu_vvod1(gettext("Введите код записи"),kodzap_ch,sizeof(kodzap_ch),data->window) != 0)
 return;
char strsql[512];
int kodzap=atoi(kodzap_ch);
iceb_clock sss(data->window);

sprintf(strsql,"select kodzap from Usldokum1 where datd='%04d-%02d-%02d' and podr=%d \
and nomd='%s' and tp=%d and kodzap=%d",
data->gd,data->md,data->dd,data->podr,data->nomd.ravno(),data->tipz,kodzap);

SQL_str row;
SQLCURSOR cur;
int kolstr;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

if(kolstr == 0)
  return;

int nomzap=0;
while(cur.read_cursor(&row) != 0)
 {
  if(kodzap == atoi(row[1]))
    break;
  nomzap++;          
 }
data->snanomer=nomzap;
usldok_create_list(data);
}
/*******************************************/
/*Чтение реквизитов шапки документа и формирование имён распечаток*/
/********************************************************************/

void l_usldok_read_sap(class usldok_data *data,GtkWidget *wpredok)
{
char strsql[512];

//читаем реквизиты документа
l_usldok_rd(data,wpredok);

/*Имя файла акта выполненых работ без учета сальдо*/
if(data->tipz == 1)
  sprintf(strsql,"apu%ld.lst",data->vrem_zap);
if(data->tipz == 2)
  sprintf(strsql,"aru%ld.lst",data->vrem_zap);
data->imaf.free_class();
data->imaf.plus(strsql);

/*имя файла налоговой накладной*/
if(data->tipz == 1)
     sprintf(strsql,"nnpu%ld.lst",data->vrem_zap);
if(data->tipz == 2)
     sprintf(strsql,"nnru%ld.lst",data->vrem_zap);
data->imaf.plus(strsql);

/*имя файла счета*/
if(data->tipz == 1)
     sprintf(strsql,"shpu%ld.lst",data->vrem_zap);
if(data->tipz == 2)
     sprintf(strsql,"shru%ld.lst",data->vrem_zap);
data->imaf.plus(strsql);

/*Имя файла акта выполненых работ с учетом сальдо*/
if(data->tipz == 1)
  sprintf(strsql,"apus%ld.lst",data->vrem_zap);
if(data->tipz == 2)
  sprintf(strsql,"arus%ld.lst",data->vrem_zap);
data->imaf.plus(strsql);
}


