#pragma once
#include "sc2api/sc2_agent.h"

#include "marine_feature.h"
#include "marine_test_feature.h"
#include "qllib/Stav.h"
#include "qllib/QL.h"

class MarineBot : public sc2::Agent
{
    uint32_t restarts_;
    std::unordered_map<unsigned long long, MarineFeature*> feature_;
    QL* ql_;
    Stav* state_;

    float reward;
    const float radiusQuadrant; //velkost radiusu kvadrantu
    int lastAction;
    int step;

    void SetFeatures(const sc2::Unit*, MarineFeature*&);
    
    float* GetFeatureQuadrant(const sc2::Unit*);

    void ActionMoveBack(const sc2::Unit*);
    void ActionMoveForward(const sc2::Unit*);
    void ActionAttack(const sc2::Unit*);
    void ActionMoveToQuadrant(const sc2::Unit*);

    void MoveBorderBend(const sc2::Unit*, const sc2::Unit*, float&, float&, int);

    float GetClosestEnemy(const sc2::Unit*, sc2::Unit*&);
    int GetQuadrantIndex(const sc2::Unit*, const sc2::Unit*);
    
public:
    MarineBot();
    void OnGameStart() override final;
    void OnStep() override final;
    void OnGameEnd() override final;
    
    
};
