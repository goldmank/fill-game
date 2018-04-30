#include "Pref.h"
#include <stdio.h>
#include <cocos2d.h>

using namespace cocos2d;
using namespace std;

// =========================================================== 
// Static memeber
// ===========================================================

Pref * Pref::s_pInstance = NULL;

cocos2d::Color3B palette_1[] = { cocos2d::ccc3(253, 255, 246), cocos2d::ccc3(248, 61, 150), cocos2d::ccc3(39, 40, 45), cocos2d::ccc3(224, 247, 0) };
cocos2d::Color3B palette_2[] = { cocos2d::ccc3(247, 247, 202), cocos2d::ccc3(143, 188, 220), cocos2d::ccc3(42, 65, 83), cocos2d::ccc3(69, 114, 149) };
cocos2d::Color3B palette_3[] = { cocos2d::ccc3(100, 210, 233), cocos2d::ccc3(238, 214, 84), cocos2d::ccc3(224, 229, 203), cocos2d::ccc3(245, 134, 30) };
cocos2d::Color3B palette_4[] = { cocos2d::ccc3(240, 232, 212), cocos2d::ccc3(130, 118, 117), cocos2d::ccc3(70, 66, 70), cocos2d::ccc3(79, 104, 135) };

// ===========================================================
// Singleton methods
// ===========================================================

Pref * Pref::getInstance()
{
	if (NULL == s_pInstance)
	{
		s_pInstance = new Pref();
	}
	return s_pInstance;
}

// =========================================================== 
// Initialization & cleanup
// ===========================================================

Pref::Pref()
{
	m_iBestScore = 0;
    m_iGameCounts = 0;
    m_bSoundEnabled = 0;
    m_iCurrPallete = 0;
    m_bSoundsLoaded = false;
    m_iTimeBounsCount = 3;
    m_iFillBounsCount = 3;
    m_iSpeedBounsCount = 3;
	restore();
}

Pref::~Pref()
{
}

// =========================================================== 
// Public methods
// ===========================================================

void Pref::setBestScore(int score)
{
    m_iBestScore = score;
}

int Pref::getBestScore()
{
    return m_iBestScore;
}

bool Pref::isSoundEnabled()
{
    return m_bSoundEnabled == 1;
}

void Pref::enableSound(bool enable)
{
    m_bSoundEnabled = enable ? 1 : 0;
}

cocos2d::Color3B * Pref::getCurrPallete()
{
    switch (getCurrPalleteIndex())
    {
        case 0 : return palette_1; break;
        case 1 : return palette_2; break;
        case 2 : return palette_3; break;
        case 3 : return palette_4; break;
    }
    return palette_1;
}

void Pref::preloadSounds()
{
    if (m_bSoundsLoaded)
    {
        return;
    }
    m_bSoundsLoaded = true;
}

void Pref::save()
{
	string path = getFilePath();
	FILE *fp = fopen(path.c_str(), "w");

	if (!fp)
	{
		CCLOG("can not create file %s", path.c_str());
		return;
	}
	
	fwrite(&m_iBestScore, sizeof(int), 1, fp);
    fwrite(&m_iGameCounts, sizeof(m_iGameCounts), 1, fp);
    fwrite(&m_bSoundEnabled, sizeof(m_bSoundEnabled), 1, fp);
    fwrite(&m_iCurrPallete, sizeof(m_iCurrPallete), 1, fp);
    fwrite(&m_iTimeBounsCount, sizeof(m_iTimeBounsCount), 1, fp);
    fwrite(&m_iFillBounsCount, sizeof(m_iFillBounsCount), 1, fp);
    fwrite(&m_iSpeedBounsCount, sizeof(m_iSpeedBounsCount), 1, fp);
    
    fclose(fp);
}

// =========================================================== 
// Private methods
// ===========================================================

string Pref::getFilePath()
{
	string path("");

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	// In android, every programe has a director under /data/data.
	// The path is /data/data/ + start activity package name.
	// You can save application specific data here.
	path.append("/data/data/com.flambo.blocksrush/pref.ini");
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	// You can save file in anywhere if you have the permision.
	path.append("pref.ini");
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	// You can save file in anywhere if you have the permision.
    path.append(CCFileUtils::sharedFileUtils()->getWritablePath());
	path.append("pref.ini");
#endif

	return path;
}

void Pref::restore()
{
	string path = getFilePath();   
	FILE *fp = fopen(path.c_str(), "r");
	       
	if (!fp)
	{
		CCLOG("can not open file %s", path.c_str());
		return;
	}
	
	fread(&m_iBestScore, sizeof(int), 1, fp);
    fread(&m_iGameCounts, sizeof(m_iGameCounts), 1, fp);
    fread(&m_bSoundEnabled, sizeof(m_bSoundEnabled), 1, fp);
    fread(&m_iCurrPallete, sizeof(m_iCurrPallete), 1, fp);
    if (m_iCurrPallete < 0 || m_iCurrPallete > 4)
    {
        m_iCurrPallete = 0;
    }
    fread(&m_iTimeBounsCount, sizeof(m_iTimeBounsCount), 1, fp);
    fread(&m_iFillBounsCount, sizeof(m_iFillBounsCount), 1, fp);
    fread(&m_iSpeedBounsCount, sizeof(m_iSpeedBounsCount), 1, fp);
    
	fclose(fp);
}
