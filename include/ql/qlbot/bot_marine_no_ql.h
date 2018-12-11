#pragma once
#include "sc2api/sc2_agent.h"

#include "bot_marine.h"

class MarineBotNoQL : public MarineBot
{

    
public:
    MarineBotNoQL();
    void OnGameStart() override final;
    void OnStep() override final;
    void OnGameEnd() override final;
    
    
};
