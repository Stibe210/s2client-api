#include "qlbot/bot_marine.h"
#include "qllib/Stav.h"
#include "qllib/QInit.h"

#include "sc2api/sc2_interfaces.h"
#include "sc2lib/sc2_lib.h"

#include <iostream>
#include <cfloat>

using namespace sc2;
using namespace std;

MarineBot::MarineBot() : restarts_(0), reward(0), global_reward(0), lastAction(0), poslednySelectnuty(0)
{
	double GAMMA = 0.9;
	double ALPHA = 0.05;
	double EPSILON = 0.75;
	mfeature_ = new MarineFeature();
	state_ = new Stav(new vector<int>(6, 0));///TODO NATVRDO nasraaaaaat com to tu ide
	ql_ = new QL(state_, 6, 3, new QInit());
	ql_->SetHyperparemeters(ALPHA, GAMMA, EPSILON);
    //srand(time(nullptr)); //??co je toto za kod?
	
}

void MarineBot::OnGameStart()
{
    std::cout << "Starting a new game (" << restarts_ << " restarts)" << std::endl;
	auto units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
	tagPrveho = units[0]->tag;
	GetState();	
}

void MarineBot::OnStep()
{
	
    uint32_t game_loop = Observation()->GetGameLoop();
    auto units = Observation()->GetUnits(Unit::Alliance::Self);
    if (units.empty())
    {
        return;
    }
	auto enemy = Observation()->GetUnits(Unit::Enemy);
	if (enemy.empty()) return;
	if (units.size() == 1)
		poslednySelectnuty = 0;
	else {
		poslednySelectnuty++;
		if (poslednySelectnuty > units.size() - 1)
			poslednySelectnuty = 0;
	}
	auto unit = units[poslednySelectnuty];

	Debug()->DebugMoveCamera(unit->pos);
        Debug()->SendDebug();


		/*float pomocna = Observation()->GetScore().score_details.total_damage_dealt.life;
		pomocna += Observation()->GetScore().score_details.total_damage_dealt.shields;
		pomocna += Observation()->GetScore().score_details.total_damage_dealt.energy;
		reward = pomocna - dmg;
		global_reward += reward;
		*/
		reward = 0;
		ql_->Learn(reward, new Stav(mfeature_->to_array()), lastAction, false);//TODO Pozor momentalne to je spravene na tu minihru s dierou ktoru ma obchadzat (natvrdo bohuzial) takze velkosti stavou nesedia

		if (units.empty())
		{
			//this->Vypis(" Nemame jednotky.");
			return;//ak nemame vojakov step sa nedeje (padlo)
		}
		GetState();
		srand(time(NULL));
		int akcia = ql_->ChooseAction(false, this->state_);///TODO tu mu posli stav
		if (akcia == 0)
		{
			//Vypis("  Strategia USTUP");
			step = 30;
			this->StrategiaUstup(unit);
		}
		else if (akcia == 1)
		{
			//Vypis("  Strategia ZMEN TARGET");
			step = 100;
			this->StrategiaDopredu(unit);
		}
		else
		{
			//Vypis("  Strategia UTOC");
			step = 100;
			this->StrategiaUtoc(unit);
		}
		lastAction = akcia;
	
   
	

}

void MarineBot::OnGameEnd()
{

	++restarts_;
	if (restarts_ % 10 == 0)
	{
		this->SaveQL();
		//Priebezne uklada to je asi len docasne alebo sa potom zvacsi interval
		//ak by padlo a podobne
		cout << "Ukladam." << endl;
	}
	//cout << Observation()->GetUnitTypeData().at(static_cast<int>(sc2::UNIT_TYPEID::PROTOSS_ZEALOT)).l;

	auto vysledky = Observation()->GetResults();
	for (auto player_result : vysledky)
	{
		if (player_result.player_id == Observation()->GetPlayerID())
		{
			if (player_result.result == 0)
			{
				reward += 1000;
				global_reward += reward;
				cout << "Vyhral som. " /*<< this->ReportNaKonciHry()*/ << endl;
				ql_->Learn(reward, new Stav(mfeature_->to_array()), lastAction, false);
				break;
			}
			cout << "Prehral som. " << endl /*<< this->ReportNaKonciHry()*/ << endl;
			break;
		}
	}
	std::cout << "Game ended after: " << Observation()->GetGameLoop() << " loops " << std::endl;
}

void MarineBot::SaveQL()
{
	this->ql_->Save("marine_saveQL.csv");
}

void MarineBot::StrategiaUstup(const Unit* unit)
{
    Unit* closest_unit = nullptr;
    float distance = GetClosestEnemy(unit, closest_unit);
    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(Unit::Ally);
    auto pomocna = Observation()->GetUnitTypeData().at((*unit).unit_type).weapons;
    if (distance == pomocna[0].range)
    {
        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, unit->pos);
    }
    else
    {
        /*cout << "X najblizsieho nepriatela: " << closest_unit->pos.x << endl;
        cout << "Y najblizsieho nepriatela: " << closest_unit->pos.y << endl;
        cout << "X moje: " << unit->pos.x << endl;
        cout << "Y moje: " << unit->pos.y << endl;
        cout << "Koeficient: " << 5 / distance << endl;*/
        float x;
        float y;
        if (closest_unit->pos.x > unit->pos.x)
        {
            if (closest_unit->pos.y > unit->pos.y)
            {
                x = closest_unit->pos.x - abs(closest_unit->pos.x - unit->pos.x) * (pomocna[0].range / distance);
                y = closest_unit->pos.y - abs(closest_unit->pos.y - unit->pos.y) * (pomocna[0].range / distance);
            }
            else
            {
                x = closest_unit->pos.x - abs(closest_unit->pos.x - unit->pos.x) * (pomocna[0].range / distance);
                y = closest_unit->pos.y + abs(closest_unit->pos.y - unit->pos.y) * (pomocna[0].range / distance);
            }
        }
        else
        {
            if (closest_unit->pos.y > unit->pos.y)
            {
                x = closest_unit->pos.x + abs(closest_unit->pos.x - unit->pos.x) * (pomocna[0].range / distance);
                y = closest_unit->pos.y - abs(closest_unit->pos.y - unit->pos.y) * (pomocna[0].range / distance);
            }
            else
            {
                x = closest_unit->pos.x + abs(closest_unit->pos.x - unit->pos.x) * (5 / distance);
                y = closest_unit->pos.y + abs(closest_unit->pos.y - unit->pos.y) * (5 / distance);
            }
        }
        /*cout << "X kam by som mal ist: " << x << endl;
        cout << "Y kam by som mal ist: " << y << endl;
        cout << "Povodny distance " << distance << endl;*/
        Actions()->UnitCommand(unit, ABILITY_ID::MOVE, *new Point2D(x, y), false);
    }
}

void MarineBot::StrategiaDopredu(const Unit* unit)
{
    Unit* closest_unit = nullptr;
    GetClosestEnemy(unit, closest_unit);
    Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, closest_unit);
}

void MarineBot::StrategiaUtoc(const Unit* unit)
{
    if (!unit->orders.empty()) {
        if (unit->orders[0].ability_id != ABILITY_ID::ATTACK)
        {
            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, unit->pos);
        }
    }

}

float MarineBot::GetClosestEnemy(const Unit* source_unit, Unit*& closest_unit)
{
    auto enemy_units = Observation()->GetUnits(Unit::Enemy);
    float distance = FLT_MAX;
    for (auto& enemy_unit : enemy_units)
    {
        float current_distance = Distance2D(enemy_unit->pos, source_unit->pos);
        if (current_distance < distance)
        {
            closest_unit = const_cast<Unit*>(enemy_unit);
            distance = current_distance;
        }
    }
    return distance;
}

void MarineBot::GetState() const
{
	float totalHealth = 0.0;
	float maxHealth = 0.0;
	float totalShield = 0.0;
	float maxShield = 0.0;
	auto units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
	auto enemy_units = Observation()->GetUnits(sc2::Unit::Alliance::Enemy);
	for (auto unit : units) {
		float current_distance = 10.0;
		if (enemy_units.size() != 0)
			current_distance = Distance2D(enemy_units[0]->pos, unit->pos);
		float hpPerc = unit->health / unit->health_max;
		if (unit->tag == tagPrveho) {
			mfeature_->set_hp1(hpPerc);
			mfeature_->set_vzd1odNepriatela(current_distance);
		}
		else {
			mfeature_->set_hp2(hpPerc);
			mfeature_->set_vzd2odNepriatela(current_distance);
		}		
	}
}
