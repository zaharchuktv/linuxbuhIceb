/*$Id: i_rest_k.c,v 1.88 2011-02-21 07:36:20 sasa Exp $*/
/*29.09.2009	13.09.2004	Белых А.И.	i_rest_k.c
Программа расчёта клиентов
*/
#define         DVERSIQ "10.11.2007"
#include <glib.h>
#include        <stdlib.h>
#include        <unistd.h>
#include        <pwd.h>
#include        <locale.h>
#include        <sys/stat.h>
//#include        <iceb_d_libdev.h>
#include        "i_rest.h"
#include        "rest_oth.h"
#include        "../buhg_g/icebr200.xpm"
#include        "../buhg_g/flag.xpm"


enum
 {
  RASHET,
  RAB_S_KAS,
  SPISOK_OPLAT,
  SALDO_PO_KASE,
  KOLIH_KLIENT,
  VVOD_SUM_NA_SHET,
  SLUG_VNES,
  SLUG_IZQ,
  DEN_ROG,
  VIHOD,
  KOLKNOP
 };

class i_rest_data
 {
  public:
  GtkWidget *label;
  GtkWidget *window;
  GtkWidget *hrift;
  int       knopka;
 };

gboolean i_rest_destroy(GtkWidget *widget,GdkEvent *event,int *knopka);
void   i_rest_knopka(GtkWidget *widget,class i_rest_data *data);
int glmenu(int,int);
void rest_k_vibor(int);
int restnast_k(void);
int vsnsk(void);
void l_spis_opl_z(int,GtkWidget*);
double ras_sal_kas(int kod_kas,GtkWidget *wpredok);
void vibor_sr(char *kod_kl);
void l_rest_k_kolih_klient();
int vv_kolih(class iceb_u_str *podr,class iceb_u_str *datan,class iceb_u_str *vremn,GtkWidget *wpredok);
void spdnr_r(GtkWidget*);
void ras_kl(const char *kod_kl);

class KASSA kasr;

extern char	*imabaz;
extern SQL_baza	bd;
extern char     *putnansi;
extern char     *host;
const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};
extern char	*organ;
int             nomer_kas=0;  //номер кассы
uid_t           kod_operatora=0;
extern iceb_u_str shrift_rek_raz;
extern class iceb_get_dev_data config_dev;
extern char	*parol;


int main(int argc,char **argv)
{
char		bros[512];
iceb_start(argc,argv);


//Читаем в настройках имя компъютера и имя порта
//читать нужно до появления первого меню.

//sprintf(strsql,"%s/config_dev.alx",putnansi);
if(iceb_get_dev("config_dev.alx") == 0)
 {
  //Соединение с сервером чтения карточек
  iceb_connect_dserver(config_dev.ima_komp.ravno(),config_dev.name_dev_card.ravno());
 }

printf("Имя компъютера=%s\n\
Имя порта=%s\n",config_dev.ima_komp.ravno(),config_dev.name_dev_card.ravno());



kasr.handle = -1;
/* прочитаем параметры кассового регистратора из файла настройки	*/
//char *cpath;
//cpath = new char[strlen(putnansi)+strlen(imabaz)+2];
//sprintf(cpath, "%s%s%s", putnansi,G_DIR_SEPARATOR_S,imabaz);
//if (iceb_getnkas(cpath, 1, &kasr) == 0) 
if(iceb_getnkas(1, &kasr) == 0) 
 {
   nomer_kas=kasr.nomer;
   printf("handle = %d; sklad = %d; nomer = %d; \n\
   host = %s; password = %s; port = %s; prod = %s;\n",
   kasr.handle, kasr.sklad, kasr.nomer, kasr.host, kasr.parol, kasr.port, kasr.prodavec.ravno());
   
 }

if(nomer_kas == 0)
 {
  iceb_u_str nomkas;
  nomkas.plus("");
  iceb_mous_klav(gettext("Введите номер кассы"),&nomkas,10,0,0,0,NULL);
  kasr.nomer=nomer_kas=nomkas.ravno_atoi();
  if(nomer_kas == 0)
   {
    sql_closebaz(&bd);
        
       
    return( 0 );
   }  
  
 }



if(iceb_nastsys() != 0)
 iceb_exit(1);

if(restnast_k() != 0)
 iceb_exit(1);

char spis_login[512];
memset(spis_login,'\0',sizeof(spis_login));
//class iceb_u_str imaf_nast;
//iceb_imafn("restnast.alx",&imaf_nast);
iceb_poldan("Список логинов, которым разрешён внос/изъятие денег в кассу",spis_login,"restnast.alx",NULL);
int metka_dostupa=0;
struct  passwd  *ktoz; /*Кто записал*/
  
if(config_dev.metka_mpr == 1)
 {
  iceb_u_str parol;
  char imaf_parol[500];
//  sprintf(imaf_parol,"%s/parol.alx",putnansi);
  sprintf(imaf_parol,"parol.alx");
  char kodop[56];
  for(;;)
   {
    parol.new_plus("");
    if(iceb_mous_klav(gettext("Введите пароль"),&parol,40,1,1,1,NULL) != 0)
     {
      if(iceb_menu_danet(gettext("Завершить работу с программой ?"),2,NULL) == 2)
        continue;
      break;
     }

    if(parol.getdlinna() <= 1)
     {
      char cod[11];
      memset(cod,'\0',sizeof(cod));      
//      int kodoh=0;
#ifdef   READ_CARD
//      read_card(1,cod,NULL);
#endif
//      printf("код = %s\n",cod);
      parol.new_plus((char*)cod);
      
     }

    if(parol.getdlinna() <= 1)
     continue;
    if(iceb_u_SRAV(",,,",parol.ravno(),0) == 0)
     break;
    memset(kodop,'\0',sizeof(kodop));
//    printf("%s\n%s\n",parol.ravno(),imaf_parol);
    if(iceb_poldan(parol.ravno(),kodop,imaf_parol,NULL) != 0)
     {
      
      iceb_menu_soob(gettext("Не правильно введен пароль !"),NULL);
      continue;
     }
    
    kod_operatora=atoi(kodop);

    if(kod_operatora == 0)
     {
      iceb_menu_soob(gettext("Нулевой код оператора!"),NULL);
      continue;
     }

    if((ktoz=getpwuid(kod_operatora)) == NULL)
     {
      sprintf(bros,"%d %s",kod_operatora,gettext("Неизвестный логин"));
      iceb_menu_soob(bros,NULL);
      continue;
     }        
    kasr.prodavec.new_plus(ktoz->pw_name);
    metka_dostupa=0;
    if(iceb_u_proverka(spis_login,ktoz->pw_name,0,1) == 0)
      metka_dostupa=1;


    rest_k_vibor(metka_dostupa);
   }
 }
else  
 {
  kod_operatora=getuid();
  ktoz=getpwuid(kod_operatora);
  metka_dostupa=0;
  if(iceb_u_proverka(spis_login,ktoz->pw_name,0,1) == 0)
    metka_dostupa=1;
  rest_k_vibor(metka_dostupa);
 }


//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());
iceb_close_dserver();
sql_closebaz(&bd);
    
   
return( 0 );

}


/**************************/
/*выбор*/
/**************************/
void rest_k_vibor(int metka_dostupa)
{
int kom=0;
class iceb_u_str kodkl("");
double suma;

while( kom >= 0 && kom < KOLKNOP-1 )
 {
  kom=glmenu(kom,metka_dostupa);
  switch(kom)
   {

    case  RASHET:
      for(;;) 
       {
        kodkl.new_plus("");      
        if(iceb_mous_klav(gettext("Введите код клиента"),&kodkl,20,1,0,1,NULL) == 0)
          ras_kl(kodkl.ravno());
        else
         break;
       }      
      break;

    case RAB_S_KAS:
      iceb_rab_kas(&kasr,1);
      break;

    case VVOD_SUM_NA_SHET:
      vsnsk();
      break;
    case SLUG_VNES:
     if(iceb_mariq_slvi(&kasr,&suma,0) == 0)
        zap_v_kas(nomer_kas,"",suma,0,"",0,0,0,0,kod_operatora,"");
      break;

    case SLUG_IZQ:
      if(iceb_mariq_slvi(&kasr,&suma,1) == 0)
        zap_v_kas(nomer_kas,"",suma,0,"",0,0,0,0,kod_operatora,"");
      break;

    case SPISOK_OPLAT:
      l_spis_opl_z(0,NULL);
      break;

    case SALDO_PO_KASE:
      suma=ras_sal_kas(nomer_kas,NULL);

      kodkl.new_plus(gettext("Касса"));
      kodkl.plus(":");
      kodkl.plus(nomer_kas);
      kodkl.ps_plus(gettext("Сальдо"));
      kodkl.plus(":");
      kodkl.plus(suma);
      iceb_menu_soob(&kodkl,NULL);
      
      break;
      
    case KOLIH_KLIENT:
      l_rest_k_kolih_klient();
      break;

 

    case DEN_ROG:
      spdnr_r(NULL);
      break;

   case VIHOD:

      break;

   }

 }    



}


/***********************************/
/* Главное меню*/
/***********************************/
int glmenu(int knfokus,int metka_dostupa)
{
struct  tm      *bf;
time_t  tmm;
iceb_u_str NADPIS;
char     bros[512];
i_rest_data data;



time(&tmm);
bf=localtime(&tmm);


short ddd=bf->tm_mday;
short mmm=bf->tm_mon+1;
short ggg=bf->tm_year+1900;

iceb_infosys(host,VERSION,DVERSIQ,ddd,mmm,ggg,imabaz,&NADPIS,kod_operatora);

data.window = gtk_window_new( GTK_WINDOW_TOPLEVEL );

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

sprintf(bros,"%s %s",NAME_SYSTEM,gettext("Расчёт клиентов"));
gtk_window_set_title(GTK_WINDOW(data.window),iceb_u_toutf(bros));

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);
/******************
gtk_window_set_default_size(GTK_WINDOW(data.window),
    500, //Горизонталь
    500); //вертикаль
*******************/
//gtk_widget_show(data.window); //Отрисовываем, чтобы можно было взять цвет

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(i_rest_destroy),&data.knopka);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_vihod),&data.knopka);

GtkWidget *hbox=gtk_hbox_new(FALSE,0);
GtkWidget *vbox=gtk_vbox_new(FALSE,1);
GtkWidget *vbox1=gtk_vbox_new(FALSE,5);

gtk_container_add(GTK_CONTAINER(data.window),hbox);
gtk_container_add(GTK_CONTAINER(hbox),vbox);
gtk_container_add(GTK_CONTAINER(hbox),vbox1);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
iceb_u_str stroka;
stroka.plus(organ);
stroka.ps_plus(gettext("Расчёт клиентов"));
stroka.ps_plus(gettext("Касса"));
stroka.plus(" N");
stroka.plus(nomer_kas);
data.label=gtk_label_new(stroka.ravno_toutf());

//PangoFontDescription *font_pango=pango_font_description_from_string("Sans 16");
//gtk_widget_modify_font(data.label,font_pango);
//pango_font_description_free(font_pango);


gtk_box_pack_start((GtkBox*)vbox,data.label,FALSE,FALSE,0);

//font_pango=pango_font_description_from_string("Sans 14");

GtkWidget *knopka[KOLKNOP];

GtkWidget *labelk=gtk_label_new(gettext("Расчёт клиента"));
//gtk_widget_modify_font(labelk,font_pango);

knopka[RASHET]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[RASHET]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[RASHET],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[RASHET]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[RASHET]),(gpointer)RASHET);

labelk=gtk_label_new(gettext("Работа с кассовым регистратором"));
knopka[RAB_S_KAS]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[RAB_S_KAS]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[RAB_S_KAS],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[RAB_S_KAS]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[RAB_S_KAS]),(gpointer)RAB_S_KAS);
/**************
labelk=gtk_label_new(gettext("Внесение в кассу клиентом"));
knopka[VVOD_SUM_V_KAS]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[VVOD_SUM_V_KAS]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[VVOD_SUM_V_KAS],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[VVOD_SUM_V_KAS]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[VVOD_SUM_V_KAS]),(gpointer)VVOD_SUM_V_KAS);

labelk=gtk_label_new(gettext("Выдача из кассы клиенту"));
knopka[POLUH_SUM_V_KAS]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[POLUH_SUM_V_KAS]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[POLUH_SUM_V_KAS],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[POLUH_SUM_V_KAS]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[POLUH_SUM_V_KAS]),(gpointer)POLUH_SUM_V_KAS);
*****************/

labelk=gtk_label_new(gettext("Просмотр списка оплат"));
knopka[SPISOK_OPLAT]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[SPISOK_OPLAT]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[SPISOK_OPLAT],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[SPISOK_OPLAT]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[SPISOK_OPLAT]),(gpointer)SPISOK_OPLAT);

labelk=gtk_label_new(gettext("Расчёт сальдо по кассе"));
knopka[SALDO_PO_KASE]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[SALDO_PO_KASE]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[SALDO_PO_KASE],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[SALDO_PO_KASE]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[SALDO_PO_KASE]),(gpointer)SALDO_PO_KASE);

labelk=gtk_label_new(gettext("Расчёт количества входов/выходов"));
knopka[KOLIH_KLIENT]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[KOLIH_KLIENT]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[KOLIH_KLIENT],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[KOLIH_KLIENT]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[KOLIH_KLIENT]),(gpointer)KOLIH_KLIENT);

labelk=gtk_label_new(gettext("Именины"));
knopka[DEN_ROG]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[DEN_ROG]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[DEN_ROG],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[DEN_ROG]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[DEN_ROG]),(gpointer)DEN_ROG);

labelk=gtk_label_new(gettext("Ввод суммы на счёт клиента"));
knopka[VVOD_SUM_NA_SHET]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[VVOD_SUM_NA_SHET]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[VVOD_SUM_NA_SHET],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[VVOD_SUM_NA_SHET]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[VVOD_SUM_NA_SHET]),(gpointer)VVOD_SUM_NA_SHET);

labelk=gtk_label_new(gettext("Служебное внесение денег в кассу"));
knopka[SLUG_VNES]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[SLUG_VNES]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[SLUG_VNES],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[SLUG_VNES]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[SLUG_VNES]),(gpointer)SLUG_VNES);

labelk=gtk_label_new(gettext("Служебное изъятие денег из кассы"));
knopka[SLUG_IZQ]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[SLUG_IZQ]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[SLUG_IZQ],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[SLUG_IZQ]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[SLUG_IZQ]),(gpointer)SLUG_IZQ);

labelk=gtk_label_new(gettext("Выход"));
knopka[VIHOD]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[VIHOD]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[VIHOD],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[VIHOD]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[VIHOD]),(gpointer)VIHOD);

GtkWidget *mainvbox1=gtk_vbox_new(FALSE,5);
gtk_widget_realize(data.window);
GtkStyle *style;
GdkPixmap *pixmap;
GdkBitmap *mask;
GtkWidget *pixmapWidget;

style = gtk_widget_get_style(data.window);
pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],
                                    (gchar **)icebr200_xpm);
pixmapWidget=gtk_pixmap_new(pixmap,mask);
gtk_container_add(GTK_CONTAINER(mainvbox1),pixmapWidget);

pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],
                                    (gchar **)flag_xpm);
pixmapWidget=gtk_pixmap_new(pixmap,mask);
gtk_container_add(GTK_CONTAINER(mainvbox1),pixmapWidget);

gtk_container_add(GTK_CONTAINER(vbox1),mainvbox1);




GtkWidget *label = gtk_label_new (iceb_u_toutf(NADPIS.ravno()));

gtk_box_pack_start (GTK_BOX (vbox1), label, TRUE, TRUE, 0);

if(shrift_rek_raz.getdlinna() > 1)
 {
  PangoFontDescription *font_pango=pango_font_description_from_string(shrift_rek_raz.ravno());
  gtk_widget_modify_font(label,font_pango);
  pango_font_description_free(font_pango);
 }

gtk_widget_grab_focus(knopka[knfokus]);

gtk_widget_show_all( data.window );
if(metka_dostupa == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(knopka[SLUG_VNES]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(knopka[VVOD_SUM_NA_SHET]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(knopka[SLUG_IZQ]),FALSE);//Недоступна
 }
gtk_main();


return(data.knopka);

}

/***************************/
/*Обработчик нажатия кнопок*/
/***************************/

void   i_rest_knopka(GtkWidget *widget,class i_rest_data *data)
{

//int knop=GPOINTER_TO_INT(gtk_widget_get_name(widget);
//int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("vmenu_knopka knop=%d\n",knop);

data->knopka=(int)knop;

gtk_widget_destroy(data->window);


}
/*************************************/
/*Обработчик  сигнала destroy_event*/
/************************************/
gboolean i_rest_destroy(GtkWidget *widget,GdkEvent *event,int *knopka)
{

*knopka=-1;

gtk_widget_destroy(widget);
return(FALSE);
}

/*****************************/
/*Определиние количества человек находящихся в подразделении*/
/*************************************************************/

void l_rest_k_kolih_klient()
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,NULL);
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kol_klient_plus=0;
int kol_klient_minus=0;
char grup_sotrud[100];
memset(grup_sotrud,'\0',sizeof(grup_sotrud));

iceb_poldan("Коды групп персонала предприятия",grup_sotrud,"restnast.alx",NULL);

static class iceb_u_str vremn("");
static class iceb_u_str datan("");
static class iceb_u_str kod_podr("");
if(kod_podr.getdlinna() <= 1)
  kod_podr.new_plus(kasr.sklad);

if(vv_kolih(&kod_podr,&datan,&vremn,NULL) != 0)
 return;



char strsql[1024];

sprintf(strsql,"select kk,tp from Restvv where dv >= '%s %s' and kp=%d",
datan.ravno_sqldata(),
vremn.ravno_time(),
kod_podr.ravno_atoi());
printf("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return;
 }
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  sprintf(strsql,"select grup from Taxiklient where kod='%s'",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,NULL) == 1)
   if(iceb_u_proverka(grup_sotrud,row1[0],0,1) == 0)
    continue;
//  printf("%s %s\n",row[0],row[1]);
    
  if(atoi(row[1]) == 1)
    kol_klient_plus++;
  
  if(atoi(row[1]) == 2)
    kol_klient_minus++;
  
 }

char naim_podr[50];
memset(naim_podr,'\0',sizeof(naim_podr));

sprintf(strsql,"select naik from Restpod where kod=%d",kod_podr.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
 strncpy(naim_podr,row[0],sizeof(naim_podr)-1);
 
sprintf(strsql,"%s:%s %s\n%s:%d %s\n%s:%d %s:%d %s:%d",
gettext("Дата начала расчёта"),
datan.ravno(),
vremn.ravno_time(),
gettext("Подразделение"),
kod_podr.ravno_atoi(),
naim_podr,
gettext("Количество входов"),kol_klient_plus,
gettext("Количество выходов"),kol_klient_minus,
gettext("Разница"),kol_klient_plus-kol_klient_minus);



gdite.close(); //Закрываем окно 

iceb_menu_soob(strsql,NULL);


}





