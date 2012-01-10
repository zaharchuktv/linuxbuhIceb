/*$Id: l_nahud_vz.c,v 1.12 2011-04-14 16:09:35 sasa Exp $*/
/*10.03.2009	04.10.2006	Белых А.И.	l_nahud_vz.c
Вставка начислений/удержаний
*/
#include <stdlib.h>
#include <pwd.h>
#include "buhg_g.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SUMA,
  E_DEN_ZAP,
  E_VKM,
  E_KOMENT,
  E_NOMZAP,
  E_PODR,
  KOLENTER  
 };

class l_nahud_vz_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *radiobutton2[3];
  GtkWidget *opt;
  short kl_shift;
  int voz;  
  
  //реквизиты меню
  class iceb_u_str suma;
  class iceb_u_str den_zap;
  class iceb_u_str vkm;
  class iceb_u_str koment;
  class iceb_u_str nomzap;
  class iceb_u_str podr;
  short pol;
    
  //Полученные данные
  short mp;
  short gp;
  int kod_nah_ud;
  int prn;
  
  l_nahud_vz_data() //Конструктор
   {
    kl_shift=0;
    voz=0;  
    clear_data();
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }

  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    clear_data();
   }

  void clear_data()
   {
    suma.new_plus("");
    den_zap.new_plus("");
    vkm.new_plus("");
    koment.new_plus("");
    nomzap.new_plus("");
    podr.new_plus("");
    pol=0;    
   }
 };

gboolean   l_nahud_vz_key_press(GtkWidget *widget,GdkEventKey *event,class l_nahud_vz_data *data);
void    l_nahud_vz_vvod(GtkWidget *widget,class l_nahud_vz_data *data);
void  l_nahud_vz_knopka(GtkWidget *widget,class l_nahud_vz_data *data);

void  l_nahud_v_e_knopka(GtkWidget *widget,class l_nahud_vz_data *data);
int l_nahud_vz_zap(class l_nahud_vz_data *data);
void  l_nahud_vz_radio2_0(GtkWidget *widget,class l_nahud_vz_data *data);
void  l_nahud_vz_radio2_1(GtkWidget *widget,class l_nahud_vz_data *data);
void  l_nahud_vz_radio2_2(GtkWidget *widget,class l_nahud_vz_data *data);

extern char *name_system;
extern SQL_baza bd;
extern double   okrg; /*Округление*/

int l_nahud_vz(int prn,int kod_nah_ud,
short mp,short gp,
GtkWidget *wpredok)
{
class l_nahud_vz_data data;
char strsql[512];
data.mp=mp;
data.gp=gp;
data.kod_nah_ud=kod_nah_ud;
data.prn=prn;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Ввод записей"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_nahud_vz_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Ввод записей"));

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator1=gtk_hseparator_new();
GtkWidget *separator3=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox), separator1);
//Вставляем радиокнопки
GSList *group;

data.radiobutton2[0]=gtk_radio_button_new_with_label(NULL,gettext("Всем"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[0]), "clicked",GTK_SIGNAL_FUNC(l_nahud_vz_radio2_0),&data);
if(data.pol == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[0]),TRUE); //Устанавливем активной кнопку
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton2[0]));

data.radiobutton2[1]=gtk_radio_button_new_with_label(group,gettext("Только мужчинам"));
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[1]), "clicked",GTK_SIGNAL_FUNC(l_nahud_vz_radio2_1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);
if(data.pol == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[1]),TRUE); //Устанавливем активной кнопку

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton2[1]));

data.radiobutton2[2]=gtk_radio_button_new_with_label(group,gettext("Только женщинам"));
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[2]), "clicked",GTK_SIGNAL_FUNC(l_nahud_vz_radio2_2),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[2], TRUE, TRUE, 0);
if(data.pol == 2)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[2]),TRUE); //Устанавливем активной кнопку

gtk_container_add (GTK_CONTAINER (vbox), separator3);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Cумма"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE, 0);

data.entry[E_SUMA] = gtk_entry_new_with_max_length(10);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(l_nahud_vz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA]),(gpointer)E_SUMA);

sprintf(strsql,"%s",gettext("День записи"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DEN_ZAP]), label, FALSE, FALSE, 0);

data.entry[E_DEN_ZAP] = gtk_entry_new_with_max_length(2);
gtk_box_pack_start (GTK_BOX (hbox[E_DEN_ZAP]), data.entry[E_DEN_ZAP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DEN_ZAP]), "activate",GTK_SIGNAL_FUNC(l_nahud_vz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DEN_ZAP]),data.suma.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DEN_ZAP]),(gpointer)E_DEN_ZAP);

sprintf(strsql,"%s",gettext("Номер записи"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMZAP]), label, FALSE, FALSE, 0);

data.entry[E_NOMZAP] = gtk_entry_new_with_max_length(2);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMZAP]), data.entry[E_NOMZAP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMZAP]), "activate",GTK_SIGNAL_FUNC(l_nahud_vz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMZAP]),data.suma.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMZAP]),(gpointer)E_NOMZAP);

sprintf(strsql,"%s",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(l_nahud_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подразделения"),NULL);

data.entry[E_PODR] = gtk_entry_new_with_max_length(10);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(l_nahud_vz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);



sprintf(strsql,"%s (%s)",gettext("В счёт какого месяца"),gettext("м.г"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_VKM]), label, FALSE, FALSE, 0);

data.entry[E_VKM] = gtk_entry_new_with_max_length(7);
gtk_box_pack_start (GTK_BOX (hbox[E_VKM]), data.entry[E_VKM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VKM]), "activate",GTK_SIGNAL_FUNC(l_nahud_vz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VKM]),data.vkm.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VKM]),(gpointer)E_VKM);



sprintf(strsql,"%s",gettext("Коментарий"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(l_nahud_vz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Сделать записи"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_nahud_vz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(l_nahud_vz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_nahud_vz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));





return(data.voz);


}

/********************/
/*работа радиокнопок*/
/*********************/
void  l_nahud_vz_radio2_0(GtkWidget *widget,class l_nahud_vz_data *data)
{

data->pol=0;

}
void  l_nahud_vz_radio2_1(GtkWidget *widget,class l_nahud_vz_data *data)
{
data->pol=1;
}
void  l_nahud_vz_radio2_2(GtkWidget *widget,class l_nahud_vz_data *data)
{
data->pol=2;
}


/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_nahud_v_e_knopka(GtkWidget *widget,class l_nahud_vz_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_nahud_v_e_knopka knop=%d\n",knop);

switch (knop)
 {

  case E_PODR:

    if(l_zarpodr(1,&kod,&naim,data->window) == 0)
     data->podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->podr.ravno_toutf());

    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_nahud_vz_key_press(GtkWidget *widget,GdkEventKey *event,class l_nahud_vz_data *data)
{

//printf("l_nahud_vz_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_nahud_vz_knopka(GtkWidget *widget,class l_nahud_vz_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_nahud_vz_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    if(l_nahud_vz_zap(data) != 0)
     return;

    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_nahud_vz_vvod(GtkWidget *widget,class l_nahud_vz_data *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_nahud_vz_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_PODR:
    data->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SUMA:
    data->suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DEN_ZAP:
    data->den_zap.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMZAP:
    data->nomzap.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_VKM:
    data->vkm.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOMENT:
    data->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }

enter+=1;

if(enter >= KOLENTER)
enter=0;


gtk_widget_grab_focus(data->entry[enter]);
 
}

/****************************/
/*ввод записей*/
/***************************/
int l_nahud_vz_zap(class l_nahud_vz_data *data)
{
char strsql[512];
data->read_rek(); //Читаем реквизиты меню
short vkm,vkg;

if(iceb_u_rsdat1(&vkm,&vkg,data->vkm.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели в счёт какого месяца !"),data->window);
  return(1);     
 }

//проверяем код подразделения
if(data->podr.getdlinna() > 1)
 {
  sprintf(strsql,"select naik from Podr where kod=%d",data->podr.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),data->podr.ravno_atoi());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }
short d,m,g;

sprintf(strsql,"%d.%d.%d",data->den_zap.ravno_atoi(),data->mp,data->gp);
if(iceb_u_rsdat(&d,&m,&g,strsql,0) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введён день начисления/удержания !"),data->window);
  return(1);
 }

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

char shetu[30];
memset(shetu,'\0',sizeof(shetu));
SQLCURSOR cur,cur1;
SQL_str row,row1;

/*Читаем наименование*/
if(data->prn == 1)
 sprintf(strsql,"select shet from Nash where kod=%d",data->kod_nah_ud);
if(data->prn == 2)
 sprintf(strsql,"select shet from Uder where kod=%d",data->kod_nah_ud);

if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
  strncpy(shetu,row[0],sizeof(shetu)-1);



short metkaproc=0;
if(iceb_u_polen(data->suma.ravno(),strsql,sizeof(strsql),1,'%') == 0)
  metkaproc=1;

double proc,suma;

proc=suma=data->suma.ravno_atof();
suma=iceb_u_okrug(suma,okrg);
if(data->prn == 2)
 {
  proc*=-1;
  suma*=-1;
 }

if(data->podr.getdlinna() <= 1)
  sprintf(strsql,"select tabn,fio,podr,shet,kodg,kateg,sovm,zvan,lgoti,\
datn,datk,dolg,pl from Kartb where datn <= '%04d-%d-31' and \
(datk >= '%04d-%d-31' or datk = '0000-00-00')",
  data->gp,data->mp,data->gp,data->mp);
else
  sprintf(strsql,"select tabn,fio,podr,shet,kodg,kateg,sovm,zvan,lgoti,\
datn,datk,dolg,pl from Kartb where datn <= '%04d-%d-31' and \
(datk >= '%04d-%d-31' or datk = '0000-00-00') and podr=%d",
  data->gp,data->mp,data->gp,data->mp,data->podr.ravno_atoi());

int kolstr=0;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  return(1);
 }
char shetkar[20];

float kolstr1=0.;
int tabn=0;
int podr=0;
char shet[32];
int kateg;
int sovm;
int zvan;
char lgot[50];
short dn,mn,gn;
short du,mu,gu;
char dolg[512];

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
//  strzag(LINES-1,0,kolstr,++kolstr1);

  if(data->pol == 1 && atoi(row[12]) != 0)
      continue;      

  if(data->pol == 2 && atoi(row[12]) != 1)
      continue;      

  tabn=atoi(row[0]);
  podr=atoi(row[2]);  

  memset(shet,'\0',sizeof(shet));
  if(shetu[0] != '\0')
   {
    strcpy(shet,shetu);
   }
  else
    strcpy(shet,row[3]);

  strncpy(shetkar,row[3],sizeof(shetkar)-1);
  
  kateg=atoi(row[5]);
  sovm=atoi(row[6]);
  zvan=atoi(row[7]);
  strcpy(lgot,row[8]);
  iceb_u_rsdat(&dn,&mn,&gn,row[9],2);
  iceb_u_rsdat(&du,&mu,&gu,row[10],2);
  strcpy(dolg,row[11]);
  
  //Проверяем блокировку карточки
  sprintf(strsql,"select blok,podr from Zarn where god=%d and mes=%d \
and tabn=%s",data->gp,data->mp,row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(atoi(row1[0]) != 0)
     {
      struct  passwd  *ktoz; /*Кто записал*/
      int blok=atoi(row1[0]);
      class iceb_u_str repl;
      repl.plus(gettext("Табельный номер"));
      repl.plus(" ");
      repl.plus(row[0]);
      
      if((ktoz=getpwuid(blok)) != NULL)
         sprintf(strsql,"%s %d %s",gettext("Карточка заблокирована !"),blok,ktoz->pw_gecos);
      else
         sprintf(strsql,gettext("Заблокировано неизвестным логином %d !"),blok);
      repl.ps_plus(strsql);
      
      iceb_menu_soob(&repl,data->window);
      continue;

     }
    podr=atoi(row1[1]);
   }
  else
   zapzarnw(data->mp,data->gp,podr,tabn,kateg,sovm,zvan,shetkar,lgot,dn,mn,gn,du,mu,gu,0,dolg,data->window);

  /*Проверяем записан ли стандартный набор*/
  sprintf(strsql,"select datz from Zarp where datz >='%04d-%02d-01' and \
  datz <= '%04d-%02d-31' and tabn=%d and prn='%d' order by knah,nomz asc",
  data->gp,data->mp,data->gp,data->mp,tabn,data->prn);

  if(iceb_sql_readkey(strsql,data->window) <= 0)
    zapzagotw(data->mp,data->gp,tabn,data->prn,podr,data->window);

  //Удаляем нулевые начисления/удержания
  sprintf(strsql,"delete from Zarp where datz >= '%04d-%d-01' and \
datz <= '%04d-%d-31' and tabn=%d and prn='%d' and knah=%d and suma=0.",
  data->gp,data->mp,data->gp,data->mp,tabn,data->prn,data->kod_nah_ud);

  iceb_sql_zapis(strsql,1,0,data->window);

  class ZARP zp;

  zp.tabnom=tabn;
  zp.prn=data->prn;
  zp.knu=data->kod_nah_ud;
  zp.dz=data->den_zap.ravno_atoi();
  zp.mz=data->mp;
  zp.gz=data->gp;
  zp.godn=vkg;
  zp.mesn=vkm;
  zp.nomz=data->nomzap.ravno_atoi();
  zp.podr=podr;
  strncpy(zp.shet,shet,sizeof(zp.shet)-1);
/*
  memset(&zp,'\0',sizeof(zp));  zp.dz=data->dez_zap.ravno_atoi();
  zp.mesn=data->mp; zp.godn=data->gp;
  zp.nomz=0;
  zp.podr=podr;
  strcpy(zp.shet,shet);
*/
  if(metkaproc == 1)
   {
    int i=0;
    suma=okladw(tabn,data->mp,data->gp,&i,data->window)*proc/100.;
//    suma=okrug(suma,okrg);
    suma=iceb_u_okrug(suma,10.);
   }
  zapzarpw(&zp,suma,data->den_zap.ravno_atoi(),vkm,vkg,0,shet,data->koment.ravno(),data->nomzap.ravno_atoi(),podr,"",data->window);
//  zapzarpw(d,data->mp,data->gp,tabn,data->prn,data->kod_nah_ud,suma,shet,mp,gp,0,nomz,koment,podr,"",zp); 

  zapzarn1w(tabn,data->prn,data->kod_nah_ud,0,0,0,shet,data->window);

  if(suma != 0.)
   {
    zarsocw(data->mp,data->gp,tabn,NULL,data->window);

    zaravprw(tabn,data->mp,data->gp,NULL,data->window);
   }  

 }
return(0);
}




