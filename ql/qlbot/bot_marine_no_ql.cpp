#include "qlbot/bot_marine_no_ql.h"
#include "qllib/Stav.h"
#include "qllib/QInit.h"

#include "sc2api/sc2_interfaces.h"
#include "sc2lib/sc2_lib.h"

#include <iostream>
#include <cfloat>
#include <time.h>
#include "sc2api/sc2_score.h"
#include <fstream>

using namespace sc2;
using namespace std;

MarineBotNoQL::MarineBotNoQL()
{
	step = 10;
	//radiusQuadrant = 5;
	restarts_ = 0;
	statistics.insert({ "uspenost", new Statistic(30) });
	statistics.insert({ "dmg", new Statistic(30) });	
}

void MarineBotNoQL::OnGameStart()
{
    std::cout << "Starting a new game (" << ++restarts_ << " restarts)" << std::endl;
	Debug()->DebugMoveCamera(*new Point2D(
		Observation()->GetGameInfo().playable_min.x + (Observation()->GetGameInfo().playable_max.x - Observation()->GetGameInfo().playable_min.x) / 2,
		Observation()->GetGameInfo().playable_min.y + (Observation()->GetGameInfo().playable_max.y - Observation()->GetGameInfo().playable_min.y) / 2 - 4
	));
	Debug()->SendDebug();
}

void MarineBotNoQL::OnStep()
{	
	if (Observation()->GetGameLoop() % step != 0)
		return;
    auto alliedUnits = Observation()->GetUnits(Unit::Alliance::Self);
    if (alliedUnits.empty()) return;    
	auto enemyUnits = Observation()->GetUnits(Unit::Enemy);
	if (enemyUnits.empty()) return;
	srand(time(NULL));
	for (auto unit : alliedUnits)
	{
		int action = -1;
		Unit* enemy = nullptr;
		auto vzdZeal = this->GetClosestEnemy(unit, enemy);
		if (vzdZeal < 2)
			action = 0;
		else if (vzdZeal > 5)
			action = 1;
		else
			action = 2;			
		switch (action)
		{
		case 0:
			step = 10;
			this->ActionMoveBack(unit);
			break;
		case 1:
			step = 10;
			this->ActionMoveForward(unit);
			break;
		case 2:
			step = 10;
			this->ActionAttack(unit);
			break;
		case 3:
			step = 10;
			this->ActionMoveToQuadrant(unit);
			break;
		default:
			break;
		}
	}
}

void MarineBotNoQL::OnGameEnd()
{
	auto vysledky = Observation()->GetResults();
	for (auto player_result : vysledky)
	{
		if (player_result.player_id == Observation()->GetPlayerID())
		{
			double pomocna = Observation()->GetScore().score_details.total_damage_dealt.life;
			pomocna += Observation()->GetScore().score_details.total_damage_dealt.shields;
			statistics["dmg"]->add(pomocna);
			cout << pomocna << " dmg" << endl;
			if (player_result.result == 0)
			{
				statistics["uspenost"]->add(1);
				cout << "Vyhral som. " << endl;		
				break;
			}
			statistics["uspenost"]->add(0);
			cout << "Prehral som. " << endl;
			break;
		}
	}
	std::cout << "Game ended after: " << Observation()->GetGameLoop() << " loops " << std::endl;
}
