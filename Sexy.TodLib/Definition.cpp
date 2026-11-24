#include "TodCommon.h"
#include "TodParticle.h"
#include "Trail.h"
#include <assert.h>
#include <cstring>
#include <stddef.h>
#include <sys/stat.h>
#include "TodDebug.h"
#include "Definition.h"
#include "../PakLib/PakInterface.h"
#include "../SexyAppFramework/PerfTimer.h"
#include "../SexyAppFramework/XMLParser.h"
#include "TodCommon.h"
#include "../Resources.h"
#include <zlib.h>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>

#ifdef OPENFL_HOST
static std::string gOpenPVZDefinitionError;
const std::string& DefinitionGetLastError() { return gOpenPVZDefinitionError; }
#endif

DefSymbol gTrailFlagDefSymbols[] = {            
    { 0, "Loops" },                 { -1, nullptr }
};
DefField gTrailDefFields[] = {            
    { "Image",            offsetof(TrailDefinition, mImage),           DefFieldType::DT_IMAGE,         nullptr },
    { "MaxPoints",        offsetof(TrailDefinition, mMaxPoints),       DefFieldType::DT_INT,           nullptr },
    { "MinPointDistance", offsetof(TrailDefinition, mMinPointDistance),DefFieldType::DT_FLOAT,         nullptr },
    { "TrailFlags",       offsetof(TrailDefinition, mTrailFlags),      DefFieldType::DT_FLAGS,         gTrailFlagDefSymbols },
    { "WidthOverLength",  offsetof(TrailDefinition, mWidthOverLength), DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "WidthOverTime",    offsetof(TrailDefinition, mWidthOverTime),   DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "AlphaOverLength",  offsetof(TrailDefinition, mAlphaOverLength), DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "AlphaOverTime",    offsetof(TrailDefinition, mAlphaOverTime),   DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "TrailDuration",    offsetof(TrailDefinition, mTrailDuration),   DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "",                 0x0,                                         DefFieldType::DT_INVALID,       nullptr }
};
DefMap gTrailDefMap = { gTrailDefFields, sizeof(TrailDefinition), TrailDefinitionConstructor };            

DefSymbol gParticleFlagSymbols[] = {            
    {  0, "RandomLaunchSpin" },     {  1, "AlignLaunchSpin" },  {  2, "AlignToPixel" },     {  4, "ParticleLoops" },    {  3, "SystemLoops" },
    {  5, "ParticlesDontFollow" },  {  6, "RandomStartTime" },  {  7, "DieIfOverloaded" },  {  8, "Additive" },         {  9, "FullScreen" },
    { 10, "SoftwareOnly" },         { 11, "HardwareOnly" },     { -1, nullptr }
};
DefSymbol gEmitterTypeSymbols[] = {            
    {  0, "Circle" },               {  1, "Box" },              {  2, "BoxPath" },          {  3, "CirclePath" },       {  4, "CircleEvenSpacing" },
    { -1, nullptr }
};
DefSymbol gParticleTypeSymbols[] = {            
    {  1, "Friction" },             {  2, "Acceleration" },     {  3, "Attractor" },        {  4, "MaxVelocity" },      {  5, "Velocity" },
    {  6, "Position" },             {  7, "SystemPosition" },   {  8, "GroundConstraint" }, {  9, "Shake" },            { 10, "Circle" },
    { 11, "Away" },                 { -1, nullptr }
};

DefField gParticleFieldDefFields[] = {            
    { "FieldType",          offsetof(ParticleField, mFieldType), DefFieldType::DT_ENUM,          gParticleTypeSymbols },
    { "x",                  offsetof(ParticleField, mX),         DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "y",                  offsetof(ParticleField, mY),         DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "",                   0x0,                                 DefFieldType::DT_INVALID,       nullptr },
};
DefMap gParticleFieldDefMap = { gParticleFieldDefFields, sizeof(ParticleField), ParticleFieldConstructor };            

DefField gEmitterDefFields[] = {            
    { "Image",              offsetof(TodEmitterDefinition,mImage)               ,  DefFieldType::DT_IMAGE,         nullptr },
    { "ImageRow",           offsetof(TodEmitterDefinition,mImageRow)            ,  DefFieldType::DT_INT,           nullptr },
    { "ImageCol",           offsetof(TodEmitterDefinition,mImageCol)            ,  DefFieldType::DT_INT,           nullptr },
    { "ImageFrames",        offsetof(TodEmitterDefinition,mImageFrames)         ,  DefFieldType::DT_INT,           nullptr },
    { "Animated",           offsetof(TodEmitterDefinition,mAnimated)            ,  DefFieldType::DT_INT,           nullptr },
    { "ParticleFlags",      offsetof(TodEmitterDefinition,mParticleFlags)       ,  DefFieldType::DT_FLAGS,         gParticleFlagSymbols },
    { "EmitterType",        offsetof(TodEmitterDefinition,mEmitterType)         ,  DefFieldType::DT_ENUM,          gEmitterTypeSymbols },
    { "Name",               offsetof(TodEmitterDefinition,mName)                ,  DefFieldType::DT_STRING,        nullptr },
    { "SystemDuration",     offsetof(TodEmitterDefinition,mSystemDuration)      ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "OnDuration",         offsetof(TodEmitterDefinition,mOnDuration)          ,  DefFieldType::DT_STRING,        nullptr },
    { "CrossFadeDuration",  offsetof(TodEmitterDefinition,mCrossFadeDuration)  ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SpawnRate",          offsetof(TodEmitterDefinition,mSpawnRate)          ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SpawnMinActive",     offsetof(TodEmitterDefinition,mSpawnMinActive)     ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SpawnMaxActive",     offsetof(TodEmitterDefinition,mSpawnMaxActive)     ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SpawnMaxLaunched",   offsetof(TodEmitterDefinition,mSpawnMaxLaunched)   ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterRadius",      offsetof(TodEmitterDefinition,mEmitterRadius)      ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterOffsetX",     offsetof(TodEmitterDefinition,mEmitterOffsetX)     ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterOffsetY",     offsetof(TodEmitterDefinition,mEmitterOffsetY)     ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterBoxX",        offsetof(TodEmitterDefinition,mEmitterBoxX)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterBoxY",        offsetof(TodEmitterDefinition,mEmitterBoxY)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterPath",        offsetof(TodEmitterDefinition,mEmitterPath)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterSkewX",       offsetof(TodEmitterDefinition,mEmitterSkewX)       ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterSkewY",       offsetof(TodEmitterDefinition,mEmitterSkewY)       ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleDuration",   offsetof(TodEmitterDefinition,mParticleDuration)   ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SystemRed",          offsetof(TodEmitterDefinition,mSystemRed)          ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SystemGreen",        offsetof(TodEmitterDefinition,mSystemGreen)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SystemBlue",         offsetof(TodEmitterDefinition,mSystemBlue)         ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SystemAlpha",        offsetof(TodEmitterDefinition,mSystemAlpha)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SystemBrightness",   offsetof(TodEmitterDefinition,mSystemBrightness)   ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "LaunchSpeed",        offsetof(TodEmitterDefinition,mLaunchSpeed)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "LaunchAngle",        offsetof(TodEmitterDefinition,mLaunchAngle)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "Field",              offsetof(TodEmitterDefinition,mParticleFields)     ,  DefFieldType::DT_ARRAY,         &gParticleFieldDefMap },
    { "SystemField",        offsetof(TodEmitterDefinition,mSystemFields)       ,  DefFieldType::DT_ARRAY,         &gParticleFieldDefMap },
    { "ParticleRed",        offsetof(TodEmitterDefinition,mParticleRed)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleGreen",      offsetof(TodEmitterDefinition,mParticleGreen)      ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleBlue",       offsetof(TodEmitterDefinition,mParticleBlue)       ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleAlpha",      offsetof(TodEmitterDefinition,mParticleAlpha)      ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleBrightness", offsetof(TodEmitterDefinition,mParticleBrightness) ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleSpinAngle",  offsetof(TodEmitterDefinition,mParticleSpinAngle)  ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleSpinSpeed",  offsetof(TodEmitterDefinition,mParticleSpinSpeed)  ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleScale",      offsetof(TodEmitterDefinition,mParticleScale)      ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleStretch",    offsetof(TodEmitterDefinition,mParticleStretch)    ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "CollisionReflect",   offsetof(TodEmitterDefinition,mCollisionReflect)   ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "CollisionSpin",      offsetof(TodEmitterDefinition,mCollisionSpin)      ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ClipTop",            offsetof(TodEmitterDefinition,mClipTop)            ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ClipBottom",         offsetof(TodEmitterDefinition,mClipBottom)         ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ClipLeft",           offsetof(TodEmitterDefinition,mClipLeft)           ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ClipRight",          offsetof(TodEmitterDefinition,mClipRight)          ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "AnimationRate",      offsetof(TodEmitterDefinition,mAnimationRate)      ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "",                   0x0,                                                  DefFieldType::DT_INVALID,       nullptr },
};
DefMap gEmitterDefMap = { gEmitterDefFields, sizeof(TodEmitterDefinition), TodEmitterDefinitionConstructor };            

DefField gParticleDefFields[] = {            
    { "Emitter",            offsetof(TodParticleDefinition,mEmitterDefs),        DefFieldType::DT_ARRAY,         &gEmitterDefMap },
    { "",                   0x0,        DefFieldType::DT_INVALID,       nullptr }
};
DefMap gParticleDefMap = { gParticleDefFields, sizeof(TodParticleDefinition), TodParticleDefinitionConstructor };            

DefField gReanimatorTransformDefFields[] = {            
    { "x",    offsetof(ReanimatorTransform,mTransX), DefFieldType::DT_FLOAT,         nullptr },
    { "y",    offsetof(ReanimatorTransform,mTransY), DefFieldType::DT_FLOAT,         nullptr },
    { "kx",   offsetof(ReanimatorTransform,mSkewX), DefFieldType::DT_FLOAT,         nullptr },
    { "ky",   offsetof(ReanimatorTransform,mSkewY), DefFieldType::DT_FLOAT,         nullptr },
    { "sx",   offsetof(ReanimatorTransform,mScaleX), DefFieldType::DT_FLOAT,         nullptr },
    { "sy",   offsetof(ReanimatorTransform,mScaleY), DefFieldType::DT_FLOAT,         nullptr },
    { "f",    offsetof(ReanimatorTransform,mFrame), DefFieldType::DT_FLOAT,         nullptr },
    { "a",    offsetof(ReanimatorTransform,mAlpha), DefFieldType::DT_FLOAT,         nullptr },
    { "i",    offsetof(ReanimatorTransform,mImage), DefFieldType::DT_IMAGE,         nullptr },
    { "font", offsetof(ReanimatorTransform,mFont), DefFieldType::DT_FONT,          nullptr },
    { "text", offsetof(ReanimatorTransform,mText), DefFieldType::DT_STRING,        nullptr },
    { "",     0, DefFieldType::DT_INVALID,       nullptr }
};
DefMap gReanimatorTransformDefMap = { gReanimatorTransformDefFields, sizeof(ReanimatorTransform), ReanimatorTransformConstructor };            

DefField gReanimatorTrackDefFields[] = {            
    { "name",               offsetof(ReanimatorTrack,mName),      DefFieldType::DT_STRING,        nullptr },
    { "t",                  offsetof(ReanimatorTrack,mTransforms),DefFieldType::DT_ARRAY,         &gReanimatorTransformDefMap },
    { "",                   0x0,                                  DefFieldType::DT_INVALID,       nullptr }
};
DefMap gReanimatorTrackDefMap = { gReanimatorTrackDefFields, sizeof(ReanimatorTrack), ReanimatorTrackConstructor };            

DefField gReanimatorDefFields[] = {
    { "track",              offsetof(ReanimatorDefinition,mTracks),DefFieldType::DT_ARRAY,         &gReanimatorTrackDefMap },
    { "fps",                offsetof(ReanimatorDefinition,mFPS),   DefFieldType::DT_FLOAT,         nullptr },
    { "",                   0x0,                                   DefFieldType::DT_INVALID,       nullptr }
};            
DefMap gReanimatorDefMap = { gReanimatorDefFields, sizeof(ReanimatorDefinition), ReanimatorDefinitionConstructor };            

static DefLoadResPath gDefLoadResPaths[4] = { {"IMAGE_", ""}, {"IMAGE_", "particles/"}, {"IMAGE_REANIM_", "reanim/"}, {"IMAGE_REANIM_", "images/"} };            

          
void* ParticleFieldConstructor(void* thePointer)
{
    if (thePointer)
    {
        ((ParticleField*)thePointer)->mX.mNodes = nullptr;
        ((ParticleField*)thePointer)->mX.mCountNodes = 0;
        ((ParticleField*)thePointer)->mY.mNodes = nullptr;
        ((ParticleField*)thePointer)->mY.mCountNodes = 0;
        ((ParticleField*)thePointer)->mFieldType = ParticleFieldType::FIELD_INVALID;
    }
    return thePointer;
}

          
void* TodEmitterDefinitionConstructor(void* thePointer)
{
    if (thePointer)
    {
        memset(thePointer, 0, sizeof(TodEmitterDefinition));
        ((TodEmitterDefinition*)thePointer)->mImageFrames = 1;
        ((TodEmitterDefinition*)thePointer)->mEmitterType = EmitterType::EMITTER_BOX;
        ((TodEmitterDefinition*)thePointer)->mName = "";
        ((TodEmitterDefinition*)thePointer)->mOnDuration = "";
        ((TodEmitterDefinition*)thePointer)->mImageRow = 0;
        ((TodEmitterDefinition*)thePointer)->mImageCol = 0;
        ((TodEmitterDefinition*)thePointer)->mAnimated = 0;
        ((TodEmitterDefinition*)thePointer)->mImage = nullptr;
        ((TodEmitterDefinition*)thePointer)->mParticleFieldCount = 0;
    }
    return thePointer;
}

          
void* TodParticleDefinitionConstructor(void* thePointer)
{
    if (thePointer)
    {
        ((TodParticleDefinition*)thePointer)->mEmitterDefs = nullptr;
        ((TodParticleDefinition*)thePointer)->mEmitterDefCount = 0;
    }
    return thePointer;
}

void* TrailDefinitionConstructor(void* thePointer)
{
    if (thePointer)
    {
        memset(thePointer, 0, sizeof(TrailDefinition));
        ((TrailDefinition*)thePointer)->mMinPointDistance = 1.0f;
        ((TrailDefinition*)thePointer)->mMaxPoints = 2;
        ((TrailDefinition*)thePointer)->mTrailFlags = 0U;
        ((TrailDefinition*)thePointer)->mImage = nullptr;
    }
    return thePointer;
}

          
void* ReanimatorTransformConstructor(void* thePointer)
{
    if (thePointer)
    {
        ((ReanimatorTransform*)thePointer)->mTransX = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mTransY = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mSkewX = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mSkewY = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mScaleX = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mScaleY = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mFrame = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mAlpha = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mImage = nullptr;
        ((ReanimatorTransform*)thePointer)->mFont = nullptr;
        ((ReanimatorTransform*)thePointer)->mText = "";
    }
    return thePointer;
}

          
void* ReanimatorTrackConstructor(void* thePointer)
{
    if (thePointer)
    {
        ((ReanimatorTrack*)thePointer)->mName = "";
        ((ReanimatorTrack*)thePointer)->mTransforms = { nullptr, 0 };
    }
    return thePointer;
}

          
void* ReanimatorDefinitionConstructor(void* thePointer)
{
    if (thePointer)
    {
        ((ReanimatorDefinition*)thePointer)->mTracks = { nullptr, 0 };
        ((ReanimatorDefinition*)thePointer)->mFPS = 12.0f;
        ((ReanimatorDefinition*)thePointer)->mReanimAtlas = nullptr;
    }
    return thePointer;
}

                    
unsigned int DefGetSizeString(char** theValue) {
    return strlen(*theValue) + sizeof(unsigned int);
}

unsigned int DefinitionGetArraySize(DefinitionArrayDef* theValue, DefMap* theDefMap) {
    unsigned int aResult = theValue->mArrayCount * theDefMap->mDefSize + sizeof(unsigned int);
    for (int i = 0; theValue->mArrayCount > i; ++i) {
        aResult += DefinitionGetDeepSize(theDefMap, (void*)((intptr_t)theValue->mArrayData + i * theDefMap->mDefSize));
    }
    return aResult;
}

unsigned int DefGetSizeFloatTrack(FloatParameterTrack* theValue) {
    return sizeof(FloatParameterTrackNode) * theValue->mCountNodes + sizeof(unsigned int);
}

unsigned int DefGetSizeImage(Image** theValue) {
    std::string aImagePath{};
    if (*theValue)
        TodFindImagePath(*theValue, &aImagePath);
    return aImagePath.length() + sizeof(unsigned int);
}

unsigned int DefGetSizeFont(Font** theValue) {
    std::string aFontPath{};
    if (*theValue)
        TodFindFontPath(*theValue, &aFontPath);
    return aFontPath.length() + sizeof(unsigned int);
}

unsigned int DefinitionGetDeepSize(DefMap* theDefMap, void* theDefinition) {
    unsigned int aResult = 0;
    for (DefField* aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++) {
        void* aDest = (void*)((intptr_t)theDefinition + aField->mFieldOffset);
        switch (aField->mFieldType) {
        case DefFieldType::DT_STRING:
            aResult += DefGetSizeString((char**)aDest);
            break;
        case DefFieldType::DT_ARRAY:
            aResult += DefinitionGetArraySize((DefinitionArrayDef*)aDest, (DefMap*)aField->mExtraData);
            break;
        case DefFieldType::DT_TRACK_FLOAT:
            aResult += DefGetSizeFloatTrack((FloatParameterTrack*)aDest);
            break;
        case DefFieldType::DT_IMAGE:
            aResult += DefGetSizeImage((Image**)aDest);
            break;
        case DefFieldType::DT_FONT:
            aResult += DefGetSizeFont((Font**)aDest);
            break;
        default:
            continue;
        }
    }

    return aResult;
}

unsigned int DefinitionGetSize(DefMap* theDefMap, void* theDefinition) {
    return theDefMap->mDefSize + DefinitionGetDeepSize(theDefMap, theDefinition);
}

void* DefinitionAlloc(int theSize)
{
    void* aPtr = operator new[](theSize);
    TOD_ASSERT(aPtr);
    memset(aPtr, 0, theSize);
    return aPtr;
}

          
bool DefinitionLoadImage(Image** theImage, const SexyString& theName)
{
                                                             
    if (theName.size() == 0)
    {
        *theImage = nullptr;
        return true;
    }

                                                            
    Image* anImage = (Image*)gSexyAppBase->mResourceManager->LoadImage(theName);
    if (anImage)
    {
        *theImage = anImage;
        return true;
    }

#ifdef OPENFL_HOST
                                                                                 
                                                                                
    for (const DefLoadResPath& path : gDefLoadResPaths)
    {
        const size_t prefix = strlen(path.mPrefix);
        if (prefix >= theName.size()) continue;
        SexyString candidate = path.mDirectory + theName.substr(prefix);
        SharedImageRef image = gSexyAppBase->GetSharedImage(_S("resourcepack/") + candidate);
        if ((Image*)image == nullptr) image = gSexyAppBase->GetSharedImage(_S("extension/") + candidate);
        if ((Image*)image == nullptr) image = gSexyAppBase->GetSharedImage(_S("dependency/") + candidate);
        if ((Image*)image == nullptr) image = gSexyAppBase->GetSharedImage(candidate);
        if ((Image*)image != nullptr)
        {
            *theImage = (Image*)image;
            TodAddImageToMap(&image, theName);
            TodMarkImageForSanding(*theImage);
            return true;
        }
    }
    return false;
#else
    atomic<bool> found{false};
    SharedImageRef foundImage;
    std::mutex imageMutex;

    vector<thread> ts;
    for (const DefLoadResPath& aLoadResPath : gDefLoadResPaths)
    {
        ts.emplace_back(([theName, aLoadResPath, theImage, &found, &foundImage, &imageMutex]() {
            if (found.load(memory_order_relaxed))
                return;

            int aNameLen = theName.size();
            int aPrefixLen = strlen(aLoadResPath.mPrefix);
            if (aPrefixLen < aNameLen)
            {
                SexyString aPathToTry = aLoadResPath.mDirectory + theName.substr(aPrefixLen, aNameLen);
                SharedImageRef aImageRef = gSexyAppBase->GetSharedImage(_S("resourcepack/") + aPathToTry);
                if ((Image*)aImageRef == nullptr) aImageRef = gSexyAppBase->GetSharedImage(_S("extension/") + aPathToTry);
                if ((Image*)aImageRef == nullptr) aImageRef = gSexyAppBase->GetSharedImage(_S("dependency/") + aPathToTry);
                if ((Image*)aImageRef == nullptr) aImageRef = gSexyAppBase->GetSharedImage(aPathToTry);
                if ((Image*)aImageRef != nullptr)
                {
                                                                             
                    bool expected = false;
                    if (found.compare_exchange_strong(expected, true, std::memory_order_relaxed))
                    {
                        lock_guard<mutex> lock(imageMutex);
                        foundImage = aImageRef;
                    }
                }
            }
        }));
    }

    for (auto& t : ts)
        t.join();

    if (found)
    {
        *theImage = (Image*)foundImage;
        TodAddImageToMap(&foundImage, theName);
        TodMarkImageForSanding((Image*)foundImage);
    }

    return found;
#endif

                                              
                                                    
                                                                 
     
        
       
}

          
bool DefinitionLoadFont(Font** theFont, const SexyString& theName)
{
    Font* aFont = gSexyAppBase->mResourceManager->LoadFont(SexyStringToString(theName));
    *theFont = aFont;
    return aFont != nullptr;
}

bool DefinitionLoadXML(const SexyString& theFileName, DefMap* theDefMap, void* theDefinition)
{
    return DefinitionCompileAndLoad(theFileName, theDefMap, theDefinition);
}

          
inline bool DefReadFromCacheArray(void*& theReadPtr, DefinitionArrayDef* theArray, DefMap* theDefMap)
{
    int aDefSize;
    SMemR(theReadPtr, &aDefSize, sizeof(int));                                                                            
    if (aDefSize != theDefMap->mDefSize)                                                                          
    {
        TodTrace("cache has old def: array size");
        return false;
    }
    if (theArray->mArrayCount == 0)                                               
        return true;

    int aArraySize = aDefSize * theArray->mArrayCount;
    theArray->mArrayData = DefinitionAlloc(aArraySize);                                        
    SMemR(theReadPtr, theArray->mArrayData, aArraySize);                                                                                                    
    for (int i = 0; i < theArray->mArrayCount; i++)
        if (!DefMapReadFromCache(theReadPtr, theDefMap, (void*)((intptr_t)theArray->mArrayData + theDefMap->mDefSize * i)))                                      
            return false;
    return true;
}

          
inline bool DefReadFromCacheFloatTrack(void*& theReadPtr, FloatParameterTrack* theTrack)
{
    int& aCountNodes = theTrack->mCountNodes;
    SMemR(theReadPtr, &aCountNodes, sizeof(int));
    if (aCountNodes > 0)
    {
        int aSize = aCountNodes * sizeof(FloatParameterTrackNode);
        FloatParameterTrackNode* aPtr = (FloatParameterTrackNode*)DefinitionAlloc(aSize);
        theTrack->mNodes = aPtr;
        SMemR(theReadPtr, aPtr, aSize);
    }
    return true;
}

          
inline bool DefReadFromCacheString(void*& theReadPtr, char** theString)
{
    int aLen;
    SMemR(theReadPtr, &aLen, sizeof(int));
    TOD_ASSERT(aLen >= 0 && aLen <= 100000);
    if (aLen == 0)
        *theString = (char*)"";
    else
    {
        char* aPtr = (char*)DefinitionAlloc(aLen + 1);
        *theString = aPtr;
        SMemR(theReadPtr, aPtr, aLen);
        aPtr[aLen] = '\0';
    }
    return true;
}

          
inline bool DefReadFromCacheImage(void*& theReadPtr, Image** theImage)
{
    int aLen;
    SMemR(theReadPtr, &aLen, sizeof(int));                                            
    char* aImageName = (char*)alloca(aLen + 1);                                                           
    SMemR(theReadPtr, aImageName, aLen);                                   
    aImageName[aLen] = '\0';

    *theImage = nullptr;
    return aImageName[0] == '\0' || DefinitionLoadImage(theImage, aImageName);
}

          
inline bool DefReadFromCacheFont(void*& theReadPtr, Font** theFont)
{
    int aLen;
    SMemR(theReadPtr, &aLen, sizeof(int));                                            
    char* aFontName = (char*)alloca(aLen + 1);                                                           
    SMemR(theReadPtr, aFontName, aLen);                                   
    aFontName[aLen] = '\0';

    *theFont = nullptr;
    return aFontName[0] == '\0' || DefinitionLoadFont(theFont, aFontName);
}

          
bool DefMapReadFromCache(void*& theReadPtr, DefMap* theDefMap, void* theDefinition)
{
                                                                                                 
    for (DefField* aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
    {
        bool aSucceed = true;
        void* aDest = (void*)((intptr_t)theDefinition + aField->mFieldOffset);                                   
        switch (aField->mFieldType)
        {
        case DefFieldType::DT_STRING:
            aSucceed = DefReadFromCacheString(theReadPtr, (char**)aDest);
            break;
        case DefFieldType::DT_ARRAY:
            aSucceed = DefReadFromCacheArray(theReadPtr, (DefinitionArrayDef*)aDest, (DefMap*)aField->mExtraData);
            break;
        case DefFieldType::DT_IMAGE:
            aSucceed = DefReadFromCacheImage(theReadPtr, (Image**)aDest);
            break;
        case DefFieldType::DT_FONT:
            aSucceed = DefReadFromCacheFont(theReadPtr, (Font**)aDest);
            break;
        case DefFieldType::DT_TRACK_FLOAT:
            aSucceed = DefReadFromCacheFloatTrack(theReadPtr, (FloatParameterTrack*)aDest);
            break;
        default:
            break;
        }

        if (!aSucceed)
            return false;
    }
    return true;
}

          
uint DefinitionCalcHashSymbolMap(int aSchemaHash, DefSymbol* theSymbolMap)
{
    while (theSymbolMap->mSymbolName != nullptr)
    {
        aSchemaHash = crc32(aSchemaHash, (const Bytef*)theSymbolMap->mSymbolName, strlen(theSymbolMap->mSymbolName));
        aSchemaHash = crc32(aSchemaHash, (const Bytef*)&theSymbolMap->mSymbolValue, sizeof(int));
        theSymbolMap++;
    }
    return aSchemaHash;
}

          
uint DefinitionCalcHashDefMap(int aSchemaHash, DefMap* theDefMap, TodList<DefMap*>& theProgressMaps)
{
    for (TodListNode<DefMap*>* aNode = theProgressMaps.mHead; aNode != nullptr; aNode = aNode->mNext)
        if (aNode->mValue == theDefMap)
            return aSchemaHash;
    theProgressMaps.AddTail(theDefMap);

    aSchemaHash = crc32(aSchemaHash, (Bytef*)&theDefMap->mDefSize, sizeof(int));
    for (DefField* aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
    {
        aSchemaHash = crc32(aSchemaHash, (Bytef*)&aField->mFieldType, sizeof(DefFieldType));
        aSchemaHash = crc32(aSchemaHash, (Bytef*)&aField->mFieldOffset, sizeof(int));
        switch (aField->mFieldType)
        {
        case DefFieldType::DT_ENUM:
        case DefFieldType::DT_FLAGS:
            aSchemaHash = DefinitionCalcHashSymbolMap(aSchemaHash, (DefSymbol*)aField->mExtraData);
            break;
        case DefFieldType::DT_ARRAY:
            aSchemaHash = DefinitionCalcHashDefMap(aSchemaHash, (DefMap*)aField->mExtraData, theProgressMaps);
            break;
        default:
            break;
        }
    }
    return aSchemaHash;
}

          
uint DefinitionCalcHash(DefMap* theDefMap)
{
                      
    TodList<DefMap*> aProgressMaps = TodList<DefMap*>();
    uint aResult = DefinitionCalcHashDefMap(crc32(0L, (Bytef*)Z_NULL, 0) + 1, theDefMap, aProgressMaps);

                                                               
    return aResult;
}

                                                                                                  
void* DefinitionUncompressCompiledBuffer(void* theCompressedBuffer, size_t theCompressedBufferSize, size_t& theUncompressedSize, const SexyString& theCompiledFilePath)
{
                                                                                                                                       
    if (theCompressedBufferSize < 8)
    {
        TodTrace("Compile def too small", theCompiledFilePath.c_str());
        return nullptr;
    }
                                                                                           
    CompressedDefinitionHeader* aHeader = (CompressedDefinitionHeader*)theCompressedBuffer;
    if (aHeader->mCookie != 0xDEADFED4L)
    {
        TodTrace("Compiled fire cookie wrong: %s\n", theCompiledFilePath.c_str());
        return nullptr;
    }

    Bytef* aUncompressedBuffer = (Bytef*)DefinitionAlloc(aHeader->mUncompressedSize);
    Bytef* aSrc = (Bytef*)((intptr_t)theCompressedBuffer + sizeof(CompressedDefinitionHeader));                                                  
                
    ulong aUncompressedSizeResult = aHeader->mUncompressedSize;                                               
    int aResult = uncompress(aUncompressedBuffer, &aUncompressedSizeResult, aSrc, theCompressedBufferSize - sizeof(CompressedDefinitionHeader));
    (void)aResult;                                                                
    TOD_ASSERT(aResult == Z_OK);
    TOD_ASSERT(aUncompressedSizeResult == aHeader->mUncompressedSize);
    theUncompressedSize = aHeader->mUncompressedSize;
    return aUncompressedBuffer;
}

#ifdef OPENFL_HOST
#include "../openfl/jit/compiled-definitions.hpp"
#endif

                                                                             
bool DefinitionReadCompiledFile(const SexyString& theCompiledFilePath, DefMap* theDefMap, void* theDefinition)
{
#ifdef OPENFL_HOST
    std::unique_ptr<PFILE, int(*)(PFILE*)> file(p_fopen(theCompiledFilePath.c_str(), "rb"), p_fclose);
    if (!file)
    {
        gOpenPVZDefinitionError = "Cannot open compiled definition: " + theCompiledFilePath;
        return false;
    }
    try
    {
        pvz_host_trace(("Reading compiled definition: " + theCompiledFilePath).c_str());
        if (p_fseek(file.get(), 0, SEEK_END) != 0) throw std::runtime_error("Cannot seek compiled definition");
        const long length = p_ftell(file.get());
        if (length < 8 || length > 256 * 1024 * 1024) throw std::runtime_error("Invalid compressed definition size");
        if (p_fseek(file.get(), 0, SEEK_SET) != 0) throw std::runtime_error("Cannot seek compiled definition");
        std::vector<unsigned char> compressed(static_cast<size_t>(length));
        if (p_fread(compressed.data(), 1, length, file.get()) != length)
            throw std::runtime_error("Truncated compressed definition");
        uint32_t header[2];
        std::memcpy(header, compressed.data(), sizeof(header));
        if (header[0] != 0xDEADFED4 || header[1] < 4 || header[1] > 256 * 1024 * 1024)
            throw std::runtime_error("Invalid compiled definition header");
        std::vector<unsigned char> expanded(header[1]);
        uLongf expandedSize = header[1];
        if (uncompress(expanded.data(), &expandedSize, compressed.data() + 8, compressed.size() - 8) != Z_OK ||
            expandedSize != expanded.size()) throw std::runtime_error("Invalid compressed definition payload");
        std::string reason;
        if (!OpenPVZDefinitions::read(expanded.data(), expanded.size(), theDefMap, theDefinition, reason))
            throw std::runtime_error(reason);
        return true;
    }
    catch (const std::exception& error)
    {
        gOpenPVZDefinitionError = "Compiled definition error [" + theCompiledFilePath + "]: " + error.what();
        pvz_host_trace(gOpenPVZDefinitionError.c_str());
        return false;
    }
#else
    PerfTimer aTimer;
    aTimer.Start();
    FILE* pFile = fopen(theCompiledFilePath.c_str(), "rb");
    if (!pFile) return false;

    fseek(pFile, 0, 2);                                                  
    size_t aCompressedSize = ftell(pFile);                                                           
    fseek(pFile, 0, 0);                                                  
    void* aCompressedBuffer = DefinitionAlloc(aCompressedSize);
                                                                                                                      
    bool aReadCompressedFailed = fread(aCompressedBuffer, sizeof(char), aCompressedSize, pFile) != aCompressedSize;
    fclose(pFile);                                                         
    if (aReadCompressedFailed) {                            
        TodTrace(_S("Failed to read compiled file: %s\n"), theCompiledFilePath.c_str());
        free(aCompressedBuffer);
        return false;
    }

    size_t aUncompressedSize;
    void* aUncompressedBuffer = DefinitionUncompressCompiledBuffer(aCompressedBuffer, aCompressedSize, aUncompressedSize, theCompiledFilePath);
    delete[](char*)aCompressedBuffer;
    if (!aUncompressedBuffer) return false;

    uint aDefHash = DefinitionCalcHash(theDefMap);                                                                
    if (aUncompressedSize < theDefMap->mDefSize + sizeof(uint32_t)) {
        TodTrace(_S("Compiled file size too small: %s\n"), theCompiledFilePath.c_str());
        delete[](char*)aUncompressedBuffer;
        return false;
    }                                                                                                    


                                                                                                                                   
    void* aBufferPtr = aUncompressedBuffer;
    uint32_t aCashHash;
    SMemR(aBufferPtr, &aCashHash, sizeof(uint32_t));                                  
    if (aCashHash != aDefHash) {
        TodTrace(_S("Compiled file schema wrong: %s\n"), theCompiledFilePath.c_str());
        delete[](char*)aUncompressedBuffer;
        return false;
    }                                                                         

                                             
                                                                                                                                           
                                                                                                                                                    
                                                                                                                         
    SMemR(aBufferPtr, theDefinition, theDefMap->mDefSize);
                                                                                                 
    bool aResult = DefMapReadFromCache(aBufferPtr, theDefMap, theDefinition);
    size_t aReadMemSize = (uintptr_t)aBufferPtr - (uintptr_t)aUncompressedBuffer;
    delete[](char*)aUncompressedBuffer;
    if (aResult && aReadMemSize != aUncompressedSize) {
        TodTrace(_S("Compiled file wrong size: %s\n"), theCompiledFilePath.c_str());
        return false;
    }
    return aResult;
#endif
}

          
SexyString DefinitionGetCompiledFilePathFromXMLFilePath(const SexyString& theXMLFilePath)
{
    return _S("compiled\\") + theXMLFilePath + _S(".compiled");
}

bool IsFileInPakFile(const SexyString& theFilePath)
{
    PFILE* pFile = p_fopen(theFilePath.c_str(), _S("rb"));
    bool aIsInPak = pFile && !pFile->mFP;                                                                                                                           
    if (pFile)
    {
        p_fclose(pFile);
    }
    return aIsInPak;
}

bool DefinitionIsCompiled(const SexyString& thePrefix, const SexyString& theXMLFilePath)
{
    SexyString aCompiledFilePath = thePrefix + DefinitionGetCompiledFilePathFromXMLFilePath(theXMLFilePath);
#ifdef OPENFL_HOST
                                                                               
                                                                                 
    std::unique_ptr<PFILE, int(*)(PFILE*)> compiled(p_fopen(aCompiledFilePath.c_str(), "rb"), p_fclose);
    if (!compiled)
    {
        if (thePrefix.empty() && gOpenPVZDefinitionError.empty())
        {
            gOpenPVZDefinitionError = "Cannot open compiled definition: " + aCompiledFilePath;
            char directory[32768];
            DWORD count = GetCurrentDirectoryA(sizeof(directory), directory);
            if (count > 0 && count < sizeof(directory)) gOpenPVZDefinitionError += " (working directory: " + std::string(directory) + ")";
            pvz_host_trace(gOpenPVZDefinitionError.c_str());
        }
        return false;
    }
    if (!compiled->mFP) return true;
    const SexyString aSourcePath = thePrefix + theXMLFilePath;
    std::unique_ptr<PFILE, int(*)(PFILE*)> source(p_fopen(aSourcePath.c_str(), "rb"), p_fclose);
    if (!source)
    {
        pvz_host_trace(("Using compiled-only definition: " + aCompiledFilePath).c_str());
        return true;
    }
    WIN32_FILE_ATTRIBUTE_DATA compiledInfo{}, sourceInfo{};
    if (!GetFileAttributesExA(aCompiledFilePath.c_str(), GetFileExInfoStandard, &compiledInfo) ||
        !GetFileAttributesExA(aSourcePath.c_str(), GetFileExInfoStandard, &sourceInfo))
    {
        gOpenPVZDefinitionError = "Cannot compare definition timestamps: " + aSourcePath;
        return false;                                                                     
    }
    const auto compiledTime = (uint64_t(compiledInfo.ftLastWriteTime.dwHighDateTime) << 32) | compiledInfo.ftLastWriteTime.dwLowDateTime;
    const auto sourceTime = (uint64_t(sourceInfo.ftLastWriteTime.dwHighDateTime) << 32) | sourceInfo.ftLastWriteTime.dwLowDateTime;
                                                                                 
                                                                              
    WIN32_FILE_ATTRIBUTE_DATA receiptInfo{};
    if (GetFileAttributesExA(".import-complete.json", GetFileExInfoStandard, &receiptInfo))
    {
        const auto importTime = (uint64_t(receiptInfo.ftLastWriteTime.dwHighDateTime) << 32) | receiptInfo.ftLastWriteTime.dwLowDateTime;
        if (sourceTime <= importTime)
        {
            pvz_host_trace(("Using imported compiled definition: " + aCompiledFilePath).c_str());
            return true;
        }
    }
    const bool useCompiled = sourceTime <= compiledTime;
    pvz_host_trace(((useCompiled ? "Using current compiled definition: " : "Compiling newer definition source: ") +
        (useCompiled ? aCompiledFilePath : aSourcePath)).c_str());
    return useCompiled;
#else

    if (IsFileInPakFile(aCompiledFilePath))
        return true;

    struct stat attr;

    if (stat(aCompiledFilePath.c_str(), &attr) != 0)
        return false;
    
    time_t aCompiledFileTime = attr.st_mtime;

    if (stat((thePrefix + theXMLFilePath).c_str(), &attr) != 0)
    {
        if (thePrefix == _S("")) TodTrace(_S("Can't file source file to compile '%s'"), (thePrefix + theXMLFilePath).c_str());
        return false;
    }

    time_t aXMLFileTime = attr.st_mtime;

    return aXMLFileTime <= aCompiledFileTime;
      
                                          
                                
                                                                                                                                
                 
                                                       

                                                                                                                  
     
                                                                                       
                     
     
        
                                                                                                 
      
#endif
}

void DefinitionFillWithDefaults(DefMap* theDefMap, void* theDefinition)
{
    memset(theDefinition, 0, theDefMap->mDefSize);                                           
    for (DefField* aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)                                                  
        if (aField->mFieldType == DefFieldType::DT_STRING)
            *(char**)((uintptr_t)theDefinition + aField->mFieldOffset) = (char*)"";                                                                           
}

void DefinitionXmlError(XMLParser* theXmlParser, const char* theFormat, ...)
{
    va_list argList;
    va_start(argList, theFormat);
    std::string aFormattedMessage = SexyStringToString(vformat(theFormat, argList));
    va_end(argList);
#ifdef OPENFL_HOST
    const std::string error = "Definition XML error [" + theXmlParser->GetFileName() +
        ":" + std::to_string(theXmlParser->GetCurrentLineNum()) + "]: " + aFormattedMessage;
    if (gOpenPVZDefinitionError.empty()) gOpenPVZDefinitionError = error;
    pvz_host_trace(error.c_str());
#endif

    int aLine = theXmlParser->GetCurrentLineNum();
    std::string aFileName = theXmlParser->GetFileName();
    TodTraceAndLog("%s(%d): XML Definition Error: %s\n", aFileName.c_str(), aLine, aFormattedMessage.c_str());
}

bool DefinitionReadXMLString(XMLParser* theXmlParser, SexyString& theValue)
{
    XMLElement aXMLElement;
    if (!theXmlParser->NextElement(&aXMLElement))                               
    {
        DefinitionXmlError(theXmlParser, "Missing element value");
        return false;
    }
    if (aXMLElement.mType == XMLElement::TYPE_END)                                         
        return true;
    else if (aXMLElement.mType != XMLElement::TYPE_ELEMENT)                                                                                         
    {
        DefinitionXmlError(theXmlParser, "unknown element type");
        return false;
    }

    theValue = aXMLElement.mValue;                     

    if (!theXmlParser->NextElement(&aXMLElement))                                     
    {
        DefinitionXmlError(theXmlParser, "Can't read element end");
        return false;
    }
    if (aXMLElement.mType != XMLElement::TYPE_END)                                                           
    {
        DefinitionXmlError(theXmlParser, "Missing element end");
        return false;
    }
    return true;
}

bool DefSymbolValueFromString(DefSymbol* theSymbolMap, const char* theName, int* theResultValue)
{
    while (theSymbolMap->mSymbolName != nullptr)
    {
        if (sexystricmp(theName, theSymbolMap->mSymbolName) == 0)
        {
            *theResultValue = theSymbolMap->mSymbolValue;
            return true;
        }
        theSymbolMap++;
    }
    return false;
}

bool DefinitionReadIntField(XMLParser* theXmlParser, int* theValue)
{
    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    if (sexysscanf(aStringValue.c_str(), _S("%d"), theValue) == 1)
        return true;

    DefinitionXmlError(theXmlParser, "Can't parse int value '%s'", aStringValue.c_str());
    return false;
}

bool DefinitionReadFloatField(XMLParser* theXmlParser, float* theValue)
{
    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    if (sexysscanf(aStringValue.c_str(), _S("%f"), theValue) == 1)
        return true;

    DefinitionXmlError(theXmlParser, "Can't parse float value '%s'", aStringValue.c_str());
    return false;
}

bool DefinitionReadStringField(XMLParser* theXmlParser, char** theValue)
{
    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    if (aStringValue.size() == 0)
    {
        *theValue = (char*)"";
    }
    else
    {
                                       
        *theValue = (char*)DefinitionAlloc(aStringValue.size() + 1);
        strncpy(*theValue, aStringValue.c_str(), aStringValue.size() + 1);
    }
    return true;
}

bool DefinitionReadEnumField(XMLParser* theXmlParser, int* theValue, DefSymbol* theSymbolMap)
{
    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    if (DefSymbolValueFromString(theSymbolMap, aStringValue.c_str(), theValue))
        return true;

    DefinitionXmlError(theXmlParser, "Can't parse enum value '%s'", aStringValue.c_str());
    return false;
}

bool DefinitionReadVector2Field(XMLParser* theXmlParser, SexyVector2* theValue)
{
    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    if (sexysscanf(aStringValue.c_str(), _S("%f %f"), theValue) == 1)
        return true;

    DefinitionXmlError(theXmlParser, "Can't parse vector2 value '%s'", aStringValue.c_str());
    return false;
}

bool DefinitionReadArrayField(XMLParser* theXmlParser, DefinitionArrayDef* theArray, DefField* theField)
{
    DefMap* aDefMap = (DefMap*)theField->mExtraData;

    if (theArray->mArrayCount == 0)
    {
        theArray->mArrayCount = 1;
        theArray->mArrayData = DefinitionAlloc(aDefMap->mDefSize);
    }
    else
    {
                                                                                       
                                                             
        if (theArray->mArrayCount >= 1 && (theArray->mArrayCount == 1 || ((theArray->mArrayCount & (theArray->mArrayCount - 1)) == 0)))
        {
            void* anOldData = theArray->mArrayData;
            theArray->mArrayData = DefinitionAlloc(2 * theArray->mArrayCount * aDefMap->mDefSize);
            memcpy(theArray->mArrayData, anOldData, theArray->mArrayCount * aDefMap->mDefSize);
            delete[](char*)anOldData;
        }
        theArray->mArrayCount++;
    }

    if (DefinitionLoadMap(theXmlParser, aDefMap, (unsigned char*)theArray->mArrayData + aDefMap->mDefSize * (theArray->mArrayCount - 1)))
        return true;

    DefinitionXmlError(theXmlParser, "failed to read sub def");
    return false;
}

  
                       
                                                  
                                       
                                      
                                                        
                                                                                        
                                                                                      
                                                                       
                           
                        
  

  
                          
                  
                         
  

                                             
                                                       
                                                                 
                                                                 
                                                         
                                                        
  
  

DefSymbol gDefTrackEaseSymbols[] = {
    {TodCurves::CURVE_EASE_IN_OUT_WEAK,   "EaseInOutWeak"},
    {TodCurves::CURVE_FAST_IN_OUT_WEAK,   "FastInOutWeak"},
    {TodCurves::CURVE_EASE_IN_OUT,        "EaseInOut"},
    {TodCurves::CURVE_FAST_IN_OUT,        "FastInOut"},
    {TodCurves::CURVE_EASE_IN,            "EaseIn"},
    {TodCurves::CURVE_EASE_OUT,           "EaseOut"},
    {TodCurves::CURVE_EASE_SIN_WAVE,      "EaseSinWave"},
    {TodCurves::CURVE_BOUNCE_FAST_MIDDLE, "BounceFastMiddle"},
    {TodCurves::CURVE_BOUNCE_SLOW_MIDDLE, "BounceSlowMiddle"},
    {TodCurves::CURVE_BOUNCE,             "Bounce"},
    {TodCurves::CURVE_SIN_WAVE,           "SinWave"},
    {TodCurves::CURVE_LINEAR,             "Linear"},
};

bool DefinitionReadFloatTrackField(XMLParser* theXmlParser, FloatParameterTrack* theTrack)
{
    SexyString aStringValue;

    if (!DefinitionReadXMLString(theXmlParser, aStringValue)) return false;

    float aValue = 0;
    int aLen = 0;

    const char* aStringChars = aStringValue.c_str();
    size_t anIdx = 0;

    theTrack->mCountNodes = 0;

    std::vector<FloatParameterTrackNode> aFloatTrackVec = std::vector<FloatParameterTrackNode>();
    FloatParameterTrackNode aTrackNode = FloatParameterTrackNode();
    while (true) {
        if (anIdx >= aStringValue.length()) {
            return false;
        }
        if (aStringChars[anIdx] == '\0') goto _m_break;                            

        aTrackNode.mTime = -1;
        aTrackNode.mCurveType = TodCurves::CURVE_LINEAR;
        aTrackNode.mDistribution = TodCurves::CURVE_LINEAR;

        if (aStringChars[anIdx] == '[') {
                      
            anIdx++;
            if (sexysscanf(aStringChars + anIdx, "%f%n", &aValue, &aLen) != 1) return false;             
            anIdx += aLen;
            aTrackNode.mLowValue = aValue;
            aTrackNode.mHighValue = aValue;
            if (aStringChars[anIdx] != ']') {
                anIdx++;               
                          
                for (size_t i = 0; i < sizeof(gDefTrackEaseSymbols) / sizeof(gDefTrackEaseSymbols[0]); ++i) {
                    size_t aStrLen = strlen(gDefTrackEaseSymbols[i].mSymbolName);
                    if (strncmp(gDefTrackEaseSymbols[i].mSymbolName, aStringChars + anIdx, aStrLen) == 0)                              
                    {
                        aTrackNode.mDistribution = (TodCurves)gDefTrackEaseSymbols[i].mSymbolValue;
                        anIdx += aStrLen + 1;                                                                
                        break;
                    }
                }
                switch (sexysscanf(aStringChars + anIdx, "%f%n", &aValue, &aLen))              
                {
                case 1:                           
                    anIdx += aLen;
                    aTrackNode.mHighValue = aValue;
                    break;
                case 0:                                  
                    break;
                default:                                  
                    return false;
                }
            }
            if (aStringChars[anIdx] != ']') return false;                  
            anIdx++;
            if (aStringChars[anIdx] == '\0') goto _m_break;         

            if (aStringChars[anIdx] == ',') {
                anIdx++;
                if (sexysscanf(aStringChars + anIdx, "%f%n", &aValue, &aLen) != 1) return false;         
                anIdx += aLen;
                aTrackNode.mTime = aValue * 0.01;
            }
            if (aStringChars[anIdx] == '\0') goto _m_break;         
            anIdx++;
        }
        else {
                        
            if (sexysscanf(aStringChars + anIdx, "%f%n", &aValue, &aLen) != 1) return false;                  
            anIdx += aLen;
            aTrackNode.mLowValue = aValue;
            aTrackNode.mHighValue = aValue;

            if (aStringChars[anIdx] == '\0') goto _m_break;         

            if (aStringChars[anIdx] == ',') {
                anIdx++;
                if (sexysscanf(aStringChars + anIdx, "%f%n", &aValue, &aLen) != 1) return false;         
                anIdx += aLen;
                aTrackNode.mTime = aValue * 0.01;
            }
            if (aStringChars[anIdx] == '\0') goto _m_break;         
            anIdx++;
                      
            for (size_t i = 0; i < sizeof(gDefTrackEaseSymbols) / sizeof(gDefTrackEaseSymbols[0]); ++i) {
                size_t aStrLen = strlen(gDefTrackEaseSymbols[i].mSymbolName);
                if (strncmp(gDefTrackEaseSymbols[i].mSymbolName, aStringChars + anIdx, aStrLen) == 0)              
                {
                    aTrackNode.mCurveType = (TodCurves)gDefTrackEaseSymbols[i].mSymbolValue;
                    anIdx += aStrLen;
                    if (aStringChars[anIdx] == '\0') goto _m_break;         
                    anIdx++;
                    break;
                }
            }
        }

        aFloatTrackVec.push_back(aTrackNode);
    }
_m_break:
    aFloatTrackVec.push_back(aTrackNode);

                                      
    size_t aBaseIdx = 0;
    anIdx = 0;
    float high = 0.0;
    float low = 0.0;
    do {
        for (anIdx = aBaseIdx; anIdx < aFloatTrackVec.size(); ++anIdx) {
            if (aFloatTrackVec[anIdx].mTime >= 0.0) {
                                     
                high = aFloatTrackVec[anIdx].mTime;
                goto _m_found;                                               
            }
        }
                                                     
                                                                                             
        high = 1.0;
    _m_found:
                                                  
        for (size_t i = aBaseIdx; i < anIdx; ++i) {                           
            float interp;
            if (((anIdx - 1) - aBaseIdx) != 0) interp = ((float)(i - aBaseIdx)) / ((float)((anIdx - 1) - aBaseIdx));
            else if (aBaseIdx == 0) interp = 0.0;
            else interp = 1.0;
            aFloatTrackVec[i].mTime = high * interp + low * (1 - interp);
        }
                      
        aBaseIdx = anIdx + 1;
        low = high;
    } while (aBaseIdx < aFloatTrackVec.size());


      
                                                                   
                                    
                                      
     
      

    size_t alloc_size = aFloatTrackVec.size() * sizeof(FloatParameterTrackNode);
    theTrack->mNodes = (FloatParameterTrackNode*)DefinitionAlloc(alloc_size);
    if (!theTrack->mNodes) return false;

    ::memcpy(theTrack->mNodes, aFloatTrackVec.data(), alloc_size);
    theTrack->mCountNodes = aFloatTrackVec.size();

    return true;
}

bool DefinitionReadFlagField(XMLParser* theXmlParser, const SexyString& theElementName, uint* theResultValue, DefSymbol* theSymbolMap)
{
    int aValue;
    if (!DefSymbolValueFromString(theSymbolMap, theElementName.c_str(), &aValue))
        return false;

    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    float aFlag;                                                                                                         
    if (sexysscanf(aStringValue.c_str(), _S("%f %f"), &aFlag) != 1)
    {
        DefinitionXmlError(theXmlParser, "Can't parse int value '%s'", aStringValue.c_str());
        return false;
    }

                                                            
    int aFlag_int = aFlag;

    if (theResultValue) {
        if (aFlag_int)
        {
            *theResultValue |= 1 << aValue;
        }
        else
        {
            *theResultValue &= ~(1 << aValue);
        }
    }

    return true;
}

bool DefinitionReadImageField(XMLParser* theXmlParser, Image** theImage)
{
    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    if (DefinitionLoadImage(theImage, aStringValue))
        return true;

#ifdef OPENFL_HOST
    DefinitionXmlError(theXmlParser, "Failed to find image '%s'", aStringValue.c_str());
#else
    std::string aMessgae = StrFormat("Failed to find image '%s' in %s", SexyStringToStringFast(aStringValue).c_str(), theXmlParser->GetFileName().c_str());
    TodErrorMessageBox(aMessgae.c_str(), "Missing image");
#endif

    return false;
}

bool DefinitionReadFontField(XMLParser* theXmlParser, Font** theFont)
{
    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    if (DefinitionLoadFont(theFont, aStringValue))
        return true;

    std::string aMessgae = StrFormat("Failed to find font '%s' in %s", SexyStringToStringFast(aStringValue).c_str(), theXmlParser->GetFileName().c_str());
    TodErrorMessageBox(aMessgae.c_str(), "Missing font");

    return false;
}

bool DefinitionReadField(XMLParser* theXmlParser, DefMap* theDefMap, void* theDefinition, bool* theDone)
{
    if (theXmlParser->HasFailed())
        return false;

    XMLElement aXMLElement = XMLElement();
    if (!theXmlParser->NextElement(&aXMLElement) || aXMLElement.mType == XMLElement::TYPE_END)                               
    {
        if (theXmlParser->HasFailed())
        {
            DefinitionXmlError(theXmlParser, "%s", theXmlParser->GetErrorText().c_str());
            return false;
        }
        *theDone = true;                                               
        return true;
    }
    if (aXMLElement.mType != XMLElement::TYPE_START)                                                                                                              
    {
        DefinitionXmlError(theXmlParser, "Missing element start");
        return false;
    }

    for (DefField* aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
    {
        void* pVar = (void*)((uintptr_t)theDefinition + aField->mFieldOffset);
                                              
        if (aField->mFieldType == DefFieldType::DT_FLAGS && DefinitionReadFlagField(theXmlParser, aXMLElement.mValue, (uint*)pVar, (DefSymbol*)aField->mExtraData))
            return true;

        if (sexystricmp(aXMLElement.mValue.c_str(), aField->mFieldName) == 0)                                                         
        {
            bool aSuccess;
            switch (aField->mFieldType)
            {
            case DefFieldType::DT_INT:
                aSuccess = DefinitionReadIntField(theXmlParser, (int*)pVar);
                break;
            case DefFieldType::DT_FLOAT:
                aSuccess = DefinitionReadFloatField(theXmlParser, (float*)pVar);
                break;
            case DefFieldType::DT_STRING:
                aSuccess = DefinitionReadStringField(theXmlParser, (char**)pVar);
                break;
            case DefFieldType::DT_ENUM:
                aSuccess = DefinitionReadEnumField(theXmlParser, (int*)pVar, (DefSymbol*)aField->mExtraData);
                break;
            case DefFieldType::DT_VECTOR2:
                aSuccess = DefinitionReadVector2Field(theXmlParser, (SexyVector2*)pVar);
                break;
            case DefFieldType::DT_ARRAY:
                aSuccess = DefinitionReadArrayField(theXmlParser, (DefinitionArrayDef*)pVar, aField);
                break;
            case DefFieldType::DT_TRACK_FLOAT:
                aSuccess = DefinitionReadFloatTrackField(theXmlParser, (FloatParameterTrack*)pVar);
                break;
            case DefFieldType::DT_IMAGE:
                aSuccess = DefinitionReadImageField(theXmlParser, (Image**)pVar);
                break;
            case DefFieldType::DT_FONT:
                aSuccess = DefinitionReadFontField(theXmlParser, (Font**)pVar);
                break;
            default:
                aSuccess = false;
                TOD_ASSERT(false);
                break;
            }
            if (aSuccess)
                return true;

            DefinitionXmlError(theXmlParser, "Failed to read '%s' field", aXMLElement.mValue.c_str());
            return false;
        }
    }
    DefinitionXmlError(theXmlParser, "Ignoring unknown element '%s'", aXMLElement.mValue.c_str());                                               
    return false;
}

bool DefinitionLoadMap(XMLParser* theXmlParser, DefMap* theDefMap, void* theDefinition)
{
    if (theDefMap->mConstructorFunc)
        theDefMap->mConstructorFunc(theDefinition);                                           
    else
        DefinitionFillWithDefaults(theDefMap, theDefinition);                                     

    bool aDone = false;
    while (!aDone)
        if (!DefinitionReadField(theXmlParser, theDefMap, theDefinition, &aDone))
            return false;
    return true;
}

                      
void DefWriteToCacheString(void*& theWritePtr, char** theValue) {
    unsigned int aStringSize = strlen(*theValue);
    SMemW(theWritePtr, &aStringSize, sizeof(unsigned int));
    if (aStringSize > 0)
        SMemW(theWritePtr, *theValue, aStringSize);
}

void DefWriteToCacheArray(void*& theWritePtr, DefinitionArrayDef* theValue, DefMap* theDefMap) {
    SMemW(theWritePtr, &theDefMap->mDefSize, sizeof(unsigned int));
    SMemW(theWritePtr, theValue->mArrayData, theDefMap->mDefSize * theValue->mArrayCount);
    for (int i = 0; i < theValue->mArrayCount; ++i)
        DefMapWriteToCache(theWritePtr, theDefMap, (void*)((intptr_t)theValue->mArrayData + i * theDefMap->mDefSize));
}

void DefWriteToCacheFloatTrack(void*& theWritePtr, FloatParameterTrack* theValue) {
    SMemW(theWritePtr, &theValue->mCountNodes, sizeof(unsigned int));
    if (theValue->mCountNodes > 0)
        SMemW(theWritePtr, theValue->mNodes, theValue->mCountNodes * sizeof(FloatParameterTrackNode));
}

void DefWriteToCacheImage(void*& theWritePtr, Image** theValue) {
    std::string aImageName{};
    if (*theValue)
        TodFindImagePath(*theValue, &aImageName);

    unsigned int aImageSize = aImageName.length();
    SMemW(theWritePtr, &aImageSize, sizeof(unsigned int));
    if (aImageSize > 0)
        SMemW(theWritePtr, aImageName.c_str(), aImageSize);
}

void DefWriteToCacheFont(void*& theWritePtr, Font** theValue) {
    std::string aFontName{};
    if (*theValue) {
        TodFindFontPath(*theValue, &aFontName);
    }

    unsigned int aFontSize = aFontName.length();
    SMemW(theWritePtr, &aFontSize, sizeof(unsigned int));
    if (aFontSize > 0)
        SMemW(theWritePtr, aFontName.c_str(), aFontSize);
}

void DefMapWriteToCache(void*& theWritePtr, DefMap* theDefMap, void* theDefinition) {
    for (DefField* aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++) {
        void* aDest = (void*)((intptr_t)theDefinition + aField->mFieldOffset);
        switch (aField->mFieldType) {
        case DefFieldType::DT_STRING:
            DefWriteToCacheString(theWritePtr, (char**)aDest);
            break;
        case DefFieldType::DT_ARRAY:
            DefWriteToCacheArray(theWritePtr, (DefinitionArrayDef*)aDest, (DefMap*)aField->mExtraData);
            break;
        case DefFieldType::DT_TRACK_FLOAT:
            DefWriteToCacheFloatTrack(theWritePtr, (FloatParameterTrack*)aDest);
            break;
        case DefFieldType::DT_IMAGE:
            DefWriteToCacheImage(theWritePtr, (Image**)aDest);
            break;
        case DefFieldType::DT_FONT:
            DefWriteToCacheFont(theWritePtr, (Font**)aDest);
            break;
        default:
            break;
        }
    }
}

void* DefinitionCompressCompiledBuffer(void* theBuffer, unsigned int theBufferSize, unsigned int* theResultSize) {
    uLongf aCompressedSize = compressBound(theBufferSize);
    auto aCompressedBuffer = (CompressedDefinitionHeader*)DefinitionAlloc(aCompressedSize + sizeof(CompressedDefinitionHeader));
    compress((Bytef*)((uintptr_t)aCompressedBuffer + sizeof(CompressedDefinitionHeader)), &aCompressedSize, (Bytef*)theBuffer, theBufferSize);
    aCompressedBuffer->mCookie = 0xDEADFED4;
    aCompressedBuffer->mUncompressedSize = theBufferSize;
    *theResultSize = aCompressedSize + sizeof(CompressedDefinitionHeader);
    return aCompressedBuffer;
}

bool DefinitionWriteCompiledFile(const SexyString& theCompiledFilePath, DefMap* theDefMap, void* theDefinition) {
    unsigned int aCompressedSize = 0;
    unsigned int aDefSize = DefinitionGetSize(theDefMap, theDefinition) + sizeof(unsigned int);
    void* aDefBasePtr = DefinitionAlloc(aDefSize);
    void* aDef = aDefBasePtr;
    uint aDefHash = DefinitionCalcHash(theDefMap);

    SMemW(aDef, &aDefHash, sizeof(uint));
    SMemW(aDef, theDefinition, theDefMap->mDefSize);
    DefMapWriteToCache(aDef, theDefMap, theDefinition);
    void* aCompressedDef = DefinitionCompressCompiledBuffer(aDefBasePtr, aDefSize, &aCompressedSize);

    delete[](uint*)aDefBasePtr;                                                           

    std::string aFilePath = GetFileDir(theCompiledFilePath);
    MkDir(aFilePath);

    auto aFileStream = fopen(theCompiledFilePath.c_str(), "wb");
    if (aFileStream) {
        unsigned int aBytesWritten = fwrite(aCompressedDef, 1u, aCompressedSize, aFileStream);

        delete[](char*)aCompressedDef;

        fclose(aFileStream);
        return aBytesWritten == aCompressedSize;
    }

    delete[](char*)aCompressedDef;
    return false;
}

bool DefinitionCompileFile(const SexyString theXMLFilePath, const SexyString& theCompiledFilePath, DefMap* theDefMap, void* theDefinition)
{
    XMLParser aXMLParser = XMLParser();
    if (!aXMLParser.OpenFile(theXMLFilePath))
    {
                                                                           
#ifdef OPENFL_HOST
        if (theCompiledFilePath == DefinitionGetCompiledFilePathFromXMLFilePath(theXMLFilePath))
        {
            if (gOpenPVZDefinitionError.empty()) gOpenPVZDefinitionError = "Cannot open definition source: " + theXMLFilePath;
            pvz_host_trace(("Cannot open definition source: " + theXMLFilePath).c_str());
        }
#endif
        return false;
    }
    else if (!DefinitionLoadMap(&aXMLParser, theDefMap, theDefinition))
    {
#ifdef OPENFL_HOST
        if (gOpenPVZDefinitionError.empty()) gOpenPVZDefinitionError = "Cannot parse definition source: " + theXMLFilePath;
        pvz_host_trace(gOpenPVZDefinitionError.c_str());
#endif
        return false;
    }

    const bool written = DefinitionWriteCompiledFile(theCompiledFilePath, theDefMap, theDefinition);
#ifdef OPENFL_HOST
    if (!written)
    {
        pvz_host_trace(("Cannot cache compiled definition: " + theCompiledFilePath).c_str());
    }
    return true;
#else
    return written;
#endif
}

#define _COMPILEXML
                                                                    
bool DefinitionCompileAndLoad(const SexyString& theXMLFilePath, DefMap* theDefMap, void* theDefinition)
{
#ifdef OPENFL_HOST
    gOpenPVZDefinitionError.clear();
    pvz_host_trace(("Resolving definition: " + theXMLFilePath).c_str());
#endif
#ifdef _COMPILEXML                             
    TodHesitationTrace(_S("predef"));

    SexyString aCompiledFilePathInResourcePack = _S("resourcepack\\") + DefinitionGetCompiledFilePathFromXMLFilePath(theXMLFilePath);
    if (DefinitionIsCompiled(_S("resourcepack\\"), theXMLFilePath) && DefinitionReadCompiledFile(aCompiledFilePathInResourcePack, theDefMap, theDefinition))
    {
        TodHesitationTrace(_S("loaded %s"), aCompiledFilePathInResourcePack.c_str());
        return true;
    }
    else
    {
        PerfTimer aTimer;
        aTimer.Start();
        bool aResult = DefinitionCompileFile(_S("resourcepack\\") + theXMLFilePath, aCompiledFilePathInResourcePack, theDefMap, theDefinition);
        if (aResult)
        {
            TodTrace(_S("compile %d ms:'%s'"), (int)aTimer.GetDuration(), aCompiledFilePathInResourcePack.c_str());
            TodHesitationTrace(_S("compiled %s"), aCompiledFilePathInResourcePack.c_str());
            return true;
        }
    }
    
    SexyString aCompiledFilePathInExtension = _S("extension\\") + DefinitionGetCompiledFilePathFromXMLFilePath(theXMLFilePath);
    if (DefinitionIsCompiled(_S("extension\\"), theXMLFilePath) && DefinitionReadCompiledFile(aCompiledFilePathInExtension, theDefMap, theDefinition))
    {
        TodHesitationTrace(_S("loaded %s"), aCompiledFilePathInExtension.c_str());
        return true;
    }
    else
    {
        PerfTimer aTimer;
        aTimer.Start();
        bool aResult = DefinitionCompileFile(_S("extension\\") + theXMLFilePath, aCompiledFilePathInExtension, theDefMap, theDefinition);
        if (aResult)
        {
            TodTrace(_S("compile %d ms:'%s'"), (int)aTimer.GetDuration(), aCompiledFilePathInExtension.c_str());
            TodHesitationTrace(_S("compiled %s"), aCompiledFilePathInExtension.c_str());
            return true;
        }
    }
    
    SexyString aCompiledFilePathInDependency = _S("dependency\\") + DefinitionGetCompiledFilePathFromXMLFilePath(theXMLFilePath);
    if (DefinitionIsCompiled(_S("dependency\\"), theXMLFilePath) && DefinitionReadCompiledFile(aCompiledFilePathInDependency, theDefMap, theDefinition))
    {
        TodHesitationTrace(_S("loaded %s"), aCompiledFilePathInDependency.c_str());
        return true;
    }
    else
    {
        PerfTimer aTimer;
        aTimer.Start();
        bool aResult = DefinitionCompileFile(_S("dependency\\") + theXMLFilePath, aCompiledFilePathInDependency, theDefMap, theDefinition);
        if (aResult)
        {
            TodTrace(_S("compile %d ms:'%s'"), (int)aTimer.GetDuration(), aCompiledFilePathInDependency.c_str());
            TodHesitationTrace(_S("compiled %s"), aCompiledFilePathInDependency.c_str());
            return true;
        }
    }

    SexyString aCompiledFilePath = DefinitionGetCompiledFilePathFromXMLFilePath(theXMLFilePath);
    if (DefinitionIsCompiled("", theXMLFilePath) && DefinitionReadCompiledFile(aCompiledFilePath, theDefMap, theDefinition))
    {
        TodHesitationTrace(_S("loaded %s"), aCompiledFilePath.c_str());
        return true;
    }
    else
    {
        PerfTimer aTimer;
        aTimer.Start();
        bool aResult = DefinitionCompileFile(theXMLFilePath, aCompiledFilePath, theDefMap, theDefinition);
        TodTrace(_S("compile %d ms:'%s'"), (int)aTimer.GetDuration(), aCompiledFilePath.c_str());
        TodHesitationTrace(_S("compiled %s"), aCompiledFilePath.c_str());
        return aResult;
    }

#else                          

    SexyString aCompiledFilePathInResourcePack = _S("resourcepack\\") + theXMLFilePath;
    if (DefinitionReadCompiledFile(aCompiledFilePathInResourcePack, theDefMap, theDefinition))
        return true;

    SexyString aCompiledFilePathInExtension = _S("extension\\") + theXMLFilePath;
    if (DefinitionReadCompiledFile(aCompiledFilePathInExtension, theDefMap, theDefinition))
        return true;

    SexyString aCompiledFilePathInDependency = _S("dependency\\") + theXMLFilePath;
    if (DefinitionReadCompiledFile(aCompiledFilePathInDependency, theDefMap, theDefinition))
        return true;

    SexyString aCompiledFilePath = DefinitionGetCompiledFilePathFromXMLFilePath(theXMLFilePath);
    if (DefinitionReadCompiledFile(aCompiledFilePath, theDefMap, theDefinition))
        return true;

    TodErrorMessageBox(StrFormat("missing resource %s", aCompiledFilePath.c_str()).c_str(), "Error");
    exit(0);

#endif
}

          
float FloatTrackEvaluate(FloatParameterTrack& theTrack, float theTimeValue, float theInterp)
{
    if (theTrack.mCountNodes == 0)
        return 0.0f;

    if (theTimeValue < theTrack.mNodes[0].mTime)                                                           
        return TodCurveEvaluate(theInterp, theTrack.mNodes[0].mLowValue, theTrack.mNodes[0].mHighValue, theTrack.mNodes[0].mDistribution);

    for (int i = 1; i < theTrack.mCountNodes; i++)
    {
        FloatParameterTrackNode* aNodeNxt = &theTrack.mNodes[i];
        if (theTimeValue <= aNodeNxt->mTime)                                                        
        {
            FloatParameterTrackNode* aNodeCur = &theTrack.mNodes[i - 1];
                                                                                          
            float aTimeFraction = (theTimeValue - aNodeCur->mTime) / (aNodeNxt->mTime - aNodeCur->mTime);
            float aLeftValue = TodCurveEvaluate(theInterp, aNodeCur->mLowValue, aNodeCur->mHighValue, aNodeCur->mDistribution);
            float aRightValue = TodCurveEvaluate(theInterp, aNodeNxt->mLowValue, aNodeNxt->mHighValue, aNodeNxt->mDistribution);
            return TodCurveEvaluate(aTimeFraction, aLeftValue, aRightValue, aNodeCur->mCurveType);
        }
    }

    FloatParameterTrackNode* aLastNode = &theTrack.mNodes[theTrack.mCountNodes - 1];                                                              
    return TodCurveEvaluate(theInterp, aLastNode->mLowValue, aLastNode->mHighValue, aLastNode->mDistribution);
}

          
void FloatTrackSetDefault(FloatParameterTrack& theTrack, float theValue)
{
    if (theTrack.mNodes == nullptr && theValue != 0.0f)                                                                                     
    {
        theTrack.mCountNodes = 1;                                               
        FloatParameterTrackNode* aNode = (FloatParameterTrackNode*)DefinitionAlloc(sizeof(FloatParameterTrackNode));
        theTrack.mNodes = aNode;
        aNode->mTime = 0.0f;
        aNode->mLowValue = theValue;
        aNode->mHighValue = theValue;
        aNode->mCurveType = TodCurves::CURVE_CONSTANT;
        aNode->mDistribution = TodCurves::CURVE_LINEAR;
    }
    else if (theTrack.mNodes == nullptr) {
        theTrack.mCountNodes = 0;
    }
}

bool FloatTrackIsSet(const FloatParameterTrack& theTrack)
{
    return theTrack.mCountNodes != 0 && theTrack.mNodes[0].mCurveType != TodCurves::CURVE_CONSTANT;
}

bool FloatTrackIsConstantZero(FloatParameterTrack& theTrack)
{
                                                                                                                                     
    return theTrack.mCountNodes == 0 || (theTrack.mCountNodes == 1 && theTrack.mNodes[0].mLowValue == 0.0f && theTrack.mNodes[0].mHighValue == 0.0f);
}

          
float FloatTrackEvaluateFromLastTime(FloatParameterTrack& theTrack, float theTimeValue, float theInterp)
{
    return theTimeValue < 0.0f ? 0.0f : FloatTrackEvaluate(theTrack, theTimeValue, theInterp);
}

          
void DefinitionFreeArrayField(DefinitionArrayDef* theArray, DefMap* theDefMap)
{
    for (int i = 0; i < theArray->mArrayCount; i++)
        DefinitionFreeMap(theDefMap, (void*)((intptr_t)theArray->mArrayData + theDefMap->mDefSize * i));                                      
    delete[](char*)theArray->mArrayData;
    theArray->mArrayData = nullptr;
}

          
void DefinitionFreeMap(DefMap* theDefMap, void* theDefinition)
{
                                                                  
    for (DefField* aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
    {
        void* aVar = (void*)((intptr_t)theDefinition + aField->mFieldOffset);                                   
        switch (aField->mFieldType)
        {
        case DefFieldType::DT_STRING:
                                                                                                                              
                                         
                                                                
            *(char**)aVar = nullptr;
            break;
        case DefFieldType::DT_ARRAY:
            DefinitionFreeArrayField((DefinitionArrayDef*)aVar, (DefMap*)aField->mExtraData);
            break;
        case DefFieldType::DT_TRACK_FLOAT:
            if (((FloatParameterTrack*)aVar)->mCountNodes != 0)
                delete[]((FloatParameterTrack*)aVar)->mNodes;                                      
            ((FloatParameterTrack*)aVar)->mNodes = nullptr;
            break;
        default:
            break;
        }
    }
}
