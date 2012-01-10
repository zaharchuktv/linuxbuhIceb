/*$Id: l_nahud_t.c,v 1.31 2011-04-14 16:09:35 sasa Exp $*/
/*27.06.2010	30.08.2006	Белых А.И.	l_nahud_t.c
Работа с начислениями/удержаниями по конкретному табельному номеру
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"
#include "vkartz.h"



enum
{
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 SFK5,
 FK6,
 FK7,
 FK8,
 FK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_KOD,
 COL_NAIM,
 COL_DEN,
 COL_MES_GOD,
 COL_SUMA,
 COL_SHET,
 COL_NOM_ZAP,
 COL_PODR,
 COL_KOMENT,
 COL_KDATA,
 COL_NOMDOK,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  l_nahud_t_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей
  
  class vkartz_vs *rk;
  short prn;
  char imafprot[56];
  int metka_avr;  
  //Реквизиты выбранной строки
  short den;
  short knah;
  short god,mes; //дата в счёт которой сумма
  int podr;
  class iceb_u_str shet;
  int nom_zap;
  class iceb_u_str nom_dok;
          
  //Конструктор
  l_nahud_t_data()
   {
    memset(imafprot,'\0',sizeof(imafprot));
    sprintf(imafprot,"zrpr%d.lst",getpid());
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    metka_avr=0;
   }      
 };

gboolean   l_nahud_t_key_press(GtkWidget *widget,GdkEventKey *event,class l_nahud_t_data *data);
void l_nahud_t_vibor(GtkTreeSelection *selection,class l_nahud_t_data *data);
void l_nahud_t_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class l_nahud_t_data *data);
void  l_nahud_t_knopka(GtkWidget *widget,class l_nahud_t_data *data);
void l_nahud_t_add_columns (GtkTreeView *treeview);
void l_nahud_t_create_list(class l_nahud_t_data *data);

void l_nahud_t_vnzs(class  l_nahud_t_data *data);
void l_nahud_t_vnzpk(class  l_nahud_t_data *data);
void l_nahud_t_ar(class  l_nahud_t_data *data);
void l_nahud_t_pm(class  l_nahud_t_data *data);

extern SQL_baza	bd;
extern char *name_system;
extern char     *organ;
extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
//extern float dnei,hasov; //Количество рабочих дней и часов в определённом месяце. Читает программа redkalw.c перед началом расчёта начислений/удержаний
 
void l_nahud_t(int prn, //1-просмотр начислений 2-удержаний
class vkartz_vs *rek_kart, //реквизиты карточки
GtkWidget *wpredok)
{
char strsql[512];
class  l_nahud_t_data data;
data.rk=rek_kart;
data.prn=prn;

//Читаем настройки здесь, так как после выхода расчитываются отчисления на фонд зарплаты
zar_read_tnw(1,data.rk->mp,data.rk->gp,NULL,wpredok);


data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


if(data.prn == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Начисления"));
if(data.prn == 2)
  sprintf(strsql,"%s %s",name_system,gettext("Удержания"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_nahud_t_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

if(data.prn == 1)
  data.label_kolstr=gtk_label_new (gettext("Начисления"));
if(data.prn == 2)
  data.label_kolstr=gtk_label_new (gettext("Удержания"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),-1,200);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_widget_show (data.sw);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];


sprintf(strsql,"F2 %s",gettext("Начисления"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_nahud_t_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Просмотр начислений"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_nahud_t_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_nahud_t_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"%sF3 %s",RFK,gettext("Обнуление"));
data.knopka[SFK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(l_nahud_t_knopka),&data);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Обнуление всех записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(strsql,"F4 %s",gettext("Удержания"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_nahud_t_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Просмотр удержаний"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"F5 %s",gettext("Ввести"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_nahud_t_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Ввод новой записи c выбором из списка"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"%sF5 %s",RFK,gettext("Ввести"));
data.knopka[SFK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK5]), "clicked",GTK_SIGNAL_FUNC(l_nahud_t_knopka),&data);
tooltips[SFK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK5],data.knopka[SFK5],gettext("Ввод новой записи по коду"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK5]),(gpointer)SFK5);
gtk_widget_show(data.knopka[SFK5]);

sprintf(strsql,"F6 %s",gettext("Расчёт"));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(l_nahud_t_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
if(data.prn == 1)
 gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Расчитать начисления"),NULL);
if(data.prn == 2)
 gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Расчитать удержания"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(strsql,"F7 %s",gettext("Пред."));
data.knopka[FK7]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(l_nahud_t_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Просмотреть записи предыдущего месяца"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(strsql,"F8 %s",gettext("НСИ"));
data.knopka[FK8]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(l_nahud_t_knopka),&data);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Ввод и корректировка нормативно-справочной информации"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);

sprintf(strsql,"F9 %s",gettext("Протокол"));
data.knopka[FK9]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(l_nahud_t_knopka),&data);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Просмотр протокола хода расчёта"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_nahud_t_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_nahud_t_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();

unlink(data.imafprot);

//printf("l_l_nahud_t end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


return;

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_nahud_t_knopka(GtkWidget *widget,class l_nahud_t_data *data)
{
class ZARP zp;
char strsql[512];
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_nahud_t_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2: //Просмотр нарахувань
    if(data->prn == 1)
     return;
    data->prn=1;
    data->metka_avr=0;
    l_nahud_t_create_list(data);
    return;  

  case SFK2: //Коригування запису
    if(provblokzarpw(data->rk->tabnom.ravno_atoi(),data->rk->mp,data->rk->gp,data->rk->flagrk,0,data->window) != 0)
     return;

    l_nahud_t_v(data->rk->tabnom.ravno_atoi(),data->den,data->rk->mp,data->rk->gp,data->prn,data->knah,
    data->mes,data->god,data->podr,data->shet.ravno(),data->nom_zap,data->window);

    l_nahud_t_create_list(data);
    
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;

    if(provblokzarpw(data->rk->tabnom.ravno_atoi(),data->rk->mp,data->rk->gp,data->rk->flagrk,0,data->window) != 0)
     return;

    if(data->nom_dok.getdlinna() > 1)
     {
      iceb_menu_soob(gettext("Записи сделанные из документов, удаляются только в документах !"),data->window);
      return;
     }
  
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
     return;

    
    zp.tabnom=data->rk->tabnom.ravno_atoi();
    zp.prn=data->prn;
    zp.knu=data->knah;
    zp.dz=data->den;
    zp.mz=data->rk->mp;
    zp.gz=data->rk->gp;
    zp.godn=data->god;
    zp.mesn=data->mes;
    zp.podr=data->podr;
    strncpy(zp.shet,data->shet.ravno(),sizeof(zp.shet)-1);
    
    zarudnuw(&zp,data->window);

    l_nahud_t_create_list(data);
    return;  

  case SFK3:

    if(data->kolzap == 0)
      return;

    if(provblokzarpw(data->rk->tabnom.ravno_atoi(),data->rk->mp,data->rk->gp,data->rk->flagrk,0,data->window) != 0)
     return;

  
    if(iceb_menu_danet(gettext("Обнулить все записи ? Вы уверены ?"),2,data->window) == 2)
     return;

    sprintf(strsql,"update Zarp \
set \
suma=0. \
where datz >='%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and \
prn=%d and nd = ''",data->rk->gp,data->rk->mp,data->rk->gp,data->rk->mp,data->rk->tabnom.ravno_atoi(),data->prn);

    iceb_sql_zapis(strsql,1,0,data->window);

    l_nahud_t_create_list(data);
    return;  

  case FK4: //Просмотр утримань
    if(data->prn == 2)
     return;
    data->prn=2;
    data->metka_avr=0;
    l_nahud_t_create_list(data);
    return;  

  case FK5:
    if(provblokzarpw(data->rk->tabnom.ravno_atoi(),data->rk->mp,data->rk->gp,data->rk->flagrk,0,data->window) != 0)
     return;
    l_nahud_t_vnzs(data);
    l_nahud_t_create_list(data);
    return;  

  case SFK5:
    if(provblokzarpw(data->rk->tabnom.ravno_atoi(),data->rk->mp,data->rk->gp,data->rk->flagrk,0,data->window) != 0)
     return;
    l_nahud_t_vnzpk(data);
    l_nahud_t_create_list(data);
    return;  

  case FK6:
    if(provblokzarpw(data->rk->tabnom.ravno_atoi(),data->rk->mp,data->rk->gp,data->rk->flagrk,0,data->window) != 0)
     return;
    l_nahud_t_ar(data);
    data->metka_avr=1;
    l_nahud_t_create_list(data);
    return;  

  case FK7:
    l_nahud_t_pm(data);
    return;  


  case FK8: //Работа с НСИ
    v_nsi(data->window);  
    //Читаем настройки 
    zar_read_tnw(1,data->rk->mp,data->rk->gp,NULL,data->window);
    return;  

  case FK9:
    iceb_prosf(data->imafprot,data->window);
    return;  
    
  case FK10: 
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_nahud_t_key_press(GtkWidget *widget,GdkEventKey *event,class l_nahud_t_data *data)
{
iceb_u_str repl;
//printf("l_nahud_t_key_press keyval=%d state=%d\n",event->keyval,event->state);

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

  case GDK_plus: //это почемуто не работает Нажата клавиша плюс на основной клавиатуре
  case GDK_KP_Add: //Нажата клавиша "+" на дополнительной клавиатуре
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
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
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
    printf("l_nahud_t_key_press-Нажата клавиша Shift\n");

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
void l_nahud_t_create_list (class l_nahud_t_data *data)
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
iceb_u_str zagolov;
short prohod=0;

//printf("l_nahud_t_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_nahud_t_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_nahud_t_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

nazad:;

sprintf(strsql,"select * from Zarp where datz >='%04d-%02d-01' and \
datz <= '%04d-%02d-31' and tabn=%d and prn='%d' order by \
knah,godn,mesn,nomz asc",
data->rk->gp,data->rk->mp,data->rk->gp,data->rk->mp,data->rk->tabnom.ravno_atoi(),data->prn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);
if(kolstr == 0 && prohod == 0)
 {
  prohod++;
  if(provblokzarpw(data->rk->tabnom.ravno_atoi(),data->rk->mp,data->rk->gp,data->rk->flagrk,0,data->window) != 0)
    goto nazad;  
  zapzagotw(data->rk->mp,data->rk->gp,data->rk->tabnom.ravno_atoi(),data->prn,data->rk->podr.ravno_atoi(),data->window);
  goto nazad;  
 }

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
char naim[512];
float kolstr1=0.;
short d,m,g;

SQL_str row1;
double suma_i=0.;
double suma=0.;
double sumaib=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  //Код начисления/удержания
  ss[COL_KOD].new_plus(iceb_u_toutf(row[3]));

  memset(naim,'\0',sizeof(naim));
  //Читаем наименование начисления/удержания
  if(data->prn == 1)
    sprintf(strsql,"select naik from Nash where kod=%s",row[3]);
  if(data->prn == 2)
    sprintf(strsql,"select naik from Uder where kod=%s",row[3]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strncpy(naim,row1[0],sizeof(naim)-1);
   
  //Наименование
  ss[COL_NAIM].new_plus(iceb_u_toutf(naim));

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  ss[COL_DEN].new_plus(d);
  
  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[11])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[10],0,data->window));
  
  //В счёт какого месяца
  sprintf(strsql,"%02d.%d",atoi(row[7]),atoi(row[6]));
  ss[COL_MES_GOD].new_plus(iceb_u_toutf(strsql));

  //Сумма
  suma=atof(row[4]);
  sprintf(strsql,"%8.2f",suma);
  ss[COL_SUMA].new_plus(strsql);
  suma_i+=suma;

  if(data->prn == 1 && shetb != NULL)
    if(iceb_u_proverka(shetb,row[5],0,1) == 0)
       sumaib+=suma; 
  if(data->prn == 2 && shetbu != NULL)
    if(iceb_u_proverka(shetbu,row[5],0,1) == 0)
       sumaib+=suma; 
  
  //Номер записи
  ss[COL_NOM_ZAP].new_plus(row[9]);

  //Подразделение     
  ss[COL_PODR].new_plus(row[13]);

  //Коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[12]));

  //Номер документа
  ss[COL_NOMDOK].new_plus(iceb_u_toutf(row[14]));

  //Счёт
  ss[COL_SHET].new_plus(iceb_u_toutf(row[5]));


  sprintf(strsql,"select datd from Zarn1 where tabn=%d and prn='%d' and knah=%s",
  data->rk->tabnom.ravno_atoi(),data->prn,row[3]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    d=m=g=0;
   }
  else
    iceb_u_rsdat(&d,&m,&g,row1[0],2);
  
  ss[COL_KDATA].new_plus("");
  if(d != 0)
   {
    sprintf(strsql,"%02d.%02d.%d",d,m,g);
    ss[COL_KDATA].new_plus(strsql);
   }
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,ss[COL_KOD].ravno(),
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_SUMA,ss[COL_SUMA].ravno(),
  COL_NOM_ZAP,ss[COL_NOM_ZAP].ravno(),
  COL_PODR,ss[COL_PODR].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_MES_GOD,ss[COL_MES_GOD].ravno(),
  COL_NOMDOK,ss[COL_NOMDOK].ravno(),
  COL_DEN,ss[COL_DEN].ravno(),
  COL_KDATA,ss[COL_KDATA].ravno(),
  COL_SHET,ss[COL_SHET].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
gtk_tree_view_set_model (GTK_TREE_VIEW(data->treeview),GTK_TREE_MODEL (model));

g_object_unref(GTK_TREE_MODEL (model));

l_nahud_t_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),TRUE);//Доступна
 }

if(data->prn == 2)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK4]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
 }
if(data->prn == 1)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK4]),TRUE);//Доступна
 }

if(data->metka_avr == 0)
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),FALSE);//Недоступна
else
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),TRUE);//Доступна

gtk_widget_show (data->treeview);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

if(data->prn == 1)
 zagolov.new_plus(gettext("Начисления"));
if(data->prn == 2)
 zagolov.new_plus(gettext("Удержания"));
 
zagolov.plus(". ");
zagolov.plus(gettext("Дата"));
zagolov.plus(":");
zagolov.plus(data->rk->mp);
zagolov.plus(".");
zagolov.plus(data->rk->gp);
zagolov.plus(gettext("г."));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

zagolov.ps_plus(data->rk->tabnom.ravno());
zagolov.plus(" ");
zagolov.plus(data->rk->fio.ravno());

zagolov.plus(" ");
zagolov.plus(gettext("Сумма всех записей"));
zagolov.plus(":");
zagolov.plus(suma_i);

if(shetb != NULL)
 if(data->prn == 1)
  {
   sprintf(strsql,"/%.2f/%.2f",sumaib,suma_i-sumaib);
   zagolov.plus(strsql);
  }
if(shetbu != NULL)
 if(data->prn == 2)
  {
   sprintf(strsql,"/%.2f/%.2f",sumaib,suma_i-sumaib);
   zagolov.plus(strsql);
  }
zagolov.ps_plus(gettext("Подразделение"));
zagolov.plus(":");
zagolov.plus(data->rk->podr.ravno());

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void l_nahud_t_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

//printf("l_nahud_t_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("День"), renderer,"text", COL_DEN,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("В счёт"), renderer,"text", COL_MES_GOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт"), renderer,"text", COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"N", renderer,"text", COL_NOM_ZAP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Подразделение"), renderer,"text", COL_PODR,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Действует до"), renderer,"text", COL_KDATA,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Ном.док."), renderer,"text", COL_NOMDOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

//printf("l_nahud_t_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void l_nahud_t_vibor(GtkTreeSelection *selection,class l_nahud_t_data *data)
{
//printf("l_nahud_t_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *den;
gchar *kod;
gchar *mes_god;
gchar *podr;
gchar *shet;
gchar *nom_zap;
gchar *nom_dok;
gint  nomer;

gtk_tree_model_get(model,&iter,COL_DEN,&den,COL_KOD,&kod,COL_MES_GOD,&mes_god,
COL_PODR,&podr,COL_SHET,&shet,COL_NOM_ZAP,&nom_zap,COL_NOMDOK,&nom_dok,NUM_COLUMNS,&nomer,-1);

data->den=atoi(den);
data->knah=atoi(kod);

short m=0,g=0;
iceb_u_rsdat1(&m,&g,mes_god);
data->mes=m;
data->god=g;

data->podr=atoi(podr);
data->shet.new_plus(shet);
data->nom_zap=atoi(nom_zap);
data->nom_dok.new_plus(nom_dok);


data->snanomer=nomer;

g_free(den);
g_free(kod);
g_free(mes_god);
g_free(podr);
g_free(shet);
g_free(nom_zap);
g_free(nom_dok);

//printf("l_nahud_t_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void l_nahud_t_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_nahud_t_data *data)
{
//printf("l_nahud_t_v_row\n");
//printf("l_nahud_t_v_row корректировка\n");

gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");

}

/**********************/
/*Ввод новой записи с выбором из списка*/
/**************************************/

void l_nahud_t_vnzs(class  l_nahud_t_data *data)
{

if(provblokzarpw(data->rk->tabnom.ravno_atoi(),data->rk->mp,data->rk->gp,data->rk->flagrk,0,data->window) != 0)
    return;
class iceb_u_str kod;
class iceb_u_str naim;
kod.plus("");
naim.plus("");

if(data->prn == 1)
 if(l_zarnah(1,&kod,&naim,data->window) != 0)
  return;
if(data->prn == 2)
 if(l_zarud(1,&kod,&naim,data->window) != 0)
  return;

l_nahud_t_v(data->rk->tabnom.ravno_atoi(),
0,data->rk->mp,data->rk->gp,data->prn,kod.ravno_atoi(),data->rk->mp,data->rk->gp,data->rk->podr.ravno_atoi(),
"",0,data->window);

}
/******************************/
/*Ввод новой записи по коду*/
/*****************************/
void l_nahud_t_vnzpk(class  l_nahud_t_data *data)
{
if(provblokzarpw(data->rk->tabnom.ravno_atoi(),data->rk->mp,data->rk->gp,data->rk->flagrk,0,data->window) != 0)
    return;
char kod[32];
memset(kod,'\0',sizeof(kod));
char strsql[512];
memset(strsql,'\0',sizeof(strsql));

if(data->prn == 1)
 strncpy(strsql,gettext("Введите код начисления"),sizeof(strsql)-1);
if(data->prn == 2)
 strncpy(strsql,gettext("Введите код удержания"),sizeof(strsql)-1);
 
if(iceb_menu_vvod1(strsql,kod,10,data->window) != 0)
 return;
if(kod[0] == '\0')
 {
  l_nahud_t_vnzs(data);
  return;
 }

l_nahud_t_v(data->rk->tabnom.ravno_atoi(),
0,data->rk->mp,data->rk->gp,data->prn,atoi(kod),data->rk->mp,data->rk->gp,data->rk->podr.ravno_atoi(),
"",0,data->window);

}
/**************************/
/*Автоматический расчёт начислений/удержаний*/
/********************************/
void l_nahud_t_ar(class  l_nahud_t_data *data)
{
if(provblokzarpw(data->rk->tabnom.ravno_atoi(),data->rk->mp,data->rk->gp,data->rk->flagrk,0,data->window) != 0)
    return;


FILE *ff;

if((ff=fopen(data->imafprot,"w")) == NULL)
  iceb_er_op_fil(data->imafprot,"",errno,data->window);

iceb_u_startfil(ff);


//Читаем настройки повторно - может их ктого изменил
zarreadnw(1,data->rk->mp,data->rk->gp,data->prn,ff,data->window);

rasshetzw(data->rk->tabnom.ravno_atoi(),data->rk->mp,data->rk->gp,data->rk->podr.ravno_atoi(),data->prn,"","",ff,data->window); /*Расчет*/

fclose(ff);

}
/*************************/
/*Просмотр предыдущего месяца*/
/****************************/

void l_nahud_t_pm(class  l_nahud_t_data *data)
{
char strsql[512];
short d=1,m=data->rk->mp,g=data->rk->gp;
iceb_u_dpm(&d,&m,&g,4);//Уменьшаем на месяц

sprintf(strsql,"select knah,mesn,godn,suma from Zarp where \
datz >='%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%d and prn=%d and suma <> 0.",
g,m,g,m,data->rk->tabnom.ravno_atoi(),data->prn);
SQLCURSOR cur;
SQLCURSOR cur1;

int kolstr=0;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  return;
 }

FILE *ff;
char imaf[30];
sprintf(imaf,"prm%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }
SQL_str row,row1;

fprintf(ff,"%s %d.%d%s %s %s %s\n",
gettext("Дата"),m,g,
gettext("г."),
gettext("Табельный номер"),data->rk->tabnom.ravno(),data->rk->fio.ravno());

fprintf(ff,"\
-----------------------------------------------------\n");

char naim[512];
double suma;
double sumai=0.;
while(cur.read_cursor(&row) != 0)
 {
  if(data->prn == 1)
    sprintf(strsql,"select naik from Nash where kod=%s",row[0]);
  if(data->prn == 2)
    sprintf(strsql,"select naik from Uder where kod=%s",row[0]);
  memset(naim,'\0',sizeof(naim));     
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    strncpy(naim,row1[0],sizeof(naim)-1);
  
  suma=atof(row[3]);
  sumai+=suma;
  
  fprintf(ff,"%2s %-30.30s %2s.%4s %8.2f\n",
  row[0],naim,row[1],row[2],suma);
 }

fprintf(ff,"\
-----------------------------------------------------\n");
fprintf(ff,"%41s %8.2f\n",gettext("Итого"),sumai);

iceb_podpis(ff,data->window);
fclose(ff);

iceb_prosf(imaf,data->window);

unlink(imaf);

}



