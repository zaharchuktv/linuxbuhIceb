/*$Id: lzaktaxi.h,v 1.25 2011-01-13 13:50:09 sasa Exp $*/
/*14.03.2005	10.10.2003	Белых А.И.	lzaktaxi.h
*/
//struct zaktaxi_rek
class zaktaxi_rek
 {
  public:
  iceb_u_str kodk; //Код клиента
  iceb_u_str kv;     // код водителя - читается вместе с фамилией
  iceb_u_str fio;    // фамилия заказчика
  iceb_u_str telef;  // телефон клиента
  iceb_u_str kolp;    //количество пассажиров
  iceb_u_str adreso;//  адрес отуда забрать клиента
  iceb_u_str adresk;//  адрес куда отвезти
  iceb_u_str koment;//  коментарий
  iceb_u_str suma;   // сумма
  iceb_u_str kodzav; //Код завершения

  //Реквизиты только в меню ввода записи
  iceb_u_str datvz;  // дата когда нужно выполнить заказ
  iceb_u_str vremvz;  //время когда нужно выполнить заказ

  //Реквизиты только в меню поиска записей
  iceb_u_str datan;
  iceb_u_str datak;
  iceb_u_str kod_operatora;
  
  void clear_data()
   {
    kodk.new_plus("");
    kv.new_plus("");
    fio.new_plus("");
    telef.new_plus("");
    kolp.new_plus("");
    adreso.new_plus("");
    adresk.new_plus("");
    koment.new_plus("");
    suma.new_plus("");
    kodzav.new_plus("");
    datvz.new_plus("");
    vremvz.new_plus("");
    datan.new_plus("");
    datak.new_plus("");
    kod_operatora.new_plus("");
   }
 };


#define KOLENTRY 11

class lzaktaxi_data
 {
  public:
  //Выбранная запись
  SQL_str  rowv;  
  int      metkavz; //0-запись не выбрана 1-выбрана
  int      metkarr; //0-только не завершенные заказы 1-все
  int      kolzap;  //Количество отображаемых записей
  
  gint      timer_read;
  GtkWidget *spin;
  GtkWidget *spin1;
  GtkWidget *window;
  GtkWidget *label;  //Строка реквизитов поиска
  GtkWidget *knopka[10];
  GtkWidget *list;
  GtkWidget *scrolled_window;
  GtkWidget *zzwindow;  //Окно для завершения заказа
  GtkWidget *hboxradio; 
  GtkWidget *radiobutton0; //Не завершенные заказы
  GtkWidget *radiobutton1; //Все заказы
  GtkWidget *radiobutton2; //Сутки
  GtkWidget *radiobutton3; //Первые 10 заказов
  GtkWidget *labelkz;      //Количество записей
  GtkWidget *entry[KOLENTRY];
  GtkWidget *kodzav;
  GdkColor color_blue;
  GdkColor color_red;

  int       metkazapisi; //0-новая запись 1-корректировка
  int       metkavrp;    //Метка ввода реквизитов поиска 0-ввод новой записи 1- ввод реквизитов поиска
  int       metkapvk;    //Метка проверки введенных кодов 0-не проверять 1-проверять
  SQLCURSOR cur;
  int       kl_shift; //0-отжата 1-нажата  

   class iceb_u_str kod_zav; //для ввода кода завершения в отдельном меню
   
  //Реквизиты поиска
  zaktaxi_rek  poi_zaktaxi;
  int       metkapoi; //0-без поиска 1-с поиском


  //Конструктор    
  lzaktaxi_data()
   {
    timer_read=0;
    metkapoi=metkapvk=metkavrp=metkarr=metkavz=0;
    kl_shift=0;
    metkapoi=0;
    metkazapisi=0;    
    zzwindow=NULL;
    kod_zav.plus("");
//    cur=new SQLCURSOR(0);
   }

  //Деструктор
  ~lzaktaxi_data()
   {
//    printf("~lzaktaxi_data\n");
    
    if(zzwindow != NULL)
      gtk_widget_destroy(zzwindow);


   }
 };

void       udzzaktaxi(class lzaktaxi_data *);
//int        zzaktaxi(class lzaktaxi_data *);
void       zapzaktaxi(class lzaktaxi_data *);
gboolean   lzaktaxi_key_press(GtkWidget *,GdkEventKey *,class lzaktaxi_data *);
void       lzaktaxi_knopka(GtkWidget *,class lzaktaxi_data *);
gboolean   vzaktaxi_delete_event(GtkWidget *,GdkEvent *,class lzaktaxi_data *);
//gboolean   vzaktaxi_key_press(GtkWidget *,GdkEventKey *,class lzaktaxi_data *);
// void       vzaktaxi_knopka(GtkWidget *,class lzaktaxi_data *);
void       lzaktaxiv(class lzaktaxi_data*);
void       lzaktaxi_unselection_row( GtkWidget *,gint,gint,GdkEventButton *,class lzaktaxi_data*);
void       lzaktaxi_selection_row( GtkWidget *,gint,gint,GdkEventButton*,class lzaktaxi_data*);
int        lzaktaxi_prov_row(SQL_str,class lzaktaxi_data *);
void	   raszaktaxi(class lzaktaxi_data *);
//void       voditel_get(GtkWidget*,class lzaktaxi_data*);
//void       kodzav_get(GtkWidget *,class lzaktaxi_data *);
//void       kodkl_get(GtkWidget *,class lzaktaxi_data *);
void       zapros(char *,class lzaktaxi_data *);
gboolean   zavzak_delete_event(GtkWidget *,GdkEvent *,class lzaktaxi_data *);
void       zavzak(class lzaktaxi_data *);
void       zavzak_zaktaxi_knopka(GtkWidget *,class lzaktaxi_data *);
gboolean   zavzak_zaktaxi_key_press(GtkWidget *,GdkEventKey *,class lzaktaxi_data *);
int        zapkz(class lzaktaxi_data *);
void       lzaktaxi_knopaos(GtkWidget *,class lzaktaxi_data *);

