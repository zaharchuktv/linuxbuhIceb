/*$Id: iceb_u_spisok.c,v 1.24 2011-01-13 13:50:05 sasa Exp $*/
/*13.02.2010	06.06.2002	Белых А.И.	iceb_u_spisok.c
Работа со списками
*/
#include        <stdio.h>
#include <stdlib.h>
#include        <ctype.h>
#include        "iceb_util.h"

/**************************/
/*Создать список*/
/*************************/
void iceb_u_spisok::soz_spisok(int mr) //0-добавлять в конец списка 1- в начало
{
 metka=mr;  
 //Пустой "запредельный узел"
 zag=kon=new STROKA("12345678",7,0);
 kon->next=NULL;
// stroka_filtr=NULL;
}

/*******************************/
/*Конструктор*/
/*******************************/
iceb_u_spisok::iceb_u_spisok(int mr) //0-добавлять в конец списка 1- в начало
{
soz_spisok(mr); 
}  

/*******************************/
/*Конструктор без параметров*/
/*******************************/
iceb_u_spisok::iceb_u_spisok()
{
soz_spisok(0); 
}  

/*************************/
/*Деструктор*/
/*************************/
iceb_u_spisok::~iceb_u_spisok()
{
spisok_delete();
}    

/*********************************/
//Удалить спиок
/*********************************/
void iceb_u_spisok::spisok_delete()
{
STROKA *udal=zag;
for(STROKA *sn = zag; sn != kon;)
 {
  sn=sn->next;
  delete udal;
  udal=sn;
 }
delete udal;
zag=NULL;
kon=NULL;


}    
/***************************************************/
/*Создать новый список предварительно удалив старый*/
/***************************************************/
void iceb_u_spisok::free_class(void)
{
spisok_delete();
soz_spisok(0); 
}

/*********************************/
   //Добавить элемент в список 
/*обязательно полное повторение кода !!! иначе валится*/
/*********************************/
int iceb_u_spisok::plus(const char *masiv)
{
//printf("iceb_u_spisok plus %s\n",masiv);
  
STROKA *nov=new STROKA(masiv,strlen(masiv),0);
if(metka == 0)
 {
  kon->next=nov;      
  nov->next=NULL;
  kon=nov;
 }
if(metka == 1)
 {
  nov->next=zag;
  zag=nov;
 }
return(0);

}  

/*********************************/
//Добавить элемент в список с удалением предыдущего списка
/*обязательно полное повторение кода !!! иначе валится*/
/*********************************/
int iceb_u_spisok::new_plus(const char *masiv)
 {
  spisok_delete();
  zag=kon=new STROKA("12345678",7,0);


  STROKA *nov=new STROKA(masiv,strlen(masiv),0);
  if(metka == 0)
   {
    kon->next=nov;      
    nov->next=NULL;
    kon=nov;
   }
  if(metka == 1)
   {
    nov->next=zag;
    zag=nov;
   }
  return(0);
 }  


/*********************************/
//Добавить элемент в список 
/*обязательно полное повторение кода !!! иначе валится*/
/*******************************/
int iceb_u_spisok::plus(const char *masiv,int dlina)
{
if(dlina < 0)
 printf("%s-Отрицательная длинна !!!\n",__FUNCTION__);
 
STROKA *nov=new STROKA(masiv,dlina,0);
if(metka == 0)
 {
  kon->next=nov;      
  nov->next=NULL;
  kon=nov;
 }
if(metka == 1)
 {
  nov->next=zag;
  zag=nov;
 }
return(0);
}  

/****************************/
//Добавить элемент в список 
/*обязательно полное повторение кода !!! иначе валится*/
/****************************/
int iceb_u_spisok::plus(int dlina)
 {
  char masiv[8];
  memset(masiv,'\0',sizeof(masiv));
  STROKA *nov=new STROKA(masiv,dlina,1);
  if(metka == 0)
   {
    kon->next=nov;      
    nov->next=NULL;
    kon=nov;
   }
  if(metka == 1)
   {
    nov->next=zag;
    zag=nov;
   }
  return(0);
 }  

/**********************************/
   //Вернуть адрес элемента из списка
/***********************************/
const char *iceb_u_spisok::ravno(int nom)
 {
 //     printw("\niceb_u_spisok_return номер=%d\n",nom);
//     refresh();
  int nomer=0;
  STROKA *sn=NULL; 

  if(metka == 0)
    sn=zag->next;
  if(metka == 1)
    sn=zag;

  if(sn == NULL)
   {
    return(NULL);
   }

  for( ; ; )
   {
    if(nomer++ == nom)
     {
      return(sn->ukstr);
     }      

    if(sn == kon)
      break;

    sn=sn->next;
   }
  return(NULL);
 }

/****************************/
 //Вернуть количество элементов в списке
/***************************/
int iceb_u_spisok::kolih(void)
 {
  int kol=0;
  for(STROKA *sn = zag; sn != kon;sn=sn->next)
    kol++;
  return(kol);
 }

/*******************************/
//Найти в списке нужную строку
/*****************************/
//Если вернули -1 значит не нашли
int iceb_u_spisok::find(const char *obr)
 {
  int i;
  int dlina=strlen(obr);
  int nomer=0;
  STROKA *sn1=NULL;       
  for(STROKA *sn = zag; sn != kon;sn=sn->next)
   {
    sn1=sn->next;
    
    if(dlina == (int)strlen(sn1->ukstr))
     {
      for(i=0; i < dlina ; i++)
       if(sn1->ukstr[i] != obr[i])
        break;

      if(i == dlina)
       return(nomer);
     }
    nomer++;
   }
  return(-1);
 }

/*******************************/
//Найти в списке нужную строку
/*****************************/
//Если вернули -1 значит не нашли
int iceb_u_spisok::find(const char *obr,int metka) //0-полное сравнение 1-начальных символов
 {
//  int i;
//  int dlina=strlen(obr);
  int nomer=0;
  STROKA *sn1;       
  for(STROKA *sn = zag; sn != kon;sn=sn->next)
   {
    sn1=sn->next;

    if(metka == 1 && sn1->ukstr[0] == '\0')
     return(nomer);

    if(iceb_u_SRAV(obr,sn1->ukstr,metka) == 0)
      return(nomer);
/*******************    
    if(dlina == strlen(sn1->ukstr))
     {
      for(i=0; i < dlina ; i++)
       if(sn1->ukstr[i] != obr[i])
        break;

      if(i == dlina)
       return(nomer);
     }
***************/
    nomer++;
   }
  return(-1);
 }

/*******************************/
/*Найти в списке нужную строку без учета регистра с полным сравнением*/
/*****************************/
//Если вернули -1 значит не нашли
int iceb_u_spisok::find_r(const char *obr)
 {
  int i;

  int dlina=strlen(obr);
  int nomer=0;
  STROKA *sn1;       
  
  for(STROKA *sn = zag; sn != kon;sn=sn->next)
   {
    sn1=sn->next;
    int dlina0=strlen(sn1->ukstr);
    
    if(dlina == dlina0)
     {
      char obr1[dlina*2+1];
      char obr2[dlina0*2+1];
      strcpy(obr2,sn1->ukstr);
      strcpy(obr1,obr);

      iceb_u_toupper_str(obr1);
      iceb_u_toupper_str(obr2);

      for(i=0; i < dlina ; i++)
       if(obr2[i] != obr1[i])
        break;

      if(i == dlina)
       return(nomer);
     }
    nomer++;
   }
  return(-1);
 }

/*******************************/
//Найти в списке нужную строку без учета регистра с полным или неполным сравнением
/*****************************/
//Если вернули -1 значит не нашли
int iceb_u_spisok::find_r(const char *obr,int metka)
 {

  int dlina=strlen(obr);
  int nomer=0;
  STROKA *sn1;       
  
  for(STROKA *sn = zag; sn != kon;sn=sn->next)
   {
    sn1=sn->next;
    int dlina0=strlen(sn1->ukstr);
    
    if(dlina == dlina0)
     {
      char obr1[dlina*2+1];
      char obr2[dlina0*2+1];
      strcpy(obr2,sn1->ukstr);
      strcpy(obr1,obr);

      iceb_u_toupper_str(obr1);
      iceb_u_toupper_str(obr2);

      if(iceb_u_SRAV(obr1,obr2,metka) == 0)
       return(nomer);
     }
    nomer++;
   }
  return(-1);
 }

/***************************************************/
/*Получить елемент списка перекодированным в UTF-8*/
/***************************************************/
const char *iceb_u_spisok::ravno_toutf(int nom)
{
/**************
if(stroka_filtr != NULL)
 {
  delete stroka_filtr;
  stroka_filtr=NULL;
 }

int dlinna=strlen(ravno(nom))+1;
stroka_filtr=new char[dlinna*sizeof(wchar_t)];
strcpy(stroka_filtr,iceb_u_toutf(ravno(nom)));
return(stroka_filtr);
******************/
if(nom >= kolih())
 return("");
class STROKA *ss=(STROKA*)return_all(nom);
if(ss->str_filtr != NULL)
  delete [] ss->str_filtr;

int razmer=strlen(ss->ukstr)+1;

ss->str_filtr=new char[razmer*sizeof(wchar_t)];

strcpy(ss->str_filtr,iceb_u_toutf(ss->ukstr));


return(ss->str_filtr);

}
/*****************************/
/*Записать данные в уже существующий елемент списка с удалением его предыдущего содержимого*/
/*************************************/
/*Если вернули 0-записали 1-нет*/
int iceb_u_spisok::new_plus(int nomer,const char *znah)
{
if(nomer >= kolih())
 return(1);

class STROKA *ss=(STROKA*)return_all(nomer);

delete [] ss->ukstr;
int razmer=strlen(znah)+1;
ss->kolbait=razmer;
ss->ukstr=new char[razmer];
memset(ss->ukstr,'\0',razmer);

if(znah[0] != '\0') //Только так
  strcpy(ss->ukstr,znah);  


return(0);
}
/*****************************/
/*Добавить к существующей строке*/
/*************************************/
/*Если вернули 0-записали 1-нет*/
int iceb_u_spisok::plus(int nomer,const char *znah)
{
if(nomer >= kolih())
 return(1);

class STROKA *ss=(STROKA*)return_all(nomer);

int dlina=strlen(znah)+ss->kolbait+1;

if((ss->ukstr=(char *)realloc(ss->ukstr,sizeof(char)*dlina)) == NULL)
 {
  printf("\n%s-Не могу перераспределить память !!!\n",__FUNCTION__);
  return(-1);
 }  
ss->kolbait=dlina;
strcat(ss->ukstr,znah);
return(0);
}

/***************************/
/*Удалить елемент из списка*/
/***************************/

int iceb_u_spisok::del(int nomer)
{
/*Необходимо помнить что имеется первая техническая запись*/
/*поэтому номер удаляемой строки будет на единицу больше*/
class STROKA *pred_zap=NULL;
int nom=0;
class STROKA *sn;
for(sn = zag; sn != kon;)
 {
  if(nom < nomer+1) /*Запоминаем предыдущюю запись*/
   {
    pred_zap=sn;
   }
  if(nom == nomer+1) /*Удаляем запись*/
   {
    /*переключаем ссылку на следующюю запись*/
    pred_zap->next=sn->next;
    delete sn;
    return(0);
   }
  sn=sn->next;
  nom++;
 }
/*Устанавливаем последнюю запись*/
if(nom == nomer+1)
 {
  pred_zap->next=NULL;
  kon=pred_zap;
  delete sn;
 } 
return(1);
}
/******************/
/*Вернуть адрес записи всего класса*/
/**************************/
void *iceb_u_spisok::return_all(int nom)
{
  int nomer=0;
  STROKA *sn=NULL; 

  if(metka == 0)
    sn=zag->next;
  if(metka == 1)
    sn=zag;

  if(sn == NULL)
   {
    return(NULL);
   }

  for( ; ; )
   {
    if(nomer++ == nom)
     {
      return(sn);
     }      

    if(sn == kon)
      break;

    sn=sn->next;
   }
  return(NULL);
}
/**************************/
/*Вернуть чило челого типа*/
/*************************/
int iceb_u_spisok::ravno_atoi(int nomer)
{
return(atoi(ravno(nomer)));
}