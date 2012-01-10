/*$Id: zarpensm1w_r.c,v 1.9 2011-09-05 08:18:27 sasa Exp $*/
/*16.07.2011	21.02.2010	Белых А.И.	zarpensm1w_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zarpensmw.h"
#include        "dbfhead.h"

class zarpensm1_nastr
 {
  public: 
   class iceb_u_str kodpfu; /*код пенсионного фонда Украины*/
   class iceb_u_str kodfil; /*код филиала*/
   class iceb_u_str kodzvna; /*код звания неатестованых*/
   class iceb_u_str ktn1;
   class iceb_u_str ktn2;
   class iceb_u_str ktn3;
   class iceb_u_str ktn4;
   class iceb_u_str ktn5;
   
  zarpensm1_nastr()
   {
    kodpfu.new_plus("");
    kodfil.new_plus("");
    kodzvna.new_plus("");
    ktn1.new_plus("");    
    ktn2.new_plus("");    
    ktn3.new_plus("");    
    ktn4.new_plus("");    
    ktn5.new_plus("");    
   }
 };
const char *version_dok1={"D"};

class zarpensm1w_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zarpensmw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zarpensm1w_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zarpensm1w_r_key_press(GtkWidget *widget,GdkEventKey *event,class zarpensm1w_r_data *data);
gint zarpensm1w_r1(class zarpensm1w_r_data *data);
void  zarpensm1w_r_v_knopka(GtkWidget *widget,class zarpensm1w_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern int      kol_strok_na_liste;
extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/
extern class iceb_u_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/
extern short *kodbl;  /*Коды начисления больничного*/
//extern float  max_sum_for_soc; //Максимальная сумма с которой начисляются соц.отчисления

int zarpensm1w_r(class zarpensmw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zarpensm1w_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка списка документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zarpensm1w_r_key_press),&data);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zarpensm1w_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zarpensm1w_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zarpensm1w_r_v_knopka(GtkWidget *widget,class zarpensm1w_r_data *data)
{
//printf("zarpensm1w_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zarpensm1w_r_key_press(GtkWidget *widget,GdkEventKey *event,class zarpensm1w_r_data *data)
{
// printf("zarpensm1w_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}
/********************************/
/*Чтение настроек*/
/********************************/
int zarpensm1_read_nast(class zarpensm1_nastr *nastr,GtkWidget *wpredok)
{
class iceb_u_str naim("");
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zarpensm.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],&naim,1,'|') != 0)
   continue;  
  if(iceb_u_SRAV("Код отделения пенсионного фонда Украины",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->kodpfu,2,'|');
    continue;
   }
  if(iceb_u_SRAV("Код филиала пенсионного фонда",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->kodfil,2,'|');
    continue;
   }
  if(iceb_u_SRAV("Коды начислений для кода типа начисления 1",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->ktn1,2,'|');
    continue;
   }
  if(iceb_u_SRAV("Коды начислений для кода типа начисления 2",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->ktn2,2,'|');
    continue;
   }
  if(iceb_u_SRAV("Коды начислений для кода типа начисления 3",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->ktn3,2,'|');
    continue;
   }
  if(iceb_u_SRAV("Коды начислений для кода типа начисления 4",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->ktn4,2,'|');
    continue;
   }
  if(iceb_u_SRAV("Коды начислений для кода типа начисления 5",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->ktn5,2,'|');
    continue;
   }
  if(iceb_u_SRAV("Коды звания неатестованых",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->kodzvna,2,'|');
    continue;
   }
 }
return(0);
}
/**************************************/
void zarpensm1_xml_sap(short mr,short gr,
const char *kodpfu,
const char *imaf,
const char *c_doc_sub,
FILE *ff_xml,
GtkWidget *wpredok)
{
class iceb_u_str naim_or("");
short dt,mt,gt;
class iceb_u_str inn("");
iceb_u_poltekdat(&dt,&mt,&gt);
SQL_str row;
class SQLCURSOR cur;

if(iceb_sql_readkey("select naikon,kod from Kontragent where kodkon='00'",&row,&cur,wpredok) == 1)
 {
  inn.new_plus(row[1]);
  naim_or.new_plus(row[0]);
 } 

fprintf(ff_xml,"\
<?xml version=\"1.0\" encoding=\"windows-1251\" ?>\n\
<DECLAR xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"%s.xsd\">\n",imaf);

fprintf(ff_xml,"\
 <DECLARHEAD>\n\
  <VER>1.1</VER>\n\
  <C_DOC>P04</C_DOC>\n\
  <C_DOC_SUB>%s</C_DOC_SUB>\n\
  <C_DOC_VER>%s</C_DOC_VER>\n",
  c_doc_sub,version_dok1);

if(kodpfu[0] != '\0')  
 fprintf(ff_xml,"  <C_PFU>%s</C_PFU>\n",kodpfu);
else
 fprintf(ff_xml,"  <C_PFU/>\n");

fprintf(ff_xml,"\
  <SOFTWARE>iceBw %s</SOFTWARE>\n\
  <TIN>%s</TIN>\n\
  <PERIOD_MONTH>%d</PERIOD_MONTH>\n\
  <PERIOD_YEAR>%d</PERIOD_YEAR>\n\
  <DATA_FILL>%02d%02d%d</DATA_FILL>\n\
 </DECLARHEAD>\n",
VERSION,inn.ravno(),mr,gr,dt,mt,gt);

fprintf(ff_xml,"\
 <DECLARBODY>\n\
  <PAGE>\n\
   <HEADER>\n\
    <FIRM_EDRPOU>%s</FIRM_EDRPOU>\n\
    <FIRM_FIL></FIRM_FIL>\n\
    <FORM_TYPE>1</FORM_TYPE>\n\
    <FIRM_NAME>%s</FIRM_NAME>\n\
    <PAGE_NUM>1</PAGE_NUM>\n\
   </HEADER>\n\
   <TABLE>\n",
inn.ravno(),iceb_u_filtr_xml(naim_or.ravno()));
/************
fprintf(ff_xml,"\
 <DECLARBODY>\n\
  <FIRM_EDRPOU>%s</FIRM_EDRPOU>\n\
  <FORM_TYPE>1</FORM_TYPE>\n\
  <FIRM_NAME>%s</FIRM_NAME>\n",
inn.ravno(),iceb_u_filtr_xml(naim_or.ravno()));
*************/
}
/************************************/
/*концовка распечатки*/
/*************************************/

void zarpensm1_kon_lst(int kolstr,
const char *inn_ker,
const char *fio_ker,
const char *inn_gb,
const char *fio_gb,
FILE *ff)
{
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

fprintf(ff,"\
Кількість заповнених рядків %d\n\
Дата формування звіту у страхувальника %d.%d.%d \n\n\
Керівник           %-10s _______________ %s\n\n\
M.П.\n\n\
Головний бухгалтер %-10s ________________ %s\n",
kolstr,
dt,mt,gt,
inn_ker,
fio_ker,
inn_gb,
fio_gb); 


}
/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void zarpensm11_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
int *header_len,int *rec_len)
{

strncpy(f->name, fn, 11);
f->type = ft;
f->length = l1;
f->dec_point = l2;
*header_len=*header_len+sizeof(DBASE_FIELD);
*rec_len=*rec_len+l1;
}
/***********************************************/
void zarpensm1_tab5_dbf(const char *imaf,int kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;
const int kolpol=10;
DBASE_FIELD f[kolpol];
memset(&f, '\0', sizeof(f));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

memset(&h,'\0',sizeof(h));

h.version = 3;

time(&tmm);
bf=localtime(&tmm);

h.l_update[0] = bf->tm_year;       /* yymmdd for last update*/
h.l_update[1] = bf->tm_mon+1;       /* yymmdd for last update*/
h.l_update[2] = bf->tm_mday;       /* yymmdd for last update*/

h.count = kolz;              /* number of records in file*/

header_len = sizeof(h);
rec_len = 0;
int shetshik=0;
zarpensm11_f(&f[shetshik++],"PERIOD_M", 'N', 2, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"PERIOD_Y", 'N', 4, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"ROWNUM", 'N', 6, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"UKR_GROMAD", 'N', 1, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"NUMIDENT", 'C', 10, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"LN", 'C',100, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"NM", 'C',100, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"FTN", 'C',100, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"START_DT", 'N',2, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"END_DT", 'N', 2, 0,&header_len,&rec_len);


h.header = header_len + 1;/* length of the header
                           * includes the \r at end
                           */
h.lrecl= rec_len + 1;     /* length of a record
                           * includes the delete
                           * byte
                          */
/*
 printw("h.header=%d h.lrecl=%d\n",h.header,h.lrecl);
*/


fd = fileno(ff);

write(fd, &h, sizeof(h));
for(i=0; i < kolpol; i++) 
 {
  write(fd, &f[i], sizeof(DBASE_FIELD));
 }
fputc('\r', ff);

fclose(ff);

}

/***********************************************/
void zarpensm1_tab6_dbf(const char *imaf,int kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;
const int kolpol=18;
DBASE_FIELD f[kolpol];
memset(&f, '\0', sizeof(f));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

memset(&h,'\0',sizeof(h));

h.version = 3;

time(&tmm);
bf=localtime(&tmm);

h.l_update[0] = bf->tm_year;       /* yymmdd for last update*/
h.l_update[1] = bf->tm_mon+1;       /* yymmdd for last update*/
h.l_update[2] = bf->tm_mday;       /* yymmdd for last update*/

h.count = kolz;              /* number of records in file*/

header_len = sizeof(h);
rec_len = 0;
int shetshik=0;
zarpensm11_f(&f[shetshik++],"PERIOD_M", 'N', 2, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"PERIOD_Y", 'N', 4, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"ROWNUM", 'N', 6, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"UKR_GROMAD", 'N', 1, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"ST", 'N', 1, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"NUMIDENT", 'C', 10, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"LN", 'C',100, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"NM", 'C',100, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"FTN", 'C',100, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"ZO", 'N',2, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"PAY_TP", 'N', 2, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"PAY_MNTH", 'N', 2, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"PAY_YEAR", 'N', 4, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"SUM_TOTAL", 'N',16, 2,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"SUM_MAX", 'N', 16, 2,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"SUM_INS", 'N', 16, 2, &header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"OTK", 'N', 1, 0, &header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"EXP", 'N', 1, 0, &header_len,&rec_len);


h.header = header_len + 1;/* length of the header
                           * includes the \r at end
                           */
h.lrecl= rec_len + 1;     /* length of a record
                           * includes the delete
                           * byte
                          */
/*
 printw("h.header=%d h.lrecl=%d\n",h.header,h.lrecl);
*/


fd = fileno(ff);

write(fd, &h, sizeof(h));
for(i=0; i < kolpol; i++) 
 {
  write(fd, &f[i], sizeof(DBASE_FIELD));
 }
fputc('\r', ff);

fclose(ff);

}

/***********************************************/
void zarpensm1_tab7_dbf(const char *imaf,int kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;
const int kolpol=19;
DBASE_FIELD f[kolpol];
memset(&f, '\0', sizeof(f));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

memset(&h,'\0',sizeof(h));

h.version = 3;

time(&tmm);
bf=localtime(&tmm);

h.l_update[0] = bf->tm_year;       /* yymmdd for last update*/
h.l_update[1] = bf->tm_mon+1;       /* yymmdd for last update*/
h.l_update[2] = bf->tm_mday;       /* yymmdd for last update*/

h.count = kolz;              /* number of records in file*/

header_len = sizeof(h);
rec_len = 0;
int shetshik=0;
zarpensm11_f(&f[shetshik++],"PERIOD_M", 'N', 2, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"PERIOD_Y", 'N', 4, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"ROWNUM", 'N', 6, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"UKR_GROMAD", 'N', 1, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"NUMIDENT", 'C', 10, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"LN", 'C',100, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"NM", 'C',100, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"FTN", 'C',100, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"C_PID", 'C',9, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"START_DT", 'N', 2, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"STOP_DT", 'N',2, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"DAYS", 'N',4, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"NORMZ", 'N',4, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"HH", 'N',4, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"MM", 'N', 2, 0,&header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"NORMA", 'N', 6, 0, &header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"NUM_NAK", 'C', 8, 0, &header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"DT_NAK", 'N', 8, 0, &header_len,&rec_len);
zarpensm11_f(&f[shetshik++],"SEAZON", 'N', 1, 0, &header_len,&rec_len);


h.header = header_len + 1;/* length of the header
                           * includes the \r at end
                           */
h.lrecl= rec_len + 1;     /* length of a record
                           * includes the delete
                           * byte
                          */
/*
 printw("h.header=%d h.lrecl=%d\n",h.header,h.lrecl);
*/


fd = fileno(ff);

write(fd, &h, sizeof(h));
for(i=0; i < kolpol; i++) 
 {
  write(fd, &f[i], sizeof(DBASE_FIELD));
 }
fputc('\r', ff);

fclose(ff);

}
/*********************************************/
/*вывод в таблицу 6*/
/*******************************/
void zarpensm1_str_tab6(int nomer_str_tab6,
short mr,short gr, /*дата расчёта*/
const char *inn,
const char *fio,
int metka_ukr_grom,
int kodtipnah,
const char *metka_pola,
const char *mv_gv, /*Месяц и год в счёт которых выплачено*/
double suma_obh,
double suma,
double suma_esv_v,
int metka_otk,
int metka_exp,
const char *tabnom,
FILE *ff_tab6,
FILE *ff_tab6_dbf,
FILE *ff_tab6_xml)
{
short mv=0,gv=0;
class iceb_u_str prizv("");
class iceb_u_str ima("");
class iceb_u_str othestvo("");

if(iceb_u_polen(fio,&prizv,1,' ') != 0)
 prizv.new_plus(fio);
 
iceb_u_polen(fio,&ima,2,' ');
iceb_u_polen(fio,&othestvo,3,' ');

iceb_u_rsdat1(&mv,&gv,mv_gv);
int zo=0;
fprintf(ff_tab6,"%5d|%d|%s|%10s|%-*.*s|%4d|%4d|%02d.%04d|%10.2f|%10.2f|%8.2f|%4d|%4d|%s\n",
nomer_str_tab6,
metka_ukr_grom,
metka_pola,
inn,
iceb_u_kolbait(30,fio), 
iceb_u_kolbait(30,fio), 
fio,
zo,
kodtipnah,
mv,gv,
suma_obh,suma,suma_esv_v,metka_otk,metka_exp,
tabnom);

int metka_pola_int=0;

if(iceb_u_SRAV(metka_pola,"Ч",0) == 0)
 metka_pola_int=1;



fprintf(ff_tab6_xml,"\
  <ROW>\n\
   <ROWNUM>%d</ROWNUM>\n",nomer_str_tab6);

fprintf(ff_tab6_xml,"\
   <UKR_GROMAD>%d</UKR_GROMAD>\n",metka_ukr_grom);

fprintf(ff_tab6_xml,"\
   <ST>%ds</ST>\n",metka_pola_int);
   
if(inn[0] == '\0')
  fprintf(ff_tab6_xml,"\
   <NUMIDENT xsi:nil=\"true\" />\n");
else
  fprintf(ff_tab6_xml,"\
   <NUMIDENT>%s</NUMIDENT>\n",inn);
/*************
fprintf(ff_tab6_xml,"\
   <ZO>%d</ZO>\n",zo);
**************/
if(kodtipnah > 0)
fprintf(ff_tab6_xml,"\
   <PAY_TP>%d</PAY_TP>\n",kodtipnah);
else
fprintf(ff_tab6_xml,"\
   <PAY_TP xsi:nil=\"true\" />\n");

if(gv != gr || mv != mr)   
 {
  fprintf(ff_tab6_xml,"\
   <PAY_MNTH>%d</PAY_MNTH>\n",mv);
  fprintf(ff_tab6_xml,"\
   <PAY_YEAR>%d</PAY_YEAR>\n",gv);
 }
if(prizv.getdlinna() > 1)    
 fprintf(ff_tab6_xml,"\
   <LN>%s</LN>\n",iceb_u_filtr_xml(prizv.ravno()));
else
 fprintf(ff_tab6_xml,"\
   <LN xsi:nil=\"true\"></LN>\n");

if(ima.getdlinna() > 1)
 fprintf(ff_tab6_xml,"\
   <NM>%s</NM>\n",iceb_u_filtr_xml(ima.ravno()));
else
 fprintf(ff_tab6_xml,"\
   <NM xsi:nil=\"true\"></NM>\n");

if(othestvo.getdlinna() > 1)
 fprintf(ff_tab6_xml,"\
   <FTN>%s</FTN>\n",iceb_u_filtr_xml(othestvo.ravno()));
else
 fprintf(ff_tab6_xml,"\
   <FTN xsi:nil=\"true\"></FTN>\n");


fprintf(ff_tab6_xml,"\
   <SUM_TOTAL>%.2f</SUM_TOTAL>\n\
   <SUM_MAX>%.2f</SUM_MAX>\n\
   <SUM_INS>%.2f</SUM_INS>\n\
   <OTK>%d</OTK>\n\
   <EXP>%d</EXP>\n\
  </ROW>\n",
suma_obh,
suma,
suma_esv_v,
metka_otk,
metka_exp);


 
fprintf(ff_tab6_dbf," %02d%04d%06d%1d%1d%-10s%-*.*s%-*.*s%-*.*s%2d%2d%2d%4d%16.2f%16.2f%16.2f%1d%1d",
mr,gr,
nomer_str_tab6,
metka_ukr_grom,
metka_pola_int,
inn,
iceb_u_kolbait(100,prizv.ravno()),
iceb_u_kolbait(100,prizv.ravno()),
prizv.ravno(),
iceb_u_kolbait(100,ima.ravno()),
iceb_u_kolbait(100,ima.ravno()),
ima.ravno(),
iceb_u_kolbait(100,othestvo.ravno()),
iceb_u_kolbait(100,othestvo.ravno()),
othestvo.ravno(),
zo,
kodtipnah,
mv,gv,
suma_obh,
suma,
suma_esv_v,
metka_otk,
metka_exp);



}
/**********************************************/
/*Вывод строки в таблицу 5*/
/********************************/
void zarpensm1_str_tab5(int nomer_str_tab5,
const char *inn,
const char *fio,
short dpnr,
short dusr,
short mr,short gr,
int metka_ukr_grom,
int zo, /*категория работника*/
FILE *ff_tab5,
FILE *ff_tab5_dbf,
FILE *ff_tab5_xml)
{
class iceb_u_str prizv("");
class iceb_u_str ima("");
class iceb_u_str othestvo("");

if(iceb_u_polen(fio,&prizv,1,' ') != 0)
 prizv.new_plus(fio);
iceb_u_polen(fio,&ima,2,' ');
iceb_u_polen(fio,&othestvo,3,' ');

fprintf(ff_tab5,"%5d|%10s|%-*.*s|%7d|%7d|\n",
nomer_str_tab5,
inn,
iceb_u_kolbait(40,fio), 
iceb_u_kolbait(40,fio), 
fio,
dpnr,
dusr);

fprintf(ff_tab5_dbf," %02d%04d%06d%1d%10.10s%-*.*s%-*.*s%-*.*s%02d%02d",
mr,gr,nomer_str_tab5,metka_ukr_grom,
inn,
iceb_u_kolbait(100,prizv.ravno()),
iceb_u_kolbait(100,prizv.ravno()),
prizv.ravno(),    
iceb_u_kolbait(100,ima.ravno()),
iceb_u_kolbait(100,ima.ravno()),
ima.ravno(),    
iceb_u_kolbait(100,othestvo.ravno()),
iceb_u_kolbait(100,othestvo.ravno()),
othestvo.ravno(),
dpnr,
dusr);

fprintf(ff_tab5_xml,"\
<ROW>\n\
 <ROWNUM>%d</ROWNUM>\n",nomer_str_tab5);

fprintf(ff_tab5_xml,"\
 <UKR_GROMAD>%d</UKR_GROMAD>\n",metka_ukr_grom);

if(inn[0] == '\0')
 fprintf(ff_tab5_xml,"\
 <NUMIDENT xsi:nil=\"true\">%s</NUMIDENT>\n",inn);
else
 fprintf(ff_tab5_xml,"\
 <NUMIDENT>%s</NUMIDENT>\n",inn);

fprintf(ff_tab5_xml,"\
 <ZO>%d</ZO>\n",zo); /*категория работника*/

if(prizv.ravno()[0] != '\0')    
 fprintf(ff_tab5_xml,"\
 <LN>%s</LN>\n",iceb_u_filtr_xml(prizv.ravno()));
else
 fprintf(ff_tab5_xml,"\
 <LN xsi:nil=\"true\"></LN>\n");

if(ima.ravno()[0] != '\0')
 fprintf(ff_tab5_xml,"\
 <NM>%s</NM>\n",iceb_u_filtr_xml(ima.ravno()));
else
 fprintf(ff_tab5_xml,"\
 <NM xsi:nil=\"true\"></NM>\n");

if(othestvo.ravno()[0] != '\0')
 fprintf(ff_tab5_xml,"\
 <FTN>%s</FTN>\n",iceb_u_filtr_xml(othestvo.ravno()));
else
 fprintf(ff_tab5_xml,"\
 <FTN xsi:nil=\"true\"></FTN>\n");

/*дата выводится только если она не нулевая*/
if(dpnr != 0)
 fprintf(ff_tab5_xml,"\
 <START_DT>%d</START_DT>\n",dpnr);

if(dusr != 0)
 fprintf(ff_tab5_xml,"\
 <END_DT>%d</END_DT>\n",dusr);      

fprintf(ff_tab5_xml,"\
</ROW>\n");

}

/******************************************/
/*Таблица 5 принятые на работу и уволенные*/
/*Возвращает количество человек в таблице*/
/******************************************/
int zarpensm1_t5(short mr,short gr,
const char *inn_or,
const char *kodpfu,
const char *imaf_tab5,
const char *imaf_tab5_xml,
const char *imaf_tab5_dbf,
const char *imaf_tab5_dbf_tmp,
const char *kodzvna,
FILE *ff_prot,
GtkWidget *wpredok)
{
int nomer_str_tab5=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
int metka_ukr_grom=1;

char strsql[512];
FILE *ff_tab5;
FILE *ff_tab5_xml;
FILE *ff_tab5_dbf;



sprintf(strsql,"select tabn,datn,datk from Zarn where god=%d and mes=%d and ((datn >= '%04d-%02d-01' and datn <= '%04d-%02d-31') \
or (datk >= '%04d-%02d-01' and datk <= '%04d-%02d-31'))",gr,mr,gr,mr,gr,mr,gr,mr,gr,mr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдено ни одного принятого на роботу или уволенного в записях месячных настроек\n");
 }




if((ff_tab5 = fopen(imaf_tab5,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tab5,"",errno,wpredok);
  return(0);
 }

if((ff_tab5_xml = fopen(imaf_tab5_xml,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tab5_xml,"",errno,wpredok);
  return(0);
 }

if((ff_tab5_dbf = fopen(imaf_tab5_dbf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tab5_dbf_tmp,"",errno,wpredok);
  return(0);
 }

fprintf(ff_tab5,"\
Вихідний No у страхувальника__________ No аркушу звіту___________________\n");
fprintf(ff_tab5,"\n\
         Таблиця 5. Трудові відносини застрахованих осіб\n\n");
fprintf(ff_tab5,"\
Код за ЄДРПОУ/ %-10s        Філія за реєстрацією %s\n\
індефікаційний номер        в органі Пенсійного фонду\n\n",inn_or,kodpfu);
fprintf(ff_tab5,"Назва страхувальника:%s\n",organ);
fprintf(ff_tab5,"Звітний місяць:%d    рік:%d           Тип:\n",mr,gr);

fprintf(ff_tab5,"\
--------------------------------------------------------------------------\n");
fprintf(ff_tab5,"\
  N  |Ном.обл.к.|       Призвище, ім'я, по батькові      |Трудові віднос.|\n\
     |          |                                        |Дата п.|Дата к.|\n");

fprintf(ff_tab5,"\
--------------------------------------------------------------------------\n");

sprintf(strsql,"E04T05%s",version_dok1);
zarpensm1_xml_sap(mr,gr,kodpfu,strsql,"T05",ff_tab5_xml,wpredok);
short dpnr=0;
short dusr=0;
short d,m,g;
class iceb_u_str inn("");
class iceb_u_str fio("");
class iceb_u_int sp_tabn;
int zo=1; /*категория работника 1-трудовая книжка на предприятии 2-совместитель 3-договора подряда 4-уход за детьми 5 беременность и роды*/

while(cur.read_cursor(&row) != 0)
 {
  dpnr=dusr=0;
  fio.new_plus("");
  inn.new_plus("");
  sprintf(strsql,"select fio,inn,zvan,sovm from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Не найден табельный номер"),row[0]);
    iceb_menu_soob(strsql,wpredok);
    continue;
   }

  if(iceb_u_proverka(kodzvna,row1[2],0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s %s Атестованный - в отчёт не входит!\n",row[0],row1[0]);
    continue;
   }
  zo=1; /*категория работника 1-трудовая книжка на предприятии 2-совместитель 3-договора подряда 4-уход за детьми 5 беременность и роды*/
  if(atoi(row1[3]) == 1) /*нет трудовой книжки*/
   zo=2;
  if(zarprtnw(mr, gr,atoi(row[0]),"zardog.alx",NULL,wpredok) == 1)
    zo=3;

  fio.new_plus(row1[0]);

  inn.new_plus(row1[1]);
     
  if(row[1][0] != '0')
   {
    iceb_u_rsdat(&d,&m,&g,row[1],2);  

    if(iceb_u_sravmydat(1,m,g,1,mr,gr) == 0)
     dpnr=d;
   }
  if(row[2][0] != '0')
   {
    iceb_u_rsdat(&d,&m,&g,row[2],2);  

    if(iceb_u_sravmydat(1,m,g,1,mr,gr) == 0)
     dusr=d;
   }
 
  sp_tabn.plus(row[0]);
  nomer_str_tab5++;
  zarpensm1_str_tab5(nomer_str_tab5,inn.ravno(),fio.ravno(),dpnr,dusr,mr,gr,metka_ukr_grom,zo,ff_tab5,ff_tab5_dbf,ff_tab5_xml);

 }
/*Если в карточке небыло начислений то записи с настройкой не будет. Смотрим в карточках*/

sprintf(strsql,"select tabn,fio,datn,datk,inn,zvan,sovm from Kartb where ((datn >= '%04d-%02d-01' and datn <= '%04d-%02d-31') \
or (datk >= '%04d-%02d-01' and datk <= '%04d-%02d-31'))",gr,mr,gr,mr,gr,mr,gr,mr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдено ни одного принятого на роботу или уволенного в записях карточек\n");
 }

while(cur.read_cursor(&row) != 0)
 {
  if(sp_tabn.find(row[0]) >= 0)
    continue;
  if(iceb_u_proverka(kodzvna,row[5],0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s %s Атестованный - в отчёт не входит!\n",row[0],row[1]);
    continue;
   }
  zo=1; /*категория работника 1-трудовая книжка на предприятии 2-совместитель 3-договора подряда 4-уход за детьми 5 беременность и роды*/
  if(atoi(row[6]) == 1) /*нет трудовой книжки*/
   zo=2;
  if(zarprtnw(mr, gr,atoi(row[0]),"zardog.alx",NULL,wpredok) == 1)
    zo=3;

  if(row[2][0] != '0')
   iceb_u_rsdat(&dpnr,&m,&g,row[2],2);  
  if(row[3][0] != '0')
   iceb_u_rsdat(&dusr,&m,&g,row[3],2);  
  
  nomer_str_tab5++;
  zarpensm1_str_tab5(nomer_str_tab5,row[4],row[1],dpnr,dusr,mr,gr,metka_ukr_grom,zo,ff_tab5,ff_tab5_dbf,ff_tab5_xml);

 }

class iceb_fioruk_rk kerivnik;
class iceb_fioruk_rk glavbuh;
iceb_fioruk(1,&kerivnik,wpredok);
iceb_fioruk(2,&glavbuh,wpredok);


fprintf(ff_tab5,"\
--------------------------------------------------------------------------\n");
zarpensm1_kon_lst(nomer_str_tab5,kerivnik.inn.ravno(),kerivnik.fio.ravno(),glavbuh.inn.ravno(),glavbuh.fio.ravno(),ff_tab5);


fprintf(ff_tab5_xml,"\
   </TABLE>\n\
   <FOOTER>\n\
    <ROWS>%d</ROWS>\n\
    <BOSS_NUMIDENT>%s</BOSS_NUMIDENT>\n\
    <FIRM_BOSS>%s %s</FIRM_BOSS>\n",
nomer_str_tab5,
kerivnik.inn.ravno(),
iceb_u_filtr_xml(kerivnik.famil.ravno()),
kerivnik.inic);

fprintf(ff_tab5_xml,"\
    <BUH_NUMIDENT>%s</BUH_NUMIDENT>\n\
    <FIRM_BUH>%s %s</FIRM_BUH>\n\
   </FOOTER>\n\
  </PAGE>\n\
 </DECLARBODY>\n\
</DECLAR>\n",
glavbuh.inn.ravno(),
iceb_u_filtr_xml(glavbuh.famil.ravno()),
glavbuh.inic);

iceb_podpis(ff_tab5,wpredok);

fclose(ff_tab5);
fclose(ff_tab5_xml);

fputc(26, ff_tab5_dbf);
fclose(ff_tab5_dbf);
iceb_perecod(2,imaf_tab5_dbf_tmp,wpredok);

zarpensm1_tab5_dbf(imaf_tab5_dbf,nomer_str_tab5,wpredok);
iceb_cat(imaf_tab5_dbf,imaf_tab5_dbf_tmp,wpredok);
unlink(imaf_tab5_dbf_tmp);

if(nomer_str_tab5 == 0)
 {
  unlink(imaf_tab5);
  unlink(imaf_tab5_dbf);
  unlink(imaf_tab5_xml);
  
 }

return(nomer_str_tab5);

}


/******************************************/
/******************************************/

gint zarpensm1w_r1(class zarpensm1w_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1;
SQLCURSOR cur,cur1;
int kolstr=0;
class iceb_u_str inn_or("");
const char *imaf_tab5_dbf={"E04T05D.dbf"};
const char *imaf_tab6_dbf={"E04T06D.dbf"};
const char *imaf_tab7_dbf={"E04T07D.dbf"};
const char *imaf_tab5_dbf_tmp={"E04T05D.tmp"};
const char *imaf_tab6_dbf_tmp={"E04T06D.tmp"};
const char *imaf_tab7_dbf_tmp={"E04T07D.tmp"};
char imaf_pfz[64];
class iceb_u_str inn("");
int metka_ukr_grom=1;




short mr,gr;
if(iceb_u_rsdat1(&mr,&gr,data->rk->datar.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


class iceb_u_spisok stab6; /*код типа начисления|месяц.год*/
class iceb_u_double tab6_sumao;
class iceb_u_double tab6_sumavr;
class iceb_u_double tab6_sumaesv;

class iceb_u_spisok data_boln; /*Дата больничного*/
class iceb_u_double suma_data_boln; /*сумма больничного по датам*/
class iceb_u_double suma_data_boln_esv; /*сумма единого социального взноса с больничного по датам*/

class zarpensm1_nastr nastr;

zarpensm1_read_nast(&nastr,data->window); /*Читаем настройки*/

/*чтение величины минимальной зарплаты и прожиточного минимума*/
zar_read_tnw(1,mr,gr,NULL,data->window); 

/*Чтение настроек для расчёта единого социального взноса*/
zarrnesvw(NULL,data->window);

sprintf(strsql,"select distinct tabn from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and prn='1' and suma <> 0.",
gr,mr,1,gr,mr,31);

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

if(iceb_sql_readkey("select kod from Kontragent where kodkon='00'",&row1,&cur1,data->window) == 1)
 {
  inn_or.new_plus(row1[0]);
 } 

char imaf_tab5[64];
sprintf(imaf_tab5,"E04T05_%d.lst",getpid());
char imaf_tab5_xml[64];
sprintf(imaf_tab5_xml,"E04T05%s.xml",version_dok1);

char imaf_tab6[64];
FILE *ff_tab6;

sprintf(imaf_tab6,"E04T06_%d.lst",getpid());
if((ff_tab6 = fopen(imaf_tab6,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tab6,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_tab7[64];
FILE *ff_tab7;

sprintf(imaf_tab7,"E04T07_%d.lst",getpid());
if((ff_tab7 = fopen(imaf_tab7,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tab7,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_tab6_xml[64];
FILE *ff_tab6_xml;
sprintf(imaf_tab6_xml,"E04T06%s.xml",version_dok1);
if((ff_tab6_xml = fopen(imaf_tab6_xml,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tab6_xml,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
FILE *ff_tab6_dbf;
if((ff_tab6_dbf = fopen(imaf_tab6_dbf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tab6_dbf_tmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_tab7_xml[64];
FILE *ff_tab7_xml;
sprintf(imaf_tab7_xml,"E04T07%s.xml",version_dok1);
if((ff_tab7_xml = fopen(imaf_tab7_xml,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tab7_xml,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
FILE *ff_tab7_dbf;
if((ff_tab7_dbf = fopen(imaf_tab7_dbf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tab7_dbf_tmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf_prot[64];
sprintf(imaf_prot,"prot%d.lst",getpid());
FILE *ff_prot;
if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_prot,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(ff_prot != NULL)
 {
  fprintf(ff_prot,"Коды не входящие в расчёт единого социального взноса:%s\n",knvr_esv_r.ravno());

  fprintf(ff_prot,"Коды начислений для кода типа начисления 1:%s\n",nastr.ktn1.ravno());
  fprintf(ff_prot,"Коды начислений для кода типа начисления 2:%s\n",nastr.ktn2.ravno());
  fprintf(ff_prot,"Коды начислений для кода типа начисления 3:%s\n",nastr.ktn3.ravno());
  fprintf(ff_prot,"Коды начислений для кода типа начисления 4:%s\n",nastr.ktn4.ravno());
  fprintf(ff_prot,"Коды начислений для кода типа начисления 5:%s\n",nastr.ktn5.ravno());
  fprintf(ff_prot,"Коды званий неатестованых:%s\n",nastr.kodzvna.ravno());
  fprintf(ff_prot,"Коды больничных:");

  if(kodbl != NULL)
    for(int nom=1; nom <= kodbl[0]; nom++)
      fprintf(ff_prot,"%d ",kodbl[nom]);
 }

short dkm=1;
iceb_u_dpm(&dkm,&mr,&gr,5);

 



fprintf(ff_tab6,"\
Вихідний No у страхувальника__________ No аркушу звіту___________________\n");
fprintf(ff_tab6,"\n\
Таблиця 6. Відомості про нарахування заробітку (доходу) застрахованим особам\n\n");
fprintf(ff_tab6,"\
Код за ЄДРПОУ/ %-10s        Філія за реєстрацією %s\n\
індефікаційний номер        в органі Пенсійного фонду\n\n",
inn_or.ravno(),nastr.kodpfu.ravno());
fprintf(ff_tab6,"Назва страхувальника:%s\n",organ);
fprintf(ff_tab6,"Звітний місяць:%d    рік:%d           Тип:\n",mr,gr);

fprintf(ff_tab6,"\
---------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_tab6,"\
  N  |Г|С|Номер облі|   Призвище та ініціали ЗО    |Код |Тип |Місяць |Сума нарах|Сума нарах|Сума еди|Озна|Озна|\n\
     |р|т|кової карт|                              |кате|нара|та рік |увань заро|увань заро|ного вне|ка  |ка  |\n\
     |о|а|ки застра-|                              |грії|хува|за який|бітку/дохо|бітку/дохо|ску за  |наяв|наяв|\n\
     |м|т|хованої   |                              | ЗО |нь**|проведе|ду за відп|ду, в межа|звітинй |ност|ност|\n\
     |а|ь|особи (ЗО)|                              |    |    |но нара|овідний   |х максимал|місяць  |і тр|і сп|\n\
     |д| |за ДРФО   |                              |    |    |хування|місяць    |ьної велич|(із заро|удов|ецст|\n\
     |я| |   ДПА    |                              |    |    |       |          |ини на яку|бітной  |ої к|ажу |\n\
     |н| |          |                              |    |    |       |          |нараховуют|плати/до|нижк|(1-т|\n\
     |и| |          |                              |    |    |       |          |ься эдиний|ходу)   |и   |ак,0|\n\
     |н| |          |                              |    |    |       |          |внесок    |        |(1-т|-ні)|\n\
     | | |          |                              |    |    |       |          |          |        |ак,0|    |\n\
     | | |          |                              |    |    |       |          |          |        |-ні)|    |\n");
fprintf(ff_tab6,"\
---------------------------------------------------------------------------------------------------------------\n");





fprintf(ff_tab7,"\
Вихідний No у страхувальника___________________                    No аркушу звіту___________________\n");
fprintf(ff_tab7,"\n\
  Таблиця 7. Наявність підстав для обліку стажу окремим категоріям осіб відповідно до законодавства\n");

fprintf(ff_tab7,"\
Код за ЄДРПОУ/індефікаційний номер %-10s   Філія за реєстрацією в органі Пенсійного фонду %s\n",
inn_or.ravno(),nastr.kodpfu.ravno());
fprintf(ff_tab7,"Назва страхувальника:%s\n",organ);
fprintf(ff_tab7,"Звітний місяць:%d    рік:%d           Тип:\n",mr,gr);
fprintf(ff_tab7,"\
----------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_tab7,"\
  N  |Г|Номер облі|Код підста|   Призвище та ініціали ЗО    |Поч|Кін|Варіант|Кільк|Кількі|Кільк|Норма|N нак|Дата наказ|Озна|\n\
     |р|кової карт|ви для обл|                              |ато|нец|необхід|ість |сть но|ість |трива|азу п|у про пров|ка  |\n\
     |о|ки застра-|іку спецст|                              |к п|ь п|ної оди|місяц|рмо-зм|годин|лості|ро пр|едення ате|СЕЗО|\n\
     |м|хованої   |ажу       |                              |ері|ері|ниці ви|ів/дн|ін    |-хвил|робот|оведе|стації роб| Н  |\n\
     |а|особи (ЗО)|          |                              |оду|оду|міру та|ів   |      |ин   |и для|ння а|очого місц|    |\n\
     |д|за ДРФО   |          |                              |   |   |фактичн|     |      |     |її за|теста|я         |    |\n\
     |я|   ДПА    |          |                              |   |   |а трива|     |      |     |рахув|ції р|          |    |\n\
     |н|          |          |                              |   |   |лість с|     |      |     |ання |обочо|          |    |\n\
     |и|          |          |                              |   |   |пецстаж|     |      |     |за по|го мі|          |    |\n\
     |н|          |          |                              |   |   |у за ко|     |      |     |вний |сця  |          |    |\n\
     | |          |          |                              |   |   |дом під|     |      |     |місяц|     |          |    |\n\
     | |          |          |                              |   |   |стави  |     |      |     |ь    |     |          |    |\n");
fprintf(ff_tab7,"\
----------------------------------------------------------------------------------------------------------------------------\n");



sprintf(strsql,"E04T06%s",version_dok1);
zarpensm1_xml_sap(mr,gr,nastr.kodpfu.ravno(),strsql,"T06",ff_tab6_xml,data->window);

sprintf(strsql,"E04T07%s",version_dok1);
zarpensm1_xml_sap(mr,gr,nastr.kodpfu.ravno(),strsql,"T07",ff_tab7_xml,data->window);

double suma_nah=0.;   

double suma=0.,isuma=0.;
double suma_esv_v=0.,isuma_esv_v=0.; /*Cумма единого социального взноса не больничного*/
double suma_esv_v_bol=0.; /*Cумма единого социального взноса больничного*/

int nomer_str_tab6=0;
double suma_obh=0.;
double isuma_obh=0.;
char metka_pola[8]; /*чоловік Ч жінка Ж*/
int metka_otk=1; /*метка наличия трудовой книжки 0-нет трудовой 1-есть*/
int metka_exp=0; /*метка наличия спецстажа 0-нет 1-есть*/
int nomer_vsp=0;
int nomer_str_tab7=0;
int kodtipnah=0; /*Код типа начисления*/
short dpnr=0; /*День приёма на работу*/
short dusr=0; /*День увольнения с работы*/
int nomer_str_tab5=0;
char prizv[112];
char ima[112];
char othestvo[112];
int kodzvna=0;
char kodss[64];
class iceb_u_str data_prik("");
class iceb_u_str nomer_prik("");
short dn=1;
short dk=1;
iceb_u_dpm(&dk,&mr,&gr,5);
short koldn=dk-dn+1;
float kolstr1=0.;
class iceb_u_str fio("");

class zar_read_tn1h nastr_mz;
zar_read_tn1w(1,mr,gr,&nastr_mz,ff_prot,data->window);

while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
   continue;

  
  sprintf(strsql,"select knah,suma,godn,mesn from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and tabn=%s and prn='1' and suma <> 0.",
  gr,mr,1,gr,mr,31,row[0]);

  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"\n\nТабельный номер %s\n",row[0]);
    fprintf(ff_prot,"%s\n",strsql);
   }

  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  suma_obh=0.;
  suma=0.;
  stab6.free_class();
  tab6_sumao.free_class();  
  tab6_sumavr.free_class();  
  tab6_sumaesv.free_class();  

  data_boln.free_class();
  suma_data_boln.free_class();
  suma_data_boln_esv.free_class();
  while(cur1.read_cursor(&row1) != 0)
   {
    if(ff_prot != NULL)
      fprintf(ff_prot,"%s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
    if(iceb_u_proverka(knvr_esv_r.ravno(),row1[0],0,1) == 0)
     {
      if(ff_prot != NULL)
        fprintf(ff_prot,"Не входит в расчёт\n");
      continue;
     }    

    suma_nah=atof(row1[1]);   
    kodtipnah=0;
    
    if(provkodw(kodbl,row1[0]) >= 0) /*больничные*/
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"Больничный\n");
       
      sprintf(strsql,"%s.%s",row1[3],row1[2]);
      
      if((nomer_vsp=data_boln.find(strsql)) < 0)
       data_boln.plus(strsql);
      suma_data_boln.plus(suma_nah,nomer_vsp);
      suma_data_boln_esv.plus(0.,nomer_vsp);
      continue;      
     }
    if(iceb_u_proverka(nastr.ktn1.ravno(),row1[0],0,1) == 0)
     {
      kodtipnah=1;
      if(ff_prot != NULL)
        fprintf(ff_prot,"Подходит для кода типа начисления 1\n");

     }

    if(iceb_u_proverka(nastr.ktn2.ravno(),row1[0],0,1) == 0)
     {
      kodtipnah=2;
      if(ff_prot != NULL)
       fprintf(ff_prot,"Подходит для кода типа начисления 2\n");
     }

    if(iceb_u_proverka(nastr.ktn3.ravno(),row1[0],0,1) == 0)
     {
      kodtipnah=3;
      if(ff_prot != NULL)
       fprintf(ff_prot,"Подходит для кода типа начисления 3\n");
     }

    if(iceb_u_proverka(nastr.ktn4.ravno(),row1[0],0,1) == 0)
     {
      kodtipnah=4;
      if(ff_prot != NULL)
       fprintf(ff_prot,"Подходит для кода типа начисления 4\n");
     }

    if(iceb_u_proverka(nastr.ktn5.ravno(),row1[0],0,1) == 0)
     {
      kodtipnah=5;
      if(ff_prot != NULL)
       fprintf(ff_prot,"Подходит для кода типа начисления 5\n");

     }




    sprintf(strsql,"%d|%s.%s",kodtipnah,row1[3],row1[2]);

    if((nomer_vsp=stab6.find(strsql)) < 0)
     stab6.plus(strsql);

    tab6_sumao.plus(suma_nah,nomer_vsp);

    tab6_sumavr.plus(0.,nomer_vsp);

    tab6_sumaesv.plus(0.,nomer_vsp);








   }



  sprintf(strsql,"select knah,suma from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and tabn=%s and prn='2'and suma <> 0.",
  gr,mr,1,gr,mr,31,row[0]);

  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  int kod_zap=0;
  suma_esv_v=suma_esv_v_bol=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s %s\n",row1[0],row1[1]);    

    kod_zap=atoi(row1[0]);        

    if(kod_zap == kod_esv)
       suma_esv_v+=atof(row1[1])*-1;   

    if(kod_zap == kod_esv_bol)
       suma_esv_v_bol+=atof(row1[1])*-1;   

    if(kod_zap == kod_esv_inv)
       suma_esv_v+=atof(row1[1])*-1;   

    if(kod_zap == kod_esv_dog)
       suma_esv_v+=atof(row1[1])*-1;   
    if(ff_prot != NULL)
     fprintf(ff_prot,"suma_esv_v=%.2f\n",suma_esv_v);     
   }

  if(ff_prot != NULL)
   fprintf(ff_prot,"Единый соц.взнос = %.2f\n\
Единый соц.взнос с больничного=%.2f\n",suma_esv_v,suma_esv_v_bol);
   
  if(suma_esv_v+suma_esv_v_bol == 0.)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Нет единой социальной выплаты!\n");
    continue;
   }
  double suma_vr_tab=0.;
  int kolih_nah=stab6.kolih();
  /*Определяем общюю сумму взятую в расчёт*/
  for(int nom=0; nom < kolih_nah; nom++)
   {  
    suma=tab6_sumao.ravno(nom);
    if(suma > nastr_mz.max_sum_for_soc)
     suma=nastr_mz.max_sum_for_soc;
    suma_vr_tab+=suma;
    tab6_sumavr.plus(suma,nom);
   }

  /*разбираемся с небольничными начилениями*/  
  suma=tab6_sumavr.suma();
  double proc_per=0.;
  double suma_per=0.;
  for(int nom=0; nom < kolih_nah; nom++)
   {
    suma=tab6_sumao.ravno(nom);

    proc_per=tab6_sumavr.ravno(nom)*100./suma_vr_tab;

    suma_per=suma_esv_v*proc_per/100.;
    tab6_sumaesv.plus(suma_per,nom);
    if(ff_prot != NULL)
     {
      
      fprintf(ff_prot,"Процент = %.2f*100./%.2f=%.2f\n",tab6_sumavr.ravno(nom),suma_vr_tab,proc_per);
      fprintf(ff_prot,"Расчёт = %.2f*%.2f/100.=%.2f\n",suma_esv_v,proc_per,suma_per);
 
     }
   }

  /*разбираемся с больничными начислениями*/
  kolih_nah=data_boln.kolih();
  suma=suma_data_boln.suma();
  for(int nom=0; nom < kolih_nah; nom++)
   {
    proc_per=suma_data_boln.ravno(nom)*100./suma;
    suma_per=suma_esv_v_bol*proc_per/100.;
    suma_data_boln_esv.plus(suma_per,nom);
   }


  
  fio.new_plus("");
  inn.new_plus("");
  memset(prizv,'\0',sizeof(prizv));
  memset(ima,'\0',sizeof(ima));
  memset(othestvo,'\0',sizeof(othestvo));
    
  metka_otk=1;  
  
  dpnr=dusr=0;
  kodzvna=0;  
  memset(kodss,'\0',sizeof(kodss));
  memset(metka_pola,'\0',sizeof(metka_pola));
  sprintf(strsql,"select fio,datn,datk,sovm,inn,zvan,kss,pl from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    fio.new_plus(row1[0]);
    inn.new_plus(row1[4]);
    if(atoi(row1[3]) == 1)
     metka_otk=0;
    kodzvna=atoi(row1[5]);

    if(iceb_u_pole3(row1[0],prizv,1,' ') != 0)
     strncpy(prizv,row1[0],sizeof(prizv)-1);

    iceb_u_pole3(row1[0],ima,2,' ');
    iceb_u_pole3(row1[0],othestvo,3,' ');

    if(iceb_u_polen(row1[6],kodss,sizeof(kodss),1,' ') != 0)
      strncpy(kodss,row1[6],sizeof(kodss)-1);
    iceb_u_polen(row1[6],&data_prik,2,' ');
    iceb_u_polen(row1[6],&nomer_prik,3,' ');
    
    
    if(atoi(row1[7]) == 0)
     strcpy(metka_pola,"Ч");
    else
     strcpy(metka_pola,"Ж");     
   }
  else
   {
     if(ff_prot != NULL)
      fprintf(ff_prot,"Не найдена карточка работника %d!\n",atoi(row[0]));
    continue;
   }
  
  iceb_printw(fio.ravno_ps(),data->buffer,data->view);

  if(iceb_u_proverka(nastr.kodzvna.ravno(),kodzvna,0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s Атестованный - в отчёт не входит!\n",fio.ravno());
    continue;
   }


  if(inn.getdlinna() <= 1)
   {
    sprintf(strsql,"%s %s\n%s",row[0],fio.ravno(),gettext("Не введён идентификационный номер"));
    iceb_menu_soob(strsql,data->window);
   }

  metka_exp=0;
  if(kodss[0] != '\0')
   metka_exp=1;   

  /*не больничные начисления*/
  for(int nom=0; nom < stab6.kolih(); nom++)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"tab6_sumao[%d]=%.2f\n\
tab6_sumavr[%d]=%.2f\n\
tab6_sumaesv[%d]=%.2f\n",
     nom,tab6_sumao.ravno(nom),
     nom,tab6_sumavr.ravno(nom),
     nom,tab6_sumaesv.ravno(nom));

    isuma_obh+=suma_obh=tab6_sumao.ravno(nom);
     
    isuma+=suma=tab6_sumavr.ravno(nom);
    isuma_esv_v+=suma_esv_v=tab6_sumaesv.ravno(nom);
    char data_zap[16];
    iceb_u_polen(stab6.ravno(nom),&kodtipnah,1,'|');      
    iceb_u_polen(stab6.ravno(nom),data_zap,sizeof(data_zap),2,'|');      
        
    zarpensm1_str_tab6(++nomer_str_tab6,mr,gr,inn.ravno(),fio.ravno(),metka_ukr_grom,kodtipnah,metka_pola,data_zap,suma_obh,suma,suma_esv_v,metka_otk,metka_exp,row[0],ff_tab6,ff_tab6_dbf,ff_tab6_xml);

   }
  
  /*больничные начисления*/
  for(int nom=0; nom < data_boln.kolih(); nom++)
   {
    isuma_obh+=suma_obh=suma_data_boln.ravno(nom);
    isuma+=suma=suma_data_boln.ravno(nom);
    isuma_esv_v+=suma_esv_v=suma_data_boln_esv.ravno(nom);
    kodtipnah=0;
//    zo=29;
    zarpensm1_str_tab6(++nomer_str_tab6,mr,gr,inn.ravno(),fio.ravno(),metka_ukr_grom,kodtipnah,metka_pola,data_boln.ravno(nom),suma_obh,suma,suma_esv_v,metka_otk,metka_exp,row[0],ff_tab6,ff_tab6_dbf,ff_tab6_xml);

   }
   

  if(ff_prot != NULL)
   fprintf(ff_prot,"Код спецстажа=%s %s %s\n",kodss,data_prik.ravno(),nomer_prik.ravno());
   
  if(kodss[0] != '\0') /*Таблица 7*/
   {

    nomer_str_tab7++;
    short dprik=0,mprik=0,gprik=0;
    iceb_u_rsdat(&dprik,&mprik,&gprik,data_prik.ravno(),1);


    fprintf(ff_tab7,"\
%5d|%d|%-10s|%-10s|%*.*s|%3d|%3d|       |     |      |     |     |%-*s|%02d.%02d.%04d|    |%s\n",
    nomer_str_tab7,
    metka_ukr_grom,
    inn.ravno(),kodss,
    iceb_u_kolbait(30,fio.ravno()),
    iceb_u_kolbait(30,fio.ravno()),
    fio.ravno(),
    dn,
    dk,
    iceb_u_kolbait(5,nomer_prik.ravno()),
    nomer_prik.ravno(),
    dprik,mprik,gprik,    
    row[0]);

    fprintf(ff_tab7_dbf," %02d%04d%06d%1d%-10s%-*.*s%-*.*s%-*.*s%-*.*s%02d%02d%04d%04d%04d%02d%06d%-8s%08d%d",
    mr,gr,nomer_str_tab7,metka_ukr_grom,inn.ravno(),
    iceb_u_kolbait(100,prizv),
    iceb_u_kolbait(100,prizv),
    prizv,    
    iceb_u_kolbait(100,ima),
    iceb_u_kolbait(100,ima),
    ima,    
    iceb_u_kolbait(100,othestvo),
    iceb_u_kolbait(100,othestvo),
    othestvo,
    iceb_u_kolbait(9,kodss),
    iceb_u_kolbait(9,kodss),
    kodss,
    0,0,0,0,0,0,0,
    "wwww",
    0,
    0);


    fprintf(ff_tab7_xml,"\
    <ROW>\n\
     <ROWNUM>%d</ROWNUM>\n",nomer_str_tab7);
    fprintf(ff_tab7_xml,"\
     <UKR_GROMAD>%d</UKR_GROMAD>\n",metka_ukr_grom);

    if(inn.getdlinna() <= 1)
     fprintf(ff_tab7_xml,"\
     <NUMIDENT xsi:nil=\"true\">%s</NUMIDENT>\n",inn.ravno());
    else
     fprintf(ff_tab7_xml,"\
     <NUMIDENT>%s</NUMIDENT>\n",inn.ravno());

    if(prizv[0] != '\0')    
     fprintf(ff_tab7_xml,"\
     <LN>%s</LN>\n",iceb_u_filtr_xml(prizv));
    else
     fprintf(ff_tab7_xml,"\
     <LN xsi:nil=\"true\"></LN>\n");
    
    if(ima[0] != '\0')
     fprintf(ff_tab7_xml,"\
     <NM>%s</NM>\n",iceb_u_filtr_xml(ima));
    else
     fprintf(ff_tab7_xml,"\
     <NM xsi:nil=\"true\"></NM>\n");
    
    if(othestvo[0] != '\0')
     fprintf(ff_tab7_xml,"\
     <FTN>%s</FTN>\n",iceb_u_filtr_xml(othestvo));
    else
     fprintf(ff_tab7_xml,"\
     <FTN xsi:nil=\"true\"></FTN>\n");


    fprintf(ff_tab7_xml,"\
     <C_PID>%s</C_PID>\n\
     <START_DT>%d</START_DT>\n\
     <STOP_DT>%d</STOP_DT>\n\
     <DAYS>%d</DAYS>\n\
     <NORMZ>%d</NORMZ>\n\
     <HH>%d</HH>\n\
     <MM>%d</MM>\n\
     <NORMA>%d</NORMA>\n\
     <SEASON>%d</SEASON>\n",
     kodss,
     dn,
     dk,
     koldn,
     0,
     0,
     0,
     koldn,
     0);

    if(nomer_prik.getdlinna() > 1)
     fprintf(ff_tab7_xml,"\
     <NUM_NAK>%s</NUM_NAK>\n",nomer_prik.ravno());
    if(dprik > 0)
     fprintf(ff_tab7_xml,"\
     <DT_NAK>%02d%02d%04d</DT_NAK>\n",dprik,mprik,gprik);

    fprintf(ff_tab7_xml,"\
    </ROW>\n");
     

   }
    
 }
class iceb_fioruk_rk kerivnik;
class iceb_fioruk_rk glavbuh;
iceb_fioruk(1,&kerivnik,data->window);
iceb_fioruk(2,&glavbuh,data->window);


fprintf(ff_tab6,"\
----------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_tab6,"%*s:%10.2f %10.2f %8.2f\n",
iceb_u_kolbait(69,gettext("Итого")),
gettext("Итого"),
isuma_obh,isuma,isuma_esv_v);


zarpensm1_kon_lst(nomer_str_tab6,kerivnik.inn.ravno(),kerivnik.fio.ravno(),glavbuh.inn.ravno(),glavbuh.fio.ravno(),ff_tab6);

fprintf(ff_tab7,"\
----------------------------------------------------------------------------------------------------------------------------\n");

zarpensm1_kon_lst(nomer_str_tab7,kerivnik.inn.ravno(),kerivnik.fio.ravno(),glavbuh.inn.ravno(),glavbuh.fio.ravno(),ff_tab7);



fprintf(ff_tab6_xml,"\
   </TABLE>\n\
   <FOOTER>\n\
    <PAGE_SUM_TOTAL>%.2f</PAGE_SUM_TOTAL>\n\
    <PAGE_SUM_MAX>%.2f</PAGE_SUM_MAX>\n\
    <PAGE_SUM_INS>%.2f</PAGE_SUM_INS>\n\
    <ROWS>%d</ROWS>\n\
    <BOSS_NUMIDENT>%s</BOSS_NUMIDENT>\n\
    <FIRM_BOSS>%s %s</FIRM_BOSS>\n",
isuma_obh,
isuma,
isuma_esv_v,
nomer_str_tab6,
kerivnik.inn.ravno(),
kerivnik.famil.ravno_filtr_xml(),
kerivnik.inic);

fprintf(ff_tab6_xml,"\
    <BUH_NUMIDENT>%s</BUH_NUMIDENT>\n\
    <FIRM_BUH>%s %s</FIRM_BUH>\n\
   </FOOTER>\n\
  </PAGE>\n\
 </DECLARBODY>\n\
</DECLAR>\n",
glavbuh.inn.ravno(),
glavbuh.famil.ravno_filtr_xml(),
glavbuh.inic);


fprintf(ff_tab7_xml,"\
    <ROWS>%d</ROWS>\n\
   </TABLE>\n\
   <FOOTER>\n\
    <BOSS_NUMIDENT>%s</BOSS_NUMIDENT>\n\
    <FIRM_BOSS>%s %s</FIRM_BOSS>\n",
nomer_str_tab7,
kerivnik.inn.ravno(),
iceb_u_filtr_xml(kerivnik.famil.ravno()),
kerivnik.inic);

fprintf(ff_tab7_xml,"\
    <BUH_NUMIDENT>%s</BUH_NUMIDENT>\n\
    <FIRM_BUH>%s %s</FIRM_BUH>\n", 
glavbuh.inn.ravno(),
iceb_u_filtr_xml(glavbuh.famil.ravno()),
glavbuh.inic);

fprintf(ff_tab7_xml,"\
   </FOOTER>\n\
  </PAGE>\n\
 </DECLARBODY>\n\
</DECLAR>\n");

iceb_podpis(ff_tab6,data->window);
iceb_podpis(ff_tab7,data->window);

fclose(ff_tab6);
fclose(ff_tab7);
fclose(ff_tab6_xml);
fclose(ff_tab7_xml);


fputc(26, ff_tab6_dbf);
fclose(ff_tab6_dbf);

fputc(26, ff_tab7_dbf);
fclose(ff_tab7_dbf);

iceb_perecod(2,imaf_tab6_dbf_tmp,data->window);
iceb_perecod(2,imaf_tab7_dbf_tmp,data->window);

zarpensm1_tab6_dbf(imaf_tab6_dbf,nomer_str_tab6,data->window);
zarpensm1_tab7_dbf(imaf_tab7_dbf,nomer_str_tab7,data->window);

/*Сливаем два файла*/
iceb_cat(imaf_tab6_dbf,imaf_tab6_dbf_tmp,data->window);
iceb_cat(imaf_tab7_dbf,imaf_tab7_dbf_tmp,data->window);

unlink(imaf_tab6_dbf_tmp);
unlink(imaf_tab7_dbf_tmp);


time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

char kodedrp[16];
memset(kodedrp,'\0',sizeof(kodedrp));
strncpy(kodedrp,inn_or.ravno(),10);
for(int nom=0; nom < 10; nom++)
 if(kodedrp[nom] == '\0')
  kodedrp[nom]='Z';
char datadok[16];
sprintf(datadok,"%02d%02d%04d",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
char vremdok[16];
sprintf(vremdok,"%02d%02d%02d",bf->tm_hour,bf->tm_min,bf->tm_sec);

char kodfil[8];
memset(kodfil,'\0',sizeof(kodfil));
strncpy(kodfil,nastr.kodfil.ravno(),5);
for(int nom=0; nom < 5; nom++)
 if(kodfil[nom] == '\0')
  kodfil[nom]='Z';

sprintf(imaf_pfz,"%05d%10.10s%5.5s%8s%6sE04%s0.pfz",
nastr.kodpfu.ravno_atoi(),kodedrp,kodfil,datadok,vremdok,version_dok1);

char imaf_tmp[56];
sprintf(imaf_tmp,"zarpensm11%d.tmp",getpid());
nomer_str_tab5=zarpensm1_t5(mr,gr,inn_or.ravno(),nastr.kodpfu.ravno(),imaf_tab5,imaf_tab5_xml,imaf_tab5_dbf,imaf_tab5_dbf_tmp,nastr.kodzvna.ravno(),ff_prot,data->window);

if(ff_prot != NULL)
 fclose(ff_prot);

if(nomer_str_tab5 > 0)
 {
  iceb_cp(imaf_tab5_xml,imaf_tmp,0,data->window);
  iceb_perecod(2,imaf_tab5_xml,data->window);
  inn.new_plus("zip");
  inn.plus(" ",imaf_pfz);
  inn.plus(" ",imaf_tab5_xml);
  system(inn.ravno());
  rename(imaf_tmp,imaf_tab5_xml);
  
 }
if(nomer_str_tab6 > 0)
 {
  iceb_cp(imaf_tab6_xml,imaf_tmp,0,data->window);
  iceb_perecod(2,imaf_tab6_xml,data->window);
  inn.new_plus("zip");
  inn.plus(" ",imaf_pfz);
  inn.plus(" ",imaf_tab6_xml);
  system(inn.ravno());
  rename(imaf_tmp,imaf_tab6_xml);
 }
if(nomer_str_tab7 > 0)
 {
  iceb_cp(imaf_tab7_xml,imaf_tmp,0,data->window);
  iceb_perecod(2,imaf_tab7_xml,data->window);
  inn.new_plus("zip");
  inn.plus(" ",imaf_pfz);
  inn.plus(" ",imaf_tab7_xml);
  system(inn.ravno());
  rename(imaf_tmp,imaf_tab7_xml);
 }
 
if(nomer_str_tab5 > 0)
 {
  data->rk->imaf.plus(imaf_tab5);
  data->rk->naimf.plus("Таблиця 5");
 }
data->rk->imaf.plus(imaf_tab6);
data->rk->naimf.plus("Таблиця 6");

data->rk->imaf.plus(imaf_tab7);
data->rk->naimf.plus("Таблиця 7");



if(ff_prot != NULL)
 {
  data->rk->imaf.plus(imaf_prot);
  data->rk->naimf.plus("Протокол хода расчёта");
 }
for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),3,data->window);

if(nomer_str_tab5 > 0)
 {
  data->rk->imaf.plus(imaf_tab5_xml);
  data->rk->naimf.plus("Таблиця 5 в форматі xml");
 }

data->rk->imaf.plus(imaf_tab6_xml);
data->rk->naimf.plus("Таблиця 6 в форматі xml");


data->rk->imaf.plus(imaf_tab7_xml);
data->rk->naimf.plus("Таблиця 7 в форматі xml");

if(nomer_str_tab5 > 0)
 {
  data->rk->imaf.plus(imaf_tab5_dbf);
  data->rk->naimf.plus("Таблиця 5 в форматі dbf");
 }

data->rk->imaf.plus(imaf_tab6_dbf);
data->rk->naimf.plus("Таблиця 6 в форматі dbf");

data->rk->imaf.plus(imaf_tab7_dbf);
data->rk->naimf.plus("Таблиця 7 в форматі dbf");

data->rk->imaf.plus(imaf_pfz);
data->rk->naimf.plus("Архив файлов xml");


















gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
