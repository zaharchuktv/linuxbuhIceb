/*$Id: spis_p_kontrw_r.c,v 1.14 2011-02-21 07:35:57 sasa Exp $*/
/*16.11.2009	10.03.2006	Белых А.И.	spis_p_kontrw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "spis_p_kontrw.h"

class spis_p_kontrw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class spis_p_kontrw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  spis_p_kontrw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   spis_p_kontrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class spis_p_kontrw_r_data *data);
gint spis_p_kontrw_r1(class spis_p_kontrw_r_data *data);
void  spis_p_kontrw_r_v_knopka(GtkWidget *widget,class spis_p_kontrw_r_data *data);

int spis_p_kontr_provup(SQL_str row,const char *sklad,const char *grup_mat,const char *kodmat,const char *kontr,const char *kodop,const char *shetz,const char *nomdokp,int *kodmati,char *ei,char *kontr_v_dok,GtkWidget *wpredok);
void spis_p_kont_rekpoi(const char *sklad,const char *grup_mat,const char *kodmat,const char *kontr,const char *kodop,const char *shetz,const char *nomdokp,FILE *ff);
void spis_p_kontr_rasrr(class iceb_u_spisok *kodmat_ei,class iceb_u_spisok *spis_kontr,class iceb_u_double *m_kolih,class iceb_u_double *m_summa,FILE *ff,GtkWidget *wpredok);
void spis_p_kontr_rasrr2(class iceb_u_spisok *kodmat_ei,class iceb_u_spisok *spis_kontr,class iceb_u_double *m_kolih,class iceb_u_double *m_summa,double *itogo_k,double *itogo_s,FILE *ff,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern double   okrg1;
extern double	okrcn;

int spis_p_kontrw_r(class spis_p_kontrw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class spis_p_kontrw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка списка документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(spis_p_kontrw_r_key_press),&data);

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

repl.plus(gettext("Распечатка списка документов"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(spis_p_kontrw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)spis_p_kontrw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  spis_p_kontrw_r_v_knopka(GtkWidget *widget,class spis_p_kontrw_r_data *data)
{
//printf("spis_p_kontrw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   spis_p_kontrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class spis_p_kontrw_r_data *data)
{
// printf("spis_p_kontrw_r_key_press\n");
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

gint spis_p_kontrw_r1(class spis_p_kontrw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);

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


SQL_str row;
SQLCURSOR cur;
int kolstr=0;
float kolstr1=0.;

int tipz=data->rk->prih_rash;


if(tipz != 0)
 sprintf(strsql,"select * from Zkart where datdp >= '%d-%d-%d' and datdp <= '%d-%d-%d' and tipz=%d",
 gn,mn,dn,gk,mk,dk,tipz);
else
 sprintf(strsql,"select * from Zkart where datdp >= '%d-%d-%d' and datdp <= '%d-%d-%d'",
 gn,mn,dn,gk,mk,dk);

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

FILE *fftmp;
char imaftmp[50];
sprintf(imaftmp,"skontr%d.tmp",getpid());

if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_spisok kodmat_ei;
class iceb_u_spisok spis_kontr;

class iceb_u_spisok kodmat_ei_p;
class iceb_u_spisok spis_kontr_p;

char ei[32];
char kontr_v_dok[30];

//Создаём нужные списки
int kolstr2=0;
kolstr1=0.;
int tipzz=0;
int kodmati=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
  if(iceb_u_SRAV(row[2],"000",0) == 0) //Стартовый остаток пропускаем
   continue;
  
  tipzz=atoi(row[5]);

  memset(kontr_v_dok,'\0',sizeof(kontr_v_dok));
  if(spis_p_kontr_provup(row,data->rk->sklad.ravno(),data->rk->grup_mat.ravno(),data->rk->kodmat.ravno(),data->rk->kontr.ravno(),
  data->rk->kodop.ravno(),data->rk->shetu.ravno(),data->rk->nomdok.ravno(),&kodmati,ei,kontr_v_dok,data->window) != 0) 
   continue;
  
  if(iceb_u_SRAV(kontr_v_dok,"00-",1) == 0)
   strcpy(kontr_v_dok,"00");

  sprintf(strsql,"%d|%s",kodmati,ei);
    
  if(tipzz == 2)
   { 
    if(kodmat_ei.find(strsql) < 0)
     kodmat_ei.plus(strsql);

    if(spis_kontr.find_r(kontr_v_dok) < 0)
      spis_kontr.plus(kontr_v_dok);
   }
  
  if(tipzz == 1)
   { 
    if(kodmat_ei_p.find(strsql) < 0)
     kodmat_ei_p.plus(strsql);

    if(spis_kontr_p.find_r(kontr_v_dok) < 0)
     spis_kontr_p.plus(kontr_v_dok);
   }
   
  fprintf(fftmp,"%d|%s|%s|%s|%s|%d|\n",kodmati,ei,kontr_v_dok,row[6],row[7],tipzz);
  kolstr2++;
 }
fclose(fftmp);

if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
unlink(imaftmp);
//Создаём массивы для количества и суммы
class iceb_u_double m_kolih;
class iceb_u_double m_summa;

m_kolih.make_class(kodmat_ei.kolih()*spis_kontr.kolih());
m_summa.make_class(kodmat_ei.kolih()*spis_kontr.kolih());

class iceb_u_double m_kolih_p;
class iceb_u_double m_summa_p;

m_kolih_p.make_class(kodmat_ei_p .kolih()*spis_kontr_p.kolih());
m_summa_p.make_class(kodmat_ei_p.kolih()*spis_kontr_p.kolih());

int nomer_kodmat_ei;
int nomer_kontr;
double kolih,cena;

kolstr1=0.;
double suma;
char stroka[1024];
int kolih_km=kodmat_ei.kolih();
int kolih_km_p=kodmat_ei_p.kolih();

while(fgets(stroka,sizeof(stroka)-1,fftmp) != NULL)
 {
  iceb_pbar(data->bar,kolstr2,++kolstr1);    

  iceb_u_polen(stroka,strsql,sizeof(strsql),1,'|');
  kodmati=atoi(strsql);

  iceb_u_polen(stroka,ei,sizeof(ei),2,'|');

  iceb_u_polen(stroka,kontr_v_dok,sizeof(kontr_v_dok),3,'|');

  iceb_u_polen(stroka,strsql,sizeof(strsql),4,'|');
  kolih=atof(strsql);

  iceb_u_polen(stroka,strsql,sizeof(strsql),5,'|');
  cena=atof(strsql);

  iceb_u_polen(stroka,strsql,sizeof(strsql),6,'|');
  tipzz=atoi(strsql);

  suma=kolih*cena;
  
  sprintf(strsql,"%d|%s",kodmati,ei);
  if(tipzz == 2)
   {
    nomer_kodmat_ei=kodmat_ei.find(strsql);
    nomer_kontr=spis_kontr.find_r(kontr_v_dok);

    m_kolih.plus(kolih,nomer_kontr*kolih_km+nomer_kodmat_ei);
    m_summa.plus(suma,nomer_kontr*kolih_km+nomer_kodmat_ei);
   }  
  if(tipzz == 1)
   {
    nomer_kodmat_ei=kodmat_ei_p.find(strsql);
    nomer_kontr=spis_kontr_p.find_r(kontr_v_dok);

    m_kolih_p.plus(kolih,nomer_kontr*kolih_km_p+nomer_kodmat_ei);
    m_summa_p.plus(suma,nomer_kontr*kolih_km_p+nomer_kodmat_ei);
   }  
 }
fclose(fftmp);

FILE *ff;


char imaf[56];
sprintf(imaf,"skontr%d.lst",getpid());

data->rk->imaf.new_plus(imaf);
data->rk->naimf.new_plus(gettext("Расчёт списания/получения материаллов по контрагентам"));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_u_zagolov(gettext("Расчёт списания/получения материаллов по контрагентам"),dn,mn,gn,dk,mk,gk,organ,ff);

spis_p_kont_rekpoi(data->rk->sklad.ravno(),data->rk->grup_mat.ravno(),data->rk->kodmat.ravno(),data->rk->kontr.ravno(),data->rk->kodop.ravno(),data->rk->shetu.ravno(),data->rk->nomdok.ravno(),ff);

if(tipz == 0 || tipz == 2)
 {
  fprintf(ff,"\n%s\n",gettext("Расчёт списания материалов по контрагентам"));
  spis_p_kontr_rasrr(&kodmat_ei,&spis_kontr,&m_kolih,&m_summa,ff,data->window);
 }
if(tipz == 0 || tipz == 1)
 {
  fprintf(ff,"\n%s\n",gettext("Расчёт получения материалов по контрагентам"));
  spis_p_kontr_rasrr(&kodmat_ei_p,&spis_kontr_p,&m_kolih_p,&m_summa_p,ff,data->window);
 }
iceb_podpis(ff,data->window);

fclose(ff);

char imaf2[50];
sprintf(imaf2,"skontr2%d.lst",getpid());
data->rk->imaf.plus(imaf2);
data->rk->naimf.plus(gettext("Расчёт списания/получения материаллов по контрагентам"));

if((ff = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_u_zagolov(gettext("Расчёт списания/получения материаллов по контрагентам"),dn,mn,gn,dk,mk,gk,organ,ff);

spis_p_kont_rekpoi(data->rk->sklad.ravno(),data->rk->grup_mat.ravno(),data->rk->kodmat.ravno(),data->rk->kontr.ravno(),data->rk->kodop.ravno(),data->rk->shetu.ravno(),data->rk->nomdok.ravno(),ff);

double itogo_k_p=0.;
double itogo_s_p=0.;
double itogo_k_r=0.;
double itogo_s_r=0.;

if(tipz == 0 || tipz == 2)
 {
  fprintf(ff,"\n%s\n",gettext("Расчёт списания материалов по контрагентам"));
  spis_p_kontr_rasrr2(&kodmat_ei,&spis_kontr,&m_kolih,&m_summa,&itogo_k_r,&itogo_s_r,ff,data->window);
 }
if(tipz == 0 || tipz == 1)
 {
  fprintf(ff,"\n%s\n",gettext("Расчёт поучения материалов по контрагентам"));
  spis_p_kontr_rasrr2(&kodmat_ei_p,&spis_kontr_p,&m_kolih_p,&m_summa_p,&itogo_k_p,&itogo_s_p,ff,data->window);
 }
iceb_podpis(ff,data->window);

fclose(ff);


iceb_printw(gettext("Получено"),data->buffer,data->view);
iceb_printw("\n",data->buffer,data->view);


sprintf(strsql,"%s:%10.10g\n",gettext("Количество"),itogo_k_p);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s:%10.2f\n",gettext("Сумма"),itogo_s_p);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

iceb_printw("-------------------------------\n",data->buffer,data->view);

iceb_printw(gettext("Списано"),data->buffer,data->view);
iceb_printw("\n",data->buffer,data->view);

sprintf(strsql,"%s:%10.10g\n",gettext("Количество"),itogo_k_r);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s:%10.2f\n",gettext("Сумма"),itogo_s_r);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


iceb_ustpeh(imaf,0,data->window);
iceb_ustpeh(imaf2,0,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/***********************************/
/*Проверка на условия поиска*/
/*******************************/

int spis_p_kontr_provup(SQL_str row,
const char *sklad,
const char *grup_mat,
const char *kodmat,
const char *kontr,
const char *kodop,
const char *shetz,
const char *nomdokp,
int *kodmati,
char *ei,
char *kontr_v_dok,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row1;
SQLCURSOR cur1;

if(iceb_u_proverka(sklad,row[0],0,0) != 0)
  return(1);
if(iceb_u_proverka(nomdokp,row[2],0,0) != 0)
  return(1);

sprintf(strsql,"select kontr,kodop from Dokummat where datd='%s' and sklad=%s and nomd='%s'",
row[4],row[0],row[2]);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
 {
  class iceb_u_str repl;
  repl.plus(gettext("Не найдена шапка документа !"));

  sprintf(strsql,"%s %s %s",row[4],row[0],row[2]);
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }


char kontr_dok[30];
strcpy(kontr_dok,row1[0]);
if(iceb_u_SRAV(kontr_dok,"00-",1) == 0)
   strcpy(kontr_dok,"00");

if(iceb_u_proverka(kontr,kontr_dok,0,0) != 0)
  return(1);

if(iceb_u_proverka(kodop,row1[1],0,0) != 0)
  return(1);

strcpy(kontr_v_dok,row1[0]);

//Читаем запись в документе

sprintf(strsql,"select kodm from Dokummat1 where datd='%s' and sklad=%s and nomd='%s' and \
nomkar=%s",row[4],row[0],row[2],row[1]);

if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
 {
  class iceb_u_str repl;

  sprintf(strsql,"%s !",gettext("Не найдена запись в документе"));
  repl.plus(strsql);

  sprintf(strsql,"%s %s %s %s",row[4],row[0],row[2],row[1]);
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }    
if(iceb_u_proverka(kodmat,row1[0],0,0) != 0)
 return(1);

*kodmati=atoi(row1[0]);

if(grup_mat[0] != '\0') //Узнаём группу материалла
 {
  sprintf(strsql,"select kodgr from Material where kodm=%s",row1[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    
    sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),row1[0]);
    iceb_menu_soob(strsql,wpredok);
    return(1);

   }
  if(iceb_u_proverka(grup_mat,row1[0],0,0) != 0)
   return(1);
 }     


//Читаем карточку документа
sprintf(strsql,"select ei,shetu from Kart where sklad=%s and nomk=%s",row[0],row[1]);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
 {
  class iceb_u_str repl;
  sprintf(strsql,"%s !",gettext("Не найдена карточка материалла"));
  repl.plus(strsql);
  
  sprintf(strsql,"%s %s",row[0],row[1]);
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
 }
if(iceb_u_proverka(shetz,row1[1],0,0) != 0)
   return(1);
strcpy(ei,row1[0]);
return(0);
}
/*******************************/
/*строка подчёркивания*/
/*************************/
void spis_p_kontr_strp(int kolih_km,int metka,FILE *ff)
{
//Полка над наименованием контрагента
if(metka == 0)
 fprintf(ff,"------------------------------");
if(metka == 1)
  fprintf(ff,"-------------------------------------");
for(int ii=0; ii < kolih_km+1; ii++)
 fprintf(ff,"---------------------");
 //          123456789|1234567890|
fprintf(ff,"\n");
}

/*******************************************/
/*Распечатка результатов расчёта*/
/***********************************/
void spis_p_kontr_rasrr(class iceb_u_spisok *kodmat_ei,
class iceb_u_spisok *spis_kontr,
class iceb_u_double *m_kolih,
class iceb_u_double *m_summa,
FILE *ff,
GtkWidget *wpredok)
{

char strsql[512];
char kodmat[30];
char naim[512];
SQL_str row;
SQLCURSOR cur;
int kolih_km=kodmat_ei->kolih();


spis_p_kontr_strp(kolih_km,0,ff);


fprintf(ff,"  Код   |  Контрагент        |");
//Пишем первую строку с наименованиями материаллов
for(int ii=0; ii < kolih_km; ii++)
 {
  iceb_u_polen(kodmat_ei->ravno(ii),kodmat,sizeof(kodmat),1,'|');
  
  memset(naim,'\0',sizeof(naim));
  //узнаём наименование материалла

  sprintf(strsql,"select naimat from Material where kodm=%s",kodmat);  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);  
  sprintf(strsql,"%s %s",kodmat,row[0]);

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,strsql),iceb_u_kolbait(20,strsql),strsql);
 }
fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,gettext("Итого")),iceb_u_kolbait(20,gettext("Итого")),gettext("Итого"));
fprintf(ff,"\n");

fprintf(ff,"        |                    |");

//Пишем вторую строку с единицами измерения

for(int ii=0; ii < kolih_km; ii++)
 {
  iceb_u_polen(kodmat_ei->ravno(ii),kodmat,sizeof(kodmat),2,'|');

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,kodmat),iceb_u_kolbait(20,kodmat),kodmat);
 }
fprintf(ff,"%-20.20s|","");
fprintf(ff,"\n");

fprintf(ff,"        |                    |");

//строка разделения

for(int ii=0; ii < kolih_km+1; ii++)
 {
  fprintf(ff,"---------------------");
 }
fprintf(ff,"\n");


fprintf(ff,"        |                    |");


for(int ii=0; ii < kolih_km+1; ii++)
 {
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_u_kolbait(9,gettext("Количество")),
  iceb_u_kolbait(9,gettext("Количество")),
  gettext("Количество"),
  iceb_u_kolbait(10,gettext("Сумма")),
  iceb_u_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));
 }
fprintf(ff,"\n");

spis_p_kontr_strp(kolih_km,0,ff);
char kontr[54];
//Распечатываем массив
for(int skontr=0; skontr < spis_kontr->kolih(); skontr++)
 {
  if(iceb_u_SRAV(spis_kontr->ravno(skontr),"00-",1) == 0)
   strcpy(kontr,"00");
  else
   strcpy(kontr,spis_kontr->ravno(skontr));
  
  memset(naim,'\0',sizeof(naim));
  //узнаём наименование контрагента
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);  

  fprintf(ff,"%-*s|%-*.*s|",
  iceb_u_kolbait(8,spis_kontr->ravno(skontr)),spis_kontr->ravno(skontr),
  iceb_u_kolbait(20,naim),iceb_u_kolbait(20,naim),naim);
  
  double itog_str_k=0.;
  double itog_str_s=0.;
  for(int ii=0; ii < kolih_km; ii++)
   {
    double kolih=m_kolih->ravno(skontr*kolih_km+ii);
    double suma=m_summa->ravno(skontr*kolih_km+ii);

    if(kolih == 0.)
      fprintf(ff,"%9s|","");
    else
      fprintf(ff,"%9.9g|",kolih);

    if(suma == 0.)
      fprintf(ff,"%10s|","");
    else
      fprintf(ff,"%10.2f|",suma);
    itog_str_k+=kolih;
    itog_str_s+=suma;
   }
  fprintf(ff,"%9.9g|%10.2f|",itog_str_k,itog_str_s);
  fprintf(ff,"\n");    
  
 }

spis_p_kontr_strp(kolih_km,0,ff);

fprintf(ff,"%*s|",iceb_u_kolbait(29,gettext("Итого")),gettext("Итого"));

//Распечатка итоговой строки
double itogo_po_kol_kolih=0.;
double itogo_po_kol_suma=0.;
double itogo_k=0.;
double itogo_s=0.;

for(int ii=0; ii < kolih_km; ii++)
 {
  itogo_po_kol_kolih=0.;
  itogo_po_kol_suma=0.;
  for(int skontr=0; skontr < spis_kontr->kolih(); skontr++)
   {
    itogo_po_kol_kolih+=m_kolih->ravno(skontr*kolih_km+ii);
    itogo_po_kol_suma+=m_summa->ravno(skontr*kolih_km+ii);
   }
  fprintf(ff,"%9.9g|%10.2f|",itogo_po_kol_kolih,itogo_po_kol_suma);
  itogo_k+=itogo_po_kol_kolih;
  itogo_s+=itogo_po_kol_suma;  
 }
fprintf(ff,"%9.9g|%10.2f|",itogo_k,itogo_s);
fprintf(ff,"\n");

}


/*******************************************/
/*Распечатка результатов расчёта*/
/***********************************/
void spis_p_kontr_rasrr2(class iceb_u_spisok *kodmat_ei,
class iceb_u_spisok *spis_kontr,
class iceb_u_double *m_kolih,
class iceb_u_double *m_summa,
double *itogo_k,
double *itogo_s,
FILE *ff,
GtkWidget *wpredok)
{

char strsql[512];
char kodmat[30];
char naim[512];
SQL_str row;
SQLCURSOR cur;
char kontr[50];
int kolih_kontr=spis_kontr->kolih();
int kolih_km=kodmat_ei->kolih();


spis_p_kontr_strp(kolih_kontr,1,ff);


fprintf(ff," Код |Наименование материалла|Ед.из.|");
//          12345 12345678901234567890123 123456     
//Пишем первую строку с наименованиями контрагентов
for(int ii=0; ii < kolih_kontr; ii++)
 {
  if(iceb_u_SRAV(spis_kontr->ravno(ii),"00-",1) == 0)
   strcpy(kontr,"00");
  else
   strcpy(kontr,spis_kontr->ravno(ii));
  
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);  

  sprintf(strsql,"%s %s",kontr,row[0]);

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,strsql),iceb_u_kolbait(20,strsql),strsql);
 }
fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,gettext("Итого")),iceb_u_kolbait(20,gettext("Итого")),gettext("Итого"));
fprintf(ff,"\n");


fprintf(ff,"     |                       |      |");

//строка разделения

for(int ii=0; ii < kolih_kontr+1; ii++)
 {
  fprintf(ff,"---------------------");
 }
fprintf(ff,"\n");


fprintf(ff,"     |                       |      |");


for(int ii=0; ii < kolih_kontr+1; ii++)
 {
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_u_kolbait(9,gettext("Количество")),
  iceb_u_kolbait(9,gettext("Количество")),
  gettext("Количество"),
  iceb_u_kolbait(10,gettext("Сумма")),
  iceb_u_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));
 }
fprintf(ff,"\n");

spis_p_kontr_strp(kolih_kontr,1,ff);
char ei[32];
//Распечатываем массив
for(int skontr=0; skontr < kodmat_ei->kolih(); skontr++)
 {
  iceb_u_polen(kodmat_ei->ravno(skontr),kodmat,sizeof(kodmat),1,'|');
  iceb_u_polen(kodmat_ei->ravno(skontr),ei,sizeof(ei),2,'|');

  memset(naim,'\0',sizeof(naim));

  sprintf(strsql,"select naimat from Material where kodm=%s",kodmat);  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);  

  fprintf(ff,"%-5s|%-*.*s|%-*.*s|",
  kodmat,
  iceb_u_kolbait(23,naim),iceb_u_kolbait(23,naim),naim,
  iceb_u_kolbait(6,ei),iceb_u_kolbait(6,ei),ei);

  double itog_str_k=0.;
  double itog_str_s=0.;
  for(int ii=0; ii < kolih_kontr; ii++)
   {
    double kolih=m_kolih->ravno(ii*kolih_km+skontr);
    double suma=m_summa->ravno(ii*kolih_km+skontr);

    if(kolih == 0.)
      fprintf(ff,"%9s|","");
    else
      fprintf(ff,"%9.9g|",kolih);

    if(suma == 0.)
      fprintf(ff,"%10s|","");
    else
      fprintf(ff,"%10.2f|",suma);
    itog_str_k+=kolih;
    itog_str_s+=suma;
   }
  fprintf(ff,"%9.9g|%10.2f|",itog_str_k,itog_str_s);
  fprintf(ff,"\n");    
  
 }

spis_p_kontr_strp(kolih_kontr,1,ff);

fprintf(ff,"%*s|",iceb_u_kolbait(36,gettext("Итого")),gettext("Итого"));

//Распечатка итоговой строки
double itogo_po_kol_kolih=0.;
double itogo_po_kol_suma=0.;
*itogo_k=0.;
*itogo_s=0.;

for(int ii=0; ii < kolih_kontr; ii++)
 {
  itogo_po_kol_kolih=0.;
  itogo_po_kol_suma=0.;
  for(int skontr=0; skontr < kodmat_ei->kolih(); skontr++)
   {
    itogo_po_kol_kolih+=m_kolih->ravno(ii*kolih_km+skontr);
    itogo_po_kol_suma+=m_summa->ravno(ii*kolih_km+skontr);
   }
  fprintf(ff,"%9.9g|%10.2f|",itogo_po_kol_kolih,itogo_po_kol_suma);
  *itogo_k+=itogo_po_kol_kolih;
  *itogo_s+=itogo_po_kol_suma;  
 }
fprintf(ff,"%9.9g|%10.2f|",*itogo_k,*itogo_s);

fprintf(ff,"\n");


}



/*******************************/
/*Печать реквизитов поиска*/
/****************************/
void spis_p_kont_rekpoi(const char *sklad,
const char *grup_mat,
const char *kodmat,
const char *kontr,
const char *kodop,
const char *shetz,
const char *nomdokp,
FILE *ff)
{
if(sklad[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Склад"),sklad);
if(grup_mat[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Группа"),grup_mat);
if(kodmat[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код материалла"),kodmat);
if(kontr[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код контрагента"),kontr);
if(kodop[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код операции"),kodop);
if(shetz[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),shetz);
if(nomdokp[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Номер документа"),nomdokp);

}
