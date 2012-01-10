/*$Id: ukrprik_r.c,v 1.8 2011-02-21 07:35:58 sasa Exp $*/
/*17.11.2009	28.02.2008	Белых А.И.	ukrprik_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "ukrprik.h"

class ukrprik_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class ukrprik_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  ukrprik_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   ukrprik_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrprik_r_data *data);
gint ukrprik_r1(class ukrprik_r_data *data);
void  ukrprik_r_v_knopka(GtkWidget *widget,class ukrprik_r_data *data);


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

int ukrprik_r(class ukrprik_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class ukrprik_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка приказов на командировку"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(ukrprik_r_key_press),&data);

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

repl.plus(gettext("Распечатка приказов на командировку"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(ukrprik_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)ukrprik_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ukrprik_r_v_knopka(GtkWidget *widget,class ukrprik_r_data *data)
{
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   ukrprik_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrprik_r_data *data)
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

gint ukrprik_r1(class ukrprik_r_data *data)
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


int	kolstr=0;
char    imaf[30];
FILE	*ff;
char	nompr[30];
short   d,m,g;
char    fio[512];
int     kolprik=0;
short   dnp,mnp,gnp;
short   dkp,mkp,gkp;
char	fioruk[60];
char    kodruk[50];
short   metkaruk=0;
char	tabnom[50];
char    prist[20];
short   dlinnapr=0;
char	dolgn[40];
short   dlinnakont=0;
int     i=0,i2=0;
char	dolgnruk[60];
char    tabnomruk[60];

memset(tabnomruk,'\0',sizeof(tabnomruk));
class iceb_u_str imaf_znast("zarnast.alx");

iceb_poldan("Табельный номер руководителя",tabnomruk,imaf_znast.ravno(),data->window);

memset(dolgnruk,'\0',sizeof(dolgnruk));
memset(fioruk,'\0',sizeof(fioruk));
  
//Узнаем должность
sprintf(strsql,"select fio,dolg from Kartb where tabn=%d",atoi(tabnomruk));
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  strncpy(fioruk,row[0],sizeof(fioruk)-1);
  strncpy(dolgnruk,row[1],sizeof(dolgnruk)-1);
 }

memset(nompr,'\0',sizeof(nompr));

sprintf(strsql,"select kont,pnaz,organ,celk,datn,datk,nompr,datp from Ukrdok \
where datp >= '%d-%d-%d' and datp <= '%d-%d-%d' order by nompr asc",
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

iceb_poldan("Приставка к табельному номеру",prist,imaf_znast.ravno(),data->window);
dlinnapr=strlen(prist);
sprintf(kodruk,"%s%s",prist,tabnomruk);

sprintf(imaf,"pkr%d.tmp",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int nomer=0;
float kolstr1=0.;
SQL_str row1;
class SQLCURSOR curr;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_SRAV(kodruk,row[0],0) == 0)
   {
    metkaruk++;
    continue;
   }

  if(iceb_u_SRAV(nompr,row[6],0) != 0 || (kolstr1 == 1 && row[6][0] == '\0'))
   {
    if(kolprik != 0)
     {
      fprintf(ff,"\n%10s %s___________________%s\n"," ",dolgnruk,fioruk);
      fprintf(ff,"\f");
     }

    fprintf(ff,"%s\n\n",organ);
    fprintf(ff,"%25s%s %s\n"," ",gettext("ПРИКАЗ НОМЕР"),row[6]);

    iceb_u_rsdat(&d,&m,&g,row[7],2);

    fprintf(ff,"%25s%s %02d.%02d.%d%s\n\n"," ",gettext("от"),d,m,g,
    gettext("г."));

    fprintf(ff,"%s\n",gettext("Про командировку"));
    fprintf(ff,"%s:\n",gettext("Откомандировать"));

    strcpy(nompr,row[6]);
    nomer=0;    
    kolprik++;    
   }  

  iceb_u_rsdat(&dnp,&mnp,&gnp,row[4],2);
  iceb_u_rsdat(&dkp,&mkp,&gkp,row[5],2);

  memset(dolgn,'\0',sizeof(dolgn));
    
  if(iceb_u_SRAV(prist,row[0],1) == 0)
   {
   
    memset(tabnom,'\0',sizeof(tabnom));
    dlinnakont=strlen(row[0]);
    i2=0;
    for(i=dlinnapr; i < dlinnakont; i++)
      tabnom[i2++]=row[0][i];     

    //Узнаем должность
    sprintf(strsql,"select dolg from Kartb where tabn=%s",tabnom);
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
      strncpy(dolgn,row1[0],sizeof(dolgn)-1);

   }
  
  memset(fio,'\0',sizeof(fio));
  //Читаем наименование контрагента
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    strncpy(fio,row1[0],sizeof(fio)-1);
    
  fprintf(ff,"\n%d. %s %s\n",++nomer,fio,dolgn);
  fprintf(ff,"%10s %s %s\n"," ",row[1],row[2]);
  fprintf(ff,"%10s %s %s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
  " ",row[3],
  gettext("с"),dnp,mnp,gnp,gettext("г."),
  gettext("по"),dkp,mkp,gkp,gettext("г."));
  fprintf(ff,"\n%10s%s________________________\n"," ",gettext("с приказом ознакомлен"));
 }

fprintf(ff,"\n%10s %s___________________%s\n"," ",dolgnruk,fioruk);

if(metkaruk != 0)
 {
  short kolprik1=0;
  
  sprintf(strsql,"select kont,pnaz,organ,celk,datn,datk,nompr,datp from Ukrdok \
where datp >= '%d-%d-%d' and datp <= '%d-%d-%d' and kont='%s' order by nompr asc",
  gn,mn,dn,gk,mk,dk,kodruk);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  
  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    if(iceb_u_SRAV(nompr,row[6],0) != 0 || (kolstr1 == 1 && row[6][0] == '\0'))
     {
      if(kolprik != 0 || kolprik1 != 0)
        fprintf(ff,"\f");

      if(kolprik1 != 0)
          fprintf(ff,"\n%10s %s___________________%s\n"," ",dolgnruk,fioruk);

      fprintf(ff,"%s\n\n",organ);
      fprintf(ff,"%25s%s %s\n"," ",gettext("ПРИКАЗ НОМЕР"),row[6]);

      iceb_u_rsdat(&d,&m,&g,row[7],2);

      fprintf(ff,"%25s%s %02d.%02d.%d%s\n\n"," ",gettext("от"),d,m,g,
      gettext("г."));

      fprintf(ff,"%s\n",gettext("Про командировку"));
      fprintf(ff,"%s:\n",gettext("Отбываю в командировку"));

      strcpy(nompr,row[6]);
      nomer=0;    
      kolprik1++;    
     }  
    iceb_u_rsdat(&dnp,&mnp,&gnp,row[4],2);
    iceb_u_rsdat(&dkp,&mkp,&gkp,row[5],2);
    
    memset(fio,'\0',sizeof(fio));
    //Читаем наименование контрагента
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[0]);
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
      strncpy(fio,row1[0],sizeof(fio)-1);
      
    fprintf(ff,"\n%s %s\n",fio,dolgnruk);
    fprintf(ff,"%10s %s %s\n"," ",row[1],row[2]);
    fprintf(ff,"%10s %s %s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
    " ",row[3],
    gettext("с"),dnp,mnp,gnp,gettext("г."),
    gettext("по"),dkp,mkp,gkp,gettext("г."));
   }
  fprintf(ff,"\n%10s %s___________________%s\n"," ",dolgnruk,fioruk);
   
 }



fclose(ff);

data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Распечатка приказов на командировку"));
iceb_ustpeh(imaf,1,data->window);



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
