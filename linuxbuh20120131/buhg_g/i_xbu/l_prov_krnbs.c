/*$Id: l_prov_krnbs.c,v 1.19 2011-01-13 13:49:50 sasa Exp $*/
/*18.11.2008	13.01.2004	Белых А.И.	l_prov_krnbs.c
Меню корректировки проводки
*/
#include  <stdlib.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include  "l_prov.h"

enum
{
  KFK2,
  KFK3,
  KFK10,
  KOL_KF_KL
};

enum
 {
  E_KSHET,
  E_KDATA,
  E_KKOMENT,
  E_KDEBET,
  E_KKREDIT,
  E_KKONTR,
  K_KOLENTER  
 };

class prov_krnbs_data
 {
  public:
  prov_rek_data *rek_prov_s;
  prov_rek_data rek_prov_n;

  GtkWidget *entry[K_KOLENTER];
  GtkWidget *knopka[KOL_KF_KL];
  GtkWidget *kwindow;
  short kl_shift;
  short metka_zap; //0-записали 1-не записали
  //Конструктор
  prov_krnbs_data()
   {
    metka_zap=0;
    kl_shift=0;
    kwindow=NULL;
   }

  void activate()
   {
    for(int i=0; i < K_KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }


 };

gboolean   prov_krnbs_key_press(GtkWidget *widget,GdkEventKey *event,class prov_krnbs_data *data);
void  prov_krnbs_knopka(GtkWidget *widget,class prov_krnbs_data *data);
void    prov_krnbs_vvod(GtkWidget *widget,class prov_krnbs_data *data);
int  prov_krnbs_zap(class prov_krnbs_data *data);

int l_prov_krnbs(class prov_data *wdata)
{
char strsql[512];
iceb_u_str stroka;
prov_krnbs_data data;

printf("l_prov_krnbs\n");

data.rek_prov_s=&wdata->rek_vibr;

data.kwindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.kwindow),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.kwindow),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Корректировать"));
gtk_window_set_title (GTK_WINDOW (data.kwindow),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.kwindow), 5);

gtk_signal_connect(GTK_OBJECT(data.kwindow),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.kwindow),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

if(wdata->window != NULL)
 {
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.kwindow),GTK_WINDOW(wdata->window));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.kwindow),TRUE);
 }

gtk_signal_connect_after(GTK_OBJECT(data.kwindow),"key_press_event",GTK_SIGNAL_FUNC(prov_krnbs_key_press),&data);

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox1 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox3 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox4 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox5 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox6 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox7 = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

stroka.new_plus(gettext("Корректировка проводки"));
stroka.ps_plus(gettext("Дата и время записи"));
stroka.plus(" ");
stroka.plus(iceb_u_vremzap(data.rek_prov_s->vremz));
stroka.plus(" ");
stroka.plus(gettext("Записал"));
stroka.plus(" ");
stroka.plus(iceb_kszap(data.rek_prov_s->ktoz,1,NULL));

GtkWidget *label=gtk_label_new(stroka.ravno_toutf());

gtk_container_add (GTK_CONTAINER (data.kwindow), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hbox4);
gtk_container_add (GTK_CONTAINER (vbox), hbox5);
gtk_container_add (GTK_CONTAINER (vbox), hbox6);
gtk_container_add (GTK_CONTAINER (vbox), hbox7);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

label=gtk_label_new(gettext("Счет"));
data.entry[E_KSHET] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_KSHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KSHET]), "activate",GTK_SIGNAL_FUNC(prov_krnbs_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KSHET]),data.rek_prov_s->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KSHET]),(gpointer)E_KSHET);

label=gtk_label_new(gettext("Дата проводки (д.м.г)"));
data.entry[E_KDATA] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox3), data.entry[E_KDATA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KDATA]), "activate",GTK_SIGNAL_FUNC(prov_krnbs_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KDATA]),data.rek_prov_s->datap.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KDATA]),(gpointer)E_KDATA);

label=gtk_label_new(gettext("Коментарий"));
data.entry[E_KKOMENT] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), data.entry[E_KKOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KKOMENT]), "activate",GTK_SIGNAL_FUNC(prov_krnbs_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KKOMENT]),data.rek_prov_s->koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KKOMENT]),(gpointer)E_KKOMENT);

label=gtk_label_new(gettext("Дебет"));
data.entry[E_KDEBET] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox5), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox5), data.entry[E_KDEBET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KDEBET]), "activate",GTK_SIGNAL_FUNC(prov_krnbs_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KDEBET]),data.rek_prov_s->debet.ravno_filtr());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KDEBET]),(gpointer)E_KDEBET);


label=gtk_label_new(gettext("Кредит"));
data.entry[E_KKREDIT] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox6), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox6), data.entry[E_KKREDIT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KKREDIT]), "activate",GTK_SIGNAL_FUNC(prov_krnbs_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KKREDIT]),data.rek_prov_s->kredit.ravno_filtr());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KKREDIT]),(gpointer)E_KKREDIT);

label=gtk_label_new(gettext("Код контрагента"));
data.entry[E_KKONTR] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox7), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox7), data.entry[E_KKONTR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KKONTR]), "activate",GTK_SIGNAL_FUNC(prov_krnbs_vvod),&data);

sprintf(strsql,"%s",data.rek_prov_s->kontr.ravno_toutf());

if(data.rek_prov_s->kontrk.ravno()[0] != '\0')
 if(iceb_u_SRAV(data.rek_prov_s->kontr.ravno(),data.rek_prov_s->kontrk.ravno(),0) != 0)
  sprintf(strsql,"%s/%s",data.rek_prov_s->kontr.ravno_toutf(),data.rek_prov_s->kontrk.ravno_toutf());

gtk_entry_set_text(GTK_ENTRY(data.entry[E_KKONTR]),strsql);
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KKONTR]),(gpointer)E_KKONTR);



GtkTooltips *tooltips[KOL_KF_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[KFK2]=gtk_button_new_with_label(strsql);
tooltips[KFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[KFK2],data.knopka[KFK2],gettext("Запись введенной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[KFK2]),"clicked",GTK_SIGNAL_FUNC(prov_krnbs_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[KFK2]),(gpointer)KFK2);

sprintf(strsql,"F3 %s",gettext("Контрагенты"));
data.knopka[KFK3]=gtk_button_new_with_label(strsql);
tooltips[KFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[KFK3],data.knopka[KFK3],gettext("Просмотр общего списка контрагентов"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[KFK3]),"clicked",GTK_SIGNAL_FUNC(prov_krnbs_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[KFK3]),(gpointer)KFK3);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[KFK10]=gtk_button_new_with_label(strsql);
tooltips[KFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[KFK10],data.knopka[KFK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[KFK10]),"clicked",GTK_SIGNAL_FUNC(prov_krnbs_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[KFK10]),(gpointer)KFK10);


gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[KFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[KFK3], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[KFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.kwindow);

gtk_main();

return(data.metka_zap);
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   prov_krnbs_key_press(GtkWidget *widget,GdkEventKey *event,class prov_krnbs_data *data)
{
iceb_u_str kod;
iceb_u_str naim;

//printf("prov_krnbs_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[KFK2]),"clicked");
    return(TRUE);

  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[KFK3]),"clicked");
    return(TRUE);


  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[KFK10]),"clicked");
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
void  prov_krnbs_knopka(GtkWidget *widget,class prov_krnbs_data *data)
{
iceb_u_str kod;
iceb_u_str naim;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case KFK2:
    data->activate();
    prov_krnbs_zap(data);
    return;  

  case KFK3:
    iceb_l_kontr(1,&kod,&naim,data->kwindow);
    return;  


  case KFK10:
    gtk_widget_destroy(data->kwindow);
    data->kwindow=NULL;
    data->metka_zap=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    prov_krnbs_vvod(GtkWidget *widget,class prov_krnbs_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("prov_krnbs_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KSHET:
    data->rek_prov_n.shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KKONTR:
    data->rek_prov_n.kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KKOMENT:
    data->rek_prov_n.koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KDEBET:
    data->rek_prov_n.debet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KKREDIT:
    data->rek_prov_n.kredit.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KDATA:
    data->rek_prov_n.datap.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }

enter+=1;
  
if(enter >= K_KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/*********************************/
/*Запись исправленной проводки   */
/*********************************/
//Если вернули 0-записали 1-не записали
int  prov_krnbs_zap(class prov_krnbs_data *data)
{
printf("prov_krnbs_zap\n");

char strsql[512];
short d,m,g;  //Дата старая
short d1,m1,g1;  //Дата новая

if(data->rek_prov_n.shet.ravno()[0] == '\0')
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("Не введен счет !"));
  iceb_menu_soob(&SOOB,data->kwindow);
  return(1);  
 }

if(iceb_u_rsdat(&d1,&m1,&g1,data->rek_prov_n.datap.ravno(),0) != 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("Не верно ведена дата проводки !"));
  iceb_menu_soob(&SOOB,data->kwindow);
  return(1);  
 }

if(atof(data->rek_prov_n.debet.ravno()) != 0. && atof(data->rek_prov_n.kredit.ravno()) != 0.)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("Введено дебет и кредит !"));
  iceb_menu_soob(&SOOB,data->kwindow);
  return(1);  
 }


//Проверяем старую дату
iceb_u_rsdat(&d,&m,&g,data->rek_prov_s->datap.ravno(),1);
if(iceb_pvglkni(m,g,data->kwindow) != 0)
 return(1);

//Проверяем новую дату
if(iceb_pvglkni(m1,g1,data->kwindow) != 0)
 return(1);

if(provvkp(data->rek_prov_s->metka.ravno(),data->kwindow) != 0)
  return(1);  

char kontr[20];
char kontr1[20];
//SQL_str row;

memset(kontr,'\0',sizeof(kontr));
memset(kontr1,'\0',sizeof(kontr1));
if(data->rek_prov_n.kontr.ravno()[0] != '\0')
 {
  if(iceb_u_pole(data->rek_prov_n.kontr.ravno(),kontr,1,'/') != 0)
   {
    strcpy(kontr,data->rek_prov_n.kontr.ravno());
    strcpy(kontr1,data->rek_prov_n.kontr.ravno());
   }
  else
   iceb_u_pole(data->rek_prov_n.kontr.ravno(),kontr1,2,'/');
 }
data->rek_prov_n.kontr.new_plus(kontr);
data->rek_prov_n.kontrk.new_plus(kontr1);

/*Проверяем есть ли счет в списке счетов*/
OPSHET rek_shet;
if(iceb_prsh1(data->rek_prov_n.shet.ravno(),&rek_shet,data->kwindow) != 0)
  return(1);

if(rek_shet.saldo == 3 && kontr[0] == '\0')
 {
  iceb_u_str SOOB;
  sprintf(strsql,"%s %s !",gettext("Не введен контрагент для счета"),data->rek_prov_n.shet.ravno());
  SOOB.plus(strsql);
  iceb_menu_soob(&SOOB,data->kwindow);
  return(1);
 }


/*Проверяем есть ли такой код контрагента*/
if(rek_shet.saldo == 3)
if(kontr[0] != '\0')
 {
  sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",data->rek_prov_n.shet.ravno(),kontr);
  if(sql_readkey(&bd,strsql) != 1)
   {
    iceb_u_str SOOB;
    sprintf(strsql,"%s %s %s %s!",
    gettext("Не нашли код контрагента"),
    kontr,
    gettext("в списке счета"),
    data->rek_prov_n.shet.ravno());
    
    SOOB.plus(strsql);
    iceb_menu_soob(&SOOB,data->kwindow);
    return(1);
   } 
 }


if(rek_shet.saldo == 3 && kontr[0] == '\0')
 {  
  iceb_u_str SOOB;
  SOOB.plus(gettext("Не введён код контрагента !"));
  iceb_menu_soob(&SOOB,data->kwindow);
  return(1);
 }

if(rek_shet.saldo < 3  && kontr[0] != '\0')
 {  
  iceb_u_str SOOB;
  SOOB.plus(gettext("Счет имеет свернутое сальдо ! Код контрагента не вводится !"));
  iceb_menu_soob(&SOOB,data->kwindow);
  return(1);
 }

if(iceb_udprov(data->rek_prov_s->val,g,m,d,
data->rek_prov_s->shet.ravno_filtr(),
data->rek_prov_s->shetk.ravno_filtr(),
data->rek_prov_s->vremz,
atof(data->rek_prov_s->debet.ravno()),
atof(data->rek_prov_s->kredit.ravno()),
data->rek_prov_s->koment.ravno_filtr(),
2,data->kwindow) != 0)
   return(1);

time_t vrem;
time(&vrem);
int kekv=0;
iceb_zapprov(-1,g1,m1,d1,data->rek_prov_n.shet.ravno(),
data->rek_prov_n.shetk.ravno_filtr(),
data->rek_prov_n.kontr.ravno_filtr(),
data->rek_prov_n.kontrk.ravno_filtr(),
"","","",atof(data->rek_prov_n.debet.ravno()),atof(data->rek_prov_n.kredit.ravno()),
data->rek_prov_n.koment.ravno_filtr(),1,0,vrem,0,0,0,
0,
kekv,
data->kwindow);

gtk_widget_destroy(data->kwindow);
data->kwindow=NULL;
return(0);
}
