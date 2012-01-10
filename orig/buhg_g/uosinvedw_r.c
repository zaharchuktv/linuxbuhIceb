/*$Id: uosinvedw_r.c,v 1.15 2011-02-21 07:35:58 sasa Exp $*/
/*11.01.2010	13.01.2008	Белых А.И.	uosinvedw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "uosinvedw.h"

class drag_met
 {
  public:
  class iceb_u_spisok dm_ei; /*Список драг металлов-единиц измерения*/
  class iceb_u_double ves_dm; /*Массив для итоговых весов по горизонтали*/
  class iceb_u_double ves_dmv; /*Массив для итоговых весов по вертикали*/
  class iceb_u_int sp_in; /*Список инвентарных номеров*/
  class iceb_u_double ves_hax; /*Шахматка основное средство-вес*/
 };

class uosinvedw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class uosinvedw_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  uosinvedw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosinvedw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosinvedw_r_data *data);
gint uosinvedw_r1(class uosinvedw_r_data *data);
void  uosinvedw_r_v_knopka(GtkWidget *widget,class uosinvedw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern iceb_u_str shrift_ravnohir;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;
extern char *imabaz;

int uosinvedw_r(class uosinvedw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class uosinvedw_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать инвентаризационные ведомости"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uosinvedw_r_key_press),&data);

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

repl.plus(gettext("Распечатать инвентаризационные ведомости"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(uosinvedw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)uosinvedw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosinvedw_r_v_knopka(GtkWidget *widget,class uosinvedw_r_data *data)
{
/*printf("uosinvedw_r_v_knopka\n");*/
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosinvedw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosinvedw_r_data *data)
{
/* printf("uosinvedw_r_key_press\n");*/
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}
/****************************************/
/*Распечатка по драгоценным металлам*/
/****************************************/
int uosinved_rdm(short di,short mi,short gi,
class drag_met *dragm,
class iceb_u_str *imaf,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
FILE *ff_dm;
char imaf_dm[56];
sprintf(imaf_dm,"indm%d.lst",getpid());
imaf->new_plus(imaf_dm);
if((ff_dm = fopen(imaf_dm,"w")) == NULL)
 {
   iceb_er_op_fil(imaf_dm,"",errno,wpredok);
  return(1);
 }

iceb_u_zagolov(gettext("Инвентаризационная опись драгоценных металлов"),0,0,0,di,mi,gi,organ,ff_dm);
class iceb_u_str liniq;

int kolih_dm=dragm->dm_ei.kolih();

liniq.plus("-----------------------------------------------------------------------");
for(int ii=0; ii < kolih_dm; ii++)
 {
  if(dragm->ves_dm.ravno(ii) == 0.)
   continue;
  liniq.plus("-----------");
 }
fprintf(ff_dm,"%s\n",liniq.ravno());
  
fprintf(ff_dm,gettext("\
Номер|Инвентарный|        Наименование          | Модель   |Заводской |"));
char drm[24];
char naim_dm[112];
for(int ii=0; ii < kolih_dm; ii++)
 {
  if(dragm->ves_dm.ravno(ii) == 0.)
   continue;
  memset(naim_dm,'\0',sizeof(naim_dm));
  iceb_u_polen(dragm->dm_ei.ravno(ii),drm,sizeof(drm),1,'|');
  sprintf(strsql,"select naik from Uosdm where kod=%d",atoi(drm));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     strncpy(naim_dm,row[0],sizeof(naim_dm)-1);    
  sprintf(strsql,"%s %s",drm,naim_dm);
  fprintf(ff_dm,"%-*.*s|",iceb_u_kolbait(10,strsql),iceb_u_kolbait(10,strsql),strsql);
 }
fprintf(ff_dm,"\n");

fprintf(ff_dm,gettext("\
 п-п | номер     |                              |          |  номер   |"));
/*
12345 12345678901 123456789012345678901234567890 1234567890 1234567890
*/
for(int ii=0; ii < kolih_dm; ii++)
 {
  if(dragm->ves_dm.ravno(ii) == 0.)
   continue;
  iceb_u_polen(dragm->dm_ei.ravno(ii),drm,sizeof(drm),2,'|');
  fprintf(ff_dm,"%-*.*s|",iceb_u_kolbait(10,drm),iceb_u_kolbait(10,drm),drm);
 }

fprintf(ff_dm,"\n%s\n",liniq.ravno());
char naim[512];
char model[112];
char zav_nomer[112];
int nomer_pp_dm=0;
for(int nom_in=0; nom_in < dragm->sp_in.kolih(); nom_in++)
 {
  if(dragm->ves_dmv.ravno(nom_in) == 0.)
   continue;
  memset(naim,'\0',sizeof(naim));
  memset(model,'\0',sizeof(model));
  memset(zav_nomer,'\0',sizeof(zav_nomer));
 
  sprintf(strsql,"select naim,model,zavnom from Uosin where innom=%d",dragm->sp_in.ravno(nom_in));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    strncpy(naim,row[0],sizeof(naim)-1);
    strncpy(model,row[1],sizeof(model)-1);
    strncpy(zav_nomer,row[2],sizeof(zav_nomer)-1);
   }
  fprintf(ff_dm,"%5d|%-11d|%-*.*s|%-*.*s|%-*.*s|",
  ++nomer_pp_dm,
  dragm->sp_in.ravno(nom_in),
  iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,
  iceb_u_kolbait(10,model),iceb_u_kolbait(10,model),model,
  iceb_u_kolbait(10,zav_nomer),iceb_u_kolbait(10,zav_nomer),zav_nomer);
  for(int nom_dm=0; nom_dm < kolih_dm; nom_dm++)
   {
    if(dragm->ves_dm.ravno(nom_dm) == 0.)
     continue;
    fprintf(ff_dm,"%10.10g|",dragm->ves_hax.ravno(kolih_dm*nom_in+nom_dm));
   }
  fprintf(ff_dm,"\n");
  
 }

fprintf(ff_dm,"%s\n",liniq.ravno());
fprintf(ff_dm,"%*s|",iceb_u_kolbait(70,gettext("Итого")),gettext("Итого"));
for(int ii=0; ii < kolih_dm; ii++)
 {
  if(dragm->ves_dm.ravno(ii) == 0.)
     continue;
  fprintf(ff_dm,"%10.10g|",dragm->ves_dm.ravno(ii));
 }
fprintf(ff_dm,"\n%s\n",liniq.ravno());

iceb_podpis(ff_dm,wpredok);
fclose(ff_dm);

return(0);

}

/******************/
/*Шапка распечатки*/
/******************/

void	uosinsap(int *kollist,int *kolst,FILE *ff)
{

*kollist+=1;
*kolst+=5;
fprintf(ff,"%65s %s N%d\n"," ",gettext("Лист"),*kollist);
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Nп-п|Инвентарный|        Наименование          |Год |Дата ввода|  Модель  |   Бухучет     | Факт. наличие |    Разница    |\n\
    |  номер    |                              |вып.|в эксплуа.|          |Кол.|Стоимость |Кол.|Стоимость |Кол.|Стоимость |\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");
}
/***************/
/*Счетчик строк*/
/****************/
void  uosinvedsh(int *kolst,int *kollist,FILE *ff)
{

*kolst+=1;
if(*kolst >= kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kolst=0;
  uosinsap(kollist,kolst,ff);
 }
}
/**************************************/
/*Шапка для бюджетных организиций*/
/*********************************/

void uosinved_sbu(short dost,short most,short gost,int *kollist,int *kolst,FILE *ff)
{
*kollist+=1;
*kolst+=6;

fprintf(ff,"%80s %s N%d\n"," ",gettext("Лист"),*kollist);
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 N  |        Наименование          |Год |            Номер             |По состоянию на %02d.%02d.%04dг.   |Отметка о  |\n\
з/п |                              |    |Инвентарный|заводсь- |паспорта| Факт. наличие |   Бухучет     |выбытии    |\n\
    |                              |    |           |  кий    |        |Кол.|Стоимость |Кол.|Стоимость |           |\n",
dost,most,gost);
//   123456789012345678901234567890 1234 12345678901 123456789 12345678 1234 1234567890 1234 1234567890
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");

}
/***************/
/*Счетчик строк для бюджетной распечатки*/
/****************/
void  uosinvedsh_bu(short di,short mi,short gi,
int *kolst,int *kollist,FILE *ff)
{

*kolst+=1;
if(*kolst >= kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kolst=1;
  uosinved_sbu(di,mi,gi,kollist,kolst,ff);
 }
}
/****************************************/
/*Первый лист бюджетного документа*/
/***********************************/

void uosinved_pl(short di,short mi,short gi,
char *ikod,
int *kollist,
int *kolstr,
FILE *ffbu)
{
*kollist=*kolstr=0;

fprintf(ffbu,"\
                                                            Додаток 1\n\
                                                            до інструкції з інвентаризації матеріальних цінностей,\n\
                                                            розрахунків та інших статей балансу бюджених установ.\n");
fprintf(ffbu,"%s\n",organ);
fprintf(ffbu,"Індентифікаційний код за ЄДРПОУ %s\n",ikod);
                                                                          
fprintf(ffbu,"                                        ІНВЕТАРИЗАЦІЙНИЙ ОПИС ОСНОВНИХ ЗАСОБІВ\n");
fprintf(ffbu,"                                                  %02d.%02d.%04d\n",di,mi,gi);
fprintf(ffbu,"Основні засоби_______________________________________________________________________________\n\n");
fprintf(ffbu,"Місце знаходження____________________________________________________________________________\n");

fprintf(ffbu,"\
                          РАСПИСКА\n\
 К началу проведения инвентаризации все расходные и приходные документы\n\
на ценности сданы в бухгалтерию и все ценности, поступившие на мою\n\
(нашу) ответственность, оприходованы, а выбывшие списаны в расход.\n\n");

fprintf(ffbu,"\
______________________________   _____________________________   _______________________________\n\
   (посада, підпис)                   (посада, підпис)               (посада, підпис)\n\n\
______________________________   _____________________________   _______________________________\n\
(призвище, ім'я, по батькові)    (призвище, ім'я, по батькові)   (призвище, ім'я, по батькові)\n\n");

fprintf(ffbu,"На підставі (розпорядження) від \"____\"_____________________р.\n\n");
fprintf(ffbu,"Виконано знімання фактичних залишків__________________________________________\n\n");
fprintf(ffbu,"що значаться на обліку станом на \"____\"____________________р.\n\n");
fprintf(ffbu,"Інвентаризація: розпочата \"___\"____________________р.\n\n");
fprintf(ffbu,"                закінчена \"___\"____________________p.\n");

fprintf(ffbu,"\n\nПри інвентаризіції встановлено наступне:\n");
*kolstr+=32;
uosinved_sbu(di,mi,gi,kollist,kolstr,ffbu);

}
/*******************************/
/*Концовка отчёта для бюджета*/
/*******************************/
void uosinved_end_bu(FILE *ffbu)
{
fprintf(ffbu,"\
Разом за описом : а) кількість порядкових номерів____________________________\n\
                                                          (прописом)\n\
                  б) загальна кількість лдиниць, фактично____________________\n\
                                                          (прописом)\n\
                  в) на суму, грн., фактично_________________________________\n\
                                                          (прописом)\n\n");
fprintf(ffbu,"\
Голова комісії ______________________  ___________________  ______________________________\n\
                  (посада)                 (підпис)         (призвище, ім'я, по батькові)\n\n\
Члени комісії: ______________________  ___________________  ______________________________\n\
                  (посада)                 (підпис)         (призвище, ім'я, по батькові)\n\n\
               ______________________  ___________________  ______________________________\n\
                  (посада)                 (підпис)         (призвище, ім'я, по батькові)\n\n\
              ______________________  ___________________  ______________________________\n\
                  (посада)                 (підпис)         (призвище, ім'я, по батькові)\n\n\
 Усі цінності, пойменовані в цьому інвентаризаційному описі з N__________ до N___________,\n\
перевірені комісією в натурі за моєї (нашої) присутності та внесені в опис, у зв'язку з чим\n\
претензій до інвентаризаційної комісії не маю (не маємо). Цінності, перелічені в описі,\n\
знаходяться на моєму (нашому) відповідальному зберіганні.\n\n\
Особа(и), відповідальна(і) за збереження основних засобів:\n\n\
\"____\"_____________________р.\n\n\
____________________________________________________________________________________________\n\n\
 Вказані у данному описі дані превірив ____________________  ______________________\n\
                                             (посада)            (підпис)\n\n\
\"____\"_____________________р.\n");

}
/*****************************/
/*Шапка */
/******************************/
void uosinved_sap(short mi,short gi,int podr,int kodotl,int *kollist,int *kolst,FILE *ff,GtkWidget *wpredok)
{
char strsql[512];
char naipod[112];
char fiootv[112];
SQL_str row;
class SQLCURSOR curr;
memset(fiootv,'\0',sizeof(fiootv));
if(kodotl != 0)
 {
  sprintf(strsql,"select naik from Uosol where kod=%d",kodotl);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден мат.-ответственный"),kodotl);
    iceb_menu_soob(strsql,wpredok);
   }
  else 
   strncpy(fiootv,row[0],sizeof(fiootv)-1);
 }

memset(naipod,'\0',sizeof(naipod));
if(podr != 0)
 {
  sprintf(strsql,"select naik from Uospod where kod=%d",podr);
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),podr);
    iceb_menu_soob(strsql,wpredok);
   }
  else 
    strncpy(naipod,row[0],sizeof(naipod)-1);
 }

fprintf(ff,"%s\n\n",organ);
fprintf(ff,gettext("ИНВЕНТАРИЗАЦИОННАЯ ОПИСЬ N_____ ОСНОВНЫХ СРЕДСТВ.\n"));
fprintf(ff,"%s %s\n",gettext("Подразделение"),naipod);
fprintf(ff,gettext("\
                          РАСПИСКА\n\
 К началу проведения инвентаризации все расходные и приходные документы\n\
на ценности сданы в бухгалтерию и все ценности, поступившие на мою\n\
(нашу) ответственность, оприходованы, а выбывшие списаны в расход.\n\n"));

fprintf(ff,"\n\
%s __________________________%s\n\n",
gettext("Материально-ответственное лицо (лица)"),
fiootv);

*kolst=12;
*kolst+=iceb_insfil("inventuos.alx",ff,wpredok);

char mesqc[32];
memset(mesqc,'\0',sizeof(mesqc));
iceb_mesc(mi,1,mesqc);

fprintf(ff,"\n\
%*s:%*s \"______\" %s %d%s\n\
%15s %*s \"______\" %s %d%s\n",
iceb_u_kolbait(15,gettext("Инвентаризация")),gettext("Инвентаризация"),
iceb_u_kolbait(10,gettext("начата")),gettext("начата"),
mesqc,gi,
gettext("г."),
" ",
iceb_u_kolbait(10,gettext("окончена")),gettext("окончена"),
mesqc,gi,
gettext("г."));

fprintf(ff,"\n\%s:\n",gettext("При инвентаризации установлено следующее"));

*kolst+=5;

*kollist=0;
uosinsap(kollist,kolst,ff);

}
/**********************************/
/*Итого по счёту*/
/*******************************/

void uosinved_ips(short di,short mi,short gi,
char *shetuz,
int *kolst,int *kollist,int *kolst_bu,int *kollist_bu,
int *shethiksh,
double *balstsh,
FILE *ff,FILE *ffbu)
{
char bros[512];
//Счетчик строк
uosinvedsh(kolst,kollist,ff);
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");
sprintf(bros,"%s %s",gettext("Итого по счету"),shetuz);

uosinvedsh(kolst,kollist,ff);
fprintf(ff,"%*s:%4d %10.2f\n",iceb_u_kolbait(74,bros),bros,*shethiksh,*balstsh);

uosinvedsh_bu(di,mi,gi,kolst_bu,kollist_bu,ffbu);

fprintf(ffbu,"\
--------------------------------------------------------------------------------------------------------------------\n");
sprintf(bros,"%s %s",gettext("Итого по счету"),shetuz);

uosinvedsh_bu(di,mi,gi,kolst_bu,kollist_bu,ffbu);
fprintf(ffbu,"%*s:%4s %10s %4d %10.2f\n",iceb_u_kolbait(71,bros),bros,"","",*shethiksh,*balstsh);
*balstsh=0.;
*shethiksh=0;
}

/*******************************/
/*концовка распечатки*/
/*****************************/
void uosinved_konec(short mi,short gi,char *shetuz,int *shethik,double *itogo,int *shethiksh,double *balstsh,FILE *ff,FILE *ffbu)
{
char bros[512];

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");

sprintf(bros,"%s %s",gettext("Итого по счету"),shetuz);
fprintf(ff,"%*s:%4d %10.2f\n",iceb_u_kolbait(74,bros),bros,*shethiksh,*balstsh);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%4d %10.2f\n",iceb_u_kolbait(74,gettext("Итого")),gettext("Итого"),*shethik,*itogo);


fprintf(ffbu,"\
--------------------------------------------------------------------------------------------------------------------\n");
sprintf(bros,"%s %s",gettext("Итого по счету"),shetuz);
fprintf(ffbu,"%*s:%4s %10s %4d %10.2f\n",iceb_u_kolbait(71,bros),bros,"","",*shethiksh,*balstsh);

fprintf(ffbu,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(ffbu,"%*s:%4s %10s %4d %10.2f\n",iceb_u_kolbait(71,gettext("Итого")),gettext("Итого"),"","",*shethik,*itogo);

char mesqc[32];
memset(mesqc,'\0',sizeof(mesqc));
iceb_mesc(mi,1,mesqc);

koncinvw(mesqc,gi,ff);

uosinved_end_bu(ffbu);
*balstsh=0.;
*shethik=0;
*shethiksh=0;
*itogo=0.;
}
/*************************/
/*Чтение записей по драг.металлам*/
/*********************************/
/*Если вернули 0-драг.металлы есть 1-нет*/
int uosinved_dm(int innom,class iceb_u_spisok *dm_ei,double *ves,GtkWidget *wpredok)
{
int kolih=0;
class SQLCURSOR cur;
SQL_str row;
char strsql[512];
sprintf(strsql,"select kod,ei,ves from Uosindm where innom=%d",innom);

if((kolih=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
if(kolih == 0)
 return(1);
int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s",row[0],row[1]);
  if((nomer=dm_ei->find(strsql)) < 0)
   {
    iceb_menu_soob("Не найдена нужная запись в списке!!!",wpredok);
    continue;
   }
  ves[nomer]+=atof(row[2]);
 }
return(0);
}


/******************************************/
/******************************************/

gint uosinvedw_r1(class uosinvedw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;

char imaf[56];
memset(imaf,'\0',sizeof(imaf));
short di,mi,gi;

iceb_u_rsdat(&di,&mi,&gi,data->rk->datak.ravno(),1);
char fiomatot[112];
memset(fiomatot,'\0',sizeof(fiomatot));

class SQLCURSOR curr;
if(data->rk->mat_ot.getdlinna() > 1)
 {
  if(iceb_u_pole2(data->rk->mat_ot.ravno(),',') == 0)
   {
    sprintf(strsql,"select naik from Uosol where kod=%s",data->rk->mat_ot.ravno());
    if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)
     strncpy(fiomatot,row[0],sizeof(fiomatot)-1);
   }
 }
 


//читаем код
char ikod[32];
memset(ikod,'\0',sizeof(ikod));
sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 strncpy(ikod,row[0],sizeof(ikod)-1);

class drag_met dragm;

int kolih_dm=0;
/*Читаем списко драг.металлов*/
sprintf(strsql,"select distinct kod,ei from Uosindm");
if((kolih_dm=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s",row[0],row[1]);
  dragm.dm_ei.plus(strsql);
 }
if(kolih_dm > 0)
 dragm.ves_dm.make_class(kolih_dm);


int kolstr=0;
sprintf(strsql,"select innom,god,naim,model,pasp,zavnom,datvv from Uosin order by innom asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafsort[56];
sprintf(imafsort,"inv%d.tmp",getpid());
FILE *ffsort;
if((ffsort = fopen(imafsort,"w")) == NULL)
 {
  iceb_er_op_fil(imafsort,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

float kolstr1=0;
int innom=0;
int poddz=0,kodotl=0;
char bros[312];
int kolstr_p=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6]);
  
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->innom.ravno(),row[0],0,0) != 0)
   continue;
  innom=atoi(row[0]);

  if(poiinw(innom,di,mi,gi,&poddz,&kodotl,data->window) != 0) //Произошел расход или оно пришло позже даты
    continue;
    
  sprintf(bros,"%d",poddz);
  if(iceb_u_proverka(data->rk->podr.ravno(),bros,0,0) != 0)
    continue;
  sprintf(bros,"%d",kodotl);
  if(iceb_u_proverka(data->rk->mat_ot.ravno(),bros,0,0) != 0)
    continue;
  class poiinpdw_data rekin;
  poiinpdw(innom,mi,gi,&rekin,data->window);

  if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
    continue;
  kolstr_p++;
  fprintf(ffsort,"%d|%s|%s|%s|%s|%d|%s|%s|%s|%s|\n",kodotl,rekin.shetu.ravno(),row[0],row[1],row[2],poddz,row[3],row[4],row[5],row[6]);
  dragm.sp_in.plus(row[0]);
 }
fclose(ffsort);

if(kolih_dm > 0)
 {
  dragm.ves_hax.make_class(kolih_dm*dragm.sp_in.kolih());
  dragm.ves_dmv.make_class(dragm.sp_in.kolih());
 } 

//sprintf(strsql,"sort -o  %s -t\\| -n +0 -1 +1 -2 +2 -3 %s",imafsort,imafsort);
sprintf(strsql,"sort -o  %s -t\\| -n -k1,2 -k2,3 -k3,4 %s",imafsort,imafsort);
system(strsql);

class iceb_u_spisok sp_shet;
class iceb_u_double sum_shet;
class iceb_u_double kol_shet;
char shetuz[56];
char shetu[56];
memset(shetuz,'\0',sizeof(shetuz));
short dvv,mvv,gvv;
int kod_otv_z=-1; //может равняться нулю
int nomer_sheta=0;
kolstr1=0.;
double balst=0.;
char god[16];
char naim[512];
char		model[64];
char zav_nomer[32];
char nom_pasp[512];
int		shethik=0,shethiksh=0;
double itogo=0.;
double balstsh=0.;
int		kolst=0,kollist=0;
char naimshet[512];

if((ffsort = fopen(imafsort,"r")) == NULL)
 {
  iceb_er_op_fil(imafsort,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafbu[32];

sprintf(imafbu,"invb%d.lst",getpid());
FILE *ffbu;
if((ffbu = fopen(imafbu,"w")) == NULL)
 {
  iceb_er_op_fil(imafbu,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int kollist_bu=0;
int kolst_bu=0;

//uosinved_pl(di,mi,gi,ikod,&kollist_bu,&kolst_bu,ffbu);
sprintf(imaf,"inv%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_ipsh[56];
FILE *ffipsh;
sprintf(imaf_ipsh,"dod%d.lst",getpid());
if((ffipsh = fopen(imaf_ipsh,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_ipsh,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

in_d12_startw(ffipsh);

while(fgets(strsql,sizeof(strsql),ffsort) != NULL)
 {
  iceb_pbar(data->bar,kolstr_p,++kolstr1);    

  iceb_u_polen(strsql,&kodotl,1,'|');

  iceb_u_polen(strsql,shetu,sizeof(shetu),2,'|');
  iceb_u_polen(strsql,&innom,3,'|');
  iceb_u_polen(strsql,god,sizeof(god),4,'|');
  iceb_u_polen(strsql,naim,sizeof(naim),5,'|');

  iceb_u_polen(strsql,&poddz,6,'|');
  iceb_u_polen(strsql,model,sizeof(model),7,'|');
  iceb_u_polen(strsql,nom_pasp,sizeof(nom_pasp),8,'|');
  iceb_u_polen(strsql,zav_nomer,sizeof(zav_nomer),9,'|');
  iceb_u_polen(strsql,bros,sizeof(bros),10,'|');
  iceb_u_rsdat(&dvv,&mvv,&gvv,bros,2);

  if(kod_otv_z != kodotl)
   {
    if(kod_otv_z != -1)
     {
      uosinved_konec(mi,gi,shetuz,&shethik,&itogo,&shethiksh,&balstsh,ff,ffbu);

      fprintf(ff,"\f");
      fprintf(ffbu,"\f");
      
     }
    uosinved_pl(di,mi,gi,ikod,&kollist_bu,&kolst_bu,ffbu);
    uosinved_sap(mi,gi,poddz,kodotl,&kollist,&kolst,ff,data->window);
    
    kod_otv_z=kodotl;
    
   }

  if(iceb_u_SRAV(shetu,shetuz,0) != 0)
   {
    if(shetuz[0] != '\0')
     {
      if(balstsh != 0.)
        uosinved_ips(di,mi,gi,shetuz,&kolst,&kollist,&kolst_bu,&kollist_bu,&shethiksh,&balstsh,ff,ffbu);

     }
    memset(naimshet,'\0',sizeof(naimshet));    
    sprintf(strsql,"select nais from Plansh where ns='%s'",shetu);
    if(sql_readkey(&bd,strsql,&row,&curr) > 0)
      strncpy(naimshet,row[0],sizeof(naimshet)-1);

    //Счетчик строк
    uosinvedsh(&kolst,&kollist,ff);
    fprintf(ff,"%s:%s %s\n",gettext("Счет"),shetu,naimshet);

    uosinvedsh_bu(di,mi,gi,&kolst_bu,&kollist_bu,ffbu);
    fprintf(ffbu,"%s:%s %s\n",gettext("Счет"),shetu,naimshet);
  
    shethiksh=0;
    balstsh=0.;
    strcpy(shetuz,shetu);
   }

  class bsizw_data bal_st;
  bsizw(innom,poddz,di,mi,gi,&bal_st,NULL,data->window);

  if(data->rk->metka_bal_st == 1)
    balst=bal_st.bs+bal_st.sbs;
  if(data->rk->metka_bal_st == 0)
    balst=bal_st.bsby+bal_st.sbsby;
//  if(balst == 0.)
//    continue;

  //Счетчик строк
  uosinvedsh(&kolst,&kollist,ff);

  fprintf(ff,"%4d %-11d %-*.*s %4s %02d.%02d.%04d %-*.*s|%4s|%10.2f|    |          |    |          |\n",
  ++shethik,innom,
  iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,
  god,dvv,mvv,gvv,
  iceb_u_kolbait(10,model),iceb_u_kolbait(10,model),model,
  "1",balst);

  uosinvedsh_bu(di,mi,gi,&kolst_bu,&kollist_bu,ffbu);

  fprintf(ffbu,"%4d %-*.*s %4s %-11d %-*.*s %-*.*s|%4s|%10s|%4s|%10.2f|\n",
  shethik,
  iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,
  god,
  innom,
  iceb_u_kolbait(9,zav_nomer),iceb_u_kolbait(9,zav_nomer),zav_nomer,
  iceb_u_kolbait(8,nom_pasp),iceb_u_kolbait(8,nom_pasp),nom_pasp,
  "","","1",balst);

  if(iceb_u_strlen(naim) > 30)
   {
    //Счетчик строк
    uosinvedsh(&kolst,&kollist,ff);
    fprintf(ff,"%4s %-11s %s\n"," "," ",iceb_u_adrsimv(30,naim));

    uosinvedsh_bu(di,mi,gi,&kolst_bu,&kollist_bu,ffbu);
    fprintf(ffbu,"%4s %s\n","",iceb_u_adrsimv(30,naim));

   }

  //Счетчик строк
  uosinvedsh(&kolst,&kollist,ff);
  fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");
  if(kolih_dm > 0)
   {
    double ves[kolih_dm];
    memset(ves,'\0',sizeof(ves));        
    if(uosinved_dm(innom,&dragm.dm_ei,ves,data->window) == 0)
     {

      int nomer_os=dragm.sp_in.find(innom);      
      for(int ii=0; ii < kolih_dm; ii++)
       {
        dragm.ves_dm.plus(ves[ii],ii);
        dragm.ves_hax.plus(ves[ii],kolih_dm*nomer_os+ii);
        dragm.ves_dmv.plus(ves[ii],nomer_os);
       }

     }    
   }
  if((nomer_sheta=sp_shet.find(shetu)) < 0)
   sp_shet.plus(shetu);
  kol_shet.plus(1,nomer_sheta);
  sum_shet.plus(balst,nomer_sheta);
  
  itogo+=balst;
  balstsh+=balst;
  shethiksh++;
 }
uosinved_konec(mi,gi,shetuz,&shethik,&itogo,&shethiksh,&balstsh,ff,ffbu);

/*Распечатка формы додаток 12*/
in_d12_itw(&sp_shet,&kol_shet,&sum_shet,ffipsh,data->window);

iceb_insfil("in_d12_end.alx",ffipsh,data->window);

iceb_podpis(ff,data->window);
iceb_podpis(ffbu,data->window);
iceb_podpis(ffipsh,data->window);

fclose(ff);
fclose(ffbu);
fclose(ffsort);
fclose(ffipsh);

unlink(imafsort);

class iceb_u_str imaf_dragm;

if(kolih_dm > 0)
  uosinved_rdm(di,mi,gi,&dragm,&imaf_dragm,data->window);

data->rk->imafil.plus(imaf);
data->rk->naimfil.plus(gettext("Инвентаризационная опись"));

data->rk->imafil.plus(imafbu);
data->rk->naimfil.plus(gettext("Приложение 1"));

data->rk->imafil.plus(imaf_ipsh);
data->rk->naimfil.plus(gettext("Приложение 12"));
if(kolih_dm > 0)
 {
  data->rk->imafil.plus(imaf_dragm.ravno());
  data->rk->naimfil.plus(gettext("Инвентаризационная опись драг. металлов"));
 }
for(int nom=0; nom < data->rk->imafil.kolih(); nom++)
 iceb_ustpeh(data->rk->imafil.ravno(nom),1,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
