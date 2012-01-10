/*$Id: zag_nn_xmlw.c,v 1.6 2011-08-29 07:13:44 sasa Exp $*/
/*23.07.2011	13.10.2006	Белых А.И.	zag_nn_xmlw.c
Заголовок налоговой накладной в формате xml.
*/
#include "buhg_g.h"

extern class iceb_rnfxml_data rek_zag_nn;



void zag_nn_xmlw(int tipz,
const char *nomdok,
short dnp,short mnp,short gnp,
short dd,short md,short gd,
const char *kontr,
const char *uslov_prod,
const char *forma_rash,
FILE *ff_xml,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
char naim_kontr[512];
char inn_kontr[32];
char adres_kontr[512];
char telef_kontr[512];
char nspnds[32];
class iceb_u_str vid_dog("");
class iceb_u_str data_dog("");
class iceb_u_str nomer_dog("");

memset(naim_kontr,'\0',sizeof(naim_kontr));
memset(adres_kontr,'\0',sizeof(adres_kontr));
memset(inn_kontr,'\0',sizeof(inn_kontr));
memset(telef_kontr,'\0',sizeof(telef_kontr));
memset(nspnds,'\0',sizeof(nspnds));

if(iceb_u_polen(uslov_prod,&vid_dog,1,':') != 0)
 vid_dog.new_plus(uslov_prod);
iceb_u_polen(uslov_prod,&data_dog,2,':');
iceb_u_polen(uslov_prod,&nomer_dog,3,':');

//читаем реквизиты контрагента
sprintf(strsql,"select naikon,adres,innn,nspnds,telef,pnaim from Kontragent where kodkon='%s'",kontr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(row[5][0] == '\0')
    strncpy(naim_kontr,row[0],sizeof(naim_kontr)-1);
  else
    strncpy(naim_kontr,row[5],sizeof(naim_kontr)-1);
  strncpy(adres_kontr,row[1],sizeof(adres_kontr)-1);
  strncpy(inn_kontr,row[2],sizeof(inn_kontr)-1);
  strncpy(nspnds,row[3],sizeof(nspnds)-1);
  strncpy(telef_kontr,row[4],sizeof(telef_kontr)-1);
 }

fprintf(ff_xml," <DECLARBODY>\n");

if(tipz == 1)
 {
  fprintf(ff_xml,"  <HORIG>1</HORIG>\n"); /*Видається покупцю*/
  if(dnp != 0)
    fprintf(ff_xml,"  <HFILL>%02d%02d%04d</HFILL>\n",dnp,mnp,gnp);
  else
    fprintf(ff_xml,"  <HFILL>%02d%02d%04d</HFILL>\n",dd,md,gd);
 }
if(tipz == 2)
 {
  fprintf(ff_xml,"  <HCOPY>1</HCOPY>\n"); /*Копия (залишається у продавця)*/

  fprintf(ff_xml,"  <HFILL>%02d%02d%04d</HFILL>\n",dd,md,gd);
 }
fprintf(ff_xml,"  <HNUM>%s</HNUM>\n",nomdok);

if(tipz == 1)
 {
  fprintf(ff_xml,"  <HNAMEBUY>%s</HNAMEBUY>\n",iceb_u_filtr_xml(naim_kontr));
  fprintf(ff_xml,"  <HNAMESEL>%s</HNAMESEL>\n",iceb_u_filtr_xml(rek_zag_nn.naim_kontr));

  if(inn_kontr[0] != '\0')
    fprintf(ff_xml,"  <HKBUY>%s</HKBUY>\n",inn_kontr);
  if(rek_zag_nn.innn[0] != '\0')
    fprintf(ff_xml,"  <HKSEL>%s</HKSEL>\n",rek_zag_nn.innn);

  if(adres_kontr[0] != '\0')
   fprintf(ff_xml,"  <HLOCBUY>%s</HLOCBUY>\n",iceb_u_filtr_xml(adres_kontr));
  if(rek_zag_nn.adres[0] != '\0')
   fprintf(ff_xml,"  <HLOCSEL>%s</HLOCSEL>\n",iceb_u_filtr_xml(rek_zag_nn.adres));

  if(telef_kontr[0] != '\0')    
   fprintf(ff_xml,"  <HTELBUY>%s</HTELBUY>\n",telef_kontr);
  if(rek_zag_nn.telefon[0] != '\0')
   fprintf(ff_xml,"  <HTELSEL>%s</HTELSEL>\n",rek_zag_nn.telefon);
  
  if(nspnds[0] != '\0')
   fprintf(ff_xml,"  <HNSPDVBUY>%s</HNSPDVBUY>\n",nspnds);
  if(rek_zag_nn.nspnds[0] != '\0')
    fprintf(ff_xml,"  <HNSPDVSEL>%s</HNSPDVSEL>\n",rek_zag_nn.nspnds);
 }

if(tipz == 2)
 {
  fprintf(ff_xml,"  <HNAMESEL>%s</HNAMESEL>\n",iceb_u_filtr_xml(rek_zag_nn.naim_kontr));
  fprintf(ff_xml,"  <HNAMEBUY>%s</HNAMEBUY>\n",iceb_u_filtr_xml(naim_kontr));

  if(rek_zag_nn.innn[0] != '\0')
   fprintf(ff_xml,"  <HKSEL>%s</HKSEL>\n",rek_zag_nn.innn);
  if(inn_kontr[0] != '\0')
   fprintf(ff_xml,"  <HKBUY>%s</HKBUY>\n",inn_kontr);

  if(rek_zag_nn.adres[0] != '\0')
   fprintf(ff_xml,"  <HLOCSEL>%s</HLOCSEL>\n",iceb_u_filtr_xml(rek_zag_nn.adres));
  if(adres_kontr[0] != '\0')
   fprintf(ff_xml,"  <HLOCBUY>%s</HLOCBUY>\n",iceb_u_filtr_xml(adres_kontr));

  if(rek_zag_nn.telefon[0] != '\0')
   fprintf(ff_xml,"  <HTELSEL>%s</HTELSEL>\n",rek_zag_nn.telefon);
  if(telef_kontr[0] != '\0')    
   fprintf(ff_xml,"  <HTELBUY>%s</HTELBUY>\n",telef_kontr);

  if(rek_zag_nn.nspnds[0] != '\0')
   fprintf(ff_xml,"  <HNSPDVSEL>%s</HNSPDVSEL>\n",rek_zag_nn.nspnds);
  if(nspnds[0] != '\0')
   fprintf(ff_xml,"  <HNSPDVBUY>%s</HNSPDVBUY>\n",nspnds);
 }


if(vid_dog.getdlinna() > 1) /*Вид цівілно-правового договору*/
  fprintf(ff_xml,"  <H01G1S>%s</H01G1S>\n",vid_dog.ravno_filtr());

if(data_dog.getdlinna() > 1) /*дата договора*/
  fprintf(ff_xml,"  <H01G2D>%s</H01G2D>\n",data_dog.ravno_xmldata());
if(nomer_dog.getdlinna() > 1) /*номер договора*/
  fprintf(ff_xml,"  <H01G3S>%s</H01G3S>\n",nomer_dog.ravno_filtr());

if(forma_rash[0] != '\0')
  fprintf(ff_xml,"  <H02G1S>%s</H02G1S>\n",iceb_u_filtr_xml(forma_rash));


}

