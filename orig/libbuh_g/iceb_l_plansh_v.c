/*$Id: iceb_l_plansh_v.c,v 1.31 2011-01-13 13:49:59 sasa Exp $*/
/*14.05.2010	03.01.2004	Белых А.И.	iceb_l_plansh_v.c
Ввод и корректировка счета в план счетов
*/
#include        <stdlib.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include        "iceb_libbuh.h"
#include        "iceb_l_plansh.h"

enum
 {
  E_SHET,
  E_NAIM,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK10,
  KOL_PFK
 };

class korshet_data
 {
  public:
  GtkWidget *window;
  iceb_u_str sshet;
  iceb_u_str nshet;
 };
 
class iceb_l_plansh_v
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class  plansh_rek rk;
  short      saldo; //для проверки при попытке развернуть сальдо
  short      status; //для проверки при попытке изменить статус
    
  class iceb_u_str shet_i;
  
  iceb_l_plansh_v() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM]))));
   }
 };

gboolean	obr_klav_plansh_vvod(GtkWidget *widget,GdkEventKey *event,class iceb_l_plansh_v *data);
GtkWidget       *tipsh(int*);
GtkWidget       *vidsh(int*);
GtkWidget       *tipsaldo(int*);
GtkWidget       *status(int*);
void	zplansh(class iceb_l_plansh_v *data);
void get_shet(GtkWidget *widget,char *text);
int  prnsh(const char *shet,GtkWidget*);
void		korshet(const char*,const char*,GtkWidget*);
void		korshet1(class korshet_data *data);
void  plansh_v_knopka(GtkWidget *widget,class iceb_l_plansh_v *data);
void    plansh_vvod(GtkWidget *widget,class iceb_l_plansh_v *data);

int iceb_l_plansh_v(class iceb_u_str *shetk,GtkWidget *wpredok)
{
iceb_u_str stroka;
iceb_u_str strokatit;
char		strsql[512];
SQL_str		row;
time_t          vremz=0;
int             ktoz=0;
iceb_u_str      kikz;
class iceb_l_plansh_v data;
data.shet_i.plus(shetk->ravno());

data.rk.clear_zero();

if(data.shet_i.getdlinna() > 1)
 {
  if(data.shet_i.ravno()[0] == '\0')
   {
    iceb_menu_soob(gettext("Не выбрана запись !"),wpredok);
    return(1);
   }

  SQLCURSOR cur;
  sprintf(strsql,"select * from Plansh where ns='%s'",data.shet_i.ravno());
//  g_print("vvodplansh %s\n",strsql);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.rk.shet.new_plus(row[0]);
  data.rk.naim.new_plus(row[1]);
  data.rk.tip=atoi(row[2]);
  data.rk.vid=atoi(row[3]);    
  data.saldo=data.rk.saldo=atoi(row[6]);    
  if(data.rk.saldo == 3)
    data.rk.saldo=1;
  data.status=data.rk.status=atoi(row[7]);    
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
  
  vremz=atol(row[5]);
  ktoz=atoi(row[4]);
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
//gtk_widget_set_usize(GTK_WIDGET(data.window), 600, 100);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.shet_i.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
  strokatit.new_plus(strsql);
  stroka.new_plus(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
  strokatit.new_plus(strsql);

  stroka.new_plus(gettext("Корректировка записи"));
  stroka.ps_plus(kikz.ravno());
 }
gtk_window_set_title(GTK_WINDOW(data.window),strokatit.ravno_toutf());
/**********
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(obr_klav_plansh_vvod),data);
//gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(obr_klav_plansh_o),data);
*************/
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(obr_klav_plansh_vvod),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label1=gtk_label_new(stroka.ravno_toutf());

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox), label1, FALSE, FALSE, 1);

GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 1);

GtkWidget *vbox1 = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 1);

GtkWidget *vbox2 = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 1);

GtkWidget *hbox1 = gtk_hbox_new (FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox), hbox1, FALSE, FALSE, 1);


GtkWidget *label[6];
label[0]=gtk_label_new(iceb_u_toutf(gettext("Номер счета")));
label[1]=gtk_label_new(iceb_u_toutf(gettext("Наименование счета")));
label[2]=gtk_label_new(iceb_u_toutf(gettext("Тип счета")));
label[3]=gtk_label_new(iceb_u_toutf(gettext("Вид счета")));
label[4]=gtk_label_new(iceb_u_toutf(gettext("Сальдо")));
label[5]=gtk_label_new(iceb_u_toutf(gettext("Статус")));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
GtkTooltips *tooltips0=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips0,data.knopka[PFK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(plansh_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
GtkTooltips *tooltips1=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips1,data.knopka[PFK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]), "clicked",GTK_SIGNAL_FUNC(plansh_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK10]),(gpointer)PFK10);

data.entry[E_SHET] = gtk_entry_new_with_max_length (20);
gtk_entry_set_text (GTK_ENTRY (data.entry[E_SHET]),data.rk.shet.ravno_toutf());
gtk_widget_set_usize(data.entry[E_SHET],100,-1);
gtk_signal_connect(GTK_OBJECT(data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(plansh_vvod),&data);
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

data.entry[E_NAIM] = gtk_entry_new_with_max_length (60);
gtk_widget_set_usize(data.entry[E_NAIM],400,-1);
gtk_entry_set_text (GTK_ENTRY (data.entry[E_NAIM]),data.rk.naim.ravno_toutf());
gtk_signal_connect(GTK_OBJECT(data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(plansh_vvod),&data);
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

GtkWidget *menutip = tipsh(&data.rk.tip);
GtkWidget *menuvid = vidsh(&data.rk.vid);
GtkWidget *menusaldo = tipsaldo(&data.rk.saldo);
GtkWidget *menustatus = status(&data.rk.status);



gtk_box_pack_start (GTK_BOX (vbox1), label[0], FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox2),data.entry[E_SHET], TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox1), label[1],FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox2),data.entry[E_NAIM], TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox1), label[2],FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox2), menutip, TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox1), label[3],FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox2), menuvid, TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox1), label[4],FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox2), menusaldo,TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox1), label[5],FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox2), menustatus, TRUE, TRUE, 1);

gtk_box_pack_start(GTK_BOX(hbox1), data.knopka[0], TRUE, TRUE, 1);
gtk_box_pack_start(GTK_BOX(hbox1), data.knopka[1], TRUE, TRUE, 1);


gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 shetk->new_plus(data.rk.shet.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
 
return(data.voz);

}
/*****************************************/
/*Обработчик клавиатуры на нажатие для окна ввода и корректировки записи*/
/******************************************/

gboolean	obr_klav_plansh_vvod(GtkWidget *widget,GdkEventKey *event,class iceb_l_plansh_v *data)
{
//printf("obr_klav_plansh_vvod-Нажата клавиша\n");

switch(event->keyval)
 {


  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK2]),"clicked");
    return(TRUE);


  case GDK_Escape:  //Выход и
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша переключения регистра\n");

    data->kl_shift=1;

    return(TRUE);
       

  
 }

 return(TRUE);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  plansh_v_knopka(GtkWidget *widget,class iceb_l_plansh_v *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("kontrv_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case PFK2:
    zplansh(data);
    data->voz=0;
    gtk_widget_destroy(data->window);
    
    return;  

  case PFK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}


/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    plansh_vvod(GtkWidget *widget,class iceb_l_plansh_v *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("kontr_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SHET:
    data->rk.shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(iceb_u_SRAV(data->rk.shet.ravno(),data->shet_i.ravno(),0) != 0)
      prnsh(data->rk.shet.ravno(),data->window);
    break;
  case E_NAIM:
    data->rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }
enter++;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}


/*********************************************/
/**********************************************/

GtkWidget *make_menu_item( gchar *name,GtkSignalFunc callback,gpointer data,int nomer)
{
GtkWidget *item;
  
item = gtk_menu_item_new_with_label (name);
gtk_signal_connect (GTK_OBJECT (item), "activate",callback, data);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer);
gtk_widget_show (item);
//printf("make_menu_item\n");
return(item);
}


/*****************************/
/*Меню выбора типа счета     */
/*****************************/

GtkWidget *tipsh(int *nomerstr)
{
int nomer_punk=0;
GtkWidget *opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();

GtkWidget *item = make_menu_item (iceb_u_toutf(gettext("Активный")) ,GTK_SIGNAL_FUNC(iceb_get_menu),nomerstr,nomer_punk++);
gtk_menu_append (GTK_MENU (menu), item);

item = make_menu_item (iceb_u_toutf(gettext("Пассивный")), GTK_SIGNAL_FUNC (iceb_get_menu),nomerstr,nomer_punk++);
gtk_menu_append (GTK_MENU (menu), item);

item = make_menu_item (iceb_u_toutf(gettext("Активно-пассивный")), GTK_SIGNAL_FUNC (iceb_get_menu),nomerstr,nomer_punk++);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);

gtk_option_menu_set_history(GTK_OPTION_MENU(opt),*nomerstr);
return(opt);
}

/*****************************/
/*Меню выбора вида счета     */
/*****************************/

GtkWidget *vidsh(int *nomerstr)
{
int nomer_punk=0;
GtkWidget *opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();
GtkWidget *item = make_menu_item (iceb_u_toutf(gettext("Счёт")) ,GTK_SIGNAL_FUNC(iceb_get_menu),nomerstr,nomer_punk++);
gtk_menu_append (GTK_MENU (menu), item);

item = make_menu_item (iceb_u_toutf(gettext("Субсчет")), GTK_SIGNAL_FUNC (iceb_get_menu),nomerstr,nomer_punk++);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);

//gtk_option_menu_set_history(GTK_OPTION_MENU(opt),(unsigned int)nomerstr);
gtk_option_menu_set_history(GTK_OPTION_MENU(opt),*nomerstr);
return(opt);
}
/*****************************/
/*Меню выбора типа сальдо    */
/*****************************/

GtkWidget *tipsaldo(int *nomerstr)
{
int nomer_punk=0;

GtkWidget *opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();
GtkWidget *item = make_menu_item (iceb_u_toutf(gettext("Свернутое")) ,GTK_SIGNAL_FUNC(iceb_get_menu),nomerstr,nomer_punk++);
gtk_menu_append (GTK_MENU (menu), item);

item = make_menu_item (iceb_u_toutf(gettext("Развернутое")), GTK_SIGNAL_FUNC (iceb_get_menu),nomerstr,nomer_punk++);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);
int saldo=*nomerstr;
if(saldo == 3)
  saldo=1;
gtk_option_menu_set_history(GTK_OPTION_MENU(opt),saldo);
//printf("tipsaldo=%d\n",*nomerstr);
return(opt);
}

/*****************************/
/*Меню выбора статуса счета    */
/*****************************/

GtkWidget *status(int *nomerstr)
{
int nomer_punk=0;
GtkWidget *opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();
GtkWidget *item = make_menu_item (iceb_u_toutf(gettext("Балансовый")) ,GTK_SIGNAL_FUNC(iceb_get_menu),nomerstr,nomer_punk++);
gtk_menu_append (GTK_MENU (menu), item);

item = make_menu_item (iceb_u_toutf(gettext("Внебалансовый")), GTK_SIGNAL_FUNC (iceb_get_menu),nomerstr,nomer_punk++);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);

gtk_option_menu_set_history(GTK_OPTION_MENU(opt),*nomerstr);
return(opt);
}
/************************************/
/*Запись в план счетов              */
/************************************/
void	zplansh(class iceb_l_plansh_v *data)
{
time_t vrem;
char   strsql[512];
data->read_rek();

if(data->rk.shet.ravno()[0] == '\0')
 {
  iceb_u_str SP;
  SP.plus(gettext("Не введен номер счета !"));
  iceb_menu_soob(&SP,NULL);
  return;
 }

if(data->rk.naim.ravno()[0] == '\0')
 {
  iceb_u_str SP;
  SP.plus(gettext("Не введено наименование !"));
  iceb_menu_soob(&SP,NULL);
  return;
 }

short metkor=0;

time(&vrem);
if(data->rk.saldo == 1)
 data->rk.saldo=3;
 
if(data->shet_i.getdlinna() <= 1)        
 {

  if(prnsh(data->rk.shet.ravno(),data->window) != 0)
    return;


  sprintf(strsql,"insert into Plansh \
values ('%s','%s',%d,%d,%d,%ld,%d,%d,%d,%d)",
  data->rk.shet.ravno_filtr(),
  data->rk.naim.ravno_filtr(),
  data->rk.tip,
  data->rk.vid,
  iceb_getuid(data->window),vrem,
  data->rk.saldo,
  data->rk.status,
  data->rk.val,0);
 }
else
 {
  if(data->saldo == 0 && data->rk.saldo == 3)
   {
    sprintf(strsql,"select sh from Prov where sh='%s' \
and kodkon='' limit 1",data->shet_i.ravno_filtr());
    if(sql_readkey(&bd,strsql) > 0)
     {
      iceb_u_str repl;
      
      repl.plus(gettext("По этому счету уже введены проводки без кода контрагента !"));
      repl.ps_plus(gettext("Сальдо можно развернуть после удаления этих проводок"));
      iceb_menu_soob(&repl,data->window);
      return;
     }
   }

  if(data->status != data->rk.status)
   {

    //Проверяем есть ли проводки по этому счету
    sprintf(strsql,"select sh from Prov where sh='%s' limit 1",data->shet_i.ravno_filtr());
    if(sql_readkey(&bd,strsql) > 0)
     {
      iceb_u_str repl;
      
      repl.plus(gettext("По этому счету уже введены проводки !"));
      repl.ps_plus(gettext("Статус можно изменить только после удаления всех проводок по этому счету"));
      iceb_menu_soob(&repl,data->window);
      return;
     }

   }

  if(iceb_u_SRAV(data->shet_i.ravno(),data->rk.shet.ravno(),0) != 0)
   {
    if(prnsh(data->rk.shet.ravno(),data->window) != 0)
      return;

    iceb_u_str SP;
    SP.plus(gettext("Кооректировать номер счета ? Вы уверены !"));

    if(iceb_menu_danet(&SP,2,NULL) == 2)
     return;
    metkor=1;
    
   }




  sprintf(strsql,"update Plansh \
set \
ns='%s',\
nais='%s',\
tips=%d,\
vids=%d,\
ktoi=%d,\
vrem=%ld,\
saldo=%d,\
stat=%d,\
val=%d \
where ns='%s'",
  data->rk.shet.ravno_filtr(),
  data->rk.naim.ravno_filtr(),
  data->rk.tip,
  data->rk.vid,
  iceb_getuid(data->window),vrem,
  data->rk.saldo,
  data->rk.status,
  data->rk.val,
  data->shet_i.ravno_filtr());
 }

printf("\nzplansh-strsql=%s\n",strsql);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_u_str SP;
   SP.plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&SP,NULL);
   return;
  }
 else
  {
   iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);
   return;
  }
 }
if(metkor == 1)
 {
  gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  korshet(data->shet_i.ravno(),data->rk.shet.ravno(),data->window);
 }


}


/****************************/
/*Прверка номера счета*/
/**************************/

int  prnsh(const char *shet,GtkWidget *wpredok)
{
char  strsql[512];

sprintf(strsql,"select ns from Plansh where ns='%s'",shet);
if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str SP;
  sprintf(strsql,gettext("Счет %s уже есть в плане счетов !"),shet);
  SP.plus(strsql);
  iceb_menu_soob(&SP,wpredok);
  return(1);
 }
return(0);
}


/*************************************/
/*Меню корректировки */
/*************************************/


void korshet(const char *sshet, //Старый номер счета
const char *nshet,
GtkWidget *wpredok)  //Новый номер счета
{
char strsql[512];

//printf("korshet\n");

korshet_data data;
data.sshet.plus(sshet);
data.nshet.plus(nshet);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(wpredok != NULL);
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
sprintf(strsql,"%s %s",name_system,gettext("Корректировка"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

//gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

GtkWidget *label=gtk_label_new(gettext("Ждите !!!"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

label=gtk_label_new(gettext("Выполняется каскадная корректировка в таблицах"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);


gtk_widget_show_all(data.window);
//printf("korshet 1\n");

gtk_idle_add((GtkFunction)korshet1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

}


/*****************************************************************/
/*Программа каскадной корректировки номера счета во всех таблицах*/
/*****************************************************************/
void		korshet1(class korshet_data *data)
{
char		strsql[512];

sprintf(strsql,"update Skontr set ns='%s' where ns='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_u_str SP;
   SP.plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&SP,data->window);
   return;
  }
 else
  {
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);
   return;
  }
 }
sprintf(strsql,"update Saldo set ns='%s' where ns='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);


sprintf(strsql,"update Prov set sh='%s' where sh='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Prov set shk='%s' where shk='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Kart set shetu='%s' where shetu='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);


sprintf(strsql,"update Kartb set shet='%s' where shet='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Nash set shet='%s' where shet='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);


sprintf(strsql,"update Uder set shet='%s' where shet='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);


sprintf(strsql,"update Zarp set shet='%s' where shet='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);


sprintf(strsql,"update Zarn set shet='%s' where shet='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Zarn1 set shet='%s' where shet='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Zarsoc set shet='%s' where shet='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Zarsocz set shet='%s' where shet='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Uosinp set shetu='%s' where shetu='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Uslugi set shetu='%s' where shetu='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Usldokum1 set shetu='%s' where shetu='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Usldokum2 set shetu='%s' where shetu='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Kas set shet='%s' where shet='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Kasop1 set shetkas='%s' where shetkas='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Kasop1 set shetkor='%s' where shetkor='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Kasop2 set shetkas='%s' where shetkas='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Kasop2 set shetkor='%s' where shetkor='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Kasord set shetk='%s' where shetk='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Ukrkras set shet='%s' where shet='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Ukrdok1 set shet='%s' where shet='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Upldok2 set shet='%s' where shet='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Uplost set shet='%s' where shet='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

sprintf(strsql,"update Opldok set shet='%s' where shet='%s'",data->nshet.ravno_filtr(),data->sshet.ravno_filtr());

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

gtk_widget_destroy(data->window);

}

