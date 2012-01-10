/*$Id: copdok_r.c,v 1.17 2011-02-21 07:35:51 sasa Exp $*/
/*28.03.2008	19.10.2004	Белых А.И.	copdok_r.c
Выполнение операции копирования документов в материальном учёте
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "buhg_g.h"
#include "copdok.h"

class copdok_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  short prkk;
  
  class copdok_data *rk;
    

 };

gboolean   copdok_r_key_press(GtkWidget *widget,GdkEventKey *event,class copdok_r_data *data);
gint copdok_r1(class copdok_r_data *data);
void  copdok_r_v_knopka(GtkWidget *widget,class copdok_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern iceb_u_str shrift_ravnohir;
 
void copdok_r(class copdok_data *rek_ras,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class copdok_r_data data;
data.rk=rek_ras;

repl_s.plus(gettext("Если не делать привязки к карточкам, то одинаковые материалы\n\
с разных документов и разных карточек будут записаны одной записью с общим\n\
количеством и наибольшей ценой."));

if(rek_ras->tipz == 1)
  repl_s.plus(gettext("При выполнении привязки, привязка выполняется\n\
к карточкам с такою же ценой, или заводятся новые карточки."));

repl_s.plus(gettext("Сделать привязку к карточкам ?"));

data.prkk=iceb_menu_danet(&repl_s,2,wpredok);






data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Копировать записи из других документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(copdok_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Копировать записи из других документов"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(copdok_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)copdok_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  copdok_r_v_knopka(GtkWidget *widget,class copdok_r_data *data)
{
 printf("copdok_r_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   copdok_r_key_press(GtkWidget *widget,GdkEventKey *event,class copdok_r_data *data)
{
 printf("copdok_r_key_press\n");

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

gint copdok_r1(class copdok_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
char bros[512];
int kolstr;
iceb_u_str repl;
int koldok=0;
int kolsz=0;
sprintf(strsql,"select * from Dokummat \
where datd >= '%s' and datd <= '%s'",
data->rk->datan.ravno_sqldata(),data->rk->datak.ravno_sqldata());
SQLCURSOR cur;
SQLCURSOR cur1;
SQL_str row;
SQL_str row1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  repl.new_plus(gettext("Не найдено ни одного документа !"));
  iceb_menu_soob(&repl,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int tp;
int tipzz=0;
//if(prra[0] == '+')
if(data->rk->pr_ras.ravno()[0] == '+')
  tipzz=1;
if(data->rk->pr_ras.ravno()[0] == '-')
  tipzz=2;
char nomdok1[30];
char kodop[32];
int skl1;
short d,m,g;
double sumkor;
float kolstr1=0.;
int kolstr2;
int kodm;
int nk;
double cena;
double cenaz;
double cenap;
double kolih;
double nds;
int mnds;
char ei[32];
char nomz[50];
char shbm[30];
double krt;
double fas;
int kodt;
char innom[30];
char rnd[40];
short denv=0,mesv=0,godv=0; //Дата ввода в эксплуатацию для малоценки
short deng,mesg,godg;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  tp=atoi(row[0]);
  if(tipzz != 0 && tipzz != tp)
      continue;

  if(iceb_u_proverka(data->rk->kontr.ravno(),row[3],0,0) != 0)
     continue;

  if(iceb_u_proverka(data->rk->kodop.ravno(),row[6],0,0) != 0)
     continue;

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[2],0,0) != 0)
     continue;

  if(iceb_u_proverka(data->rk->nomdok.ravno(),row[4],0,0) != 0)
     continue;

  strncpy(nomdok1,row[4],sizeof(nomdok1));    

  iceb_u_rsdat(&d,&m,&g,row[1],2);  
  strncpy(kodop,row[6],sizeof(kodop)-1);

  skl1=atoi(row[2]);
  /*Не копировать себя самого*/
  if(skl1 == data->rk->skl && iceb_u_SRAV(nomdok1,data->rk->nomdok_c.ravno(),0) == 0 && data->rk->gd == g)
    continue;

  memset(bros,'\0',sizeof(bros));
  if(tp == 1)
   strcpy(bros,"+");
  if(tp == 1)
   strcpy(bros,"-");

  sprintf(strsql,"%s %02d.%02d.%d %-3s %-4s %-5s %s\n",
  bros,d,m,g,row[6],row[3],row[4],row[2]);

  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  
  /*Читаем сумму корректировки*/
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%s and nomerz=13",
  g,row[4],row[2]);

  sumkor=0.;
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    sumkor=atof(row1[0]);
  if(sumkor != 0)
   {
    sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=13",
    data->rk->gd,data->rk->nomdok_c.ravno(),data->rk->skl);

    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      sumkor+=atof(row1[0]);
      sprintf(strsql,"delete from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=13",
      data->rk->gd,data->rk->nomdok_c.ravno(),data->rk->skl);
      if(sql_zap(&bd,strsql) != 0)
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',%d,'%.2f')",data->rk->gd,data->rk->skl,data->rk->nomdok_c.ravno(),13,sumkor);
    if(sql_zap(&bd,strsql) != 0)
        iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);
    
   }  


  sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s'",
  g,m,d,skl1,nomdok1);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr2 == 0)
    continue;
  deng=mesg=godg=0;         
  koldok++;
  while(cur1.read_cursor(&row1) != 0)
   {

    kodm=atoi(row1[4]);
    nk=atoi(row1[3]);
    cena=atof(row1[6]);
    kolih=atof(row1[5]);
    nds=atof(row1[9]);
    mnds=atoi(row1[10]);
    strncpy(ei,row1[7],sizeof(ei)-1);
    strncpy(nomz,row1[17],sizeof(nomz)-1);
    
    /*Проверяем нет ли уже такой записи*/
    
    if(data->rk->tipz == 2)
     if(data->prkk == 2 || skl1 != data->rk->skl)
       nk=0;
    if(data->rk->tipz == 1 && data->prkk == 2)
       nk=0;

    SQLCURSOR cur2;
    SQL_str row2;
    if(data->rk->tipz == 1 && data->rk->skl != skl1 && nk != 0 && data->prkk == 1) /*Приход*/
     {

/*Читаем реквизиты карточки на чужом складе и если нет на текущем
складе карточки с такимиже реквизитами то заводим ее*/

      sprintf(strsql,"select * from Kart where sklad=%d and nomk=%d",
      skl1,nk);
      if(sql_readkey(&bd,strsql,&row2,&cur2) != 1)
       {
        sprintf(strsql,"Не знайшли картки %d, склад %d, матеріал %d\n",
        nk,skl1,kodm);
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
        continue;
       }

      mnds=atoi(row2[3]);
      memset(ei,'\0',sizeof(ei));
      strncpy(ei,row2[4],sizeof(ei)-1);
      memset(shbm,'\0',sizeof(shbm));
      strncpy(shbm,row2[5],sizeof(shbm)-1); 
      cenaz=atof(row2[6]);
      cenap=atof(row2[7]);
      krt=atof(row2[8]);
      nds=atof(row2[9]);
      fas=atof(row2[10]);
      kodt=atoi(row2[11]);
      strcpy(innom,row2[15]);
      strncpy(rnd,row2[16],sizeof(rnd)-1);
      iceb_u_rsdat(&deng,&mesg,&godg,row[18],2);    
            
      sprintf(strsql,"select nomk from Kart where kodm=%d and \
sklad=%d and shetu='%s' and cena=%.10g and ei='%s' and nds=%.10g \
and cenap=%.10g and krat=%.10g and mnds=%d and innom='%s'",
      kodm,data->rk->skl,shbm,cenaz,ei,nds,cenap,krt,mnds,innom);

      if(sql_readkey(&bd,strsql,&row2,&cur2) != 1)
       {
        nk=nomkrw(data->rk->skl,data->window);

        sprintf(strsql,"Записываем новую карточку N%d, склад %d, материал %d\n",
        nk,skl1,kodm);
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        zapkarw(data->rk->skl,nk,kodm,cenaz,ei,shbm,krt,fas,kodt,nds,mnds,cenap,\
        denv,mesv,godv,innom,rnd,nomz,deng,mesg,godg,0,data->window);

       }
      else
       nk=atoi(row2[0]);
     }

    kolsz++;

    sprintf(strsql,"select kolih,cena from Dokummat1 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and \
nomkar=%d",
    data->rk->gd,data->rk->md,data->rk->dd,data->rk->skl,data->rk->nomdok_c.ravno(),kodm,nk);
      
    if(sql_readkey(&bd,strsql,&row2,&cur2) == 1)
     {
      kolih+=atof(row2[0]);
      cenaz=atof(row2[1]);
/*
      printw("Перезаписываем %d.%d.%d %s %d\n",data->rk->dd,data->rk->md,data->rk->gd,nomn,data->rk->skl);
      refresh();  
*/
      if(fabs(cenaz-cena) > 0.009)
       {
        sprintf(strsql,"%s %s %s %d.%d.%d, %s %d. %s %.10g %.10g\n\%s.\n",
        gettext("Документ"),
        nomdok1,
        gettext("от"),
        d,m,g,
        gettext("материал"),
        kodm,
        gettext("Не совпадает цена !"),
        cena,cenaz,
        gettext("Берем большую цену"));
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        if(cena-cenaz < 0.)
         cena=cenaz;
       }
      
      sprintf(strsql,"update Dokummat1 \
set \
cena=%.10g,\
kolih=%.10g,\
ktoi=%d \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
      cena,kolih,iceb_getuid(data->window),data->rk->gd,data->rk->md,data->rk->dd,data->rk->skl,data->rk->nomdok_c.ravno(),kodm,nk);

      if(sql_zap(&bd,strsql) != 0)
        iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);

     }
    else
     {
/*
      printw("Пишем %d.%d.%d %s %d %d\n",data->rk->dd,data->rk->md,data->rk->gd,data->rk->nomdok_c.ravno(),data->rk->skl,nk);
      refresh();  
*/
      zapvdokw(data->rk->dd,data->rk->md,data->rk->gd,data->rk->skl,nk,kodm,
      data->rk->nomdok_c.ravno(),kolih,cena,ei,nds,mnds,0,
      data->rk->tipz,0,"",nomz,data->window);

     }


   }


  /*Удаление после копирования*/
/*********************************
  if(mud == 1)
   {
//    sprintf(strsql,"%s/%s/blokmak.alx",putnansi,imabaz);
    if(prblm(m,g,strsql) != 0)
     {
      beep();
      printw(gettext("Документ %s удалить невозможно. Дата %d.%dг. заблокирована !"),
      data->rk->nomdok_c.ravno()1,m,g);
      printw("\n");
      continue;
     }

    if(udprgr(gettext("МУ"),d,m,g,data->rk->nomdok_c.ravno(),skl1,tp) != 0)
     continue;
    matudd(0,0,0,tp,d,m,g,data->rk->nomdok_c.ravno(),skl1,0,0);

   } 
****************************/

 }




gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

sprintf(strsql,"%s %d !",gettext("Количество переписаних документов"),koldok);
repl.new_plus(strsql);
sprintf(strsql,"%s %d !",gettext("Количество переписаних записей"),kolsz);
repl.ps_plus(strsql);
iceb_menu_soob(&repl,data->window);

return(FALSE);
}
