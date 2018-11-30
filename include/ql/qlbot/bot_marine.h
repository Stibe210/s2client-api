#pragma once
#include "sc2api/sc2_agent.h"

#include "marine_feature.h"
#include "qllib/Stav.h"
#include "qllib/QL.h"

class MarineBot : public sc2::Agent
{
    uint32_t restarts_;
    MarineFeature* mfeature_;
    QL* ql_;
    Stav* state_;

    float reward;
    float global_reward;
    const float radiusQuadrant; //velkost radiusu kvadrantu
    int lastAction;
    int step;

    void SetFeatures(const sc2::Unit*);
    void SaveQL();

    int GetQuadrantIndex(const sc2::Unit*, const sc2::Unit*);
    float* GetFeatureQuadrant(const sc2::Unit*);

    void HraniceKerovanie(const sc2::Unit*, const sc2::Unit*, float&, float&, int);
public:
    MarineBot();
    void OnGameStart() override final;
    void OnStep() override final;
    void OnGameEnd() override final;
    void StrategiaUstup(const sc2::Unit*);
    void StrategiaDopredu(const sc2::Unit*);
    void StrategiaUtoc(const sc2::Unit*);
    void AkciaPohybKvadrant(const sc2::Unit*);
    float GetClosestEnemy(const sc2::Unit*, sc2::Unit*&);
};
