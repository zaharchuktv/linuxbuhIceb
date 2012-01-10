/*$Id: mudmcw_r.c,v 1.10 2011-02-21 07:35:55 sasa Exp $*/
/*14.11.2009	23.04.2009	Белых А.И.	mudmcw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "mudmcw.h"

class mudmc_oper
 {
  public:
   class iceb_u_spisok kodm_ei;
   class iceb_u_spisok oper;
   class iceb_u_double suma;
   class iceb_u_double kolih;
   class iceb_u_double kodm_ei_kol; /*Итог по горизонтали*/
   class iceb_u_double kodm_ei_sum; /*Итог по горизонтали*/
   class iceb_u_double oper_kol; /*Итог по вертикали*/
   class iceb_u_double oper_sum; /*Итог по вертикали*/
   
   int metka; /*1-приход 2-расход*/
   
 };

class mudmcw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class mudmcw_poi *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  mudmcw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   mudmcw_r_key_press(GtkWidget *widget,GdkEventKey *event,class mudmcw_r_data *data);
gint mudmcw_r1(class mudmcw_r_data *data);
void  mudmcw_r_v_knopka(GtkWidget *widget,class mudmcw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;

int mudmcw_r(class mudmcw_poi *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class mudmcw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт движения по кодам материалов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(mudmcw_r_key_press),&data);

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

repl.plus(gettext("Расчёт движения по кодам материалов"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(mudmcw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)mudmcw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  mudmcw_r_v_knopka(GtkWidget *widget,class mudmcw_r_data *data)
{
printf("mudmcw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   mudmcw_r_key_press(GtkWidget *widget,GdkEventKey *event,class mudmcw_r_data *data)
{
 printf("mudmcw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}
/********************************/
/*Реквизиты поиска*/
/**********************/
void mudmc_rkp(class mudmcw_poi *poi,FILE *ff)
{
if(poi->sklad.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Склад"),poi->sklad.ravno());
if(poi->shet.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт"),poi->shet.ravno());
if(poi->kodmat.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код материалла"),poi->kodmat.ravno());
if(poi->grupa.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код группы материалла"),poi->grupa.ravno());
if(poi->naim.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Наименование материалла"),poi->naim.ravno());
}
/**************************************/
/*Распечатка масивов по кодам операций*/
/**************************************/
void mudmc_po(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *imaf,class mudmc_oper *prr,
class mudmcw_poi *poi,
GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str naim("");
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str liniq;
double kol=0.;
double sum=0.;
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

if(prr->metka == 1)
  iceb_u_zagolov(gettext("Расшифровка приходов по операциям"),dn,mn,gn,dk,mk,gk,organ,ff);
if(prr->metka == 2)
  iceb_u_zagolov(gettext("Расшифровка расходов по операциям"),dn,mn,gn,dk,mk,gk,organ,ff);

/*печать реквизитов поиска*/
mudmc_rkp(poi,ff);

liniq.plus("-----------------------------------");
//          12345678901234567890123456789012345
for(int ii=0; ii < prr->oper.kolih()+1; ii++)
 {
  if(prr->oper_kol.ravno(ii) == 0.)
   continue;
  liniq.plus("----------------------");
 }
//            1234567890123456789012
fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"\
 Код  |    Наименование    |Едини.|");
/*
12345678901234567890123456789012345
*/
for(int ii=0; ii < prr->oper.kolih(); ii++)
 {
  if(prr->oper_kol.ravno(ii) == 0.)
   continue;
  naim.new_plus(prr->oper.ravno(ii));
  naim.plus(" ");
  if(prr->metka == 1)
    sprintf(strsql,"select naik from Prihod where kod='%s'",prr->oper.ravno(ii));
  if(prr->metka == 2)
    sprintf(strsql,"select naik from Rashod where kod='%s'",prr->oper.ravno(ii));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
    naim.plus(row[0]);   
  fprintf(ff,"%-*.*s|",iceb_u_kolbait(21,naim.ravno()),iceb_u_kolbait(21,naim.ravno()),naim.ravno());
 }
fprintf(ff,"       Итого         |\n");

fprintf(ff,"\
матер.|   Материала        |измер.|");
for(int ii=0; ii < prr->oper.kolih()+1; ii++)
 {
  if(prr->oper_kol.ravno(ii) == 0.)
   continue;
  fprintf(ff,"Количеств.|  Сумма   |");
 }
fprintf(ff,"\n");
fprintf(ff,"%s\n",liniq.ravno());
char kodm[20];
char ei[32];
double itogo_kol=0;
double itogo_sum=0;
int koloper=prr->oper.kolih();
for(int nomer_mat=0; nomer_mat < prr->kodm_ei.kolih(); nomer_mat++)
 {
  /*Строку с нолями пропускаем*/
  if(prr->kodm_ei_kol.ravno(nomer_mat) == 0.)
   continue;

  iceb_u_polen(prr->kodm_ei.ravno(nomer_mat),kodm,sizeof(kodm),1,'|');
  iceb_u_polen(prr->kodm_ei.ravno(nomer_mat),ei,sizeof(ei),2,'|');
  naim.new_plus("");
  sprintf(strsql,"select naimat from Material where kodm=%s",kodm);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim.new_plus(row[0]);
  
  fprintf(ff,"%6s %-*.*s %-*.*s|",
  kodm,
  iceb_u_kolbait(20,naim.ravno()),iceb_u_kolbait(20,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(6,ei),iceb_u_kolbait(6,ei),ei);
  
  itogo_kol=itogo_sum=0.;
  for(int nomer_op=0; nomer_op < koloper; nomer_op++)
   {
    if(prr->oper_kol.ravno(nomer_op) == 0.)
     continue;

    kol=prr->kolih.ravno(nomer_mat*koloper+nomer_op);
    sum=prr->suma.ravno(nomer_mat*koloper+nomer_op);
    fprintf(ff,"%10.10g|%10.2f|",kol,sum);
    
   }
  fprintf(ff,"%10.10g|%10.2f|\n",prr->kodm_ei_kol.ravno(nomer_mat),prr->kodm_ei_sum.ravno(nomer_mat));


 }

fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"\
         Итого                    |");
itogo_kol=itogo_sum=0.;
for(int nomer_op=0; nomer_op < koloper; nomer_op++)
 {
  if(prr->oper_kol.ravno(nomer_op) == 0.)
   continue;

  fprintf(ff,"%10.10g|%10.2f|",prr->oper_kol.ravno(nomer_op),prr->oper_sum.ravno(nomer_op));
 }
fprintf(ff,"%10.10g|%10.2f|\n",prr->oper_kol.suma(),prr->oper_sum.suma());

fprintf(ff,"%s\n",liniq.ravno());

iceb_podpis(ff,wpredok);
fclose(ff);


}


/******************************************/
/******************************************/

gint mudmcw_r1(class mudmcw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;
class mudmc_oper prihod;
class mudmc_oper rashod;
char imaf[56];
short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);

 }
sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);



SQL_str row1,row2;
class SQLCURSOR cur1,cur2;

prihod.metka=1;
rashod.metka=2;
int kolstr1=0;

sprintf(strsql,"select distinct kodm,ei from Kart order by kodm asc");
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  sprintf(strsql,"%s|%s",row[0],row[1]);
  prihod.kodm_ei.plus(strsql);
  rashod.kodm_ei.plus(strsql);
 }

sprintf(strsql,"select kod from Prihod");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
 prihod.oper.plus(row[0]);

sprintf(strsql,"select kod from Rashod");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
 rashod.oper.plus(row[0]);
 
prihod.suma.make_class(prihod.kodm_ei.kolih()*prihod.oper.kolih());
rashod.suma.make_class(rashod.kodm_ei.kolih()*rashod.oper.kolih());

prihod.kolih.make_class(prihod.kodm_ei.kolih()*prihod.oper.kolih());
rashod.kolih.make_class(rashod.kodm_ei.kolih()*rashod.oper.kolih());

prihod.kodm_ei_kol.make_class(prihod.kodm_ei.kolih());
prihod.kodm_ei_sum.make_class(prihod.kodm_ei.kolih());
prihod.oper_kol.make_class(prihod.oper.kolih());
prihod.oper_sum.make_class(prihod.oper.kolih());

rashod.kodm_ei_kol.make_class(rashod.kodm_ei.kolih());
rashod.kodm_ei_sum.make_class(rashod.kodm_ei.kolih());
rashod.oper_kol.make_class(rashod.oper.kolih());
rashod.oper_sum.make_class(rashod.oper.kolih());


class iceb_u_spisok kodm_ei; /*Список кодов материалла-единиц измерения*/

class iceb_u_double kolih1; /*количество на начало периода*/
class iceb_u_double kolih2; /*Количество приход за период*/
class iceb_u_double kolih3; /*Количество расход за период*/
class iceb_u_double kolih4; /*Количество остаток на конец периода*/

class iceb_u_double suma1; /*CУмма на начало периода*/
class iceb_u_double suma2; /*CУмма приход за период*/
class iceb_u_double suma3; /*CУмма расход за период*/
class iceb_u_double suma4; /*CУмма остаток на конец периода*/

sprintf(strsql,"select sklad,nomk,kodm,ei,shetu,cena from Kart order by kodm asc");
kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
kolstr1=0;
int nomer=0;
int kolstr2=0;
int nomer_op=0;
double kolih=0.;
double suma=0.;
short d,m,g;
int tipz=0;
double cena_po_kart=0.;
class ostatok ost;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->shet.ravno(),row[4],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->kodmat.ravno(),row[2],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->sklad.ravno(),row[0],0,0) != 0)
    continue;

  sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
      if(iceb_u_proverka(data->rk->grupa.ravno(),row1[0],0,0) != 0)
        continue;
      if(iceb_u_proverka(data->rk->naim.ravno(),row1[1],4,0) != 0)
        continue;
   }
  ostkarw(dn,mn,gn,dk,mk,gk,atoi(row[0]),atoi(row[1]),&ost);

  if(ost.ostm[0] == 0. && ost.ostm[1] == 0. && ost.ostm[3] == 0.)
   continue;  

  cena_po_kart=atof(row[5]);
  cena_po_kart=iceb_u_okrug(cena_po_kart,okrcn);
  
  sprintf(strsql,"%s|%s",row[2],row[3]);
  if((nomer=kodm_ei.find_r(strsql)) < 0)
    kodm_ei.plus(strsql);

  kolih1.plus(ost.ostm[0],nomer);
  kolih2.plus(ost.ostm[1],nomer);
  kolih3.plus(ost.ostm[2],nomer);
  kolih4.plus(ost.ostm[3],nomer);

  suma1.plus(ost.ostmc[0],nomer);
  suma2.plus(ost.ostmc[1],nomer);
  suma3.plus(ost.ostmc[2],nomer);
  suma4.plus(ost.ostmc[3],nomer);


  sprintf(strsql,"select nomd,datd,tipz,kolih,cena,datdp from Zkart where sklad=%s and nomk=%s and \
datdp >= '%04d-%02d-%02d' and datdp <= '%04d-%02d-%02d'",row[0],row[1],gn,mn,dn,gk,mk,dk);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
    continue;
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    tipz=atoi(row1[2]);
    iceb_u_rsdat(&d,&m,&g,row1[5],2);

    if(iceb_u_SRAV(row1[0],"000",0) == 0)
        continue;

    kolih=atof(row1[3]);
    suma=kolih*cena_po_kart;
    suma=iceb_u_okrug(suma,okrg1);
    /*Узнаём код операции*/
    sprintf(strsql,"select kodop from Dokummat where datd='%s' and sklad=%s and nomd='%s'",

    row1[1],row[0],row1[0]);
    if(iceb_sql_readkey(strsql,&row2,&cur2,data->window) != 1)
     {
      sprintf(strsql,"%s %s %s %s",gettext("Не нашли документ!"),row1[1],row[0],row1[0]);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
  
    sprintf(strsql,"%s|%s",row[2],row[3]);

    if(tipz == 1)
     {
      nomer_op=prihod.oper.find(row2[0]);
      nomer=prihod.kodm_ei.find(strsql);     
      prihod.suma.plus(suma,nomer*prihod.oper.kolih()+nomer_op);
      prihod.kolih.plus(kolih,nomer*prihod.oper.kolih()+nomer_op);

      prihod.kodm_ei_kol.plus(kolih,nomer);
      prihod.kodm_ei_sum.plus(suma,nomer);

      prihod.oper_kol.plus(kolih,nomer_op);
      prihod.oper_sum.plus(suma,nomer_op);

     }
    else
     {
  
      nomer_op=rashod.oper.find(row2[0]);
      nomer=rashod.kodm_ei.find(strsql);     
      rashod.suma.plus(suma,nomer*rashod.oper.kolih()+nomer_op);
      rashod.kolih.plus(kolih,nomer*rashod.oper.kolih()+nomer_op);
      
      rashod.kodm_ei_kol.plus(kolih,nomer);
      rashod.kodm_ei_sum.plus(suma,nomer);


      rashod.oper_kol.plus(kolih,nomer_op);
      rashod.oper_sum.plus(suma,nomer_op);
     }
   }
 }
FILE *ff;
sprintf(imaf,"mudmc%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_zagolov(gettext("Расчёт движения по кодам материалов"),dn,mn,gn,dk,mk,gk,organ,ff);

fprintf(ff,"\x1B\x4D"); /*12-знаков*/
fprintf(ff,"\x0F");  /*Ужатый режим*/

/*печать реквизитов поиска*/
mudmc_rkp(data->rk,ff);


fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 Код  |        Наименование          |Едини.|Остаток на %02d.%02d.%04d|      Приход         |       Расход        |Остаток на %02d.%02d.%04d|\n\
м-ала |         материалла           |измер.|Количество|  Сумма   |Количество|  Сумма   |Количество|  Сумма   |Количество|  Сумма   |\n",
dn,mn,gn,dk,mk,gk);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------\n");
class iceb_u_str kodmat;
class iceb_u_str ei;
char naim[512];
for(int ii=0; ii < kodm_ei.kolih(); ii++)
 {
  iceb_u_polen(kodm_ei.ravno(ii),&kodmat,1,'|');
  iceb_u_polen(kodm_ei.ravno(ii),&ei,2,'|');
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naimat from Material where kodm=%d",kodmat.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);
  fprintf(ff,"\
%6d %-*.*s %-*.*s %10.10g %10.2f %10.10g %10.2f %10.10g %10.2f %10.10g %10.2f\n",
  kodmat.ravno_atoi(),
  iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,
  iceb_u_kolbait(6,ei.ravno()),iceb_u_kolbait(6,ei.ravno()),ei.ravno(),
  kolih1.ravno(ii),suma1.ravno(ii),
  kolih2.ravno(ii),suma2.ravno(ii),
  kolih3.ravno(ii),suma3.ravno(ii),
  kolih4.ravno(ii),suma4.ravno(ii));
  
 }

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s %10.10g %10.2f %10.10g %10.2f %10.10g %10.2f %10.10g %10.2f\n",
iceb_u_kolbait(44,gettext("Итого")),gettext("Итого"),
kolih1.suma(), 
suma1.suma(), 
kolih2.suma(), 
suma2.suma(), 
kolih3.suma(), 
suma3.suma(), 
kolih4.suma(), 
suma4.suma()); 

iceb_podpis(ff,data->window);
fclose(ff);

iceb_ustpeh(imaf,0,data->window);

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт движения по кодам материалов"));

sprintf(imaf,"opp%d.lst",getpid());

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расшифровка приходов по операциям"));

mudmc_po(dn,mn,gn,dk,mk,gk,imaf,&prihod,data->rk,data->window);

iceb_ustpeh(imaf,0,data->window);

sprintf(imaf,"opr%d.lst",getpid());


data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расшифровка расходов по операциям"));
mudmc_po(dn,mn,gn,dk,mk,gk,imaf,&rashod,data->rk,data->window);

iceb_ustpeh(imaf,0,data->window);





gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
