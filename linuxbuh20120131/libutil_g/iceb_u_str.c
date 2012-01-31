/*$Id: iceb_u_str.c,v 1.87 2011-08-29 07:13:49 sasa Exp $*/
/*12.08.2011	03.12.2003	Белых А.И.	iceb_u_str.c
Подпрограмма динамического создания и увеличения размера строки 
*/
#include <glib.h>
#include	<stdlib.h>
#include        "iceb_util.h"

/****************************/
/*Конструктор объекта       */
/****************************/
iceb_u_str::iceb_u_str()
{
//printf("Конструктор\n");
stroka=NULL;
stroka_filtr=NULL;  //Всегда определяется перед запросом копированием с фильтрацией из "stroka"
dlinna=0;
}

/****************************/
/*Конструктор объекта  с записью нулевого байта */
/****************************/
iceb_u_str::iceb_u_str(const char *zapis)
{
 
//printf("Конструктор\n");
stroka=NULL;
stroka_filtr=NULL;  //Всегда определяется перед запросом копированием с фильтрацией из "stroka"
dlinna=0;

plus(zapis);
}

/****************************/
/*Деструктор класса*/
/****************************/
iceb_u_str::~iceb_u_str()
{
 if(stroka != NULL)
  {
   free(stroka);
   stroka=NULL;
  }
 if(stroka_filtr != NULL)
  {
   free(stroka_filtr);
   stroka_filtr=NULL;
  }
 stroka=stroka_filtr=NULL;
 dlinna=0;
}

/********************************/
/*Очистка от введеной информации*/
/********************************/
void iceb_u_str::free_class(void)
{
 if(stroka != NULL)
  {
   free(stroka);
   stroka=NULL; 
  }
 if(stroka_filtr != NULL)
  {
   free(stroka_filtr);
   stroka_filtr=NULL;
  }
 stroka=stroka_filtr=NULL;
 dlinna=0;
}
/***********************************/
/*проверить правильность ввода даты (д.м.г)*/
/************************************/
//если вернули 0-всё правильно 1-нет
int iceb_u_str::prov_dat()
{
if(dlinna <= 1)
 return(0);
short d,m,g;
return(iceb_u_rsdat(&d,&m,&g,stroka,1));
}
/***********************************/
/*проверить правильность ввода времени (ч.м.с)*/
/************************************/
//если вернули 0-всё правильно 1-нет
int iceb_u_str::prov_time()
{
if(dlinna <= 1)
 return(0);
short h,m,s;
return(iceb_u_rstime(&h,&m,&s,stroka));
}
/***********************************/
/*проверить правильность ввода даты (м.г)*/
/************************************/
//если вернули 0-всё правильно 1-нет
int iceb_u_str::prov_dat1()
{
if(dlinna <= 1)
 return(0);
short m,g;
return(iceb_u_rsdat1(&m,&g,stroka));
}


/*************************************************************************/
/*Записать дату преобразовав её из SQL формата в нормальное представление*/
/*************************************************************************/
void iceb_u_str::plus_sqldata(const char *row)
{
short d,m,g;
char bros[64];
memset(bros,'\0',sizeof(bros));

iceb_u_rsdat(&d,&m,&g,row,2);

if(d != 0)
  sprintf(bros,"%02d.%02d.%04d",d,m,g);

plus(bros);

}
/*************************************************************************/
/*Записать дату в символьном виде*/
/*************************************************************************/
void iceb_u_str::plus_data(short d,short m,short g)
{
char bros[32];
sprintf(bros,"%02d.%02d.%d",d,m,g);
plus(bros);

}
/*************************************************************************/
/*Записать только указанное количество символов*/
/*************************************************************************/
int iceb_u_str::plus(const char *str,int kol_simv)
{
int dlina=strlen(str)+1;
if(dlina < kol_simv)
 dlina=kol_simv+1;

char *bros=new char[dlina];

memset(bros,'\0',dlina);
//strncpy(bros,str,kol_simv);
sprintf(bros,"%.*s",iceb_u_kolbait(kol_simv,str),str);
int voz=plus(bros);
delete [] bros;
return(voz);
}
/*************************************************************************/
/*Записать только указанное количество символов*/
/*************************************************************************/
int iceb_u_str::new_plus(const char *str,int kol_simv)
{
int dlina=strlen(str)+1;
if(dlina < kol_simv)
 dlina=kol_simv+1;

char *bros=new char[dlina];

memset(bros,'\0',dlina);
sprintf(bros,"%.*s",iceb_u_kolbait(kol_simv,str),str);
int voz=new_plus(bros);
delete [] bros;
return(voz);
}
/*************************************************************************/
/*Записать две строки */
/*************************************************************************/
int iceb_u_str::plus(const char *pref,const char *str)
{
plus(pref);
return(plus(str));
}
/*************************************************************************/
/*Записать дату в символьном виде удалив предыдущюю строку*/
/*************************************************************************/
void iceb_u_str::new_plus_data(short d,short m,short g)
{
char bros[32];
sprintf(bros,"%02d.%02d.%d",d,m,g);
new_plus(bros);

}
/*************************************************************************/
/*Записать дату преобразовав её из SQL формата в нормальное представление*/
/*************************************************************************/
void iceb_u_str::new_plus_sqldata(const char *row)
 {
  free_class();
  plus_sqldata(row);
 }
/*****************************/
/*Eсли первый символ "+" вернуть 1, если "-" вернуть 2, иначе возвращаем 0*/
/****************************************/

int iceb_u_str::ravno_pr()
{
if(stroka[0] == '+')
 return(1);
if(stroka[0] == '-')
 return(2);
return(0); 
}
 


/********************************/
/*Добавить с увеличением размера*/
/********************************/

int iceb_u_str::plus(const char *strokaplus)
{
short metkan=0;

if(strokaplus == NULL)
 return(0);
 
if(dlinna != 0)
  metkan=1;

if(stroka == NULL)
 dlinna+=strlen(strokaplus)+1;
else
 dlinna+=strlen(strokaplus);

if((stroka=(char *)realloc(stroka,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-Не могу добавить память для stroka !!!\n",__FUNCTION__);
  return(-1);
 }  

if(metkan == 0)
  strcpy(stroka,strokaplus);
else
  strcat(stroka,strokaplus);

return(0);

}
/******************************/

/********************************/
/*Добавить с увеличением размера*/
/********************************/

int iceb_u_str::plus(double suma)
{
char strokaplus[112];
sprintf(strokaplus,"%.10g",suma);

return(plus(strokaplus));
}
/********************************/
/*Удалить предыдущее Добавить с увеличением размера*/
/********************************/

int iceb_u_str::new_plus(double suma)
{
char strokaplus[112];
sprintf(strokaplus,"%.10g",suma);

return(new_plus(strokaplus));

}
/********************************/
/*Добавить с увеличением размера*/
/********************************/

int iceb_u_str::plus(int suma)
{
char strokaplus[112];
sprintf(strokaplus,"%d",suma);


return(plus(strokaplus));

}
/********************************/
/*Удалить предыдущее Добавить с увеличением размера*/
/********************************/

int iceb_u_str::new_plus(int suma)
{
char strokaplus[112];
sprintf(strokaplus,"%d",suma);

return(new_plus(strokaplus));

}
/********************************/
/*Добавить с увеличением размера и пероводом строки в конце*/
/********************************/

int iceb_u_str::plus_ps(const char *strokaplus)
{
short metkan=0;

if(strokaplus == NULL)
 return(0);

if(dlinna != 0)
  metkan=1;

if(stroka == NULL)
 dlinna+=strlen(strokaplus)+2;
else
 dlinna+=strlen(strokaplus)+1;

if((stroka=(char *)realloc(stroka,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-Не могу добавить память для stroka !!!\n",__FUNCTION__);
  return(-1);
 }  

if(metkan == 0)
 {
  strcpy(stroka,strokaplus);
  strcat(stroka,"\n");
 }
else
 {
  strcat(stroka,strokaplus);
  strcat(stroka,"\n");
 }

return(0);

}
/********************************/
/*Добавить с увеличением размера и пероводом строки в начале*/
/********************************/

int iceb_u_str::ps_plus(int strokaplus)
{
char bros[112];
sprintf(bros,"%d",strokaplus);
return(ps_plus(bros));
}
/********************************/
/*Добавить с увеличением размера и пероводом строки в начале*/
/********************************/

int iceb_u_str::ps_plus(const char *strokaplus)
{
short metkan=0;

if(strokaplus == NULL)
 return(0);

if(dlinna != 0)
  metkan=1;

if(stroka == NULL)
 dlinna+=strlen(strokaplus)+2;
else
 dlinna+=strlen(strokaplus)+1;

if((stroka=(char *)realloc(stroka,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-Не могу добавить память для stroka !!!\n",__FUNCTION__);
  return(-1);
 }  

if(metkan == 0)
 {
  strcpy(stroka,"\n");
  strcat(stroka,strokaplus);
 }
else
 {
  strcat(stroka,"\n");
  strcat(stroka,strokaplus);
 }

return(0);

}
/********************************/
/*Записать с предварительной вставкой перевода строки все записи кроме первой*/
/********************************/

int iceb_u_str::plus_v_spisok(const char *strokaplus)
{
if(dlinna <= 1)
 return(new_plus(strokaplus));
else 
 {
  plus("\n");
  return(plus(strokaplus));
 }
return(1);
}
/**************************************************/
/*Записать с освобождением ранее выделенной памяти*/
/**************************************************/

int iceb_u_str::new_plus(const char *strokaplus)
{
//printf("iceb_u_str::new_plus %s\n",strokaplus);

if(strokaplus == NULL)
 return(0);

if(stroka != NULL)
 {
  free(stroka);
  stroka=NULL;
 }


dlinna=strlen(strokaplus)+1;

if((stroka=(char *)realloc(stroka,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-Не могу выделить память для stroka !!!\n",__FUNCTION__);
  return(-1);
 }  
//memset(stroka,'\0',sizeof(stroka));
strcpy(stroka,strokaplus);

return(0);

}
/**************************************************/
/*Записать с освобождением ранее выделенной памяти и добавлением перевода строки в конце*/
/**************************************************/

int iceb_u_str::new_plus_ps(const char *strokaplus)
{
//printf("iceb_u_str::new_plus %s\n",strokaplus);

if(strokaplus == NULL)
 return(0);

if(stroka != NULL)
 {
  free(stroka);
  stroka=NULL;
 }


dlinna=strlen(strokaplus)+2;

if((stroka=(char *)realloc(stroka,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-Не могу добавить память для stroka !!!\n",__FUNCTION__);
  return(-1);
 }  
//memset(stroka,'\0',sizeof(stroka));
strcpy(stroka,strokaplus);
strcat(stroka,"\n");

return(0);

}
/*************************/
/*Вернуть адрес строки   */
/*************************/

const char *iceb_u_str::ravno()
{
return(stroka);
}
/****************************/
/*вернуть строчку с переводом строки в конце*/
/*******************************************/
const char *iceb_u_str::ravno_ps()
{

if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }



if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(stroka)+4))) == NULL)
 {
  printf("\n%s-Не могу выделть память для stroka_filtr !!!\n",__FUNCTION__);
  return(NULL);
 }  
//memset(stroka_filtr,'\0',sizeof(stroka_filtr));
strcpy(stroka_filtr,stroka);
strcat(stroka_filtr,"\n");


return(stroka_filtr);
}
/***************************************************/
/*Получить строку преобразованную в маленькие буквы*/
/**************************************************/
const char *iceb_u_str::ravno_tolower()
{
if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(stroka)+1))) == NULL)
 {
  printf("\n%s-Не могу выделить память для stroka_filtr !!!\n",__FUNCTION__);
  return(NULL);
 }  

strcpy(stroka_filtr,stroka);
iceb_u_tolower_str(stroka_filtr);
return(stroka_filtr);

}
/**************************************************/
/*Получить строку перобразованную в большие буквы*/
/**************************************************/
const char *iceb_u_str::ravno_toupper()
{
if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(stroka)+1))) == NULL)
 {
  printf("\n%s-Не могу выделить память для stroka_filtr !!!\n",__FUNCTION__);
  return(NULL);
 }  

strcpy(stroka_filtr,stroka);
iceb_u_toupper_str(stroka_filtr);
return(stroka_filtr);

}
/*********************************/
/*получить строку для xml файла*/
/********************************/
const char *iceb_u_str::ravno_filtr_xml()
{
if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }
int dlinna=strlen(iceb_u_filtr_xml(stroka))+1;

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-Не могу выделить память для stroka_filtr !!!\n",__FUNCTION__);
  return(NULL);
 }  

strcpy(stroka_filtr,iceb_u_filtr_xml(stroka));

return(stroka_filtr);

}

/*************************/
/*Вернуть адрес отфильтрованной строки   */
/*************************/

const char *iceb_u_str::ravno_filtr()
{

char *prom_str=new char[dlinna*2+1];

int i1=0;
for(int i=0; i<dlinna ; i++)
 {
  switch (stroka[i])
   {
    case '\'':
     prom_str[i1]='\'';
     i1++;
     prom_str[i1]=stroka[i];
     break;
    case '\\':
     prom_str[i1]='\\';
     i1++;
     prom_str[i1]=stroka[i];
     break;

    default:
     prom_str[i1]=stroka[i];
     break;    
   }
  
  i1++;
 }

prom_str[i1]='\0';


if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }



if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(prom_str)+1))) == NULL)
 {
  printf("\n%s-Не могу выделть память для stroka_filtr !!!\n",__FUNCTION__);
  delete [] prom_str;
  return(NULL);
 }  
//memset(stroka_filtr,'\0',sizeof(stroka_filtr));
strcpy(stroka_filtr,prom_str);
delete [] prom_str;


return(stroka_filtr);

}

/*************************/
/*Вернуть адрес отфильтрованной строки до первого пробела  */
/*************************/

const char *iceb_u_str::ravno_filtr_dpp()
{

char *prom_str=new char[dlinna*2+1];
char *prom_strv=new char[dlinna*2+1];
//memset(prom_strv,'\0',sizeof(prom_strv));
int i1=0;
for(int i=0; i<dlinna ; i++)
 {
  switch (stroka[i])
   {
    case '\'':
     prom_str[i1]='\'';
     i1++;
     prom_str[i1]=stroka[i];
     break;

    default:
     prom_str[i1]=stroka[i];
     break;    
   }
  
  i1++;
 }

prom_str[i1]='\0';
if(iceb_u_pole(prom_str,prom_strv,1,' ') != 0)
 strcpy(prom_strv,prom_str);

delete [] prom_str;

if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }



if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(prom_strv)+1))) == NULL)
 {
  printf("\n%s-Не могу выделить память для stroka_filtr !!!\n",__FUNCTION__);
  delete [] prom_strv;
  return(NULL);
 }  
//memset(stroka_filtr,'\0',sizeof(stroka_filtr));
strcpy(stroka_filtr,prom_strv);

delete [] prom_strv;

return(stroka_filtr);

}
/*************************/
/*Вернуть строки до первого пробела  */
/*************************/

const char *iceb_u_str::ravno_dpp()
{

char *prom_str=new char[strlen(stroka)+1];

prom_str[0]='\0';
if(iceb_u_pole(stroka,prom_str,1,' ') != 0)
 strcpy(prom_str,stroka);

if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }



if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(prom_str)+1))) == NULL)
 {
  printf("\n%s-Не могу выделить память для stroka_filtr !!!\n",__FUNCTION__);
  delete [] prom_str;
  return(NULL);
 }  
//memset(stroka_filtr,'\0',sizeof(stroka_filtr));
strcpy(stroka_filtr,prom_str);

delete [] prom_str;

return(stroka_filtr);

}

/*************************/
/*Вернуть адрес строки перекодированной в UTF-8  */
/*************************/

const char *iceb_u_str::ravno_toutf()
{

int  dlinna_prom=(strlen(stroka)*sizeof(wchar_t))+1;


if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,(sizeof(char)*(dlinna_prom)))) == NULL)
 {
  printf("\n%s-Не могу выделить память для stroka_filtr !!!\n",__FUNCTION__);
  return(NULL);
 }  

strncpy(stroka_filtr,iceb_u_toutf(stroka),dlinna_prom);

return(stroka_filtr);
}
/*************************/
/*Вернуть адрес строки перекодированной из UTF-8 в KOI-8 */
/*************************/

const char *iceb_u_str::ravno_fromutf()
{

int  dlinna_prom=strlen(stroka)+1;


if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,(dlinna_prom))) == NULL)
 {
  printf("\n%s-Не могу выделить память для stroka_filtr !!!\n",__FUNCTION__);
  return(NULL);
 }  

strcpy(stroka_filtr,iceb_u_fromutf(stroka));

return(stroka_filtr);

}
/*************************/
/*Вернуть адрес строки заданной длинны перекодированной в UTF-8  */
/*************************/

const char *iceb_u_str::ravno_toutf(int dlinna)
{

int razmer_prom=strlen(stroka)+1;

char *prom=new char[razmer_prom];

memset(prom,'\0',razmer_prom);

if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

/*Если данные будут в koi8u то для преобразования их в utf8 нужно больше памяти*/
int razmer=sizeof(wchar_t)*strlen(stroka)+1;
if((stroka_filtr=(char *)realloc(stroka_filtr,razmer)) == NULL)
 {
  printf("\n%s-Не могу выделить память для stroka_filtr !!!\n",__FUNCTION__);
  delete [] prom;
  return(NULL);
 }  
memset(stroka_filtr,'\0',razmer);
if(g_utf8_validate(stroka,-1,NULL) == TRUE) /*текст в utf8 кодировке*/
  g_utf8_strncpy(prom,stroka,dlinna);
else
  strncpy(prom,stroka,dlinna);

/*Делать имеено так*/
strcpy(stroka_filtr,iceb_u_toutf(prom));
delete [] prom;
return(stroka_filtr);

}

/****************************************/
/*Получить количество символов в массиве*/
/****************************************/
int iceb_u_str::getdlinna()
{
return(dlinna);
}
/**********************************/
/*Получить в виде double*/
/***************************/
double iceb_u_str::ravno_atof()
{
return(iceb_u_atof(stroka));
}
/**********************************/
/*Получить в виде int*/
/***************************/
int iceb_u_str::ravno_atoi()
{
return(atoi(stroka));
}

/**********************************/
/*Получить год если введена дата*/
/***************************/
short iceb_u_str::ravno_god()
{
short d,m,g;
iceb_u_rsdat(&d,&m,&g,stroka,1);
return(g);
}
/**********************************/
/*Получить дату в формате мм.гг*/
/***************************/
const char *iceb_u_str::ravno_mes_god()
{
short d,m,g;
iceb_u_rsdat(&d,&m,&g,stroka,1);
static char bros[64];
sprintf(bros,"%02d.%04d",m,g);
return(bros);
}
/**********************************/
/*Получить в виде long int*/
/***************************/
long int iceb_u_str::ravno_atol()
{
return(atol(stroka));
}

/*************************/
/*Вернуть дату в формате SQL DATA */
/*************************/

const char *iceb_u_str::ravno_sqldata()
{


if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*15)) == NULL)
 {
  printf("\n%s-Не могу выделить память для stroka_filtr !!!\n",__FUNCTION__);
  return(NULL);
 }  
short d,m,g;

iceb_u_rsdat(&d,&m,&g,stroka,1);
sprintf(stroka_filtr,"%04d-%02d-%02d",g,m,d);

return(stroka_filtr);

}
/*************************/
/*Вернуть дату в формате ддммгггг*/
/*************************/

const char *iceb_u_str::ravno_xmldata()
{


if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*15)) == NULL)
 {
  printf("\n%s-Не могу выделить память для stroka_filtr !!!\n",__FUNCTION__);
  return(NULL);
 }  
short d,m,g;

iceb_u_rsdat(&d,&m,&g,stroka,1);
sprintf(stroka_filtr,"%02d%02d%04d",d,m,g);

return(stroka_filtr);

}

/*************************/
/*Вернуть врумя в формате час:минута:секунда*/
/*************************/

const char *iceb_u_str::ravno_time()
{


if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*15)) == NULL)
 {
  printf("\n%s-Не могу выделить память для stroka_filtr !!!\n",__FUNCTION__);
  return(NULL);
 }  
short hh,mm,ss;

iceb_u_rstime(&hh,&mm,&ss,stroka);
sprintf(stroka_filtr,"%02d:%02d:%02d",hh,mm,ss);

return(stroka_filtr);

}
/*************************/
/*Вернуть врумя в формате час:минута:секунда  Если строка пустая то выдать 24:00:00*/
/*************************/

const char *iceb_u_str::ravno_time_end()
{


if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*15)) == NULL)
 {
  printf("\n%s-Не могу выделить память для stroka_filtr !!!\n",__FUNCTION__);
  return(NULL);
 }  
short hh,mm,ss;

iceb_u_rstime(&hh,&mm,&ss,stroka);
if(stroka[0] == '\0')
  sprintf(stroka_filtr,"23:59:59");
else
  sprintf(stroka_filtr,"%02d:%02d:%02d",hh,mm,ss);

return(stroka_filtr);

}
/********************************/
/*Добавить с увеличением размера и вставкой запятой если запись не первая*/
/********************************/

int iceb_u_str::z_plus(const char *strokaplus)
{
short metkan=0;

if(strokaplus == NULL)
 return(0);

if(dlinna > 1)
  metkan=1;

dlinna+=strlen(strokaplus)+1; /*прибавляем 1 для нулевого байта если строки нет и для запятой если строка есть*/

if((stroka=(char *)realloc(stroka,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-Не могу добавить память для stroka !!!\n",__FUNCTION__);
  return(-1);
 }  

if(metkan == 0)
 {
  strcpy(stroka,strokaplus);
 }
else
 {
  strcat(stroka,",");
  strcat(stroka,strokaplus);
 }

return(0);

}
/********************************/
/*Добавить с увеличением размера и вставкой запятой если запись не первая*/
/********************************/

int iceb_u_str::z_plus(int strokaplus)
{
char str[124];
sprintf(str,"%d",strokaplus);
return(z_plus(str));
}
/******************************/
/*Записать текущюю дату*/
/************************/
void iceb_u_str::plus_tek_dat()
{

short d,m,g;
iceb_u_poltekdat(&d,&m,&g);
char strok[1024];
sprintf(strok,"%d.%d.%d",d,m,g);

plus(strok);

}
/******************************/
/*Записать текущюю дату удалив предыдущюю информацию если она была введена*/
/************************/
void iceb_u_str::new_plus_tek_dat()
{

short d,m,g;
iceb_u_poltekdat(&d,&m,&g);
char strok[1024];
sprintf(strok,"%d.%d.%d",d,m,g);

new_plus(strok);

}
/************************/
/*записать текущюю дату в формате д.м.г*/
/*************************/
void iceb_u_str::poltekdat()
{
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
char bros[56];
sprintf(bros,"%d.%d.%d",dt,mt,gt);
plus(bros);
}
/************************/
/*записать текущюю дату в формате м.г */
/*************************/
void iceb_u_str::poltekdat1()
{
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
char bros[56];
sprintf(bros,"%d.%d",mt,gt);
plus(bros);
}
/************************/
/*выдать количество символов в строке*/
/*************************/
int iceb_u_str::kolih_simv(const char simvol)
{
int kolih_simv=0;

for(int ii=0; ii < (int)strlen(stroka); ii++)
 {
  if(stroka[ii] == simvol)
   kolih_simv++;
 }
return(kolih_simv);
}

/************************/
/*выдать количество символов в строке*/
/*************************/
const char *iceb_u_str::ravno_udsimv(const char *strsimv)
{
if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }



if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(stroka)+1))) == NULL)
 {
  printf("\n%s-Не могу выделить память для stroka_filtr !!!\n",__FUNCTION__);
  return(NULL);
 }  

strcpy(stroka_filtr,stroka);
iceb_u_ud_simv(stroka_filtr,strsimv);
return(stroka_filtr);


}

