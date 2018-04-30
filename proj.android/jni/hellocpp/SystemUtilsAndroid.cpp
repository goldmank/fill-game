#include "SystemUtilsAndroid.h"
#include "stdlib.h"
#include "Pref.h"

using namespace cocos2d;

// =========================================================== 
// Constructor
// ===========================================================

SystemUtilsAndroid::SystemUtilsAndroid()
{	
}

// =========================================================== 
// Public methods
// ===========================================================
	
void SystemUtilsAndroid::showLeaderboard()
{	
	runIntParamMethod("showLeaderboard", Pref::getInstance()->getBestScore());
}

void SystemUtilsAndroid::sendScore(int score)
{
	runIntParamMethod("sendScore", score);
}

void SystemUtilsAndroid::purchase(int count)
{
	runIntParamMethod("purchase", count);
}

void SystemUtilsAndroid::shareScore(int score)
{
	runIntParamMethod("shareScore", score);
}

void SystemUtilsAndroid::rateUs()
{
	runVoidMethod("rateUs");
}

void SystemUtilsAndroid::onExit()
{
	runVoidMethod("onExit");
	exit(1);
}

void SystemUtilsAndroid::trackPage(const char * page)
{
	JavaVM * pJavaVM = JniHelper::getJavaVM();

	JNIEnv *env;
	pJavaVM->GetEnv((void **)&env, JNI_VERSION_1_6);

	jstring javaString = (jstring)env->NewStringUTF(page);

	jclass interfaceClass = env->FindClass("com/flambo/blocksrush/BlocksRun");
	jmethodID method = env->GetStaticMethodID(interfaceClass, "trackPage", "(Ljava/lang/String;)V");
	env->CallStaticVoidMethod(interfaceClass, method, javaString);
}

void SystemUtilsAndroid::trackEvent(const char * event, const char * paramName, const char * paramValue)
{
    JavaVM * pJavaVM = JniHelper::getJavaVM();
    
    JNIEnv *env;
    pJavaVM->GetEnv((void **)&env, JNI_VERSION_1_6);
    
    jstring javaString = (jstring)env->NewStringUTF(event);
    jstring javaPName = (jstring)env->NewStringUTF(paramName);
    jstring javaPValue = (jstring)env->NewStringUTF(paramValue);
    
    jclass interfaceClass = env->FindClass("com/flambo/blocksrush/BlocksRun");
    jmethodID method = env->GetStaticMethodID(interfaceClass, "trackEvent", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    env->CallStaticVoidMethod(interfaceClass, method, javaString, javaPName, javaPValue);
}

void SystemUtilsAndroid::trackTimedEvent(const char * event)
{
    JavaVM * pJavaVM = JniHelper::getJavaVM();
    
    JNIEnv *env;
    pJavaVM->GetEnv((void **)&env, JNI_VERSION_1_6);
    
    jstring javaString = (jstring)env->NewStringUTF(event);
    
    jclass interfaceClass = env->FindClass("com/flambo/blocksrush/BlocksRun");
    jmethodID method = env->GetStaticMethodID(interfaceClass, "trackTimedEvent", "(Ljava/lang/String;)V");
    env->CallStaticVoidMethod(interfaceClass, method, javaString);
}

void SystemUtilsAndroid::showAd()
{
    runVoidMethod("showAd");
}

void SystemUtilsAndroid::hideAd()
{
    runVoidMethod("hideAd");
}

void SystemUtilsAndroid::showLoadingScreen()
{
    runVoidMethod("showLoadingScreen");
}

void SystemUtilsAndroid::hideLoadingScreen()
{
    runVoidMethod("hideLoadingScreen");
}

// =========================================================== 
// Private methods
// ===========================================================

void SystemUtilsAndroid::runVoidMethod(const char * methodName)
{
	JavaVM * pJavaVM = JniHelper::getJavaVM();

	JNIEnv *env;
    pJavaVM->GetEnv((void **)&env, JNI_VERSION_1_6); 
	
	jclass interfaceClass = env->FindClass("com/flambo/blocksrush/BlocksRun");
    jmethodID method = env->GetStaticMethodID(interfaceClass, methodName, "()V");       
    env->CallStaticVoidMethod(interfaceClass, method);
}

int SystemUtilsAndroid::runIntMethod(const char * methodName)
{
	JavaVM * pJavaVM = JniHelper::getJavaVM();

	JNIEnv *env;
    pJavaVM->GetEnv((void **)&env, JNI_VERSION_1_6); 
	
	jclass interfaceClass = env->FindClass("com/flambo/blocksrush/BlocksRun");
    jmethodID method = env->GetStaticMethodID(interfaceClass, methodName, "()I");       
    return env->CallStaticIntMethod(interfaceClass, method);
}

void SystemUtilsAndroid::runIntParamMethod(const char * methodName, int param)
{
	JavaVM * pJavaVM = JniHelper::getJavaVM();

	JNIEnv *env;
	pJavaVM->GetEnv((void **)&env, JNI_VERSION_1_6);

	jclass interfaceClass = env->FindClass("com/flambo/blocksrush/BlocksRun");
	jmethodID method = env->GetStaticMethodID(interfaceClass, methodName, "(I)V");
	return env->CallStaticVoidMethod(interfaceClass, method, param);
}

void SystemUtilsAndroid::runMultiIntParamMethod(const char * methodName, int param1, int param2, int param3)
{
	JavaVM * pJavaVM = JniHelper::getJavaVM();

	JNIEnv *env;
	pJavaVM->GetEnv((void **)&env, JNI_VERSION_1_6);

	jclass interfaceClass = env->FindClass("com/flambo/blocksrush/BlocksRun");
	jmethodID method = env->GetStaticMethodID(interfaceClass, methodName, "(III)V");
	return env->CallStaticVoidMethod(interfaceClass, method, param1, param2, param3);
}
