#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"

#include <iostream>
#include "qlbot/bot_marine.h"
#include "bot_marine_no_ql.h"
#include <fstream>


//*************************************************************************************************


void LaunchGame(sc2::Coordinator coordinator)
{
    
}

int main(int argc, char* argv[]) {

    srand(time(NULL));
    int pocetHier = 100;
    int pocetPokusov = 2;
    double alpha = 0.10;
    double gamma = 0.90;
    double epsilon = 0.75;
    for (int i = 0; i < pocetPokusov; i++) {
        sc2::Coordinator coordinator;
        if (!coordinator.LoadSettings(argc, argv)) {
            return 1;
        }
        coordinator.SetMultithreaded(true);
        // Add the custom bot, it will control the players.
        //ZealotBot zealot;
    
        MarineBot marine(alpha, gamma, epsilon, false);

        coordinator.SetParticipants({
            CreateParticipant(sc2::Race::Terran, &marine)/*,
            //CreateParticipant(sc2::Race::Protoss, &zealot)//*/
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
                if (marine.experimentGameCount == pocetHier) {
                    do_break = true;
                }
            }
        }

        //ulozenie do suboru

        std::ofstream file;
        file.open("experiments/bot_marine.csv", std::ios::out | std::ios::app);
        file << pocetHier << ";" << marine.ToCSV() << endl;
    }

    return 0;
}

