/*$Id: uoskart2.c,v 1.20 2011-09-07 11:44:20 sasa Exp $*/
/*06.09.2011	12.11.2007	Белых А.И.	uoskart2.c
Просмотр и корректировка карточки на инветарный номер
*/

#include "buhg_g.h"
#include <unistd.h>
enum
{
 FK2,
 FK4,
 FK5,
 FK6,
 FK7,
 FK10,
 KOL_F_KL
};

enum
 {
  E_NAIM,
  E_ZAV_IZ,
  E_PASPORT,
  E_MODEL,
  E_GOD,
  E_ZAV_NOMER,
  E_DATAVV,
  E_SHETU,
  E_SHET_AMORT,
  E_SIFR_AU,
  E_GRUP_NU,
  E_KOEFIC_NU,
  E_KOEFIC_BU,
  E_SOST_OB,
  E_NOMER_ZNAK,
  E_GRUP_BU,
  KOLENTER  
 };

class uoskart2_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_arrow[2];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *label_naim[KOLENTER];
  GtkWidget *label_mo_pd;
  GtkWidget *label_bsiz;
  GtkWidget *opt;
  short kl_shift;
  short voz;      //0-ввели 1 нет
  
  int innom;
  short dp,mp,gp; /*Дата просмотра карточки*/
  int podr; /*Код подразделения в котором находится*/      
  
  /*Реквизиты которые вводим*/  
  class iceb_u_str god; /*Год изготовления*/
  class iceb_u_str naim;
  class iceb_u_str zav_iz;
  class iceb_u_str pasport;
  class iceb_u_str model;
  class iceb_u_str zav_nomer;  
  class iceb_u_str datavv;  
  class iceb_u_str shetu;  
  class iceb_u_str shet_amort;  
  class iceb_u_str sifr_au;  
  class iceb_u_str grup_nu;
  class iceb_u_str koefic_nu;
  class iceb_u_str koefic_bu;
  class iceb_u_str nomer_znak;
  class iceb_u_str grup_bu;
  int sost_ob;  
  
  double bs;
  double iz;  
  
  uoskart2_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    god.plus("");
    naim.plus("");
    zav_iz.plus("");
    pasport.plus("");
    model.plus("");
    zav_nomer.plus("");
    datavv.plus("");
    shetu.plus("");
    shet_amort.plus("");
    sifr_au.plus("");
    grup_nu.plus("");
    koefic_nu.plus("1");
    koefic_bu.plus("1");
    nomer_znak.plus("");
    grup_bu.plus("");
    sost_ob=0;
    bs=iz=0.;
   }

  void read_rek()
   {
    god.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GOD]))));
    naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM]))));
    zav_iz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ZAV_IZ]))));
    pasport.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PASPORT]))));
    model.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_MODEL]))));
    zav_nomer.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_MODEL]))));
    datavv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAVV]))));
    shetu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHETU]))));
    shet_amort.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET_AMORT]))));
    sifr_au.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SIFR_AU]))));
    grup_nu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GRUP_NU]))));
    koefic_nu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOEFIC_NU]))));
    koefic_bu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOEFIC_BU]))));
    nomer_znak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMER_ZNAK]))));
    grup_bu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GRUP_BU]))));

   }
 };

gboolean   uoskart2_v_key_press(GtkWidget *widget,GdkEventKey *event,class uoskart2_data *data);
void    uoskart2_v_vvod(GtkWidget *widget,class uoskart2_data *data);
void  uoskart2_v_knopka(GtkWidget *widget,class uoskart2_data *data);
void uosopp_kkvt(char *skod,char *nkod,GtkWidget *wpredok);
int uosopp_pk(char *kod,GtkWidget *wpredok);
void  uoskart2_e_knopka(GtkWidget *widget,class uoskart2_data *data);

void uoskart2_read(class uoskart2_data *data);
void uoskart2_read_pd(short dr,short mr,short gr,class uoskart2_data *data);
int uoskart2_zk(class uoskart2_data *data);
void  uoskart2_knopka_arrow(GtkWidget *widget,class uoskart2_data *data);
void uoskart2_f4(class uoskart2_data *data);
void l_uosamort(int innom,int podr,const char *hzt,const char *hna,GtkWidget *wpredok);
void l_uosamortbu(int innom,int podr,const char *hzt,const char *hna,GtkWidget *wpredok);
void uosrsd1w(int in,GtkWidget *wpredok);
void rasnasw(int in,GtkWidget *wpredok);
void uoskart2_rsi(class uoskart2_data *data);
void uoskarw(int in,double bs,double iz,GtkWidget *wpredok);
int l_uosindm(int innom,GtkWidget *wpredok);
void l_uosls(int innom,GtkWidget *wpredok);
int uoskart2_rk();
void rasdopuosw(short d,short m,short g,int innom,GtkWidget *wpredok);

extern char *name_system;
extern SQL_baza bd;
extern short startgoduos;
extern iceb_u_str shrift_ravnohir;
extern char *imabaz;

int uoskart2(int innom, /*Инвентарный номер*/
short dp,short mp,short gp, /*Дата просмотра карточки*/
GtkWidget *wpredok)
{
class uoskart2_data data;
char strsql[512];

data.innom=innom;
data.dp=dp;
data.mp=mp;
data.gp=gp;

if(data.dp == 0)
 iceb_u_poltekdat(&data.dp,&data.mp,&data.gp);

 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Просмотр карточки инвентарного номера"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uoskart2_v_key_press),&data);

iceb_u_str zagolov;
zagolov.new_plus(gettext("Просмотр карточки инвентарного номера"));
sprintf(strsql,"%s:%d",gettext("Инвентарный номер"),innom);
zagolov.ps_plus(strsql);
GtkWidget *label=gtk_label_new(zagolov.ravno_toutf());

data.label_mo_pd=gtk_label_new("");

GtkWidget *vbox = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox_knop = gtk_vbox_new (FALSE, 1);
GtkWidget *hbox_main = gtk_hbox_new (FALSE, 1);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 1);


gtk_container_add (GTK_CONTAINER (data.window), hbox_main);
gtk_box_pack_start (GTK_BOX (hbox_main), vbox_knop, FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (hbox_main), vbox, FALSE, FALSE, 1);

gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.label_mo_pd, FALSE, FALSE, 1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], FALSE, FALSE, 1);

GtkWidget *hbox_label = gtk_hbox_new (FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox), hbox_label, FALSE, FALSE, 1);

data.label_bsiz=gtk_label_new(".\n.\n.\n.\n.\n");
gtk_box_pack_start (GTK_BOX (hbox_label),data.label_bsiz, FALSE, FALSE, 1);
PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_modify_font(GTK_WIDGET(data.label_bsiz),font_pango);
pango_font_description_free(font_pango);


data.knopka_arrow[0]=gtk_button_new();
data.knopka_arrow[1]=gtk_button_new();

GtkWidget *arrow1=gtk_arrow_new(GTK_ARROW_UP,GTK_SHADOW_IN);
gtk_container_add (GTK_CONTAINER (data.knopka_arrow[0]), arrow1);

GtkWidget *arrow2=gtk_arrow_new(GTK_ARROW_DOWN,GTK_SHADOW_IN);
gtk_container_add (GTK_CONTAINER (data.knopka_arrow[1]), arrow2);


gtk_signal_connect(GTK_OBJECT(data.knopka_arrow[0]),"clicked",GTK_SIGNAL_FUNC(uoskart2_knopka_arrow),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_arrow[0]),(gpointer)0);

gtk_signal_connect(GTK_OBJECT(data.knopka_arrow[1]),"clicked",GTK_SIGNAL_FUNC(uoskart2_knopka_arrow),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_arrow[1]),(gpointer)1);


GtkTooltips *tooltips_arrow[2];
tooltips_arrow[0]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_arrow[0],data.knopka_arrow[0],gettext("Увеличить дату просмотра"),NULL);
tooltips_arrow[1]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_arrow[1],data.knopka_arrow[1],gettext("Уменьшить дату просмотра"),NULL);

gtk_box_pack_start (GTK_BOX (hbox_label),data.knopka_arrow[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (hbox_label),data.knopka_arrow[1], TRUE, TRUE, 1);



GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Наименование"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label,FALSE, FALSE, 1);

data.entry[E_NAIM] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

sprintf(strsql,"%s",gettext("Завод изготовитель"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_ZAV_IZ]), label,FALSE, FALSE, 1);

data.entry[E_ZAV_IZ] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_ZAV_IZ]), data.entry[E_ZAV_IZ],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ZAV_IZ]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZAV_IZ]),data.zav_iz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ZAV_IZ]),(gpointer)E_ZAV_IZ);

sprintf(strsql,"%s",gettext("Паспорт/чертёж"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PASPORT]), label,FALSE, FALSE, 1);

data.entry[E_PASPORT] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_PASPORT]), data.entry[E_PASPORT],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PASPORT]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PASPORT]),data.pasport.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PASPORT]),(gpointer)E_PASPORT);

sprintf(strsql,"%s",gettext("Модель"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_MODEL]), label,FALSE, FALSE, 1);

data.entry[E_MODEL] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_MODEL]), data.entry[E_MODEL],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_MODEL]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MODEL]),data.model.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_MODEL]),(gpointer)E_MODEL);


sprintf(strsql,"%s",gettext("Год изготовления"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label, FALSE, FALSE, 1);

data.entry[E_GOD] = gtk_entry_new_with_max_length (4);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GOD]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.god.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GOD]),(gpointer)E_GOD);




sprintf(strsql,"%s",gettext("Заводской номер"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_ZAV_NOMER]), label,FALSE, FALSE, 1);

data.entry[E_ZAV_NOMER] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_ZAV_NOMER]), data.entry[E_ZAV_NOMER],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ZAV_NOMER]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZAV_NOMER]),data.zav_nomer.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ZAV_NOMER]),(gpointer)E_ZAV_NOMER);


sprintf(strsql,"%s (%s)",gettext("Дата ввода в эксплуатацию"),gettext("д.м.г"));
data.knopka_enter[E_DATAVV]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAVV]), data.knopka_enter[E_DATAVV], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAVV]),"clicked",GTK_SIGNAL_FUNC(uoskart2_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAVV]),(gpointer)E_DATAVV);
tooltips_enter[E_DATAVV]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAVV],data.knopka_enter[E_DATAVV],gettext("Выбор даты"),NULL);

data.entry[E_DATAVV] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAVV]), data.entry[E_DATAVV],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAVV]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAVV]),data.datavv.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAVV]),(gpointer)E_DATAVV);


sprintf(strsql,"%s",gettext("Счёт учёта"));
data.knopka_enter[E_SHETU]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.knopka_enter[E_SHETU], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHETU]),"clicked",GTK_SIGNAL_FUNC(uoskart2_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHETU]),(gpointer)E_SHETU);
tooltips_enter[E_SHETU]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHETU],data.knopka_enter[E_SHETU],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHETU] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.entry[E_SHETU],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHETU]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETU]),data.shetu.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHETU]),(gpointer)E_SHETU);

data.label_naim[E_SHETU]=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.label_naim[E_SHETU],TRUE, TRUE, 1);

sprintf(strsql,"%s",gettext("Шифр затрат амортотчислений"));
data.knopka_enter[E_SHET_AMORT]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_AMORT]), data.knopka_enter[E_SHET_AMORT], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET_AMORT]),"clicked",GTK_SIGNAL_FUNC(uoskart2_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET_AMORT]),(gpointer)E_SHET_AMORT);
tooltips_enter[E_SHET_AMORT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET_AMORT],data.knopka_enter[E_SHET_AMORT],gettext("Выбор шифра"),NULL);

data.entry[E_SHET_AMORT] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_AMORT]), data.entry[E_SHET_AMORT],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET_AMORT]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_AMORT]),data.shet_amort.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET_AMORT]),(gpointer)E_SHET_AMORT);

data.label_naim[E_SHET_AMORT]=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_AMORT]), data.label_naim[E_SHET_AMORT],TRUE, TRUE, 1);


sprintf(strsql,"%s",gettext("Шифр аналитического учёта"));
data.knopka_enter[E_SIFR_AU]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SIFR_AU]), data.knopka_enter[E_SIFR_AU], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SIFR_AU]),"clicked",GTK_SIGNAL_FUNC(uoskart2_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SIFR_AU]),(gpointer)E_SIFR_AU);
tooltips_enter[E_SIFR_AU]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SIFR_AU],data.knopka_enter[E_SIFR_AU],gettext("Выбор шифра"),NULL);

data.entry[E_SIFR_AU] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_SIFR_AU]), data.entry[E_SIFR_AU],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SIFR_AU]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SIFR_AU]),data.sifr_au.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SIFR_AU]),(gpointer)E_SIFR_AU);

data.label_naim[E_SIFR_AU]=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_SIFR_AU]), data.label_naim[E_SIFR_AU],TRUE, TRUE, 1);


sprintf(strsql,"%s",gettext("Группа налогового учёта"));
data.knopka_enter[E_GRUP_NU]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_NU]), data.knopka_enter[E_GRUP_NU], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUP_NU]),"clicked",GTK_SIGNAL_FUNC(uoskart2_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUP_NU]),(gpointer)E_GRUP_NU);
tooltips_enter[E_GRUP_NU]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUP_NU],data.knopka_enter[E_GRUP_NU],gettext("Выбор шифра"),NULL);

data.entry[E_GRUP_NU] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_NU]), data.entry[E_GRUP_NU],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUP_NU]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUP_NU]),data.grup_nu.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUP_NU]),(gpointer)E_GRUP_NU);

data.label_naim[E_GRUP_NU]=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_NU]), data.label_naim[E_GRUP_NU],TRUE, TRUE, 1);



sprintf(strsql,"%s",gettext("Поправочный коэффициент для налогового учёта"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOEFIC_NU]), label,FALSE, FALSE, 1);

data.entry[E_KOEFIC_NU] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOEFIC_NU]), data.entry[E_KOEFIC_NU],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOEFIC_NU]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOEFIC_NU]),data.koefic_nu.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOEFIC_NU]),(gpointer)E_KOEFIC_NU);


sprintf(strsql,"%s",gettext("Поправочный коэффициент для бухгалтерского учёта"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOEFIC_BU]), label,FALSE, FALSE, 1);

data.entry[E_KOEFIC_BU] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOEFIC_BU]), data.entry[E_KOEFIC_BU],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOEFIC_BU]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOEFIC_BU]),data.koefic_bu.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOEFIC_BU]),(gpointer)E_KOEFIC_BU);


sprintf(strsql,"%s",gettext("Государственный номерной знак автотранспортного средства"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_ZNAK]), label,FALSE, FALSE, 1);

data.entry[E_NOMER_ZNAK] = gtk_entry_new_with_max_length (39);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_ZNAK]), data.entry[E_NOMER_ZNAK],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMER_ZNAK]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_ZNAK]),data.nomer_znak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMER_ZNAK]),(gpointer)E_NOMER_ZNAK);




sprintf(strsql,"%s",gettext("Группа бухгалтерского учёта"));
data.knopka_enter[E_GRUP_BU]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_BU]), data.knopka_enter[E_GRUP_BU], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUP_BU]),"clicked",GTK_SIGNAL_FUNC(uoskart2_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUP_BU]),(gpointer)E_GRUP_BU);
tooltips_enter[E_GRUP_BU]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUP_BU],data.knopka_enter[E_GRUP_BU],gettext("Выбор шифра"),NULL);

data.entry[E_GRUP_BU] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_BU]), data.entry[E_GRUP_BU],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUP_BU]), "activate",GTK_SIGNAL_FUNC(uoskart2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUP_BU]),data.grup_bu.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUP_BU]),(gpointer)E_GRUP_BU);

data.label_naim[E_GRUP_BU]=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_BU]), data.label_naim[E_GRUP_BU],TRUE, TRUE, 1);



label=gtk_label_new(gettext("Состояние объекта"));
gtk_box_pack_start (GTK_BOX (hbox[E_SOST_OB]), label, FALSE, FALSE, 1);

/*Создаём путкт меню для выбора режима расчёта амортизации*/
class iceb_u_spisok sost_ob;
sost_ob.plus(gettext("Амортизация считается для бух. учёта и налогового учёта"));
sost_ob.plus(gettext("Амортизиция не считается для бух. учёта и налогового учёта"));
sost_ob.plus(gettext("Амортизация считается для бух. учёта и не считается для налогового учёта"));
sost_ob.plus(gettext("Амортизация не считается для бух. учёта и считается для налогового учёта"));


iceb_pm_vibor(&sost_ob,&data.opt,&data.sost_ob);
gtk_box_pack_start (GTK_BOX (hbox[E_SOST_OB]), data.opt, FALSE, FALSE, 1);



GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(uoskart2_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK2], TRUE, TRUE, 1);



sprintf(strsql,"F4 %s",gettext("Просмотр"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Просмотр информации по инвентарному номеру"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(uoskart2_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK4], TRUE, TRUE, 1);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатать карточку"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]),"clicked",GTK_SIGNAL_FUNC(uoskart2_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK5], TRUE, TRUE, 1);

sprintf(strsql,"F6 %s",gettext("Драг.мет."));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Ввод и корректировка списка драгоценных металлов"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]),"clicked",GTK_SIGNAL_FUNC(uoskart2_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK6], TRUE, TRUE, 1);

sprintf(strsql,"F7 %s",gettext("Ликв.стоимость"));
data.knopka[FK7]=gtk_button_new_with_label(strsql);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Ввод и корректировка ликвидационной стоимости"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]),"clicked",GTK_SIGNAL_FUNC(uoskart2_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK7], TRUE, TRUE, 1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(uoskart2_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK10], TRUE, TRUE, 1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

uoskart2_read(&data); /*читаем реквизиты карточки*/

gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  uoskart2_e_knopka(GtkWidget *widget,class uoskart2_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("uoskart2_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATAVV:
    iceb_calendar(&data->datavv,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAVV]),data->datavv.ravno());
      
    return;  

  case E_SHETU:
    if(iceb_vibrek(1,"Plansh",&data->shetu,&naim,data->window) == 0)
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHETU]),naim.ravno_toutf(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),data->shetu.ravno_toutf());
    return;

  case E_SHET_AMORT:
    if(l_uoshz(1,&kod,&naim,data->window) == 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHET_AMORT]),naim.ravno_toutf(20));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_AMORT]),kod.ravno_toutf());
     }
    return;

  case E_SIFR_AU:
    if(l_uoshau(1,&kod,&naim,data->window) == 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_SIFR_AU]),naim.ravno_toutf(20));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SIFR_AU]),kod.ravno_toutf());
     }
    return;

  case E_GRUP_NU:
    if(l_uosgrnu(1,&kod,&naim,data->window) == 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_NU]),naim.ravno_toutf(20));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUP_NU]),kod.ravno_toutf());
     }
    return;

  case E_GRUP_BU:
    if(l_uosgrbu(1,&kod,&naim,data->window) == 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_BU]),naim.ravno_toutf(20));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUP_BU]),kod.ravno_toutf());
     }
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uoskart2_v_key_press(GtkWidget *widget,GdkEventKey *event,class uoskart2_data *data)
{

//printf("uoskart2_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    return(TRUE);

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  case GDK_F7:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);


  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case GDK_Page_Up:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka_arrow[0]),"clicked");
    return(TRUE);

  case GDK_Page_Down:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka_arrow[1]),"clicked");
    return(TRUE);

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
void  uoskart2_v_knopka(GtkWidget *widget,class uoskart2_data *data)
{
int nomer=0;
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

/*g_print("uoskart2_v_knopka knop=%d\n",knop)*/

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(uoskart2_zk(data) != 0)
     return;

    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
    return;  

  case FK5: 
    if((nomer=uoskart2_rk()) < 0)
     return;
    if(nomer == 0)
      uoskarw(data->innom,data->bs,data->iz,data->window);
    if(nomer == 1)
     rasdopuosw(data->dp,data->mp,data->gp,data->innom,data->window);
     
    return;

  case FK6: 
    l_uosindm(data->innom,data->window);
    return;

  case FK7: 
    l_uosls(data->innom,data->window);
    return;

  case FK4: 
    uoskart2_f4(data);
    return;
    
  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*****************************/
/*Обработчик нажатия кнопок со стрелками */
/*****************************/
void  uoskart2_knopka_arrow(GtkWidget *widget,class uoskart2_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

/*g_print("uoskart2_v_knopka knop=%d\n",knop);*/

switch (knop)
 {
  case 0:
    iceb_u_dpm(&data->dp,&data->mp,&data->gp,3);
    break;
  case 1:
    iceb_u_dpm(&data->dp,&data->mp,&data->gp,4);
    break;
 }
printf("Смена даты просмотра %d.%d.%d\n",data->dp,data->mp,data->gp);
uoskart2_read_pd(1,data->mp,data->gp,data);

}

/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    uoskart2_v_vvod(GtkWidget *widget,class uoskart2_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

//g_print("uoskart2_v_vvod enter=%d\n",enter);
struct OPSHET rek_shet;
switch (enter)
 {
  case E_SHETU:
    data->shetu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(iceb_prsh1(data->shetu.ravno(),&rek_shet,data->window) != 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHETU]),"");
      return;
     }

    gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHETU]),rek_shet.naim.ravno_toutf(20));
    break;

  case E_SHET_AMORT:
    data->shet_amort.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select naik from Uoshz where kod='%s'",data->shet_amort.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Ненайден шифр аморт-отчислений"),data->shet_amort.ravno());
      iceb_menu_soob(strsql,data->window);
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHET_AMORT]),"");
      return;
     }
    sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);    
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHET_AMORT]),iceb_u_toutf(strsql));

    break;

  case E_SIFR_AU:
    data->sifr_au.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select naik from Uoshau where kod='%s'",data->sifr_au.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Ненайден шифр аналитического учёта"),data->sifr_au.ravno());
      iceb_menu_soob(strsql,data->window);
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_SIFR_AU]),"");
      return;
     }
    sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);    
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_SIFR_AU]),iceb_u_toutf(strsql));

    break;

  case E_GRUP_NU:
    data->grup_nu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select naik from Uosgrup where kod='%s'",data->grup_nu.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Ненайденa группа налогового учёта"),data->grup_nu.ravno());
      iceb_menu_soob(strsql,data->window);
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_NU]),"");
      return;
     }
    sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);    
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_NU]),iceb_u_toutf(strsql));

    break;


  case E_GRUP_BU:
    data->grup_bu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",data->grup_bu.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Ненайденa группа бухгалтерского учёта"),data->grup_bu.ravno());
      iceb_menu_soob(strsql,data->window);
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_BU]),"");
      return;
     }
    sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);    
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_BU]),iceb_u_toutf(strsql));

    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/******************************/
/*Чтение реквизитов заданного инвентарного номера*/
/***********************************************/

void uoskart2_read(class uoskart2_data *data)
{

class iceb_gdite_data gdite;
iceb_gdite(&gdite,1,data->window);


SQL_str row;
class SQLCURSOR cur;

char strsql[512];
sprintf(strsql,"select * from Uosin where innom=%d",data->innom);
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s:%d!",gettext("Ненайден инвентарный номер"),data->innom);
  iceb_menu_soob(strsql,data->window);
  return;   
 }

iceb_refresh();


gtk_entry_set_text(GTK_ENTRY(data->entry[E_GOD]),iceb_u_toutf(row[1]));

gtk_entry_set_text(GTK_ENTRY(data->entry[E_NAIM]),iceb_u_toutf(row[2]));

gtk_entry_set_text(GTK_ENTRY(data->entry[E_ZAV_IZ]),iceb_u_toutf(row[3]));

gtk_entry_set_text(GTK_ENTRY(data->entry[E_PASPORT]),iceb_u_toutf(row[4]));

gtk_entry_set_text(GTK_ENTRY(data->entry[E_MODEL]),iceb_u_toutf(row[5]));

gtk_entry_set_text(GTK_ENTRY(data->entry[E_ZAV_NOMER]),iceb_u_toutf(row[6]));

gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAVV]),iceb_u_datzap(row[7]));
 


class poiinpdw_data rekin;

if(poiinpdw(data->innom,data->mp,data->gp,&rekin,data->window) != 0)
 return;

data->shet_amort.new_plus(rekin.hzt.ravno());
data->grup_nu.new_plus(rekin.hna.ravno());
data->grup_bu.new_plus(rekin.hnaby.ravno());

class iceb_u_str naim("");
sprintf(strsql,"select nais from Plansh where ns='%s'",rekin.shetu.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 naim.new_plus(row[0]);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),iceb_u_toutf(rekin.shetu.ravno()));
gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHETU]),naim.ravno_toutf(20));


naim.new_plus("");
sprintf(strsql,"select naik from Uoshz where kod='%s'",rekin.hzt.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 naim.new_plus(row[0]);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_AMORT]),iceb_u_toutf(rekin.hzt.ravno()));
gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHET_AMORT]),naim.ravno_toutf(20));


naim.new_plus("");
sprintf(strsql,"select naik from Uoshau where kod='%s'",rekin.hau.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 naim.new_plus(row[0]);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_SIFR_AU]),iceb_u_toutf(rekin.hau.ravno()));
gtk_label_set_text(GTK_LABEL(data->label_naim[E_SIFR_AU]),naim.ravno_toutf(20));

naim.new_plus("");
sprintf(strsql,"select naik from Uosgrup where kod='%s'",rekin.hna.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 naim.new_plus(row[0]);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUP_NU]),iceb_u_toutf(rekin.hna.ravno()));
gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_NU]),naim.ravno_toutf(20));

naim.new_plus("");
sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",rekin.hnaby.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 naim.new_plus(row[0]);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUP_BU]),iceb_u_toutf(rekin.hnaby.ravno()));
gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_BU]),naim.ravno_toutf(20));

sprintf(strsql,"%.10g",rekin.popkf);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOEFIC_NU]),strsql);

sprintf(strsql,"%.10g",rekin.popkfby);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOEFIC_BU]),strsql);

data->sost_ob=rekin.soso;

gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMER_ZNAK]),iceb_u_toutf(rekin.nomz.ravno()));

uoskart2_read_pd(1,data->mp,data->gp,data);


}
/*********************************/
/*Запись новой карточки */
/*******************************/
/*Если вернули 0-записали*/
int uoskart2_zk(class uoskart2_data *data)
{
char strsql[512];

class iceb_u_str mes_god;
if(iceb_menu_mes_god(&mes_god,gettext("Введите каким месяцем записать изменённые данные"),"",data->window) != 0)
 return(1);
short mz,gz;

iceb_u_rsdat1(&mz,&gz,mes_god.ravno());


/*Проверяем дату*/
if(iceb_pbpds(mz,gz,data->window) != 0)
  return(1);

if(data->naim.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введено наименование!"),data->window);
  return(1);
 }

if(data->koefic_nu.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Коэффициент для налогового учёта не может быть равным нолю!"),data->window);
  return(1);
 }

if(data->koefic_bu.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Коэффициент для бух. учёта не может быть равным нолю!"),data->window);
  return(1);
 }

struct OPSHET rek_shet;

if(iceb_prsh1(data->shetu.ravno(),&rek_shet,data->window) != 0)
  return(1);

sprintf(strsql,"select naik from Uoshz where kod='%s'",data->shet_amort.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Ненайден шифр аморт-отчислений"),data->shet_amort.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

sprintf(strsql,"select naik from Uoshau where kod='%s'",data->sifr_au.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Ненайден шифр аналитического учёта"),data->sifr_au.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

sprintf(strsql,"select naik from Uosgrup where kod='%s'",data->grup_nu.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Ненайденa группа налогового учёта"),data->grup_nu.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",data->grup_bu.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Ненайденa группа бухгалтерского учёта"),data->grup_bu.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }



time_t vrem=time(NULL);


sprintf(strsql,"update Uosin \
set \
god=%d,\
naim='%s',\
zaviz='%s',\
pasp='%s',\
model='%s',\
zavnom='%s',\
datvv='%s',\
ktoz=%d,\
vrem=%ld \
where innom=%d",
data->god.ravno_atoi(),
data->naim.ravno_filtr(),
data->zav_iz.ravno_filtr(),
data->pasport.ravno_filtr(),
data->model.ravno_filtr(),
data->zav_nomer.ravno_filtr(),
data->datavv.ravno_sqldata(),
iceb_getuid(data->window),
vrem,
data->innom);


if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 return(1);

sprintf(strsql,"replace into Uosinp values (%d,%d,%d,'%s','%s','%s','%s',%.6g,%d,'%s',%d,%ld,'%s',%.10g)",
data->innom,
mz,gz,
data->shetu.ravno_filtr(),
data->shet_amort.ravno_filtr(),
data->sifr_au.ravno_filtr(),
data->grup_nu.ravno_filtr(),
data->koefic_nu.ravno_atof(),
data->sost_ob,
data->nomer_znak.ravno_filtr(),
iceb_getuid(data->window),
vrem,
data->grup_bu.ravno_filtr(),
data->koefic_bu.ravno_atof());

iceb_sql_zapis(strsql,1,0,data->window);

return(0);
}
/****************************/
/*Чтение переменных данных*/
/**************************/

void uoskart2_read_pd(short dr,short mr,short gr,class uoskart2_data *data)
{
/*printf("uoskart2_read_pd start %d.%d.%d\n",dr,mr,gr);*/
char strsql[512];

/*Узнаём в каком подразделении находится инвентарный номер для того, чтобы потом взять балансовую стоимость*/
data->podr=0;
int kodotl=0;
class bsizw_data bal_st;
class iceb_u_str mo_pd("");
if(poiinw(data->innom,dr,mr,gr,&data->podr,&kodotl,data->window) == 0)
 {
  SQL_str row;
  class SQLCURSOR cur;
  
  sprintf(strsql,"select naik from Uospod where kod=%d",data->podr);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   {
    sprintf(strsql,"%s:%d/%s\n",gettext("Подразделение"),data->podr,row[0]);
    mo_pd.plus(strsql);
   }
  sprintf(strsql,"select naik from Uosol where kod=%d",kodotl);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   {
    sprintf(strsql,"%s:%d/%s",gettext("Материально-ответственный"),kodotl,row[0]);
    mo_pd.plus(strsql);
   }
  
  bsizw(data->innom,data->podr,dr,mr,gr,&bal_st,NULL,data->window);
 }


class iceb_u_str text;

sprintf(strsql,"%55s     %s",gettext("Налоговый учет"),gettext("Бухгалтерский учет"));
text.plus(strsql);

char bros[512];

sprintf(bros,"%s 1.1.%d",gettext("Балансовая стоимость на"),startgoduos);

sprintf(strsql,"%*s:%10.2f %10.2f %10.2f %10.2f\n",iceb_u_kolbait(35,bros),bros,bal_st.sbs,bal_st.siz,bal_st.sbsby,bal_st.sizby);
text.ps_plus(strsql);

sprintf(strsql,"%*s:%10.2f %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(35,gettext("Изменение за период")),gettext("Изменение за период"),bal_st.bs,bal_st.iz,bal_st.bsby,bal_st.izby);
text.plus(strsql);


sprintf(strsql,"%*s:%10s %10.2f %10s %10.2f\n",
iceb_u_kolbait(35,gettext("Амортизация")),gettext("Амортизация")," ",bal_st.iz1," ",bal_st.iz1by);
text.plus(strsql);

sprintf(bros,"%s %02d.%02d.%d",
gettext("Балансовая стоимость на"),dr,mr,gr);
sprintf(strsql,"%*s:%10.2f %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(35,bros),bros,bal_st.sbs+bal_st.bs,bal_st.siz+bal_st.iz+bal_st.iz1,bal_st.sbsby+bal_st.bsby,bal_st.sizby+bal_st.izby+bal_st.iz1by);
text.plus(strsql);

sprintf(strsql,"%*s %10.2f %10s %10.2f",
iceb_u_kolbait(46,gettext("Остаток")),gettext("Остаток"),
(bal_st.sbs+bal_st.bs)-(bal_st.siz+bal_st.iz+bal_st.iz1),
" ",
(bal_st.sbsby+bal_st.bsby)-(bal_st.sizby+bal_st.izby+bal_st.iz1by));

text.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_bsiz),text.ravno_toutf());
gtk_label_set_text(GTK_LABEL(data->label_mo_pd),mo_pd.ravno_toutf());

data->bs=bal_st.sbs+bal_st.bs;
data->iz=bal_st.siz+bal_st.iz+bal_st.iz1;

}
/*****************************/
/*Меню F4*/
/****************************/
void uoskart2_f4(class uoskart2_data *data)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Просмотр амортизации налогового учёта"));//0
punkt_m.plus(gettext("Просмотр амортизации бухгалтерского учёта"));//1
punkt_m.plus(gettext("Просмотр списка документов"));//2
punkt_m.plus(gettext("Просмотр списка изменений"));//3
punkt_m.plus(gettext("Просмотр дополнительной информации"));//3
punkt_m.plus(gettext("Просмотр нормативно-справочной информации"));//5


int nomer=0;

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);


switch (nomer)
 {
  case -1:
    break;
   
  case 0:
    l_uosamort(data->innom,data->podr,data->shet_amort.ravno(),data->grup_nu.ravno(),data->window);
    break;

  case 1:
    l_uosamortbu(data->innom,data->podr,data->shet_amort.ravno(),data->grup_bu.ravno(),data->window);
    break;

  case 2:
    uosrsd1w(data->innom,data->window);
    break;

  case 3:
   rasnasw(data->innom,data->window);
   break;
  
  case 4:
   uoskart2_rsi(data);
   break;

  case 5:
    i_xuos_vnsi(data->window);
    break;
 }

}


/***************************/
/*Работа с дополнительной информацией*/
/**********************************/
void uoskart2_rsi(class uoskart2_data *data)
{

char strsql[512];
 
 //Установить флаг работы с карточкой
sprintf(strsql,"u%s%d",imabaz,data->innom);
class iceb_sql_flag flag;
if(flag.flag_on(strsql) != 0)
 {
  iceb_menu_soob(gettext("С дополнением работает другой оператор!"),data->window);
  return;
 }
char bros[312];

sprintf(bros,"%d",data->innom);
sprintf(strsql,"kontr%d.tmp",getpid());
iceb_dikont('-',strsql,bros,"Uosin1");

iceb_vizred(strsql,data->window);

iceb_dikont('+',strsql,bros,"Uosin1");
unlink(strsql);


}
/****************************************/
/*распечатки*/
/********************************/
int uoskart2_rk()
{


class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Распечатка типовой формы N03-6"));
punkt_m.plus(gettext("Распечатка карточки"));


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

return(nomer);


}
