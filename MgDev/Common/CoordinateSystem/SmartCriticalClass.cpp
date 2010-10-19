#include "CriticalSection.h"

//utility implementation to access the central "CriticalClass critical section"
//
SmartCriticalClass::SmartCriticalClass(bool bEnter)
    :m_bEntered(bEnter)
{
    if (bEnter)
    {
        CriticalClass.Enter();
    }
}

SmartCriticalClass::~SmartCriticalClass()
{
    if (m_bEntered)
    {
        CriticalClass.Leave();
    }
}

void SmartCriticalClass::Enter()
{
    m_bEntered = true;
    CriticalClass.Enter();
}
