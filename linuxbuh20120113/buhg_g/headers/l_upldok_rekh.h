/*$Id:*/
/*12.03.2008	12.03.2008	Белых А.И.	l_upldok_rekh.h
Реквизиты шапки путевого листа
*/


class l_upldok_rekh
 {
  public:
  class iceb_u_str data_dok;
  class iceb_u_str kod_pod;
  class iceb_u_str nomdok;
  class iceb_u_str kod_avt;
  class iceb_u_str kod_vod;       /*Код водителя*/
  class iceb_u_str ps_viezd;      /*Показания спидометра при выезде*/
  class iceb_u_str ps_vozv;       /*Показания спидометра при возвращении*/
  class iceb_u_str probeg_po_gor; /*Пробег по городу*/
  class iceb_u_str nst_po_gor;    /*норма списания топлива по городу*/
  class iceb_u_str vtpn_po_gor;   /*затраты топлива по норме по городу*/
  class iceb_u_str vtf_po_gor;    /*Затраты топлива фактические по городу*/
  class iceb_u_str probeg_za_gor; /*пробег за городом*/
  class iceb_u_str nst_za_gor;    /*норма списания топлива за городом*/
  class iceb_u_str vtpn_za_gor;   /*затраты топлива по норме за городом*/
  class iceb_u_str vtf_za_gor;    /*затраты топлива фактические за городом*/

  class iceb_u_str data_viezd;   /*дата выезда*/
  class iceb_u_str vrem_viezd;   /*время выезда*/
  class iceb_u_str data_vozv;    /*дата возврата*/
  class iceb_u_str vrem_vozv;    /*время возврата*/
  class iceb_u_str osob_otm;     /*особые отметки*/
  class iceb_u_str nbso;         /*Номер бланка суровой отчетности*/    

  /*страница 1*/
  class iceb_u_str ves_gruza;    /*вес груза*/
  class iceb_u_str tono_kil;     /*тоно-километры*/
  class iceb_u_str nzt_na_gruz;  /*норма затрат топлива на перевозку груза*/
  class iceb_u_str ztf_na_gruz;  /*затраты топлива фактические на перевозку груза*/
  class iceb_u_str vr_dvig;      /*время работы двигателя*/
  class iceb_u_str nzt_vr_dvig;  /*норма затрат на время работы двигателя*/
  class iceb_u_str ztf_vr_dvig;  /*затраты фактические на время работы двигателя*/

  void clear()
   {
    data_dok.new_plus("");
    kod_pod.new_plus("");
    nomdok.new_plus("");
    kod_avt.new_plus("");
    kod_vod.new_plus("");
    ps_viezd.new_plus("");
    ps_vozv.new_plus("");
    probeg_po_gor.new_plus("");
    nst_po_gor.new_plus("");
    vtpn_po_gor.new_plus("");
    vtf_po_gor.new_plus("");
    probeg_za_gor.new_plus("");
    nst_za_gor.new_plus("");
    vtpn_za_gor.new_plus("");
    vtf_za_gor.new_plus("");
    
    ves_gruza.new_plus("");
    tono_kil.new_plus("");
    nzt_na_gruz.new_plus("");
    ztf_na_gruz.new_plus("");
    vr_dvig.new_plus("");
    nzt_vr_dvig.new_plus("");
    ztf_vr_dvig.new_plus("");
    
    data_viezd.new_plus("");
    vrem_viezd.new_plus("");
    data_vozv.new_plus("");
    vrem_vozv.new_plus("");
    osob_otm.new_plus("");
    nbso.new_plus("");
   }

 };
