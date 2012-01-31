/*$Id: iceb_u_fplus.c,v 1.6 2011-01-13 13:50:05 sasa Exp $*/
/*11.11.2008	28.11.2007	Белых А.И.	iceb_u_fplus.c
Подпрограмма чтения продолжения строки из файла
*/
#include "iceb_util.h"



void iceb_u_fplus(const char *stroka,class iceb_u_str *stroka1,FILE *ff)
{

if(iceb_u_polen(stroka,stroka1,1,'\\') != 0)
 {
  stroka1->new_plus(stroka);
  return;
 }
char strokk[2048];
for(;;)
 {

  memset(strokk,'\0',sizeof(strokk));
  if(fgets(strokk,sizeof(strokk),ff) == NULL)
   return;

  if(strokk[0] == '#')
   continue;

  class iceb_u_str strokk1("");
  if(iceb_u_polen(strokk,&strokk1,1,'\\') != 0)
   {
    /*удаляем символ перевода строки*/
    int dlina=strlen(strokk);
    if(strokk[dlina-1] == '\n')
     strokk[dlina-1]='\0';
    stroka1->plus(strokk);
    return;
   }
  stroka1->plus(strokk1.ravno());
 }
}
/******************************/
void iceb_u_fplus(int metka_sp, /*0-разделённые запятыми в список 1-прибавить всю строку*/
const char *stroka,class iceb_u_spisok *nastr,FILE *ff)
{
class iceb_u_str s_nastr;
iceb_u_fplus(stroka,&s_nastr,ff);
if(metka_sp == 1)
 {
  nastr->plus(s_nastr.ravno());
  return;
 }
char sodp[1024];
/*printw("iceb_u_fplus-%s %s\n",stroka,s_nastr.ravno());*/
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
