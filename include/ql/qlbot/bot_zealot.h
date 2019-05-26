#pragma once
#include "sc2api/sc2_agent.h"
#include "zealot_state.h"
//#include "../build/QLLib/QLLib/QL.h"
#include "qllib/Stav.h"
#include "qllib/QL.h"
#include "statistic.h"

using namespace std;
class ZealotBot : public sc2::Agent
{
    long game_start;
    bool is_restarting = false;
    bool is_vs_pc;
    double alpha;
    double gamma;
    double epsilon;
    uint32_t restarts_;
    const double pi;
    float dmg;
    float hp;
    float shield;
    bool jeVypis;
    int lastAction;
    float reward;
    float global_reward;
    int start_count;
    int step;
    ZealotState* zstav_;
    map<string, Statistic*> statistics{};
    QL* ql_;
    Stav* state_;
    void get_state(const sc2::Unit& unit) const;
    void save_statistics();
    void triangulate(float speed, float degree, float& x, float& y) const;
    
    string saveFileName;
    string CreateSaveFileParameterPart(double, string);


protected:
    float GetLocalReward();
    float GetGlobalReward();
public:
    int experimentGameCount;
    int static experimentNumber;
    ZealotBot(int count = 0);
    ZealotBot(double, double, double, int count = 1, bool is_vs_pc = false);
    void Vypis(std::string);
    void OnGameStart() override final;
    void OnStep() override final;
    void OnGameEnd() override final;
    void StrategiaUstup(const sc2::Unit*, const sc2::Units);
    void StrategiaZmenTarget(const sc2::Unit*);
    void StrategiaUtoc(const sc2::Unit*);
    void HladajNepriatela(const sc2::Unit*);
    void UlozNaucene();
    string ToCSV();
    //virtual void OnUnitDestroyed(const sc2::Unit* unit) override;
    string ReportNaKonciHry();
    //vector<int>* DajStav();
    ~ZealotBot();
private:
    void EndGame();
    void StartGame();
};
