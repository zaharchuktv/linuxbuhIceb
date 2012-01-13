/*$Id: rspvk_r.c,v 1.20 2011-02-21 07:35:57 sasa Exp $*/
/*16.11.2009	28.03.2004	Белых А.И.	rspvk_r.c
Расчет сальдо по контрагентам
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "rspvk.h"

#define	   KOLST  77 /*Количество строк на листе*/

class rspvk_r_data
 {
  public:
  
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  iceb_u_spisok imaf;
  iceb_u_spisok naim;

  rspvk_rr *rek_r;
  short dn,mn,gn;
  short dk,mk,gk;
  short  godn;
  time_t vremn;

  iceb_u_spisok sheta_srs;
  iceb_u_spisok spkontr;
  iceb_u_double mdo;
  SQLCURSOR     cur;      

 };

gboolean   rspvk_r_key_press(GtkWidget *widget,GdkEventKey *event,class rspvk_r_data *data);
gint rspvk_r1(class rspvk_r_data *data);
void  rspvk_r_v_knopka(GtkWidget *widget,class rspvk_r_data *data);

void            sstrsl(int*,int*,FILE*,FILE*);
void            sstrsl1(int*,int*,FILE*);
void		sal1(int,FILE*,FILE*);
void		sal2(int,FILE*);

extern SQL_baza bd;
extern short startgodb;
extern char *organ;
extern char *name_system;
extern short vplsh; /*0-двух порядковый план счетов 1-многпорядковый*/
extern short	startgodb; /*Стартовый год*/
extern short    koolk; /*Корректор отступа от левого края*/
extern iceb_u_str shrift_ravnohir;

void rspvk_r(class rspvk_rr *data_rr)
{
rspvk_r_data data;

data.rek_r=data_rr;

char strsql[512];
iceb_u_str soob;



if(iceb_rsdatp(&data.dn,&data.mn,&data.gn,data.rek_r->datan.ravno(),
&data.dk,&data.mk,&data.gk,data.rek_r->datak.ravno(),NULL) != 0)
  return;
  
data.godn=startgodb;
if(startgodb == 0 || startgodb > data.gn)
 data.godn=data.gn;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчет"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rspvk_r_key_press),&data);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

sprintf(strsql,"%s %s",gettext("Расчет сальдо по всем контрагентам"),data.rek_r->shet.ravno());
soob.new_plus(strsql);

sprintf(strsql,"%s %d.%d.%d => %d.%d.%d",gettext("Расчет за период"),
data.dn,data.mn,data.gn,
data.dk,data.mk,data.gk);
soob.ps_plus(strsql);

GtkWidget *label=gtk_label_new(soob.ravno_toutf());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);


data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),400,300);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(rspvk_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)rspvk_r1,&data);

gtk_main();

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);





}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rspvk_r_v_knopka(GtkWidget *widget,class rspvk_r_data *data)
{
// printf("rspvk_r_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rspvk_r_key_press(GtkWidget *widget,GdkEventKey *event,class rspvk_r_data *data)
{
// printf("rspvk_r_key_press\n");

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

gint rspvk_r1(class rspvk_r_data *data)
{
char strsql[512];
iceb_u_str soob;
iceb_clock sss(data->window);
SQL_str row1;
class SQLCURSOR cur1;


time(&data->vremn);

sprintf(strsql,"select ns from Plansh where saldo=3 order by ns asc");

SQLCURSOR cur;
int kolstr;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("В плане счетов нет счетов с развёрнутым сальдо !"));
  iceb_menu_soob(&repl,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int pozz=0;
SQL_str row;
char sh[20];
while(cur.read_cursor(&row) != 0)
 {
  strncpy(sh,row[0],sizeof(sh)-1);
  if( pozz < kolstr-1)
   {
    cur.poz_cursor(pozz+1);
    cur.read_cursor(&row);

    /*Если нет субсчетов запоминаем счет*/
    if(iceb_u_SRAV(sh,row[0],1) != 0)
     {
      cur.poz_cursor(pozz);
      cur.read_cursor(&row);
     }
   }
  pozz++;
  
  data->sheta_srs.plus(row[0]);
  
 }
sprintf(strsql,"%s: %d\n",gettext("Количество счетов с развёрнутым сальдо"),
data->sheta_srs.kolih());
soob.new_plus(strsql);

iceb_printw(soob.ravno_toutf(),data->buffer,data->view);


  /*Определяем все организации и открываем массивы дебетов и
  кредитов организаций
  */

/*Смотрим по каким контрагентам введено сальдо*/
sprintf(strsql,"select kodkon from Saldo where kkk=1 and gs=%d",data->godn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[0],0,0) != 0)
    continue;
  if(data->rek_r->kodgr.getdlinna() > 1)
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[0]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Не найден код контрагента"),row[0]);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
    if(iceb_u_proverka(data->rek_r->kodgr.ravno(),row[1],0,0) != 0)
      continue;
   }
  data->spkontr.plus(row[0]);
 }

/*Смотрим по каким контрагентам были сделаны проводки*/
sprintf(strsql,"select distinct kodkon from Prov where datp >= '%04d-1-1' and \
datp <= '%04d-%02d-%02d' and kodkon <> ''",data->godn,data->gk,data->mk,data->dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[0],0,0) != 0)
    continue;
  if(data->rek_r->kodgr.getdlinna() > 1)
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[0]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Не найден код контрагента"),row[0]);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
    if(iceb_u_proverka(data->rek_r->kodgr.ravno(),row[1],0,0) != 0)
      continue;
   }
  if(data->spkontr.find_r(row[0]) < 0)
    data->spkontr.plus(row[0]);
 }

#if 0
################################################33
sprintf(strsql,"select kodkon,grup from Kontragent order by kodkon asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolstr == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не введено ни одного контрагента !"));
  iceb_menu_soob(&repl,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rek_r->kodgr.ravno(),row[1],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[0],0,0) != 0)
    continue;
    
  data->spkontr.plus(row[0]);
 }
##################################################3
#endif
sprintf(strsql,"%s: %d\n",gettext("Количество контрагентов"),
data->spkontr.kolih());
soob.new_plus(strsql);

iceb_printw(soob.ravno_toutf(),data->buffer,data->view);

if(data->spkontr.kolih() == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдено ни одного контрагента !"));
  iceb_menu_soob(&repl,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->mdo.make_class(data->spkontr.kolih()*data->sheta_srs.kolih()*4);

//  printw(gettext("Вычисляем стартовое сальдо по всем контрагентам.\n"));
sprintf(strsql,"select ns,kodkon,deb,kre from Saldo where kkk='%d' and gs=%d",
1,data->godn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int i,i1;
int kls=data->sheta_srs.kolih(); 
int rzm;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if((i=data->sheta_srs.find(row[0])) < 0)
   continue;
   
  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[1],0,0) != 0)
    continue;

  if(data->rek_r->kodgr.getdlinna() > 1)
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)    
     if(iceb_u_proverka(data->rek_r->kodgr.ravno(),row1[0],0,0) != 0)
       continue;
   }
   
  if((i1=data->spkontr.find_r(row[1])) >= 0)
   {
    rzm=(i1*kls*4)+(i*4);
    data->mdo.plus(atof(row[2]),rzm);
    data->mdo.plus(atof(row[3]),rzm+1);
   }
  else
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдено код контрагента в массиве кодов!"));
    repl.ps_plus(row[1]);
    iceb_menu_soob(&repl,data->window);
   }
 }

//  printw(gettext("Просматриваем проводки.\n"));

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov \
where val=0 and datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
order by datp,sh asc",data->godn,1,1,data->gk,data->mk,data->dk);

if((kolstr=data->cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдено ни одной записи !"));
  iceb_menu_soob(&repl,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 } 

short d,m,g;
kls=data->sheta_srs.kolih();
float kolstr1=0.;  
while(data->cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if((i=data->sheta_srs.find(row[1])) < 0)
   continue;

  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[3],0,0) != 0)
    continue;

  if(data->rek_r->kodgr.getdlinna() > 1)
   {
    SQLCURSOR cur1;
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[3]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)    
     if(iceb_u_proverka(data->rek_r->kodgr.ravno(),row1[0],0,0) != 0)
       continue;
   }


  if((i1=data->spkontr.find_r(row[3])) < 0)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдено код контрагента в массиве кодов!"));
    repl.ps_plus(row[3]);

    sprintf(strsql,"%s %s %s %s %s %s",row[0],row[1],row[2],row[3],row[4],
    row[5]);
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,data->window);

    continue;
   }   

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  
    rzm=(i1*kls*4)+(i*4);

   /*Период*/

   if(iceb_u_sravmydat(d,m,g,data->dn,data->mn,data->gn) >= 0)
   if(iceb_u_sravmydat(d,m,g,data->dk,data->mk,data->gk) <= 0)
    {
   /*     printw("Период\n");*/
     data->mdo.plus(atof(row[4]),rzm+2);
     data->mdo.plus(atof(row[5]),rzm+3);
    }

   /*До периода*/
   if(iceb_u_sravmydat(d,m,g,data->dn,data->mn,data->gn) < 0)
    {
     /*     printw("До периода\n");*/
     data->mdo.plus(atof(row[4]),rzm);
     data->mdo.plus(atof(row[5]),rzm+1);
    }

 }


FILE *ff,*ff1,*ffdeb,*ffkre;
char imaf[30];

sprintf(imaf,"sl%d.lst",getpid());
data->imaf.plus(imaf);
data->naim.plus(gettext("Cальдо по всем контрагентам"));
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(imaf,"slk%d.lst",getpid());
data->imaf.plus(imaf);
data->naim.plus(gettext("Cальдо по всем контрагентам конечное"));
if((ff1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(imaf,"sldeb%d.lst",getpid());
data->imaf.plus(imaf);
data->naim.plus(gettext("Контрагенты дебиторы"));
if((ffdeb = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(imaf,"slkre%d.lst",getpid());
data->imaf.plus(imaf);
data->naim.plus(gettext("Контрагенты кредиторы"));
if((ffkre = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

fprintf(ff,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

fprintf(ff1,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

fprintf(ffdeb,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

fprintf(ffkre,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

iceb_u_zagolov(gettext("Сальдо по всем контрагентам"),data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,organ,ff);
iceb_u_zagolov(gettext("Сальдо по всем контрагентам"),data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,organ,ff1);
iceb_u_zagolov(gettext("Контрагенты дебиторы"),data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,organ,ffdeb);
iceb_u_zagolov(gettext("Контрагенты кредиторы"),data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,organ,ffkre);

int kst,kstdeb,kstkre;
int kli,klideb,klikre;

kst=kstdeb=kstkre=5;
kli=klideb=klikre=1;
if(data->rek_r->shet.getdlinna() > 1)
 {
  kst++; kstdeb++; kstkre++;
  fprintf(ff,"%s:%s\n",gettext("Счет"),data->rek_r->shet.ravno());
  fprintf(ff1,"%s:%s\n",gettext("Счет"),data->rek_r->shet.ravno());
  fprintf(ffdeb,"%s:%s\n",gettext("Счет"),data->rek_r->shet.ravno());
  fprintf(ffkre,"%s:%s\n",gettext("Счет"),data->rek_r->shet.ravno());
 }
if(data->rek_r->kodgr.getdlinna() > 1)
 {
  kst++; kstdeb++; kstkre++;
  fprintf(ff,"%s:%s\n",gettext("Группа"),data->rek_r->kodgr.ravno());
  fprintf(ff1,"%s:%s\n",gettext("Группа"),data->rek_r->kodgr.ravno());
  fprintf(ffdeb,"%s:%s\n",gettext("Группа"),data->rek_r->kodgr.ravno());
  fprintf(ffkre,"%s:%s\n",gettext("Группа"),data->rek_r->kodgr.ravno());
 }


if(data->rek_r->kontr.getdlinna() > 1)
 {
  kst++; kstdeb++; kstkre++;
  fprintf(ff,"%s:%s\n",gettext("Контрагент"),data->rek_r->kontr.ravno());
  fprintf(ff1,"%s:%s\n",gettext("Контрагент"),data->rek_r->kontr.ravno());
  fprintf(ffdeb,"%s:%s\n",gettext("Контрагент"),data->rek_r->kontr.ravno());
  fprintf(ffkre,"%s:%s\n",gettext("Контрагент"),data->rek_r->kontr.ravno());
 }

sal1(kli,ff,ff1);
sal2(klideb,ffdeb);
sal2(klikre,ffkre);

kst+=5;
kstdeb+=5;
kstkre+=5; //Увеличиваем на шапку

double mas2[6];
double mas1[6];

for(int i=0; i< 6 ;i++)
  mas2[i]=0.;

int i2;
int  mro,mrodeb,mrokre;
double  itdeb,itkre,db,kr;
char kor[24];
kls=data->sheta_srs.kolih();
int koo=data->spkontr.kolih();

char bros[312];              
for(int i1=0;i1<koo;i1++)
 {

  for(i2=0; i2< 6 ;i2++)
    mas1[i2]=0.;
  mro=mrodeb=mrokre=0;

  itdeb=itkre=db=kr=0;
  memset(kor,'\0',sizeof(kor));
  strcpy(kor,data->spkontr.ravno(i1));

  for(int i=0; i < kls; i++)
   {

    strcpy(sh,data->sheta_srs.ravno(i));
    
    if(iceb_u_proverka(data->rek_r->shet.ravno(),sh,1,0) != 0)
     continue;
     
    rzm=(i1*kls*4)+(i*4);

    if(fabs(data->mdo.ravno(rzm)) < 0.01 && 
       fabs(data->mdo.ravno(rzm+1)) < 0.01 &&
       fabs(data->mdo.ravno(rzm+2)) < 0.01 &&
       fabs(data->mdo.ravno(rzm+3)) < 0.01)
        continue;
    if(fabs(data->mdo.ravno(rzm)-data->mdo.ravno(rzm+1)) < 0.01 && 
       fabs(data->mdo.ravno(rzm+2)) < 0.01 &&
       fabs(data->mdo.ravno(rzm+3)) < 0.01)
        continue;
    

     mas1[2]+=data->mdo.ravno(rzm+2);
     mas1[3]+=data->mdo.ravno(rzm+3);

    if(mro == 0)
     {
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
      kor);
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        iceb_u_str repl;
        repl.plus(gettext("Не найдено код контрагента в общем списке !"));
        repl.ps_plus(kor);
        iceb_menu_soob(&repl,data->window);
        continue;
       }

      strncpy(bros,row[0],sizeof(bros)-1);
      if(data->mdo.ravno(rzm) > data->mdo.ravno(rzm+1))
       {
        fprintf(ff,"%*s %-*.*s %-*s %11.2f %11s %11.2f %11.2f",
        iceb_u_kolbait(7,kor),kor,
        iceb_u_kolbait(25,bros),iceb_u_kolbait(25,bros),bros,
        iceb_u_kolbait(5,sh),sh,
        data->mdo.ravno(rzm)-data->mdo.ravno(rzm+1)," ",data->mdo.ravno(rzm+2),data->mdo.ravno(rzm+3));
        
        mas1[0]+=data->mdo.ravno(rzm)-data->mdo.ravno(rzm+1);
       }
      else
       {
        fprintf(ff,"%*s %-*.*s %-*s %11s %11.2f %11.2f %11.2f",
        iceb_u_kolbait(7,kor),kor,
        iceb_u_kolbait(25,bros),iceb_u_kolbait(25,bros),bros,
        iceb_u_kolbait(5,sh),sh,
        " ",data->mdo.ravno(rzm+1)-data->mdo.ravno(rzm),data->mdo.ravno(rzm+2),data->mdo.ravno(rzm+3));
        mas1[1]+=data->mdo.ravno(rzm+1)-data->mdo.ravno(rzm);
       } 
     }
    else
     {
      if(data->mdo.ravno(rzm) > data->mdo.ravno(rzm+1))
       {
        fprintf(ff,"%7s %-25.25s %-*s %11.2f %11s %11.2f %11.2f",
        " "," ",
        iceb_u_kolbait(5,sh),sh,
        data->mdo.ravno(rzm)-data->mdo.ravno(rzm+1)," ",data->mdo.ravno(rzm+2),data->mdo.ravno(rzm+3));
        mas1[0]+=data->mdo.ravno(rzm)-data->mdo.ravno(rzm+1);
       }
      else
       {
        fprintf(ff,"%7s %-25.25s %-*s %11s %11.2f %11.2f %11.2f",
        " "," ",
        iceb_u_kolbait(5,sh),sh,
        " ",data->mdo.ravno(rzm+1)-data->mdo.ravno(rzm),data->mdo.ravno(rzm+2),data->mdo.ravno(rzm+3));
        mas1[1]+=data->mdo.ravno(rzm+1)-data->mdo.ravno(rzm);
       }
     }    

    if(data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2) > data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3))
     {
      if(mro == 0)
       {
        fprintf(ff1,"%*s %-*.*s %-*s %11.2f\n",
        iceb_u_kolbait(7,kor),kor,
        iceb_u_kolbait(25,bros),iceb_u_kolbait(25,bros),bros,
        iceb_u_kolbait(5,sh),sh,
        (data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2))-(data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3)));
       }
      else
       {
        fprintf(ff1,"%7s %-25.25s %-*s %11.2f\n",
        " "," ",iceb_u_kolbait(5,sh),sh,
        (data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2))-(data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3)));

       }

      if(fabs((data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2))-(data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3))) > 0.009)
       {
        itdeb+=(data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2))-(data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3));
        if(mrodeb == 0)
         {
          fprintf(ffdeb,"%*s %-*.*s %-*s %11.2f\n",
          iceb_u_kolbait(7,kor),kor,
          iceb_u_kolbait(25,bros),iceb_u_kolbait(25,bros),bros,
          iceb_u_kolbait(5,sh),sh,
          (data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2))-(data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3)));
         }
        else
         {
          fprintf(ffdeb,"%7s %-25.25s %-*s %11.2f\n",
          " "," ",iceb_u_kolbait(5,sh),sh,
          (data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2))-(data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3)));
         }
        mrodeb++;
        sstrsl1(&kstdeb,&klideb,ffdeb);
       }        
      fprintf(ff," %11.2f\n",(data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2))-(data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3)));
      mas1[4]+=(data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2))-(data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3));
     }
    else
     {
      if(mro == 0)
       {
        fprintf(ff1,"%*s %-*.*s %-*s %11s %11.2f\n",
        iceb_u_kolbait(7,kor),kor,
        iceb_u_kolbait(25,bros),iceb_u_kolbait(25,bros),bros,
        iceb_u_kolbait(5,sh),sh,
        " ",
        (data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3))-(data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2)));
       }
      else
       {
        fprintf(ff1,"%7s %-25.25s %-*s %11s %11.2f\n",
        " "," ",iceb_u_kolbait(5,sh),sh," ",
        (data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3))-(data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2)));

       }

      if(fabs((data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3))-(data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2))) > 0.009)
       {
        itkre+=(data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3))-(data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2));

        if(mrokre == 0)
         {
          fprintf(ffkre,"%*s %-*.*s %-*s %11s %11.2f\n",
          iceb_u_kolbait(7,kor),kor,
          iceb_u_kolbait(25,bros),iceb_u_kolbait(25,bros),bros,
          iceb_u_kolbait(5,sh),sh,
          " ",
          (data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3))-(data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2)));
         }
        else
         {
          fprintf(ffkre,"%7s %-25.25s %-*s %11s %11.2f\n",
          " "," ",iceb_u_kolbait(5,sh),sh," ",
          (data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3))-(data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2)));

         }
        mrokre++;

        sstrsl1(&kstkre,&klikre,ffkre);
       }      
      fprintf(ff," %11s %11.2f\n"," ",(data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3))-(data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2)));
      mas1[5]+=(data->mdo.ravno(rzm+1)+data->mdo.ravno(rzm+3))-(data->mdo.ravno(rzm)+data->mdo.ravno(rzm+2));
     }
    sstrsl(&kst,&kli,ff,ff1);
    mro++;
   }

  if(mro > 1)
   {
    sprintf(bros,"------%s %s :",gettext("Итого по"),kor);
    if(mas1[0] > mas1[1])
     {
      fprintf(ff,"%*s %11.2f %11s %11.2f %11.2f",
      iceb_u_kolbait(39,bros),bros,mas1[0]-mas1[1]," ",mas1[2],mas1[3]);
     }
    else
     {
      fprintf(ff,"%*s %11s %11.2f %11.2f %11.2f",
      iceb_u_kolbait(39,bros),bros," ",mas1[1]-mas1[0],mas1[2],mas1[3]);
     }

    if(mas1[0]+mas1[2] > mas1[1]+mas1[3])
     {
      fprintf(ff1,"%*s %11.2f\n",
      iceb_u_kolbait(39,bros),bros,
      (mas1[0]+mas1[2])-(mas1[1]+mas1[3]));
      fprintf(ff," %11.2f\n",(mas1[0]+mas1[2])-(mas1[1]+mas1[3]));

     }
    else
     {
      fprintf(ff1,"%*s %11s %11.2f\n",
      iceb_u_kolbait(39,bros),bros," ",
      (mas1[1]+mas1[3])-(mas1[0]+mas1[2]));
      fprintf(ff," %11s %11.2f\n"," ",(mas1[1]+mas1[3])-(mas1[0]+mas1[2]));
     }
    sstrsl(&kst,&kli,ff,ff1);

   }

  if(mrodeb > 1)
   {
    sprintf(bros,"------%s %s :",gettext("Итого по"),kor);
    fprintf(ffdeb,"%*s %11.2f\n",
    iceb_u_kolbait(39,bros),bros,itdeb);
    sstrsl1(&kstdeb,&klideb,ffdeb);
   }
  if(mrokre > 1)
   {
    sprintf(bros,"------%s %s :",gettext("Итого по"),kor);
      fprintf(ffkre,"%*s %11s %11.2f\n",
      iceb_u_kolbait(39,bros),bros," ",itkre);
      sstrsl1(&kstkre,&klikre,ffkre);
   }
  mas2[0]+=mas1[0];
  mas2[1]+=mas1[1];
  mas2[2]+=mas1[2];
  mas2[3]+=mas1[3];
  mas2[4]+=mas1[4];
  mas2[5]+=mas1[5];

 }

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------\n");

fprintf(ff1,"\
----------------------------------------------------------------\n");
fprintf(ffdeb,"\
----------------------------------------------------------------\n");
fprintf(ffkre,"\
----------------------------------------------------------------\n");

sprintf(strsql,"\n%20s",iceb_u_prnbr(mas2[0]));
soob.new_plus(strsql);
sprintf(strsql," %20s\n",iceb_u_prnbr(mas2[1]));
soob.plus(strsql);
sprintf(strsql,"%20s",iceb_u_prnbr(mas2[2]));
soob.plus(strsql);
sprintf(strsql," %20s\n",iceb_u_prnbr(mas2[3]));
soob.plus(strsql);
sprintf(strsql,"%20s",iceb_u_prnbr(mas2[4]));
soob.plus(strsql);
sprintf(strsql," %20s\n",iceb_u_prnbr(mas2[5]));
soob.plus(strsql);
iceb_printw(soob.ravno_toutf(),data->buffer,data->view);

fprintf(ff,"%*s %11.2f %11.2f %11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(39,gettext("Итого")),gettext("Итого"),mas2[0],mas2[1],mas2[2],mas2[3],mas2[4],mas2[5]);

fprintf(ff1,"%*s %11.2f %11.2f\n",
iceb_u_kolbait(39,gettext("Итого")),gettext("Итого"),mas2[4],mas2[5]);

fprintf(ffdeb,"%*s %11.2f\n",
iceb_u_kolbait(39,gettext("Итого")),gettext("Итого"),mas2[4]);

fprintf(ffkre,"%*s %11s %11.2f\n",
iceb_u_kolbait(39,gettext("Итого")),gettext("Итого")," ",mas2[5]);

if(mas2[0] > mas2[1])
    fprintf(ff,"%*s %11.2f %11s %11.2f %11.2f",
    iceb_u_kolbait(39,gettext("Итого (свернуто)")),gettext("Итого (свернуто)"),mas2[0]-mas2[1]," ",mas2[2],mas2[3]);
  else
    fprintf(ff,"%*s %11s %11.2f %11.2f %11.2f",
    iceb_u_kolbait(39,gettext("Итого (свернуто)")),gettext("Итого (свернуто):")," ",mas2[1]-mas2[0],mas2[2],mas2[3]);
if(mas2[0]+mas2[2] > mas2[1]+mas2[3])
 {
  fprintf(ff1,"%*s %11.2f\n",
  iceb_u_kolbait(39,gettext("Итого (свернуто)")),gettext("Итого (свернуто)"),
  (mas2[0]+mas2[2])-(mas2[1]+mas2[3]));
  fprintf(ff," %11.2f\n",(mas2[0]+mas2[2])-(mas2[1]+mas2[3]));

 }
else
 {
  fprintf(ff1,"%*s %11s %11.2f\n",
  iceb_u_kolbait(39,gettext("Итого (свернуто)")),gettext("Итого (свернуто)")," ",
  (mas2[1]+mas2[3])-(mas2[0]+mas2[2]));
  fprintf(ff," %11s %11.2f\n"," ",(mas2[1]+mas2[3])-(mas2[0]+mas2[2]));
 }
iceb_podpis(ff,data->window);

iceb_podpis(ff1,data->window);

iceb_podpis(ffdeb,data->window);

iceb_podpis(ffkre,data->window);


fclose(ff);
fclose(ff1);
fclose(ffdeb);
fclose(ffkre);


for(int nom=0; nom < data->imaf.kolih(); nom++)
 iceb_ustpeh(data->imaf.ravno(nom),1,data->window);


iceb_printw_vr(data->vremn,data->buffer,data->view);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);
//printf("rspvk_r1 end\n");
return(FALSE);

}
/*************************/
/*Счетчик строк и листов*/
/*************************/
void            sstrsl(int *kst, //Количество строк
int *kli, //Количество листов
FILE *ff,FILE *ff1)
{

*kst+=1;
if(*kst >= KOLST)
 {
  fprintf(ff,"\f");
  fprintf(ff1,"\f");
  *kli=*kli+1;
  sal1(*kli,ff,ff1);
  *kst=5;
 }  

}

/**********/
/*Шапка 1*/
/*********/
void		sal1(int kli,FILE *ff,FILE *ff1)
{

fprintf(ff,"%90s%s N%d\n","",gettext("Лист"),kli);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext(" Код   | Наименование организации|Счет |  Сальдо начальное     |  Оборот за период     |    Сальдо конечное    |\n"));
fprintf(ff,gettext("       |                         |     |   Дебет   |  Кредит   |   Дебет   |  Кредит   |   Дебет   |  Кредит   |\n"));

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------\n");

fprintf(ff1,"%50s%s N%d\n","",gettext("Лист"),kli);
fprintf(ff1,"\
----------------------------------------------------------------\n");

fprintf(ff1,gettext("  Код  | Наименование организации|Счет |    Сальдо конечное    |\n"));
fprintf(ff1,gettext("       |                         |     |   Дебет   |  Кредит   |\n"));

fprintf(ff1,"\
----------------------------------------------------------------\n");
}
/*************************/
/*Счетчик строк и листов*/
/*************************/
void            sstrsl1(int *kst, //Количество строк
int *kli, //Количество листов
FILE *ff)
{

*kst+=1;
if(*kst >= KOLST)
 {
  fprintf(ff,"\f");
  *kli+=1;
  sal2(*kli,ff);
  *kst=5;
 }  

}

/**********/
/*Шапка 2*/
/*********/
void		sal2(int kli,FILE *ff1)
{

fprintf(ff1,"%50s%s N%d\n","",gettext("Лист"),kli);
fprintf(ff1,"\
----------------------------------------------------------------\n");

fprintf(ff1,gettext("  Код  | Наименование организации|Счет |    Сальдо конечное    |\n"));
fprintf(ff1,gettext("       |                         |     |   Дебет    | Кредит   |\n"));

fprintf(ff1,"\
----------------------------------------------------------------\n");
}
