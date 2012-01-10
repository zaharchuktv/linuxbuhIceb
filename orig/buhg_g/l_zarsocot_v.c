/*$Id: l_zarsocot_v.c,v 1.12 2011-02-21 07:35:55 sasa Exp $*/
/*14.09.2006	13.09.2006	Белых А.И.	l_zarsocot_v.c
Ввод и корректировка соц-отчислений
*/

#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "buhg_g.h"

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOD,
  E_SHET,
  E_SUMA_O,
  E_SOCOT_O,
  E_SUMA_B,
  E_SOCOT_B,
  E_PROC,
  KOLENTER  
 };

class l_zarsocot_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  
  int tabnom;
  short mp,gp;
  int kodzap; //Код записи которую корректируют
  
  //реквизиты меню
  class iceb_u_str kod;
  class iceb_u_str shet;
  class iceb_u_str suma_o;
  class iceb_u_str socot_o;
  class iceb_u_str suma_b;
  class iceb_u_str socot_b;
  class iceb_u_str proc;
  
  l_zarsocot_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_rek();
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
  void clear_rek()
   {
    kod.new_plus("");
    shet.new_plus("");
    suma_o.new_plus("");
    socot_o.new_plus("");
    suma_b.new_plus("");
    socot_b.new_plus("");
    proc.new_plus("");
    
   }
 };

gboolean   l_zarsocot_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsocot_v_data *data);
void    l_zarsocot_v_v_vvod(GtkWidget *widget,class l_zarsocot_v_data *data);
void  l_zarsocot_v_v_knopka(GtkWidget *widget,class l_zarsocot_v_data *data);
int zarsocot_zap(class l_zarsocot_v_data *data);

void  zarsocot_v_e_knopka(GtkWidget *widget,class l_zarsocot_v_data *data);
int l_zarsocot_v_zap(class l_zarsocot_v_data *data);

extern char *name_system;
extern SQL_baza bd;


int l_zarsocot_v(int tabnom,short mp,short gp,class iceb_u_str *kodzap,
const char *fio,
GtkWidget *wpredok)
{
class iceb_u_str kikz;
char strsql[512];
class l_zarsocot_v_data data;
data.tabnom=tabnom;
data.mp=mp;
data.gp=gp;
data.kodzap=kodzap->ravno_atoi();

if(kodzap->ravno_atoi() != 0)
 {
  SQL_str row;
  class SQLCURSOR cur;
  sprintf(strsql,"select * from Zarsocz where datz='%04d-%02d-01' and tabn=%d and kodz=%d",
  data.gp,data.mp,data.tabnom,data.kodzap);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  
  data.kod.new_plus(row[2]);
  data.shet.new_plus(row[3]);
  data.suma_o.new_plus(row[4]);
  data.socot_o.new_plus(row[5]);
  data.suma_b.new_plus(row[10]);
  data.socot_b.new_plus(row[9]);
  data.proc.new_plus(row[6]);
  kikz.plus(iceb_kikz(row[7],row[8],wpredok));
  
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(kodzap->ravno_atoi() != 0)
 sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
else
 sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_zarsocot_v_v_key_press),&data);

iceb_u_str zagolov;
if(kodzap->ravno_atoi() == 0)
 zagolov.new_plus(gettext("Ввод новой записи"));
else
 {
  zagolov.new_plus(gettext("Корректировка записи"));
  zagolov.ps_plus(kikz.ravno());
 }
zagolov.ps_plus(data.tabnom);
zagolov.plus(" ");
zagolov.plus(fio);

GtkWidget *label=gtk_label_new(zagolov.ravno_toutf());

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);


gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

//sprintf(strsql,"%s",gettext("Код"));
//label=gtk_label_new(iceb_u_toutf(strsql));
//gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);

sprintf(strsql,"%s",gettext("Код"));
data.knopka_enter[E_KOD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.knopka_enter[E_KOD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD]),"clicked",GTK_SIGNAL_FUNC(zarsocot_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD]),(gpointer)E_KOD);
tooltips_enter[E_KOD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD],data.knopka_enter[E_KOD],gettext("Выбор кода в списке соц-отчислений"),NULL);

data.entry[E_KOD] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(l_zarsocot_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(zarsocot_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(l_zarsocot_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

sprintf(strsql,"%s",gettext("С какой суммы начислено"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_SUMA_O] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_O]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_O]), data.entry[E_SUMA_O],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA_O]), "activate",GTK_SIGNAL_FUNC(l_zarsocot_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA_O]),data.suma_o.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA_O]),(gpointer)E_SUMA_O);

sprintf(strsql,"%s",gettext("Общая сумма соц-отчисления"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_SOCOT_O] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SOCOT_O]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SOCOT_O]), data.entry[E_SOCOT_O],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SOCOT_O]), "activate",GTK_SIGNAL_FUNC(l_zarsocot_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SOCOT_O]),data.socot_o.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SOCOT_O]),(gpointer)E_SOCOT_O);

sprintf(strsql,"%s",gettext("С какой суммы начислено бюджет"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_SUMA_B] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_B]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_B]), data.entry[E_SUMA_B],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA_B]), "activate",GTK_SIGNAL_FUNC(l_zarsocot_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA_B]),data.suma_b.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA_B]),(gpointer)E_SUMA_B);

sprintf(strsql,"%s",gettext("Сумма бюджетного соц-отчисления"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_SOCOT_B] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SOCOT_B]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SOCOT_B]), data.entry[E_SOCOT_B],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SOCOT_B]), "activate",GTK_SIGNAL_FUNC(l_zarsocot_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SOCOT_B]),data.socot_b.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SOCOT_B]),(gpointer)E_SOCOT_B);

sprintf(strsql,"%s",gettext("Процент"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_PROC] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_PROC]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_PROC]), data.entry[E_PROC],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PROC]), "activate",GTK_SIGNAL_FUNC(l_zarsocot_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROC]),data.proc.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PROC]),(gpointer)E_PROC);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_zarsocot_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_zarsocot_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 kodzap->new_plus(data.kod.ravno());

return(data.voz);
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  zarsocot_v_e_knopka(GtkWidget *widget,class l_zarsocot_v_data *data)
{
iceb_u_str kod(0);
iceb_u_str naim(0);

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {

  case E_KOD:
    if(l_zarsocf(1,&kod,&naim,data->window) == 0)
     data->kod.new_plus(kod.ravno());

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->kod.ravno_toutf());

    return;
  case E_SHET:
    iceb_vibrek(1,"Plansh",&data->shet,&naim,data->window);
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno_toutf());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zarsocot_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsocot_v_data *data)
{

//printf("l_zarsocot_v_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
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
void  l_zarsocot_v_v_knopka(GtkWidget *widget,class l_zarsocot_v_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(l_zarsocot_v_zap(data) != 0)
     return;

    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_zarsocot_v_v_vvod(GtkWidget *widget,class l_zarsocot_v_data *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_zarsocot_v_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SUMA_O:
    data->suma_o.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_SUMA_B:
    data->suma_b.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_SOCOT_O:
    data->socot_o.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_SOCOT_B:
    data->socot_b.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_PROC:
    data->proc.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


  case E_SHET:
    data->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/***********************************/
/*Запись*/
/****************************/

int l_zarsocot_v_zap(class l_zarsocot_v_data *data)
{


if(data->kod.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }
char strsql[512];
 

 //Проверяем есть ли такой код 
sprintf(strsql,"select kod from Zarsoc where kod=%s",data->kod.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
{
 sprintf(strsql,"%s %s !",gettext("Не найден код соц. страхования"),data->kod.ravno());
 iceb_menu_soob(strsql,data->window);
 return(1);
}

 //Проверяем есть ли такой счет
struct OPSHET shetv;
if(iceb_prsh1(data->shet.ravno(),&shetv,data->window) != 0)
 return(1);

time_t vrem;  
time(&vrem);
 
if(data->kodzap == 0)
  sprintf(strsql,"insert into Zarsocz values (datz,tabn,kodz,shet,sumas,sumap,proc,ktoz,vrem,sumapb,sumasb) \
('%d-%d-01',%d,%d,'%s',%.2f,%.2f,%.2f,%d,%ld,%.2f,%.2f)",
  data->gp,data->mp,data->tabnom,data->kod.ravno_atoi(),data->shet.ravno(),data->suma_o.ravno_atof(),data->socot_o.ravno_atof(),
  data->proc.ravno_atof(),iceb_getuid(data->window),vrem,data->suma_b.ravno_atof(),
  data->socot_b.ravno_atof());

if(data->kodzap != 0)
   sprintf(strsql,"update Zarsocz set \
shet='%s',\
sumas=%.2f,\
sumap=%.2f,\
proc=%.2f,\
ktoz=%d,\
vrem=%ld,\
sumapb=%.2f,\
sumasb=%.2f \
where datz='%04d-%d-01' and tabn=%d and kodz=%d",
   data->shet.ravno(),data->suma_o.ravno_atof(),data->socot_o.ravno_atof(),
  data->proc.ravno_atof(),iceb_getuid(data->window),vrem,data->suma_b.ravno_atof(),
  data->socot_b.ravno_atof(),
  data->gp,data->mp,data->tabnom,data->kodzap);

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
  return(1);

return(0);
}





