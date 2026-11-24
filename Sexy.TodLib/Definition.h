#ifndef __TODDEFINITION_H__
#define __TODDEFINITION_H__

#include <string>
#include "TodList.h"
#include "Reanimator.h"
#include "TodParticle.h"

enum class DefFieldType : int
{
    DT_INVALID,
    DT_INT,
    DT_FLOAT,
    DT_STRING,
    DT_ENUM,
    DT_VECTOR2,
    DT_ARRAY,
    DT_TRACK_FLOAT,
    DT_FLAGS,
    DT_IMAGE,
    DT_FONT
};

  
                                                
                                                                                                                                                               
                                                                                                                                                                                               
  

                                                                                                       
                         
                                                                                                       
                                                                                                                                           
                                                                                                       
class DefSymbol
{
public:
    int                 mSymbolValue;                                                                                                              
    const char* mSymbolName;                                                                                                                                      
};
                                                      
extern DefSymbol gEmitterTypeSymbols[];            
                                                      

                                                                                                       
                         
                                                                                                       
                                                                                                                   
                                                                                                       
class DefField
{
public:
    const char* mFieldName;                                                                                                                                       
    int                 mFieldOffset;                                                                                       
    DefFieldType        mFieldType;                                                                                                                  
    void* mExtraData;                                                                                                          
                                                                                                                                                 
                                                                                                                                                                    
                                                                                                                                                        
                                                                                                                                                                     
                                                                                                                                                                                                                            
};

                                                                                                       
                            
                                                                                                       
                                                                                                                                             
                                                                                                       
class DefMap
{
public:
    DefField* mMapFields;                                                                                                                                                       
    int                 mDefSize;                                                                                                                                                          
    void* (*mConstructorFunc)(void*);                                                            
};

void* TodParticleDefinitionConstructor(void* thePointer);           
void* TodEmitterDefinitionConstructor(void* thePointer);            
void* ParticleFieldConstructor(void* thePointer);                   
void* TrailDefinitionConstructor(void* thePointer);                 
void* ReanimatorTransformConstructor(void* thePointer);             
void* ReanimatorTrackConstructor(void* thePointer);                 
void* ReanimatorDefinitionConstructor(void* thePointer);            

                                                        
extern DefMap gParticleFieldDefMap;            
  
                                                  
extern DefMap gEmitterDefMap;            
  
                                                   
extern DefMap gParticleDefMap;            
  
extern DefMap gTrailDefMap;            
  
                                                              
extern DefMap gReanimatorTransformDefMap;            
  
                                                          
extern DefMap gReanimatorTrackDefMap;            
  
                                                     
extern DefMap gReanimatorDefMap;            

                                                                                                       
                         
                                                                                                       
                                                                     
                                                                                                       
class DefinitionArrayDef
{
public:
    void* mArrayData;                                                                                                                                       
    int                 mArrayCount;                                                                                                                                         
                                                                                                                                                 
                                                                                                                                                                  
                                                                                                                                                                                                        
};

                                                                                                       
                                  
                                                                                                       
                                                                                                                                                
                                                                                                       
class CompressedDefinitionHeader
{
public:
    unsigned int        mCookie;                                                               
    unsigned int        mUncompressedSize;                                               
};

                                                                                                       
                         
                                                                                                       
                                                                                                      
                                                                                                       
class DefLoadResPath
{
public:
    const char* mPrefix;                                                                
    const char* mDirectory;                                                                                     
};

SexyString       DefinitionGetCompiledFilePathFromXMLFilePath(const SexyString& theXMLFilePath);
bool                    IsFileInPakFile(const SexyString& theFilePath);
bool                    DefinitionIsCompiled(const SexyString& thePrefix, const SexyString& theXMLFilePath);
bool                    DefinitionReadCompiledFile(const SexyString& theCompiledFilePath, DefMap* theDefMap, void* theDefinition);
void                    DefinitionFillWithDefaults(DefMap* theDefMap, void* theDefinition);
void                    DefinitionXmlError(XMLParser* theXmlParser, char* theFormat, ...);
bool                    DefSymbolValueFromString(DefSymbol* theSymbolMap, const char* theName, int* theResultValue);
bool                    DefinitionReadXMLString(XMLParser* theXmlParser, SexyString& theValue);
bool                    DefinitionReadIntField(XMLParser* theXmlParser, int* theValue);
bool                    DefinitionReadFloatField(XMLParser* theXmlParser, float* theValue);
bool                    DefinitionReadStringField(XMLParser* theXmlParser, const char** theValue);
bool                    DefinitionReadEnumField(XMLParser* theXmlParser, int* theValue, DefSymbol* theSymbolMap);
bool                    DefinitionReadVector2Field(XMLParser* theXmlParser, SexyVector2* theValue);
bool                    DefinitionReadArrayField(XMLParser* theXmlParser, DefinitionArrayDef* theArray, DefField* theField);
bool                    DefinitionReadFloatTrackField(XMLParser* theXmlParser, FloatParameterTrack* theTrack);
bool                    DefinitionReadFlagField(XMLParser* theXmlParser, const SexyString& theElementName, uint* theResultValue, DefSymbol* theSymbolMap);
bool                    DefinitionReadImageField(XMLParser* theXmlParser, Image** theImage);
bool                    DefinitionReadFontField(XMLParser* theXmlParser, Font** theFont);
bool                    DefinitionReadField(XMLParser* theXmlParser, DefMap* theDefMap, void* theDefinition, bool* theDone);
bool                    DefinitionWriteCompiledFile(const SexyString& theCompiledFilePath, DefMap* theDefMap, void* theDefinition);
bool                    DefinitionCompileFile(const SexyString theXMLFilePath, const SexyString& theCompiledFilePath, DefMap* theDefMap, void* theDefinition);

void                    DefMapWriteToCache(void*& theWritePtr, DefMap* theDefMap, void* theDefinition);
void                    DefWriteToCacheString(void*& theWritePtr, char** theValue);
void                    DefWriteToCacheArray(void*& theWritePtr, DefinitionArrayDef* theValue, DefMap* theDefMap);
void                    DefWriteToCacheFloatTrack(void*& theWritePtr, FloatParameterTrack* theValue);
void                    DefWriteToCacheImage(void*& theWritePtr, Image** theValue);
void                    DefWriteToCacheFont(void*& theWritePtr, Font** theValue);

void* DefinitionCompressCompiledBuffer(void* theBuffer, unsigned int theBufferSize, unsigned int* theResultSize);

           unsigned int DefGetSizeString(char** theValue);
           unsigned int DefinitionGetArraySize(DefinitionArrayDef* theValue, DefMap* theDefMap);
           unsigned int DefGetSizeFloatTrack(FloatParameterTrack* theValue);
           unsigned int DefGetSizeImage(Image** theValue);
           unsigned int DefGetSizeFont(Font** theValue);

           unsigned int DefinitionGetDeepSize(DefMap* theDefMap, void* theDefinition);
           unsigned int DefinitionGetSize(DefMap* theDefMap, void* theDefinition);
           void* DefinitionAlloc(int theSize);
void* DefinitionUncompressCompiledBuffer(void* theCompressedBuffer, size_t theCompressedBufferSize, size_t& theUncompressedSize, const SexyString& theCompiledFilePath);
uint             DefinitionCalcHashSymbolMap(int aSchemaHash, DefSymbol* theSymbolMap);
uint             DefinitionCalcHashDefMap(int aSchemaHash, DefMap* theDefMap, TodList<DefMap*>& theProgressMaps);
uint             DefinitionCalcHash(DefMap* theDefMap);
inline bool             DefReadFromCacheString(void*& theReadPtr, char** theString);
inline bool             DefReadFromCacheArray(void*& theReadPtr, DefinitionArrayDef* theArray, DefMap* theDefMap);
inline bool             DefReadFromCacheImage(void*& theReadPtr, Image** theImage);
inline bool             DefReadFromCacheFont(void*& theReadPtr, Font** theFont);
inline bool             DefReadFromCacheFloatTrack(void*& theReadPtr, FloatParameterTrack* theTrack);
bool                    DefMapReadFromCache(void*& theReadPtr, DefMap* theDefMap, void* theDefinition);
bool                    DefinitionCompileAndLoad(const SexyString& theXMLFilePath, DefMap* theDefMap, void* theDefinition);
bool                    DefinitionLoadMap(XMLParser* theXmlParser, DefMap* theDefMap, void* theDefinition);
bool                    DefinitionLoadImage(Image** theImage, const SexyString& theName);
bool                    DefinitionLoadFont(Font** theFont, const SexyString& theName);
bool                    DefinitionLoadXML(const SexyString& theFilename, DefMap* theDefMap, void* theDefinition);
void                    DefinitionFreeArrayField(DefinitionArrayDef* theArray, DefMap* theDefMap);
void                    DefinitionFreeMap(DefMap* theDefMap, void* theDefinition);

           bool         FloatTrackIsSet(const FloatParameterTrack& theTrack);
           void         FloatTrackSetDefault(FloatParameterTrack& theTrack, float theValue);
float                   FloatTrackEvaluate(FloatParameterTrack& theTrack, float theTimeValue, float theInterp);
float                   FloatTrackEvaluateFromLastTime(FloatParameterTrack& theTrack, float theTimeValue, float theInterp);
           bool         FloatTrackIsConstantZero(FloatParameterTrack& theTrack);

#ifdef OPENFL_HOST
const std::string& DefinitionGetLastError();
#endif

#endif