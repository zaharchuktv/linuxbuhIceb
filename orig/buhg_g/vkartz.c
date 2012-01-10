/*$Id: vkartz.c,v 1.38 2011-04-14 16:09:36 sasa Exp $*/
/*22.02.2010	29.06.2006	Белых А.И.	vkartz.c
Ввод и корректировка карточки работника в подсистеме "Заработная плата"
Если вернули 0- вышли по F10
             1- это значит записали новую карточку или удалили существующюю
*/
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include "buhg_g.h"
#include "vkartz.h"
enum
 {
  E_TABNOM,
  E_FIO,
  E_DOLGN,
  E_ZVANIE,
  E_PODR,
  E_KATEG,
  E_DATA_PNR,
  E_DATA_USR,
  E_SHET,
  E_KOD_LG,
  KOLENTER  
 };

enum
 {
  FK2,
  FK3,
  FK4,
  FK5,
  SFK5,
  FK6,
  SFK6,
  FK7,
  FK8,
  SFK8,
  FK9,
  SFK9,
  PAGE_UP,
  PAGE_DOWN,
  FK10,
  FK11,
  KOL_FK
 };


class vkartz_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_tek_dat;
  GtkWidget *label_sovm;
  GtkWidget *label_oklad;
  GtkWidget *label_tabel;
  GtkWidget *label_niz;
  GtkWidget *label_blok_kart;
  
  short kl_shift;
  short voz;      //0-ввели 1 нет

  
  class vkartz_vs rk; //реквизиты карточки
  class iceb_u_str tabel; //Текст с перечнем видов табеля 
  class iceb_u_str niz; //Текст внизу карточки
  
  short           mettab;/*Метка табеля*/
  double okld; //Оклад
  int mhas; //метка часовой ставки
  short		prov; //0-проводки не сделаны 1-сделаны
  
  //Табельный номер для просмотра
  class iceb_u_str tabnom_p;
  class iceb_u_str sfio; //Фамилия до корректировки
  class iceb_u_str shet_old; //Старий счёт 
  class iceb_u_str podr_old; //Старое подразделение    
  double nah,uder,sald;
  vkartz_data() //Конструктор
   {
    nah=uder=sald=0.;
    kl_shift=0;
    voz=0;
    window=NULL;    
    mettab=0;
    okld=0.;
    mhas=0;
    prov=0;
    podr_old.plus("");
    shet_old.plus("");
    sfio.plus("");
   }

  void read_rek()
   {

    rk.tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM]))));
    rk.fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_FIO]))));
    rk.dolgn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DOLGN]))));
    rk.data_pnr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_PNR]))));
    rk.data_usr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_USR]))));
    if(rk.data_usr.getdlinna() > 1)
     rk.uvol=1;
    else
     rk.uvol=0;

    rk.kod_lg.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_LG]))));
    rk.zvanie.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ZVANIE]))));

    rk.podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PODR]))));

    rk.kateg.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KATEG]))));
    rk.shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
   }
      
  void set_all_text()
   {
    gtk_entry_set_text(GTK_ENTRY( entry[E_TABNOM]), rk.tabnom.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY( entry[E_FIO]), rk.fio.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY( entry[E_DOLGN]), rk.dolgn.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY( entry[E_ZVANIE]), rk.zvanie.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY( entry[E_PODR]), rk.podr.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY( entry[E_KATEG]), rk.kateg.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY( entry[E_DATA_PNR]), rk.data_pnr.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY( entry[E_DATA_USR]), rk.data_usr.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY( entry[E_SHET]), rk.shet.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY( entry[E_KOD_LG]), rk.kod_lg.ravno_toutf());
  }
 };


gboolean   vkartz_key_press(GtkWidget *widget,GdkEventKey *event,class vkartz_data *data);
void  vkartz_knopka(GtkWidget *widget,class vkartz_data *data);
void    vkartz_vvod(GtkWidget *widget,class vkartz_data *data);
void  vkartz_v_e_knopka(GtkWidget *widget,class vkartz_data *data);

int vkartz_read_kart(const char *tabnom,class vkartz_data *data);
void vkartz_refresh(class vkartz_data *data);
void zapkrtbw(int stbn,class vkartz_vs *data,GtkWidget *wpredok);
int vkartz_prov_zap_rek(class vkartz_data *data,int);
int vkartz_prov_nu(class vkartz_data *data);
void vkartz_zapzarn(class vkartz_data *data);
void poiflw(int tbn,GtkWidget *wpredok);
void vkartz_smena_sheta(class vkartz_data *data);
void vkartz1(class vkartz_vs *rk,GtkWidget *wpredok);
GtkWidget *vkartz2(class vkartz_vs *rk,GtkWidget *wpredok);
void l_nahud_t(int prn,class vkartz_vs *rek_kart,GtkWidget *wpredok);
void vkartz_ras(class vkartz_data *data);
void koreshw(int ttt,short mp,short gp,const char *fio,const char *inn,const char *dolgn,int podr,int kateg,GtkWidget *wpredok);
void raszkartw(int tabn,GtkWidget *wpredok);
void l_prov_zar(int tabnom,short md,short gd,GtkWidget *wpredok);
void l_otrvr_t(int tabnom,short mp,short gp,GtkWidget *wpredok);
void l_zarsocot(int tabnom,short mp,short gp,GtkWidget *wpredok);

extern SQL_baza  bd;
extern char      *name_system;
extern short mmm,ggg;
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern iceb_u_str shrift_ravnohir;
extern char *imabaz;
extern char *organ;

int vkartz(class iceb_u_str *tabnom_p,GtkWidget *wpredok)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,1,wpredok);

class vkartz_data data;
char strsql[512];
iceb_u_str kikz;
data.tabnom_p.new_plus(tabnom_p->ravno());
data.rk.mp=mmm;
data.rk.gp=ggg;
class iceb_sql_flag frsk; //флаг работы с карточкой

iceb_refresh();
if(data.tabnom_p.getdlinna() >  1)
 {
  if(vkartz_read_kart(data.tabnom_p.ravno(),&data) != 0)
   return(1);

  data.okld=okladw(data.rk.tabnom.ravno_atoi(),mmm,ggg,&data.mhas,wpredok); 
  //Установить флаг работы с карточкой
  sprintf(strsql,"z%s%d",imabaz,data.tabnom_p.ravno_atoi());

  if(frsk.flag_on(strsql) != 0)
   {
    gdite.close(); //закрываем окно ждите
    data.rk.flagrk=1;
    iceb_menu_soob(gettext("С карточкой работает другой оператор !\nВаши изменения записаны не будут !"),wpredok);
   }
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Kartb","tabn",0,wpredok));
  data.rk.tabnom.new_plus(strsql);
 } 
iceb_refresh();

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),5);

GtkWidget *label=NULL;

if(data.tabnom_p.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой карточки работника"));
  label=gtk_label_new(gettext("Ввод новой карточки работника"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Просмотр карточки работника"));
  iceb_u_str repl;
  repl.plus(gettext("Просмотр карточки работника"));
//  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno_toutf());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(vkartz_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *hbox_main;
hbox_main = gtk_hbox_new (FALSE, 2);
gtk_container_add (GTK_CONTAINER (data.window), hbox_main);


GtkWidget *vbox_knop = gtk_vbox_new (FALSE, 2);

GtkWidget *vbox_menu_st1 = gtk_vbox_new (FALSE, 2);


GtkWidget *hbox_zagol=gtk_hbox_new (FALSE, 2);

GtkWidget *hbox_menu[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox_menu[i] = gtk_hbox_new (FALSE, 1);


sprintf(strsql,"%s:",gettext("Табель"));
data.label_tabel=gtk_label_new(iceb_u_toutf(strsql));

gtk_box_pack_start (GTK_BOX (hbox_main), vbox_knop, FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (hbox_main), vbox_menu_st1, TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox_menu_st1), label, FALSE,FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox_menu_st1), hbox_zagol, FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (hbox_main), data.label_tabel, FALSE, FALSE, 1);


sprintf(strsql,"%s:%02d.%d",gettext("Дата просмотра"),data.rk.mp,data.rk.gp);

data.label_tek_dat=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox_zagol),data.label_tek_dat, FALSE, FALSE, 1);

memset(strsql,'\0',sizeof(strsql));
if(data.okld != 0.)
 {
  sprintf(strsql,"%s:%.2f",gettext("Оклад"),data.okld);
  if(data.mhas == 1)
   strcat(strsql,"Ч");
 }
data.label_oklad=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox_zagol),data.label_oklad, FALSE, FALSE, 1);

if(data.rk.sovm == 1)
  data.label_sovm=gtk_label_new(gettext("Совместитель"));
else
  data.label_sovm=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox_zagol),data.label_sovm, FALSE, FALSE, 1);


gtk_box_pack_start (GTK_BOX (vbox_menu_st1), hbox_menu[0], FALSE, FALSE, 1);

label=gtk_label_new(gettext("Фамилия Имя Отчество"));
gtk_box_pack_start (GTK_BOX (vbox_menu_st1), label, FALSE, FALSE, 1);


for(int i=1; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox_menu_st1), hbox_menu[i], FALSE, FALSE, 1);

data.label_niz=gtk_label_new("");
PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_modify_font(GTK_WIDGET(data.label_niz),font_pango);
gtk_widget_modify_font(GTK_WIDGET(data.label_tabel),font_pango);
pango_font_description_free(font_pango);

gtk_box_pack_start (GTK_BOX (vbox_menu_st1), data.label_niz, FALSE, FALSE, 1);

GdkColor color;
data.label_blok_kart=gtk_label_new("");
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_blok_kart,GTK_STATE_NORMAL,&color);
gtk_box_pack_start (GTK_BOX (vbox_menu_st1), data.label_blok_kart, FALSE, FALSE, 1);


GtkTooltips *tooltips_enter[KOLENTER];

label=gtk_label_new(gettext("Табельный номер"));
data.entry[E_TABNOM] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_TABNOM]), label, FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TABNOM]), "activate",GTK_SIGNAL_FUNC(vkartz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.rk.tabnom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TABNOM]),(gpointer)E_TABNOM);


data.entry[E_FIO] = gtk_entry_new_with_max_length (60);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FIO]), "activate",GTK_SIGNAL_FUNC(vkartz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.rk.fio.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FIO]),(gpointer)E_FIO);

label=gtk_label_new(gettext("Должность"));
data.entry[E_DOLGN] = gtk_entry_new_with_max_length (60);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_DOLGN]), label, FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_DOLGN]), data.entry[E_DOLGN], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DOLGN]), "activate",GTK_SIGNAL_FUNC(vkartz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOLGN]),data.rk.dolgn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DOLGN]),(gpointer)E_DOLGN);


sprintf(strsql,"%s",gettext("Звание"));
data.knopka_enter[E_ZVANIE]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_ZVANIE]), data.knopka_enter[E_ZVANIE], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_ZVANIE]),"clicked",GTK_SIGNAL_FUNC(vkartz_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_ZVANIE]),(gpointer)E_ZVANIE);
tooltips_enter[E_ZVANIE]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_ZVANIE],data.knopka_enter[E_ZVANIE],gettext("Выбор звания"),NULL);

data.entry[E_ZVANIE] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox_menu[E_ZVANIE]), data.entry[E_ZVANIE], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ZVANIE]), "activate",GTK_SIGNAL_FUNC(vkartz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZVANIE]),data.rk.zvanie.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ZVANIE]),(gpointer)E_ZVANIE);

sprintf(strsql,"%s",gettext("Подразделение"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(vkartz_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подразделения"),NULL);

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox_menu[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(vkartz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk.podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);

sprintf(strsql,"%s",gettext("Категория"));
data.knopka_enter[E_KATEG]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_KATEG]), data.knopka_enter[E_KATEG], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KATEG]),"clicked",GTK_SIGNAL_FUNC(vkartz_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KATEG]),(gpointer)E_KATEG);
tooltips_enter[E_KATEG]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KATEG],data.knopka_enter[E_KATEG],gettext("Выбор категории"),NULL);

data.entry[E_KATEG] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox_menu[E_KATEG]), data.entry[E_KATEG], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KATEG]), "activate",GTK_SIGNAL_FUNC(vkartz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KATEG]),data.rk.kateg.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KATEG]),(gpointer)E_KATEG);


sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(vkartz_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox_menu[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(vkartz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk.shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);





sprintf(strsql,"%s",gettext("Дата прийома"));
data.knopka_enter[E_DATA_PNR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_DATA_PNR]), data.knopka_enter[E_DATA_PNR], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_PNR]),"clicked",GTK_SIGNAL_FUNC(vkartz_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_PNR]),(gpointer)E_DATA_PNR);
tooltips_enter[E_DATA_PNR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_PNR],data.knopka_enter[E_DATA_PNR],gettext("Выбор даты"),NULL);

data.entry[E_DATA_PNR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_DATA_PNR]), data.entry[E_DATA_PNR], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_PNR]), "activate",GTK_SIGNAL_FUNC(vkartz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_PNR]),data.rk.data_pnr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_PNR]),(gpointer)E_DATA_PNR);

sprintf(strsql,"%s",gettext("Дата увольнения"));
data.knopka_enter[E_DATA_USR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_DATA_USR]), data.knopka_enter[E_DATA_USR], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_USR]),"clicked",GTK_SIGNAL_FUNC(vkartz_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_USR]),(gpointer)E_DATA_USR);
tooltips_enter[E_DATA_USR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_USR],data.knopka_enter[E_DATA_USR],gettext("Выбор даты"),NULL);

data.entry[E_DATA_USR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_DATA_USR]), data.entry[E_DATA_USR], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_USR]), "activate",GTK_SIGNAL_FUNC(vkartz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_USR]),data.rk.data_usr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_USR]),(gpointer)E_DATA_USR);

label=gtk_label_new(gettext("Код льготы"));
data.entry[E_KOD_LG] = gtk_entry_new_with_max_length (60);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_KOD_LG]), label, FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_KOD_LG]), data.entry[E_KOD_LG], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_LG]), "activate",GTK_SIGNAL_FUNC(vkartz_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_LG]),data.rk.kod_lg.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_LG]),(gpointer)E_KOD_LG);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Начисления"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Работа с начислениями"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK2], TRUE, TRUE, 1);

sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить карточку работника"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK3], TRUE, TRUE, 1);

sprintf(strsql,"F4 %s",gettext("Удержания"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Работа с удержаниями"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK4], TRUE, TRUE, 1);


sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Печать нужной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK5], TRUE, TRUE, 1);

sprintf(strsql,"%sF5 %s",RFK,gettext("Проводки"));
data.knopka[SFK5]=gtk_button_new_with_label(strsql);
tooltips[SFK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK5],data.knopka[SFK5],gettext("Работа с проводками"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK5]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK5]),(gpointer)SFK5);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[SFK5], TRUE, TRUE, 1);

sprintf(strsql,"F6 %s",gettext("Табель"));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Ввод и корректировка отработанного времени"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK6], TRUE, TRUE, 1);

sprintf(strsql,"%sF6 %s",RFK,gettext("Соц.фонды"));
data.knopka[SFK6]=gtk_button_new_with_label(strsql);
tooltips[SFK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK6],data.knopka[SFK6],gettext("Работа с начислениями на фонд оплаты труда"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK6]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK6]),(gpointer)SFK6);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[SFK6], TRUE, TRUE, 1);

sprintf(strsql,"F7 %s",gettext("Совместитель"));
data.knopka[FK7]=gtk_button_new_with_label(strsql);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Устанавливать/снимать метку совместителя"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK7], TRUE, TRUE, 1);

sprintf(strsql,"F8 %s",gettext("НСИ"));
data.knopka[FK8]=gtk_button_new_with_label(strsql);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Ввод и корректировка нормативно-справочной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK8], TRUE, TRUE, 1);

sprintf(strsql,"%sF8 %s",RFK,gettext("Блокировка"));
data.knopka[SFK8]=gtk_button_new_with_label(strsql);
tooltips[SFK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK8],data.knopka[SFK8],gettext("Установка/снятие блокировки карточки на текущий месяц"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK8]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK8]),(gpointer)SFK8);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[SFK8], TRUE, TRUE, 1);

sprintf(strsql,"F9 %s",gettext("Настройка"));
data.knopka[FK9]=gtk_button_new_with_label(strsql);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Просмотр настроек на табельный номер"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK9], TRUE, TRUE, 1);

sprintf(strsql,"%sF9 %s",RFK,gettext("Дополнение"));
data.knopka[SFK9]=gtk_button_new_with_label(strsql);
tooltips[SFK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK9],data.knopka[SFK9],gettext("Работа с дополнительной информацией"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK9]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK9]),(gpointer)SFK9);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[SFK9], TRUE, TRUE, 1);

sprintf(strsql,"Page Up %s",gettext("Следующая"));
data.knopka[PAGE_UP]=gtk_button_new_with_label(strsql);
tooltips[PAGE_UP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PAGE_UP],data.knopka[PAGE_UP],gettext("Увеличить дату просмотра карточки"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PAGE_UP]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PAGE_UP]),(gpointer)PAGE_UP);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[PAGE_UP], TRUE, TRUE, 1);

sprintf(strsql,"Page Down %s",gettext("Пред."));
data.knopka[PAGE_DOWN]=gtk_button_new_with_label(strsql);
tooltips[PAGE_DOWN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PAGE_DOWN],data.knopka[PAGE_DOWN],gettext("Уменьшить дату просмотра карточки"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PAGE_DOWN]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PAGE_DOWN]),(gpointer)PAGE_DOWN);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[PAGE_DOWN], TRUE, TRUE, 1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK10], TRUE, TRUE, 1);

sprintf(strsql,"FK11 %s",gettext("Переход"));
data.knopka[FK11]=gtk_button_new_with_label(strsql);
tooltips[FK11]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK11],data.knopka[FK11],gettext("Переход на вторую страницу карточки работника"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK11]),"clicked",GTK_SIGNAL_FUNC(vkartz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK11]),(gpointer)FK11);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK11], TRUE, TRUE, 1);

//gtk_widget_grab_focus(data.entry[0]);
gtk_widget_grab_focus(data.knopka[FK10]);

vkartz_refresh(&data);

gtk_widget_show_all (data.window);

if(data.tabnom_p.getdlinna() >  1)
  gtk_widget_set_sensitive(GTK_WIDGET(hbox_menu[E_TABNOM]),FALSE);//Недоступна

gdite.close(); //закрываем окно ждите

gtk_main();

if(data.voz == 0)
 tabnom_p->new_plus(data.rk.tabnom.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vkartz_v_e_knopka(GtkWidget *widget,class vkartz_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");


int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("vkartz_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_ZVANIE:
    if(l_zarzvan(1,&kod,&naim,data->window) == 0)
     {
      data->rk.zvanie.new_plus(kod.ravno());
      data->rk.zvanie.plus(" ");
      data->rk.zvanie.plus(naim.ravno());
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_ZVANIE]),data->rk.zvanie.ravno_toutf());
    return;

  case E_PODR:
    if(l_zarpodr(1,&kod,&naim,data->window) == 0)
     {
      data->rk.podr.new_plus(kod.ravno());
      data->rk.podr.plus(" ");
      data->rk.podr.plus(naim.ravno());
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk.podr.ravno_toutf());
    return;

  case E_KATEG:
    if(l_zarkateg(1,&kod,&naim,data->window) == 0)
     {
      data->rk.kateg.new_plus(kod.ravno());
      data->rk.kateg.plus(" ");
      data->rk.kateg.plus(naim.ravno());
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KATEG]),data->rk.kateg.ravno_toutf());
    return;

  case E_SHET:

    iceb_vibrek(1,"Plansh",&data->rk.shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk.shet.ravno_toutf());

    return;  

  case E_DATA_PNR:

    if(iceb_calendar(&data->rk.data_pnr,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_PNR]),data->rk.data_pnr.ravno());
      
    return;  
  
  case E_DATA_USR:

    if(iceb_calendar(&data->rk.data_usr,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_USR]),data->rk.data_usr.ravno());
      
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vkartz_key_press(GtkWidget *widget,GdkEventKey *event,class vkartz_data *data)
{
//char  bros[512];

//printf("vkartz_key_press\n");
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
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK5]),"clicked");
    return(TRUE);

  case GDK_F6:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK6]),"clicked");
    return(TRUE);

  case GDK_F7:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
    return(TRUE);

  case GDK_F8:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK8]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK8]),"clicked");
    return(TRUE);

  case GDK_F9:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK9]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK9]),"clicked");
    return(TRUE);

  case GDK_Page_Up:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PAGE_UP]),"clicked");
    return(TRUE);

  case GDK_Page_Down:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PAGE_DOWN]),"clicked");
    return(TRUE);

  case GDK_F11:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK11]),"clicked");
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
void  vkartz_knopka(GtkWidget *widget,class vkartz_data *data)
{
class iceb_gdite_data gdite;
class iceb_u_str repl;
char strsql[512];
short dd=0;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("vkartz_knopka knop=%d\n",knop);

data->read_rek();
vkartz_zapzarn(data); //Записываем в таблицу настроек по датам

switch (knop)
 {
  case FK11:  //Работа со второй страницей карточки
    vkartz1(&data->rk,data->window);
    return;  

  case PAGE_UP:  //Увеличить дату
    printf("Увеличиваем дату.\n");
    iceb_gdite(&gdite,1,data->window);    
    iceb_u_dpm(&dd,&data->rk.mp,&data->rk.gp,3);

    vkartz_read_kart(data->rk.tabnom.ravno(),data); //Читаем настройки в карточке и промежуточной таблице

    vkartz_refresh(data); //Выводим реквизиты меню, считаем сальдо
    gdite.close();
    return;  

  case PAGE_DOWN:  //Уменьшить дату
    printf("Уменьшаем дату.\n");
    iceb_gdite(&gdite,1,data->window);    
    iceb_u_dpm(&dd,&data->rk.mp,&data->rk.gp,4);

    vkartz_read_kart(data->rk.tabnom.ravno(),data); //Читаем настройки в карточке и промежуточной таблице

    vkartz_refresh(data); //Выводим реквизиты меню, считаем сальдо
    gdite.close();
    return;  
  
  case FK2:  //Работа с начислениями
    if(vkartz_prov_nu(data) != 0)
     return;

    if(vkartz_prov_zap_rek(data,0) != 0) //Проверки на возможность записи внутри подпрограммы
      return;
    data->tabnom_p.new_plus(data->rk.tabnom.ravno());
    data->sfio.new_plus(data->rk.fio.ravno());

    l_nahud_t(1,&data->rk,data->window);

    if(data->rk.mtd == 0 && data->rk.blok == 0 && data->rk.flagrk == 0)
     {
      zarsocw(data->rk.mp,data->rk.gp,data->rk.tabnom.ravno_atoi(),NULL,data->window);
      zaravprw(data->rk.tabnom.ravno_atoi(),data->rk.mp,data->rk.gp,NULL,data->window);
     }
      
    vkartz_refresh(data); //Выводим реквизиты меню, считаем сальдо
    return;  

  case FK3: //Удаление карточки 
    if(zarudkarw(data->rk.tabnom.ravno_atoi(),data->window) == 0)
     {
      gtk_widget_destroy(data->window);
      data->voz=1;
     }
    return;  

  case FK4: //Работа с удержаниями
    if(vkartz_prov_nu(data) != 0)
     return;

    if(vkartz_prov_zap_rek(data,0) != 0) //Проверки на возможность записи внутри подпрограммы
     return;
    data->tabnom_p.new_plus(data->rk.tabnom.ravno());
    data->sfio.new_plus(data->rk.fio.ravno());

    l_nahud_t(2,&data->rk,data->window);

    if(data->rk.mtd == 0 && data->rk.blok == 0 && data->rk.flagrk == 0)
     {
      zarsocw(data->rk.mp,data->rk.gp,data->rk.tabnom.ravno_atoi(),NULL,data->window);
      zaravprw(data->rk.tabnom.ravno_atoi(),data->rk.mp,data->rk.gp,NULL,data->window);
     }
      
    vkartz_refresh(data); //Выводим реквизиты меню, считаем сальдо
    return;  

  case FK5: //Распечатка
    vkartz_ras(data);
    return;


  case SFK5: //Работа с проводками
    l_prov_zar(data->rk.tabnom.ravno_atoi(),data->rk.mp,data->rk.gp,data->window);
    vkartz_refresh(data); //Выводим реквизиты меню, считаем сальдо
    return;  

  case FK6: //Ввод и корректировка отработанного времени
    l_otrvr_t(data->rk.tabnom.ravno_atoi(),data->rk.mp,data->rk.gp,data->window);
    vkartz_refresh(data); //Выводим реквизиты меню, считаем сальдо
    return;  

  case SFK6: //Работа с соц-отчислениями
    l_zarsocot(data->rk.tabnom.ravno_atoi(),data->rk.mp,data->rk.gp,data->window);
    return;    

  case FK7: //Установка/снятие пометки - совместитель

    if(provblokzarpw(data->rk.tabnom.ravno_atoi(),data->rk.mp,data->rk.gp,data->rk.flagrk,0,data->window) != 0) //dirzarp.c
        return;

    if(data->rk.sovm == 0)
      repl.new_plus(gettext("Пометить как совместителя ?"));
    if(data->rk.sovm == 1)
      repl.new_plus(gettext("Снять отметку совместителя ?"));

    if(iceb_menu_danet(&repl,2,data->window) == 2)
     return;
    data->rk.sovm++;
    if(data->rk.sovm == 2)
      data->rk.sovm=0;

    vkartz_refresh(data); //Выводим реквизиты меню, считаем сальдо
    return;  


  case FK8: //Работа с НСИ
    v_nsi(data->window);  
    return;  

  case SFK8: //Установка/снятие блокировки карточки
    
    if(data->rk.blok == 0)
      repl.new_plus(gettext("Заблокировать карточку ? Вы уверены ?"));
    if(data->rk.blok != 0 )
      repl.new_plus(gettext("Разблокировать карточку ? Вы уверены ?"));

    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return;
    
    if(data->rk.blok != 0 && data->rk.blok != iceb_getuid(data->window))
     {
      struct  passwd  *ktoz; /*Кто записал*/
      class iceb_u_spisok text_menu;
      if((ktoz=getpwuid(data->rk.blok)) != NULL)
         sprintf(strsql,"%s %d %.*s",gettext("Карточка заблокирована"),
         data->rk.blok,
         iceb_u_kolbait(20,ktoz->pw_gecos),ktoz->pw_gecos);
      else
         sprintf(strsql,gettext("Заблокировано неизвестным логином %d !"),data->rk.blok);
      text_menu.plus(strsql);

      if(iceb_parol(0,&text_menu,data->window) != 0)
        return;
      
     }

    if(data->rk.blok == 0)
     data->rk.blok=iceb_getuid(data->window);
    else
     data->rk.blok=0;

     sprintf(strsql,"update Zarn set blok=%d \
where tabn=%d and god=%d and mes=%d",data->rk.blok,data->rk.tabnom.ravno_atoi(),data->rk.gp,data->rk.mp);

    if(sql_zap(&bd,strsql) != 0)
       iceb_msql_error(&bd,"Ошибка ввода записи !",strsql,data->window);

    vkartz_refresh(data); //Выводим реквизиты меню, считаем сальдо

    return;  

  case FK9: //Посмотреть все настройки на этот табельный номер
    poiflw(data->rk.tabnom.ravno_atoi(),data->window);
    return;  

  
  case SFK9: //Ввод текстовой иформации по данному работнику

      if(data->rk.flagrk != 0)
       {
        iceb_menu_soob(gettext("С карточкой работает другой оператор !"),data->window);
        return;
       }

      sprintf(strsql,"tabn%d.tmp",getpid());
      iceb_dikont('-',strsql,data->rk.tabnom.ravno(),"Kartb1");

      iceb_vizred(strsql,data->window);

      iceb_dikont('+',strsql,data->rk.tabnom.ravno(),"Kartb1");
      unlink(strsql);

      return;

  case FK10:
    if(vkartz_prov_zap_rek(data,1) != 0)
     return;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    vkartz_vvod(GtkWidget *widget,class vkartz_data *data)
{
if(provblokzarpw(data->rk.tabnom.ravno_atoi(),data->rk.mp,data->rk.gp,data->rk.flagrk,0,data->window) != 0)
 {
  data->set_all_text();
  return;
 }

char strsql[512];
SQL_str row;
SQLCURSOR cur;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("vkartz_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_TABNOM:

    data->rk.tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    if(data->tabnom_p.ravno_atoi() != data->rk.tabnom.ravno_atoi())
     {
       iceb_menu_soob(gettext("Корректировка табельного номера невозможна !"),data->window);
       data->rk.tabnom.new_plus(data->tabnom_p.ravno());
       gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->rk.tabnom.ravno_toutf());
     }
    break;

  case E_FIO:
    data->rk.fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    if(iceb_u_SRAV(data->sfio.ravno(),data->rk.fio.ravno(),0) != 0)
     {
      sprintf(strsql,"select tabn from Kartb where fio='%s'",data->rk.fio.ravno_filtr());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) >= 1)
       {
        sprintf(strsql,"%s:%s",gettext("Такая фамилия уже есть. Таб. номер"),row[0]);
        iceb_menu_soob(strsql,data->window);
       }
      
     }

    break;

  case E_DOLGN:
    data->rk.dolgn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATA_PNR:
    data->rk.data_pnr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATA_USR:
    data->rk.data_usr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->rk.data_usr.getdlinna() > 1)
     data->rk.uvol=1;
    else
     data->rk.uvol=0;
     
    break;

  case E_KOD_LG:
    data->rk.kod_lg.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_ZVANIE:
    data->rk.zvanie.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select naik from Zvan where kod=%d",data->rk.zvanie.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%d %s",data->rk.zvanie.ravno_atoi(),row[0]);
      data->rk.zvanie.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_ZVANIE]),data->rk.zvanie.ravno_toutf());
     }         
    break;

  case E_PODR:
    data->rk.podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select naik from Podr where kod=%d",data->rk.podr.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%d %s",data->rk.podr.ravno_atoi(),row[0]);
      data->rk.podr.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk.podr.ravno_toutf());
     }         
    break;

  case E_KATEG:
    data->rk.kateg.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select naik from Kateg where kod=%d",data->rk.kateg.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%d %s",data->rk.kateg.ravno_atoi(),row[0]);
      data->rk.kateg.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KATEG]),data->rk.kateg.ravno_toutf());
     }         
    break;

  case E_SHET:
    data->rk.shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }


enter+=1;
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/******************************/
/*Чтение реквизитов карточки*/
/******************************/

int vkartz_read_kart(const char *tabnom,class vkartz_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select * from Kartb where tabn=%d",atoi(tabnom));
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден табельный номер"),tabnom);
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
data->rk.tabnom.new_plus(row[0]);
data->rk.fio.new_plus(row[1]);
data->sfio.new_plus(row[1]);

data->rk.dolgn.new_plus(row[2]);
data->rk.zvanie.new_plus(row[3]);

data->rk.podr.new_plus(row[4]);
data->podr_old.new_plus(row[4]);

data->rk.kateg.new_plus(row[5]);

data->rk.data_pnr.new_plus(iceb_u_datzap(row[6]));
data->rk.data_usr.new_plus(iceb_u_datzap(row[7]));
data->rk.uvol=0;
if(data->rk.data_usr.getdlinna() > 1)
 data->rk.uvol=1;

data->rk.shet.new_plus(row[8]);
data->shet_old.new_plus(row[8]);
data->rk.sovm=atoi(row[9]);
data->rk.in.new_plus(row[10]);
data->rk.adres.new_plus(row[11]);
data->rk.nomer_pasp.new_plus(row[12]);
data->rk.kem_vidan.new_plus(row[13]);
data->rk.telefon.new_plus(row[14]);
data->rk.gorod_ni.new_plus(row[15]);
data->rk.har_rab.new_plus(row[16]);
data->rk.kod_lg.new_plus(row[17]);
data->rk.kart_shet.new_plus(row[18]);
data->rk.data_vp.new_plus(iceb_u_datzap(row[19]));
data->rk.tip_kk.new_plus(row[20]);
data->rk.pol=atoi(row[22]);
data->rk.data_rog.new_plus(iceb_u_datzap(row[21]));
data->rk.pol=atoi(row[22]);
data->rk.data_ppz.new_plus(iceb_u_datzap(row[23]));
data->rk.kodbank=atoi(row[24]);
data->rk.kodss.new_plus(row[25]);
data->rk.blok=0;

/*Читаем текущие настройки если они есть*/
sprintf(strsql,"select podr,kateg,sovm,zvan,shet,lgot,\
datn,datk,dolg,blok from Zarn where god=%d and mes=%d and tabn=%d",
data->rk.gp,data->rk.mp,atoi(tabnom));

if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  data->rk.zvanie.new_plus(row[3]);
  data->rk.podr.new_plus(row[0]);
  data->rk.kateg.new_plus(row[1]);
  data->rk.sovm=atoi(row[2]);
  data->rk.shet.new_plus(row[4]);
  data->rk.kod_lg.new_plus(row[5]);

  data->rk.data_pnr.new_plus(iceb_u_datzap(row[6]));
  data->rk.data_usr.new_plus(iceb_u_datzap(row[7]));
  data->rk.uvol=0;
  if(data->rk.data_usr.getdlinna() > 1)
   data->rk.uvol=1;

  data->rk.dolgn.new_plus(row[8]);
  data->rk.blok=atoi(row[9]);
  
 }    

//Читаем звание
sprintf(strsql,"select naik from Zvan where kod=%d",data->rk.zvanie.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  sprintf(strsql,"%d %s",data->rk.zvanie.ravno_atoi(),row[0]);
  data->rk.zvanie.new_plus(strsql);
 }
else
 data->rk.zvanie.new_plus("");

//Читаем подразделение
sprintf(strsql,"select naik from Podr where kod=%d",data->rk.podr.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  sprintf(strsql,"%d %s",data->rk.podr.ravno_atoi(),row[0]);
  data->rk.podr.new_plus(strsql);
 }
else
 data->rk.podr.new_plus("");

//Читаем категорию
sprintf(strsql,"select naik from Kateg where kod=%d",data->rk.kateg.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  sprintf(strsql,"%d %s",data->rk.kateg.ravno_atoi(),row[0]);
  data->rk.kateg.new_plus(strsql);
 }
else
 data->rk.kateg.new_plus("");


return(0);
}

/*************************************/
/*Отображение прочитанных реквизитов*/
/*************************************/

void vkartz_refresh(class vkartz_data *data)
{
char strsql[512];

data->set_all_text(); //прописываем в меню все реквизиты

SQL_str row;
class SQLCURSOR cur;

if(iceb_pblok(data->rk.mp,data->rk.gp,ICEB_PS_ZP,data->window) == 0)
  data->rk.mtd=0; //Дата не заблокирована
else
  data->rk.mtd=1; //Дата заблокирована

int kolstr=0;

sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where god=%d and mes=%d and tabn=%d",
data->rk.gp,data->rk.mp,data->rk.tabnom.ravno_atoi());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

sprintf(strsql,"%s:\n",gettext("Табель"));

data->tabel.new_plus(strsql);
data->mettab=0;
float		dnei=0.,has=0.,kdn=0.;

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  data->mettab=1;
  sprintf(strsql,"%s=%.6g/%.6g/%.6g\n",row[0],atof(row[1]),atof(row[2]),atof(row[3]));
  data->tabel.plus(strsql);
  dnei+=atof(row[1]);
  has+=atof(row[2]);
  kdn+=atof(row[3]);
 }
data->tabel.plus("--------------\n");

sprintf(strsql,"S=%.6g/%.6g/%.6g",dnei,has,kdn);
data->tabel.plus(strsql);





gtk_label_set_text(GTK_LABEL(data->label_tabel),data->tabel.ravno_toutf());

sprintf(strsql,"%s:%02d.%d",gettext("Дата просмотра"),data->rk.mp,data->rk.gp);
gtk_label_set_text(GTK_LABEL(data->label_tek_dat),iceb_u_toutf(strsql));

memset(strsql,'\0',sizeof(strsql));
if(data->okld != 0.)
 {
  sprintf(strsql,"%s:%.2f",gettext("Оклад"),data->okld);
  if(data->mhas == 1)
   strcat(strsql,"Ч");
 }
gtk_label_set_text(GTK_LABEL(data->label_oklad),iceb_u_toutf(strsql));

if(data->rk.sovm == 1)
  gtk_label_set_text(GTK_LABEL(data->label_sovm),gettext("Совместитель"));
else
  gtk_label_set_text(GTK_LABEL(data->label_sovm),"");





double saldb=0.;
data->sald=zarsaldw(1,data->rk.mp,data->rk.gp,data->rk.tabnom.ravno_atoi(),&saldb,data->window);
data->nah=0.;
double nahprov=0.;
data->uder=0.;
double uderprov=0.;
double nahb=0.;
double uderb=0.;
double uderbprov=0.;

nahudw(data->rk.gp,data->rk.mp,data->rk.tabnom.ravno_atoi(),&data->nah,&nahprov,&data->uder,&uderprov,&nahb,&uderb,&uderbprov,0,data->sald,0,
data->rk.fio.ravno(),data->window);

memset(strsql,'\0',sizeof(strsql));

double sumd=0.,sumk=0.;
double bb=0.;
if(data->tabnom_p.getdlinna() > 1)
 {
  sprintf(strsql,"%d-%d",data->rk.mp,data->rk.tabnom.ravno_atoi());

  bb=sumprzw(data->rk.mp,data->rk.gp,strsql,0,&sumd,&sumk,data->window);

 }


data->prov=0;
if(shetbu == NULL)
 { 
  
  sprintf(strsql,"%-*s %15s\n",iceb_u_kolbait(10,gettext("Сальдо")),gettext("Сальдо"),iceb_u_prnbr(data->sald));
  data->niz.new_plus(strsql);
  
  sprintf(strsql,"%-*s %15s",iceb_u_kolbait(10,gettext("Начислено")),gettext("Начислено"),iceb_u_prnbr(data->nah));
  data->niz.plus(strsql);
  
  sprintf(strsql,"%*s:%7.2f\n",iceb_u_kolbait(30,gettext("Сумма по дебету")),gettext("Сумма по дебету"),sumk);
  data->niz.plus(strsql);

  sprintf(strsql,"%-*s %15s",iceb_u_kolbait(10,gettext("Удержано")),gettext("Удержано"),iceb_u_prnbr(data->uder));
  data->niz.plus(strsql);
  
  sprintf(strsql,"%*s:%7.2f\n",iceb_u_kolbait(30,gettext("Сумма по кредиту")),gettext("Сумма по кредиту"),sumd);
  data->niz.plus(strsql);
  
  bb=data->nah+data->uder+data->sald;

  sprintf(strsql,"%-*s %15s",iceb_u_kolbait(10,gettext("Сальдо")),gettext("Сальдо"),iceb_u_prnbr(bb));
  data->niz.plus(strsql);

  if(fabs((sumd+sumk)-(uderprov*(-1)+nahprov)) <= 0.009 )
   data->prov=1;

  sprintf(strsql,"%*s:%7.2f",iceb_u_kolbait(30,gettext("Сумма выполненных проводок")),gettext("Сумма выполненных проводок"),sumd+sumk);
  data->niz.plus(strsql);
 }

if(shetbu != NULL)
 {
  sprintf(strsql,"%12s %s\n","",gettext("Всего   Бюджет  Хозрасчёт"));
  data->niz.new_plus(strsql);
  
  sprintf(strsql,"%-*s%8.2f %8.2f %8.2f\n",iceb_u_kolbait(10,gettext("Сальдо")),gettext("Сальдо"),data->sald,saldb,data->sald-saldb);
  data->niz.plus(strsql);
  
  sprintf(strsql,"%-*s%8.2f %8.2f %8.2f",iceb_u_kolbait(10,gettext("Начислено")),gettext("Начислено"),data->nah,nahb,data->nah-nahb);
  data->niz.plus(strsql);
  sprintf(strsql,"%*s:%7.2f\n",iceb_u_kolbait(30,gettext("Сумма по дебету")),gettext("Сумма по дебету"),sumk);
  data->niz.plus(strsql);

  sprintf(strsql,"%-*s%8.2f %8.2f %8.2f",iceb_u_kolbait(10,gettext("Удержано")),gettext("Удержано"),data->uder,uderb,data->uder-uderb);
  data->niz.plus(strsql);

  sprintf(strsql,"%*s:%7.2f\n",iceb_u_kolbait(30,gettext("Сумма по кредиту")),gettext("Сумма по кредиту"),sumd);
  data->niz.plus(strsql);

  bb=data->nah+data->uder+data->sald;

  sprintf(strsql,"%-*s%8.2f %8.2f %8.2f",iceb_u_kolbait(10,gettext("Сальдо")),gettext("Сальдо"),bb,nahb+uderb+saldb,(data->nah-nahb)+(data->uder-uderb)+(data->sald-saldb));
  data->niz.plus(strsql);

  if(fabs((sumd+sumk)-(uderprov*(-1)+nahprov)) <= 0.009 )
   data->prov=1;

  sprintf(strsql,"%*s:%7.2f",iceb_u_kolbait(30,gettext("Сумма выполненных проводок")),gettext("Сумма выполненных проводок"),sumd+sumk);
  data->niz.plus(strsql);


 }

vkartz_zapzarn(data);

gtk_label_set_text(GTK_LABEL(data->label_niz),data->niz.ravno_toutf());


if(data->rk.blok != 0)
 {
  struct  passwd  *ktoz; /*Кто записал*/
  char prom[200];
  if((ktoz=getpwuid(data->rk.blok)) != NULL)
    sprintf(prom,"%d %s",data->rk.blok,ktoz->pw_gecos);
  else
    sprintf(prom,"%s %d !",gettext("Неизвестный логин"),data->rk.blok);

  sprintf(strsql,"%s ! %.*s",gettext("Карточка заблокирована"),iceb_u_kolbait(20,prom),prom);

  gtk_label_set_text(GTK_LABEL(data->label_blok_kart),iceb_u_toutf(strsql));
 }
else
  gtk_label_set_text(GTK_LABEL(data->label_blok_kart),"");





}


/**************************************/
/*проверка всех реквизитов на правильность ввода и запись карточки*/
/**********************************/

int vkartz_prov_zap_rek(class vkartz_data *data,int metka_f10)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;


//Если заблокирована значит записывать не надо
if(provblokzarpw(data->rk.tabnom.ravno_atoi(),data->rk.mp,data->rk.gp,data->rk.flagrk,1,data->window) != 0)
 return(0);
int metka_oh=0; 
if(data->rk.fio.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена фамилия !"),data->window);
  metka_oh++;
 }

if(data->rk.tabnom.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён табельный номер !"),data->window);
  metka_oh++;
 }

if(metka_f10 == 0 && metka_oh != 0)
 return(1);

if(metka_f10 == 1 && metka_oh != 0)
 {
  iceb_menu_soob(gettext("Карточка не записывается !"),data->window);
  return(0);
 }

if(data->tabnom_p.getdlinna() <= 1)
 {
  sprintf(strsql,"select fio from Kartb where tabn=%d",data->rk.tabnom.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   {
    sprintf(strsql,"%s\n%s",gettext("Такой табельный номер уже есть !"),row[0]);    
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }
else
 if(data->tabnom_p.ravno_atoi() != data->rk.tabnom.ravno_atoi())
  {
   iceb_menu_soob(gettext("Корректировка табельного номера невозможна !"),data->window);
   data->rk.tabnom.new_plus(data->tabnom_p.ravno());
   gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->rk.tabnom.ravno_toutf());
   return(1);
  }


if(data->rk.zvanie.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Zvan where kod=%d",data->rk.zvanie.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код звания"),data->rk.zvanie.ravno_atoi());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }

if(data->rk.data_pnr.getdlinna() > 1)
  if(data->rk.data_pnr.prov_dat() != 0)
   {
    iceb_menu_soob(gettext("Неправильно введена дата прийома на работу !"),data->window);
    return(1);
   }
if(data->rk.data_usr.getdlinna() > 1)
  if(data->rk.data_usr.prov_dat() != 0)
   {
    iceb_menu_soob(gettext("Неправильно введена дата увольнения с работы !"),data->window);
    return(1);
   }


if(data->rk.podr.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Podr where kod=%d",data->rk.podr.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),data->rk.podr.ravno_atoi());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }

if(data->rk.podr.ravno_atoi() != data->podr_old.ravno_atoi())
 {
  //Исправляем код подразделения в начислениях/удержаниях
  sprintf(strsql,"update Zarp set podr=%d \
where datz >= '%04d-%d-1' and datz <= '%04d-%d-31' and tabn=%d and podr=%d",
   data->rk.podr.ravno_atoi(),data->rk.gp,data->rk.mp,data->rk.gp,data->rk.mp,data->rk.tabnom.ravno_atoi(),data->podr_old.ravno_atoi());

   iceb_sql_zapis(strsql,1,0,data->window);
 
 }

if(data->rk.kateg.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Kateg where kod=%d",data->rk.kateg.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код категории"),data->rk.kateg.ravno_atoi());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }

OPSHET rek_shet;
if(data->rk.shet.getdlinna() > 1)
 if(iceb_prsh1(data->rk.shet.ravno(),&rek_shet,data->window) != 0)
  return(1);

vkartz_smena_sheta(data); //Проверяем-если изменён счёт меняем счёт в начислениях

zapkrtbw(data->tabnom_p.ravno_atoi(),&data->rk,data->window);
if(data->tabnom_p.getdlinna() <= 1)
 data->voz=1;
else 
 data->voz=0;
return(0);
}
/*****************************/
/*Проверка реквизитов карточки перед входом в список начислений или удержаний*/
/*****************************************************************************/
int vkartz_prov_nu(class vkartz_data *data)
{

if(data->rk.uvol == 1)
 iceb_menu_soob(gettext("Внимание !!! Работник уволен !!!"),data->window);


if(data->rk.fio.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена фамилия !"),data->window);
  return(1);
 }

if(data->rk.tabnom.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён табельный номер !"),data->window);
  return(1);
 }

if(data->rk.podr.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён код подразделения !"),data->window);
  return(1);
 }
if(data->rk.kateg.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена категория !"),data->window);
  return(1);
 }

return(0);
}

/***********************************/
/*Запись в таблицу настроек по дате*/
/***********************************/

void vkartz_zapzarn(class vkartz_data *data)
{
if(data->tabnom_p.getdlinna() > 1 && data->rk.podr.ravno_atoi() != 0 && data->rk.kateg.ravno_atoi() != 0 && data->rk.blok == 0)
 {
  if(data->nah != 0 || data->uder != 0. || data->sald != 0. || data->mettab == 1)
   {
    short dn=0,mn=0,gn=0;
    short du=0,mu=0,gu=0;
    iceb_u_rsdat(&dn,&mn,&gn,data->rk.data_pnr.ravno(),1);
    iceb_u_rsdat(&du,&mu,&gu,data->rk.data_usr.ravno(),1);
    
    if(data->rk.mtd == 0 && data->rk.blok == 0 && data->rk.flagrk == 0)
      zapzarnw(data->rk.mp,data->rk.gp,data->rk.podr.ravno_atoi(),data->rk.tabnom.ravno_atoi(),data->rk.kateg.ravno_atoi(),
      data->rk.sovm,data->rk.zvanie.ravno_atoi(),data->rk.shet.ravno(),data->rk.kod_lg.ravno(),dn,mn,gn,du,mu,gu,
      data->prov,data->rk.dolgn.ravno(),data->window);
   }
 }
}
/***********************/
/*Корректировка счетов в начислениях после изменения в карточке*/
/********************************************/
void vkartz_smena_sheta(class vkartz_data *data)
{

if(iceb_u_SRAV(data->shet_old.ravno(),data->rk.shet.ravno(),0) == 0)
 return;

char strsql[512];
//Меняем номер счета в начислениях
sprintf(strsql,"update Zarp set \
shet='%s' where datz >= '%d-%d-1' and datz <= '%d-%d-31' and tabn=%d and shet='%s' and prn='1'",
data->rk.shet.ravno(),data->rk.gp,data->rk.mp,data->rk.gp,data->rk.mp,data->rk.tabnom.ravno_atoi(),data->shet_old.ravno());

iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"update Zarn1 set \
shet='%s' where tabn=%d and shet='%s'",
data->rk.shet.ravno(),data->rk.tabnom.ravno_atoi(),data->shet_old.ravno());

iceb_sql_zapis(strsql,1,0,data->window);

zaravprw(data->rk.tabnom.ravno_atoi(),data->rk.mp,data->rk.gp,NULL,data->window);

data->shet_old.new_plus(data->rk.shet.ravno());
}

/*********************/
/*Распечатка*/
/*******************/
void vkartz_ras(class vkartz_data *data)
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка"));


zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Распечатать корешок"));//0
punkt_m.plus(gettext("Распечатать карточку"));//1

int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);



class iceb_u_str tabnom;
tabnom.plus("");    
switch (nomer)
 {
  case -1:
    return;

  case 0:
    koreshw(data->rk.tabnom.ravno_atoi(),data->rk.mp,data->rk.gp,data->rk.fio.ravno(),data->rk.in.ravno(),
    data->rk.dolgn.ravno(),data->rk.podr.ravno_atoi(),data->rk.kateg.ravno_atoi(),data->window);
    break;

  case 1:
   raszkartw(data->rk.tabnom.ravno_atoi(),data->window);
    break;
 }
}
