/*$Id: iceb_u_zvstr.c,v 1.8 2011-08-29 07:13:49 sasa Exp $*/
/*28.07.2011	15.04.2009	Белых А.И.	iceb_u_zvstr.c
Замена в строке одной цепочки символов другой
*/
#include "iceb_util.h"

int iceb_u_zvstr(class iceb_u_str *stroka,const char *obr,const char *zamena)
{
if(zamena[0] == '\0')
 return(1);
if(obr[0] == '\0')
 return(1);
if(stroka->getdlinna() <= 1)
 return(1); 

int dlina=stroka->getdlinna()+1;
int metka_zam=0;
char *stroka1=new char[dlina];
memset(stroka1,'\0',dlina);

class iceb_u_str strout;

int dlinna_prov=strlen(obr)+1;
char *prov=new char[dlinna_prov];

int nomer_str=0;
for(int ii=0; ii < stroka->getdlinna(); ii++)
 {
  if(stroka->ravno()[ii] == obr[0])
   {
   
    memset(prov,'\0',dlinna_prov);
    strncpy(prov,&stroka->ravno()[ii],dlinna_prov-1);
    if(iceb_u_SRAV(prov,obr,0) == 0)
     {
      metka_zam++;
      stroka1[nomer_str]='\0';
      strout.plus(stroka1);
      strout.plus(zamena);      
      memset(stroka1,'\0',dlina);
      nomer_str=0;
      ii+=strlen(obr)-1; /*пропускаем образец который заменён на новую цепочку символов уменьшаем на 1 так как в цикле прибавиться 1*/
      continue; /*делаем так чтобы проверка началась со следующего за образцом байта иначе он без проверки скопируется в stroka1*/
     }
    
   }
  stroka1[nomer_str++]=stroka->ravno()[ii];
 }
//stroka->new_plus(stroka1);
if(metka_zam == 0)
 {
  delete [] stroka1;
  delete [] prov;
  return(0);
 }
strout.plus(stroka1);
delete [] stroka1;
delete [] prov;
stroka->new_plus(strout.ravno());
return(0);
}
