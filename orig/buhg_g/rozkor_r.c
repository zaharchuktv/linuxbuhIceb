/*$Id: rozkor_r.c,v 1.20 2011-02-21 07:35:57 sasa Exp $*/
/*20.10.2004	20.10.2004	Белых А.И.	rozkor_r.c
Расчет оформления возврата
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"
#include "rozkor.h"

class rozkor_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  
  class rozkor_data *rk;
  short metka_fil; //0-расчет выполнен 1-нет
  
  iceb_u_spisok imafil;
  iceb_u_spisok naimfil;
    
  rozkor_r_data()
   {
    metka_fil=1;
   }
  

 };

gboolean   rozkor_r_key_press(GtkWidget *widget,GdkEventKey *event,class rozkor_r_data *data);
gint rozkor_r1(class rozkor_r_data *data);
void  rozkor_r_v_knopka(GtkWidget *widget,class rozkor_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern iceb_u_str shrift_ravnohir;
 
void rozkor_r(class rozkor_data *rek_ras,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class rozkor_r_data data;
data.rk=rek_ras;



data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Оформить возврат"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rozkor_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Оформить возврат"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),450,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
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


gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(rozkor_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)rozkor_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

if(data.metka_fil == 0)
 iceb_rabfil(&data.imafil,&data.naimfil,"",0,wpredok);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rozkor_r_v_knopka(GtkWidget *widget,class rozkor_r_data *data)
{
 printf("rozkor_r_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rozkor_r_key_press(GtkWidget *widget,GdkEventKey *event,class rozkor_r_data *data)
{
 printf("rozkor_r_key_press\n");

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

gint rozkor_r1(class rozkor_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
char bros[500];
iceb_u_str repl;

SQLCURSOR cur;
SQL_str row;

/*Проверяем есть ли такая накладная*/
sprintf(strsql,"select datd,sklad,nomd,kontr from Dokummat where \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomnn='%s'",
data->rk->god_nal_nak.ravno_atoi(),
data->rk->god_nal_nak.ravno_atoi(),
data->rk->nomer_nal_nak.ravno());

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"Не найден документ с номером налоговой накладной %s !",
  data->rk->nomer_nal_nak.ravno());
  iceb_menu_soob(strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short di,mi,gi;

iceb_u_rsdat(&di,&mi,&gi,row[0],2);
if(iceb_u_sravmydat(data->rk->dd,data->rk->md,data->rk->gd,di,mi,gi) < 0)
 {
  sprintf(strsql,"Дата налоговой накладной %d.%d.%d больше даты возврата %d.%d.%d !",
  di,mi,gi,data->rk->dd,data->rk->md,data->rk->gd);

  iceb_menu_soob(strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);

 }
char nin[20];
char kontr[30];

strncpy(nin,row[2],sizeof(nin)-1);
strncpy(kontr,row[3],sizeof(kontr)-1);


//Узнаем с каким НДС
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%s and \
nomd='%s' and nomerz=11",data->rk->god_nal_nak.ravno_atoi(),row[1],row[2]);
int lnds1=0;
if(sql_readkey(&bd,strsql,&row,&cur) > 0)
   lnds1=atoi(row[0]);


/*Читаем условие продажи*/
char uslpr[100];
memset(uslpr,'\0',sizeof(uslpr));

sprintf(strsql,"select sodz from Dokummat2 where god=%d and \
nomd='%s' and sklad=%d and nomerz=7",data->rk->god_nal_nak.ravno_atoi(),nin,data->rk->skl);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  strncpy(uslpr,row[0],sizeof(uslpr)-1);


/*Читаем форму оплаты*/
char forop[100];
memset(forop,'\0',sizeof(forop));
sprintf(strsql,"select sodz from Dokummat2 where god=%d and \
nomd='%s' and sklad=%d and nomerz=8",data->rk->god_nal_nak.ravno_atoi(),nin,data->rk->skl);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  strncpy(forop,row[0],sizeof(forop)-1);
if(forop[0] != '\0')
 {
  sprintf(strsql,"select naik from Foroplat where kod='%s'",forop);
  memset(forop,'\0',sizeof(forop));

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    strncpy(forop,row[0],sizeof(forop)-1);
 }
 
/*Читаем реквизиты организации свои */
sprintf(strsql,"select naikon,adres,innn,nspnds,telef from Kontragent \
where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {

  repl.new_plus(gettext("Не найден код контрагента 00 !"));
  iceb_menu_soob(&repl,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char		nai[40],nai1[512];
char		inn[20],inn1[20];
char		adr[40],adr1[40];
char		telef[40],telef1[40];
char		nspdv[20],nspdv1[20];

strncpy(nai,row[0],sizeof(nai)-1);
strncpy(adr,row[1],sizeof(adr)-1);
strncpy(inn,row[2],sizeof(inn)-1);
strncpy(nspdv,row[3],sizeof(nspdv)-1);
strncpy(telef,row[4],sizeof(telef)-1);

sprintf(bros,"select naikon,adres,innn,nspnds,telef from Kontragent \
where kodkon='%s'",kontr);
if(sql_readkey(&bd,bros,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента !"),kontr);
  iceb_menu_soob(strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

strncpy(nai1,row[0],sizeof(nai1)-1);
strncpy(adr1,row[1],sizeof(adr1)-1);
strncpy(inn1,row[2],sizeof(inn1)-1);
strncpy(nspdv1,row[3],sizeof(nspdv1)-1);
strncpy(telef1,row[4],sizeof(telef1)-1);

int kolstr;
/*Читаем документ*/
sprintf(strsql,"select kodm,nomkar,kolih,ei from Dokummat1 where sklad=%d and nomd='%s' \
and datd='%d-%d-%d'",data->rk->skl,data->rk->nomdok_c.ravno(),data->rk->gd,data->rk->md,data->rk->dd);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  repl.new_plus(gettext("Не найдено ни одной записи !"));
  iceb_menu_soob(&repl,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_str imaf_dok("dokrozr.alx");
FILE *ff1;
if(iceb_alxout(imaf_dok.ravno(),data->window) != 0)
  {
   gtk_widget_destroy(data->window);
   return(FALSE);
  }
 
if((ff1 = fopen(imaf_dok.ravno(),"r")) == NULL)
 {
 if(errno == ENOENT)
  {
   sprintf(strsql,"%s %s !",gettext("Не найден файл"),imaf_dok.ravno());
   iceb_menu_soob(strsql,data->window);
   gtk_widget_destroy(data->window);
   return(FALSE);
  }
 else
  {
   iceb_er_op_fil(imaf_dok.ravno(),"",errno,data->window);
   gtk_widget_destroy(data->window);
   return(FALSE);
  }
 }

char imaf[512];
sprintf(imaf,"rozvoz%d.lst",getpid());
FILE *ff;
data->imafil.new_plus(imaf);
data->naimfil.new_plus("Розсчёт коригування кількісних і вартісних показників до податкавої накладної");

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_startfil(ff);

/*Начинаем формирование нового документа*/
fprintf(ff,"\x1b\x6C%c",10); /*Установка левого поля*/
fprintf(ff,"\x0F");  /*Ужатый режим*/
fprintf(ff,"\x1B\x33%c",28); /*Уменьшаем межстрочный интервал*/
fprintf(ff,"\x1B\x4D"); /*12-знаков*/
char str[1000];
memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
fprintf(ff,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
fprintf(ff,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
sprintf(bros,"%s / %s",data->rk->nomer_ras.ravno(),data->rk->nomer_nal_nak.ravno());
iceb_u_bstab(str,bros,121,145,1);
fprintf(ff,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
fprintf(ff,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
sprintf(bros,"%d.%d.%d",di,mi,gi);
iceb_u_bstab(str,bros,78,89,1);
iceb_u_bstab(str,data->rk->nomer_nal_nak.ravno(),94,106,1);
iceb_u_bstab(str,data->rk->data_dogovora.ravno(),124,136,1);
iceb_u_bstab(str,data->rk->nomer_dogovora.ravno(),140,155,1);
fprintf(ff,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
fprintf(ff,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
fprintf(ff,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
iceb_u_bstab(str,nai,36,76,1);
iceb_u_bstab(str,nai1,155,194,1);
fprintf(ff,"%s",str);

for(int i=0; i < 3 ; i++)
 {
  memset(str,'\0',sizeof(str));
  fgets(str,sizeof(str),ff1);
  fprintf(ff,"%s",str);
 }

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
iceb_u_bstab(str,inn,13,14,1);
iceb_u_bstab(str,&inn[1],17,18,1);
iceb_u_bstab(str,&inn[2],21,22,1);
iceb_u_bstab(str,&inn[3],25,26,1);
iceb_u_bstab(str,&inn[4],29,30,1);
iceb_u_bstab(str,&inn[5],33,34,1);
iceb_u_bstab(str,&inn[6],37,38,1);
iceb_u_bstab(str,&inn[7],41,42,1);
iceb_u_bstab(str,&inn[8],45,46,1);
iceb_u_bstab(str,&inn[9],49,50,1);
iceb_u_bstab(str,&inn[10],53,54,1);
iceb_u_bstab(str,&inn[11],57,58,1);

iceb_u_bstab(str,inn1,139,140,1);
iceb_u_bstab(str,&inn1[1],143,144,1);
iceb_u_bstab(str,&inn1[2],147,148,1);
iceb_u_bstab(str,&inn1[3],151,152,1);
iceb_u_bstab(str,&inn1[4],155,156,1);
iceb_u_bstab(str,&inn1[5],159,160,1);
iceb_u_bstab(str,&inn1[6],163,164,1);
iceb_u_bstab(str,&inn1[7],167,168,1);
iceb_u_bstab(str,&inn1[8],171,172,1);
iceb_u_bstab(str,&inn1[9],175,176,1);
iceb_u_bstab(str,&inn1[10],179,180,1);
iceb_u_bstab(str,&inn1[11],183,184,1);

fprintf(ff,"%s",str);

for(int i=0; i < 2 ; i++)
 {
  memset(str,'\0',sizeof(str));
  fgets(str,sizeof(str),ff1);
  fprintf(ff,"%s",str);
 }

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
iceb_u_bstab(str,adr,25,65,1);
iceb_u_bstab(str,adr1,120,160,1);
fprintf(ff,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
iceb_u_bstab(str,telef,17,57,1);
iceb_u_bstab(str,telef1,150,190,1);
fprintf(ff,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
fprintf(ff,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
iceb_u_bstab(str,nspdv,45,65,1);
iceb_u_bstab(str,nspdv1,138,158,1);
fprintf(ff,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
fprintf(ff,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
iceb_u_bstab(str,uslpr,20,60,1);
fprintf(ff,"%s",str);

for(int i=0; i < 2 ; i++)
 {
  memset(str,'\0',sizeof(str));
  fgets(str,sizeof(str),ff1);
  fprintf(ff,"%s",str);
 }

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
iceb_u_bstab(str,forop,30,70,1);
fprintf(ff,"%s",str);

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff1);
iceb_u_bstab(str,data->rk->kto_v_ras.ravno(),40,70,1);
fprintf(ff,"%s",str);

fprintf(ff,"\x1B\x33%c",20); /*Уменьшаем межстрочный интервал*/
for(int i=0; i < 19 ; i++)
 {
  memset(str,'\0',sizeof(str));
  fgets(str,sizeof(str),ff1);
  fprintf(ff,"%s",str);
 }
fprintf(ff,"\x1B\x33%c",28); /*Уменьшаем межстрочный интервал*/

sprintf(strsql,"Номер накладной %s Номер налоговой накладной %s\n",
nin,data->rk->nomer_nal_nak.ravno());

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(ff,"\x1B\x2D\x31"); /*Включить подчеркивание*/
SQLCURSOR cur1;
SQL_str row1;
double it1=0.,it2=0.;
float kolstr1=0.;
int kodm;
int nk;
double kolih;
char ei[32];
char naimat[512];

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  kodm=atoi(row[0]);
  nk=atoi(row[1]);
  kolih=atof(row[2]);
  memset(ei,'\0',sizeof(ei));
  strncpy(ei,row[3],sizeof(ei)-1);
      
  /*Узнаем наименование материалла*/
  memset(naimat,'\0',sizeof(naimat));
  sprintf(strsql,"select naimat from Material where kodm=%s",
  row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найдено код материалла"),row[0]);
    iceb_menu_soob(strsql,data->window);
   }
  else
    strncpy(naimat,row1[0],sizeof(naimat)-1);


  sprintf(strsql,"%4d %4d %-*.*s %.8g\n",
  kodm,nk,
  iceb_u_kolbait(30,naimat),iceb_u_kolbait(30,naimat),naimat,
  kolih);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  /*Проверяем есть ли материал в исходной накладной и если есть то 
  сколько по количеству*/
  sprintf(strsql,"select kolih,cena from Dokummat1 where \
sklad=%d and nomd='%s' and datd='%d-%d-%d' and kodm=%d and nomkar=%d",
data->rk->skl,nin,gi,mi,di,kodm,nk);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s\n",gettext("Нет такого материалла в исходной накладной !"));
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    continue;
   }


  double kolih1=atof(row1[0]);
  double cenaz=atof(row1[1]);

  if(kolih1-kolih < 0.)
   {
    sprintf(strsql,"%s (%.8g) !\n",gettext("В исходной накладной количество меньше чем в приходной"),kolih1);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    continue;
   } 


  double bb1=kolih*cenaz*(-1);
  bb1=iceb_u_okrug(bb1,0.01);
  it1+=bb1;
  
  double bb=kolih*cenaz*data->rk->pnds/100.*(-1);
  bb=iceb_u_okrug(bb,0.01);
  it2+=bb;     
  if(lnds1 == 0)
   fprintf(ff,"|%02d.%02d.%d|%-*.*s|%-*.*s|%-*s|%10.2f|%10.2f|%10s|\
%10s|%10.2f|%10s|%10s|%11.2f|%11.2f|%11s|%11s|\n",
   data->rk->dd,data->rk->md,data->rk->gd,
   iceb_u_kolbait(20,data->rk->prihina.ravno()),iceb_u_kolbait(20,data->rk->prihina.ravno()),data->rk->prihina.ravno(),
   iceb_u_kolbait(30,naimat),iceb_u_kolbait(30,naimat),naimat,
   iceb_u_kolbait(5,ei),ei,
   kolih*(-1),cenaz," "," ",
   bb1," "," ",bb,bb," "," ");

  if(lnds1 == 3)
   fprintf(ff,"|%02d.%02d.%d|%-*.*s|%-*.*s|%-*s|%10.2f|%10.2f|%10s|\
%10s|%10s|%10s|%10.2f|%11s|%11s|%11s|%11s|\n",
   data->rk->dd,data->rk->md,data->rk->gd,
   iceb_u_kolbait(20,data->rk->prihina.ravno()),iceb_u_kolbait(20,data->rk->prihina.ravno()),data->rk->prihina.ravno(),
   iceb_u_kolbait(30,naimat),iceb_u_kolbait(30,naimat),naimat,
   iceb_u_kolbait(5,ei),ei,
   kolih*(-1),cenaz," "," ",
   " "," ",bb1," "," "," "," ");


/*
|    1     |          2         |               3              |  4  |    5     |    6     |    7     |    8     |     9    |    10    |    11    |    12     |     13    |     14    |    15     |
*/     
   
 }
if(lnds1 == 0)
  fprintf(ff,"%*s: %10.2f %21s %11.2f %11.2f\n",
  iceb_u_kolbait(112,gettext("Итого")),gettext("Итого"),it1," ",it2,it2);

if(lnds1 == 3)
  fprintf(ff,"%*s: %10.2f %11s %11s\n",
  iceb_u_kolbait(134,gettext("Итого")),gettext("Итого"),it1," "," ");

fprintf(ff,"\x1B\x2D\x30"); /*Выключить подчеркивание*/

int shet=1; //счетчик строк
memset(str,'\0',sizeof(str));
while(fgets(str,sizeof(str),ff1) != NULL)
 { 
  if(shet == 3)
    iceb_u_bstab(str,data->rk->kto_v_ras.ravno(),160,190,1);
  if(shet == 8)
   {
    sprintf(bros,"%d.%d.%d",data->rk->dd,data->rk->md,data->rk->gd);
    iceb_u_bstab(str,bros,30,42,1);
    iceb_u_bstab(str,data->rk->nomer_ras.ravno(),48,60,1);
    sprintf(bros,"%d.%d.%d",di,mi,gi);
    iceb_u_bstab(str,bros,89,102,1);
    iceb_u_bstab(str,data->rk->nomer_nal_nak.ravno(),106,118,1);
   }
  fprintf(ff,"%s",str);
  shet++;
  memset(str,'\0',sizeof(str));
 }  
  
fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
fprintf(ff,"\x12"); /*Выключить ужатый режим*/
fprintf(ff,"\x1B\x50"); /*10-знаков*/
fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/
fclose(ff);
fclose(ff1);
unlink(imaf_dok.ravno());



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);
data->metka_fil=0;
return(FALSE);
}
