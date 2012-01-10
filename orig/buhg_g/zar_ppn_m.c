/*$Id: zar_ppn_m.c,v 1.16 2011-02-21 07:35:59 sasa Exp $*/
/*11.02.2010	30.11.2006	Белых А.И.	zar_ppn_m.c
Меню для перерасчёта подоходного налога
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "zar_ppn.h"
enum
 {
  E_MESN,
  E_MESK,
  E_GOD,
  E_TABNOM,
  E_PODR,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK5,
  FK10,
  KOL_FK
 };

class zar_ppn_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *spin_enter;
  GtkWidget *spin_enter_mesn;
  GtkWidget *spin_enter_mesk;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *label_nast;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class zar_ppn_rek *rk;
  class iceb_u_str stroka_nast;
    
  zar_ppn_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->mesn.new_plus(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_enter_mesn)));
    rk->mesk.new_plus(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_enter_mesk)));

    rk->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PODR]))));
    rk->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM]))));
    int god=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_enter));
    rk->god.new_plus(god);
   }
  
  void clear_rek()
   {

    rk->clear_data();

    for(int i=3; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

//    gtk_widget_grab_focus(entry[0]);

   }
 };


gboolean   zar_ppn_m_key_press(GtkWidget *widget,GdkEventKey *event,class zar_ppn_m_data *data);
void  zar_ppn_m_knopka(GtkWidget *widget,class zar_ppn_m_data *data);
void    zar_ppn_m_vvod(GtkWidget *widget,class zar_ppn_m_data *data);
void  zar_ppn_m_e_knopka(GtkWidget *widget,class zar_ppn_m_data *data);

int zar_ppn_read_nastr(class iceb_u_str *stroka_nast,GtkWidget *wpredok);

extern SQL_baza  bd;
extern char      *name_system;

extern float    prog_min_ng; //Прожиточный минимум на начало года
extern float    kof_prog_min; //Коэффициент прожиточного минимума
extern double minzar_ng; /*Минимальная зарплата*/		
extern float  pomzp; //Процент от минимальной зароботной платы
extern float  ppn; //Процент подоходного налога
extern float  procpen[2]; /*Процент отчисления в пенсионный фонд*/

extern float procsoc; /*Процент отчисления в соц-страх*/
extern float procsoc1; /*Процент отчисления в соц-страх*/
extern float procbez; /*Процент отчисления на безработицу*/

int zar_ppn_m(class zar_ppn_rek *rek,GtkWidget *wpredok)
{


class zar_ppn_m_data data;
char strsql[1024];
iceb_u_str kikz;
data.rk=rek;
 

if(zar_ppn_read_nastr(&data.stroka_nast,wpredok) != 0)
 return(1);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",name_system,gettext("Перерасчёт подоходного налога"));
label=gtk_label_new(gettext("Перерасчёт подоходного налога"));

data.label_nast=gtk_label_new(data.stroka_nast.ravno_toutf());



gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_ppn_m_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 1);
GtkWidget *separator1=gtk_hseparator_new();
gtk_box_pack_start (GTK_BOX (vbox),separator1, TRUE, TRUE, 2);

gtk_box_pack_start (GTK_BOX (vbox),data.label_nast, TRUE, TRUE, 1);
GtkWidget *separator2=gtk_hseparator_new();
gtk_box_pack_start (GTK_BOX (vbox),separator2, TRUE, TRUE, 2);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE, 1);



gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 1);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Месяц начала"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_MESN]), label, FALSE, FALSE, 0);

data.spin_enter_mesn=gtk_spin_button_new_with_range(1,12,1);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(data.spin_enter_mesn),(gfloat)data.rk->mesn.ravno_atof());
gtk_box_pack_start (GTK_BOX (hbox[E_MESN]),data.spin_enter_mesn,FALSE, FALSE, 0);


sprintf(strsql,"%s",gettext("Месяц конца"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_MESN]), label, FALSE, FALSE, 0);

data.spin_enter_mesk=gtk_spin_button_new_with_range(1,12,1);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(data.spin_enter_mesk),(gfloat)data.rk->mesk.ravno_atof());
gtk_box_pack_start (GTK_BOX (hbox[E_MESN]),data.spin_enter_mesk,FALSE, FALSE, 0);


sprintf(strsql,"%s",gettext("Год"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_MESN]), label, FALSE, FALSE, 0);

data.spin_enter=gtk_spin_button_new_with_range(data.rk->god.ravno_atoi()-1000,data.rk->god.ravno_atoi()+1000,1);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(data.spin_enter),(gfloat)data.rk->god.ravno_atof());
gtk_box_pack_start (GTK_BOX (hbox[E_MESN]),data.spin_enter,FALSE, FALSE, 0);


sprintf(strsql,"%s (,,)",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_TABNOM]),"clicked",GTK_SIGNAL_FUNC(zar_ppn_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_TABNOM]),(gpointer)E_TABNOM);
tooltips_enter[E_TABNOM]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_TABNOM],data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"),NULL);

data.entry[E_TABNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TABNOM]), "activate",GTK_SIGNAL_FUNC(zar_ppn_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.rk->tabnom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TABNOM]),(gpointer)E_TABNOM);


sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(zar_ppn_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подразделения"),NULL);

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(zar_ppn_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);




GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчёт"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(zar_ppn_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(zar_ppn_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F5 %s",gettext("Настройка"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Настройка расчёта"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]),"clicked",GTK_SIGNAL_FUNC(zar_ppn_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK5], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(zar_ppn_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

//gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  zar_ppn_m_e_knopka(GtkWidget *widget,class zar_ppn_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_MESN:
    iceb_calendar1(&data->rk->mesn,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_MESN]),data->rk->mesn.ravno_toutf());
    return;

  case E_MESK:
    iceb_calendar1(&data->rk->mesk,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_MESK]),data->rk->mesk.ravno_toutf());
    return;

  case E_TABNOM:
    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     data->rk->tabnom.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->rk->tabnom.ravno_toutf());
    return;
  
  case E_PODR:
    if(l_zarpodr(1,&kod,&naim,data->window) == 0)
     data->rk->podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno_toutf());
    return;
  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_ppn_m_key_press(GtkWidget *widget,GdkEventKey *event,class zar_ppn_m_data *data)
{
//char  bros[512];

//printf("zar_ppn_m_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

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
void  zar_ppn_m_knopka(GtkWidget *widget,class zar_ppn_m_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zar_ppn_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();

    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

    
  case FK4:
    data->clear_rek();
    return;

  case FK5:
    iceb_f_redfil("zarppn.alx",0,data->window);
    zar_ppn_read_nastr(&data->stroka_nast,data->window);
    gtk_label_set_text(GTK_LABEL(data->label_nast),data->stroka_nast.ravno_toutf());
    return;
      
  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    zar_ppn_m_vvod(GtkWidget *widget,class zar_ppn_m_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zar_ppn_m_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_TABNOM:
    data->rk->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_PODR:
    data->rk->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
 enter=3;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Чтение настроек для расчёта*/
/*****************************/

int zar_ppn_read_nastr(class iceb_u_str *stroka_nast,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zarppn.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }


char pole[1024];

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  iceb_u_polen(row_alx[0],pole,sizeof(pole),1,'|');
  if(iceb_u_SRAV(pole,"Коэффициент прожиточного минимума",0) == 0)
   {
    iceb_u_polen(row_alx[0],pole,sizeof(pole),2,'|');
    kof_prog_min = iceb_u_atof(pole);
    continue;
   }


  if(iceb_u_SRAV(pole,"Процент подоходного налога",0) == 0)
   {
    iceb_u_polen(row_alx[0],pole,sizeof(pole),2,'|');
    ppn = iceb_u_atof(pole);
    continue;
   }

  if(iceb_u_SRAV(pole,"Процент от минимальной заработной платы",0) == 0)
   {
    iceb_u_polen(row_alx[0],pole,sizeof(pole),2,'|');
    pomzp = iceb_u_atof(pole);
    continue;
   }
  if(iceb_u_SRAV(pole,"Процент отчисления в пенсионный фонд",0) == 0)
    {
     iceb_u_polen(row_alx[0],pole,sizeof(pole),2,'|');
     procpen[0]=(float)iceb_u_atof(pole);
     if(iceb_u_polen(row_alx[0],pole,sizeof(pole),3,'|') == 0)
      procpen[1]=(float)iceb_u_atof(pole);
     else
      procpen[1]=procpen[0];
     continue;
    }

  if(iceb_u_SRAV(pole,"Процент отчисления в соц-страх",0) == 0)
    {
     iceb_u_polen(row_alx[0],pole,sizeof(pole),2,'|');
     procsoc=(float)iceb_u_atof(pole);
     iceb_u_polen(row_alx[0],pole,sizeof(pole),3,'|');
     procsoc1=(float)iceb_u_atof(pole);
     continue;
    }

  if(iceb_u_SRAV(pole,"Процент отчисления в фонд безработицы",0) == 0)
    {
     iceb_u_polen(row_alx[0],pole,sizeof(pole),2,'|');
     procbez=(float)iceb_u_atof(pole);
     continue;
    }

 }
/*********
sprintf(strsql,"Прожиточный минимум на начало года:%.2f Коэффициент прожиточного минимума:%.2f\n\
Минимальная зарплата на начало года:%.2f Процент от минимальной зарплаты:%.2f\n\
Процент подоходного налога:%.2f",prog_min_ng,kof_prog_min,minzar_ng,pomzp,ppn);
*************/
sprintf(strsql,"Коэффициент прожиточного минимума:%.2f\n\
Процент от минимальной зарплаты:%.2f\n\
Процент подоходного налога:%.2f",kof_prog_min,pomzp,ppn);

stroka_nast->new_plus(strsql);

return(0);

}



