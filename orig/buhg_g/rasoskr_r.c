/*$Id: rasoskr_r.c,v 1.35 2011-02-21 07:35:56 sasa Exp $*/
/*14.03.2010	22.10.2004	Белых А.И.	rasoskr_r.c
Расчет отчета остатка по карточкам
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"
#include "rasoskr.h"

class rasoskr_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
    
  class rasoskr_data *rk;
  
  int voz; //0-все в порядке расчёт сделан 1-нет
  rasoskr_r_data()
   {
    voz=1;
   }
 };

gboolean   rasoskr_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasoskr_r_data *data);
gint rasoskr_r1(class rasoskr_r_data *data);
void  rasoskr_r_v_knopka(GtkWidget *widget,class rasoskr_r_data *data);

void nahalo_rasoskr(class rasoskr_r_data *data,short dos,short mos,short gos,int *kolstrlist,int mns,struct tm *bf,iceb_u_str *naiskl,FILE *ff1);
void 		sapkk1(short,short,short,FILE*,int*,int*,int);
void 		sapkk(short,short,short,int,FILE*,int);

extern SQL_baza bd;
extern char *name_system;
extern double	nds1;
extern short	vtara; /*Код группы возвратная тара*/
extern double	okrcn;
extern char     *organ; 
extern double	okrg1;  /*Округление 1*/
extern int      kol_strok_na_liste;
extern iceb_u_str shrift_ravnohir;

int rasoskr_r(class rasoskr_data *rek_ras,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class rasoskr_r_data data;
data.rk=rek_ras;



data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать остатки на карточках"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rasoskr_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Распечатать остатки на карточках"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(rasoskr_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)rasoskr_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rasoskr_r_v_knopka(GtkWidget *widget,class rasoskr_r_data *data)
{
//printf("rasoskr_r_v_knopka\n");
if(data->voz == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rasoskr_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasoskr_r_data *data)
{
// printf("rasoskr_r_key_press\n");
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

gint rasoskr_r1(class rasoskr_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;

int skl=0;
skl=data->rk->sklad.ravno_atoi();

SQLCURSOR cur,curtmp;
SQL_str   row,rowtmp;
SQL_str   row1;
SQLCURSOR cur1;

iceb_u_str matot;
matot.new_plus("");

iceb_u_str naiskl;
naiskl.new_plus("");

if(skl != 0)
 {
  sprintf(strsql,"select naik,fmol from Sklad where kod=%d",skl);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    repl.new_plus(gettext("Не найден код склада"));
    repl.plus(" ");
    repl.plus(skl);
    repl.plus(" !");
    iceb_menu_soob(&repl,data->window);    
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  naiskl.new_plus(row[0]);
  matot.new_plus(row[1]); 
 }


/*Определяем количество складов в строке*/
int mns=iceb_u_pole2(data->rk->sklad.ravno(),',');
if(data->rk->sklad.getdlinna() > 1 && mns == 0)
  mns=1; /*Только один склад в списке*/
else
  mns=0;

int kolstr;
sprintf(strsql,"select * from Kart");
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

class iceb_tmptab tabtmp;
const char *imatmptab={"rasoskrw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shu char(24) not null,\
kgrm int not null,\
skl int not null,\
nk int not null,\
km int not null,\
naim char(112) not null,\
cena double(15,6) not null,\
nds char(32) not null,\
ei char(24) not null,\
ost double(16,6) not null,\
suma double(16,6) not null,\
datki char(10) not null,\
innom char(56) not null)",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

/**************************
char imaf[56];
sprintf(imaf,"sk%d_%d.tmp",skl,getpid());


FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
*****************************/
time_t tmm;
time(&tmm);
struct tm *bf;
bf=localtime(&tmm);

if(skl != 0 && mns == 1)
 {
  sprintf(strsql,"%s\n\n%s N%s %s\n\
%s %s%s\n",organ,gettext("Перечень карточек по складу"),
  data->rk->sklad.ravno(),naiskl.ravno(),
  gettext("Остатки на"),
  data->rk->data_ost.ravno(),
  gettext("г."));
 }
if(skl == 0)
 {
  sprintf(strsql,"%s\n\n%s\n\
%s %s%s\n",organ,gettext("Перечень карточек по всем складам"),
  gettext("Остатки на"),
  data->rk->data_ost.ravno(),
  gettext("г."));
 }

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
class iceb_u_str invnom("");
float kolstr1=0.;
iceb_u_str naim;
int kgrm=0;
int nk=0;
char ei[32];
class ostatok ost;
short dos,mos,gos;
iceb_u_rsdat(&dos,&mos,&gos,data->rk->data_ost.ravno(),1);
char		mnds=' ';
char data_k_is[32];
double bb;
short d,m,g;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->invnom.ravno(),row[15],0,0) != 0)
    continue;

   if(iceb_u_proverka(data->rk->kodmat.ravno(),row[2],0,0) != 0)
      continue;

  if(provndsw(data->rk->nds.ravno(),row) != 0)
    continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[5],0,0) != 0)
    continue;

  if(data->rk->metka_innom == 1)
   if(row[15][0] == '\0')
     continue;

  naim.new_plus("");
  kgrm=0;
  /*Узнаем наименование материалла*/
  sprintf(strsql,"select * from Material where kodm=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    printf("Не нашли наименование материалу по коду %s\n",row[2]);
   }
  else
   {
    naim.new_plus(row1[2]);
    kgrm=atoi(row1[1]);
   }

  if(iceb_u_proverka(data->rk->naim.ravno(),row1[2],4,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->grupa.ravno(),row1[1],0,0) != 0)
    continue;

  iceb_u_rsdat(&d,&m,&g,row[18],2);
  memset(data_k_is,'\0',sizeof(data_k_is));
  if(data->rk->kon_dat_is.getdlinna() > 1 )
   {
    if(d == 0)
     continue;
    short dki,mki,gki;
    iceb_u_rsdat(&dki,&mki,&gki,data->rk->kon_dat_is.ravno(),1);
    if(iceb_u_sravmydat(d,m,g,dki,mki,gki) > 0)
     continue;    
   }
  if(d != 0)
   sprintf(data_k_is,"%02d.%02d.%04d",d,m,g);

  nk=atoi(row[1]);

  memset(ei,'\0',sizeof(ei));
  strncpy(ei,row[4],sizeof(ei)-1);
  ostkarw(1,1,gos,dos,mos,gos,atoi(row[0]),nk,&ost);

  if(ost.ostg[3] < -0.0000001)
   {
    repl.new_plus(gettext("Внимание"));
    repl.plus(" !!!");
    

    sprintf(strsql,"%s:%s %s:%d",gettext("Склад"),
    row[0],gettext("Карточка"),nk);
    repl.ps_plus(strsql);
    
    repl.ps_plus(gettext("Отрицательный остаток"));
    iceb_menu_soob(&repl,data->window);
   }

  if(data->rk->metka_ost == 1 || (data->rk->metka_ost == 0 && (ost.ostg[3] > 0.0000001 || ost.ostg[3] < -0.0000001 )))
   {
    if(row[3][0] == '0')
     mnds=' ';    
    if(row[3][0] == '1')
     mnds='+';    
/*********************
    fprintf(ff,"%-6s |%2d|%-2s|%-5d|%-6s|%-*.*s|%10s ",
    row[5],kgrm,row[0],nk,row[2],
    iceb_u_kolbait(37,naim.ravno()),iceb_u_kolbait(37,naim.ravno()),naim.ravno(),
    prcnw(atof(row[6])));
*****************/
    bb=ost.ostg[3]*atof(row[6]);
    bb=iceb_u_okrug(bb,okrg1);
/***************
    memset(strsql,'\0',sizeof(strsql));
    if(row[15][0] != '\0')
     {
      strcpy(strsql,row[15]);
      if(row[14][0] != '\0')
       {
        char bros[512];
        short d,m,g;
        iceb_u_rsdat(&d,&m,&g,row[14],2);
        sprintf(bros," %d.%d.%d",d,m,g);
        strcat(strsql,bros);
       }
     }
    fprintf(ff,"|%c%2.2g| %-*s|%10.10g |%10.2f|%s\n",
    mnds,
    atof(row[9]),
    iceb_u_kolbait(6,ei),ei,
    ost.ostg[3],bb,strsql);
    kolstr2++;
*************************/
    invnom.new_plus("");
    if(row[15][0] != '\0')
     {
      invnom.new_plus(row[15]);
      if(row[14][0] != '\0')
       {
        char bros[512];
        short d,m,g;
        iceb_u_rsdat(&d,&m,&g,row[14],2);
        sprintf(bros," %d.%d.%d",d,m,g);
        invnom.plus(" ",bros);
       }
     }

    sprintf(strsql,"insert into %s values ('%s',%d,%s,%d,%s,'%s',%s,'%c%2.2g','%s',%.10g,%.10g,'%s','%s')",
    imatmptab,
    row[5],kgrm,row[0],nk,row[2],
    naim.ravno_filtr(),
    row[6],
    mnds,
    atof(row[9]),
    ei,
    ost.ostg[3],
    bb,
    data_k_is,
    invnom.ravno_filtr());

    iceb_sql_zapis(strsql,1,0,data->window);    

   }

 }

//fclose(ff);


/*Сортировка файла*/
sprintf(strsql,"%s\n",gettext("Сортируем записи"));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

/*Сортировка по группе материала*/
if(data->rk->metka_r == 0)
  sprintf(strsql,"select * from %s order by kgrm asc,skl asc,naim asc",imatmptab);

/*Сортировка по счету учета материала*/
if(data->rk->metka_r == 1)
  sprintf(strsql,"select * from %s order by shu asc,skl asc,naim asc",imatmptab);
//Сортировка счет, группа
if(data->rk->metka_r == 2)
  sprintf(strsql,"select * from %s order by shu asc,kgrm asc,skl asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


#if 0
#############################################3333

/*Сортировка по группе материалла*/
if(data->rk->metka_r == 0)
  sprintf(strsql,"sort -o %s -t\\| -k2,3n -k3,4n -k6,7b %s",imaf,imaf);
//  sprintf(strsql,"sort -o %s -t\\| +1n -2 +2n -3 +5b -6 %s",imaf,imaf);
/*Сортировка по счету учета материалла*/
if(data->rk->metka_r == 1)
  sprintf(strsql,"sort -o %s -t\\| -k1,2 -k3,4n -k6,7b %s",imaf,imaf);
//  sprintf(strsql,"sort -o %s -t\\| +0 -1 +2n -3 +5b -6 %s",imaf,imaf);

if(data->rk->metka_r == 2) //Сортировка счет, группа
  sprintf(strsql,"sort -o %s -t\\| -k1,2 -k2,3n -k3,4n -k6,7b %s",imaf,imaf);
//  sprintf(strsql,"sort -o %s -t\\| +0 -1 +1n -1 +2n -3 +5b -6 %s",imaf,imaf);

system(strsql);

sprintf(strsql,"%s\n",gettext("Распечатываем"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if((ff = fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
###########################################
#endif
char imaf1[56];
FILE *ff1;
sprintf(imaf1,"sk%d_%d.lst",skl,getpid());

data->rk->imaf.new_plus(imaf1);
data->rk->naimf.new_plus(gettext("Распечатка остатков на карточках"));

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafpust[56];
FILE *ffp;
sprintf(imafpust,"skp%d_%d.lst",skl,getpid());

data->rk->imaf.plus(imafpust);
data->rk->naimf.plus(gettext("Распечатка остатков на карточках (пустографка)"));

if((ffp = fopen(imafpust,"w")) == NULL)
 {
  iceb_er_op_fil(imafpust,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafiv[56];
sprintf(imafiv,"inv%d_%d.lst",skl,getpid());
FILE *ff2;

data->rk->imaf.plus(imafiv);
data->rk->naimf.plus(gettext("Распечатка инвентаризационной ведомости"));

if((ff2 = fopen(imafiv,"w")) == NULL)
 {
  iceb_er_op_fil(imafiv,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



d=dos; 
m=mos; 
g=gos;

iceb_u_dpm(&d,&m,&g,1); //Увеличить на день
char mesqc[56];
memset(mesqc,'\0',sizeof(mesqc));
iceb_mesc(m,1,mesqc);

int kolstriv=0;
int kollistiv=0;
sapvinw(matot.ravno(),mesqc,g,gos,&kolstriv,&kollistiv,ff2);

int kolstrlist=0;
nahalo_rasoskr(data,dos,mos,gos,&kolstrlist,mns,bf,&naiskl,ff1);

int kolstrlist_p=0;
nahalo_rasoskr(data,dos,mos,gos,&kolstrlist_p,mns,bf,&naiskl,ffp);



char shet[32];
char shetz[32];
char grup[32];
char grupz[32];

shet[0]=shetz[0]=grup[0]=grupz[0]='\0';
double itogshk=0.,itogshs=0.;
double itk=0.,its=0.,itgk=0.,itgs=0.,itok=0.,itos=0.; 
int sklz=0;

kolstrlist+=4;
kolstrlist_p+=4;
int kollist=1;

sapkk(dos,mos,gos,kollist,ff1,0);
int kollist_p=1;
sapkk(dos,mos,gos,kollist_p,ffp,1);

int nomerstr=0;
char str[1024];
kolstr1=0.;
char bros[312];
int skk;
iceb_u_str naish;
iceb_u_str dopstr;
iceb_u_str innom;
iceb_u_str regnom;
int kodmat=0;
char nds_str[32];
double cena=0.,kolih=0.,summa=0.;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  iceb_pbar(data->bar,kolstrtmp,++kolstr1);    
/*******************
  iceb_u_pole(str,shet,1,'|');
  iceb_u_pole(str,grup,2,'|');
  iceb_u_pole(str,bros,3,'|');
  skk=atoi(bros);
  iceb_u_pole(str,bros,4,'|');
  nk=atoi(bros);
*************************/
  strncpy(shet,rowtmp[0],sizeof(shet)-1);
  strncpy(grup,rowtmp[1],sizeof(grup)-1);
  skk=atoi(rowtmp[2]);
  nk=atoi(rowtmp[3]);
  kodmat=atoi(rowtmp[4]);
  naim.new_plus(rowtmp[5]);
  cena=atof(rowtmp[6]);
  strncpy(nds_str,rowtmp[7],sizeof(nds_str)-1);
  strncpy(ei,rowtmp[8],sizeof(ei)-1);
  kolih=atof(rowtmp[9]);
  summa=atof(rowtmp[10]);
  strncpy(data_k_is,rowtmp[11],sizeof(data_k_is)-1);
  invnom.new_plus(rowtmp[12]);

  if((data->rk->metka_r == 1 || data->rk->metka_r == 2) && iceb_u_SRAV(shetz,shet,0) != 0)
   {
    
    if(shetz[0] != '\0')
     {
      if(data->rk->metka_r == 1)
       { 
        if(mns != 1)
         {
          sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
          fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n");
          sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
          fprintf(ff1,"%-*s %10.10g  %10.2f\n",
          iceb_u_kolbait(88,gettext("Итого по складу")),
          gettext("Итого по складу"),
          itok,itos);
         }
        itk+=itok;
        its+=itos;
        itok=itos=0.;
       }

      sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
      fprintf(ff1,"%-*s %10.10g  %10.2f\n",
      iceb_u_kolbait(88,gettext("Итого по счёту")),gettext("Итого по счёту"),
      itogshk,itogshs);

      sapkk1ivw(&kolstriv,&kollistiv,ff2);
      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n");

      sapkk1ivw(&kolstriv,&kollistiv,ff2);
      fprintf(ff2,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),
      itogshk,itogshs);

      itogshk=itogshs=0.;

     }
    /*Определяем наименование счета*/
    naish.new_plus("");
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      printf("Не найден счет %s в плане счетов !\n",shet);         
     }
    else
     naish.new_plus(row1[0]);

    sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
    fprintf(ff1,"\n");
    sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
    fprintf(ff1,"%s: %s %s\n",gettext("Счет"),shet,naish.ravno());

    sapkk1ivw(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"\n");
    sapkk1ivw(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"%s: %s %s\n",gettext("Счет"),shet,naish.ravno());

    strcpy(shetz,shet);    
    sklz=0;
   }
      
  if((data->rk->metka_r == 0 || data->rk->metka_r == 2) && iceb_u_SRAV(grupz,grup,0) != 0)
   {
    
    if(grupz[0] != '\0')
     {
      if(itok != 0. || itos != 0.)
       {
        sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
        fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n");
        sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
        fprintf(ff1,"%-*s %10.10g  %10.2f\n",
        iceb_u_kolbait(88,gettext("Итого по складу")),gettext("Итого по складу"),
        itok,itos);
        itk+=itok;
        its+=itos;
        itok=itos=0.;
       }

      sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
      fprintf(ff1,"%-*s %10.10g  %10.2f\n",
      iceb_u_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),
      itgk,itgs);

      sapkk1ivw(&kolstriv,&kollistiv,ff2);
      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1ivw(&kolstriv,&kollistiv,ff2);
      fprintf(ff2,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Итого по группе")),gettext("Итого по группе"),
      itgk,itgs);

      itgk=itgs=0.; 


     }
    /*Узнаем наименование группы*/
    iceb_u_str naimgr;
    naimgr.new_plus("");
    sprintf(strsql,"select naik from Grup where kod=%s",grup);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       printf("Нет группы %s\n",grup);
       naimgr.new_plus("Не найдена группа !!!");
     }
    else
     naimgr.new_plus(row1[0]);
     
    sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
    fprintf(ff1,"\n");
    sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
    fprintf(ff1,"%s: %s %s\n",gettext("Группа"),grup,naimgr.ravno());

    sapkk1ivw(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"\n");
    sapkk1ivw(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"%s: %s %s\n",gettext("Группа"),grup,naimgr.ravno());

    strcpy(grupz,grup);
    sklz=0;
   }


  if(sklz != skk)
   {
    if(sklz != 0 )
     {

      if(mns != 1)
       {
        sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
        fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n");
        sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
        fprintf(ff1,"%-*s %10.10g  %10.2f\n",
        iceb_u_kolbait(88,gettext("Итого по складу")),gettext("Итого по складу"),
        itok,itos);

       }      
      itk+=itok;
      its+=itos;
      itok=itos=0.;

      
     }
    iceb_u_str matot;
    matot.new_plus("");
    iceb_u_str naim_skl;
    naim_skl.new_plus("");
    /*Узнаем наименование Склада*/
    sprintf(strsql,"select naik,fmol from Sklad where kod=%d",
    skk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       printf("Нет склада %d\n",skk);
       naim_skl.new_plus("Не найден склад !!!");
     }
    else
     {
      naim_skl.new_plus(row1[0]);
      matot.new_plus(row1[1]);
     }

    sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
    fprintf(ff1,"\n");
    sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
    fprintf(ff1,"%s: %d %s\n",
    gettext("Склад"),
    skk,naim_skl.ravno());
    sklz=skk;    


   }

  itgk+=kolih;
  itgs+=summa;

  itogshk+=kolih;
  itogshs+=summa;

  itok+=kolih;
  itos+=summa;
   

  //Вывод в файл с пустографкой
  sapkk1(dos,mos,gos,ffp,&kolstrlist_p,&kollist_p,1);

  fprintf(ffp,"%-*s|",iceb_u_kolbait(7,shet),shet);
  fprintf(ffp,"%-2s|",grup);

  fprintf(ffp,"%-2d|",skk);

  fprintf(ffp,"%-5d|",nk);
  fprintf(ffp,"%-6d|",kodmat);
  fprintf(ffp,"%-*.*s|",iceb_u_kolbait(37,naim.ravno()),iceb_u_kolbait(37,naim.ravno()),naim.ravno());

  iceb_u_pole(str,bros,9,'|');
  fprintf(ffp," %-*s|",iceb_u_kolbait(6,ei),ei);

  iceb_u_pole(str,bros,10,'|');
  fprintf(ffp,"%10.10g |",kolih);

  /*Остаток по документам*/
  ostatok ost;
  ostdokw(1,1,gos,dos,mos,gos,skk,nk,&ost);
  fprintf(ffp,"%10.10g |",ost.ostg[3]);
  
  fprintf(ffp,"          |          |          |\n");

  sapkk1(dos,mos,gos,ffp,&kolstrlist_p,&kollist_p,1);
  fprintf(ffp,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

  //Вывод в файл основной
  //Читаем инвентарный номер
  dopstr.new_plus("");
  innom.new_plus("");
  regnom.new_plus("");
  
  short dv=0,mv=0,gv=0;
  char sklad[40];
  char nomkar[40];  
  iceb_u_polen(str,sklad,sizeof(sklad),3,'|');
  iceb_u_polen(str,nomkar,sizeof(nomkar),4,'|');

  sprintf(strsql,"select datv,innom,rnd from Kart where sklad=%s and nomk=%s",
  sklad,nomkar);
  if(sql_readkey(&bd,strsql,&row,&cur1) == 1)
   {
    iceb_u_rsdat(&dv,&mv,&gv,row[0],2);
    innom.new_plus(row[1]);
    regnom.new_plus(row[2]);
   }   

  if(innom.getdlinna() > 1)
   {
    if( regnom.getdlinna() <= 1)
     {
      sprintf(strsql,"%s:%s",gettext("Инвентарный номер"),innom.ravno());
      dopstr.new_plus(strsql);
      if(dv != 0)
       {
        memset(bros,'\0',sizeof(bros));
        sprintf(bros," %s:%d.%d.%d%s",
        gettext("Дата ввода в эксплуатацию"),
        dv,mv,gv,gettext("г."));
//        strcat(dopstr,bros);
        dopstr.plus(bros);
       }
     }        
    else
     {
      sprintf(strsql,"%s:%s",gettext("Регистрационный номер"),regnom.ravno());
      dopstr.new_plus(strsql);
      memset(bros,'\0',sizeof(bros));
      sprintf(bros," %s:%s",gettext("Серия"),innom.ravno());
      dopstr.plus(bros);
      if(dv != 0)
       {
        memset(bros,'\0',sizeof(bros));
        sprintf(bros," %s:%d.%d.%d%s",
        gettext("Дата регистрации"),
        dv,mv,gv,gettext("г."));
        dopstr.plus(bros);
       }
     }
   }    


  sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
//  fprintf(ff1,"%s",str);
  fprintf(ff1,"%-*s |%2d|%-2d|%-5d|%-6d|%-*.*s|%10s ",
  iceb_u_kolbait(6,shet),shet,
  atoi(grup),skk,nk,kodmat,
  iceb_u_kolbait(37,naim.ravno()),iceb_u_kolbait(37,naim.ravno()),naim.ravno(),
  prcnw(cena));

  fprintf(ff1,"|%-3s| %-*s|%10.10g |%10.2f|%10s|%s\n",
  nds_str,
  iceb_u_kolbait(6,ei),ei,
  kolih,summa,data_k_is,invnom.ravno());


  sapkk1ivw(&kolstriv,&kollistiv,ff2);
  
  fprintf(ff2,"%3d|%-*.*s|%-*s|%10.10g|%10.10g|%10.2f|%10s|%10s|%10s|%10s|\n",
  ++nomerstr,
  iceb_u_kolbait(40,naim.ravno()),iceb_u_kolbait(40,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(6,ei),ei,
  cena,kolih,summa," "," "," "," ");

  if(dopstr.getdlinna() > 1)
   {
    sapkk1ivw(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"%s\n",dopstr.ravno());
   }
  sapkk1ivw(&kolstriv,&kollistiv,ff2);
  fprintf(ff2,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");


 }

itk+=itok;
its+=itos;

if(mns == 1)
 {
    if(data->rk->metka_r == 0)
     {
      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",
      iceb_u_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),
      itgk,itgs);

      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",
      iceb_u_kolbait(88,gettext("Общий итог")),gettext("Общий итог"),
      itk,its);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Итого по группе")),gettext("Итого по группе"),
      itgk,itgs);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Общий итог")),gettext("Общий итог"),
      itk,its);
     }

    if(data->rk->metka_r == 1)
     {
      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",
      iceb_u_kolbait(88,gettext("Итого по счету")),gettext("Итого по счету"),
       itogshk,itogshs);

      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",
      iceb_u_kolbait(88,gettext("Общий итог")),gettext("Общий итог"),
      itk,its);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Итого по счету")),gettext("Итого по счету"),
       itogshk,itogshs);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Общий итог")),gettext("Общий итог"),itk,its);
     }

    if(data->rk->metka_r == 2)
     {

      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",
      iceb_u_kolbait(88,gettext("Итого по складу")),gettext("Итого по складу"),itok,itos);

      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_u_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);

      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_u_kolbait(88,gettext("Итого по счету")),gettext("Итого по счету"),itgk,itgs);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_u_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_u_kolbait(62,gettext("Итого по счету")),gettext("Итого по счету"),itgk,itgs);

     }
 }

  if(mns != 1)
   {
     fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_u_kolbait(88,gettext("Итого по складу")),gettext("Итого по складу"),itok,itos);

    if(data->rk->metka_r == 0 || data->rk->metka_r == 2)
     {
      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_u_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);
      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_u_kolbait(62,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);
     }
    if(data->rk->metka_r == 1 || data->rk->metka_r == 2)
     {
      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_u_kolbait(88,gettext("Итого по счету")),gettext("Итого по счету"),
       itogshk,itogshs);
      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_u_kolbait(62,gettext("Итого по счету")),gettext("Итого по счету"),
       itogshk,itogshs);
     }
    fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_u_kolbait(88,gettext("Общий итог")),gettext("Общий итог"),itk,its);
    fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_u_kolbait(62,gettext("Общий итог")),gettext("Общий итог"),itk,its);
   }

sprintf(strsql,"%*s %8.8g %8.2f\n",
iceb_u_kolbait(47,gettext("Общий итог")),gettext("Общий итог"),itk,its);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

koncinvw(mesqc,gos,ff2);
iceb_podpis(ff1,data->window);
iceb_podpis(ffp,data->window);
iceb_podpis(ff2,data->window);

fclose(ff1);
fclose(ffp);

fclose(ff2);


sprintf(strsql,"%s:%d/%d\n",gettext("Количество листов"),
kollist,kollistiv);


iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);
}

/***************/
/*Счетчик строк*/
/****************/
void		sapkk1(short dos,short mos,short gos,
FILE *ff1,int *kolstrlist,int *kollist,
int metka_sap) 
{

*kolstrlist+=1;

if(*kolstrlist <= kol_strok_na_liste)
 return;

*kollist+=1;
fprintf(ff1,"\f");

*kolstrlist=4; //Размер шапки
*kolstrlist+=1; //СТРОКА которая должна быть выведена

sapkk(dos,mos,gos,*kollist,ff1,metka_sap);


}


/*************/
/*Шапка*/
/*************/

void		sapkk(short dos,short mos,short gos, //дата остатка
int kollist,FILE *ff1,int metka_sap)
{

if(metka_sap == 1)
 { 
  fprintf(ff1,"%100s %s N%d\n\
----------------------------------------------------------------------------------------------------------------------------------\n",
  " ",gettext("Лист"),kollist);
  fprintf(ff1,gettext("  Счет |Гр|Ск| Н/К |  Код |            Наименование             |  Е.и. | Остаток   |Остаток док|          |          |          |\n"));

  fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
 }

if(metka_sap == 0)
 {
 
fprintf(ff1,"%s:%02d.%02d.%04d%s %60s %s N%d\n\
---------------------------------------------------------------------------------------------------------------------------\n",
 gettext("Остатки вычислены на"),
 dos,mos,gos,gettext("г."),
" ",gettext("Лист"),kollist);
fprintf(ff1,gettext(" Счет  |Гр|Ск| Н/К |  Код |            Наименование             |    Цена   |НДС|  Е.и. | Остаток   |  Сумма   |Дата к.исп|\n"));
fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n");
 }

}

/********************************************/
/*Начало распечаток*/
/************************/
void nahalo_rasoskr(class rasoskr_r_data *data,
short dos,short mos,short gos,
int *kolstrlist,
int mns,
struct tm *bf,
iceb_u_str *naiskl,
FILE *ff1)
{
if(data->rk->sklad.getdlinna() > 1)
 {
  if(data->rk->sklad.ravno_atoi() != 0 && mns == 1)
   {
    fprintf(ff1,"%s\n\n%s %s %s\n\
%s %d.%d.%d%s  %s - %d:%d\n",
    organ,
    gettext("Перечень карточек по складу"),
    data->rk->sklad.ravno(),naiskl->ravno(),
    gettext("По состоянию на"),
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
    gettext("г."),
    gettext("Время"),
    bf->tm_hour,bf->tm_min);
    *kolstrlist=4;
   }
  else
   {
    fprintf(ff1,"%s\n\n%s %s\n\
%s %d.%d.%d%s  %s - %d:%d\n",
    organ,
    gettext("Перечень карточек по складу"),
    data->rk->sklad.ravno(),
    gettext("По состоянию на"),
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
    gettext("г."),
    gettext("Время"),
    bf->tm_hour,bf->tm_min);
    *kolstrlist=4;
   }
 }

if(data->rk->sklad.getdlinna() <= 1)
 {
  fprintf(ff1,"%s\n\n%s\n\
%s %d.%d.%d%s  %s - %d:%d\n",
  organ,
  gettext("Перечень карточек по всем складам"),
  gettext("По состоянию на"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min);
  *kolstrlist=4;
 }

if(data->rk->grupa.getdlinna() > 1)
 {
  fprintf(ff1,"%s:%s\n",gettext("Група"),data->rk->grupa.ravno());
  *kolstrlist+=1;;
 }

if(data->rk->shet.getdlinna() > 1)
 {
  fprintf(ff1,"%s:%s\n",gettext("Счета учета"),data->rk->shet.ravno());
  *kolstrlist+=1;;
 }
if(data->rk->kodmat.getdlinna() > 1)
 {
  fprintf(ff1,"%s:%s\n",gettext("Код материалла"),data->rk->kodmat.ravno());
  *kolstrlist+=1;;
 }
if(data->rk->nds.getdlinna()  > 1 )
 {
  fprintf(ff1,"%s:%s\n",gettext("НДС"),data->rk->nds.ravno());
  *kolstrlist+=1;;
 }
 
if(data->rk->metka_ost == 0)
 {
  fprintf(ff1,"%s\n",gettext("Карточки с нулевыми остатками не показаны !"));
  *kolstrlist+=1;;
 }
if(data->rk->metka_r == 0)
 {
  fprintf(ff1,"%s\n",gettext("Сортировка по группам материалов"));
  *kolstrlist+=1;;
 }
if(data->rk->metka_r == 1)
 {
  fprintf(ff1,"%s\n",gettext("Сортировка по счетам учета материалов"));
  *kolstrlist+=1;;
 }
if(data->rk->metka_r == 2)
 {
  fprintf(ff1,"%s\n",gettext("Сортировка по счетам, группам"));
  *kolstrlist+=1;;
 }
if(data->rk->metka_innom == 1)
 {
  fprintf(ff1,"%s\n",gettext("Только карточки с инвентарными номерами !"));
  *kolstrlist+=1;;
 }

if(data->rk->naim.getdlinna() > 1)
 {
  fprintf(ff1,"%s:%s\n",gettext("Наименование материалла"),data->rk->naim.ravno());
  *kolstrlist+=1;;
 }
if(data->rk->invnom.getdlinna() > 1)
 {
  fprintf(ff1,"%s:%s\n",gettext("Инвентарный номер"),data->rk->invnom.ravno());
  *kolstrlist+=1;;
 }

}
