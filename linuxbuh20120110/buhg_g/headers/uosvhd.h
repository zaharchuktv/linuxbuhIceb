/*$Id: uosvhd.h,v 1.4 2011-08-29 07:13:44 sasa Exp $*/
/*10.09.2011	21.10.2007	Белых А.И.	uosvhd.h
Реквизиты для записи шапки документа
*/

class uosvhd_data
 {
  public:
    class iceb_u_str datadok_z;
    class iceb_u_str nomdok_z;
    class iceb_u_str nomdok_post_z;  //Номер парного документа запомненный
    class iceb_u_str podr_z;
    class iceb_u_str mat_ot_z;
    class iceb_u_str podr_v_z;
    class iceb_u_str mat_ot_v_z;
    class iceb_u_str kontr_z; //код контрагента запомненный
    class iceb_u_str nomnalnak_z; //Номер налоговой накладной запомненный
            
    class iceb_u_str datadok; //Дата документа
    class iceb_u_str nomdok;  //номер документа
    class iceb_u_str podr;   //Код подразделения
    class iceb_u_str podr_v; //Код подразделения в парном документе
    class iceb_u_str mat_ot; //код материально ответственного
    class iceb_u_str mat_ot_v; //материально-ответсвенный в парном документе
    class iceb_u_str kontr;   //код контрагента
    class iceb_u_str nomdok_post; //Номер документа поставщика или парного документа
    class iceb_u_str osnov;  //Основание 
    class iceb_u_str dover; //Доверенность
    class iceb_u_str data_vd; //Дата выдачи доверенности
    class iceb_u_str sherez;  //Через кого
    class iceb_u_str nomnalnak; //Номер налоговой накладной
    class iceb_u_str data_vnn;  //Дата выдачи/получения налоговой накладной
    class iceb_u_str data_opl;  //Дата оплаты
    class iceb_u_str forop;     //код формы оплаты
    class iceb_u_str dvnn; /*Дата выдачи налоговой накладной*/
    class iceb_u_str vid_dog; /*Вид договора*/
    int lnds;                //метка ндс
    float pnds; //действующий процент НДС
    class iceb_u_str kod_op; //код операции

    short tipz; //1-приход 2-расход для основного документа
    short tipzv; //1-приход 2-расход для парного документа
    short mdd;  /*0-обычный документ 1-двойной*/

  uosvhd_data()
   {
    tipzv=tipz=mdd=0;
    clear();
    datadok_z.plus("");
    nomdok_z.plus("");
    nomdok_post_z.plus("");
    podr_z.plus("");
    mat_ot_z.plus("");
    kontr_z.plus("");
    podr_z.plus("");
    mat_ot_z.plus("");
    nomnalnak_z.plus("");
    podr_v_z.plus("");
    mat_ot_v_z.plus("");
    
   }

  void clear()
   {
    pnds=0.;
    lnds=0;
    datadok.new_plus("");
    nomdok.new_plus("");

    podr.new_plus("");
    podr_v.new_plus("");

    mat_ot.new_plus("");
    mat_ot_v.new_plus("");

    kontr.new_plus("");
    nomdok_post.new_plus("");
    osnov.new_plus("");
    dover.new_plus("");
    data_vd.new_plus("");
    sherez.new_plus("");
    nomnalnak.new_plus("");
    data_vnn.new_plus("");
    data_opl.new_plus("");
    forop.new_plus("");
    kod_op.new_plus("");
    dvnn.new_plus("");
    vid_dog.new_plus("");    
   }
 };
