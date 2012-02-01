/*$Id: buhkomw_r.c,v 1.21 2011-02-21 07:35:51 sasa Exp $*/
/*09.11.2009	07.06.2005	Белых А.И. 	buhkomw_r.c
Расчёт прогноза реализации 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "buhkomw.h"

class buhkomw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class buhkomw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  buhkomw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buhkomw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhkomw_r_data *data);
gint buhkomw_r1(class buhkomw_r_data *data);
void  buhkomw_r_v_knopka(GtkWidget *widget,class buhkomw_r_data *data);

int prov_kom(class iceb_u_spisok *koment,char *row,int kol_koment);
void buhkom_it(int metka_i,double deb,double kre,FILE *ff);
void buhkom_it_dk(int metka_i,double deb,FILE *ff);
void buhkom_sh_kre(char *shet,char *naim,FILE *ff);
void buhkom_sh(char *shet,char *naim,FILE *ff);
void buhkom_sh_deb(char *shet,char *naim,FILE *ff);
void buhkom_sh_kre(char *shet,char *naim,FILE *ff);
void buhkomw_ri(double dbt,double krt,int metka,FILE *ff1);
void	buhkomw_rs(FILE *ff1);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
//extern int kol_strok_na_liste;

int buhkomw_r(class buhkomw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class buhkomw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать отчёт по комментариям"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(buhkomw_r_key_press),&data);

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

repl.plus(gettext("Распечатать отчёт по комментариям"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(buhkomw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)buhkomw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buhkomw_r_v_knopka(GtkWidget *widget,class buhkomw_r_data *data)
{
//printf("buhkomw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   buhkomw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhkomw_r_data *data)
{
// printf("buhkomw_r_key_press\n");
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

gint buhkomw_r1(class buhkomw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
sprintf(strsql,"%d.%d.%d => %d.%d.%d\n",dn,mn,gn,dk,mk,gk);
iceb_printw(strsql,data->buffer,data->view);


SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

sprintf(strsql,"select str from Alx where fil='buhkom.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"buhkom.alx");
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_spisok koment;
char bros[500];

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;


  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') != 0) 
   continue;
  koment.plus(bros);
 }
 
int kol_koment=koment.kolih();
if(kol_koment == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной настройки !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
SQL_str row;
SQLCURSOR cur; 

sprintf(strsql,"select sh,shk,deb,kre,komen from Prov \
where val=0 and datp >= '%d-%02d-%02d' and \
datp <= '%d-%02d-%02d' and komen <> ''",gn,mn,dn,gk,mk,dk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено проводок !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int nomer=0;
float kolstr1=0.;
class iceb_u_spisok sh_shk_kom;
double deb,kre;
iceb_u_double debet;
iceb_u_double kredit;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
//  refresh();
  
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);
  
  if(iceb_u_proverka(data->rk->shet.ravno(),row[0],0,0) != 0)
    continue;
//  if(iceb_u_proverka(sheta,row[1],0,0) != 0)
//    continue;
  
  if((nomer=prov_kom(&koment,row[4],kol_koment)) < 0)
   continue;
//  printw("Подходит %s\n",koment.ravno(nomer));
//  OSTANOV();
  deb=atof(row[2]);
  kre=atof(row[3]);
  
  sprintf(strsql,"%s|%s|%s",row[0],koment.ravno(nomer),row[1]);
  if((nomer=sh_shk_kom.find(strsql)) < 0)
    sh_shk_kom.plus(strsql);
  debet.plus(deb,nomer);
  kredit.plus(kre,nomer);    
 }

int kol_shet=sh_shk_kom.kolih();
if(kol_shet == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной проводки с указанными комментариями !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_sort[40];
sprintf(imaf_sort,"kom%d.tmp",getpid());
FILE *ff;

if((ff = fopen(imaf_sort,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_sort,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

for(int i=0; i < kol_shet ; i++)
 fprintf(ff,"%s|%.2f|%.2f|\n",sh_shk_kom.ravno(i),debet.ravno(i),kredit.ravno(i));

fclose(ff);

//sprintf(strsql,"sort -o %s -t\\|  +0 -1 +1 -2  %s",imaf_sort,imaf_sort);
sprintf(strsql,"sort -o %s -t\\|  -k1,2 -k2,3  %s",imaf_sort,imaf_sort);

system(strsql);

FILE *ff_sort;
if((ff_sort = fopen(imaf_sort,"r")) == NULL)
 {
  iceb_er_op_fil(imaf_sort,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

unlink(imaf_sort);

char imaf[40];
sprintf(imaf,"kom%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

data->rk->imaf.new_plus(imaf);
data->rk->naimf.new_plus(gettext("Свод по комментариям (дебет и кредит)"));

iceb_u_zagolov(gettext("Отчёт по комментариям"),dn,mn,gn,dk,mk,gk,organ,ff);

if(data->rk->shet.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());

char shet[32];
char shet_z[32];
char shetk[32];
char komentar[512];
char komentar_z[512];
double ideb=0.,ikre=0.;
double idebk=0.,ikrek=0.;
char stroka[1024];
char naim[512];

memset(shet_z,'\0',sizeof(shet_z));
memset(komentar_z,'\0',sizeof(komentar_z));
//распечатываем дебет и кредит
idebk=ikrek=0.;
ideb=ikre=0.;
while(fgets(stroka,sizeof(stroka),ff_sort) != NULL)
 {
  iceb_u_polen(stroka,shet,sizeof(shet),1,'|');
  iceb_u_polen(stroka,komentar,sizeof(komentar),2,'|');
  iceb_u_polen(stroka,shetk,sizeof(shetk),3,'|');
  iceb_u_polen(stroka,strsql,sizeof(strsql),4,'|');
  deb=atof(strsql);
  iceb_u_polen(stroka,strsql,sizeof(strsql),5,'|');
  kre=atof(strsql);
    
  if(iceb_u_SRAV(komentar_z,komentar,0) != 0)
   {
    if(komentar_z[0] != '\0')
      buhkom_it(0,idebk,ikrek,ff);
    idebk=ikrek=0.;
    strcpy(komentar_z,komentar);
   }

  if(iceb_u_SRAV(shet,shet_z,0) != 0)
   {
    if(shet_z[0] != '\0')
     {
      if(idebk != 0. || ikrek != 0.)
       {
        buhkom_it(0,idebk,ikrek,ff);
        idebk=ikrek=0.;
       }
      buhkom_it(1,ideb,ikre,ff);
     }
    memset(naim,'\0',sizeof(naim));
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet);
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     strncpy(naim,row[0],sizeof(naim)-1);

    buhkom_sh(shet,naim,ff);
     
    ideb=ikre=0.;
    strcpy(shet_z,shet);
   }

  fprintf(ff,"%-*s %-*s %10.2f %10.2f\n",
  iceb_u_kolbait(10,komentar),komentar,
  iceb_u_kolbait(7,shetk),shetk,
  deb,kre);
  
  ideb+=deb;
  ikre+=kre;
  idebk+=deb;
  ikrek+=kre;

 }
buhkom_it(0,idebk,ikrek,ff);
buhkom_it(1,ideb,ikre,ff);

iceb_podpis(ff,data->window);
fclose(ff);

rewind(ff_sort);

char imaf_deb[40];
sprintf(imaf_deb,"komd%d.lst",getpid());
if((ff = fopen(imaf_deb,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_deb,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.plus(imaf_deb);
data->rk->naimf.plus(gettext("Свод по комментариям (дебет)"));
iceb_u_zagolov(gettext("Отчёт по комментариям"),dn,mn,gn,dk,mk,gk,organ,ff);

if(data->rk->shet.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());


memset(shet_z,'\0',sizeof(shet_z));
memset(komentar_z,'\0',sizeof(komentar_z));
//распечатываем дебет
while(fgets(stroka,sizeof(stroka),ff_sort) != NULL)
 {

  iceb_u_polen(stroka,strsql,sizeof(strsql),4,'|');
  deb=atof(strsql);
  if(deb == 0.)
   continue;

  iceb_u_polen(stroka,shet,sizeof(shet),1,'|');
  iceb_u_polen(stroka,komentar,sizeof(komentar),2,'|');
  iceb_u_polen(stroka,shetk,sizeof(shetk),3,'|');
    
  if(iceb_u_SRAV(komentar_z,komentar,0) != 0)
   {
    if(komentar_z[0] != '\0')
      buhkom_it_dk(0,idebk,ff);
    idebk=ikrek=0.;
    strcpy(komentar_z,komentar);
   }

  if(iceb_u_SRAV(shet,shet_z,0) != 0)
   {
    if(shet_z[0] != '\0')
     {
      if(idebk != 0. || ikrek != 0.)
       {
        buhkom_it_dk(0,idebk,ff);
        idebk=ikrek=0.;
       }
      buhkom_it_dk(1,ideb,ff);
     }
    memset(naim,'\0',sizeof(naim));
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet);
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     strncpy(naim,row[0],sizeof(naim)-1);

    buhkom_sh_deb(shet,naim,ff);
     
    ideb=ikre=0.;
    strcpy(shet_z,shet);
   }

  fprintf(ff,"%-*s %-*s %10.2f\n",
  iceb_u_kolbait(10,komentar),komentar,
  iceb_u_kolbait(7,shetk),shetk,deb);
  
  ideb+=deb;
  idebk+=deb;

 }
buhkom_it_dk(0,idebk,ff);
buhkom_it_dk(1,ideb,ff);


iceb_podpis(ff,data->window);
fclose(ff);

rewind(ff_sort);

char imaf_kre[40];
sprintf(imaf_kre,"komk%d.lst",getpid());
if((ff = fopen(imaf_kre,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_kre,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.plus(imaf_kre);
data->rk->naimf.plus(gettext("Свод по комментариям (кредит)"));
iceb_u_zagolov(gettext("Отчёт по комментариям"),dn,mn,gn,dk,mk,gk,organ,ff);

if(data->rk->shet.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());


memset(shet_z,'\0',sizeof(shet_z));
memset(komentar_z,'\0',sizeof(komentar_z));
//распечатываем кредит
while(fgets(stroka,sizeof(stroka),ff_sort) != NULL)
 {
  iceb_u_polen(stroka,strsql,sizeof(strsql),5,'|');
  kre=atof(strsql);
  if(kre == 0.)
   continue;

  iceb_u_polen(stroka,shet,sizeof(shet),1,'|');
  iceb_u_polen(stroka,komentar,sizeof(komentar),2,'|');
  iceb_u_polen(stroka,shetk,sizeof(shetk),3,'|');
    
  if(iceb_u_SRAV(komentar_z,komentar,0) != 0)
   {
    if(komentar_z[0] != '\0')
      buhkom_it_dk(0,ikrek,ff);
    ikrek=0.;
    strcpy(komentar_z,komentar);
   }

  if(iceb_u_SRAV(shet,shet_z,0) != 0)
   {
    if(shet_z[0] != '\0')
     {
      if(ikrek != 0.)
       {
        buhkom_it_dk(0,ikrek,ff);
        ikrek=0.;
       }
      buhkom_it_dk(1,ikre,ff);
     }
    memset(naim,'\0',sizeof(naim));
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet);
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     strncpy(naim,row[0],sizeof(naim)-1);

    buhkom_sh_kre(shet,naim,ff);
     
    ikre=0.;
    strcpy(shet_z,shet);
   }

  fprintf(ff,"%-*s %-*s %10.2f\n",
  iceb_u_kolbait(10,komentar),komentar,
  iceb_u_kolbait(7,shetk),shetk,kre);
  
  ikre+=kre;
  ikrek+=kre;

 }
buhkom_it_dk(0,ikrek,ff);
buhkom_it_dk(1,ikre,ff);


iceb_podpis(ff,data->window);
fclose(ff);




//Распечатываем проводки

rewind(ff_sort);

char imaf_prov[40];
sprintf(imaf_prov,"komp%d.lst",getpid());
if((ff = fopen(imaf_prov,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_prov,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.plus(imaf_prov);
data->rk->naimf.plus(gettext("Отчёт по комментариям"));
iceb_u_zagolov(gettext("Отчёт по комментариям"),dn,mn,gn,dk,mk,gk,organ,ff);

if(data->rk->shet.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());

short d,m,g;
char naikon[512];
SQL_str row1;
SQLCURSOR cur1; 
kolstr1=0;
ideb=ikre=0.;
memset(shet_z,'\0',sizeof(shet_z));
memset(komentar_z,'\0',sizeof(komentar_z));
char komentar_prov[512];

while(fgets(stroka,sizeof(stroka),ff_sort) != NULL)
 {
//  strzag(LINES-1,0,kol_shet,++kolstr1);
  iceb_pbar(data->bar,kol_shet,++kolstr1);
  
  iceb_u_polen(stroka,shet,sizeof(shet),1,'|');
  iceb_u_polen(stroka,komentar,sizeof(komentar),2,'|');
  iceb_u_polen(stroka,shetk,sizeof(shetk),3,'|');

  if(iceb_u_SRAV(komentar_z,komentar,0) != 0)
   {
    if(komentar_z[0] != '\0')
      buhkomw_ri(idebk,ikrek,0,ff);
    idebk=ikrek=0.;
    strcpy(komentar_z,komentar);
   }

  if(iceb_u_SRAV(shet,shet_z,0) != 0)
   {
    if(shet_z[0] != '\0')
     {
      if(idebk != 0. || ikrek != 0.)
       {
        buhkomw_ri(idebk,ikrek,0,ff);
        idebk=ikrek=0.;
       }
      buhkomw_ri(ideb,ikre,1,ff);
     }
    memset(naim,'\0',sizeof(naim));
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet);
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     strncpy(naim,row[0],sizeof(naim)-1);

    fprintf(ff,"\n%s %s\n",shet,naim);
    buhkomw_rs(ff);
     
    ideb=ikre=0.;
    strcpy(shet_z,shet);
   }
    
  sprintf(strsql,"select * from Prov \
where val=0 and datp >= '%d-%02d-%02d' and \
datp <= '%d-%02d-%02d' and sh='%s' and shk='%s'  and komen <> ''",gn,mn,dn,gk,mk,dk,shet,shetk);
  int kolstr;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

  if(kolstr == 0)
   continue;

  while(cur.read_cursor(&row) != 0)
   {
    if(row[13][0] == '\0')
     continue;
    if(iceb_u_strstrm(row[13],komentar) == 0)
      continue;
    iceb_u_rsdat(&d,&m,&g,row[1],2);
    deb=atof(row[9]);
    kre=atof(row[10]);
    ideb+=deb;
    ikre+=kre;
    idebk+=deb;
    ikrek+=kre;
    
    memset(naikon,'\0',sizeof(naikon));;
    
    if(row[4][0] != '\0')
     {  
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
      row[4]);
      int i;
      if((i=sql_readkey(&bd,strsql,&row1,&cur1)) != 1)
       {
        printf("%02d.%02d.%d %-5s %-5s %-5s %-3s %-4s %10.2f %10.2f %s%-10.10s\n\
  Error %d\n",      
        d,m,g,row[2],row[3],row[4],row[5],row[6],
        deb,kre,row[8],row[13],i);

        sprintf(strsql,"%s %s",gettext("Не найден код контрагента"),row[4]);
        iceb_menu_soob(strsql,data->window);

       }
      else
       strncpy(naikon,row1[0],sizeof(naikon)-1);
     }       

    sozkom(komentar_prov,sizeof(komentar_prov),row[13],row[5],row[8]);

    fprintf(ff,"%02d.%02d.%4d %-*s %-*s %-*s %-3s%4s %-*s %10s",
    d,m,g,
    iceb_u_kolbait(6,row[2]),row[2],
    iceb_u_kolbait(6,row[3]),row[3],
    iceb_u_kolbait(5,row[4]),row[4],
    row[5],row[11],
    iceb_u_kolbait(10,row[6]),row[6],
    iceb_u_prnbr(deb));

    fprintf(ff," %10s %s %-*s %s\n",
    iceb_u_prnbr(kre),
    row[8],
    iceb_u_kolbait(32,komentar_prov),komentar_prov,
    naikon);
   }   
 }
buhkomw_ri(idebk,ikrek,0,ff);
buhkomw_ri(ideb,ikre,1,ff);


iceb_podpis(ff,data->window);
fclose(ff);

fclose(ff_sort);

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
/**************************/
/*Проверка комментария*/
/***************************/
//Возвращаем номер комментария или -1 
int prov_kom(class iceb_u_spisok *koment,char *row,int kol_koment)
{
if(row[0] == '\0')
 return(-1);
for(int i=0; i < kol_koment;i++)
 {
//printw("проверяем %s | %s\n",row,koment->ravno(i));
  if(iceb_u_strstrm(row,koment->ravno(i)) != 0)
    return(i);
 }    
return(-1);

}
/***************************/
/*шапка*/
/**************************/

void buhkom_sh(char *shet,char *naim,FILE *ff)
{

fprintf(ff,"\n%s %s\n",shet,naim);
fprintf(ff,"\
-----------------------------------------\n");
fprintf(ff,gettext("\
Коментарий| Счёт  | Дебет    |  Кредит  |\n"));
fprintf(ff,"\
-----------------------------------------\n");

}
/***************************/
/*шапка*/
/**************************/

void buhkom_sh_deb(char *shet,char *naim,FILE *ff)
{

fprintf(ff,"\n%s %s\n",shet,naim);
fprintf(ff,"\
------------------------------\n");
fprintf(ff,gettext("\
Коментарий| Счёт  | Дебет    |\n"));
fprintf(ff,"\
------------------------------\n");

}
/***************************/
/*шапка*/
/**************************/

void buhkom_sh_kre(char *shet,char *naim,FILE *ff)
{

fprintf(ff,"\n%s %s\n",shet,naim);
fprintf(ff,"\
------------------------------\n");
fprintf(ff,gettext("\
Коментарий| Счёт  |  Кредит  |\n"));
fprintf(ff,"\
------------------------------\n");

}
/************************/
/*выдача итога*/
/*************************/
void buhkom_it(int metka_i,double deb,double kre,FILE *ff)
{
if(metka_i == 0)
  fprintf(ff,"%*s %10.2f %10.2f\n",
  iceb_u_kolbait(18,gettext("Итого")),gettext("Итого"),deb,kre);

if(metka_i == 1)
  fprintf(ff,"%*s %10.2f %10.2f\n",
  iceb_u_kolbait(18,gettext("Итого по счёту")),gettext("Итого по счёту"),deb,kre);
}

/************************/
/*выдача итога*/
/*************************/
void buhkom_it_dk(int metka_i,double deb,FILE *ff)
{
if(metka_i == 0)
  fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(18,gettext("Итого")),gettext("Итого"),deb);

if(metka_i == 1)
  fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(18,gettext("Итого по счёту")),gettext("Итого по счёту"),deb);
}
/********************************/
/*Распечатка итога*/
/********************************/

void buhkomw_ri(double dbt,double krt,int metka,FILE *ff1)
{
double	brr;

fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------------\n");

if(metka == 0)
  fprintf(ff1,"%*s:",iceb_u_kolbait(46,gettext("Итого по комментарию")),gettext("Итого по комментарию"));
if(metka == 1)
  fprintf(ff1,"%*s:",iceb_u_kolbait(46,gettext("Итого по счёту")),gettext("Итого по счёту"));

fprintf(ff1,"%13s",iceb_u_prnbr(dbt));
fprintf(ff1," %10s\n",iceb_u_prnbr(krt));

if(dbt >= krt)
 {
  brr=dbt-krt;
  fprintf(ff1,"\
%50s%10s\n"," ",iceb_u_prnbr(brr));
 }
else
 {
  brr=krt-dbt;
  fprintf(ff1,"\
%50s%10s %10s\n"," "," ",iceb_u_prnbr(brr));
 }
fprintf(ff1,"\n");

}
/*******/
/*Шапка*/
/*******/
void	buhkomw_rs(FILE *ff1)
{

fprintf(ff1,"\x1B\x4D"); /*12-знаков*/
fprintf(ff1,"\x0F");
fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,gettext("  Дата    | Счет.|Сч.к-т|Конт.| Кто   |Номер док.|   Дебет  |  Кредит  |      К о м е н т а р и й       |Наименование контрагента|\n"));

fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------------\n");

}
