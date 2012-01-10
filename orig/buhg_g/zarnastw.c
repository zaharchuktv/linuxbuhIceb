/* $Id: zarnastw.c,v 1.24 2011-04-14 16:09:36 sasa Exp $ */
/*23.04.2010    21.06.2006      Белых А.И.      zarnastw.c
Читаем настройку на зарплату
*/
#include <stdlib.h>
#include        <errno.h>
#include        "buhg_g.h"
#include <unistd.h>


float  kof_prog_min=0.; //Коэффициент прожиточного минимума
float  ppn=0.; //Процент подоходного налога
float  pomzp=0.; //Процент от минимальной зароботной платы

double          okrg; /*Округление*/

short		metkarnb=0; //0-расчет налогов для бюджета пропорциональный 1-последовательный
short		avkvs=0; //0-Не вставлять автоматом код контрагента 1-вставлять
short           *knvr=NULL;/*Коды начислений не входящие в расчет подоходного налога*/
short           *kn_bsl=NULL; //Коды начислений на которые не распространяется льгота по подоходному налогу
short           *kuvr=NULL;/*Коды удержаний входящие в расчет подоходного налога*/
short           *knnf=NULL; //Коды начислений недежными формами 
short           *knnf_nds=NULL; /*Коды начислений неденежными формами на которые начисляется НДС*/
short           kodpn;   /*Код подоходного налога*/
short kodpn_sbol; /*Код подоходного с больничного*/
class iceb_u_str kodpn_all; /*Все коды подоходного налога*/
short kodvpn=0; /*Код возврата подоходного налога*/
short           kodperpn;   /*Код перерасчета подоходного налога*/
short		kodindex;  /*Код индексации*/
short		kodindexb;  /*Код индексации больничного*/

short		kodpenf; /*Код пенсионного фонда*/
short           kodpen;  /*Код пенсионных отчислений*/
short		*kodnvpen=NULL; /*Коды не входящие в расчет пенсионного отчисления*/
short           *kuvvr_pens_sr=NULL; //Коды удержаний входящие в расчет при отчислении с работника
short           *kuvvr_pens_sfz=NULL; //Коды удержаний входящие в расчет при отчислении с фонда зарплаты
float           procpen[2]; /*Процент отчисления в пенсионный фонд*/
char            *p_shet_inv=NULL; //счет для начисления пенсионных отчислений на фонд зарплаты для инвалидов (если для них нужен другой счет)

short		*kodnvrindex=NULL; //Коды не входящие в расчет индексации
float           p_tarif_inv=0.; //тариф (процент) отчисления в пенсионный фонд инвалидами

short		kodsoc;   /*Код фонда социального страхования*/
short           kodsocstr;  /*Код отчисления на соц-страх*/
float           procsoc=0.; /*Процент отчисления в соц-страх*/
float           procsoc1=0.; /*Процент отчисления в соц-страх*/
short		*kodsocstrnv=NULL; //Коды не входящие в расчет соцстраха
char		*shetsoc=NULL; //Счета фонда соц-страха не бюджет/бюджет

short		kodbezf;  /*Код фонда занятости*/
short		*kodbzrnv=NULL; //Коды не входящие в расчет отчислений на безработицу
short           kodbzr;  /*Код отчисления на безработицу*/
float           procbez; /*Процент отчисления на безработицу*/
float procent_fb_dog=0; /*Прочент начисления на фонд зарплаты для фонда безработицы для работающий по договору*/


short		kodsoci;   /*Код социального страхования инвалидов*/
short		kodfsons;   /*Код фонда страхования от несчасного случая*/

short           *kodmp=NULL;  //Коды благотворительной помощи
short           *kodbl=NULL;  /*Код начисления больничного*/
short		koddopmin; /*Код доплаты до минимальной зарплаты*/
short           *kodnvmin=NULL; /*Коды не входящие в расчет доплаты до минимальной зарплаты*/
short           *kodotrs=NULL;  /*Коды отдельного рачета доплаты до минимпльной зарплаты*/

short		*obud=NULL; /*Обязательные удержания*/
short		*obnah=NULL; /*Обязательные начисления*/
short		startgodz; /*Стаpтовый год для расчета сальдо*/
short		startgodb; /*Стартовый год просмотров главной книги*/

char		shrpz[16]; /*Счет расчетов по зарплате*/
char		shrpzbt[16]; /*Счет расчетов по зарплате бюджет*/
char		pktn[16]; /*Приставка к табельному номеру*/
class iceb_u_spisok dop_zar_sheta; //Дополнительные зарплатные счёта
class iceb_u_spisok dop_zar_sheta_spn; /*Списки начислений для каждого дополнительного зарплатного счёта*/

char		*shpn=NULL; /*Счет подоходного налога*/
char		*shetb=NULL; /*Бюджетные счета начислений*/
char		*shetbu=NULL; /*Бюджетные счета удержаний*/
char		*shpnb=NULL;   /*Счета подоходного налога*/
char		*shetps=NULL; /*Счета профсоюзного сбора*/
char		*shetfb=NULL; /*Счета фонда безработицы*/
char		*shetpf=NULL; /*Счета пенсионного фонда*/

short		kuprof=0; //Код удержания в профсоюзный фонд
float		procprof=0.; //Процент отчисления в профсоюзный фонд
short		*knvrprof=NULL; //Коды не входящие в расчет удержаний в профсоюзный фонд

double          snmpz=0.; /*Сумма не облагаемой материальной помощи заданная*/

short		pehfam=0; //0-фамилия в корешке мелко 1-крупно

short *kodkomand=NULL; //Коды командировочных расходов
short *knvr_komand=NULL; //Коды начислений не входящие в расчёт командировочных
short kodt_komand=0; //Код табеля командировочных
short kodt_b_komand=0; //Код бюджетного табеля командировочных
short *ktnvr_komand=NULL; //Коды табеля не входящие в расчёт командировочных
short koddzn=0; /*Код доплаты до средней зарплаты в командировке с учётом надбавки за награду*/
short *kvvrdzn=NULL; /*Коды входящие в расчёт доплаты до средней зарплаты в командировке с учётом надбавки за награду*/
short kod_komp_z_kom=0; /*Код компенсации за командировку*/

short kod_ud_dnev_zar=0; //Код удержания дневного зароботка
short *kodnah_nvr_dnev_zar=NULL;  //Коды начислений не входящие в расчёт удержания дневного зароботка
short *kodtab_nvr_dnev_zar=NULL;  //Коды табеля не входящие в расчёт удержания однодневного зароботка

int kekv_at=0;  //Код экономической классификации для атестованых
int kekv_nat=0; //Код экономической классификации для не атестованых
int kodzv_nat=0;//Код звания неатестованный
int kekv_nfo=0; //Код экономической классификации для начислений на фонд оплаты

short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
extern SQL_baza bd;

int zarnastw(void)
{
int		i;
char            str[1024];
char            bros[1024];

kod_komp_z_kom=0;
koddzn=0;
kodt_komand=kodt_b_komand=0;
kuprof=metkarnb=avkvs=0;
p_tarif_inv=0.;
snmpz=0.;
memset(shrpz,'\0',sizeof(shrpz));
memset(shrpzbt,'\0',sizeof(shrpzbt));
memset(pktn,'\0',sizeof(pktn));
kof_prog_min=ppn=pomzp=0.;
kekv_at=kekv_nat=kodzv_nat=kekv_nfo=0;


if(kodkomand != NULL)
 {
  delete [] kodkomand;
  kodkomand=NULL;
 }

if(kvvrdzn != NULL)
 {
  delete [] kvvrdzn;
  kvvrdzn=NULL;
 }

if(ktnvr_komand != NULL)
 {
  delete [] ktnvr_komand;
  ktnvr_komand=NULL;
 }

if(knvr_komand != NULL)
 {
  delete [] knvr_komand;
  knvr_komand=NULL;
 }

if(knvrprof != NULL)
 {
  delete [] knvrprof;
  knvrprof=NULL;
 }

if(kuvvr_pens_sfz != NULL)
 {
  delete [] kuvvr_pens_sfz;
  kuvvr_pens_sfz=NULL;
 }

if(kuvvr_pens_sr != NULL)
 {
  delete [] kuvvr_pens_sr;
  kuvvr_pens_sr=NULL;
 }

if(kodsocstrnv != NULL)
 {
  delete [] kodsocstrnv;
  kodsocstrnv=NULL;
 }

if(kodbzrnv != NULL)
 {
  delete [] kodbzrnv;
  kodbzrnv=NULL;
 }

if(kodnvrindex != NULL)
 {
  delete [] kodnvrindex;
  kodnvrindex=NULL;
 }

if(obud != NULL)
 {
  delete [] obud;
  obud=NULL;
 }
if(obnah != NULL)
 {
  delete [] obnah;
  obnah=NULL;
 }



if(knvr != NULL)
 {
  delete [] knvr;
  knvr=NULL;
 }
if(knnf != NULL)
 {
  delete [] knnf;
  knnf=NULL;
 }
if(knnf_nds != NULL)
 {
  delete [] knnf_nds;
  knnf_nds=NULL;
 }
if(kn_bsl != NULL)
 {
  delete [] kn_bsl;
  kn_bsl=NULL;
 }
if(kodnvpen != NULL)
 {
  delete [] kodnvpen;
  kodnvpen=NULL;
 }
if(kodnvmin != NULL)
 {
  delete [] kodnvmin;
  kodnvmin=NULL;
 }
if(kodotrs != NULL)
 {
  delete [] kodotrs;
  kodotrs=NULL;
 }

if(kuvr != NULL)
 {
  delete [] kuvr;
  kuvr=NULL;
 }

if(kodmp != NULL)
 {
  delete [] kodmp;
  kodmp=NULL;
 }


if(shetb != NULL)
 {
  delete [] shetb;
  shetb=NULL;
 }
if(shetbu != NULL)
 {
  delete [] shetbu;
  shetbu=NULL;
 }

if(shpnb != NULL)
 {
  delete [] shpnb;
  shpnb=NULL;
 }
if(shpn != NULL)
 {
  delete [] shpn;
  shpn=NULL;
 }

if(shetfb != NULL)
 {
  delete [] shetfb;
  shetfb=NULL;
 }
if(shetps != NULL)
 {
  delete [] shetps;
  shetps=NULL;
 }
if(shetpf != NULL)
 {
  delete [] shetpf;
  shetpf=NULL;
 }
if(shetsoc != NULL)
 {
  delete [] shetsoc;
  shetsoc=NULL;
 }
if(p_shet_inv != NULL)
 {
  delete [] p_shet_inv;
  p_shet_inv=NULL;
 }

if(kodbl != NULL)
 {
  delete [] kodbl;
  kodbl=NULL;
 }

if(kodnah_nvr_dnev_zar != NULL)
 {
  delete [] kodnah_nvr_dnev_zar;
  kodnah_nvr_dnev_zar=NULL;
 }
if(kodtab_nvr_dnev_zar != NULL)
 {
  delete [] kodtab_nvr_dnev_zar;
  kodtab_nvr_dnev_zar=NULL;
 }
dop_zar_sheta.free_class(); /*Обнуляем список*/
dop_zar_sheta_spn.free_class(); /*Обнуляем список*/

//nomin=0.;
procprof=procbez=procpen[0]=procpen[1]=procsoc=procsoc1=0.;
pehfam=koddopmin=startgodz=kodpen=kodbzr=kodsocstr=0;
kod_ud_dnev_zar=0;
okrg=0.;

kodindex=kodindexb=0;
kodfsons=kodsoci=kodsoc=kodbezf=kodpenf=kodperpn=vplsh=kodpn=kodvpn=0;
kodpn_sbol=0;
kodpn_all.new_plus("");
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zarnast.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  iceb_u_polen(row_alx[0],str,sizeof(str),1,'|');

  if(iceb_u_SRAV(str,"Код экономической классификации затрат для атестованных",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kekv_at=atoi(str);
    continue;
   }
  if(iceb_u_SRAV(str,"Код экономической классификации затрат для неатестованных",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kekv_nat=atoi(str);
    continue;
   }
  if(iceb_u_SRAV(str,"Код экономической классификации затрат для начислений на фонд оплаты",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kekv_nfo=atoi(str);
    continue;
   }
  if(iceb_u_SRAV(str,"Код звания для неатестованых",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kodzv_nat=atoi(str);
    continue;
   }

  if(iceb_u_SRAV(str,"Бюджетные счета",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((shetb=new char[i]) == NULL)
        printf("Не могу выделить пямять для shetb !\n");
       strcpy(shetb,bros);
      }
           
     continue;

   }
  if(iceb_u_SRAV(str,"Бюджетные счета удержаний",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((shetbu=new char[i]) == NULL)
        printf("Не могу выделить пямять для shetbu !\n");
       strcpy(shetbu,bros);
      }
     continue;

   }
  if(iceb_u_SRAV(str,"Код удержания дневного заработка",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kod_ud_dnev_zar=(short)iceb_u_atof(str); 
    continue;
   }
  if(iceb_u_SRAV(str,"Коды начислений не входящие в расчёт удержания дневного заработка",0) == 0)
   {
    iceb_sozmas(&kodnah_nvr_dnev_zar,row_alx[0],&cur_alx);
    continue;
   }
  if(iceb_u_SRAV(str,"Коды табеля не входящие в расчёт удержания дневного заработка",0) == 0)
   {
    iceb_sozmas(&kodtab_nvr_dnev_zar,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Счета подоходного налога",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((shpnb=new char[i]) == NULL)
        printf("Не могу выделить пямять для shpnb !\n");
       strcpy(shpnb,bros);
      }
     continue;

   }
  if(iceb_u_SRAV(str,"Счета пенсионного отчисления",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((shetpf=new char[i]) == NULL)
        printf("Не могу выделить пямять для shetpf !\n");
       strcpy(shetpf,bros);
      }
     continue;

   }
  if(iceb_u_SRAV(str,"Счета фонда социального страхования",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((shetsoc=new char[i]) == NULL)
        printf("Не могу выделить пямять для shetsoc !\n");
       strcpy(shetsoc,bros);
      }
     continue;

   }

  if(iceb_u_SRAV(str,"Счет отчисления в пенсионный фонд инвалидами (фонд зарплаты)",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((p_shet_inv=new char[i]) == NULL)
        printf("Не могу выделить пямять для shetsoc !\n");
       strcpy(p_shet_inv,bros);
      }
     continue;

   }
  if(iceb_u_SRAV(str,"Дополнительные зарплатные счёта",0) == 0)
   {
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     iceb_fplus(0,bros,&dop_zar_sheta,&cur_alx);
     continue;
   }

  if(iceb_u_SRAV(str,"Счета отчисления в фонд безработицы",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((shetfb=new char[i]) == NULL)
        printf("Не могу выделить пямять для shetfb !\n");
       strcpy(shetfb,bros);
      }
     continue;

   }

  if(iceb_u_SRAV(str,"Счета профсоюзного отчисления",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((shetps=new char[i]) == NULL)
        printf("Не могу выделить пямять для shetps !\n");
       strcpy(shetps,bros);
      }
     continue;

   }

  if(iceb_u_SRAV(str,"Коды, которые нужно взять в расчёт для расчёта доплаты до средней зарплаты в командировке",0) == 0)
   {
    iceb_sozmas(&kvvrdzn,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Код доплаты до средней зарплаты в командировке c учётом надбавки за награду",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    koddzn=atoi(str); 
    continue;
   }


  if(iceb_u_SRAV(str,"Код командировочного начисления",0) == 0)
   {
    iceb_sozmas(&kodkomand,row_alx[0],&cur_alx);
    continue;
   }
  if(iceb_u_SRAV(str,"Код компенсации за командировку",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kod_komp_z_kom=atoi(str); 
    continue;
   }

  if(iceb_u_SRAV(str,"Код табеля командировочных",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kodt_komand=atoi(str); 
    continue;
   }

  if(iceb_u_SRAV(str,"Код бюджетного табеля командировочных",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kodt_b_komand=atoi(str); 
    continue;
   }

  if(iceb_u_SRAV(str,"Код пенсионного фонда",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kodpenf=(short)iceb_u_atof(str); 
    continue;
   }

  if(iceb_u_SRAV(str,"Профсоюзный фонд-код отчисления",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kuprof=(short)iceb_u_atof(str); 
    continue;
   }
  if(iceb_u_SRAV(str,"Профсоюзный фонд-процент отчисления",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    procprof=(float)iceb_u_atof(str); 
    continue;
   }

  if(iceb_u_SRAV(str,"Код отчисления в соц-страх",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kodsocstr=(short)iceb_u_atof(str); 
    continue;
   }
  if(iceb_u_SRAV(str,"Код фонда занятости",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kodbezf=(short)iceb_u_atof(str); 
    continue;
   }
  if(iceb_u_SRAV(str,"Код фонда социального страхования",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kodsoc=(short)iceb_u_atof(str); 
    continue;
   }
  if(iceb_u_SRAV(str,"Код фонда социальной защиты инвалидов",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kodsoci=(short)iceb_u_atof(str); 
    continue;
   }

  if(iceb_u_SRAV(str,"Код фонда страхования от несчастного случая",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kodfsons=(short)iceb_u_atof(str); 
    continue;
   }


  if(iceb_u_SRAV(str,"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     startgodz=(short)iceb_u_atof(str); 
     continue;
    }

  if(iceb_u_SRAV(str,"Автоматическая вставка кода контрагента в список счета",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     avkvs=0;
     if(iceb_u_SRAV(str,"Вкл",1) == 0)
       avkvs=1;
     continue;
    }

  if(iceb_u_SRAV(str,"Печатать фамилию в корешке крупно",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     pehfam=0;
     if(iceb_u_SRAV(str,"Вкл",1) == 0)
       pehfam=1;
     continue;
    }

  if(iceb_u_SRAV(str,"Расчет налогов последовательный",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     metkarnb=0;
     if(iceb_u_SRAV(str,"Вкл",1) == 0)
       metkarnb=1;
     continue;
    }


  if(iceb_u_SRAV(str,"Счет расчетов по зарплате",0) == 0 )
    {
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     if(iceb_u_polen(bros,shrpzbt,sizeof(shrpzbt),2,',') == 0)
      {
       iceb_u_polen(bros,shrpz,sizeof(shrpz),1,',');
      }            
     else
       strncpy(shrpz,bros,sizeof(shrpz)-1);

     continue;
    }


  if(iceb_u_SRAV(str,"Коды не входящие в расчет отчислений на безработицу",0) == 0)
   {
    iceb_sozmas(&kodbzrnv,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Коды не входящие в расчет индексации",0) == 0)
   {
    iceb_sozmas(&kodnvrindex,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Коды начислений не входящие в расчет командировочных",0) == 0)
   {
    iceb_sozmas(&knvr_komand,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Коды не входящие в расчет отчислений на соцстрах",0) == 0)
   {
    iceb_sozmas(&kodsocstrnv,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Коды табеля не входящие в расчет командировочных",0) == 0)
   {
    iceb_sozmas(&ktnvr_komand,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Профсоюзный фонд-коды начислений не входящие в расчет",0) == 0)
    {
     iceb_sozmas(&knvrprof,row_alx[0],&cur_alx);
     continue;
    }

  if(iceb_u_SRAV(str,"Профсоюзный фонд-коды начислений не входящие в расчет",0) == 0)
    {
     iceb_sozmas(&knvrprof,row_alx[0],&cur_alx);
     continue;
    }

  if(iceb_u_SRAV(str,"Приставка к табельному номеру",0) == 0 )
   {
    iceb_u_polen(row_alx[0],pktn,sizeof(pktn),2,'|');
    continue;
   }

  if(iceb_u_SRAV(str,"Счет подоходного налога",0) == 0)
    {
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     shpn=new char[strlen(bros)+1];
     strcpy(shpn,bros);     
     continue;
    }

  if(iceb_u_SRAV(str,"Максимальная сумма необлагаемой благотворительной помощи",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     snmpz = iceb_u_atof(str);
     continue;
    }

  if(iceb_u_SRAV(str,"Тариф отчисления в пенсионный фонд инвалидами",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    p_tarif_inv = iceb_u_atof(str);
    continue;
   }
  if(iceb_u_SRAV(str,"Коэффициент прожиточного минимума",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    kof_prog_min = iceb_u_atof(str);
    continue;
   }

  if(iceb_u_SRAV(str,"Процент подоходного налога",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    ppn = iceb_u_atof(str);
    continue;
   }

  if(iceb_u_SRAV(str,"Процент от минимальной заработной платы",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    pomzp = iceb_u_atof(str);
    continue;
   }
  if(iceb_u_SRAV(str,"Код больничного",0) == 0)
   {
    iceb_sozmas(&kodbl,row_alx[0],&cur_alx);
    continue;
   }


  if(iceb_u_SRAV(str,"Обязательные удержания",0) == 0)
   {
    iceb_sozmas(&obud,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Коды удержаний входящие в расчет пенсионного отчисления с работника",0) == 0)
   {
    iceb_sozmas(&kuvvr_pens_sr,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Коды удержаний входящие в расчет пенсионного отчисления с фонда зарплаты",0) == 0)
   {
    iceb_sozmas(&kuvvr_pens_sfz,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Обязательные начисления",0) == 0)
    {
     iceb_sozmas(&obnah,row_alx[0],&cur_alx);
     continue;
    }


  if(iceb_u_SRAV(str,"Код подоходного налога",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     kodpn=(short)iceb_u_atof(str);
     if(kodpn != 0)
      kodpn_all.z_plus(kodpn);
     continue;
    }
  if(iceb_u_SRAV(str,"Код подоходного налога с больничного",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     kodpn_sbol=(short)iceb_u_atof(str);
     if(kodpn_sbol != 0)
      kodpn_all.z_plus(kodpn_sbol);
     continue;
    }
  if(iceb_u_SRAV(str,"Код начисления для возврата суммы подоходного налога",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     kodvpn=(short)iceb_u_atof(str);
     continue;
    }

  if(iceb_u_SRAV(str,"Код перерасчета подоходного налога",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     kodperpn=(short)iceb_u_atof(str);
     if(kodperpn != 0)
      kodpn_all.z_plus(kodperpn);
     continue;
    }

  if(iceb_u_SRAV(str,"Код индексации",0) == 0)
   {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     kodindex=(short)iceb_u_atof(str);
     continue;
   }
  if(iceb_u_SRAV(str,"Код индексации больничного",0) == 0)
   {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     kodindexb=(short)iceb_u_atof(str);
     continue;
   }

  if(iceb_u_SRAV(str,"Код пенсионного отчисления",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     kodpen=(short)iceb_u_atof(str);
     continue;
    }

  if(iceb_u_SRAV(str,"Код отчисления на безработицу",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     kodbzr=(short)iceb_u_atof(str);
     continue;
    }

  if(iceb_u_SRAV(str,"Процент отчисления в пенсионный фонд",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     procpen[0]=(float)iceb_u_atof(str);
     if(iceb_u_polen(row_alx[0],str,sizeof(str),3,'|') == 0)
       procpen[1]=(float)iceb_u_atof(str);
     else
       procpen[1]=procpen[0];
     continue;
    }
  if(iceb_u_SRAV(str,"Процент отчисления в соц-страх",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     procsoc=(float)iceb_u_atof(str);
     iceb_u_polen(row_alx[0],str,sizeof(str),3,'|');
     procsoc1=(float)iceb_u_atof(str);
     continue;
    }

  if(iceb_u_SRAV(str,"Процент отчисления в фонд безработицы",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     procbez=(float)iceb_u_atof(str);
     continue;
    }



  if(iceb_u_SRAV(str,"Коды благотворительной помощи",0) == 0)
   {
    iceb_sozmas(&kodmp,row_alx[0],&cur_alx);
    continue;
   }


  if(iceb_u_SRAV(str,"Коды начислений не входящие в расчет подоходного налога",0) == 0)
   {
    iceb_sozmas(&knvr,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Коды начислений неденежными формами",0) == 0)
   {
    iceb_sozmas(&knnf,row_alx[0],&cur_alx);
    continue;
   }
  if(iceb_u_SRAV(str,"Коды начислений неденежными формами на которые начисляется НДС",0) == 0)
   {
    iceb_sozmas(&knnf_nds,row_alx[0],&cur_alx);
    continue;
   }
  if(iceb_u_SRAV(str,"Коды начислений, на которые не распространяется социальная льгота",0) == 0)
   {
    iceb_sozmas(&kn_bsl,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Коды не входящие в расчет пенсионного отчисления",0) == 0)
   {
    iceb_sozmas(&kodnvpen,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Код доплаты до минимальной зарплаты",0) == 0)
   {
    iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
    koddopmin=(short)iceb_u_atof(str);
    continue;
   }

  if(iceb_u_SRAV(str,"Коды не входящие в расчет доплаты до минимальной зарплаты",0) == 0)
   {
    iceb_sozmas(&kodnvmin,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Коды отдельного расчета доплаты до минимальной зарплаты",0) == 0)
   {
    iceb_sozmas(&kodotrs,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Коды удержаний входящие в расчет подоходного налога",0) == 0)
   {
    iceb_sozmas(&kuvr,row_alx[0],&cur_alx);
    continue;
   }

  if(iceb_u_SRAV(str,"Округление",0) == 0)
    {
     iceb_u_polen(row_alx[0],str,sizeof(str),2,'|');
     okrg=iceb_u_atof(str);
    }

 }
/*Чтение кодов единого социального взноса*/
zarrnesvw(NULL,NULL);

memset(bros,'\0',sizeof(bros));
iceb_poldan("Многопорядковый план счетов",bros,"nastrb.alx",NULL);
if(iceb_u_SRAV(bros,"Вкл",1) == 0)
 vplsh=1;


startgodb=0;
if(iceb_poldan("Стартовый год",bros,"nastrb.alx",NULL) == 0)
 {
  startgodb=(short)iceb_u_atof(bros);
 }

/*Ищем списки начислений для дополнительных зарплатных счетов*/

for(int kkk=0; kkk < dop_zar_sheta.kolih(); kkk++)
 {
  sprintf(str,"Начисления, для которых проводки выполняются по дополнительному счёту %s",dop_zar_sheta.ravno(kkk));
  memset(bros,'\0',sizeof(bros));
  iceb_poldan(str,bros,"zarnast.alx",NULL);
  dop_zar_sheta_spn.plus(bros);

/*  printw("dop_zar_sheta=%s-%s\n",dop_zar_sheta.ravno(kkk),dop_zar_sheta_spn.ravno(kkk));*/
 
 }

class iceb_u_str znah("");

if(iceb_poldan("Процент начисления для фонда безработицы",&znah,"zardog.alx",NULL) == 0)
 {
  
  procent_fb_dog=znah.ravno_atof();
 }

return(0);
}
