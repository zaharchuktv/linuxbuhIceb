/*$Id: l_uplst_v.c,v 1.11 2011-02-21 07:35:54 sasa Exp $*/
/*24.03.2008	18.03.2008	Белых А.И.	l_uplst_v.c
Ввод и корректировка топлива по объектам списания и счетам
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
enum
 {
  E_KOD_OB,
  E_KOD_TOP,
  E_SHET_SP,
  E_PR_PO_GOR,
  E_ZT_PO_GOR,
  E_OTR_VREM,
  E_PR_ZA_GOR,
  E_ZT_ZA_GOR,
  E_TONO_KIL,
  E_ZT_NA_GRUZ,
  E_VREM_DVIG,
  E_ZT_NA_VREM, 
  KOLENTER  
 };
  
enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_uplst_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  short dd,md,gd;
  class iceb_u_str nomdok;
  int kod_pod;
  int kod_vod;
  int kod_avt;
  class iceb_u_str kod_topk;
  class iceb_u_str shet_spk;
  class iceb_u_str kod_obk;
    
  class iceb_u_str kod_ob;
  class iceb_u_str kod_top;
  class iceb_u_str shet_sp;
  class iceb_u_str pr_po_gor;
  class iceb_u_str zt_po_gor;
  class iceb_u_str otr_vrem;
  class iceb_u_str pr_za_gor;
  class iceb_u_str zt_za_gor;
  class iceb_u_str tono_kil;
  class iceb_u_str zt_na_gruz;
  class iceb_u_str vrem_dvig;
  class iceb_u_str zt_na_vrem;
  
  
  l_uplst_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear();
   }
  void clear()
   {
    kod_ob.new_plus("");
    kod_top.new_plus("");
    shet_sp.new_plus("");
    pr_po_gor.new_plus("");
    zt_po_gor.new_plus("");
    otr_vrem.new_plus("");
    pr_za_gor.new_plus("");
    zt_za_gor.new_plus("");
    tono_kil.new_plus("");
    zt_na_gruz.new_plus("");
    vrem_dvig.new_plus("");
    zt_na_vrem.new_plus("");
   }
  void read_rek()
   {
    kod_ob.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_OB]))));
    kod_top.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_TOP]))));
    shet_sp.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET_SP]))));
    pr_po_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PR_PO_GOR]))));
    zt_po_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ZT_PO_GOR]))));
    otr_vrem.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_OTR_VREM]))));
    pr_za_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PR_ZA_GOR]))));
    zt_za_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ZT_ZA_GOR]))));
    tono_kil.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TONO_KIL]))));
    zt_na_gruz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ZT_NA_GRUZ]))));
    vrem_dvig.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VREM_DVIG]))));
    zt_na_vrem.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ZT_NA_VREM]))));
   }
 };


gboolean   l_uplst_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplst_v_data *data);
void  l_uplst_v_knopka(GtkWidget *widget,class l_uplst_v_data *data);
void    l_uplst_v_vvod(GtkWidget *widget,class l_uplst_v_data *data);
int l_uplst_pk(char *god,GtkWidget*);
int l_uplst_zap(class l_uplst_v_data *data);

void  l_uplst_v_e_knopka(GtkWidget *widget,class l_uplst_v_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_uplst_v(short dd,short md,short gd,const char *nomdok,
int kod_pod,
int kod_vod,
int kod_avt,
const char *kod_top,
const char *shet_sp,
int kod_ob,
GtkWidget *wpredok)
{
class l_uplst_v_data data;
char strsql[512];
iceb_u_str kikz;

data.dd=dd;
data.md=md;
data.gd=gd;
data.nomdok.new_plus(nomdok);
data.kod_pod=kod_pod;
data.kod_vod=kod_vod;
data.kod_avt=kod_avt;

data.kod_topk.new_plus(kod_top);
data.shet_spk.new_plus(shet_sp);
data.kod_obk.new_plus(kod_ob);


if(data.kod_obk.ravno_atoi() != 0)
 {

  data.kod_top.new_plus(kod_top);
  data.shet_sp.new_plus(shet_sp);
  data.kod_ob.new_plus(kod_ob);


  sprintf(strsql,"select * from Upldok2 where datd='%04d-%02d-%02d' and kp=%d and \
nomd='%s' and kt='%s' and shet='%s' and ko=%d",
  data.gd,data.md,data.dd,
  data.kod_pod,
  data.nomdok.ravno(),
  data.kod_topk.ravno(),
  data.shet_spk.ravno(),
  data.kod_obk.ravno_atoi());
  
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.pr_po_gor.new_plus(row[6]);
  data.zt_po_gor.new_plus(row[7]);
  data.otr_vrem.new_plus(row[8]);
  data.pr_za_gor.new_plus(row[13]);
  data.zt_za_gor.new_plus(row[14]);
  data.tono_kil.new_plus(row[17]);
  data.zt_na_gruz.new_plus(row[15]);
  data.vrem_dvig.new_plus(row[18]);
  data.zt_na_vrem.new_plus(row[16]);
  
  kikz.plus(iceb_kikz(row[11],row[12],wpredok));
  
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kod_ob.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno_toutf());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uplst_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox = gtk_vbox_new (FALSE,1);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE,1);
GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

//label=gtk_label_new(gettext("Код"));
//gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OB]), label, FALSE, FALSE,1);
sprintf(strsql,"%s",gettext("Код объекта списания топлива"));
data.knopka_enter[E_KOD_OB]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OB]), data.knopka_enter[E_KOD_OB], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_OB]),"clicked",GTK_SIGNAL_FUNC(l_uplst_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_OB]),(gpointer)E_KOD_OB);
tooltips_enter[E_KOD_OB]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_OB],data.knopka_enter[E_KOD_OB],gettext("Выбор объекта списания топлива"),NULL);

data.entry[E_KOD_OB] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OB]), data.entry[E_KOD_OB], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_OB]), "activate",GTK_SIGNAL_FUNC(l_uplst_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_OB]),data.kod_ob.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_OB]),(gpointer)E_KOD_OB);


sprintf(strsql,"%s",gettext("Код топлива"));
data.knopka_enter[E_KOD_TOP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), data.knopka_enter[E_KOD_TOP], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_TOP]),"clicked",GTK_SIGNAL_FUNC(l_uplst_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_TOP]),(gpointer)E_KOD_TOP);
tooltips_enter[E_KOD_TOP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_TOP],data.knopka_enter[E_KOD_TOP],gettext("Выбор топлива"),NULL);

data.entry[E_KOD_TOP] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), data.entry[E_KOD_TOP], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_TOP]), "activate",GTK_SIGNAL_FUNC(l_uplst_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_TOP]),data.kod_top.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_TOP]),(gpointer)E_KOD_TOP);


sprintf(strsql,"%s",gettext("Счёт списания"));
data.knopka_enter[E_SHET_SP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_SP]), data.knopka_enter[E_SHET_SP], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET_SP]),"clicked",GTK_SIGNAL_FUNC(l_uplst_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET_SP]),(gpointer)E_SHET_SP);
tooltips_enter[E_SHET_SP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET_SP],data.knopka_enter[E_SHET_SP],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET_SP] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_SP]), data.entry[E_SHET_SP], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET_SP]), "activate",GTK_SIGNAL_FUNC(l_uplst_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_SP]),data.shet_sp.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET_SP]),(gpointer)E_SHET_SP);



label=gtk_label_new(gettext("Пробег по городу"));
gtk_box_pack_start (GTK_BOX (hbox[E_PR_PO_GOR]), label, FALSE, FALSE,1);

data.entry[E_PR_PO_GOR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_PR_PO_GOR]), data.entry[E_PR_PO_GOR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PR_PO_GOR]), "activate",GTK_SIGNAL_FUNC(l_uplst_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PR_PO_GOR]),data.pr_po_gor.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PR_PO_GOR]),(gpointer)E_PR_PO_GOR);


label=gtk_label_new(gettext("Затраты по городу"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZT_PO_GOR]), label, FALSE, FALSE,1);

data.entry[E_ZT_PO_GOR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_ZT_PO_GOR]), data.entry[E_ZT_PO_GOR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ZT_PO_GOR]), "activate",GTK_SIGNAL_FUNC(l_uplst_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZT_PO_GOR]),data.zt_po_gor.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ZT_PO_GOR]),(gpointer)E_ZT_PO_GOR);


label=gtk_label_new(gettext("Отработанное время"));
gtk_box_pack_start (GTK_BOX (hbox[E_OTR_VREM]), label, FALSE, FALSE,1);

data.entry[E_OTR_VREM] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_OTR_VREM]), data.entry[E_OTR_VREM], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_OTR_VREM]), "activate",GTK_SIGNAL_FUNC(l_uplst_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OTR_VREM]),data.otr_vrem.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_OTR_VREM]),(gpointer)E_OTR_VREM);


label=gtk_label_new(gettext("Пробег за городом"));
gtk_box_pack_start (GTK_BOX (hbox[E_PR_ZA_GOR]), label, FALSE, FALSE,1);

data.entry[E_PR_ZA_GOR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_PR_ZA_GOR]), data.entry[E_PR_ZA_GOR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PR_ZA_GOR]), "activate",GTK_SIGNAL_FUNC(l_uplst_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PR_ZA_GOR]),data.pr_za_gor.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PR_ZA_GOR]),(gpointer)E_PR_ZA_GOR);


label=gtk_label_new(gettext("Затраты за городом"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZT_ZA_GOR]), label, FALSE, FALSE,1);

data.entry[E_ZT_ZA_GOR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_ZT_ZA_GOR]), data.entry[E_ZT_ZA_GOR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ZT_ZA_GOR]), "activate",GTK_SIGNAL_FUNC(l_uplst_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZT_ZA_GOR]),data.zt_za_gor.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ZT_ZA_GOR]),(gpointer)E_ZT_ZA_GOR);


label=gtk_label_new(gettext("Тоннокилометры"));
gtk_box_pack_start (GTK_BOX (hbox[E_TONO_KIL]), label, FALSE, FALSE,1);

data.entry[E_TONO_KIL] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_TONO_KIL]), data.entry[E_TONO_KIL], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TONO_KIL]), "activate",GTK_SIGNAL_FUNC(l_uplst_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TONO_KIL]),data.tono_kil.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TONO_KIL]),(gpointer)E_TONO_KIL);


label=gtk_label_new(gettext("Затраты на груз"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZT_NA_GRUZ]), label, FALSE, FALSE,1);

data.entry[E_ZT_NA_GRUZ] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_ZT_NA_GRUZ]), data.entry[E_ZT_NA_GRUZ], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ZT_NA_GRUZ]), "activate",GTK_SIGNAL_FUNC(l_uplst_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZT_NA_GRUZ]),data.zt_na_gruz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ZT_NA_GRUZ]),(gpointer)E_ZT_NA_GRUZ);


label=gtk_label_new(gettext("Время работы двигателя"));
gtk_box_pack_start (GTK_BOX (hbox[E_VREM_DVIG]), label, FALSE, FALSE,1);

data.entry[E_VREM_DVIG] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_VREM_DVIG]), data.entry[E_VREM_DVIG], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VREM_DVIG]), "activate",GTK_SIGNAL_FUNC(l_uplst_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREM_DVIG]),data.vrem_dvig.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VREM_DVIG]),(gpointer)E_VREM_DVIG);


label=gtk_label_new(gettext("Затраты на отработанное время"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZT_NA_VREM]), label, FALSE, FALSE,1);

data.entry[E_ZT_NA_VREM] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_ZT_NA_VREM]), data.entry[E_ZT_NA_VREM], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ZT_NA_VREM]), "activate",GTK_SIGNAL_FUNC(l_uplst_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZT_NA_VREM]),data.zt_na_vrem.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ZT_NA_VREM]),(gpointer)E_ZT_NA_VREM);


GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_uplst_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_uplst_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uplst_v_e_knopka(GtkWidget *widget,class l_uplst_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_KOD_OB:

    if(l_uplost(1,&kod,&naim,data->window) == 0)
     data->kod_ob.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_OB]),data->kod_ob.ravno());

    sprintf(strsql,"select shet from Uplost where kod=%d",data->kod_ob.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     if(row[0][0] != '\0')
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_SP]),iceb_u_toutf(row[0]));
      
    return;  

  case E_SHET_SP:

    iceb_vibrek(1,"Plansh",&data->shet_sp,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_SP]),data->shet_sp.ravno_toutf());

    return;  

  case E_KOD_TOP:

    if(l_uplmt(1,&kod,&naim,data->window) == 0)
     data->kod_top.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_TOP]),data->kod_top.ravno_toutf());
      
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uplst_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplst_v_data *data)
{
//char  bros[512];

//printf("l_uplst_v_key_press\n");
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
void  l_uplst_v_knopka(GtkWidget *widget,class l_uplst_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplst_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_uplst_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
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

void    l_uplst_v_vvod(GtkWidget *widget,class l_uplst_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplst_v_vvod enter=%d\n",enter);
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
switch (enter)
 {
  case E_KOD_OB:
    data->kod_ob.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select shet from Uplost where kod=%d",data->kod_ob.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     if(row[0][0] != '\0')
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_SP]),iceb_u_toutf(row[0]));
     
    break;
  case E_KOD_TOP:
    data->kod_top.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_uplst_zap(class l_uplst_v_data *data)
{
char strsql[1024];
time_t  vrem=0;
struct OPSHET	shetv;

sprintf(strsql,"select kod from Uplost where kod=%d",data->kod_ob.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден код объекта списания топлива"),data->kod_ob.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

//Проверяем код топлива
sprintf(strsql,"select kodt from Uplmt where kodt='%s'",data->kod_top.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден код топлива"),data->kod_top.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

if(iceb_prsh1(data->shet_sp.ravno(),&shetv,data->window) != 0)
  return(1);



time(&vrem);

if(data->kod_obk.ravno_atoi() == 0)
  sprintf(strsql,"insert into Upldok2 \
values ('%04d-%02d-%02d',%d,'%s','%s','%s',%d,%.1f,%.3f,%.2f,%d,%d,%d,%ld,%.1f,%.3f,%.3f,%.3f,%d,%.2f)",
  data->gd,data->md,data->dd,
  data->kod_pod,
  data->nomdok.ravno(),
  data->kod_top.ravno(),
  data->shet_sp.ravno(),
  data->kod_ob.ravno_atoi(),
  data->pr_po_gor.ravno_atof(),
  data->zt_po_gor.ravno_atof(),
  data->otr_vrem.ravno_atof(),
  data->kod_vod,
  data->kod_avt,
  iceb_getuid(data->window),vrem,
  data->pr_za_gor.ravno_atof(),
  data->zt_za_gor.ravno_atof(),
  data->zt_na_gruz.ravno_atof(),
  data->zt_na_vrem.ravno_atof(),
  data->tono_kil.ravno_atoi(),
  data->vrem_dvig.ravno_atof());
else
  sprintf(strsql,"update Upldok2 set \
kt='%s',\
shet='%s',\
pr=%.1f,\
zt=%.3f,\
hs=%.2f,\
ktoz=%d,\
vrem=%ld,\
przg=%.1f,\
ztzg=%.3f,\
ztgr=%.3f,\
ztmh=%.3f,\
prgr=%d,\
vrrd=%.2f \
where datd='%04d-%d-%d'and kp=%d and nomd='%s' and kt='%s' and shet='%s' and ko=%d",
  data->kod_top.ravno(),
  data->shet_sp.ravno(),
  data->pr_po_gor.ravno_atof(),
  data->zt_po_gor.ravno_atof(),
  data->otr_vrem.ravno_atof(),
  iceb_getuid(data->window),vrem,
  data->pr_za_gor.ravno_atof(),
  data->zt_za_gor.ravno_atof(),
  data->zt_na_gruz.ravno_atof(),
  data->zt_na_vrem.ravno_atof(),
  data->tono_kil.ravno_atoi(),
  data->vrem_dvig.ravno_atof(),
  data->gd,data->md,data->dd,
  data->kod_pod,
  data->nomdok.ravno(),
  data->kod_topk.ravno(),
  data->shet_spk.ravno(),
  data->kod_ob.ravno_atoi());
  

//printw("\n%s\n",strsql);
//OSTANOV();

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 return(1);


gtk_widget_hide(data->window);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_uplst_pk(class l_uplst_v_data *data)
{
char strsql[512];

sprintf(strsql,"select datd from Upldok2 where datd='%04d-%02d-%02d' and kp=%d and nomd='%s' and \
kt='%s' and shet='%s' and ko=%d",
data->gd,data->md,data->dd,data->kod_pod,data->nomdok.ravno(),data->kod_top.ravno(),data->shet_sp.ravno(),data->kod_ob.ravno_atoi());
//printf("l_uplst_pk-%s\n",strsql);

if(iceb_sql_readkey(strsql,data->window) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такая запись уже есть !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

return(0);

}


