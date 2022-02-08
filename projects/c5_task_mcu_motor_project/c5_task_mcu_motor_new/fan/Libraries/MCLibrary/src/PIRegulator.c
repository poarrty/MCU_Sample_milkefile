/******************************************************************************
* File Name         :  PIRegulator.c
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  PI调整器                       
********************************************************************************
* History: 
* <author>        <time>        <version >        <desc>
* 陈雅枫             19/01/14      1.0               创建   
*
* 
*
* 
* 
*
* 
*
*
*
*
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "UserInclude.h"
#include "MCInclude.h"




#define VQMAX        32767
#define VDMAX        13000  //13000  3000


static void PI_Config(pPI_t this);


PIParams_t PIIqParams =
{
    #ifdef SPEED_MODE
	(int16_t)16384,//128 4096    1024     15000   8192 128 8192 128  100  8192(nidec第二次打样配12.5寸)   9000       中大二级减速(10000)    /*!< Default Kp gain*/  开环 16384    4096(互邦)  100
    (int16_t)100,// 10  100     10       20      10   10  10   10   16    12(nidec第二次打样配12.5寸)  8        16  中大二级减速(4)   /*!< Default Ki gain*/   开环 128   双闭环积分不能大于16           2(互邦) 16
    #else
    (int16_t)16384,  //16384(开环相电流抑制调整)
    (int16_t)1024,	 //1024(开环相电流抑制调整)
	#endif
    (int16_t)8192,
    (uint16_t)16384,               /*!< Kp gain divisor */    //16384 
    (uint16_t)16384,              /*!< Ki gain divisor */    //16384
    (int32_t)VQMAX * (int32_t)16384,     /*!< Upper limit used to saturate the integral term*/
    -(int32_t)VQMAX * (int32_t)16384,     /*!< Lower limit used to saturate the integral term */
    (int16_t)VQMAX,                     /*!< Upper limit used to saturate the PI output*/
    -(int16_t)VQMAX,                    /*!< Lower limit used to saturate the PI output*/
    (uint16_t)LOG2(16384),    //16384     /*!< Kp gain divisor expressed as power of 2.E.g. */
    (uint16_t)LOG2(16384)         /*!< Ki gain divisor expressed as power of 2.E.g. */
};


PIParams_t PIIdParams =
{
    (int16_t)16384,  //128   4096   128 1024 8192 128  8192(nidec第二次打样配12.5寸)   8192/*!< Default Kp gain*/ 开环 16384  128
    (int16_t)100,  //128   4096  128 1024   10  10  1024(nidec第二次打样配12.5寸)     /*!< Default Ki gain*/       开环 128  10
    (int16_t)8192,
    (uint16_t)16384, //16384                  /*!< Kp gain divisor*/
    (uint16_t)16384,                  /*!< Ki gain divisor*/
    (int32_t)(VDMAX) * 16384,         /*!< Upper limit used to saturate the integral term*/
    -(int32_t)(VDMAX) * 16384,         /*!< Lower limit used to saturate the integral term*/
    (int16_t)VDMAX,                          /*!< Upper limit used to saturate the PI output*/
    -(int16_t)VDMAX,                         /*!< Lower limit used to saturate the PI output*/
    (uint16_t)LOG2(16384), //16384            /*!< Kp gain divisor expressed as power of 2.E.g.*/
    (uint16_t)LOG2(16384)             /*!< Ki gain divisor expressed as power of 2.E.g.*/
};



PIParams_t PISpeedParams =
{
   #ifdef LINEAR_HALL
	(int16_t)8192,  //8192 8192 4096 8192(nidec第二次打样配12.5寸)         4096(联谊 值越大停机抖动越大)
    (int16_t)4,    //12    10   4    4(nidec第二次打样配12.5寸) 2          值越小停机越慢           6(联谊 值越大停机抖动越大)            测试 20   
    #else
	(int16_t)3000,// 30000  15000  8192  15000  8192  8192  4096
	(int16_t)3,  //  35     20     4      20    10    4  10  6   
	#endif     
    (int16_t)128,
    (uint16_t)4096,  //4096             
    (uint16_t)16384,     
    (int32_t)IQMAX * (int32_t)16384,//IQMAX
    -(int32_t)IQMAX * (int32_t)16384,//IQMAX
    (int16_t)IQMAX, //IQMAX   VQMAX               
    -(int16_t)IQMAX,  //IQMAX             
    (uint16_t)LOG2(8192),//4096
    (uint16_t)LOG2(16384)         //16384 
};


PIParams_t PIIsParams =
{
  #ifdef SPEED_MODE
  	(int16_t)15000, //5000(开环)  60000(闭环)
    (int16_t)400,  //200(开环)   1500(闭环)
	#else
  	(int16_t)1024, //5000(开环)  60000(闭环) 10000(堵转测试)        
    (int16_t)512,  //200(开环)   1500(闭环)	6000(堵转测试)         
	#endif
    (int16_t)8192,
    (uint16_t)4096,//16384,
    (uint16_t)4096,//16384,
    (int32_t)VQMAX * (int32_t)16384,// IQMAX
    (int32_t)0,
    (int16_t)VQMAX,//IQMAX
    (int16_t)0,
    (uint16_t)LOG2(4096),//16384),
    (uint16_t)LOG2(4096)//16384)
};


PIParams_t PIVsParams =
{
    (int16_t)15000,
    (int16_t)300,
    (int16_t)8192,
    (uint16_t)16384,
    (uint16_t)16384,
    (int32_t)VQMAX * (int32_t)16384, //IQMAX
    (int32_t)0,
    (int16_t)VQMAX,//IQMAX
    (int16_t)0,
    (uint16_t)LOG2(16384),
    (uint16_t)LOG2(16384)
};


PIParams_t PINTcParams =
{
    (int16_t)64,//15000
    (int16_t)2, //300
    (int16_t)8192,
    (uint16_t)16384,
    (uint16_t)16384,
    (int32_t)VQMAX * (int32_t)16384, //IQMAX
    (int32_t)0,
    (int16_t)VQMAX,//IQMAX
    (int16_t)0,
    (uint16_t)LOG2(16384),
    (uint16_t)LOG2(16384)
};



PI_t PIIqpool[NBR_OF_MOTORS];
PI_t PIIdpool[NBR_OF_MOTORS];
PI_t PIIspool[NBR_OF_MOTORS];
PI_t PIVspool[NBR_OF_MOTORS];
PI_t PISpeedpool[NBR_OF_MOTORS];
PI_t PINTcpool[NBR_OF_MOTORS];


    
/*******************************************************************************
* Function Name  : PI_Init
* Description    : PI 初始化函数
* parameters     : None
* Return         : None
*******************************************************************************/
void PI_Init(void)
{
    oPIDIq[0] = &PIIqpool[0];    
    oPIDId[0] = &PIIdpool[0];    
    oPIDIs[0] = &PIIspool[0];    
    oPIDVs[0] = &PIVspool[0];        
    oPIDSpeed[0] = &PISpeedpool[0];
    oPINTc[0] = &PINTcpool[0];	
   
    oPIDIq[0]->pParams = &PIIqParams;   
    oPIDId[0]->pParams = &PIIdParams;    
    oPIDIs[0]->pParams = &PIIsParams;   
    oPIDVs[0]->pParams = &PIVsParams;   
    oPIDSpeed[0]->pParams = &PISpeedParams;
	oPINTc[0]->pParams = &PINTcParams;	    

    PI_Config(oPIDIq[0]);   
    PI_Config(oPIDId[0]);   
    PI_Config(oPIDIs[0]);   
    PI_Config(oPIDVs[0]);      
    PI_Config(oPIDSpeed[0]);  
    PI_Config(oPINTc[0]);    


#ifdef DOUBLE_DRIVER   
    oPIDIq[1] = &PIIqpool[1];    
    oPIDId[1] = &PIIdpool[1];    
    oPIDIs[1] = &PIIspool[1];    
    oPIDVs[1] = &PIVspool[1];        
    oPIDSpeed[1] = &PISpeedpool[1];
    oPINTc[1] = &PINTcpool[1];	
   
    oPIDIq[1]->pParams = &PIIqParams;   
    oPIDId[1]->pParams = &PIIdParams;    
    oPIDIs[1]->pParams = &PIIsParams;   
    oPIDVs[1]->pParams = &PIVsParams;   
    oPIDSpeed[1]->pParams = &PISpeedParams;
	oPINTc[1]->pParams = &PINTcParams;	    

    PI_Config(oPIDIq[1]);   
    PI_Config(oPIDId[1]);   
    PI_Config(oPIDIs[1]);   
    PI_Config(oPIDVs[1]);      
    PI_Config(oPIDSpeed[1]);  
    PI_Config(oPINTc[1]);    
#endif    
}


/*******************************************************************************
* Function Name  : PI_Config
* Description    : PI 参数配置
* parameters     : this PIxxpool结构指针
* Return         : None
*******************************************************************************/
static void PI_Config(pPI_t this)
{
    pPIVars_t pVars = &(this->Vars);
    pPIParams_t pParams = this->pParams;
    
    pVars->hKpGain = pParams->hDefKpGain;
    pVars->hKiGain = pParams->hDefKiGain;
	pVars->hKsGain = pParams->hDefKsGain;
    pVars->wIntegralTerm = 0;
    pVars->wUpperIntegralLimit = pParams->wDefMaxIntegralTerm;
    pVars->wLowerIntegralLimit = pParams->wDefMinIntegralTerm;
    pVars->hUpperOutputLimit = pParams->hDefMaxOutput;
    pVars->hLowerOutputLimit = pParams->hDefMinOutput;
    pVars->hKpDivisor = pParams->hKpDivisor;
    pVars->hKiDivisor = pParams->hKiDivisor;
    pVars->hKpDivisorPOW2 = pParams->hKpDivisorPOW2;
    pVars->hKiDivisorPOW2 = pParams->hKiDivisorPOW2;
}


/*******************************************************************************
* Function Name  : PI_Controller
* Description    : PI调节函数
* parameters     : this PIxxpool结构指针        wProcessVarError = 目标量 - 反馈量
* Return         : PI调节后变量
*******************************************************************************/
int16_t PI_Controller(pPI_t this, int32_t wProcessVarError)
{
    int32_t wProportional_Term, wIntegral_Term, wOutput_32,wIntegral_sum_temp;
    int32_t wDischarge = 0;
    pPIVars_t pVars = &(this->Vars);
    int16_t hUpperOutputLimit = pVars->hUpperOutputLimit;
    int16_t hLowerOutputLimit = pVars->hLowerOutputLimit;

    /* Proportional term computation*/
    wProportional_Term = this->Vars.hKpGain * wProcessVarError;

    /* Integral term computation */
    if (pVars->hKiGain == 0)
    {
        pVars->wIntegralTerm = 0;
    }
    else
    {
        wIntegral_Term = pVars->hKiGain * wProcessVarError;
        wIntegral_sum_temp = pVars->wIntegralTerm + wIntegral_Term;

        if (wIntegral_sum_temp < 0)
        {
            if (pVars->wIntegralTerm > 0)
            {
                if (wIntegral_Term > 0)
                {
                    wIntegral_sum_temp = S32_MAX;
                }
            }
        }
        else
        {
            if (pVars->wIntegralTerm < 0)
            {
                if (wIntegral_Term < 0)
                {
                    wIntegral_sum_temp = -S32_MAX;
                }
            }
        }		

        if (wIntegral_sum_temp > pVars->wUpperIntegralLimit)
        {
            pVars->wIntegralTerm = pVars->wUpperIntegralLimit;
        }
        else if (wIntegral_sum_temp < pVars->wLowerIntegralLimit)
        { 
            pVars->wIntegralTerm = pVars->wLowerIntegralLimit;
        }
        else
        {
            pVars->wIntegralTerm = wIntegral_sum_temp;
        }
    }

    wOutput_32 = (wProportional_Term >>pVars->hKpDivisorPOW2) + (pVars->wIntegralTerm >> pVars->hKiDivisorPOW2);


    if (wOutput_32 > hUpperOutputLimit)
    {
        wDischarge = hUpperOutputLimit - wOutput_32;
        wOutput_32 = hUpperOutputLimit;		  			 	
    }
    else if (wOutput_32 < hLowerOutputLimit)
    {
        wDischarge = hLowerOutputLimit - wOutput_32; 
        wOutput_32 = hLowerOutputLimit;
    }
    else
    {}

    pVars->wIntegralTerm += (wDischarge *pVars->hKsGain);

    return((int16_t)(wOutput_32));
}

/*******************************************************************************
* Function Name  : PI_SetIntegralTerm
* Description    : PI积分量设定
* parameters     : this PIxxpool结构指针        wIntegralTermValue: 结构体对应积分量
* Return         : PI调节后变量
*******************************************************************************/
void PI_SetIntegralTerm(pPI_t this, int32_t wIntegralTermValue)
{
    pPIVars_t pVars = &(this->Vars);
    pVars->wIntegralTerm = wIntegralTermValue;
}




/**************************************************************END OF FILE****/
