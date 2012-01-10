/*$Id: rnn_imp_r1.c,v 1.23 2011-07-21 12:43:27 sasa Exp $*/
/*12.07.2011	06.05.2008	Белых А.И.	rnn_imp_r1.c
Импорт полученных налоговых накладных в Реестр налоговых накладных
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "rnn_imp.h"
#include "rnnovdw.h"

class rnn_imp_r1_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class rnn_imp_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  rnn_imp_r1_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rnn_imp_r1_key_press(GtkWidget *widget,GdkEventKey *event,class rnn_imp_r1_data *data);
gint rnn_imp_r11(class rnn_imp_r1_data *data);
void  rnn_imp_r1_v_knopka(GtkWidget *widget,class rnn_imp_r1_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int rnn_imp_r1(class rnn_imp_data *datark,GtkWidget *wpredok)
{
char strsql[1024];
iceb_u_str repl;
class rnn_imp_r1_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Импорт налоговых накладных"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rnn_imp_r1_key_press),&data);

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

repl.plus(gettext("Импорт налоговых накладных"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(rnn_imp_r1_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)rnn_imp_r11,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rnn_imp_r1_v_knopka(GtkWidget *widget,class rnn_imp_r1_data *data)
{
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rnn_imp_r1_key_press(GtkWidget *widget,GdkEventKey *event,class rnn_imp_r1_data *data)
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



/******************************************/
/******************************************/

gint rnn_imp_r11(class rnn_imp_r1_data *data)
{
time_t vremn;
time(&vremn);
class rnnovd_rek ovd;
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str viddok("");

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


/*Чтение настроек для определения вида документа*/
rnnovdw(1,&ovd,data->window);

char bros[512];
short vt=0;

memset(bros,'\0',sizeof(bros));
class iceb_u_str imaf_matnast("matnast.alx");

iceb_poldan("Код группы возвратная тара",bros,imaf_matnast.ravno(),data->window);
vt=(short)atoi(bros);
class iceb_u_str kodop_storn; /*Коды операций сторнирования*/
iceb_poldan("Коды операций сторнирования для приходов",&kodop_storn,imaf_matnast.ravno(),data->window);

/*составляем список накладных имеющих код группы*/
sprintf(strsql,"select datd,nnn,mi,gr,inn from Reenn1 where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d' and mi > 0 and gr <> ''",gn,mn,dn,gk,mk,dk);
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_str mu13_14("");
class iceb_u_str usl13_14(""); 
class iceb_u_str uos13_14("");

class iceb_u_str mu15_16("");
class iceb_u_str usl15_16(""); 
class iceb_u_str uos15_16("");

iceb_poldan("Материальный учёт колонки 13-14",&mu13_14,"rnn_nimp1.alx",data->window);
iceb_poldan("Учёт услуг колонки 13-14",&usl13_14,"rnn_nimp1.alx",data->window);
iceb_poldan("Учёт основных средств колонки 13-14",&uos13_14,"rnn_nimp1.alx",data->window);

iceb_poldan("Материальный учёт колонки 15-16",&mu15_16,"rnn_nimp1.alx",data->window);
iceb_poldan("Учёт основных средств колонки 15-16",&usl15_16,"rnn_nimp1.alx",data->window);
iceb_poldan("Учёт услуг колонки 15-16",&uos15_16,"rnn_nimp1.alx",data->window);


class iceb_u_spisok spdok;
class iceb_u_int gr_dok;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s|%s|%s",row[0],row[1],row[2],row[4]);
  spdok.plus(strsql);
  gr_dok.plus(row[3]);
 }

/*составляем список накладных имеющих вид документа*/
sprintf(strsql,"select datd,nnn,mi,vd,inn from Reenn1 where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d' and mi > 0 and vd <> ''",gn,mn,dn,gk,mk,dk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_spisok spdok_vd;
class iceb_u_spisok sviddok;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s|%s|%s",row[0],row[1],row[2],row[4]);
  spdok_vd.plus(strsql);
  sviddok.plus(row[3]);
 }

//Удаляем все накладные загруженные за этот период
sprintf(strsql,"delete from Reenn1 where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d' and mi > 0",gn,mn,dn,gk,mk,dk);

iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s\n",gettext("Материальный учёт"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"select datpnn,kontr,nomnn,sklad,nomd,datd,pn,kodop from Dokummat \
where datpnn >= '%d-%d-%d' and datpnn <= '%d-%d-%d' and tip=1 and \
nomnn <> '%s' order by datpnn asc",gn,mn,dn,gk,mk,dk,"\0");
//printw("%s\n",strsql);
//refresh();
SQLCURSOR cur1;
SQLCURSOR cur2;
SQL_str row1;
SQL_str row2;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s\n",gettext("В \"Материальном учете\" не найдено ни одного документа"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
 }

float kolstr1=0;
char metka[20];
memset(metka,'\0',sizeof(metka));
strncpy(metka,gettext("МУ"),sizeof(metka)-1);
short d,m,g;
int mnds=0;
double nds;
double suma,sumabn,sumkor;
double sum;
double summ[4];
double ndsm[4];

//int mt;
char inn[32];
char nkontr[512];
float pnds=0.;
short dv,mv,gv;
int storn=1;
char kod_gr[32];
int nomer_dok=0;
double kolih=0.;
int nomer_kol=0;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %-6s %-6s %s %s %s\n",metka,row[0],row[1],row[2],row[3],row[4]);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  
  iceb_u_rsdat(&d,&m,&g,row[5],2);
  pnds=atof(row[6]);
  //Узнаём индивидуальный налоговый номер и наименование контрагента
  memset(inn,'\0',sizeof(inn));
  memset(nkontr,'\0',sizeof(nkontr));
  sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[1]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(row1[2][0] == '\0')
      strncpy(nkontr,row1[0],sizeof(nkontr)-1);
    else
      strncpy(nkontr,row1[2],sizeof(nkontr)-1);
    strncpy(inn,row1[1],sizeof(inn)-1);
   }
//  printw("%s %s\n",inn,nkontr);
  
  /*Узнаем НДС документа*/
  mnds=0;
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=11",g,row[4],row[3]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    mnds=atoi(row1[0]);

  nds=0.;

  if(mnds == 0)
   {
    /*Узнаем сумму НДС документа*/
    sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=6",g,row[4],row[3]);

    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
      nds=atof(row1[0]);
   }
  /*Узнаём условие продажи - нужен виддокумента*/
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=7",g,row[4],row[3]);
  viddok.new_plus("");
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(iceb_u_pole2(row1[0],':') == 0)
     {
      if(rnnvdw(1,row1[0]) > 0)
        viddok.new_plus(row1[0]);
     }
    else
     iceb_u_polen(row1[0],&viddok,4,':');
   }
  
  dv=mv=gv=0;  
  /*Узнаём дату выписки документа*/
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=14",g,row[4],row[3]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   iceb_u_rsdat(&dv,&mv,&gv,row1[0],1);
       
  sumzap1w(d,m,g,row[4],atoi(row[3]),&suma,&sumabn,mnds,vt,&sumkor,1,&kolih,data->window);
  storn=1;
  if(iceb_u_proverka(kodop_storn.ravno(),row[7],0,1) == 0)
   storn=-1;
  
  if(mnds == 0 && nds == 0.)
    nds=(suma+sumkor)*pnds/100.;
  
  sum=(suma+sumkor+nds+sumabn)*storn;
  if(nds != 0)
   nds*=storn;
   
  memset(summ,'\0',sizeof(summ));
  memset(ndsm,'\0',sizeof(ndsm));
  
  if(mnds < 3)
   {
    nomer_kol=0;
    if(iceb_u_proverka(mu13_14.ravno(),row[7],0,1) == 0)
     nomer_kol=2;
    if(iceb_u_proverka(mu15_16.ravno(),row[7],0,1) == 0)
     nomer_kol=3;
     
    summ[nomer_kol]=sum;
    ndsm[nomer_kol]=nds;
   }
  else
   {
    if(iceb_u_SRAV("400000000000",inn,0) == 0)/*�� ���� ��������� ���������*/
     {
      summ[0]=sum;
      ndsm[0]=0.;
     }    
    else
     {
      summ[1]=sum;
      ndsm[1]=nds;
     }
   }

  /*Ищем код группы*/
  memset(kod_gr,'\0',sizeof(kod_gr));
  sprintf(strsql,"%s|%s|1|%s",row[0],row[2],inn);
  if((nomer_dok=spdok.find(strsql)) >= 0)
     sprintf(kod_gr,"%d",gr_dok.ravno(nomer_dok));

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(nds != 0. && inn[0] == '\0')
   {
     class iceb_u_str repl;
     repl.plus(gettext("Материальный учёт"));
     sprintf(strsql,"%s:%d.%d.%d",gettext("Дата получения налоговой накладной"),d,m,g);
     repl.ps_plus(strsql);
     
     sprintf(strsql,"%s:%s",gettext("Номер налоговой накладной"),row[2]);
     repl.ps_plus(strsql);

     sprintf(strsql,"%s:%s/%s",gettext("Контрагент"),row[1],nkontr);
     repl.ps_plus(strsql);

     repl.ps_plus(gettext("Не ввели индивидуальный налоговый номер!"));
     iceb_menu_soob(&repl,data->window);
   }  

  /*определение вида документа*/
  if(viddok.getdlinna() <= 1)
   {
    rnn_imp_ovd(&viddok,row[7],&ovd.muvd,&ovd.muso);
    /*Смотрим был ли введён вид документа*/
    if(viddok.getdlinna() <= 1)
     {
      sprintf(strsql,"%s|%s|1|%s",row[0],row[2],inn);
      if((nomer_dok=spdok_vd.find(strsql)) >= 0)
       viddok.new_plus(sviddok.ravno(nomer_dok));
     }  
   }
  zapvree1w(d,m,g,row[2],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
  inn,nkontr,1,row[5],row[4],"",kod_gr,row[7],dv,mv,gv,viddok.ravno(),data->window);


 }

sprintf(strsql,"%s\n",gettext("Учёт услуг"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"select datpnn,kontr,nomnn,podr,nomd,sumkor,\
nds,datd,sumnds,pn,kodop,datdp,osnov,uslpr from Usldokum where datpnn >= '%d-%d-%d' and \
datpnn <= '%d-%d-%d' and tp=1 and nomnn <> '%s' order by datpnn asc",
gn,mn,dn,gk,mk,dk,"\0");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  sprintf(strsql,"%s\n",gettext("В \"Учете услуг\" не найдено ни одного документа"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
 }
kolstr1=0;
memset(metka,'\0',sizeof(metka));
strncpy(metka,gettext("УСЛ"),sizeof(metka)-1);
while(cur.read_cursor(&row) != 0)
 {
  dv=mv=gv=0;
  sprintf(strsql,"%s %s %-6s %-6s %s %s\n",metka,row[0],row[1],row[2],row[3],row[4]);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
//  strzag(lines-1,0,kolstr,++kolstr1);
  pnds=atof(row[9]);
  //Узнаём индивидуальный налоговый номер и наименование контрагента
  memset(inn,'\0',sizeof(inn));
  memset(nkontr,'\0',sizeof(nkontr));

  sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[1]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(row1[2][0] == '\0')
      strncpy(nkontr,row1[0],sizeof(nkontr)-1);
    else
      strncpy(nkontr,row1[2],sizeof(nkontr)-1);
    strncpy(inn,row1[1],sizeof(inn)-1);
   }


  sumkor=atof(row[5]);
  mnds=atoi(row[6]);
  nds=0.;
  if(mnds == 0)
   nds=atof(row[8]);
   
  iceb_u_rsdat(&d,&m,&g,row[7],2);
  sumzap1uw(d,m,g,row[4],atoi(row[3]),&suma,&sumabn,mnds,1,data->window);

  sum=suma+sumkor+nds+sumabn;
//  printw("sum=%f\n",sum);
//  OSTANOV();
  if(mnds == 0)
   {
    if(nds == 0.)
     {
      nds=(suma+sumkor)*pnds/100.;
      sum=suma+sumkor+nds+sumabn;
     }     
    else
     {
      //Пересчитываем обратным счетом базу налогообложения
      sum=nds*((100.+pnds)/pnds);
     }
   }

  memset(summ,'\0',sizeof(summ));
  memset(ndsm,'\0',sizeof(ndsm));
  if(mnds < 3)
   {
    nomer_kol=0;
    if(iceb_u_proverka(usl13_14.ravno(),row[10],0,1) == 0)
     nomer_kol=2;
    if(iceb_u_proverka(usl15_16.ravno(),row[10],0,1) == 0)
     nomer_kol=3;
    summ[nomer_kol]=sum;
    ndsm[nomer_kol]=nds;
   }
  else
   {
    if(iceb_u_SRAV("400000000000",inn,0) == 0)/*�� ���� ��������� ���������*/
     {
      summ[0]=sum;
      ndsm[0]=0.;
     }    
    else
     {
      summ[1]=sum;
      ndsm[1]=nds;
     }
   }
  /*Ищем код группы*/
  memset(kod_gr,'\0',sizeof(kod_gr));
  sprintf(strsql,"%s|%s|2|%s",row[0],row[2],inn);
  if((nomer_dok=spdok.find(strsql)) >= 0)
     sprintf(kod_gr,"%d",gr_dok.ravno(nomer_dok));

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(nds != 0. && inn[0] == '\0')
   {
     class iceb_u_str repl;
     repl.plus(gettext("Учёт услуг"));
     sprintf(strsql,"%s:%d.%d.%d",gettext("Дата получения налоговой накладной"),d,m,g);
     repl.ps_plus(strsql);
     
     sprintf(strsql,"%s:%s",gettext("Номер налоговой накладной"),row[2]);
     repl.ps_plus(strsql);

     sprintf(strsql,"%s:%s/%s",gettext("Контрагент"),row[1],nkontr);
     repl.ps_plus(strsql);

     repl.ps_plus(gettext("Не ввели индивидуальный налоговый номер!"));
     iceb_menu_soob(&repl,data->window);
   }  

  iceb_u_rsdat(&dv,&mv,&gv,row[11],2);
  /*определение вида документа*/
  viddok.new_plus("");
  if(iceb_u_pole2(row[13],':') == 0 && row[13][0] != '\0')
   {
    if(rnnvdw(1,row[13]) > 0)
      viddok.new_plus(row[13]);
   }
  else
   iceb_u_polen(row[13],&viddok,4,':');
  if(viddok.getdlinna() <= 1)
   {
    rnn_imp_ovd(&viddok,row[10],&ovd.uuvd,&ovd.uuso);
    if(viddok.getdlinna() <= 1)
     {
      sprintf(strsql,"%s|%s|2|%s",row[0],row[2],inn);
      if((nomer_dok=spdok_vd.find(strsql)) >= 0)
       viddok.new_plus(sviddok.ravno(nomer_dok));
     }  
   }
  zapvree1w(d,m,g,row[2],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
  inn,nkontr,2,row[7],row[4],row[12],kod_gr,row[10],dv,mv,gv,viddok.ravno(),data->window);

 }

sprintf(strsql,"%s\n",gettext("Учёт основных средств"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"select datpnn,kontr,podr,nomnn,nomd,\
nds,datd,pn,kodop,dvnn from Uosdok where datpnn >= '%04d-%d-%d' and \
datpnn <= '%04d-%d-%d' and tipz=1 and nomnn <> '%s' order by datpnn asc",
gn,mn,dn,gk,mk,dk,"\0");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  sprintf(strsql,"%s\n",gettext("В \"Учете основных средств\" не найдено ни одного документа"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
 }
kolstr1=0;
memset(metka,'\0',sizeof(metka));
strncpy(metka,gettext("УОС"),sizeof(metka)-1);
while(cur.read_cursor(&row) != 0)
 {
  dv=mv=gv=0;
  sprintf(strsql,"%s %s %-6s %-6s %s %s\n",metka,row[0],row[1],row[2],row[3],row[4]);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
//  strzag(lines-1,0,kolstr,++kolstr1);
  pnds=atof(row[7]);
  //Узнаём индивидуальный налоговый номер и наименование контрагента
  memset(inn,'\0',sizeof(inn));
  memset(nkontr,'\0',sizeof(nkontr));
  sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[1]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(row1[2][0] == '\0')
      strncpy(nkontr,row1[0],sizeof(nkontr)-1);
    else
      strncpy(nkontr,row1[2],sizeof(nkontr)-1);
    strncpy(inn,row1[1],sizeof(inn)-1);
   }

  mnds=atoi(row[5]);
  iceb_u_rsdat(&d,&m,&g,row[6],2);
  sumzapuosw(d,m,g,row[4],&suma,&sumabn,mnds,1,data->window);
  nds=0.;
  if(mnds == 0)  
    nds=suma*pnds/100.;
  sum=suma+nds+sumabn;
  iceb_u_rsdat(&d,&m,&g,row[6],2);

  iceb_u_rsdat(&dv,&mv,&gv,row[9],2);
  
  memset(summ,'\0',sizeof(summ));
  memset(ndsm,'\0',sizeof(ndsm));
  if(mnds < 3)
   {
    nomer_kol=0;
    if(iceb_u_proverka(uos13_14.ravno(),row[8],0,1) == 0)
     nomer_kol=2;
    if(iceb_u_proverka(uos15_16.ravno(),row[8],0,1) == 0)
     nomer_kol=3;
    summ[nomer_kol]=sum;
    ndsm[nomer_kol]=nds;
   }
  else
   {
    if(iceb_u_SRAV("400000000000",inn,0) == 0)/*�� ���� ��������� ���������*/
     {
      summ[0]=sum;
      ndsm[0]=0.;
     }    
    else
     {
      summ[1]=sum;
      ndsm[1]=nds;
     }
   }
  /*Ищем код группы*/
  memset(kod_gr,'\0',sizeof(kod_gr));
  sprintf(strsql,"%s|%s|3|%s",row[0],row[3],inn);
  if((nomer_dok=spdok.find(strsql)) >= 0)
     sprintf(kod_gr,"%d",gr_dok.ravno(nomer_dok));
  
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(nds != 0. && inn[0] == '\0')
   {
     class iceb_u_str repl;
     repl.plus(gettext("Учёт основных средств"));
     sprintf(strsql,"%s:%d.%d.%d",gettext("Дата получения налоговой накладной"),d,m,g);
     repl.ps_plus(strsql);
     
     sprintf(strsql,"%s:%s",gettext("Номер налоговой накладной"),row[3]);
     repl.ps_plus(strsql);

     sprintf(strsql,"%s:%s/%s",gettext("Контрагент"),row[1],nkontr);
     repl.ps_plus(strsql);

     repl.ps_plus(gettext("Не ввели индивидуальный налоговый номер!"));
     iceb_menu_soob(&repl,data->window);
   }  

  /*определение вида документа*/
  rnn_imp_ovd(&viddok,row[8],&ovd.uosvd,&ovd.uosso);
  if(viddok.getdlinna() <= 1)
   {
    sprintf(strsql,"%s|%s|3|%s",row[0],row[3],inn);
    if((nomer_dok=spdok_vd.find(strsql)) >= 0)
     viddok.new_plus(sviddok.ravno(nomer_dok));
   }
  zapvree1w(d,m,g,row[3],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
  inn,nkontr,3,row[6],row[4],"",kod_gr,row[8],dv,mv,gv,viddok.ravno(),data->window);
 }

sprintf(strsql,"%s\n",gettext("Учёт командировочных расходов"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"select nomd,datd,datao,vkom from Ukrdok where datao >= '%04d-%d-%d' and \
datao <= '%04d-%d-%d' order by datao asc",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  sprintf(strsql,"%s\n",gettext("В \"Учете командировочных расходов\" не найдено ни одного документа"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
 }
kolstr1=0;
int kolstr2=0;
double strah_sbor=0.;
memset(metka,'\0',sizeof(metka));
strncpy(metka,gettext("УКР"),sizeof(metka)-1);
while(cur.read_cursor(&row) != 0)
 {
//  sprintf(strsql,"%s %-6s %-6s %s\n",metka,row[0],row[1],row[2]);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
//  strzag(lines-1,0,kolstr,++kolstr1);

  sprintf(strsql,"select cena,snds,kdrnn,ndrnn,dvnn,kodr,ss from Ukrdok1 \
where datd='%s' and nomd='%s' and kdrnn <> ''",row[1],row[0]);
  
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }  

  while(cur1.read_cursor(&row1) != 0)
   {
    dv=mv=gv=0;
    sprintf(strsql,"%s %-6s %-6s %-6s %-6s\n",metka,row1[0],row1[1],row1[2],row1[3]);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    //Узнаём индивидуальный налоговый номер и наименование контрагента
    memset(inn,'\0',sizeof(inn));
    memset(nkontr,'\0',sizeof(nkontr));
    sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row1[2]);
    
    if(iceb_sql_readkey(strsql,&row2,&cur2,data->window) == 1)
     {
      if(row2[2][0] == '\0')
        strncpy(nkontr,row2[0],sizeof(nkontr)-1);
      else
        strncpy(nkontr,row2[2],sizeof(nkontr)-1);
      strncpy(inn,row2[1],sizeof(inn)-1);
     }
    nds=atof(row1[1]);
    sum=atof(row1[0])+nds;
    strah_sbor=atof(row1[6]);
    memset(summ,'\0',sizeof(summ));
    memset(ndsm,'\0',sizeof(ndsm));
    if(nds != 0.)
     {
      summ[0]=sum;
      ndsm[0]=nds;
     }
    else
     {
      if(iceb_u_SRAV("400000000000",inn,0) == 0)/*�� ���� ��������� ���������*/
       {
        summ[0]=sum;
        ndsm[0]=0.;
       }    
      else
        summ[1]=sum;
     }
    /*Ищем код группы*/
    memset(kod_gr,'\0',sizeof(kod_gr));
    sprintf(strsql,"%s|%s|4|%s",row[1],row[0],inn);
    if((nomer_dok=spdok.find(strsql)) >= 0)
       sprintf(kod_gr,"%d",gr_dok.ravno(nomer_dok));
 
    iceb_u_rsdat(&d,&m,&g,row[2],2);
    iceb_u_rsdat(&dv,&mv,&gv,row1[4],2);

    if(nds != 0. && inn[0] == '\0')
     {
       class iceb_u_str repl;
       repl.plus(gettext("Учёт командировочных расходов"));
       sprintf(strsql,"%s:%d.%d.%d",gettext("Дата получения налоговой накладной"),d,m,g);
       repl.ps_plus(strsql);
       
       sprintf(strsql,"%s:%s",gettext("Номер налоговой накладной"),row1[3]);
       repl.ps_plus(strsql);

       sprintf(strsql,"%s:%s/%s",gettext("Контрагент"),row1[2],nkontr);
       repl.ps_plus(strsql);

       repl.ps_plus(gettext("Не ввели индивидуальный налоговый номер!"));
       iceb_menu_soob(&repl,data->window);
     }  
    /*определение вида документа*/
    rnn_imp_ovd(&viddok,row1[5],&ovd.ukrvd,&ovd.ukrsr);
    if(viddok.getdlinna() <= 1)
     {
      sprintf(strsql,"%s|%s|4|%s",row[1],row[0],inn);
      if((nomer_dok=spdok_vd.find(strsql)) >= 0)
       viddok.new_plus(sviddok.ravno(nomer_dok));
     }    
    zapvree1w(d,m,g,row1[3],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
    inn,nkontr,4,row[1],row[0],"",kod_gr,row[3],dv,mv,gv,viddok.ravno(),data->window);
    if(strah_sbor != 0.)
     {
      memset(summ,'\0',sizeof(summ));
      memset(ndsm,'\0',sizeof(ndsm));
      summ[1]=strah_sbor;      
      zapvree1w(d,m,g,row1[3],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
      inn,nkontr,4,row[1],row[0],"",kod_gr,row[3],dv,mv,gv,viddok.ravno(),data->window);
     }
   }
 }

sprintf(strsql,"%s\n",gettext("Главная книга"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

class iceb_u_str imaf_nast("rnn_nast.alx");
class iceb_u_str shet_nds("");

if(iceb_poldan("Счёт расчётов по НДС",&shet_nds,imaf_nast.ravno(),data->window) == 0)
 {
  if(shet_nds.getdlinna() > 1)
   {

    sprintf(strsql,"select datp,sh,shk,kodkon,deb,komen from Prov where datp >= '%04d-%d-%d' and \
    datp <= '%04d-%d-%d' and sh='%s' and deb <> 0. and komen like 'нн%%' order by datp asc",gn,mn,dn,gk,mk,dk,shet_nds.ravno());

    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

    if(kolstr == 0)
     {  
      sprintf(strsql,"%s\n",gettext("В \"Главной книге \" не найдено нужных проводок"));
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
     }
    kolstr1=0;
    memset(metka,'\0',sizeof(metka));
    strncpy(metka,gettext("ГК"),sizeof(metka)-1);
    char nomer_nn[64];
    class iceb_u_str koment;
    while(cur.read_cursor(&row) != 0)
     {
    //  printw("%s %-6s %-6s %s\n",metka,row[0],row[1],row[2]);
    //  strzag(lines-1,0,kolstr,++kolstr1);
      iceb_pbar(data->bar,kolstr,++kolstr1);    
      dv=mv=gv=0;
      sprintf(strsql,"%s %-6s %-6s %-6s %-6s %10s %s\n",metka,row[0],row[1],row[2],row[3],row[4],row[5]);
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
      //Узнаём индивидуальный налоговый номер и наименование контрагента
      memset(inn,'\0',sizeof(inn));
      memset(nkontr,'\0',sizeof(nkontr));
      sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[3]);
      
      if(iceb_sql_readkey(strsql,&row2,&cur2,data->window) == 1)
       {
        if(row2[2][0] == '\0')
          strncpy(nkontr,row2[0],sizeof(nkontr)-1);
        else
          strncpy(nkontr,row2[2],sizeof(nkontr)-1);
        strncpy(inn,row2[1],sizeof(inn)-1);
       }
      
      iceb_u_rsdat(&d,&m,&g,row[0],2);
      nds=atof(row[4]);
      
      /*Номер налоговой накладной*/
      if(iceb_u_polen(row[5],strsql,sizeof(strsql),1,' ') != 0)
       {
        sprintf(strsql,"Не правильно введён комментарий!\n");
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
        continue;
       }
       
      strncpy(nomer_nn,iceb_u_adrsimv(2,strsql),sizeof(nomer_nn));
      
      /*Дата выписки налоговой накладной*/
      if(iceb_u_polen(row[5],strsql,sizeof(strsql),2,' ') != 0)
       {
        sprintf(strsql,"В комментарии не найдена дата выписки налоговой накладной !\n");
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
        continue;
       }
      if(iceb_u_rsdat(&dv,&mv,&gv,strsql,1) != 0)
       {
        
        iceb_printw(iceb_u_toutf("В комментарии не правильно введена дата выписки налоговой накладной!\n"),data->buffer,data->view);
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
        continue;
       }     
      if(iceb_u_polen(row[5],&viddok,3,' ') != 0)
       viddok.new_plus("ПН");
      memset(summ,'\0',sizeof(summ));
      memset(ndsm,'\0',sizeof(ndsm));

      sum=nds*100./20.+nds;
      summ[0]=iceb_u_okrug(sum,0.01);
      ndsm[0]=nds;
      
      koment.new_plus(row[1]);
      koment.plus(" ");
      koment.plus(row[2]);
      koment.plus(" ");
      koment.plus(row[5]);

      /*Ищем код группы*/
      memset(kod_gr,'\0',sizeof(kod_gr));
      sprintf(strsql,"%s|%s|5|%s",row[0],nomer_nn,inn);
      if((nomer_dok=spdok.find(strsql)) >= 0)
         sprintf(kod_gr,"%d",gr_dok.ravno(nomer_dok));
           
      zapvree1w(d,m,g,nomer_nn,summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
      inn,nkontr,5,"","",koment.ravno(),kod_gr,"",dv,mv,gv,viddok.ravno(),data->window);

     }
   }
 }

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
