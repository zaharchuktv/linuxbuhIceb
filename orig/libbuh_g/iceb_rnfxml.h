/*$Id: iceb_rnfxml.h,v 1.1 2011-08-29 07:13:46 sasa Exp $*/
/*21.05.2008	12.10.2006	Белых А.И.	iceb_rnfxml.h
Реквизиты шапки документов в xml формате
*/

class iceb_rnfxml_data
 {
  public:
  short dt,mt,gt;
  char kod_edrpuo[112];
  char kod_oblasti[112];
  char kod_admin_r[112];
  char naim_kontr[512];
  char adres[512];
  char innn[56];
  char nspnds[56];
  char kod_dpi[56];
  char kod_tabl[56];
  char telefon[512];
    
  int kod_gr_voz_tar; //Код группы возвратная тара  
    
  iceb_rnfxml_data()
   {
    memset(kod_edrpuo,'\0',sizeof(kod_edrpuo));
    memset(kod_oblasti,'\0',sizeof(kod_oblasti));
    memset(kod_admin_r,'\0',sizeof(kod_admin_r));
    memset(naim_kontr,'\0',sizeof(naim_kontr));
    memset(kod_tabl,'\0',sizeof(kod_tabl));
    memset(adres,'\0',sizeof(adres));
    memset(innn,'\0',sizeof(innn));
    memset(nspnds,'\0',sizeof(nspnds));
    memset(kod_dpi,'\0',sizeof(kod_dpi));
    memset(telefon,'\0',sizeof(telefon));
    iceb_u_poltekdat(&dt,&mt,&gt);   
    kod_gr_voz_tar=0;
   }
 };
