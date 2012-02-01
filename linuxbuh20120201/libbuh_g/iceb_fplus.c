/*$Id: iceb_fplus.c,v 1.3 2011-01-13 13:49:59 sasa Exp $*/
/*13.12.2010	13.12.2010	Белых А.И.	iceb_fplus.c
Подпрограмма чтения продолжения строки из файла
*/
#include "iceb_libbuh.h"



void iceb_fplus(const char *stroka,class iceb_u_str *stroka1,class SQLCURSOR *cur)
{
SQL_str row_alx;
if(iceb_u_polen(stroka,stroka1,1,'\\') != 0)
 {
  stroka1->new_plus(stroka);
  return;
 }
for(;;)
 {

  if(cur->read_cursor(&row_alx) == 0)
   return;
//  if(fgets(strokk,sizeof(strokk),ff) == NULL)
//   return;

  if(row_alx[0][0] == '#')
   continue;

  class iceb_u_str strokk1("");
  if(iceb_u_polen(row_alx[0],&strokk1,1,'\\') != 0)
   {
    /*удаляем символ перевода строки*/
    int dlina=strlen(row_alx[0]);
    if(row_alx[0][dlina-1] == '\n')
     row_alx[0][dlina-1]='\0';
    stroka1->plus(row_alx[0]);
    return;
   }
  stroka1->plus(strokk1.ravno());
 }
}
/******************************/
void iceb_fplus(int metka_sp, /*0-разделённые запятыми в список 1-прибавить всю строку*/
const char *stroka,class iceb_u_spisok *nastr,class SQLCURSOR *cur)
{
class iceb_u_str s_nastr;
iceb_fplus(stroka,&s_nastr,cur);
if(metka_sp == 1)
 {
  nastr->plus(s_nastr.ravno());
  return;
 }
char sodp[1024];
int kolihp=iceb_u_pole2(s_nastr.ravno(),',');
if(kolihp == 0)
 {
  nastr->plus(stroka);
  return;  
 }
for(int ii=0; ii <= kolihp; ii++)
 {
  memset(sodp,'\0',sizeof(sodp));
  iceb_u_polen(s_nastr.ravno(),sodp,sizeof(sodp),ii+1,',');
  if(sodp[0] == '\0')
   continue;  
  nastr->plus(sodp);
 }
}
