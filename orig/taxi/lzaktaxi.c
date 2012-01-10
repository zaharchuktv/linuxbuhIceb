/*$Id: lzaktaxi.c,v 1.72 2011-02-21 07:36:21 sasa Exp $*/
/*13.07.2007	30.9.2003	Белых А.И.	lzaktaxi.c
Диспетчеризация такси
*/
#include        <stdlib.h>
#include        <time.h>
#include        <errno.h>
#include        <math.h>
#include        <unistd.h>
#include "taxi.h"
#include "lzaktaxi.h"
enum
 {
  FK2,
  SFK2,
  FK3,
  FK4,
  FK5,
  FK6,
  FK7,
  FK10,
  KOL_F_KL
 };


#define         KOLTITL 17
void       lzaktaxi_radio0(GtkWidget *,class lzaktaxi_data *);
void       lzaktaxi_radio1(GtkWidget *,class lzaktaxi_data *);
void       lzaktaxi_radio2(GtkWidget *,class lzaktaxi_data *);
void       lzaktaxi_radio3(GtkWidget *,class lzaktaxi_data *);
int prov_zak_time(char *data,char *vremq,int kolsek);
int   lzaktaxi_p(class zaktaxi_rek *rek_poi,GtkWidget *wpredok);
void       menu_poi_zaktaxi_label(class lzaktaxi_data *);
void  kodzav_get1(GtkWidget *widget,class lzaktaxi_data *v_data);


extern SQL_baza	bd;
extern char *name_system;
class lzaktaxi_data data;

void           lzaktaxi()
{
char         bros[300];
iceb_u_str kol00;
iceb_u_str kol0;
iceb_u_str kol1;
iceb_u_str kol2;
iceb_u_str kol3;
iceb_u_str kol4;
iceb_u_str kol5;
iceb_u_str kol6;
iceb_u_str kol7;
iceb_u_str kol8;
iceb_u_str kol9;
iceb_u_str kol10;
iceb_u_str kol11;
iceb_u_str kol12;
iceb_u_str kol13;
iceb_u_str kol14;
iceb_u_str kol15;
/****************
kol00.plus("M");
kol0.plus(iceb_u_toutf("Дата и время заказа"));
kol3.plus(iceb_u_toutf("Адрес подачи"));
kol5.plus(iceb_u_toutf("Адрес поездки"));
kol4.plus(iceb_u_toutf("Телефон"));
kol6.plus(iceb_u_toutf("Код клиента"));
kol1.plus(iceb_u_toutf("Водитель"));
kol2.plus(iceb_u_toutf("Гос.номер"));
kol9.plus(iceb_u_toutf("Код завершения"));//10
kol7.plus(iceb_u_toutf("К/п"));
kol8.plus(iceb_u_toutf("Коментарий"));
kol10.plus(iceb_u_toutf("Фамилия"));
kol11.plus(iceb_u_toutf("Сумма"));
kol12.plus(iceb_u_toutf("Дата и время звонка"));//11
kol13.plus(iceb_u_toutf("Дата и время завершения"));//12
kol14.plus(iceb_u_toutf("Дата и время записи"));//13
kol15.plus(iceb_u_toutf("Оператор"));//14

gchar *titles[] = \
 { 
  kol00.ravno(),
  kol0.ravno(),
  kol3.ravno(),
  kol5.ravno(),
  kol4.ravno(),
  kol6.ravno(),
  kol1.ravno(),
  kol2.ravno(),
  kol9.ravno(),
  kol7.ravno(),
  kol8.ravno(),
  kol10.ravno(),
  kol11.ravno(),
  kol12.ravno(),
  kol13.ravno(),
  kol14.ravno(),
  kol15.ravno()
 };
********************/
gchar *titles[] = \
 { 
  iceb_u_toutf("M"),
  iceb_u_toutf("Дата и время заказа"),
  iceb_u_toutf("Адрес подачи"),
  iceb_u_toutf("Адрес поездки"),
  iceb_u_toutf("Телефон"),
  iceb_u_toutf("Код клиента"),
  iceb_u_toutf("Водитель"),
  iceb_u_toutf("Гос.номер"),
  iceb_u_toutf("Код завершения"),
  iceb_u_toutf("К/п"),
  iceb_u_toutf("Коментарий"),
  iceb_u_toutf("Фамилия"),
  iceb_u_toutf("Сумма"),
  iceb_u_toutf("Дата и время звонка"),
  iceb_u_toutf("Дата и время завершения"),
  iceb_u_toutf("Дата и время записи"),
  iceb_u_toutf("Оператор")

 };
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
//gtk_widget_set_usize(GTK_WIDGET(data.window), 600, 400);
//gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

sprintf(bros,"%s %s",NAME_SYSTEM,iceb_u_toutf("Список заказов"));
gtk_window_set_title(GTK_WINDOW(data.window),bros);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(lzaktaxi_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *vbox=gtk_vbox_new(FALSE, 5);

gtk_container_add(GTK_CONTAINER(data.window), vbox);
data.label=gtk_label_new(gettext("Поиск"));
gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);


data.scrolled_window= gtk_scrolled_window_new (NULL, NULL);
//gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(data.scrolled_window),
//                                    GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.scrolled_window),
                                GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
gtk_widget_set_usize(GTK_WIDGET(data.scrolled_window), 500, 300);

gtk_box_pack_start(GTK_BOX(vbox), data.scrolled_window, TRUE, TRUE, 0);
gtk_widget_show (data.scrolled_window);

gtk_widget_realize(data.scrolled_window);

gdk_window_set_cursor(data.scrolled_window->window,gdk_cursor_new(ICEB_CURSOR));
//gdk_window_set_cursor(data.scrolled_window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

data.list = gtk_clist_new_with_titles(KOLTITL, titles);



gtk_clist_column_titles_passive((GtkCList *) data.list);

gtk_signal_connect(GTK_OBJECT(data.list), "select_row", GTK_SIGNAL_FUNC(lzaktaxi_selection_row),&data);
gtk_signal_connect(GTK_OBJECT(data.list), "unselect_row", GTK_SIGNAL_FUNC(lzaktaxi_unselection_row),&data);


//gtk_clist_set_column_width (GTK_CLIST(data.list), 0, 60);
//gtk_clist_set_column_width (GTK_CLIST(data.list), 1, 300);

gtk_container_add(GTK_CONTAINER(data.scrolled_window), data.list);
//gtk_widget_show(data.list);

/**********************
gtk_widget_realize(data.list);
gdk_window_set_cursor(data.list->window,gdk_cursor_new(GDK_HAND1));
****************/

data.hboxradio = gtk_hbox_new(FALSE, 0);
gtk_box_pack_start(GTK_BOX(vbox), data.hboxradio, FALSE, TRUE, 0);

data.metkarr=0;

//Вставляем радиокнопки
GSList *group=NULL;

data.radiobutton0=gtk_radio_button_new_with_label(NULL,iceb_u_toutf("Незавершенные заказы"));
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.radiobutton0, TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton0), "clicked",GTK_SIGNAL_FUNC(lzaktaxi_radio0),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton0));

data.radiobutton1=gtk_radio_button_new_with_label(group,iceb_u_toutf("Все заказы"));
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.radiobutton1, TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton1), "clicked",GTK_SIGNAL_FUNC(lzaktaxi_radio1),&data);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton1));

data.radiobutton2=gtk_radio_button_new_with_label(group,iceb_u_toutf("Сутки"));
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.radiobutton2, TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton2), "clicked",GTK_SIGNAL_FUNC(lzaktaxi_radio2),&data);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton2));

data.radiobutton3=gtk_radio_button_new_with_label(group,iceb_u_toutf("Первые"));
gtk_signal_connect(GTK_OBJECT(data.radiobutton3), "clicked",GTK_SIGNAL_FUNC(lzaktaxi_radio3),&data);
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.radiobutton3, TRUE, TRUE, 0);
//кнопку устанавливаем активной после отображения окна
//gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton3),TRUE); //Устанавливем активной кнопку

//Вставляем настройку количества записей
GtkObject *adjustment1=gtk_adjustment_new(10.0,1.0,1000.0,1.0,0.0,0.0);
data.spin1=gtk_spin_button_new(GTK_ADJUSTMENT(adjustment1),1.0,0);
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.spin1, TRUE, TRUE, 0);
GtkTooltips *tooltipsspin1=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltipsspin1,data.spin1,iceb_u_toutf("Установка количества записей для просмотра"),NULL);

//Вставляем кнопку автоматического обновления списка

GtkWidget *knopaos=gtk_check_button_new_with_label(iceb_u_toutf("Автоматическое обновление списка"));
gtk_signal_connect(GTK_OBJECT(knopaos), "toggled",GTK_SIGNAL_FUNC(lzaktaxi_knopaos),&data);
gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(knopaos),FALSE);
gtk_box_pack_start (GTK_BOX (data.hboxradio),knopaos, TRUE, TRUE, 0);

//Вставляем настройку автоматического обновления списка
GtkObject *adjustment=gtk_adjustment_new(30.0,1.0,3600.0,1.0,0.0,0.0);
data.spin=gtk_spin_button_new(GTK_ADJUSTMENT(adjustment),1.0,0);
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.spin, TRUE, TRUE, 0);

GtkTooltips *tooltipsspin=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltipsspin,data.spin,iceb_u_toutf("Установка интервала автоматического обновления списка в секундах"),NULL);

sprintf(bros,"К/з: 0");
data.labelkz=gtk_label_new(iceb_u_toutf(bros));
gtk_box_pack_end(GTK_BOX(data.hboxradio),data.labelkz, TRUE, TRUE, 0);

GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);
gtk_widget_show(hbox);

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(hbox), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(lzaktaxi_knopka),&data);
GtkTooltips *tooltips0=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips0,data.knopka[FK2],iceb_u_toutf("Ввод нового заказа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(hbox), data.knopka[SFK2], TRUE, TRUE, 0);
gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[SFK2]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(lzaktaxi_knopka),&data);
GtkTooltips *tooltips1=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips1,data.knopka[SFK2],gettext("Корректировка выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(hbox), data.knopka[FK3], TRUE, TRUE, 0);
gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK3]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(lzaktaxi_knopka),&data);
GtkTooltips *tooltips2=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips2,data.knopka[FK3],gettext("Удаление выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(lzaktaxi_knopka),&data);
gtk_box_pack_start(GTK_BOX(hbox), data.knopka[FK4], TRUE, TRUE, 0);
GtkTooltips *tooltips3=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips3,data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(hbox), data.knopka[FK5], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(lzaktaxi_knopka),&data);
GtkTooltips *tooltips4=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips4,data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);

sprintf(bros,"F6 %s",iceb_u_toutf("Завершить"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(hbox), data.knopka[FK6], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(lzaktaxi_knopka),&data);
GtkTooltips *tooltips5=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips5,data.knopka[FK6],iceb_u_toutf("Отметить выбранный заказ как завершенный"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);

sprintf(bros,"F7 %s",iceb_u_toutf("Обновить"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(hbox), data.knopka[FK7], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(lzaktaxi_knopka),&data);
GtkTooltips *tooltips7=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips7,data.knopka[FK7],iceb_u_toutf("Обновить список заказов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(hbox),data.knopka[FK10], TRUE, TRUE, 0);
GtkTooltips *tooltips6=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips6,data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(lzaktaxi_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);

gtk_widget_grab_focus(data.knopka[FK10]);

gtk_widget_show_all(data.window);
gtk_widget_hide(data.label); //Скрываем от показа виджет

//data.colormap=gdk_colormap_get_system();

//if(gdk_color_parse("blue",&data.color_blue) == TRUE)
//  gdk_color_alloc(data.colormap,&data.color_blue);

//if(gdk_color_parse("red",&data.color_red) == TRUE)
//  gdk_color_alloc(data.colormap,&data.color_red);
gdk_color_parse("blue",&data.color_blue);
gdk_color_parse("red",&data.color_red);

//zapzaktaxi(&data); //записывается при включении радиокнопки

gtk_widget_show_all(data.window);
gtk_widget_hide(data.label); //Скрываем от показа виджет

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton3),TRUE); //Устанавливем активной кнопку


gtk_main();


return;

}

/***********************/
/*Запись в окно        */
/***********************/

void    zapzaktaxi(class lzaktaxi_data *data)
{

class iceb_clock skur(data->window);
char  bros[300];
char  strsql[300];
SQL_str  row,row1;
int    kolstr=0;
const short DLINNA=500;
gchar *stroka[KOLTITL];
int   i=0;

data->metkavz=0;

//gdk_window_set_cursor(data->scrolled_window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
gtk_clist_clear(GTK_CLIST(data->list));

for(i=0 ; i < KOLTITL; i++)
 stroka[i]=new gchar[DLINNA];

//Формируем запрос к базе данных
zapros(strsql,data);


if((kolstr=data->cur.make_cursor(&bd,strsql)) < 0)
 {
  gdk_window_set_cursor(data->scrolled_window->window,gdk_cursor_new(ICEB_CURSOR));
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
data->kolzap=0;
short nomstr=0;
char gosnomer[40];
SQLCURSOR cur1;
int nom_row=0;
while(data->cur.read_cursor(&row) != 0)
 {
  
  if(lzaktaxi_prov_row(row,data) != 0)
   continue;

  nomstr=0;
  //Метка не выполненного заказа
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  nom_row=0;
  if(row[0][0] == '0')
   {
    strcpy(stroka[nomstr],"?");  
    nom_row=1;
   }  

  //Дата и время заказа
  nomstr++;
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  sprintf(stroka[nomstr],"%s %s",iceb_u_datzap(row[6]),row[7]);

  nomstr++;
  //Адрес подачи
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  strncpy(stroka[nomstr],iceb_u_toutf(row[10]),DLINNA-1);

  nomstr++;
  //Куда ехать
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  strncpy(stroka[nomstr],iceb_u_toutf(row[11]),DLINNA-1);

  nomstr++;
  //Телефон  
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  strncpy(stroka[nomstr],iceb_u_toutf(row[4]),DLINNA-1);


  nomstr++;
  //Код клиента
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  strncpy(stroka[nomstr],iceb_u_toutf(row[1]),DLINNA-1);

  nomstr++;
  //Водитель
  memset(gosnomer,'\0',sizeof(gosnomer));
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  if(row[2][0] != '\0')
   {
    memset(bros,'\0',sizeof(bros));
    sprintf(strsql,"select fio,gosn from Taxivod where kod='%s'",row[2]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      strncpy(bros,row1[0],sizeof(bros)-1);
      strncpy(gosnomer,row1[1],sizeof(gosnomer)-1);
     }
    sprintf(strsql,"%s %s",row[2],bros);
    strncpy(stroka[nomstr],iceb_u_toutf(strsql),DLINNA-1);
   }
  else
   strncpy(stroka[nomstr],"",DLINNA-1);

  nomstr++;
  //Гос.номер автомобиля
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  if(row[2][0] != '\0')
   {
    strncpy(stroka[nomstr],iceb_u_toutf(gosnomer),DLINNA-1);
   }

  nomstr++;
  //Код завершения
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  if(atoi(row[0]) != 0)
   { 
    memset(bros,'\0',sizeof(bros));
    sprintf(strsql,"select naik from Taxikzz where kod=%s",row[0]);
//    printf("%s\n",strsql);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     strncpy(bros,row1[0],sizeof(bros)-1);
   
    strncpy(stroka[nomstr],iceb_u_toutf(bros),DLINNA-1);
   }
  else
    strncpy(stroka[nomstr],"",DLINNA-1);


  nomstr++;
  //Количество пассажиров
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  strncpy(stroka[nomstr],row[5],DLINNA-1);

  nomstr++;
  //Коментарий
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  strncpy(stroka[nomstr],iceb_u_toutf(row[12]),DLINNA-1);


  nomstr++;
  //Фамилия
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  strncpy(stroka[nomstr],iceb_u_toutf(row[3]),DLINNA-1);

  nomstr++;
  //Сумма
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  strncpy(stroka[nomstr],row[13],DLINNA-1);



  nomstr++;
  //Дата и время заказа
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  sprintf(stroka[nomstr],"%s %s",iceb_u_datzap(row[8]),row[9]);


  nomstr++;
  //Дата и время установки отметки завершения
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  strncpy(stroka[nomstr],iceb_u_vremzap(row[14]),DLINNA-1);


  nomstr++;
  //Дата и время записи
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  strncpy(stroka[nomstr],iceb_u_vremzap(row[16]),DLINNA-1);


  nomstr++;
  //Оператор
  memset(stroka[nomstr],'\0',sizeof(DLINNA));
  strncpy(stroka[nomstr],iceb_u_kszap(row[15],0),DLINNA-1);

    
  gtk_clist_append((GtkCList *)data->list,stroka);

  gtk_clist_set_row_data((GtkCList *)data-> list,data->kolzap++,row);
  if(nom_row == 1)
   {
   
    if(prov_zak_time(row[6],row[7],1800) == 0)
     gtk_clist_set_background((GtkCList *)data->list,data->kolzap-1,&data->color_red);
    else
     gtk_clist_set_background((GtkCList *)data->list,data->kolzap-1,&data->color_blue);
   }
 }

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//доступна
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK4]),TRUE);//доступна

//gdk_window_set_cursor(data->scrolled_window->window,gdk_cursor_new(ICEB_CURSOR));

sprintf(bros,"К/з: %d",data->kolzap);

gtk_label_set_text(GTK_LABEL(data->labelkz),iceb_u_toutf(bros));

//printf("zapzaktaxi-fine\n");

}

/**********************************/
/*Проверка записи на условия поиска*/
/************************************/

int   lzaktaxi_prov_row(SQL_str row,class lzaktaxi_data *data)
{

if(data->metkapoi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poi_zaktaxi.kodk.ravno(),row[1],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->poi_zaktaxi.kv.ravno(),row[2],0,0) != 0)
   return(1);

//Поиск образца в строке
if(data->poi_zaktaxi.fio.getdlinna() > 1 && iceb_u_strstrm(row[3],data->poi_zaktaxi.fio.ravno()) == 0)
 return(1);

if(data->poi_zaktaxi.telef.getdlinna() > 1 && iceb_u_strstrm(row[4],data->poi_zaktaxi.telef.ravno()) == 0)
 return(1);

if(data->poi_zaktaxi.kolp.getdlinna() > 1)
 if(atoi(data->poi_zaktaxi.kolp.ravno()) != atoi(row[5]))
  return(1);

if(data->poi_zaktaxi.adreso.getdlinna() > 1 && iceb_u_strstrm(row[10],data->poi_zaktaxi.adreso.ravno()) == 0)
 return(1);

if(data->poi_zaktaxi.adresk.getdlinna() > 1 && iceb_u_strstrm(row[11],data->poi_zaktaxi.adresk.ravno()) == 0)
 return(1);

if(data->poi_zaktaxi.koment.getdlinna() > 1 && iceb_u_strstrm(row[12],data->poi_zaktaxi.koment.ravno()) == 0)
 return(1);

if(data->poi_zaktaxi.suma.getdlinna() > 1)
 if(fabs(atof(data->poi_zaktaxi.suma.ravno())-atof(row[13])) > 0.009)
  return(1);

if(iceb_u_proverka(data->poi_zaktaxi.kodzav.ravno(),row[0],0,0) != 0)
   return(1);


//Полное сравнение
if(iceb_u_proverka(data->poi_zaktaxi.kod_operatora.ravno(),row[15],0,0) != 0)
 return(1);

return(0);
}



/******************************/
/*Удаление записи             */
/******************************/

void   udzzaktaxi(class lzaktaxi_data *data)
{
char  strsql[500];

iceb_u_str sp;
sp.plus("Удалить запись ? Вы уверены ?");
if(iceb_menu_danet(&sp,2,NULL) == 2)
 return;

if(iceb_parol(0,NULL) != 0)
  return;

sprintf(strsql,"delete from Taxizak where \
kz=%s and kodk='%s' and kv='%s' and fio='%s' and telef='%s' and kolp=%s and datvz='%s' \
and vremvz='%s' and datz='%s' and vremz='%s' and ktoi=%s and vrem=%s",\
data->rowv[0],\
data->rowv[1],\
data->rowv[2],\
data->rowv[3],\
data->rowv[4],\
data->rowv[5],\
data->rowv[6],\
data->rowv[7],\
data->rowv[8],\
data->rowv[9],\
data->rowv[15],\
data->rowv[16]);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_u_str SP;
   SP.plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&SP,NULL);
   return;
  }
 else
  {
   iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,NULL);
   return;
  }
 }
zapzaktaxi(data);

}

/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  lzaktaxi_radio0(GtkWidget *widget,class lzaktaxi_data *data)
{
//g_print("lzaktaxi_radio0\n");
/*
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("lzaktaxi_radio knop=%s\n",knop);

data->metkarr=atoi(knop);
*/
if(data->metkarr == 0)
  return;
data->metkarr=0;
//printf("data->metkarr=%d\n",data->metkarr);

zapzaktaxi(data);

}
void  lzaktaxi_radio1(GtkWidget *widget,class lzaktaxi_data *data)
{
//g_print("lzaktaxi_radio1\n");
if(data->metkarr == 1)
  return;
data->metkarr=1;
//printf("data->metkarr=%d\n",data->metkarr);
zapzaktaxi(data);
}

void  lzaktaxi_radio2(GtkWidget *widget,class lzaktaxi_data *data)
{
//g_print("lzaktaxi_radio2\n");
if(data->metkarr == 2)
  return;

data->metkarr=2;
//printf("data->metkarr=%d\n",data->metkarr);
zapzaktaxi(data);

}
void  lzaktaxi_radio3(GtkWidget *widget,class lzaktaxi_data *data)
{
//g_print("lzaktaxi_radio3\n");
if(data->metkarr == 3)
  return;

data->metkarr=3;
//printf("data->metkarr=%d\n",data->metkarr);
zapzaktaxi(data);

}

/************************************************************************/
/*Обработчик кнопки влючения/выключения автомтического обновления списка*/
/************************************************************************/
void  lzaktaxi_knopaos(GtkWidget *widget,class lzaktaxi_data *data)
{
//printf("lzaktaxi_knopaos\n");

int vrem=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(data->spin));
//printf("vrem=%d\n",vrem);
vrem*=1000;
if(GTK_TOGGLE_BUTTON(widget)->active == TRUE)
 {
//  printf("Включено\n");
  data->timer_read=gtk_timeout_add(vrem,(GtkFunction)zapzaktaxi,(gpointer)data);
 }
if(GTK_TOGGLE_BUTTON(widget)->active == FALSE)
 {
//  printf("Выключено\n");
  gtk_timeout_remove(data->timer_read);
  data->timer_read=0;
 }

}


/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  lzaktaxi_knopka(GtkWidget *widget,class lzaktaxi_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("lzaktaxi_knopka knop=%s\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {
  case FK2:
    data->metkazapisi=0;
    lzaktaxiv(data);
    return;  

  case SFK2:
    if(data->metkavz == 0)
       return;
    data->metkazapisi=1;
    lzaktaxiv(data);
    return;  

  case FK3:
    if(data->metkavz == 1)
      udzzaktaxi(data);
    return;  

  case FK4:
    data->metkapoi=0;
    if(lzaktaxi_p(&data->poi_zaktaxi,data->window) == 0)
     {
      menu_poi_zaktaxi_label(data);
//      data->metkapoi=1;
     }    
    else
      gtk_widget_hide(data->label); //Скрываем от показа виджет

    zapzaktaxi(data);
    return;  

  case FK5:
    gtk_widget_hide(data->window);
    raszaktaxi(data);
    gtk_widget_show(data->window);
    return;  

  case FK6:
    if(data->metkavz == 1)
      zavzak(data);
    return;  

  case FK7:
    zapzaktaxi(data);
    return;  


  case FK10:
//    printf("lzaktaxi_knopka F10\n");
    if(data->timer_read != 0)
       gtk_timeout_remove(data->timer_read);

    gtk_widget_destroy(data->window);
    return;
 }
}
 
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   lzaktaxi_key_press(GtkWidget *widget,GdkEventKey *event,class lzaktaxi_data *data)
{
//mystroka soob(0);
//printf("lzaktaxi_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
//    g_print("lzaktaxi_knopka F2\n");


    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");

    return(TRUE);
   
  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);

  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    return(TRUE);

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
//    gtk_widget_destroy(widget);
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}

/**************************/
/*Обработчик выбора строки*/
/**************************/

void lzaktaxi_selection_row( GtkWidget *clist,gint row,gint column,GdkEventButton *event,
class lzaktaxi_data *data)
{
gchar *text;
//SQL_str rowsql;

gtk_clist_get_text(GTK_CLIST(clist), row, column, &text);

data->rowv=(SQL_str)gtk_clist_get_row_data(GTK_CLIST(clist), row);
data->metkavz=1;


//g_print("selection_row-You selected row %d. More specifically you clicked in "
//            "column %d, and the text in this cell is %s Shet=%s\n\n",
//	    row, column, text,data->rowv[0]);
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//доступна
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//доступна
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//доступна

if(event != NULL)
 if(event->type == GDK_2BUTTON_PRESS) //Двойное нажатие
  gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked"); 


}

/**********************************/
/*Обработчик отмены выбора строки*/
/**********************************/
void lzaktaxi_unselection_row( GtkWidget *clist,gint row,gint column,GdkEventButton *event,
class lzaktaxi_data *data)
{
gchar *text;
data->metkavz=0;
gtk_clist_get_text(GTK_CLIST(clist), row, column, &text);

//g_print("unselection_row-You selected row %d. More specifically you clicked in "
//            "column %d, and the text in this cell is %s\n\n",
//	    row, column, text);
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна

}

/********************************/
/*Установка надписи поиска      */
/********************************/
void  menu_poi_zaktaxi_label(class lzaktaxi_data *data)
{
char bros[300];
iceb_u_str spis;

data->metkapvk=1;
/*************
for(int i=0; i < KOLENTRY; i++)
   gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");

gtk_signal_emit_by_name(GTK_OBJECT(GTK_COMBO(data->windowkodzav)->entry),"activate");
gtk_signal_emit_by_name(GTK_OBJECT(GTK_COMBO(data->windowvoditel)->entry),"activate");
********************/
data->metkapvk=0;

data->metkapoi=1;
spis.plus(iceb_u_toutf("Поиск !!!"));

if(data->poi_zaktaxi.datan.getdlinna() > 1)
 {
  sprintf(bros,"Дата начала: %.20s",data->poi_zaktaxi.datan.ravno());
  spis.ps_plus(iceb_u_toutf(bros));
 }
if(data->poi_zaktaxi.datak.getdlinna() > 1)
 {
  sprintf(bros,"Дата конца: %.20s",data->poi_zaktaxi.datak.ravno());
  spis.ps_plus(iceb_u_toutf(bros));
 }

if(data->poi_zaktaxi.kodzav.getdlinna() > 1)
 {
  sprintf(bros,"Код завершения: %.20s",data->poi_zaktaxi.kodzav.ravno());
  spis.ps_plus(iceb_u_toutf(bros));
 }
if(data->poi_zaktaxi.kodk.getdlinna() > 1)
 {
  sprintf(bros,"Код клиента: %.20s",data->poi_zaktaxi.kodk.ravno());
  spis.ps_plus(iceb_u_toutf(bros));
 }

if(data->poi_zaktaxi.kv.getdlinna() > 1)
 {
  sprintf(bros,"Код водителя: %.20s",data->poi_zaktaxi.kv.ravno());
  spis.ps_plus(iceb_u_toutf(bros));
 }

if(data->poi_zaktaxi.fio.getdlinna() > 1)
 {
  sprintf(bros,"ФИО: %.20s",data->poi_zaktaxi.fio.ravno());
  spis.ps_plus(iceb_u_toutf(bros));
 }

if(data->poi_zaktaxi.telef.getdlinna() > 1)
 {
  sprintf(bros,"Телефон: %.20s",data->poi_zaktaxi.telef.ravno());
  spis.ps_plus(iceb_u_toutf(bros));
 }    

if(data->poi_zaktaxi.kolp.getdlinna() > 1)
 {
  sprintf(bros,"Колич.пас.: %.20s",data->poi_zaktaxi.kolp.ravno());
  spis.ps_plus(iceb_u_toutf(bros));
 }    


if(data->poi_zaktaxi.datvz.getdlinna() > 1)
 {
  sprintf(bros,"Дата заказа: %.20s",data->poi_zaktaxi.datvz.ravno());
  spis.ps_plus(iceb_u_toutf(bros));
 }

if(data->poi_zaktaxi.vremvz.getdlinna() > 1)
 {
  sprintf(bros,"Время заказа: %.20s",data->poi_zaktaxi.vremvz.ravno());
  spis.ps_plus(iceb_u_toutf(bros));
 }

if(data->poi_zaktaxi.adreso.getdlinna() > 1)
 {
  sprintf(bros,"Адрес подачи: %.20s",data->poi_zaktaxi.adreso.ravno());
  spis.ps_plus(iceb_u_toutf(bros));
 }

if(data->poi_zaktaxi.adresk.getdlinna() > 1)
 {
  sprintf(bros,"Куда: %.20s",data->poi_zaktaxi.adresk.ravno());
  spis.ps_plus(iceb_u_toutf(bros));
 }    
if(data->poi_zaktaxi.koment.getdlinna() > 1)
 {
  sprintf(bros,"Коментарий: %.20s",data->poi_zaktaxi.koment.ravno());
  spis.ps_plus(iceb_u_toutf(bros));
 }
if(data->poi_zaktaxi.suma.getdlinna() > 1)
 {
  sprintf(bros,"Сумма: %.20s",data->poi_zaktaxi.suma.ravno());
  spis.ps_plus(iceb_u_toutf(bros));
 }

if(data->poi_zaktaxi.kod_operatora.getdlinna() > 1)
 {
  sprintf(bros,"Оператор: %.20s",iceb_u_toutf(data->poi_zaktaxi.kod_operatora.ravno_toutf()));
  spis.ps_plus(iceb_u_toutf(bros));
 }

gtk_label_set_text(GTK_LABEL(data->label),spis.ravno());
    
gtk_widget_show(data->label); //Показываем

}
/****************************************/
/*Распечатка списка клиентов            */
/****************************************/

void	raszaktaxi(class lzaktaxi_data *data)
{
char		strsql[300];
SQL_str		row;
SQL_str		row1;
int		kolstr=0;
FILE		*ff;
char		imaf[40];
short 	        d,m,g;
SQLCURSOR cur;
SQLCURSOR cur1;

//printf("raszaktaxi\n");


//Формируем запрос к базе данных
zapros(strsql,data);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return;
 }

short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

iceb_u_rsdat(&dn,&mn,&gn,data->poi_zaktaxi.datan.ravno(),1);
iceb_u_rsdat(&dk,&mk,&gk,data->poi_zaktaxi.datak.ravno(),1);


sprintf(imaf,"vod%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

iceb_u_startfil(ff);
fprintf(ff,"\x0F"); //Ужатый режим
iceb_u_zagolov("Список заказов",dn,mn,gn,dk,mk,gk,"",ff);

char imaf_1[30];
FILE *ff_1;
sprintf(imaf_1,"vodd%d.lst",getpid());

if((ff_1 = fopen(imaf_1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_1,"",errno,data->window);
  return;
 }

iceb_u_startfil(ff_1);
fprintf(ff_1,"\x0F"); //Ужатый режим
iceb_u_zagolov("Список заказов",dn,mn,gn,dk,mk,gk,"",ff_1);

fprintf(ff,"\
-----------------------------------------------------------------------------\n\
Дата и время заказа|      Фамилия                 |    Телефон    |   Адрес \n\
-----------------------------------------------------------------------------\n");

fprintf(ff_1,"\
--------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_1,"\
Код водит.|Код клиен.|Фамилия клиента     |Дата и время заказа|   Откуда           |     Куда           |  Сумма   |\n");

fprintf(ff_1,"\
--------------------------------------------------------------------------------------------------------------------\n");

char fio_kl[100];
double isuma=0.;
while(cur.read_cursor(&row) != 0)
 {
  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  if(lzaktaxi_prov_row(row,data) != 0)
   continue;
  memset(fio_kl,'\0',sizeof(fio_kl));
  sprintf(strsql,"select fio from Taxiklient where kod='%s'",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strncpy(fio_kl,row1[0],sizeof(fio_kl)-1);

  iceb_u_rsdat(&d,&m,&g,row[6],2);
  fprintf(ff,"%02d.%02d.%04d %s %-30.30s %-15s %s\n",
  d,m,g,row[9],row[3],row[4],row[10]);
  fprintf(ff_1,"%-10s %-10s %-20.20s %02d.%02d.%04d %s %-20.20s %-20.20s %10s\n",
  row[2],row[1],fio_kl,d,m,g,row[9],row[10],row[11],row[13]);
  isuma+=atof(row[13]);
  
 }
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff_1,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_1,"%105s:%10.2f\n","Итого",isuma);

iceb_podpis(ff,data->window);
fprintf(ff,"\x12"); //Нормальные буквы
fclose(ff);

iceb_podpis(ff_1,data->window);
fprintf(ff_1,"\x12"); //Нормальные буквы
fclose(ff_1);

iceb_u_spisok fil;
iceb_u_spisok nazv;

fil.plus(imaf);
nazv.plus("Список заказов форма 1");

fil.plus(imaf_1);
nazv.plus("Список заказов форма 2");

//printf("raszaktaxi-fine\n");

iceb_rabfil(&fil,&nazv,"",0,data->window);


}
/*********************************/
/*Формирование строки запроса    */
/**********************************/

void    zapros(char *strsql,class lzaktaxi_data *data)
{
char  bros[300];

if(data->metkapoi == 0)
 {
  gtk_widget_show(data->hboxradio); //Показать
/********
  gtk_widget_show(data->radiobutton0); //Показать
  gtk_widget_show(data->radiobutton1); //Показать
  gtk_widget_show(data->radiobutton2); //Показать
  gtk_widget_show(data->radiobutton3); //Показать
***********/
  if(data->metkarr == 0 ) //Только не завершенные
    sprintf(strsql,"select * from Taxizak where kz=0 order by datvz asc,vremvz asc");

  if(data->metkarr == 1) //Все
    sprintf(strsql,"select * from Taxizak order by datvz desc,vremvz desc");

  if(data->metkarr == 2) //За последние сутки
   {
    time_t vrem;
    time(&vrem);
//    struct tm *bf;
    vrem-=24*60*60;
//    bf=localtime(&vrem);

//    sprintf(strsql,"select * from Taxizak where datz >= '%d-%d-%d' order by datvz desc,vremvz desc",
//    bf->tm_year+1900,bf->tm_mon+1,bf->tm_mday);

    sprintf(strsql,"select * from Taxizak where vrem >=%ld order by datvz desc,vremvz desc",
    vrem);
    printf("%s\n",strsql);          
   }

  if(data->metkarr == 3) //Первые 10
   {
    int kolzap=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(data->spin1));
    sprintf(strsql,"select * from Taxizak order by datvz desc,vremvz desc limit %d",kolzap);
   }
 }
else
 {

  gtk_widget_hide(data->hboxradio); //Скрываем от показа виджет
/**********
  gtk_widget_hide(data->radiobutton0); //Скрываем от показа виджет
  gtk_widget_hide(data->radiobutton1); //Скрываем от показа виджет
  gtk_widget_hide(data->radiobutton2); //Скрываем от показа виджет
  gtk_widget_hide(data->radiobutton3); //Скрываем от показа виджет
**********/ 
  sprintf(strsql,"select * from Taxizak ");
  memset(bros,'\0',sizeof(bros));  
  if(data->poi_zaktaxi.datan.getdlinna() > 1)
   {
    sprintf(bros,"where datvz >= '%s' ",data->poi_zaktaxi.datan.ravno_sqldata());
    strcat(strsql,bros);
    if(data->poi_zaktaxi.datak.getdlinna() > 1)
     {
      sprintf(bros,"and datvz <= '%s' ",data->poi_zaktaxi.datak.ravno_sqldata());
      strcat(strsql,bros);
     }
   }
  else
    if(data->poi_zaktaxi.datak.getdlinna() > 1)
     {
      sprintf(bros,"where datvz <= '%s' ",data->poi_zaktaxi.datak.ravno_sqldata());
      strcat(strsql,bros);
     }
  
/*******************
  if(data->poi_zaktaxi.datvz.getdlinna() > 1)
   {
    iceb_u_rsdat(&d,&m,&g,data->poi_zaktaxi.datvz.ravno(),1);
    sprintf(bros,"where datvz='%04d-%02d-%02d' ",g,m,d);
    strcat(strsql,bros);
    
   }
  if(data->poi_zaktaxi.vremvz.getdlinna() > 1)
   {
    iceb_u_rstime(&d,&m,&g,data->poi_zaktaxi.vremvz.ravno());
    if(bros[0] == '\0')
     sprintf(bros,"where vremvz='%02d:%02d:%02d' ",d,m,g);
    else
     sprintf(bros,"and vremvz='%02d:%02d:%02d' ",d,m,g);
    strcat(strsql,bros);
   }
********************/

  sprintf(bros,"order by datvz,vremvz desc");
  strcat(strsql,bros);
  
 } 
//printf("zapros-%s\n",strsql);
}


/**************************************************/
/****************************************************/
/**************************************************/
/****************************************************/
/**************************************************/
/****************************************************/
/*Завершение заказа*/
/*******************/

void      zavzak(class lzaktaxi_data *data)
{
//printf("metkavz=%d\n",data->metkavz);
if(data->metkavz == 0)
 return;
char  strsql[300];

data->zzwindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data->zzwindow),ICEB_POS_CENTER);

sprintf(strsql,"%s %s",NAME_SYSTEM,iceb_u_toutf("Завершение заказа"));
gtk_window_set_title(GTK_WINDOW(data->zzwindow),strsql);
gtk_signal_connect(GTK_OBJECT(data->zzwindow),"delete_event",GTK_SIGNAL_FUNC(zavzak_delete_event),data);
gtk_signal_connect_after(GTK_OBJECT(data->zzwindow),"key_press_event",GTK_SIGNAL_FUNC(zavzak_zaktaxi_key_press),data);

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data->zzwindow), vbox);

GtkWidget *label=gtk_label_new(iceb_u_toutf("Введите код завершения"));
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
//Создаем меню кодов завершения

GList *glist=NULL;
SQLCURSOR cur;
SQLCURSOR cur1;
SQL_str row;
sprintf(strsql,"select kod,naik from Taxikzz order by kod asc");
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);

iceb_u_spisok KZV;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %s",row[0],row[1]);
  KZV.plus(iceb_u_toutf(strsql));
 }
if(kolstr == 0)
  KZV.plus(iceb_u_toutf("Не введен список кодов завершения !!!"));

data->kod_zav.new_plus("");

if(data->rowv[0][0] != '\0')
 {
  sprintf(strsql,"select naik from Taxikzz where kod=%s",data->rowv[0]);
  if(sql_readkey(&bd,strsql,&row,&cur1) == 1)
   {
    sprintf(strsql,"%s %s",data->rowv[0],row[0]);
    data->kod_zav.new_plus(strsql);
   }
 }
//printf("zapzak-%s/%s\n",data->rowv[0],data->zap_zaktaxi.kodzav);

data->kodzav=gtk_combo_new();


for(int i=0 ; i < kolstr; i++)
  glist=g_list_append(glist,(void*)KZV.ravno(i));

gtk_combo_set_popdown_strings(GTK_COMBO(data->kodzav),glist);
gtk_combo_disable_activate(GTK_COMBO(data->kodzav));
gtk_box_pack_start (GTK_BOX (vbox), data->kodzav, TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (GTK_COMBO(data->kodzav)->entry), "activate",GTK_SIGNAL_FUNC(kodzav_get1),data);
gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(data->kodzav)->entry),data->kod_zav.ravno_toutf());

gtk_container_add (GTK_CONTAINER (vbox), hbox);
GtkWidget *knopka[2];

sprintf(strsql,"F2 %s",gettext("Запись"));
knopka[0]=gtk_button_new_with_label(strsql);
GtkTooltips *tooltips0=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips0,knopka[0],gettext("Запись введенной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(knopka[0]),"clicked",GTK_SIGNAL_FUNC(zavzak_zaktaxi_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(knopka[0]),(gpointer)"2");

sprintf(strsql,"F10 %s",gettext("Выход"));
knopka[1]=gtk_button_new_with_label(strsql);
GtkTooltips *tooltips1=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips1,knopka[1],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(knopka[1]),"clicked",GTK_SIGNAL_FUNC(zavzak_zaktaxi_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(knopka[1]),(gpointer)"10");

gtk_box_pack_start(GTK_BOX(hbox), knopka[0], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hbox), knopka[1], TRUE, TRUE, 0);

gtk_widget_grab_focus(data->kodzav);

gtk_widget_show_all(data->zzwindow);

}
/*********************************/
/*Обработчик сигнала delete_event*/
/*********************************/
gboolean zavzak_delete_event(GtkWidget *widget,GdkEvent *event,class lzaktaxi_data *data)
{
//printf("zavzak_delete_event\n");
gtk_widget_destroy(widget);
data->zzwindow=NULL;
return(FALSE);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zavzak_zaktaxi_knopka(GtkWidget *widget,class lzaktaxi_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("vzaktaxi_knopka knop=%s\n",knop);

switch (knop)
 {
  case 2:
    gtk_signal_emit_by_name(GTK_OBJECT(GTK_COMBO(data->kodzav)->entry),"activate");
    if(zapkz(data) != 0)
      return;
    gtk_widget_destroy(data->zzwindow);
    data->zzwindow=NULL;
    zapzaktaxi(data);
    return;  

  case 10:
    gtk_widget_destroy(data->zzwindow);
    data->zzwindow=NULL;
    zapzaktaxi(data);
    return;
 }
}
 
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zavzak_zaktaxi_key_press(GtkWidget *widget,GdkEventKey *event,class lzaktaxi_data *data)
{
//char  bros[300];

switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(GTK_COMBO(data->kodzav)->entry),"activate");
    if(zapkz(data) != 0)
      return(TRUE);
      
    gtk_widget_destroy(widget);
    data->zzwindow=NULL;
    zapzaktaxi(data);
    return(FALSE);
    
  case GDK_Escape:
  case GDK_F10:
    gtk_widget_destroy(widget);
    data->zzwindow=NULL;

    zapzaktaxi(data);

    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
       
 }

return(TRUE);
}
/*************************************/
/*Запись отметки завершения          */
/*************************************/
int  zapkz(class lzaktaxi_data *data)
{

char	strsql[512];
char    kodz[112];

time_t vrem;
time_t vremz;
time(&vrem);
vremz=vrem;

memset(kodz,'\0',sizeof(kodz));
if(data->kod_zav.getdlinna() > 1)
 {
  if(iceb_u_polen(data->kod_zav.ravno(),kodz,sizeof(kodz),1,' ') != 0)
    strncpy(kodz,data->kod_zav.ravno(),sizeof(kodz)-1);
  //Проверяем код завершения

  sprintf(strsql,"select kod from Taxikzz where kod=%s",kodz);
  if(sql_readkey(&bd,strsql) != 1)
   {
    sprintf(strsql,"Не найден код завершения %s !",kodz);
    iceb_menu_soob(strsql,NULL);
    return(1);
   }
 }
else
 vremz=0; 
 
sprintf(strsql,"update Taxizak \
set kz=%d,\
vremzz=%ld,\
vrem=%ld,\
ktoi=%d \
where \
kz=%s and kodk='%s' and kv='%s' and fio='%s' and telef='%s' and kolp=%s and datvz='%s' \
and vremvz='%s' and datz='%s' and vremz='%s' and ktoi=%s and vrem=%s",\
atoi(kodz),\
vremz,\
vrem,\
iceb_getuid(data->window),
data->rowv[0],\
data->rowv[1],\
data->rowv[2],\
data->rowv[3],\
data->rowv[4],\
data->rowv[5],\
data->rowv[6],\
data->rowv[7],\
data->rowv[8],\
data->rowv[9],\
data->rowv[15],\
data->rowv[16]);

//printf("zapkz-%s\n",strsql);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_u_str SP;
   SP.plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&SP,NULL);
   return(1);
  }
 else
  {
   iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,NULL);
   return(1);
  }
 }
return(0);
}
/*************************************/
/*Обработчик сигнала выбора кода завершения*/
/************************************/
void  kodzav_get1(GtkWidget *widget,class lzaktaxi_data *data)
{
char bros[300];
char strsql[300];
SQL_str row;
SQLCURSOR cur;
char  kod[100];

//printf("kodzav_get**********************************\n");

strcpy(kod,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

  data->kod_zav.new_plus(kod);
  if(data->kod_zav.getdlinna() <= 1)
   return;

if(data->metkapvk == 1)
 return;

if(kod[0] == '\0')
 return;

memset(bros,'\0',sizeof(bros));
if(iceb_u_polen(kod,bros,sizeof(bros),1,' ') != 0)
  strncpy(bros,kod,sizeof(bros)-1);

sprintf(strsql,"select naik from Taxikzz where kod=%s",bros);
//printf("%s\n",strsql);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"Нет такого кода завершения !!!");
  repl.plus(strsql);
  iceb_menu_soob(&repl,NULL);
  return; 
 }

sprintf(kod,"%s %s",bros,row[0]);

gtk_entry_set_text(GTK_ENTRY(widget),iceb_u_toutf(kod));
gtk_entry_select_region(GTK_ENTRY(widget),0,strlen(kod));

}
