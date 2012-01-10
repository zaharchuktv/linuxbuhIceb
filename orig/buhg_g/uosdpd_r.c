/*$Id: uosdpd_r.c,v 1.10 2011-02-21 07:35:58 sasa Exp $*/
/*12.03.2010	13.01.2008	Белых А.И.	uosdpd_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "uosdpd.h"

class uosdpd_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class uosdpd_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  uosdpd_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosdpd_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosdpd_r_data *data);
gint uosdpd_r1(class uosdpd_r_data *data);
void  uosdpd_r_v_knopka(GtkWidget *widget,class uosdpd_r_data *data);

void	sapkasord(short dd,short md,short gd,int nomlist,FILE *ffva);
void   hgrkd(FILE *ff);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern int      kol_strok_na_liste;

int uosdpd_r(class uosdpd_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class uosdpd_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать движение по документам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uosdpd_r_key_press),&data);

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

repl.plus(gettext("Распечатать движение по документам"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(uosdpd_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)uosdpd_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosdpd_r_v_knopka(GtkWidget *widget,class uosdpd_r_data *data)
{
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosdpd_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosdpd_r_data *data)
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
/**************/
/*Шапка файлов*/
/***************/
void	sapdvos(FILE *ff,const char *kodolz,const char *podrz,const char *kodop,
int vidop,const char *grupabuh,const char *grupanal,const char *shetu_p)
{


if(kodolz[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Мат.-ответственный"),kodolz);
if(podrz[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),podrz);
if(kodop[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Код операции"),kodop);
if(vidop != 0)
  fprintf(ff,"%s: %d\n",gettext("Документы"),vidop);
if(grupabuh[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Группа бухгалтерского учета"),grupabuh);
if(grupanal[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Группа налогового учета"),grupanal);
if(shetu_p[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Счёт учёта"),shetu_p);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Дата докум. |Инвент. |  Наименование      |  Налоговый учет       |Бухгалтерский учет     |Ном. |Код. |Подр-|Мат. |Группа|Группа|\n\
            | номер  |                    |Бал.стоим. |   Износ   |Бал.стоим. |   Износ   |докум|опер.|аздел|отвец|бух.уч|нал.уч|\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
}


/******************************************/
/******************************************/

gint uosdpd_r1(class uosdpd_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;

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


int kolstr=0;
SQLCURSOR curr;
sprintf(strsql,"select datd,tipz,innom,nomd,podr,kodol,bs,iz,kodop,\
bsby,izby from Uosdok1 where datd >= '%d-%d-%d' and datd <= '%d-%d-%d' \
order by datd asc",
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
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafsort[56];
sprintf(imafsort,"dvos%d.tmp",getpid());
FILE *ffsort;
if((ffsort = fopen(imafsort,"w")) == NULL)
 {
  iceb_er_op_fil(imafsort,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf[56];
sprintf(imaf,"dvos%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_zagolov(gettext("Распечатать движение по документам"),dn,mn,gn,dk,mk,gk,organ,ff);

sapdvos(ff,data->rk->mat_ot.ravno(),data->rk->podr.ravno(),data->rk->kod_op.ravno(),data->rk->prih_rash,data->rk->grup_bu.ravno(),data->rk->grup_nu.ravno(),data->rk->shetu.ravno());

float kolstr1=0.;
short d,m,g;
int tipz=0;
char		vidopt[5];
double		bsn,izn,bsb,izb;
double		ibsn=0.,iizn=0.,ibsb=0.,iizb=0.;
double		ibsnp=0.,iiznp=0.,ibsbp=0.,iizbp=0.;
double		ibsnr=0.,iiznr=0.,ibsbr=0.,iizbr=0.;
double		ibsngr=0.,iizngr=0.,ibsbgr=0.,iizbgr=0.;
double		ibsngro=0.,iizngro=0.,ibsbgro=0.,iizbgro=0.;
char naim[512];
SQL_str row1;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->podr.ravno(),row[4],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->mat_ot.ravno(),row[5],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->kod_op.ravno(),row[8],0,0) != 0)
    continue;
   
  if(data->rk->prih_rash == '2')
    if(row[1][0] == '1')
     continue;

  if(data->rk->prih_rash == 1)
    if(row[1][0] == '2')
     continue;

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  class poiinpdw_data rekin;
  poiinpdw(atol(row[2]),m,g,&rekin,data->window);

  if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),1,0) != 0)
       continue;
  if(iceb_u_proverka(data->rk->grup_bu.ravno(),rekin.hnaby.ravno(),0,0) != 0)
       continue;
  if(iceb_u_proverka(data->rk->grup_nu.ravno(),rekin.hna.ravno(),0,0) != 0)
       continue;

  tipz=atoi(row[1]);
  if(tipz == 1)
   strcpy(vidopt,"+");
  if(tipz == 2)
   strcpy(vidopt,"-");
   
  bsn=atof(row[6]);  
  izn=atof(row[7]);  
  bsb=atof(row[9]);  
  izb=atof(row[10]);  
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naim from Uosin where innom=%s",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    strncpy(naim,row1[0],sizeof(naim)-1);

  fprintf(ff,"%02d.%02d.%d %s %-8s %-*.*s %11.2f %11.2f %11.2f %11.2f \
%-*s %-*s %-5s %-5s %-*s %-*s\n",
  d,m,g,vidopt,row[2],
  iceb_u_kolbait(20,naim),iceb_u_kolbait(20,naim),naim,
  bsn,izn,bsb,izb,
  iceb_u_kolbait(5,row[3]),row[3],
  iceb_u_kolbait(5,row[8]),row[8],
  row[4],row[5],
  iceb_u_kolbait(6,rekin.hnaby.ravno()),rekin.hnaby.ravno(),
  iceb_u_kolbait(6,rekin.hna.ravno()),rekin.hna.ravno());

  fprintf(ffsort,"%s|%s|%d-%d-%d|%s|%s|%s|%.2f|%.2f|%.2f|%.2f|\
%s|%s|%s|%s|\n",
  rekin.hnaby.ravno(),rekin.hna.ravno(),g,m,d,vidopt,row[2],naim,bsn,izn,bsb,izb,
  row[3],row[8],row[4],row[5]);

  ibsn+=bsn;
  iizn+=izn;
  ibsb+=bsb;
  iizb+=izb;
  if(tipz == 1)
   {
    ibsnp+=bsn;
    iiznp+=izn;
    ibsbp+=bsb;
    iizbp+=izb;
   }  
  if(tipz == 2)
   {
    ibsnr+=bsn;
    iiznr+=izn;
    ibsbr+=bsb;
    iizbr+=izb;
   }  
 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(42,gettext("Итого")),gettext("Итого"),ibsn,iizn,ibsb,iizb);

fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnp,iiznp,ibsbp,iizbp);

fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnr,iiznr,ibsbr,iizbr);

fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);

sprintf(strsql,"%s :%11.2f %11.2f %11.2f %11.2f\n",gettext("Итого"),ibsn,iizn,ibsb,iizb);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"\n%s+:%11.2f %11.2f %11.2f %11.2f\n",gettext("Итого"),ibsnp,iiznp,ibsbp,iizbp);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s :%11.2f %11.2f %11.2f %11.2f\n",gettext("Итого"),ibsnr,iiznr,ibsbr,iizbr);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

iceb_podpis(ff,data->window);

fclose(ff);
fclose(ffsort);

//Сортировка по группам бухгалтерского учета

//sprintf(strsql,"sort -o %s -t\\| +0n -1 +2 -3 %s",imafsort,imafsort);
sprintf(strsql,"sort -o %s -t\\| -k1,2n -k3,4 %s",imafsort,imafsort);
system(strsql);

if((ffsort = fopen(imafsort,"r")) == NULL)
 {
  iceb_er_op_fil(imafsort,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafgrbuh[56];
sprintf(imafgrbuh,"dvosbh%d.lst",getpid());

if((ff = fopen(imafgrbuh,"w")) == NULL)
 {
  iceb_er_op_fil(imafgrbuh,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_zagolov(gettext("Распечатать движение по документам"),dn,mn,gn,dk,mk,gk,organ,ff);

fprintf(ff,"%s\n",gettext("Сортировка по группам бухгалтерского учета"));

//sapdvos(ff,kodolz,podrz,kodop,vidop,grupabuh,grupanal,shetu_p);
sapdvos(ff,data->rk->mat_ot.ravno(),data->rk->podr.ravno(),data->rk->kod_op.ravno(),data->rk->prih_rash,data->rk->grup_bu.ravno(),data->rk->grup_nu.ravno(),data->rk->shetu.ravno());
char grbuhz[32];
char grbuh[32];
char		grnal[32],grnalz[32];
char		datadok[11];
char		invnom[32];
char 		nalbs[32];
char		naliz[32];
char 		buhbs[32];
char		buhiz[32];
char		nomd[32];
char		kodoper[32];
char		podr[16];
char		matot[16];
memset(grbuhz,'\0',sizeof(grbuhz));

double ibsnpo=0.,iiznpo=0.,ibsbpo=0.,iizbpo=0.;
double ibsnro=0.,iiznro=0.,ibsbro=0.,iizbro=0.;
ibsnp=iiznp=ibsbp=iizbp=0.;
ibsnr=iiznr=ibsbr=iizbr=0.;
char stroka[1024];
while(fgets(stroka,sizeof(stroka),ffsort) != 0)
 {
  iceb_u_polen(stroka,grbuh,sizeof(grbuh),1,'|');
  if(iceb_u_SRAV(grbuhz,grbuh,0) != 0)
   {
    if(grbuhz[0] != '\0')
     {
      sprintf(strsql,"%s %s",gettext("Итого по группе"),grbuhz);
      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(42,strsql),strsql,ibsngr,iizngr,ibsbgr,iizbgr);
      
      ibsngro+=ibsngr;
      iizngro+=iizngr;
      ibsbgro+=ibsbgr;
      iizbgro+=iizbgr;
      
      fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnp,iiznp,ibsbp,iizbp);

      fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnr,iiznr,ibsbr,iizbr);

      fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n\n",
      iceb_u_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);
     }
    strcpy(grbuhz,grbuh);
    ibsngr=iizngr=ibsbgr=iizbgr=0.;
    ibsnp=iiznp=ibsbp=iizbp=0.;
    ibsnr=iiznr=ibsbr=iizbr=0.;
   }

  iceb_u_polen(stroka,grnal,sizeof(grnal),2,'|');
  iceb_u_polen(stroka,datadok,sizeof(datadok),3,'|');
  iceb_u_rsdat(&d,&m,&g,datadok,2);
  iceb_u_polen(stroka,vidopt,sizeof(vidopt),4,'|');
  iceb_u_polen(stroka,invnom,sizeof(invnom),5,'|');
  iceb_u_polen(stroka,naim,sizeof(naim),6,'|');
  iceb_u_polen(stroka,nalbs,sizeof(nalbs),7,'|');
  iceb_u_polen(stroka,naliz,sizeof(naliz),8,'|');
  iceb_u_polen(stroka,buhbs,sizeof(buhbs),9,'|');
  iceb_u_polen(stroka,buhiz,sizeof(buhiz),10,'|');
  iceb_u_polen(stroka,nomd,sizeof(nomd),11,'|');
  iceb_u_polen(stroka,kodoper,sizeof(kodoper),12,'|');
  iceb_u_polen(stroka,podr,sizeof(podr),13,'|');
  iceb_u_polen(stroka,matot,sizeof(matot),14,'|');

  fprintf(ff,"%02d.%02d.%d %s %-8s %-*.*s %11s %11s %11s %11s \
%-*s %-*s %-5s %-5s %-6s %-6s\n",
  d,m,g,vidopt,invnom,
  iceb_u_kolbait(20,naim),iceb_u_kolbait(20,naim),naim,
  nalbs,naliz,buhbs,buhiz,
  iceb_u_kolbait(5,nomd),nomd,
  iceb_u_kolbait(5,kodoper),kodoper,
  podr,matot,
  grbuh,
  grnal);

  ibsngr+=atof(nalbs);
  iizngr+=atof(naliz);
  ibsbgr+=atof(buhbs);
  iizbgr+=atof(buhiz);
  if(vidopt[0] == '+')
   {
    ibsnp+=atof(nalbs);
    iiznp+=atof(naliz);
    ibsbp+=atof(buhbs);
    iizbp+=atof(buhiz);

    ibsnpo+=atof(nalbs);
    iiznpo+=atof(naliz);
    ibsbpo+=atof(buhbs);
    iizbpo+=atof(buhiz);
   }  
  if(vidopt[0] == '-')
   {
    ibsnr+=atof(nalbs);
    iiznr+=atof(naliz);
    ibsbr+=atof(buhbs);
    iizbr+=atof(buhiz);

    ibsnro+=atof(nalbs);
    iiznro+=atof(naliz);
    ibsbro+=atof(buhbs);
    iizbro+=atof(buhiz);
   }  

 }
sprintf(strsql,"%s %s",gettext("Итого по группе"),grbuhz);
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(42,strsql),strsql,ibsngr,iizngr,ibsbgr,iizbgr);

      ibsngro+=ibsngr;
      iizngro+=iizngr;
      ibsbgro+=ibsbgr;
      iizbgro+=iizbgr;

      fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnp,iiznp,ibsbp,iizbp);

      fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnr,iiznr,ibsbr,iizbr);

      fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n\n",
      iceb_u_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(42,gettext("Итого по всем группам")),gettext("Итого по всем группам"),ibsngro,iizngro,ibsbgro,iizbgro);

fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnpo,iiznpo,ibsbpo,iizbpo);

fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnro,iiznro,ibsbro,iizbro);

fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnpo-ibsnro,iiznpo-iiznro,ibsbpo-ibsbro,iizbpo-iizbro);

iceb_podpis(ff,data->window);

fclose(ff);
fclose(ffsort);

//Сортировка по группам налогового учета

//sprintf(strsql,"sort -o %s -t\\| +1n -2 +2 -3 %s",imafsort,imafsort);
sprintf(strsql,"sort -o %s -t\\| -k2,3n -k3,4 %s",imafsort,imafsort);
system(strsql);

if((ffsort = fopen(imafsort,"r")) == NULL)
 {
  iceb_er_op_fil(imafsort,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafgrnal[56];
sprintf(imafgrnal,"dvosnl%d.lst",getpid());

if((ff = fopen(imafgrnal,"w")) == NULL)
 {
  iceb_er_op_fil(imafgrnal,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_zagolov(gettext("Распечатать движение по документам"),dn,mn,gn,dk,mk,gk,organ,ff);

fprintf(ff,"%s\n",gettext("Сортировка по группам налогового учета"));

//sapdvos(ff,kodolz,podrz,kodop,vidop,grupabuh,grupanal,shetu_p);
sapdvos(ff,data->rk->mat_ot.ravno(),data->rk->podr.ravno(),data->rk->kod_op.ravno(),data->rk->prih_rash,data->rk->grup_bu.ravno(),data->rk->grup_nu.ravno(),data->rk->shetu.ravno());
memset(grnalz,'\0',sizeof(grnalz));

ibsngr=iizngr=ibsbgr=iizbgr=0.;
ibsngro=iizngro=ibsbgro=iizbgro=0.;
ibsnpo=0.,iiznpo=0.,ibsbpo=0.,iizbpo=0.;
ibsnro=0.,iiznro=0.,ibsbro=0.,iizbro=0.;
ibsnp=iiznp=ibsbp=iizbp=0.;
ibsnr=iiznr=ibsbr=iizbr=0.;

while(fgets(stroka,sizeof(stroka),ffsort) != 0)
 {
  iceb_u_polen(stroka,grbuh,sizeof(grbuh),1,'|');

  iceb_u_polen(stroka,grnal,sizeof(grnal),2,'|');
  if(iceb_u_SRAV(grnalz,grnal,0) != 0)
   {
    if(grnalz[0] != '\0')
     {
      sprintf(strsql,"%s %s",gettext("Итого по группе"),grnalz);
      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(42,strsql),strsql,ibsngr,iizngr,ibsbgr,iizbgr);

      ibsngro+=ibsngr;
      iizngro+=iizngr;
      ibsbgro+=ibsbgr;
      iizbgro+=iizbgr;

      fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnp,iiznp,ibsbp,iizbp);

      fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnr,iiznr,ibsbr,iizbr);

      fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n\n",
      iceb_u_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);
     }
    strcpy(grnalz,grnal);
    ibsngr=iizngr=ibsbgr=iizbgr=0.;
    ibsnp=iiznp=ibsbp=iizbp=0.;
    ibsnr=iiznr=ibsbr=iizbr=0.;
   }
  iceb_u_polen(stroka,datadok,sizeof(datadok),3,'|');
  iceb_u_rsdat(&d,&m,&g,datadok,2);
  iceb_u_polen(stroka,vidopt,sizeof(vidopt),4,'|');
  iceb_u_polen(stroka,invnom,sizeof(invnom),5,'|');
  iceb_u_polen(stroka,naim,sizeof(naim),6,'|');
  iceb_u_polen(stroka,nalbs,sizeof(nalbs),7,'|');
  iceb_u_polen(stroka,naliz,sizeof(naliz),8,'|');
  iceb_u_polen(stroka,buhbs,sizeof(buhbs),9,'|');
  iceb_u_polen(stroka,buhiz,sizeof(buhiz),10,'|');
  iceb_u_polen(stroka,nomd,sizeof(nomd),11,'|');
  iceb_u_polen(stroka,kodoper,sizeof(kodoper),12,'|');
  iceb_u_polen(stroka,podr,sizeof(podr),13,'|');
  iceb_u_polen(stroka,matot,sizeof(matot),14,'|');

  fprintf(ff,"%02d.%02d.%d %s %-8s %-*.*s %11s %11s %11s %11s \
%-*s %-*s %-5s %-5s %-6s %-6s\n",
  d,m,g,vidopt,invnom,
  iceb_u_kolbait(20,naim),iceb_u_kolbait(20,naim),naim,
  nalbs,naliz,buhbs,buhiz,
  iceb_u_kolbait(5,nomd),nomd,
  iceb_u_kolbait(5,kodoper),kodoper,
  podr,matot,grbuh,grnal);

  ibsngr+=atof(nalbs);
  iizngr+=atof(naliz);
  ibsbgr+=atof(buhbs);
  iizbgr+=atof(buhiz);
  if(vidopt[0] == '+')
   {
    ibsnp+=atof(nalbs);
    iiznp+=atof(naliz);
    ibsbp+=atof(buhbs);
    iizbp+=atof(buhiz);

    ibsnpo+=atof(nalbs);
    iiznpo+=atof(naliz);
    ibsbpo+=atof(buhbs);
    iizbpo+=atof(buhiz);
   }  
  if(vidopt[0] == '-')
   {
    ibsnr+=atof(nalbs);
    iiznr+=atof(naliz);
    ibsbr+=atof(buhbs);
    iizbr+=atof(buhiz);

    ibsnro+=atof(nalbs);
    iiznro+=atof(naliz);
    ibsbro+=atof(buhbs);
    iizbro+=atof(buhiz);
   }  

 }
sprintf(strsql,"%s %s",gettext("Итого по группе"),grnalz);
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(42,strsql),strsql,ibsngr,iizngr,ibsbgr,iizbgr);

      ibsngro+=ibsngr;
      iizngro+=iizngr;
      ibsbgro+=ibsbgr;
      iizbgro+=iizbgr;

      fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnp,iiznp,ibsbp,iizbp);

      fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnr,iiznr,ibsbr,iizbr);

      fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n\n",
      iceb_u_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(42,gettext("Итого по всем группам")),gettext("Итого по всем группам"),ibsngro,iizngro,ibsbgro,iizbgro);

fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnpo,iiznpo,ibsbpo,iizbpo);

fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnro,iiznro,ibsbro,iizbro);

fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnpo-ibsnro,iiznpo-iiznro,ibsbpo-ibsbro,iizbpo-iizbro);

iceb_podpis(ff,data->window);

fclose(ff);
fclose(ffsort);


unlink(imafsort);

data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Реестр документов движения основных средств"));

data->rk->imaf.plus(imafgrbuh);
data->rk->naim.plus(gettext("Реестр документов с сортировкой по группам бух. учёта"));

data->rk->imaf.plus(imafgrnal);
data->rk->naim.plus(gettext("Реестр документов с сортировкой по группам налогового учёта"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);
 
gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
