#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"

#include <iostream>
#include "qlbot/bot_zealot.h"
#include "qlbot/bot_marine.h"
#include "qlbot/bot_ql_test.h"

class Human : public sc2::Agent {


};

class FooBot : public sc2::Agent {
public:
    uint32_t restarts_;

    FooBot() :
        restarts_(0) {
    }

    virtual void OnGameStart() final {
        std::cout << "Starting a new game (" << restarts_ << " restarts)" << std::endl;

    };

    virtual void OnStep() final {
        uint32_t game_loop = Observation()->GetGameLoop();

        /*if (game_loop % 100 == 0) {
        sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
        for (auto& it_unit : units) {
        sc2::Point2D target = sc2::FindRandomLocation(Observation()->GetGameInfo());
        Actions()->UnitCommand(it_unit, sc2::ABILITY_ID::ATTACK, target);
        }
        sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
        for (auto& it_unit : units) {
        sc2::Point2D target = sc2::FindRandomLocation(Observation()->GetGameInfo());
        Actions()->UnitCommand(it_unit, sc2::ABILITY_ID::SMART, target);
        }
        }*/
    };

    virtual void OnGameEnd() final {
        ++restarts_;
        std::cout << "Game ended after: " << Observation()->GetGameLoop() << " loops " << std::endl;
    };

private:
};

//*************************************************************************************************
int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }
    coordinator.SetMultithreaded(true);
    // Add the custom bot, it will control the players.
    //ZealotBot zealot(1);
    ZealotBot zealot1(1);
    //ZealotBot zealot2(2);

    FooBot human;
    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &zealot1), //aj ked je to takto, tak hrac dostane kontrolu "player 1" teda marinakov
        //CreateParticipant(sc2::Race::Terran, &zealot2)
        CreateComputer(sc2::Race::Protoss)
        });
    //coordinator.SetRealtime(true);
    // Start the game.
    coordinator.LaunchStarcraft();

    // Step forward the game simulation.
    bool do_break = false;
    while (!do_break) {
        coordinator.StartGame(sc2::kMapSmallMap2);
        while (coordinator.Update() && !do_break) {
            if (sc2::PollKeyPress()) {
                std::cout << "Koncim cyklus" << std::endl;
                do_break = true;
            }
        }
    }

    return 0;
}
