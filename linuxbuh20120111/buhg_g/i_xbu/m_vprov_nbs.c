/*$Id: m_vprov_nbs.c,v 1.19 2011-02-21 07:35:55 sasa Exp $*/
/*31.05.2009	05.01.2004	Белых А.И.	m_vprov_nbs.c
Ввод проводок для балансовых счетов
*/
#include  <math.h>
#include  <ctype.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DEBET,
  E_KREDIT,
  E_DATA,
  E_KOMENT,
  KOLENTER  
 };

class vprov_nbs_data
 {
  public:
  OPSHET     rekshet;
  iceb_u_str shet;    //Счет по которому выполняются проводки

  iceb_u_str debet;   //Сумма по дебету
  iceb_u_str kredit;  //Сумма по кредиту
  iceb_u_str data;    //Дата проводки
  iceb_u_str koment;  //Коментарий к проводке
  iceb_u_str kontr_sh; //Код контрагента для счета
  
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *label_prov;
  short kl_shift;
  
  void clear_zero()
   {
    debet.new_plus("");
    kredit.new_plus("");
    data.new_plus("");
    koment.new_plus("");
    kontr_sh.new_plus("");
   }

  void clear_zero1();

  //Конструктор
  vprov_nbs_data()
   {
    kl_shift=0;
    window=NULL;
    clear_zero();
   }


 };

gboolean   vprov_nbs_v_key_press(GtkWidget *widget,GdkEventKey *event,class vprov_nbs_data *data);
void  vprov_nbs_v_knopka(GtkWidget *widget,class vprov_nbs_data *data);
void    vprov_nbs_v_vvod(GtkWidget *widget,class vprov_nbs_data *data);
void    vprov_nbs_zp(class vprov_nbs_data *data);
void   vregp(GtkWidget*);

extern SQL_baza	bd;
extern char *name_system;

void  m_vprov_nbs(char *shet)
{
static char datpr[11];

char strsql[512];
vprov_nbs_data data;
iceb_u_str stroka;
time_t vrem;
struct tm *bf;

data.shet.new_plus(shet);
data.data.new_plus(datpr);
time(&vrem);
bf=localtime(&vrem);

iceb_prsh1(shet,&data.rekshet,NULL);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Ввод проводок"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(vprov_nbs_v_key_press),&data);

sprintf(strsql,"%s %s:%02d.%02d.%d",
gettext("Ввод проводок"),
gettext("Текущая дата"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);

stroka.new_plus(strsql);

sprintf(strsql,"%s %s %s",
gettext("Счет"),
shet,
data.rekshet.naim.ravno());

stroka.ps_plus(strsql);

sprintf(strsql,"%s: ",gettext("Тип счета"));
if(data.rekshet.tips == 0)
 strcat(strsql,gettext("Активный"));
if(data.rekshet.tips == 1)
 strcat(strsql,gettext("Пассивный"));
if(data.rekshet.tips == 2)
 strcat(strsql,gettext("Активно-пассивный"));

stroka.ps_plus(strsql);

sprintf(strsql,"%s: ",gettext("Вид счета"));
if(data.rekshet.vids == 0)
 strcat(strsql,gettext("Счет"));
if(data.rekshet.vids == 1)
 strcat(strsql,gettext("Субсчет"));
stroka.ps_plus(strsql);

sprintf(strsql,"%s: ",gettext("Сальдо"));
if(data.rekshet.saldo == 0)
 strcat(strsql,gettext("Свернутое"));
if(data.rekshet.saldo == 3)
 strcat(strsql,gettext("Развернутое"));
stroka.ps_plus(strsql);

sprintf(strsql,"%s: ",gettext("Статус"));
if(data.rekshet.stat == 0)
 strcat(strsql,gettext("Балансовый"));
if(data.rekshet.stat == 1)
 strcat(strsql,gettext("Внебалансовый"));
stroka.ps_plus(strsql);

GtkWidget *label=gtk_label_new(stroka.ravno_toutf());
data.label_prov=gtk_label_new("");
GtkWidget *separator1=gtk_hseparator_new();
GtkWidget *separator2=gtk_hseparator_new();

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox1 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox2 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox3 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox4 = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hbox4);
gtk_container_add (GTK_CONTAINER (vbox), separator1);
gtk_container_add (GTK_CONTAINER (vbox), data.label_prov);
gtk_container_add (GTK_CONTAINER (vbox), separator2);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Сумма по дебету"));
data.entry[E_DEBET] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_DEBET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DEBET]), "activate",GTK_SIGNAL_FUNC(vprov_nbs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DEBET]),data.debet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DEBET]),(gpointer)E_DEBET);

label=gtk_label_new(gettext("Сумма по кредиту"));
data.entry[E_KREDIT] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_KREDIT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KREDIT]), "activate",GTK_SIGNAL_FUNC(vprov_nbs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KREDIT]),data.kredit.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KREDIT]),(gpointer)E_KREDIT);

label=gtk_label_new(gettext("Дата проводки (д.м.г)"));
data.entry[E_DATA] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox3), data.entry[E_DATA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA]), "activate",GTK_SIGNAL_FUNC(vprov_nbs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.data.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA]),(gpointer)E_DATA);

label=gtk_label_new(gettext("Коментарий"));
data.entry[E_KOMENT] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(vprov_nbs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(vprov_nbs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);

sprintf(strsql,"F3 %s",gettext("План счетов"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Просмотр плана счетов"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(vprov_nbs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);

sprintf(strsql,"F4 %s",gettext("Просмотр проводок"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Просмотр и корректировка сделанных проводок"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(vprov_nbs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);

sprintf(strsql,"F5 %s",gettext("Блокировка"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Установка или снятие блокировки проводок"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]),"clicked",GTK_SIGNAL_FUNC(vprov_nbs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(vprov_nbs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK5], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

memset(datpr,'\0',sizeof(datpr));
strncpy(datpr,data.data.ravno(),sizeof(datpr)-1);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vprov_nbs_v_key_press(GtkWidget *widget,GdkEventKey *event,class vprov_nbs_data *data)
{
//char  bros[512];
iceb_u_str shet;
iceb_u_str naim;

//printf("vprov_nbs_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);

  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
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
void  vprov_nbs_v_knopka(GtkWidget *widget,class vprov_nbs_data *data)
{
//char bros[512];
iceb_u_str shet;
iceb_u_str naim;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    vprov_nbs_zp(data);
    return;  

  case FK3:
    iceb_l_plansh(1,&shet,&naim,data->window);
    return;  

  case FK4:
   vregp(data->window);
    return;  

  case FK5:
    iceb_f_redfil("blok.alx",0,data->window);
    return;  

  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    vprov_nbs_v_vvod(GtkWidget *widget,class vprov_nbs_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("vprov_nbs_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DEBET:
    data->debet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KREDIT:
    data->kredit.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DATA:
    data->data.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOMENT:
    data->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

if(enter != 0)
  enter+=1;
else
  enter+=2;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}

/*****************************************/
/*Запись проводки                        */
/*****************************************/
void    vprov_nbs_zp(class vprov_nbs_data *data)
{
short d,m,g;
OPSHET rekshet;
char strsql[512];

for(int i=0; i < KOLENTER; i++)
  gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");

if(iceb_u_rsdat(&d,&m,&g,data->data.ravno(),0) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не верно введена дата проводки !"));
  iceb_menu_soob(&repl,data->window);
  return;
 }
if(data->debet.ravno()[0] == '\0' && data->kredit.ravno()[0] == '\0')
 {
  iceb_u_str repl;
  repl.plus(gettext("Не введена сума !"));
  iceb_menu_soob(&repl,data->window);
  return;
 }

if(iceb_pvglkni(m,g,data->window) != 0)
 return;
 


double deb=iceb_u_atof(data->debet.ravno());
double kre=iceb_u_atof(data->kredit.ravno());
if(fabs(deb) < 0.01 && fabs(kre) < 0.01)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не введен ни дебет ни кредит !"));
  iceb_menu_soob(&repl,data->window);
  return;
 }

if(fabs(deb) > 0.009  && fabs(kre) > 0.009)
 {
  iceb_u_str repl;
  repl.plus(gettext("Введен и дебет и кредит !"));
  iceb_menu_soob(&repl,data->window);
  return;
 }


iceb_u_str kod;
iceb_u_str naim;
//SQL_str row;
short metka_c=0;

if(data->rekshet.saldo == 3) //Основной счет с развернутым сальдо
 {
  iceb_u_str repl;
  repl.plus(gettext("Введите код контрагента для счета"));
  repl.plus(" ");
  repl.plus(data->shet.ravno());

  while(metka_c == 0)
   {
  
    if(iceb_menu_vvod1(&repl,&kod,20,data->window) != 0)
      return;

    if(kod.ravno()[0] == '\0' || kod.ravno()[0] == '+') 
     {
      if(kod.ravno()[0] == '+')
        naim.new_plus(&kod.ravno()[1]);
      else
        naim.new_plus(kod.ravno());

      kod.new_plus("");
      
      if(iceb_l_kontrsh(1,data->shet.ravno(),&kod,&naim,data->window) != 0)
        continue;
      
     }  

    //Проверяем есть ли такой код контрагента
    sprintf(strsql,"select ns from Skontr where ns='%s' and kodkon='%s'",
    data->shet.ravno_filtr(),kod.ravno_filtr());
    if(sql_readkey(&bd,strsql) != 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не нашли код контрагента"));
      repl.plus(" ");
      repl.plus(kod.ravno());
      repl.plus(" ");
      repl.plus(gettext("в списке счета"));
      repl.plus(" ");
      repl.plus(data->shet.ravno());
      repl.plus(" !");

      iceb_menu_soob(&repl,data->window);
      continue;
     }

    data->kontr_sh.new_plus(kod.ravno());  
    break;
   } 
 }

time_t vrem;
time(&vrem);
int kekv=0;
iceb_zapprov(-1,g,m,d,data->shet.ravno_filtr(),"",
data->kontr_sh.ravno_filtr(),"","","","",deb,kre,data->koment.ravno_filtr(),1,0,vrem,0,0,0,0,kekv,data->window);


data->clear_zero1();


}
/*************************************/
/*Очистка меню после записи и формирование строки выполненной проводки*/
/***********************************************************************/
void vprov_nbs_data::clear_zero1()
{

char strsql[512];
iceb_u_str stroka;

stroka.plus(gettext("Последняя проводка"));
stroka.plus(":");
stroka.ps_plus(data.ravno());

stroka.plus(" ");
sprintf(strsql,"%.2f",iceb_u_atof(debet.ravno()));
stroka.plus(strsql);

stroka.plus(" ");
sprintf(strsql,"%.2f",iceb_u_atof(kredit.ravno()));
stroka.plus(strsql);


SQLCURSOR cur;
SQL_str row;

if(kontr_sh.ravno()[0] != '\0')
 {
  stroka.ps_plus(kontr_sh.ravno());
  stroka.plus(" ");
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr_sh.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) > 0)
    stroka.plus(row[0]);
 }


if(koment.ravno()[0] != '\0')
  stroka.ps_plus(koment.ravno());

gtk_label_set_text(GTK_LABEL(label_prov),stroka.ravno_toutf());
gtk_widget_show(label_prov);


//Очищаем

debet.new_plus("");
kredit.new_plus("");

koment.new_plus("");
kontr_sh.new_plus("");
gtk_entry_set_text(GTK_ENTRY(entry[E_DEBET]),debet.ravno_toutf());
gtk_entry_set_text(GTK_ENTRY(entry[E_KREDIT]),kredit.ravno_toutf());
gtk_entry_set_text(GTK_ENTRY(entry[E_DATA]),data.ravno_toutf());
gtk_entry_set_text(GTK_ENTRY(entry[E_KOMENT]),koment.ravno_toutf());

gtk_widget_grab_focus(entry[0]);

}
