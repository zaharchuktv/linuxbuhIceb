/*$Id: l_restdok2.h,v 1.10 2011-01-13 13:50:09 sasa Exp $*/
/*30.10.2007	21.03.2006	Белых А.И.	l_restdok2.h
*/
enum
{
 GR_FK_VVERH,
 GR_FK_VNIZ,
 KOL_GR_FK
};

enum
{
 MU_FK_VIBOR,
 MU_FK_VVERH,
 MU_FK_VNIZ,
 MU_FK_OSTATOK,
 KOL_MU_FK
};

enum
{
  FK2,
  SFK2,
  FK3,
  FK5,
  FK6,
  FK7,
  FK8,
  FK9,
  SFK9,
  FK10,
  F_SKLAD,
  F_PODR,
  KOL_F_KL
};


enum
{
 COL_SKL,
 COL_MZ,
 COL_KOD,
 COL_MR,
 COL_NAIM,
 COL_EI,
 COL_KOLIH,
 COL_CENA,
 COL_PROC_SK,
 COL_CENA_SK,
 COL_SUMA,
 COL_KOMENT,
 COL_SPISANIE,
 COL_VREM,
 COL_KTO,
 NUM_COLUMNS
};

class  restdok2_data_sp
 {
  public:
  GtkWidget *sw;      
  GtkWidget *treeview;
  int snanomer;
  int kolzap;
  short metka_released;  //0-отжата 1-нажата
  
  restdok2_data_sp()
   {
    metka_released=0;
    kolzap=0;
    snanomer=0;
    treeview=NULL;
   }
 };
 
class  restdok2_data
 {
  public:
 
  short       dd,md,gd; //дата документа
  iceb_u_str  nomdok; //номер документа
  

  iceb_u_str  nstol; // номер столика
  iceb_u_str  kodkl; //код клиента
  iceb_u_str  fio;
  iceb_u_str  koment;
  short       mo;    //0-не оплачено 1-оплачено
  int         podr;  //код подразделения в котороом выписывавается счёт
  iceb_u_str  naimpodr;
  time_t      vremz;
  double suma_k_oplate;
  double saldo; 
          
  int         kodn;  //код новой записи

  int         sklv; //код склада в строке документа
  int         kodv; //код товара/усдуги в строке документа
  int         mzv;  /*мента товара/услуги*/
  int         nz;  //номер записи
  
  int  kod_grupv;  //код выбранной в списке группы материалла или услуги
  int  kod_matus;  //код выбранного в списке материала или услуги
  
  short metka_mat_usl; //0-показывать списки для выбора материала 1- для выбора услуг  
  short metka_show; //0-списки групп и материалов/услуг не показывать 1-материалов 2-услуг
  short metka_kontr_ost; //0-остаток не контролировать 1-контролировать
  GtkWidget *label_mat_usl;  //Записывается какие списки показаны - материалов или услуг
  GtkWidget *label_kolstr;
  GtkWidget *label_naim_skl;
  GtkWidget *label_naim_podr;
  GtkWidget *sw;         //список записей в документе
  GtkWidget *treeview;

  GtkWidget *gor_panel; //горизонтальная панель для размещения списка групп и материалов/услуг
  GtkWidget *vert_panel; //горизонтальная панель для разделения списков выбора и списка записей в документе
  short metka_hapki;  //0-показывать шапку 1-не показывать
  class restdok2_data_sp spisok_gr;
  
  class restdok2_data_sp spisok_mu;

  GtkWidget *windowvoditel;
  GtkWidget *widget_podr;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_gr[KOL_GR_FK];
  GtkWidget *knopka_mu[KOL_MU_FK];
  
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       voz;  
  
  class iceb_u_str skladv; //Код склада выбранного для поиска товаров для выбора
  iceb_u_str podrv;
  float      proc_sk; //Процент скидки
  
  //Конструктор
  restdok2_data()
   {
    metka_kontr_ost=0;
    metka_hapki=1;
    metka_show=0;
    metka_mat_usl=0;
    skladv.new_plus("");
    kodn=0;
    voz=0;
    snanomer=0;
    kl_shift=0;
    window=NULL;
    treeview=NULL;
  
    podrv.plus("");
   }      
 };

void l_restdok2_setstr(class restdok2_data_sp *data);
void l_restdok2_vmu(class restdok2_data *data);
