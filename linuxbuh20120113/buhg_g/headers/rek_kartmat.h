/*$Id: rek_kartmat.h,v 1.6 2011-01-13 13:49:52 sasa Exp $*/
/*03.04.2006	07.06.2004	Белых А.И.	rek_kartmat.h
Реквизиты карточки материалла
*/
class rek_kartmat
 {
  public:
  
  //Реквизиты которые не меняются
  iceb_u_str skl;
  iceb_u_str n_kart;
  iceb_u_str kodm;
  
  //Реквизиты которые могут меняться
  iceb_u_str mnds;  //0-цена без НДС 1-цена с НДС
  iceb_u_str ei;
  iceb_u_str shet;
  iceb_u_str cena;
  iceb_u_str cenapr;
  iceb_u_str krat;
  iceb_u_str nds;
  iceb_u_str fas;
  iceb_u_str kodtar;
  iceb_u_str datv;
  iceb_u_str innom;
  iceb_u_str rnd;
  iceb_u_str nomzak;
  iceb_u_str data_kon_is; //Дата конечного использования
  
  //Не относится к реквизитам карточки
  iceb_u_str naimat;
  iceb_u_str naimskl;
  iceb_u_str kodgr;
  iceb_u_str naimgr;
  
 };
