/*$Id: uos_spw_r.c,v 1.8 2011-02-21 07:35:58 sasa Exp $*/
/*17.11.2009	15.01.2008	Белых А.И.	uos_spw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "uos_spw.h"

class uos_spw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class uos_spw_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  int metka_r;  
  uos_spw_r_data()
   {
    metka_r=0;
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uos_spw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uos_spw_r_data *data);
gint uos_spw_r1(class uos_spw_r_data *data);
void  uos_spw_r_v_knopka(GtkWidget *widget,class uos_spw_r_data *data);

void	sapkasord(short dd,short md,short gd,int nomlist,FILE *ffva);
void   hgrkd(FILE *ff);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern int      kol_strok_na_liste;

int uos_spw_r(int metka_r,class uos_spw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class uos_spw_r_data data;
data.rk=datark;
data.metka_r=metka_r;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

if(metka_r == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Расчёт движения основных средств по счетам списания"));
if(metka_r == 0)
  sprintf(strsql,"%s %s",name_system,gettext("Расчёт движения основных средств по счетам получения"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uos_spw_r_key_press),&data);

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
if(metka_r == 1)
  repl.plus(gettext("Расчёт движения основных средств по счетам списания"));
if(metka_r == 0)
  repl.plus(gettext("Расчёт движения основных средств по счетам получения"));

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(uos_spw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)uos_spw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uos_spw_r_v_knopka(GtkWidget *widget,class uos_spw_r_data *data)
{
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uos_spw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uos_spw_r_data *data)
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

gint uos_spw_r1(class uos_spw_r_data *data)
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

iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window );


sprintf(strsql,"select datd,innom,nomd,podr,kodol,bs,iz,kodop,bsby,izby,shs from Uosdok1 \
where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and tipz=%d and podt=1 order by shs,datd asc",
gn,mn,dn,gk,mk,dk,data->metka_r+1);

int kolstr=0;
class SQLCURSOR cur1;
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

FILE *ff;
char imaf[56];
sprintf(imaf,"uos_sp%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(data->metka_r+1 == 1)
 iceb_u_zagolov(gettext("Движение основных средств по счетам приобретения"),dn,mn,gn,dk,mk,gk,organ,ff);
if(data->metka_r+1 == 2)
 iceb_u_zagolov(gettext("Движение основных средств по счетам списания"),dn,mn,gn,dk,mk,gk,organ,ff);
 
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Инв.номер |      Наименование            |Счёт спис.|Счёт учёта|Бал.с.н.у.|Износ н.у.|Бал.с.б.у.|Износ б.у.|Дата доку.|Номер доку|Подраздел.|Мат.отв.л.|Код оп.|Контраген.|\n"));

/*************
1234567890 123456789012345678901234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567 1234567890 
**********/

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");


SQL_str row1;
float kolstr1=0;
long in_nom;
short d,m,g;
char naim_uos[200];
double bs,iz,bsbu,izbu;
double ibs=0.,iiz=0.,ibsbu=0.,iizbu=0.;
double ibs_shet=0.,iiz_shet=0.,ibsbu_shet=0.,iizbu_shet=0.;
int podr=0;
int kodol=0;
char podr_char[100];
char kodol_char[100];
char kontr[50];
char shet_sp[50];
char shet_sp_zap[50];
memset(shet_sp_zap,'\0',sizeof(shet_sp_zap));

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->shet_sp.ravno(),row[10],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->kod_op.ravno(),row[7],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->podr.ravno(),row[3],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->mat_ot.ravno(),row[4],0,0) != 0)
    continue;
  
  memset(kontr,'\0',sizeof(kontr));
  sprintf(strsql,"select kontr from Uosdok where datd='%s' and nomd='%s'",row[0],row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strncpy(kontr,row1[0],sizeof(kontr)-1);
   
  if(data->rk->kontr.getdlinna() > 1)
   {
    if(iceb_u_proverka(data->rk->kontr.ravno(),kontr,0,0) != 0)
      continue;    
   }
  in_nom=atol(row[1]);
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  class poiinpdw_data rekin;
  poiinpdw(in_nom,m,g,&rekin,data->window);

  if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
    continue;
  
  poiinw(atol(row[1]),d,m,g,&podr,&kodol,data->window);
  
  memset(naim_uos,'\0',sizeof(naim_uos));
  sprintf(strsql,"select naim from Uosin where innom=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strncpy(naim_uos,row1[0],sizeof(naim_uos)-1);

  memset(podr_char,'\0',sizeof(podr_char));
  sprintf(podr_char,"%d ",podr);
  sprintf(strsql,"select naik from Uospod where kod=%d",podr);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strcat(podr_char,row1[0]);

  memset(kodol_char,'\0',sizeof(kodol_char));
  sprintf(kodol_char,"%d ",kodol);
  sprintf(strsql,"select naik from Uosol where kod=%d",kodol);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strcat(kodol_char,row1[0]);
  
  memset(shet_sp,'\0',sizeof(shet_sp));
  strcpy(shet_sp,row[10]);
  if(row[10][0] == '\0')
   strcpy(shet_sp,"???");  
  
  bs=atof(row[5]);
  iz=atof(row[6]);  
  bsbu=atof(row[8]);
  izbu=atof(row[9]);  
  
  ibs+=bs;
  iiz+=iz;
  ibsbu+=bsbu;
  iizbu+=izbu;

  ibs_shet+=bs;
  iiz_shet+=iz;
  ibsbu_shet+=bsbu;
  iizbu_shet+=izbu;
    
  if(iceb_u_SRAV(shet_sp_zap,shet_sp,0) != 0)
   {
    if(shet_sp_zap[0] != '\0')
     {
      sprintf(strsql,"%s %s",gettext("Итого по счёту"),shet_sp);
      fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f\n",
      iceb_u_kolbait(63,strsql),strsql,
      ibs_shet,iiz_shet,ibsbu_shet,iizbu_shet);
      
      ibs_shet=0.;
      iiz_shet=0.;
      ibsbu_shet=0.;
      iizbu_shet=0.;
      
     }
   }

  fprintf(ff,"%10s %-*.*s %-*s %-*s %10.2f %10.2f %10.2f %10.2f %02d.%02d.%04d %-10s %-10.10s %-10.10s %-*s %s\n",
  row[1],
  iceb_u_kolbait(30,naim_uos),iceb_u_kolbait(30,naim_uos),naim_uos,
  iceb_u_kolbait(10,shet_sp),shet_sp,
  iceb_u_kolbait(10,rekin.shetu.ravno()),rekin.shetu.ravno(),
  bs,iz,bsbu,izbu,d,m,g,row[2],podr_char,kodol_char,
  iceb_u_kolbait(7,row[7]),row[7],
  kontr);

 }

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
sprintf(strsql,"%s %s",gettext("Итого по счёту"),shet_sp);
fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f\n",iceb_u_kolbait(63,strsql),strsql,ibs_shet,iiz_shet,ibsbu_shet,iizbu_shet);
fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f\n",iceb_u_kolbait(63,gettext("Итого")),gettext("Итого"),ibs,iiz,ibsbu,iizbu);

iceb_podpis(ff,data->window);

fclose(ff);

iceb_ustpeh(imaf,0,data->window);

data->rk->imaf.plus(imaf);

if(data->metka_r+1 == 1)
  data->rk->naim.plus(gettext("Расчёт движения основных средств по счетам получения"));
if(data->metka_r+1 == 2)
  data->rk->naim.plus(gettext("Расчёт движения основных средств по счетам списания"));







gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
