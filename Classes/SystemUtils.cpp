#include "SystemUtils.h"

// =========================================================== 
// Static memeber
// ===========================================================

SystemUtils * SystemUtils::s_pInstance = NULL;

// =========================================================== 
// Singleton methods
// ===========================================================

void SystemUtils::initInstance(SystemUtils * pSystemUtils)
{
	s_pInstance = pSystemUtils;
}

SystemUtils * SystemUtils::getInstance()
{
	return s_pInstance;
}

// =========================================================== 
// Protected Constructor
// ===========================================================

SystemUtils::SystemUtils()
{
}
