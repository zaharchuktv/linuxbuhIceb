/*$Id: l_kasdok.c,v 1.50 2011-02-21 07:35:52 sasa Exp $*/
/*27.02.2010	09.02.2006	Белых А.И.	l_kasdok.c
Работа с кассовым ордером
*/
#include <stdlib.h>
#include <errno.h>
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
 FK7,
 SFK7,
 FK8,
 FK10,
 KOL_F_KL
};

enum
{
 COL_KONTR,
 COL_NAIM,
 COL_SUMA,
 COL_DATA_POD,
 COL_KOMENT,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  kasdok_data
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

  //Уникальные реквизиты документа
  class iceb_u_str datad;
  int tipz;
  class iceb_u_str nomdok;
  class iceb_u_str kassa;
  
  //*****************  
  class iceb_u_str naim_kas;
  class iceb_u_str naim_kodop;
  class iceb_u_str kodop;
  class iceb_u_str shetd;
  class iceb_u_str shetkasd;
  class iceb_u_str osnov;
  class iceb_u_str dopol;
  class iceb_u_str fio;
  class iceb_u_str dokum;
  class iceb_u_str kod_cel_naz;
  class iceb_u_str nomv;
    
  short prov;
  short tipop; //0-для операции не нужно делать проводок 1-нужно
  //Код контрагента выбранной строки
  iceb_u_str kontr_v;
  //Код контрагента только что введённый
  iceb_u_str kontr_zap;  
  double suma;                       
  //Конструктор
  kasdok_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    naim_kas.plus("");
    naim_kodop.plus("");
   }      
 };

gboolean   kasdok_key_press(GtkWidget *widget,GdkEventKey *event,class kasdok_data *data);
void kasdok_vibor(GtkTreeSelection *selection,class kasdok_data *data);
void kasdok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class kasdok_data *data);
void  kasdok_knopka(GtkWidget *widget,class kasdok_data *data);
void kasdok_add_columns (GtkTreeView *treeview);
void kasdok_create_list(class kasdok_data *data);
 
void l_kasdok_str_rek(class kasdok_data *data,iceb_u_str *sap);
int l_kasdok_rd(class kasdok_data *data,GtkWidget *wpredok);
int vvko_kontr(const char *datad,const char *nomdok,const char *kassa,int tipz,iceb_u_str *kontr,const char *shetd,GtkWidget *wpredok);
void l_prov_kas(short dd,short md,short gd,const char *nomd,const char *kassa,const char *kodop,int tipz,const char *shetd,GtkWidget *wpredok);
void l_kasdok_ras(class kasdok_data *data);
void	raszko(class kasdok_data *data);

extern SQL_baza	bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern int kol_strok_na_liste;

void l_kasdok(const char *data_dok,
int tipz,  //1-приход 2-расход
const char *nomdok,
const char *kassa,
GtkWidget *wpredok)
{
class  kasdok_data data;
data.datad.new_plus(data_dok);
data.tipz=tipz;
data.nomdok.new_plus(nomdok);
data.kassa.new_plus(kassa);

//Чтение реквизитов документа
l_kasdok_rd(&data,wpredok);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);
char bros[312];
if(tipz == 1)
 sprintf(bros,"%s %s",name_system,gettext("Работа с приходным кассовым ордером"));
if(tipz == 2)
 sprintf(bros,"%s %s",name_system,gettext("Работа с расходным кассовым ордером"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(kasdok_key_press),&data);
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
iceb_u_str string;

l_kasdok_str_rek(&data,&string);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),-1,200);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_widget_show(data.sw);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"%sF1 %s",RFK,gettext("Шапка"));
data.knopka[SFK1]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK1]), "clicked",GTK_SIGNAL_FUNC(kasdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK1],TRUE,TRUE, 0);
tooltips[SFK1]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK1],data.knopka[SFK1],gettext("Просмотр шапки документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK1]),(gpointer)SFK1);
gtk_widget_show(data.knopka[SFK1]);

sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(kasdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(kasdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(kasdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(kasdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удалить документ"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Проводки"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(kasdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Переход в режим работы с проводками для этого документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(kasdok_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка документов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);


sprintf(bros,"F7 %s",gettext("Подтверждение"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(kasdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Подтверждение записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"%sF7 %s",RFK,gettext("Подтверждение"));
data.knopka[SFK7]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK7]), "clicked",GTK_SIGNAL_FUNC(kasdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK7],TRUE,TRUE, 0);
tooltips[SFK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK7],data.knopka[SFK7],gettext("Подтверждение всех записей в документе"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK7]),(gpointer)SFK7);
gtk_widget_show(data.knopka[SFK7]);

sprintf(bros,"F8 %s",gettext("Снять под-ние"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(kasdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Снять подтверждение со всего документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(kasdok_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);

gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

kasdok_create_list(&data);
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
void  kasdok_knopka(GtkWidget *widget,class kasdok_data *data)
{
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("kasdok_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается
iceb_u_str kontr;
kontr.plus("");

short dd,md,gd;
short d,m,g;
iceb_u_rsdat(&dd,&md,&gd,data->datad.ravno(),1);
char strsql[512];
switch (knop)
 {
  case SFK1:
    if(pbkasw(md,gd,data->window) != 0)
      return;
    if(data->tipz == 1)
     vkasord1(&data->datad,&data->nomdok,&data->kassa,data->window);
    if(data->tipz == 2)
     vkasord2(&data->datad,&data->nomdok,&data->kassa,data->window);

    //Чтение реквизитов документа
    l_kasdok_rd(data,data->window);
    kasdok_create_list(data);

    return;  

  case FK2:
    if(pbkasw(md,gd,data->window) != 0)
      return;
    if(vvko_kontr(data->datad.ravno(),data->nomdok.ravno(),data->kassa.ravno(),data->tipz,&kontr,data->shetd.ravno(),data->window) == 0)
      data->kontr_zap.new_plus(data->kontr_v.ravno());
    kasdok_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
     return;
    if(pbkasw(md,gd,data->window) != 0)
      return;
    if(vvko_kontr(data->datad.ravno(),data->nomdok.ravno(),data->kassa.ravno(),data->tipz,&data->kontr_v,data->shetd.ravno(),data->window) == 0)
      data->kontr_zap.new_plus(data->kontr_v.ravno());
    
    kasdok_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
     return;
    if(pbkasw(md,gd,data->window) != 0)
      return;

    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
     return;

    sprintf(strsql,"delete from Kasord1 where kassa=%d and tp=%d \
and nomd='%s' and kontr='%s' and god=%d",
    data->kassa.ravno_atoi(),data->tipz,data->nomdok.ravno(),data->kontr_v.ravno(),data->datad.ravno_god());

     if(sql_zap(&bd,strsql) != 0)
      {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
        {
         iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),data->window);
         return;
        }
       else
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
      }

    kasdok_create_list(data);

    return;  

  case SFK3:

    if(pbkasw(md,gd,data->window) != 0)
      return;

    if(iceb_menu_danet(gettext("Удалить документ ? Вы уверены ?"),2,data->window) == 2)
      return;    

    if(iceb_udprgr(gettext("КО"),dd,md,gd,data->nomdok.ravno(),data->kassa.ravno_atoi(),data->tipz,data->window) != 0)
     return;
    if(udkasdokw(data->kassa.ravno(),data->nomdok.ravno(),dd,md,gd,data->tipz,data->window) != 0)
      return;

    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    if(data->tipop == 0)
     {
      iceb_menu_soob(gettext("Для этой операции проводки не делаются !"),data->window);
      return;
     }
    l_prov_kas(dd,md,gd,data->nomdok.ravno(),data->kassa.ravno(),data->kodop.ravno(),data->tipz,
    data->shetd.ravno(),data->window);

    //Чтение реквизитов документа 
    l_kasdok_rd(data,data->window);

    kasdok_create_list(data);
    return;  

  case FK5:
    if(data->kolzap == 0)
     return;

    l_kasdok_ras(data);

    return;  


  case FK7:
    if(data->kolzap == 0)
     return;
    if(pbkasw(md,gd,data->window) != 0)
      return;
  
  
    kontr.new_plus(data->datad.ravno());
    

    if(iceb_menu_vvod1(gettext("Введите дату подтверждения"),&kontr,20,data->window) != 0)
     return;

    if(iceb_u_rsdat(&d,&m,&g,kontr.ravno(),0) != 0)
     {
      iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
      return;
     }

    sprintf(strsql,"UPDATE Kasord1 set datp='%d-%d-%d' where \
kassa=%s and god=%d and tp=%d and nomd='%s' and kontr='%s'",
    g,m,d,data->kassa.ravno(),gd,data->tipz,data->nomdok.ravno(),data->kontr_v.ravno());

    iceb_sql_zapis(strsql,0,0,data->window);

    kasdok_create_list(data);
    return;  

  case SFK7:
    if(data->kolzap == 0)
     return;
    
    if(pbkasw(md,gd,data->window) != 0)
      return;

    if(iceb_menu_danet(gettext("Подтвердить весь документа ? Вы уверены ?"),2,data->window) == 2)
     return;

    kontr.new_plus(data->datad.ravno());
    

    if(iceb_menu_vvod1(gettext("Введите дату подтверждения"),&kontr,20,data->window) != 0)
     return;

    if(iceb_u_rsdat(&d,&m,&g,kontr.ravno(),0) != 0)
     {
      iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
      return;
     }

    sprintf(strsql,"UPDATE Kasord1 set datp='%d-%d-%d' where \
kassa=%s and god=%d and tp=%d and nomd='%s'",
    g,m,d,data->kassa.ravno(),gd,data->tipz,data->nomdok.ravno());

    iceb_sql_zapis(strsql,0,0,data->window);


    kasdok_create_list(data);
    return;  

  case FK8:
    if(data->kolzap == 0)
     return;
    if(pbkasw(md,gd,data->window) != 0)
      return;

    if(iceb_menu_danet(gettext("С нять подтверждение с документа ? Вы уверены ?"),2,data->window) == 2)
     return;

    sprintf(strsql,"UPDATE Kasord1 set datp='0000-00-00' where \
kassa=%s and god=%d and tp=%d and nomd='%s'",
    data->kassa.ravno(),gd,data->tipz,data->nomdok.ravno());

    iceb_sql_zapis(strsql,0,0,data->window);

    kasdok_create_list(data);
    return;  

    
  case FK10:


     provpodkow(data->kassa.ravno(),data->nomdok.ravno(),dd,md,gd,data->tipz,data->window); /*проверяем подтверждение документа*/
     provprkasw(data->kassa.ravno(),data->tipz,data->nomdok.ravno(),dd,md,gd,data->kodop.ravno(),data->window); /*проверяем выполнение проводок*/
     if(data->nomv.getdlinna() > 1)
      {
        sprintf(strsql,"update Kasnomved set suma=%.2f,ktoi=%d,vrem=%ld  where god=%d and nomv='%s'",
        data->suma,
        iceb_getuid(data->window),
        time(NULL),
        data->datad.ravno_god(),
        data->nomv.ravno());
        iceb_sql_zapis(strsql,0,0,data->window);
      }     

     gtk_widget_destroy(data->window);
     return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kasdok_key_press(GtkWidget *widget,GdkEventKey *event,class kasdok_data *data)
{
iceb_u_str repl;
//printf("kasdok_key_press keyval=%d state=%d\n",event->keyval,event->state);

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
//    kasdok_create_list(data);
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
//    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
//    if(data->kl_shift == 1)
//      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK5]),"clicked");
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

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("kasdok_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
//    printf("Не выбрана клавиша ! Код=%d\n",event->keyval);

    break;
 }

return(TRUE);
}


/******************************************/
/*Формирование строки с реквизитами документа*/
/*********************************************/

void l_kasdok_str_rek(class kasdok_data *data,iceb_u_str *sap)
{
char strsql[512];

sprintf(strsql,"%s:%s %s:%s",
gettext("Дата"),data->datad.ravno(),
gettext("Номер документа"),data->nomdok.ravno()); 

sap->new_plus(strsql);

sprintf(strsql,"%s:%s %s",gettext("Касса"),data->kassa.ravno(),data->naim_kas.ravno());
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

sprintf(strsql,"%s:%s %s:%.2f %s:%d",
gettext("Счёт"),data->shetkasd.ravno(),
gettext("Сумма всех записей"),data->suma,
gettext("Количество записей"),data->kolzap);

sap->ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_rek_dok),sap->ravno_toutf());

gtk_widget_show(data->label_rek_dok);

sap->new_plus("");

if(data->prov == 0)
  sap->plus_v_spisok(gettext("Проводки не сделаны !"));

if(sap->getdlinna() > 1)
 {
  gtk_label_set_text(GTK_LABEL(data->label_red),sap->ravno_toutf());
  gtk_widget_show(data->label_red);
 }
else
 gtk_widget_hide(data->label_red);


}
/***********************************/
/*Читаем реквизиты документа*/
/******************************/
int l_kasdok_rd(class kasdok_data *data,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select shetk,kodop,osnov,dopol,fio,dokum,prov,nomv from Kasord \
where nomd='%s' and kassa=%s and tp=%d and god=%d",
data->nomdok.ravno(),data->kassa.ravno(),data->tipz,data->datad.ravno_god());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  char bros[512];
  sprintf(bros,"%s %s %s %d %d !",
  gettext("Не найден документ"),
  data->nomdok.ravno(),
  data->datad.ravno(),
  data->kassa.ravno_atoi(),
  data->tipz);
  iceb_menu_soob(bros,wpredok);
  return(1);
 }
data->nomv.new_plus(row[7]);
data->shetd.new_plus(row[0]);
data->kodop.new_plus(row[1]);

data->osnov.new_plus(row[2]);

data->dopol.new_plus(row[3]);
data->fio.new_plus(row[4]);
data->dokum.new_plus(row[5]);
data->prov=atoi(row[6]);

//Читаем счет кассы
data->shetkasd.new_plus("");
sprintf(strsql,"select naik,shet from Kas where kod=%d",data->kassa.ravno_atoi());
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  data->naim_kas.new_plus(row[0]);
  data->shetkasd.new_plus(row[1]);
 }
//Читаем нужно ли делать проводки для этой операции
if(data->tipz == 1)
 sprintf(strsql,"select naik,shetkas,shetkor,metkapr,kcn from Kasop1 where kod='%s'",data->kodop.ravno());
if(data->tipz == 2)
 sprintf(strsql,"select naik,shetkas,shetkor,metkapr,kcn from Kasop2 where kod='%s'",data->kodop.ravno());

data->naim_kodop.new_plus("");
data->kod_cel_naz.new_plus("");
data->tipop=0;
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  data->naim_kodop.new_plus(row[0]);
  if(row[3][0] == '1')
    data->tipop=1;
  data->kod_cel_naz.new_plus(row[4]);
 }
if(data->tipop == 0)
 data->prov=1;


return(0);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void kasdok_create_list (class kasdok_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row,row1;
iceb_u_str zagolov;

//printf("kasdok_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(kasdok_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(kasdok_vibor),data);

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

sprintf(strsql,"select datp,kontr,suma,ktoi,vrem,koment from Kasord1 where datd='%s' and \
kassa=%d and nomd='%s' and tp=%d order by kontr asc",
data->datad.ravno_sqldata(),data->kassa.ravno_atoi(),data->nomdok.ravno(),data->tipz);
//printf("strsql=%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
data->suma=0.;
short d,m,g;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  

  if(iceb_u_SRAV(data->kontr_zap.ravno(),row[1],0) == 0)
    data->snanomer=data->kolzap;


  //Дата подтверждения записи
  ss[COL_DATA_POD].new_plus("");
  if(row[0][0] != '0')
   {
    iceb_u_rsdat(&d,&m,&g,row[0],2);
    sprintf(strsql,"%02d.%02d.%d",d,m,g);
    ss[COL_DATA_POD].new_plus(strsql);
    
   }
  
  //Код контрагента
  ss[COL_KONTR].new_plus(iceb_u_toutf(row[1]));

  //Сумма
  ss[COL_SUMA].new_plus(row[2]);
  data->suma+=atof(row[2]);
    
  //Узнаём наименование контрагента
  ss[COL_NAIM].new_plus("");
  
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    ss[COL_NAIM].new_plus(iceb_u_toutf(row1[0]));
  
  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[4])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[3],0,data->window));
  /*Коментарий*/
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[5]));

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KONTR,ss[COL_KONTR].ravno(),
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_SUMA,ss[COL_SUMA].ravno(),
  COL_DATA_POD,ss[COL_DATA_POD].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kontr_zap.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

kasdok_add_columns (GTK_TREE_VIEW (data->treeview));

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK4]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK7]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK4]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK7]),TRUE);//Доступна
 }
gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

//формируем заголовок с реквизитами документа
l_kasdok_str_rek(data,&zagolov);




gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));
}
/*****************/
/*Создаем колонки*/
/*****************/

void kasdok_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;




renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text",COL_KONTR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Под-но"), renderer,"text", COL_DATA_POD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,"text", COL_KTO,NULL);


}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void kasdok_vibor(GtkTreeSelection *selection,class kasdok_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kontr_v;
gint  nomer;

gtk_tree_model_get(model,&iter,
COL_KONTR,&kontr_v,
NUM_COLUMNS,&nomer,-1);

data->kontr_v.new_plus(iceb_u_fromutf(kontr_v));
data->snanomer=nomer;

g_free(kontr_v);

//printf("%s %s\n",data->kodzapv.ravno(),data->naim_zapv.ravno());
}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void kasdok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class kasdok_data *data)
{
//Корректировка записи
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");


}
/***********************************/
/*Распечатка документов*/
/**************************/

void l_kasdok_ras(class kasdok_data *data)
{
char bros[512];
char strsql[512];
iceb_u_str stroka;
int nomer=0;
SQL_str row;
SQLCURSOR cur;
char dokum[200];

char fio[512];
strncpy(fio,data->fio.ravno(),sizeof(fio)-1);
strncpy(dokum,data->dokum.ravno(),sizeof(dokum)-1);

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка отчётов"));

memset(strsql,'\0',sizeof(strsql));
strncpy(strsql,organ,40);
stroka.new_plus_ps(strsql);
stroka.plus(gettext("Распечатка отчётов"));

zagolovok.plus(stroka.ravno());
/***************
memset(strsql,'\0',sizeof(strsql));
char *nameprinter = getenv("PRINTER");
if(nameprinter != NULL)
 sprintf(strsql,"%s:%-20.20s",gettext("Принтер"),nameprinter);
else
 sprintf(strsql,"%s:%-20.20s",gettext("Принтер"),gettext("По умолчанию"));

zagolovok.ps_plus(strsql);
*********************/
punkt_m.plus(gettext("Распечатать кассовый ордер"));//0
punkt_m.plus(gettext("Распечатать список контрагентов в документе"));//1

short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,data->datad.ravno(),1);



nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
switch (nomer)
 {
  case -1:
    return;

  case 0:

    if(fio[0] == '\0' && data->kolzap == 1)
     {

      sprintf(strsql,"select kontr from Kasord1 where datd='%s' and \
kassa=%d and nomd='%s' and tp=%d order by kontr asc",
data->datad.ravno_sqldata(),data->kassa.ravno_atoi(),data->nomdok.ravno(),data->tipz);
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) >= 1)
       { 
        char kontr[30];
        strcpy(kontr,row[0]);
        /*Читаем наименование контрагента*/
        sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr);
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
          strncpy(fio,row[0],sizeof(fio)-1);
        if(data->tipz == 2 && dokum[0] == '\0')
         {
          
          if(iceb_poldan("Приставка к табельному номеру",bros,"zarnast.alx",data->window) == 0)
           {
           if(iceb_u_srav_r(bros,kontr,1) == 0)
            {
             int i=strlen(bros);
             sprintf(strsql,"select nomp,vidan,datvd from Kartb where tabn=%s",&kontr[i]);
             if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
              {
               short d=0,m,g;
               iceb_u_rsdat(&d,&m,&g,row[2],2);
               if(d == 0)
                sprintf(dokum,"%s N%s %s",gettext("Паспорт"),row[0],row[1]);
               else
                sprintf(dokum,"%s N%s %02d.%02d.%d%s %s",
                gettext("Паспорт"),row[0],
                d,m,g,
                gettext("г."),
                row[1]);
              }
            }
           else
            {
             sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' and zapis like 'Паспорт|%%'",kontr);
             if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
               iceb_u_polen(row[0],dokum,sizeof(dokum),2,'|');
            }
           }
         }
       }
     }     

    if(data->tipz == 1)
     {
      class iceb_u_spisok imafr;
      class iceb_u_spisok naimr;
      char imaf[32];        
      sprintf(imaf,"kopp%d.lst",getpid());

      imafr.plus(imaf);
      naimr.plus(gettext("Приходный кассовый ордер"));
      
      iceb_kasord1(imaf,data->nomdok.ravno(),dd,md,gd,data->shetd.ravno(),data->shetkasd.ravno(),data->suma,fio,
      data->osnov.ravno(),data->dopol.ravno(),data->kod_cel_naz.ravno(),data->window);
      iceb_rabfil(&imafr,&naimr,"",0,data->window);
     }
    if(data->tipz == 2)
     {
      short ddp=dd,mdp=md,gdp=gd;
      char nomdok_p[32];
      strcpy(nomdok_p,data->nomdok.ravno());
      
      if(iceb_poldan("Не печатать в расходных ордерах дату и номер документа",strsql,"kasnast.alx",data->window) == 0)
       if(iceb_u_SRAV("Вкл",strsql,1) == 0)
        {
         memset(nomdok_p,'\0',sizeof(nomdok_p));
        }  

      class iceb_u_spisok imafr;
      class iceb_u_spisok naimr;
      char imaf[32];        

      sprintf(imaf,"kopr%d.lst",getpid());

      imafr.plus(imaf);
      naimr.plus(gettext("Расходный кассовый ордер"));

      iceb_kasord2(imaf,nomdok_p,ddp,mdp,gdp,data->shetd.ravno(),data->shetkasd.ravno(),data->suma,
      fio,data->osnov.ravno(),data->dopol.ravno(),dokum,data->kod_cel_naz.ravno(),NULL,data->window);

      iceb_rabfil(&imafr,&naimr,"",0,data->window);
     }



    break;

  case 1:
    raszko(data);
    break;

 }
   
}

/***************************************/
/*Распечатать записи в кассовом ордере*/
/**************************************/

void	raszko(class kasdok_data *data)
{
char	imaf[32],imafved[32];
FILE	*ff,*ffved;
char	naimkont[512];
SQL_str row,row1;
char	strsql[512];
time_t	tmm;
struct  tm      *bf;
double  itogo=0.;
double  suma=0.;
char     shrpz[16]; 
SQLCURSOR cur;
SQLCURSOR curr;
time(&tmm);
bf=localtime(&tmm);
int kolstr=0;

sprintf(strsql,"select kontr,suma,koment from Kasord1 where datd='%s' and \
kassa=%d and nomd='%s' and tp=%d order by kontr asc",
data->datad.ravno_sqldata(),data->kassa.ravno_atoi(),data->nomdok.ravno(),data->tipz);
//printf("strsql=%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }


sprintf(imaf,"kasor%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,NULL);
  return;
 }

iceb_u_zagolov(gettext("Распечатка списка контрагентов в кассовом ордере"),0,0,0,0,0,0,organ,ff);

fprintf(ff,"%s:%s\n",gettext("Касса"),data->kassa.ravno());
fprintf(ff,"%s:%s%s\n",gettext("Дата документа"),data->datad.ravno(),
gettext("г."));
fprintf(ff,"%s:%s\n",gettext("Номер документа"),data->nomdok.ravno());


fprintf(ff,"\
----------------------------------------------------------\n");

fprintf(ff,gettext("\
 Код |   Наименование контрагента   |  Сумма   |Коментарий\n"));
fprintf(ff,"\
----------------------------------------------------------\n");

sprintf(imafved,"kasved%d.lst",getpid());

if((ffved = fopen(imafved,"w")) == NULL)
 {
  iceb_er_op_fil(imafved,"",errno,NULL);
  return;
 }


memset(shrpz,'\0',sizeof(shrpz));

short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,data->datad.ravno(),1);
char naipod[20];
int listn=1;
int kolst=0;
memset(naipod,'\0',sizeof(naipod));


kolst=0;
if(zarstfn53w(naipod,md,gd,shrpz,data->nomv.ravno(),&kolst,ffved) != 0)
  return;
sspp1w(ffved,listn,&kolst);
int nomer_ved=0;
while(cur.read_cursor(&row) != 0)
 {
  /*Читаем наименование контрагента*/
  memset(naimkont,'\0',sizeof(naimkont));
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    strncpy(naimkont,row1[0],sizeof(naimkont)-1);
  suma=atof(row[1]);

  fprintf(ff,"%-5s|%-30.30s|%10.2f|%s\n",
  row[0],naimkont,suma,row[2]);

//  iceb_u_prnb(suma,strsql);
  kolst+=2;
  if(kolst > kol_strok_na_liste)
   {
    kolst=2;
    sspp1w(ffved,++listn,&kolst);

   }

  zarstvedw(&nomer_ved,row[0],naimkont,suma,ffved);
/*******
  fprintf(ffved,"%-3d %-7s %-35.35s",++nomer_ved,row[0],naimkont);
  fprintf(ffved,"%10s\n\
..............................................................................\n",strsql);
  
**********/
  itogo+=suma;
 }
itogw(ffved,itogo,data->window);

fprintf(ff,"\
----------------------------------------------------------\n");

fprintf(ff,"%36s:%10.2f\n",gettext("Итого"),itogo);

iceb_podpis(ff,data->window);

fclose(ff);
fclose(ffved);

iceb_u_double SUM;
iceb_u_int KOL;
SUM.plus(itogo,-1);
KOL.plus(1,-1);
sumprvedw(&SUM,&KOL,imafved);


iceb_u_spisok imafr;
iceb_u_spisok naimr;

imafr.plus(imafved);
naimr.plus(gettext("Распечатка ведомости на выдачу зарплаты"));
imafr.plus(imaf);
naimr.plus(gettext("Распечатка списка контрагентов в документе"));
for(int nomer=0; nomer < imafr.kolih(); nomer++)
 iceb_ustpeh(imafr.ravno(nomer),1,data->window);
iceb_rabfil(&imafr,&naimr,"",0,data->window);
}
