/*$Id: zar_kart_r.c,v 1.17 2011-02-21 07:35:59 sasa Exp $*/
/*16.03.2010	07.12.2006	Белых А.И.	zar_kart_r.c
Распечатка карточек с начислениями и удержаниями за период
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_kart.h"

struct kk
 {
  char		fio[512];
  char		dolg[512];
  char          inn[32];
  int		podr;
  int		sovm;
  short		dn,mn,gn;
  short		dk,mk,gk;
 };

class zar_kart_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_kart_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_kart_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_kart_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_kart_r_data *data);
gint zar_kart_r1(class zar_kart_r_data *data);
void  zar_kart_r_v_knopka(GtkWidget *widget,class zar_kart_r_data *data);

void zarkartp(int tabn,struct kk kart,short mn,short gn,int kolmes,class iceb_u_int *MKN,class iceb_u_int *MKU,class iceb_u_double *MKND,class iceb_u_double *MKUD,double *saldon,class iceb_u_spisok *MKNS,class iceb_u_double *MKNSD,double *saldono1,double *saldonb1,class iceb_u_spisok *MKUS,class iceb_u_double *MKUSD,double *saldono2,double *saldonb2,FILE *ff,FILE *ffb,FILE *ffo,GtkWidget *view,GtkTextBuffer *buffer);
void rasotrvr(short mn,short gn,int kolmes,class iceb_u_int *MVT,class iceb_u_double *MVTRD,int metka,FILE *ff);
void vidtab(short mr,short gr,int tabnom,class iceb_u_int *MVT,class iceb_u_double *MVTRD,class iceb_u_double *MVTHS,class iceb_u_double *MVTKD,int nommes,int kolmes,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_kart_r(class zar_kart_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_kart_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать карточки начислений/удержаний по работникам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_kart_r_key_press),&data);

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

repl.plus(gettext("Распечатать карточки начислений/удержаний по работникам"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_kart_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_kart_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_kart_r_v_knopka(GtkWidget *widget,class zar_kart_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_kart_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_kart_r_data *data)
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

gint zar_kart_r1(class zar_kart_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);



int	        kon=0,kou=0;
SQL_str		row,row1;
int		kolstr=0;
short		prn=0;
int		knah=0;
short		d,m,g;
double		suma;
int		nommes=0;
int		kolmes=0;
int		tabn=0,tabnz=0;
short		mesqc=0;
int		nomq=0;
FILE		*ff,*ffo=NULL,*ffb=NULL;
char		imaf[40],imafo[40],imafb[40];
struct kk kart;
int		kolns=0,kolus=0;
short mn,gn;
short mk,gk;
iceb_u_rsdat1(&mn,&gn,data->rk->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->rk->datak.ravno());
if(mk == 0)
 {
  mk=mn;
  gk=gn;
 }
 
sprintf(strsql,"%d.%d => %d.%d\n",mn,gn,mk,gk);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

/*Определяем количество начислений и удержаний*/
sprintf(strsql,"select kod from Nash order by kod asc");
SQLCURSOR cur;
SQLCURSOR cur1;
if((kon=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  return(FALSE);
 }
if(kon == 0)
 {
  iceb_menu_soob(gettext("Не введены начисления !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  return(FALSE);
 }

class iceb_u_int MKN; //Массив кодов начислений
while(cur.read_cursor(&row) != 0)
  MKN.plus(atoi(row[0]),-1);

sprintf(strsql,"select kod from Uder order by kod asc");

if((kou=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  return(FALSE);
 }

if(kou == 0)
 {
  iceb_menu_soob(gettext("Не введены удержания !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  return(FALSE);
 }

class iceb_u_int MKU(0); //Массив кодов удержаний

while(cur.read_cursor(&row) != 0)
  MKU.plus(atoi(row[0]),-1);

class iceb_u_double MKND; //Данные начислений
class iceb_u_double MKUD; //Данные удержаний

kolmes=iceb_u_period(1,mn,gn,30,mk,gk,1);

MKND.make_class(kon*kolmes);
MKUD.make_class(kou*kolmes);
sprintf(strsql,"select kod from Tabel order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не введены виды табеля !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_int MVT(0); //Массив кодов удержаний

while(cur.read_cursor(&row) != 0)
  MVT.plus(atoi(row[0]),-1);

class iceb_u_double MVTRD; //Массив рабочих дней
class iceb_u_double MVTHS; //Массив часов
class iceb_u_double MVTKD; //Массив календарных дней

MVTRD.make_class(kolstr*kolmes);
MVTHS.make_class(kolstr*kolmes);
MVTKD.make_class(kolstr*kolmes);

class iceb_u_spisok MKNS; //массив начислений/счет
class iceb_u_spisok MKUS; //массив удержаний/счет
class iceb_u_double MKNSD; //данные начислений/счет
class iceb_u_double MKUSD; //данные удержания/счет

memset(imafo,'\0',sizeof(imafo));
memset(imafb,'\0',sizeof(imafb));

if(shetb != NULL &&  shetbu != NULL)
 {

  sprintf(strsql,"select distinct prn,knah,shet from Zarp where \
datz >= '%04d-%d-1' and datz <= '%04d-%d-31' and suma <> 0.",gn,mn,gk,mk);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(FALSE);
   }
  if(kolstr == 0)
   {
    iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
    return(FALSE);
   }

  while(cur.read_cursor(&row) != 0)
   {
    prn=atoi(row[0]);
    sprintf(strsql,"%s|%s",row[1],row[2]);
    if(prn == 1)
     { 
      MKNS.plus(strsql);
      kolns++;
     }
    if(prn == 2)
     {
      MKUS.plus(strsql);
      kolus++;
     }
   }
  MKNSD.make_class(kolns*kolmes);
  MKUSD.make_class(kolus*kolmes);

  sprintf(imafo,"krb%d-%d_%d.lst",mn,mk,getpid());

  if((ffo = fopen(imafo,"w")) == NULL)
   {
    iceb_er_op_fil(imafo,"",errno,data->window);
    return(FALSE);
   }

  sprintf(imafb,"krn%d-%d_%d.lst",mn,mk,getpid());

  if((ffb = fopen(imafb,"w")) == NULL)
   {
    iceb_er_op_fil(imafb,"",errno,data->window);
    return(FALSE);
   }
  
 }

FILE *fftmp;
char imaftmp[32];
sprintf(imaftmp,"krt%d.tmp",getpid());

if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  return(FALSE);
 }

sprintf(strsql,"select datz,tabn,prn,knah,suma,shet from Zarp where \
datz >= '%d-%d-1' and datz <= '%d-%d-31' and suma <> 0. \
order by tabn,datz asc",gn,mn,gk,mk);

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
char fio[512];
float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  memset(fio,'\0',sizeof(fio));
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    strncpy(fio,row1[0],sizeof(fio));

  fprintf(fftmp,"%s|%s|%s|%s|%s|%s|%s|\n",
  fio,row[0],row[1],row[2],row[3],row[4],row[5]);
 }

fclose(fftmp);

sprintf(strsql,"sort -o %s -t\\| -k1,2 -k2,3n %s",imaftmp,imaftmp);
system(strsql);

if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
unlink(imaftmp);


sprintf(imaf,"krt%d-%d_%d.lst",mn,mk,getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

double	saldon[kolmes];
double	saldono1[kolmes];
double	saldonb1[kolmes];
double	saldono2[kolmes];
double	saldonb2[kolmes];
memset(&saldon,'\0',sizeof(saldon));
memset(&saldono1,'\0',sizeof(saldono1));
memset(&saldonb1,'\0',sizeof(saldonb1));
memset(&saldono2,'\0',sizeof(saldono2));
memset(&saldonb2,'\0',sizeof(saldonb2));

double saldb=0;
kolstr1=0;
char stroka[1024];
char tabnom1[32];
char shet[32];
SQLCURSOR curr;
while(fgets(stroka,sizeof(stroka),fftmp) != NULL)
 {
  iceb_u_polen(stroka,tabnom1,sizeof(tabnom1),3,'|');
  if(iceb_u_proverka(data->rk->tabnom.ravno(),tabnom1,0,0) != 0)
    continue;
  tabn=atol(tabnom1);

  iceb_u_polen(stroka,strsql,sizeof(strsql),2,'|');
  iceb_u_rsdat(&d,&m,&g,strsql,2);    

  if(tabnz != tabn)
   {
    if(tabnz != 0)
     {
      zarkartp(tabnz,kart,mn,gn,kolmes,&MKN,&MKU,&MKND,&MKUD,saldon,
      &MKNS,&MKNSD,saldono1,saldonb1,&MKUS,&MKUSD,saldono2,saldonb2,
      ff,ffb,ffo,data->view,data->buffer);

      rasotrvr(mn,gn,kolmes,&MVT,&MVTRD,1,ff);
      rasotrvr(mn,gn,kolmes,&MVT,&MVTHS,2,ff);
      rasotrvr(mn,gn,kolmes,&MVT,&MVTKD,3,ff);

      fprintf(ff,"\f");

      if(ffo != NULL)
       fprintf(ffo,"\f");
      if(ffb != NULL)
       fprintf(ffb,"\f");

      MKND.clear_class();
      MKUD.clear_class();
      MVTRD.clear_class();
      MVTHS.clear_class();
      MVTKD.clear_class();

      memset(&saldon,'\0',sizeof(saldon));
      memset(&saldono1,'\0',sizeof(saldono1));
      memset(&saldonb1,'\0',sizeof(saldonb1));
      memset(&saldono2,'\0',sizeof(saldono2));
      memset(&saldonb2,'\0',sizeof(saldonb2));
     }
    memset(&kart,'\0',sizeof(kart));
    sprintf(strsql,"select * from Kartb where tabn=%s",tabnom1);
    if(sql_readkey(&bd,strsql,&row1,&curr) < 1)
     {
      sprintf(strsql,"Не найден табельный номер %s",tabnom1);
      iceb_menu_soob(strsql,data->window);
     }  
    else
     {    
      if(iceb_u_proverka(data->rk->podr.ravno(),row1[4],0,0) != 0)
       {
        tabnz=0;
        continue;
       }
      if(iceb_u_proverka(data->rk->kod_kat.ravno(),row1[5],0,0) != 0)
       {
        tabnz=0;
        continue;
       }    
      strncpy(kart.fio,row1[1],sizeof(kart.fio)-1);
      strncpy(kart.dolg,row1[2],sizeof(kart.dolg)-1);
      kart.podr=atoi(row1[4]);
      iceb_u_rsdat(&kart.dn,&kart.mn,&kart.gn,row1[6],2);
      iceb_u_rsdat(&kart.dk,&kart.mk,&kart.gk,row1[7],2);
      strncpy(kart.inn,row1[10],sizeof(kart.inn)-1);
      kart.sovm=atoi(row1[9]);    
     }    
    tabnz=tabn;
   }

  if(mesqc != m)
   {
    nommes=iceb_u_period(1,mn,gn,30,m,g,1);
    saldon[nommes-1]=zarsaldw(1,m,g,tabn,&saldb,data->window);
    saldono1[nommes-1]=saldon[nommes-1];
    saldonb1[nommes-1]=saldb;
    saldono2[nommes-1]=saldon[nommes-1];
    saldonb2[nommes-1]=saldb;
    vidtab(m,g,tabn,&MVT,&MVTRD,&MVTHS,&MVTKD,nommes,kolmes,data->window);
    mesqc=m;
   }
  iceb_u_polen(stroka,strsql,sizeof(strsql),4,'|');
  prn=atoi(strsql);
  iceb_u_polen(stroka,strsql,sizeof(strsql),5,'|');
  knah=atoi(strsql);
  iceb_u_polen(stroka,strsql,sizeof(strsql),6,'|');
  suma=atof(strsql);
  iceb_u_polen(stroka,shet,sizeof(shet),7,'|');

  if(prn == 1)
   {
    nomq=MKN.find(knah);
    MKND.plus(suma,kolmes*nomq+nommes-1);
    if(shetb != NULL &&  shetbu != NULL)
     {
      sprintf(strsql,"%d|%s",knah,shet);
      nomq=MKNS.find(strsql);
      MKNSD.plus(suma,kolmes*nomq+nommes-1);
     }
   }
  if(prn == 2)
   {
    nomq=MKU.find(knah);
    MKUD.plus(suma,kolmes*nomq+nommes-1);
    if(shetb != NULL &&  shetbu != NULL)
     {
      sprintf(strsql,"%d|%s",knah,shet);
      nomq=MKUS.find(strsql);
      MKUSD.plus(suma,kolmes*nomq+nommes-1);
     }
   }
 }

fclose(fftmp);

if(tabnz != 0)
 {
  zarkartp(tabnz,kart,mn,gn,kolmes,&MKN,&MKU,&MKND,&MKUD,saldon,
  &MKNS,&MKNSD,saldono1,saldonb1,&MKUS,&MKUSD,saldono2,saldonb2,
  ff,ffb,ffo,data->view,data->buffer);
  rasotrvr(mn,gn,kolmes,&MVT,&MVTRD,1,ff);
  rasotrvr(mn,gn,kolmes,&MVT,&MVTHS,2,ff);
  rasotrvr(mn,gn,kolmes,&MVT,&MVTKD,3,ff);
 }
iceb_podpis(ff,data->window);


fclose(ff);
if(shetb != NULL &&  shetbu != NULL)
 {
  iceb_podpis(ffo,data->window);

  iceb_podpis(ffb,data->window);

  fclose(ffo);
  fclose(ffb);
 }

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Распечатка карточек с начислениями и удержаниями"));
if(shetb != NULL &&  shetbu != NULL)
 {
  data->rk->imaf.plus(imafo);
  data->rk->imaf.plus(imafb);
  data->rk->naimf.plus(gettext("Распечатка карточек с начислениями и удержаниями (хозрасчёт)"));
  data->rk->naimf.plus(gettext("Распечатка карточек с начислениями и удержаниями (бюджет)"));
 }
  
for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
/********************************/
/*Распечатка конкретного массива*/
/********************************/
void   zarkartpr(short mn,short gn,
int kolmes,
class iceb_u_int *MKN,
class iceb_u_double *MKND,
double *saldo,
int metka, //1-начисления 2-удержания
FILE *ff)
{
char		stroka[40+(kolmes+1)*9];
short		d,m,g;
int		kon=0;
int		i,ii;
int		nome=0;
char		strsql[512];
SQL_str		row;
char		naim[512];
double		suma;
double		sumai[kolmes];
double		sumagor=0.,sumagori=0.;
SQLCURSOR curr;
memset(stroka,'\0',sizeof(stroka));

strcpy(stroka,"-----------------------------------");

for(i=0 ; i < kolmes+1; i++)
  strcat(stroka,"---------");


fprintf(ff,"%s\n",stroka);
if(metka == 1)
  fprintf(ff,gettext("Код|   Наименование начисления    |"));
if(metka == 2)
  fprintf(ff,gettext("Код|   Наименование удержания     |"));
d=1; m=mn; g=gn;

for(i=0 ; i < kolmes; i++)
 {
  fprintf(ff," %.02d.%d|",m,g);
  iceb_u_dpm(&d,&m,&g,3);
 }
fprintf(ff,"%*s|\n",iceb_u_kolbait(8,gettext("Итого")),gettext("Итого"));
memset(&sumai,'\0',sizeof(sumai));

fprintf(ff,"%s\n",stroka);
if(metka == 1)
 {
  fprintf(ff,"%*.*s|",iceb_u_kolbait(34,gettext("Сальдо начальное")),iceb_u_kolbait(34,gettext("Сальдо начальное")),gettext("Сальдо начальное"));
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
   { 
    fprintf(ff,"%8.2f|",saldo[ii]);
    suma+=saldo[ii];
   }
  fprintf(ff,"%8.2f|\n",suma);
  fprintf(ff,"%s\n",stroka);
 }

kon=MKN->kolih();
for(i=0; i < kon; i++)
 {
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
    if((suma=MKND->ravno(kolmes*i+ii)) != 0.)
     break;
  if(suma == 0.)
    continue;

  memset(naim,'\0',sizeof(naim));
  nome=MKN->ravno(i);
  if(metka == 1)
    sprintf(strsql,"select naik from Nash where kod=%d",nome);
  if(metka == 2)
    sprintf(strsql,"select naik from Uder where kod=%d",nome);
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   strcpy(naim,row[0]);      
  
  sumagor=0.;
  fprintf(ff,"%3d %-*.*s|",nome,iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim);
  for(ii=0 ; ii < kolmes; ii++)
   {
    suma=MKND->ravno(kolmes*i+ii);
    if(metka == 2)
     suma*=-1;
    sumai[ii]+=suma;
    sumagor+=suma;
    fprintf(ff,"%8.2f|",suma);
   }
  fprintf(ff,"%8.2f|\n",sumagor);
  sumagori+=sumagor;
 }
fprintf(ff,"%s\n",stroka);

if(metka == 1)
 fprintf(ff,"%*s|",iceb_u_kolbait(34,gettext("Итого начислено")),gettext("Итого начислено"));
if(metka == 2)
 fprintf(ff,"%*s|",iceb_u_kolbait(34,gettext("Итого удержано")),gettext("Итого удержано"));

for(ii=0 ; ii < kolmes; ii++)
 {
  fprintf(ff,"%8.2f|",sumai[ii]);
  if(metka == 1)
   saldo[ii]+=sumai[ii];
  if(metka == 2)
   saldo[ii]-=sumai[ii];
 }
fprintf(ff,"%8.2f|\n\n",sumagori);

if(metka == 2)
 {
  fprintf(ff,"%s\n",stroka);
  fprintf(ff,"%*.*s|",iceb_u_kolbait(34,gettext("Сальдо конечное")),iceb_u_kolbait(34,gettext("Сальдо конечное")),gettext("Сальдо конечное"));
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
   { 
    fprintf(ff,"%8.2f|",saldo[ii]);
    suma+=saldo[ii];
   }
  fprintf(ff,"%8.2f|\n",suma);
  fprintf(ff,"%s\n",stroka);
 }

}

/********************************/
/*Распечатка конкретного массива счет*/
/********************************/
void   zarkartprs(short mn,short gn,
int kolmes,
class iceb_u_spisok *MKN,
class iceb_u_double *MKND,
double *saldo,
double *saldob,
int metka, //1-начисления 2-удержания
int metkab, //1-бюджет 2-небюджет
FILE *ff)
{
char		stroka[40+(kolmes+1)*9];
short		d,m,g;
int		kon=0;
int		i,ii;
char		strsql[308];
SQL_str		row;
char		naim[512];
double		suma;
double		sumai[kolmes];
double		sumagor=0.,sumagori=0.;
char		bros[108];
char		knah[24];
char		shet[32];
SQLCURSOR curr;
  
memset(stroka,'\0',sizeof(stroka));

strcpy(stroka,"-----------------------------------");

for(i=0 ; i < kolmes+1; i++)
  strcat(stroka,"---------");


fprintf(ff,"%s\n",stroka);
if(metka == 1)
  fprintf(ff,gettext("Код|Наименование начисления |Счет |"));
if(metka == 2)
  fprintf(ff,gettext("Код|Наименование удержания  |Счет |"));
d=1; m=mn; g=gn;

for(i=0 ; i < kolmes; i++)
 {
  fprintf(ff," %.02d.%d|",m,g);
  iceb_u_dpm(&d,&m,&g,3);
 }
fprintf(ff,"%*s|\n",iceb_u_kolbait(8,gettext("Итого")),gettext("Итого"));
memset(&sumai,'\0',sizeof(sumai));

fprintf(ff,"%s\n",stroka);
if(metka == 1)
 {
  fprintf(ff,"%*.*s|",iceb_u_kolbait(34,gettext("Сальдо начальное")),iceb_u_kolbait(34,gettext("Сальдо начальное")),gettext("Сальдо начальное"));
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
   { 
    if(metkab == 1)
     {
      fprintf(ff,"%8.2f|",saldob[ii]);
      suma+=saldob[ii];
     }
    if(metkab == 2)
     {
      fprintf(ff,"%8.2f|",saldo[ii]-saldob[ii]);
      suma+=saldo[ii]-saldob[ii];
     }
   }
  fprintf(ff,"%8.2f|\n",suma);
  fprintf(ff,"%s\n",stroka);
 }

kon=MKN->kolih();
for(i=0; i < kon; i++)
 {
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
    if((suma=MKND->ravno(kolmes*i+ii)) != 0.)
     break;
  if(suma == 0.)
    continue;

  strncpy(bros,MKN->ravno(i),sizeof(bros)-1);
  iceb_u_polen(bros,knah,sizeof(knah),1,'|');
  iceb_u_polen(bros,shet,sizeof(shet),2,'|');

  if(metka == 1) //начисления
   if(metkab == 1) //бюджет
    if(iceb_u_proverka(shetb,shet,0,0) != 0)
      continue;
  if(metka == 1) //начисления
   if(metkab == 2) //не бюджет
    if(iceb_u_proverka(shetb,shet,0,0) == 0)
      continue;

  if(metka == 2) //удержания
  if(metkab == 1) //бюджет
    if(iceb_u_proverka(shetbu,shet,0,0) != 0)
      continue;

  if(metka == 2) //удержания
  if(metkab == 2) //Не бюджет
    if(iceb_u_proverka(shetbu,shet,0,0) == 0)
      continue;

  memset(naim,'\0',sizeof(naim));
    
  if(metka == 1)
    sprintf(strsql,"select naik from Nash where kod=%s",knah);
  if(metka == 2)
    sprintf(strsql,"select naik from Uder where kod=%s",knah);
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   strcpy(naim,row[0]);      
  
  sumagor=0.;
  fprintf(ff,"%3s %-*.*s|%-*s|",knah,iceb_u_kolbait(24,naim),iceb_u_kolbait(24,naim),naim,iceb_u_kolbait(5,shet),shet);
  for(ii=0 ; ii < kolmes; ii++)
   {
    suma=MKND->ravno(kolmes*i+ii);
    if(metka == 2)
     suma*=-1;
    sumai[ii]+=suma;
    sumagor+=suma;
    fprintf(ff,"%8.2f|",suma);
   }
  fprintf(ff,"%8.2f|\n",sumagor);
  sumagori+=sumagor;
 }
fprintf(ff,"%s\n",stroka);

if(metka == 1)
 fprintf(ff,"%*s|",iceb_u_kolbait(34,gettext("Итого начислено")),gettext("Итого начислено"));
if(metka == 2)
 fprintf(ff,"%*s|",iceb_u_kolbait(34,gettext("Итого удержано")),gettext("Итого удержано"));

for(ii=0 ; ii < kolmes; ii++)
 {
  fprintf(ff,"%8.2f|",sumai[ii]);
  if(metka == 1)
   {
    saldo[ii]+=sumai[ii];
    if(metkab == 1)
      saldob[ii]+=sumai[ii];
   }
  if(metka == 2)
   {
    saldo[ii]-=sumai[ii];
    if(metkab == 1)
      saldob[ii]-=sumai[ii];
   }
 }
fprintf(ff,"%8.2f|\n\n",sumagori);

if(metka == 2)
 {
  fprintf(ff,"%s\n",stroka);
  fprintf(ff,"%*.*s|",iceb_u_kolbait(34,gettext("Сальдо конечное")),iceb_u_kolbait(34,gettext("Сальдо конечное")),gettext("Сальдо конечное"));
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
   { 
    if(metkab == 1)
     {
      fprintf(ff,"%8.2f|",saldob[ii]);
      suma+=saldo[ii];
     }
    if(metkab == 2)
     {
      fprintf(ff,"%8.2f|",saldo[ii]-saldob[ii]);
      suma+=saldo[ii]-saldob[ii];
     }
   }
  fprintf(ff,"%8.2f|\n",suma);
  fprintf(ff,"%s\n",stroka);
 }

}

/************************/
/*Распечатка содержимого*/
/************************/
void   zarkartp(int tabn,
struct kk kart,
short mn,short gn,
int kolmes,
class iceb_u_int *MKN,
class iceb_u_int *MKU,
class iceb_u_double *MKND,
class iceb_u_double *MKUD,
double *saldon,
class iceb_u_spisok *MKNS,
class iceb_u_double *MKNSD,
double *saldono1,
double *saldonb1,
class iceb_u_spisok *MKUS,
class iceb_u_double *MKUSD,
double *saldono2,
double *saldonb2,
FILE *ff,
FILE *ffb,
FILE *ffo,
GtkWidget *view,
GtkTextBuffer *buffer)
{

char strsql[308];
sprintf(strsql,"\n%5d %s\n",tabn,kart.fio);
iceb_printw(iceb_u_toutf(strsql),buffer,view);

fprintf(ff,"%s\n%s:%d %s\n",organ,gettext("Табельный номер"),
tabn,kart.fio);

fprintf(ff,"%s:%s\n",gettext("Индивидуальный налоговый номер"),
kart.inn);
fprintf(ff,"%s:%s\n",gettext("Должность"),kart.dolg);
if(kart.sovm == 1)
  fprintf(ff,"%s\n",gettext("Совместитель"));
if(kart.dn != 0)
  fprintf(ff,"%s:%02d.%02d.%d%s\n",gettext("Дата начала работы"),
  kart.dn,kart.mn,kart.gn,gettext("г."));

if(kart.dk != 0)
  fprintf(ff,"%s:%02d.%02d.%d%s\n",gettext("Дата увольнения"),
  kart.dk,kart.mk,kart.gk,gettext("г."));
  
zarkartpr(mn,gn,kolmes,MKN,MKND,saldon,1,ff);
zarkartpr(mn,gn,kolmes,MKU,MKUD,saldon,2,ff);

if(shetb != NULL &&  shetbu != NULL)
 {
  fprintf(ffb,"%s:%d %s\n",gettext("Табельный номер"),
  tabn,kart.fio);

  fprintf(ffb,"%s:%s\n",gettext("Индивидуальный налоговый номер"),
  kart.inn);
  fprintf(ffb,"%s:%s\n",gettext("Должность"),kart.dolg);
  if(kart.sovm == 1)
    fprintf(ffb,"%s\n",gettext("Совместитель"));

  if(kart.dn != 0)
    fprintf(ffb,"%s:%02d.%02d.%d%s\n",gettext("Дата начала работы"),
    kart.dn,kart.mn,kart.gn,gettext("г."));

  if(kart.dk != 0)
    fprintf(ffb,"%s:%02d.%02d.%d%s\n",gettext("Дата увольнения"),
    kart.dk,kart.mk,kart.gk,gettext("г."));
  fprintf(ffb,"%s.\n",gettext("Бюджет"));
  zarkartprs(mn,gn,kolmes,MKNS,MKNSD,saldono1,saldonb1,1,1,ffb);
  zarkartprs(mn,gn,kolmes,MKUS,MKUSD,saldono1,saldonb1,2,1,ffb);

  fprintf(ffo,"%s:%d %s\n",gettext("Табельный номер"),
  tabn,kart.fio);

  fprintf(ffo,"%s:%s\n",gettext("Индивидуальный налоговый номер"),
  kart.inn);
  fprintf(ffo,"%s:%s\n",gettext("Должность"),kart.dolg);
  if(kart.sovm == 1)
    fprintf(ffo,"%s\n",gettext("Совместитель"));

  if(kart.dn != 0)
    fprintf(ffo,"%s:%02d.%02d.%d%s\n",gettext("Дата начала работы"),
    kart.dn,kart.mn,kart.gn,gettext("г."));

  if(kart.dk != 0)
    fprintf(ffo,"%s:%02d.%02d.%d%s\n",gettext("Дата увольнения"),
    kart.dk,kart.mk,kart.gk,gettext("г."));
  fprintf(ffo,"%s.\n",gettext("Хозрасчёт"));
  zarkartprs(mn,gn,kolmes,MKNS,MKNSD,saldono2,saldonb2,1,2,ffo);
  zarkartprs(mn,gn,kolmes,MKUS,MKUSD,saldono2,saldonb2,2,2,ffo);

  MKNSD->clear_class();
  MKUSD->clear_class();
 }

}
/***********************************/
/*Определение отработанного времени*/
/***********************************/

void	vidtab(short mr,short gr,int tabnom,
class iceb_u_int *MVT,
class iceb_u_double *MVTRD,
class iceb_u_double *MVTHS,
class iceb_u_double *MVTKD,
int nommes,
int kolmes,
GtkWidget *wpredok)
{
char		strsql[308];
SQL_str		row;
int		kolstr;
int		kodt=0;
int		nomq=0;

sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where god=%d and \
mes=%d and tabn=%d",gr,mr,tabnom);
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 return;

while(cur.read_cursor(&row) != 0)
 {
  kodt=atoi(row[0]);
  nomq=MVT->find(kodt);
  MVTRD->plus(atof(row[1]),kolmes*nomq+nommes-1);
  MVTHS->plus(atof(row[2]),kolmes*nomq+nommes-1);
  MVTKD->plus(atof(row[3]),kolmes*nomq+nommes-1);
 }

}
/**********************************/
/*Распечатка отработанного времени*/
/**********************************/
void	rasotrvr(short mn,short gn,int kolmes,
class iceb_u_int *MVT,
class iceb_u_double *MVTRD,
int metka, //1-рабочие дни 2-часы 3-календарные дни
FILE *ff)
{
char		stroka[40+(kolmes+1)*8];
short		d,m,g;
double		sumai[kolmes];
double		sumagor=0.,sumagori=0.;
int		i=0,ii=0;
int		koltb;
double		suma=0.;
char		strsql[308];
SQL_str		row;
char		naim[512];
int		nome;
SQLCURSOR curr;

koltb=MVT->kolih();
for(i=0; i < koltb*kolmes; i++)
 if((suma=MVTRD->ravno(i)) != 0.)
     break;
if(suma == 0.)
  return;
suma=0.;  
memset(stroka,'\0',sizeof(stroka));
if(metka == 1)
 fprintf(ff,"\n%s:\n",gettext("Свод отработанных рабочих дней"));
if(metka == 2)
 fprintf(ff,"%s:\n",gettext("Свод отработанных часов"));
if(metka == 3)
 fprintf(ff,"%s:\n",gettext("Свод календарных дней"));

strcpy(stroka,"-----------------------------------");

for(i=0 ; i < kolmes+1; i++)
  strcat(stroka,"--------");


fprintf(ff,"%s\n",stroka);
fprintf(ff,gettext("Код|   Наименование табеля        |"));

d=1; m=mn; g=gn;

for(i=0 ; i < kolmes; i++)
 {
  fprintf(ff,"%.02d.%d|",m,g);
  iceb_u_dpm(&d,&m,&g,3);
 }
fprintf(ff,"%*s|\n",iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"));
fprintf(ff,"%s\n",stroka);
memset(&sumai,'\0',sizeof(sumai));


for(i=0; i < koltb; i++)
 {
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
    if((suma=MVTRD->ravno(kolmes*i+ii)) != 0.)
     break;
  if(suma == 0.)
    continue;

  memset(naim,'\0',sizeof(naim));
  nome=MVT->ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",nome);
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   strcpy(naim,row[0]);      
  
  sumagor=0.;
  fprintf(ff,"%3d %-*.*s|",nome,iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim);
  for(ii=0 ; ii < kolmes; ii++)
   {
    suma=MVTRD->ravno(kolmes*i+ii);
    sumai[ii]+=suma;
    sumagor+=suma;
    fprintf(ff,"%7.f|",suma);
   }
  fprintf(ff,"%7.f|\n",sumagor);
  sumagori+=sumagor;
 }
fprintf(ff,"%s\n",stroka);

fprintf(ff,"%*s|",iceb_u_kolbait(34,gettext("Итого")),gettext("Итого"));

for(ii=0 ; ii < kolmes; ii++)
 {
  fprintf(ff,"%7.f|",sumai[ii]);
 }
fprintf(ff,"%7.f|\n\n",sumagori);

}
