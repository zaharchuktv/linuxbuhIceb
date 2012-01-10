/*$Id: uosprs_r.c,v 1.11 2011-04-06 09:49:17 sasa Exp $*/
/*05.04.2011	27.12.2007	Белых А.И.	uosprs_r.c 
Расчёт пропорционального распределения сумм
*/


#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "uosprs.h"

class uosprs_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class uosprs_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  uosprs_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosprs_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosprs_r_data *data);
gint uosprs_r1(class uosprs_r_data *data);
void  uosprs_r_v_knopka(GtkWidget *widget,class uosprs_r_data *data);

void uosrs1(class uosprs_r_data *data);
void uosrsgr(class uosprs_r_data *data);

void	sapkasord(short dd,short md,short gd,int nomlist,FILE *ffva);
void   hgrkd(FILE *ff);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;

int uosprs_r(class uosprs_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class uosprs_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Пропрорциональное распределение сумм"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uosprs_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

repl.plus(gettext("Пропорциональное распреление сумм"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),imabaz,organ);
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno_toutf());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),450,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
//PangoFontDescription *font_pango=pango_font_description_from_string("Nimbus Mono L, Bold 12");
PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_modify_font(GTK_WIDGET(data.view),font_pango);
pango_font_description_free(font_pango);

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
//gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_ALWAYS,GTK_POLICY_ALWAYS);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


data.bar=gtk_progress_bar_new();

gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_CONTINUOUS);

gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_LEFT_TO_RIGHT);

gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(uosprs_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)uosprs_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosprs_r_v_knopka(GtkWidget *widget,class uosprs_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosprs_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosprs_r_data *data)
{
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}
/***************/
/*реквизиты*/
/*****************/
void uosrs_rek(short dr,short mr, short gr,
short dd,short md,short gd,
const char *grupa,
double sumar,const char *inv_nomer,
const char *kodop,
const char *shetuh,
int metkabr,
FILE *ff)
{
if(metkabr == 0)
 fprintf(ff,"%s\n",gettext("Расчёт с использованием остаточной балансовой стоимости"));
if(metkabr == 1)
 fprintf(ff,"%s\n",gettext("Расчёт с использованием начальной балансовой стоимости"));
 
fprintf(ff,"%s:%d.%d.%d\n",gettext("Дата балансовой стоимости"),dr,mr,gr);
if(dd != 0)
 fprintf(ff,"%s:%d.%d.%d\n",gettext("Дата создания документов"),dd,md,gd);
if(grupa[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Группа"),grupa);
fprintf(ff,"%s:%.2f\n",gettext("Сумма для распределения"),sumar);
if(inv_nomer[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Инвентарный номер"),inv_nomer);
fprintf(ff,"%s:%s\n",gettext("Код операции"),kodop); 
if(shetuh[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),shetuh);
}

/******************************************/
/******************************************/

gint uosprs_r1(class uosprs_r_data *data)
{
time_t vremn;
time(&vremn);
class iceb_clock sss(data->window);

if(data->rk->sposob_r == 0) /*пообъектно*/
  uosrs1(data);

if(data->rk->sposob_r == 1) /*в целом по группе*/
 uosrsgr(data);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/**************************************/
/*Шапка распечатки*/
/*********************************/
void uosrs_sap(int metkabr,const char *inv_nomer,FILE *ff)
{

if(inv_nomer[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Инвентарный номер"),inv_nomer);

if(metkabr == 0)
  fprintf(ff,"\
--------------------------------------------------------------------\n\
Инвентарный| Наименование основного средства |Ост. бал. |Распреде- |\n\
  номер    |                                 |стоимость | лено     |\n\
--------------------------------------------------------------------\n");
if(metkabr == 1)
  fprintf(ff,"\
--------------------------------------------------------------------\n\
Инвентарный| Наименование основного средства |Нач. бал. |Распреде- |\n\
  номер    |                                 |стоимость |  лено    |\n\
--------------------------------------------------------------------\n");

}
/**************************/
/*Запись шапки документа (если нужно) и запись в документ*/
/************************/
/*Если вернули 0-всё записано*/
int uosrs_zap(short dd,short md,short gd,
const char *kodop,
int kod_mo,
int podr,
int innom,
double bs,
class iceb_u_spisok *kod_podr_mo, /*код материально-ответственного|код подразделения*/
class iceb_u_spisok *snomdok,   /*список номеров документов*/
float dpnds, /*Процент НДС действовавший на момент создания документа*/
GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str nom_dok(""); /*Номер документа в который нужно записывать инвентарный номер*/

int nomer_par=0;
sprintf(strsql,"%d|%d",kod_mo,podr);
if((nomer_par=kod_podr_mo->find(strsql)) >= 0)
 {
  nom_dok.new_plus(snomdok->ravno(nomer_par));
 }
else
 {
  
  kod_podr_mo->plus(strsql);

  class iceb_lock_tables kkk("LOCK TABLE Uosdok WRITE,icebuser READ");

  memset(strsql,'\0',sizeof(strsql));

  uosgetnd(gd,1,&nom_dok,wpredok);

//  nom_dok.new_plus(strsql);
  snomdok->plus(nom_dok.ravno());

  sprintf(strsql,"insert into Uosdok (datd,tipz,kodop,kontr,nomd,podr,kodol,prov,podt,ktoz,vrem,pn) \
values ('%04d-%02d-%02d',%d,'%s','%s','%s',%d,%d,%d,%d,%d,%ld,%.2f)",
  gd,md,dd,1,kodop,"00",nom_dok.ravno(),podr,kod_mo,0,0,iceb_getuid(wpredok),time(NULL),dpnds);

  if(iceb_sql_zapis(strsql,0,0,wpredok) != 0)
   return(1);
 }
 
sprintf(strsql,"insert into Uosdok1 (datd,tipz,podt,innom,nomd,podr,kodol,kol,bs,kodop,ktoz,vrem) \
values ('%04d-%02d-%02d',%d,%d,%d,'%s',%d,%d,%d,%.2f,'%s',%d,%ld)",
gd,md,dd,1,0,innom,nom_dok.ravno(),podr,kod_mo,0,bs,kodop,iceb_getuid(wpredok),time(NULL));

return(iceb_sql_zapis(strsql,0,0,wpredok));
}



/***********************************************/
/*Расчет пообектный*/
/************************/
void uosrs1(class uosprs_r_data *data)

{
char    strsql[512];
int     kolstr=0;
float kolstr1=0;
SQL_str row;
FILE    *ff,*fftmp;
char    imaf[40],imaftmp[40];;
SQLCURSOR cur;
int     i=0;
char    naim[512];

sprintf(strsql,"select innom,naim from Uosin");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  return;
 }

sprintf(imaf,"uosrs%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

short dr,mr,gr;
iceb_u_rsdat(&dr,&mr,&gr,data->rk->datar.ravno(),1);
short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,data->rk->datad.ravno(),1);

fprintf(ff,"%s\n",organ);
uosrs_rek(dr,mr,gr,dd,md,gd,data->rk->grup.ravno(),data->rk->suma.ravno_atof(),data->rk->innom.ravno(),data->rk->kodop.ravno(),data->rk->shetuh.ravno(),data->rk->baza_r,ff);

sprintf(imaftmp,"uosrs.tmp");
if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  return;
 }

int podr=0;
int kodotl=0;
int innom=0;
double obs; 
double sumanbs=0.; //Сумма начально балансовой стоимости всех основных средств
double sumaobs=0.; //Сумма остаточной балансовой стоимости всех основных средств

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->innom.ravno(),row[0],0,0) != 0)
   continue;
  innom=atoi(row[0]);
  
  if((i=poiinw(innom,dr,mr,gr,&podr,&kodotl,data->window)) != 0)
   {
    fprintf(ff,"%s %d %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),innom,
    gettext("не числится по дате"),mr,gr,
    gettext("Код возврата"),i);
    continue;
   }   
  class poiinpdw_data rekin;
  if((i=poiinpdw(innom,mr,gr,&rekin,data->window)) != 0)
   {
    fprintf(ff,"%s %d %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),innom,
    gettext("не найдено настройки на дату"),mr,gr,
    gettext("Код возврата"),i);

    continue;

   }  

  if(iceb_u_proverka(data->rk->shetuh.ravno(),rekin.shetu.ravno(),0,0) != 0)
   continue;

  if(iceb_u_SRAV(rekin.hna.ravno(),data->rk->grup.ravno(),0) != 0 )
   {
    fprintf(ff,"%d %s %s %s.\n",innom,
    gettext("Инвентарный номер в группе"),rekin.hna.ravno(),
    "Пропускаем");
    continue;
   }

  class bsizw_data bal_st;
  bsizw(innom,podr,dr,mr,gr,&bal_st,NULL,data->window);

  fprintf(ff,"\n\
Инвентарный номер %d %s\n",innom,row[1]);

  fprintf(ff,"\
Стартовая балансовая стоимость %.2f\n",bal_st.sbs);

  fprintf(ff,"\
Добавленная балансовая стоимость %.2f\n",bal_st.bs);

  fprintf(ff,"\
Стартовый износ %.2f\n",bal_st.siz);
  fprintf(ff,"\
Добавленный износ %.2f\n",bal_st.iz);
  fprintf(ff,"\
Амортизация %.2f\n",bal_st.iz1);
  obs=bal_st.sbs+bal_st.bs-bal_st.siz-bal_st.iz-bal_st.iz1;
  sumaobs+=obs;
  fprintf(ff,"Остаточная балансовая стоимость = %.2f *********\n",obs);
/************
  if(data->rk->baza_r == 0)
   if(obs < 0.009)
    continue;  
****************/
  sumanbs+=bal_st.sbs+bal_st.bs;

  fprintf(fftmp,"%d|%d|%s|%d|\n",innom,podr,row[1],kodotl);
 }

iceb_podpis(ff,data->window);
fclose(ff);
fclose(fftmp);

if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  return;
 }

unlink(imaftmp);

char imaftb[50];
sprintf(imaftb,"uosrstb%d.lst",getpid());
if((ff = fopen(imaftb,"w")) == NULL)
 {
  iceb_er_op_fil(imaftb,"",errno,data->window);
  return;
 }

fprintf(ff,"%s\n",organ);
uosrs_rek(dr,mr,gr,dd,md,gd,data->rk->grup.ravno(),data->rk->suma.ravno_atof(),data->rk->innom.ravno(),data->rk->kodop.ravno(),data->rk->shetuh.ravno(),data->rk->baza_r,ff);

if(data->rk->baza_r == 1)
 fprintf(ff,"\nОбщая начальная балансовая стоимость=%.2f\n",sumanbs);
if(data->rk->baza_r == 0)
 fprintf(ff,"\nОбщая остаточная балансовая стоимость=%.2f\n",sumaobs);


double kofrnbs=0.;
if(sumanbs != 0.)
  kofrnbs=data->rk->suma.ravno_atof()/sumanbs;

if(data->rk->baza_r == 1)
 fprintf(ff,"Коэффициент распределения для начальной балансовой стоимости %.2f/%.2f=%f\n",data->rk->suma.ravno_atof(),sumanbs,kofrnbs);

double kofrobs=0.;
if(sumaobs != 0.)
  kofrobs=data->rk->suma.ravno_atof()/sumaobs;

if(data->rk->baza_r == 0)
 fprintf(ff,"Коэффициент распределения для остаточной балансовой стоимости %.2f/%.2f=%f\n",data->rk->suma.ravno_atof(),sumaobs,kofrobs);

uosrs_sap(data->rk->baza_r,data->rk->innom.ravno(),ff);

double itog1=0.,itog2=0.,itog3=0.,itog4=0.;
double ras1,ras2;
class iceb_u_spisok kod_podr_mo;
class iceb_u_spisok snomdok;
while(fgets(strsql,sizeof(strsql),fftmp) != NULL)
 {

  innom=atoi(strsql);
  iceb_u_polen(strsql,&podr,2,'|');
  iceb_u_polen(strsql,naim,sizeof(naim),3,'|');
  iceb_u_polen(strsql,&kodotl,4,'|');
   
  class bsizw_data bal_st;
  bsizw(innom,podr,dr,mr,gr,&bal_st,NULL,data->window);

//  fprintf(ff,"Для начальной стоимости %f*%f=%f\n",sbs,kofrnbs,sbs*kofrnbs);

  obs=bal_st.sbs+bal_st.bs-bal_st.siz-bal_st.iz-bal_st.iz1;
//  fprintf(ff,"Для остаточной балансовой стоимости %f*%f=%f\n",obs,kofrobs,obs*kofrobs);

  ras1=(bal_st.sbs+bal_st.bs)*kofrnbs;
  ras1=iceb_u_okrug(ras1,0.01);

  ras2=obs*kofrobs;
  ras2=iceb_u_okrug(ras2,0.01);
 
  if(data->rk->baza_r == 0)
   fprintf(ff,"%11d %-*.*s %10.2f %10.2f\n",
   innom,
   iceb_u_kolbait(33,naim),iceb_u_kolbait(33,naim),naim,
   obs,ras2);
 
  if(data->rk->baza_r == 1)
   fprintf(ff,"%11d %-*.*s %10.2f %10.2f\n",
   innom,
   iceb_u_kolbait(33,naim),iceb_u_kolbait(33,naim),naim,
   bal_st.sbs+bal_st.bs,ras1);
  if(gd != 0)
   {
    double bs=0.;
    if(data->rk->baza_r == 0)
      bs=ras2;
    if(data->rk->baza_r == 1)
      bs=ras1;
    if(bs != 0.)
     uosrs_zap(dd,md,gd,data->rk->kodop.ravno(),kodotl,podr,innom,bs,&kod_podr_mo,&snomdok,data->rk->dpnds,data->window);
   }
  itog1+=bal_st.sbs+bal_st.bs;
  itog2+=ras1;
  itog3+=obs;
  itog4+=ras2;
 }

fprintf(ff,"\
--------------------------------------------------------------------\n");
if(data->rk->baza_r == 0)
 fprintf(ff,"%*s %10.2f %10.2f\n",iceb_u_kolbait(45,"Итого"),"Итого",itog3,itog4);
if(data->rk->baza_r == 1)
 fprintf(ff,"%*s %10.2f %10.2f\n",iceb_u_kolbait(45,"Итого"),"Итого",itog1,itog2);


iceb_podpis(ff,data->window);
fclose(ff);
fclose(fftmp);

if(snomdok.kolih() > 0)
 {
  sprintf(strsql,"%s:%d",gettext("Количество документов"),snomdok.kolih());
  iceb_menu_soob(strsql,data->window);  
  
 }


data->rk->imaf.plus(imaftb);
data->rk->naim.plus(gettext("Пропорциональное распределение сумм"));

data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Протокол хода расчёта"));

}

/***********************************************/
/*Расчет по группе*/
/************************/
void uosrsgr(class uosprs_r_data *data)
{
char strsql[512];
char bros[512];
char bros1[512];
FILE *ff;
char imaf[64];
int  kolstr=0;
int i=0;
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select innom,naim from Uosin");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  return;
 }


sprintf(bros1,"Стартовая балансовая стоимость для группы %s",data->rk->grup.ravno());

if(iceb_poldan(bros1,bros,"uosnast.alx",data->window) != 0)
 {
  sprintf(strsql,"%s %s !",gettext("Не найдена стартовая балансовая стоимость для группы"),data->rk->grup.ravno());
  iceb_menu_soob(strsql,data->window);
  return;
 }

double sbs=iceb_u_atof(bros);

sprintf(bros1,"Стартовый износ для группы %s",data->rk->grup.ravno());
iceb_poldan(bros1,bros,"uosnast.alx",data->window);
double siz=iceb_u_atof(bros);
short dr,mr,gr;
iceb_u_rsdat(&dr,&mr,&gr,data->rk->datar.ravno(),1);
double bs=0.,iz=0.,iz1=0.;
bsiz23w(data->rk->grup.ravno(),dr,mr,gr,&bs,&iz,&iz1,NULL,data->window);

sprintf(imaf,"uosrsg%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

char imaf_prot[50];
sprintf(imaf_prot,"uosrsgp%d.lst",getpid());
FILE *ffprot;
if((ffprot = fopen(imaf_prot,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_prot,"",errno,data->window);
  return;
 }

short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,data->rk->datad.ravno(),1);

fprintf(ff,"%s\n",organ);
uosrs_rek(dr,mr,gr,dd,md,gd,data->rk->grup.ravno(),data->rk->suma.ravno_atof(),data->rk->innom.ravno(),data->rk->kodop.ravno(),data->rk->shetuh.ravno(),data->rk->baza_r,ff);

fprintf(ff,"Группа %s Стартовая б.c. %.2f  Стартовый износ %.2f\n\
Бал. ст. %.2f Износ %.2f \n\
Амортизация %.2f\n",data->rk->grup.ravno(),sbs,siz,bs,iz,iz1);

fprintf(ffprot,"%s\n",organ);
uosrs_rek(dr,mr,gr,dd,md,gd,data->rk->grup.ravno(),data->rk->suma.ravno_atof(),data->rk->innom.ravno(),data->rk->kodop.ravno(),data->rk->shetuh.ravno(),data->rk->baza_r,ffprot);

fprintf(ffprot,"Группа %s Стартовая б.c. %.2f  Стартовый износ %.2f\n\
Бал. ст. %.2f Износ %.2f \n\
Амортизация %.2f\n",data->rk->grup.ravno(),sbs,siz,bs,iz,iz1);

int podr=0;
int innom;
float kolstr1=0;
int kodotl=0;
double obs=0.;

double kofrnbs=0.;
double kofrobs=0.;

if(sbs != 0.)
 kofrnbs=data->rk->suma.ravno_atof()/sbs;
double ost_st=sbs+bs-siz-iz-iz1;
if(ost_st != 0.)
 kofrobs=data->rk->suma.ravno_atof()/ost_st;

fprintf(ff,"Сумма для распределения=%.2f\n",data->rk->suma.ravno_atof());

fprintf(ff,"Коэффициент распределения для начальной балансовой стоимости %.2f/%.2f=%f\n",data->rk->suma.ravno_atof(),sbs,kofrnbs);
fprintf(ff,"Коэффициент распределения для остаточной балансовой стоимости %.2f/%.2f=%f\n",data->rk->suma.ravno_atof(),ost_st,kofrobs);

uosrs_sap(data->rk->baza_r,data->rk->innom.ravno(),ff);
  
double itog1=0.,itog2=0.,itog3=0.,itog4=0.;
double ras1,ras2;
class iceb_u_spisok kod_podr_mo;
class iceb_u_spisok snomdok;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s\n",row[0]);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->innom.ravno(),row[0],0,0) != 0)
   continue;
  innom=atol(row[0]);
  if((i=poiinw(innom,(short)31,mr,gr,&podr,&kodotl,data->window)) != 0)
   {

    fprintf(ffprot,"%s %d %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),innom,
    gettext("не числится по дате"),mr,gr,
    gettext("Код возврата"),i);
    continue;
   }   
  class poiinpdw_data rekin;
  if((i=poiinpdw(innom,mr,gr,&rekin,data->window)) != 0)
   {
    fprintf(ffprot,"%s %d %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),innom,
    gettext("не найдено настройки на дату"),mr,gr,
    gettext("Код возврата"),i);

    continue;

   }  
  if(iceb_u_proverka(data->rk->shetuh.ravno(),rekin.shetu.ravno(),0,0) != 0)
   continue;

  if(iceb_u_SRAV(rekin.hna.ravno(),data->rk->grup.ravno(),0) != 0 )
   {
    
    fprintf(ffprot,"%d %s %s %s.\n",innom,
    gettext("Инвентарный номер в группе"),rekin.hna.ravno(),
    "Пропускаем");
    
    continue;
   }

  class bsizw_data bal_st;
  
  bsizw(innom,podr,dr,mr,gr,&bal_st,NULL,data->window);


  ras1=bal_st.sbs*kofrnbs;
  ras1=iceb_u_okrug(ras1,0.01);

  obs=sbs+bal_st.bs-bal_st.siz-bal_st.iz-bal_st.iz1;
  ras2=obs*kofrobs;
  ras2=iceb_u_okrug(ras2,0.01);

  if(data->rk->baza_r == 0)
   fprintf(ff,"%11d %-*.*s %10.2f %10.2f\n",
   innom,
   iceb_u_kolbait(33,row[1]),iceb_u_kolbait(33,row[1]),row[1],
   obs,ras2);

  if(data->rk->baza_r == 1)
  fprintf(ff,"%11d %-*.*s %10.2f %10.2f\n",
  innom,
  iceb_u_kolbait(33,row[1]),iceb_u_kolbait(33,row[1]),row[1],
  sbs,ras1);

  if(gd != 0)
   {
    double bs=0.;
    if(data->rk->baza_r == 0)
      bs=ras2;
    if(data->rk->baza_r == 1)
      bs=ras1;
    if(bs != 0.)
     uosrs_zap(dd,md,gd,data->rk->kodop.ravno(),kodotl,podr,innom,bs,&kod_podr_mo,&snomdok,data->rk->dpnds,data->window);
   }

  itog1+=sbs;
  itog2+=ras1;
  itog3+=obs;
  itog4+=ras2;

 }

fprintf(ff,"\
--------------------------------------------------------------------\n");
if(data->rk->baza_r == 0)
  fprintf(ff,"%*s %10.2f %10.2f\n",iceb_u_kolbait(45,"Итого"),"Итого",itog3,itog4);
if(data->rk->baza_r == 1)
  fprintf(ff,"%*s %10.2f %10.2f\n",iceb_u_kolbait(45,"Итого"),"Итого",itog1,itog2);

iceb_podpis(ff,data->window);
fclose(ff);
iceb_podpis(ffprot,data->window);
fclose(ffprot);

if(snomdok.kolih() > 0)
 {
  sprintf(strsql,"%s:%d",gettext("Количество документов"),snomdok.kolih());
  iceb_menu_soob(strsql,data->window);  
 }


data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Пропорциональное распределение сумм"));

data->rk->imaf.plus(imaf_prot);
data->rk->naim.plus(gettext("Протокол хода расчёта"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);

}
