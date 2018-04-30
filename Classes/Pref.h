#ifndef __PREF_H__
#define __PREF_H__

#include <string>
#include <cocos2d.h>

#define COLOR_BG 0
#define COLOR_FILL 1
#define COLOR_BORDER 2
#define COLOR_ENEMY 3

class Pref
{
public:
	// =========================================================== 
    // Singleton methods
    // ===========================================================

	static Pref * getInstance();
	
	// =========================================================== 
    // Public methods
    // ===========================================================
	
	void setBestScore(int score);
    int getBestScore();
    int getBestScoreUpdateCount();
    int getNumberOfGames() { return m_iGameCounts; }
    bool isSoundEnabled();
    void enableSound(bool enable);
    int getCurrPalleteIndex() { return m_iCurrPallete; }
    void setPallete(int p) { m_iCurrPallete = p; }
    cocos2d::Color3B * getCurrPallete();
    cocos2d::Color3B getColor(int index) { return getCurrPallete()[index]; };
    void preloadSounds();
    int getTimeBounsCount() { return m_iTimeBounsCount; }
    int getFillBounsCount() { return m_iFillBounsCount; }
    int getSpeedBounsCount() { return m_iSpeedBounsCount; }
    void setTimeBounsCount(int value) { m_iTimeBounsCount = value; }
    void setFillBounsCount(int value) { m_iFillBounsCount = value; }
    void setSpeedBounsCount(int value) { m_iSpeedBounsCount = value; }
    
	void save();

private:	

	// =========================================================== 
    // Fields
    // ===========================================================

	static Pref * s_pInstance;

	int m_iBestScore;
    int m_iGameCounts;
    int m_bSoundEnabled;
    int m_iCurrPallete;
    bool m_bSoundsLoaded;
    int m_iTimeBounsCount;
    int m_iFillBounsCount;
    int m_iSpeedBounsCount;
    
	// =========================================================== 
    // Initialization & cleanup
    // ===========================================================

	Pref();
	~Pref();

	// =========================================================== 
    // Private methods
    // ===========================================================

	std::string getFilePath();	
	void restore();
};

#endif // __PREF_H__
