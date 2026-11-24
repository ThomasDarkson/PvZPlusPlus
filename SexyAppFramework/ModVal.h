#ifndef __MODVAL_H__
#define __MODVAL_H__

#include <string>

  
                                                                           
                       

                                                                          
                                                                       
                                                                         
                                                                         
                                                                  
                         

         
                

         
                                                            
                                                                    
                                                                 
         

             
                                                                   
                                                                       
                                                                

   

namespace Sexy
{

#if defined(SEXY_DISABLE_MODVAL) || defined(RELEASEFINAL)
#define M(val)  (val)
#define M1(val) (val)
#define M2(val) (val)
#define M3(val) (val)
#define M4(val) (val)
#define M5(val) (val)
#define M6(val) (val)
#define M7(val) (val)
#define M8(val) (val)
#define M9(val) (val)
#else
#define MODVAL_STR_COUNTER2(x,y,z) x#y","#z
#define MODVAL_STR_COUNTER1(x,y,z) MODVAL_STR_COUNTER2(x,y,z)
#define MODVAL_STR_COUNTER(x) MODVAL_STR_COUNTER1(x,__COUNTER__,__LINE__)
#define M(val) ModVal(0, MODVAL_STR_COUNTER("SEXY_SEXYMODVAL"__FILE__), (val))
#define M1(val) M(val)
#define M2(val) M(val)
#define M3(val) M(val)
#define M4(val) M(val)
#define M5(val) M(val)
#define M6(val) M(val)
#define M7(val) M(val)
#define M8(val) M(val)
#define M9(val) M(val)
#endif

int				ModVal(int theAreaNum, const char* theFileName, int theInt);
double			ModVal(int theAreaNum, const char* theFileName, double theDouble);
float			ModVal(int theAreaNum, const char* theFileName, float theFloat);
const char*		ModVal(int theAreaNum, const char* theFileName, const char *theStr);
bool			ReparseModValues();
void			AddModValEnum(const std::string &theEnumName, int theVal);

}

#endif               