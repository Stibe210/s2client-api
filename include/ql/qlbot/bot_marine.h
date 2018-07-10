#pragma once
#include "sc2api/sc2_agent.h"

#include "marine_feature.h"
#include "qllib/Stav.h"
#include "qllib/QL.h"

class MarineBot : public sc2::Agent
{
    int tagPrveho;
    uint32_t restarts_;
    MarineFeature* mfeature_;
    QL* ql_;
    Stav* state_;

    float reward;
    float global_reward;
    int lastAction;
    int step;
    int poslednySelectnuty;

    void GetState() const;
    void SaveQL();
public:
    MarineBot();
    void OnGameStart() override final;
    void OnStep() override final;
    void OnGameEnd() override final;
    void StrategiaUstup(const sc2::Unit*);
    void StrategiaDopredu(const sc2::Unit*);
    void StrategiaUtoc(const sc2::Unit*);
    float GetClosestEnemy(const sc2::Unit*, sc2::Unit*&);
};
