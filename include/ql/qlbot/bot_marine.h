#pragma once
#include "sc2api/sc2_agent.h"

#include "marine_feature.h"
#include "qllib/Stav.h"
#include "qllib/QL.h"
#include "statistic.h"

class MarineBot : public sc2::Agent
{

    protected:
    uint32_t restarts_;
    std::unordered_map<unsigned long long, MarineFeature*> feature_;
    QL* ql_;
    Stav* state_;
    map<string, Statistic*> statistics{};

    bool is_restarting;
    int unitCount;
    int enemyUnitCount;
    bool vsZealot;

    double alpha;
    double gamma;
    double epsilon;

    int actualGameCount;

    const float radiusQuadrant; //velkost radiusu kvadrantu
    int lastAction;
    int step;
    time_t startTime;
    string saveFileName;

    void SetFeatures(const sc2::Unit*, MarineFeature*&);    
    vector<float>* GetFeatureQuadrant(const sc2::Unit*);

    void ActionMoveBack(const sc2::Unit*);
    void ActionMoveForward(const sc2::Unit*);
    void ActionAttack(const sc2::Unit*);
    void ActionMoveToQuadrant(const sc2::Unit*);

    void MoveBorderBend(const sc2::Unit*, const sc2::Unit*, float&, float&, int);

    float GetClosestEnemy(const sc2::Unit*, sc2::Unit*&);
    int GetQuadrantIndex(const sc2::Unit*, const sc2::Unit*);
    void save_statistics();
    float GetGlobalReward();
    float GetLocalReward();
    string CreateSaveFileParameterPart(double, string);
    void GameStart();
    void GameEnd();
    
public:
    int experimentGameCount;
    int static experimentNumber;

    MarineBot();
    MarineBot(double, double, double);
    ~MarineBot();
    void OnGameStart() override;
    void OnStep() override;
    void OnGameEnd() override;
    string ToCSV();
    
    
};
