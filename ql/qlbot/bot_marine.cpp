#include "qlbot/bot_marine.h"
#include "qllib/Stav.h"
#include "qllib/QInit.h"

#include "sc2api/sc2_interfaces.h"
#include "sc2lib/sc2_lib.h"

#include <iostream>
#include <cfloat>

using namespace sc2;
using namespace std;

MarineBot::MarineBot() : restarts_(0), reward(0), global_reward(0), lastAction(0), radiusQuadrant(5)
{
	double GAMMA = 0.9;
	double ALPHA = 0.05;
	double EPSILON = 0.75;

	mfeature_ = new MarineFeature();
	state_ = new Stav(new vector<int>(2, 0));///TODO NATVRDO nasraaaaaat com to tu ide - zaujimavy koment
	ql_ = new QL(state_, 2, 2, new QInit());
	ql_->SetHyperparemeters(ALPHA, GAMMA, EPSILON);
	//ql_->Load("marine_saveQL.csv");
    //srand(time(nullptr)); //??co je toto za kod?
	
}

void MarineBot::OnGameStart()
{
    std::cout << "Starting a new game (" << restarts_ << " restarts)" << std::endl;
	auto units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
	SetFeatures(units[0]);	
}

void MarineBot::OnStep()
{	
    uint32_t game_loop = Observation()->GetGameLoop();
    auto units = Observation()->GetUnits(Unit::Alliance::Self);
    if (units.empty()) return;    
	auto enemy = Observation()->GetUnits(Unit::Enemy);
	if (enemy.empty()) return;
	
	for (auto unit : units)
	{	
		auto stred = new Point2D(
			Observation()->GetGameInfo().playable_min.x + (Observation()->GetGameInfo().playable_max.x - Observation()->GetGameInfo().playable_min.x)/2,
			Observation()->GetGameInfo().playable_min.y + (Observation()->GetGameInfo().playable_max.y - Observation()->GetGameInfo().playable_min.y)/2-2
		);
		Debug()->DebugMoveCamera(*stred);
		Debug()->SendDebug();
		/*float pomocna = Observation()->GetScore().score_details.total_damage_dealt.life;
		pomocna += Observation()->GetScore().score_details.total_damage_dealt.shields;
		pomocna += Observation()->GetScore().score_details.total_damage_dealt.energy;
		reward = pomocna - dmg;
		global_reward += reward;
		*/
		reward = 0;
		ql_->Learn(reward, new Stav(mfeature_->to_array()), lastAction, false);//TODO Pozor momentalne to je spravene na tu minihru s dierou ktoru ma obchadzat (natvrdo bohuzial) takze velkosti stavou nesedia
		
		SetFeatures(unit);
		
		srand(time(NULL)); // CO TO TU ROBI????
		int akcia = ql_->ChooseAction(false, this->state_);///TODO tu mu posli stav
		
		switch (akcia)
		{
		case 0:
			//Vypis("  Strategia USTUP");
			step = 30;
			this->StrategiaUstup(unit);
			break;
		case 1:
			//Vypis("  Strategia ZMEN TARGET");
			step = 100;
			this->StrategiaDopredu(unit);
			break;
		/*case 2:
			//Vypis("  Strategia UTOC");
			step = 100;
			this->StrategiaUtoc(unit);
			break;
		case 3:
			//Vypis("  Strategia POHYB DO KVADRANTU");
			step = 30;
			this->AkciaPohybKvadrant(unit);
			break;*/
		default:
			break;
		}
		lastAction = akcia;		
	}
}

void MarineBot::OnGameEnd()
{
	++restarts_;
	if (restarts_ % 5 == 0)
	{
		this->ql_->Save("marine_saveQL.csv");
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

/**
 * Akcia ustupu od najblizsej jednotky.
 */
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
		this->HraniceKerovanie(unit, closest_unit, x, y, 5);		
			/*cout << "X kam by som mal ist: " << x << endl;
        cout << "Y kam by som mal ist: " << y << endl;
        cout << "Povodny distance " << distance << endl;*/
        Actions()->UnitCommand(unit, ABILITY_ID::MOVE, *new Point2D(x, y), false);
    }
}

/**
 * Pohyb dopredu, teda k najblizsiemu nepriatelovi.
 */
void MarineBot::StrategiaDopredu(const Unit* unit)
{
    Unit* closest_unit = nullptr;
    GetClosestEnemy(unit, closest_unit);
    Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, closest_unit);
}

/**
 * Akcia na utok.
 */
void MarineBot::StrategiaUtoc(const Unit* unit)
{
    if (!unit->orders.empty()) {
        if (unit->orders[0].ability_id != ABILITY_ID::ATTACK)
        {
            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, unit->pos);
        }
    }
}

/*
 * Akcia pohybu.
 * Premiestni daneho marinaka do stredu kvadrantu, ktory ma najvyssiu hodnotu.
 */
void MarineBot::AkciaPohybKvadrant(const Unit* unit)
{
	float* quadrant = GetFeatureQuadrant(unit);
	float max = FLT_MIN;
	int index = -1;
	for (int i = 0; i < 4; i++)
	{
		if (quadrant[i] >= max)
		{
			max = quadrant[i];
			index = i;
		}
	}
	float x = 0;
	float y = 0;
	switch (index)
	{
	case 0:
		x = radiusQuadrant / 2;
		y = -radiusQuadrant / 2;
		break;
	case 1:
		x = radiusQuadrant / 2;
		y = radiusQuadrant / 2;
		break;
	case 2:
		x = -radiusQuadrant / 2;
		y = radiusQuadrant / 2;
		break;
	case 3:
		x = -radiusQuadrant / 2;
		y = -radiusQuadrant / 2;
		break;
	default:
		break;
	}
	Actions()->UnitCommand(unit, ABILITY_ID::MOVE, *new Point2D(unit->pos.x+x, unit->pos.y+y), false);
}

/*
 * Metoda vracia featuru kvadrantov, teda pre kazdy kvadrant hodnota "bezpecnosti" - cim vyssia, tym lepsia.
 */
float* MarineBot::GetFeatureQuadrant(const Unit* unit)
{	
	Units alliedUnits = Observation()->GetUnits(Unit::Ally);
	Units enemyUnits = Observation()->GetUnits(Unit::Enemy);
	float *quadrant = new float[4];
	for (auto alliedUnit : alliedUnits)
	{
		float distance = sqrt(pow(abs(alliedUnit->pos.x - unit->pos.x), 2.0) + pow(abs(alliedUnit->pos.y - unit->pos.y), 2.0));
		if (distance <= radiusQuadrant / 2)
			quadrant[this->GetQuadrantIndex(unit, alliedUnit)] += (radiusQuadrant / 2 -distance) / radiusQuadrant / 2;
	}
	for (auto enemyUnit : enemyUnits)
	{
		float distance = sqrt(pow(abs(enemyUnit->pos.x - unit->pos.x), 2.0) + pow(abs(enemyUnit->pos.y - unit->pos.y), 2.0));
		if (distance >= radiusQuadrant / 2)
			quadrant[this->GetQuadrantIndex(unit, enemyUnit)] -= (radiusQuadrant / 2 -distance) / radiusQuadrant / 2;
	}
	return quadrant;
}

/*
 * Vrati index kvadrantu (1. kvadrant ma index 0)
 */
int MarineBot::GetQuadrantIndex(const Unit* middle, const Unit* unit)
{
	// 4 | 1
	// -----
	// 3 | 2
	int index = -1;
	if (middle->pos.x - unit->pos.x >= 0)
		if (middle->pos.y - unit->pos.y >= 0)
			index = 3;
		else
			index = 2;
	else
		if (middle->pos.y - unit->pos.y >= 0)
			index = 0;
		else
			index = 1;
	return index;
}

/*
 * Funkcia vracia vzdialenost od najblizsieho nepriatela a taktiez ho vlozi na adresu premennej closest_unit.
 */
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

/*
 * Nastavi vsetky features danej jednotky.
 */
void MarineBot::SetFeatures(const Unit* unit)
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
			mfeature_->set_hp1(hpPerc);
			mfeature_->set_vzd1odNepriatela(current_distance);
		
		
			mfeature_->set_hp2(hpPerc);
		mfeature_->set_vzd2odNepriatela(current_distance);
		
	}
	mfeature_->set_quadrantSafety(GetFeatureQuadrant(unit));
}

void MarineBot::HraniceKerovanie(const Unit* unit, const Unit* closestUnit, float& x, float& y, int posun)
{
	auto hraciaHranicaMax = Observation()->GetGameInfo().playable_max;
	auto hraciaHranicaMin = Observation()->GetGameInfo().playable_min;
	
	float hranicaX = 0;
	float hranicaY = 0;

	if (x < hraciaHranicaMin.x)
		hranicaX = (hraciaHranicaMin.x - x) / (unit->pos.x - x);
	else if (x > hraciaHranicaMax.x)
		hranicaX = (-hraciaHranicaMax.x + x) / (-unit->pos.x + x);

	if (y < hraciaHranicaMin.y)
		hranicaY = (hraciaHranicaMin.y - y) / (unit->pos.y - y);
	else if (y > hraciaHranicaMax.y)
		hranicaY = (-hraciaHranicaMax.y + y) / (-unit->pos.y + y);

	if (hranicaX != 0 && hranicaY == 0)
	{
		if (x < hraciaHranicaMin.x)
			x = hraciaHranicaMin.x + (unit->pos.x - x)*hranicaX;
		else
			x = hraciaHranicaMax.x - (-unit->pos.x + x)*hranicaX;;
		//if ((closestUnit->pos.y - hraciaHranicaMin.y) / (hraciaHranicaMax.y - hraciaHranicaMin.y) <= 0.5)
		if (closestUnit->pos.y < unit->pos.y)
			y += posun * hranicaX;
		else
			y -= posun * hranicaX;
	}		
	else if (hranicaX == 0 && hranicaY != 0)
	{
		if (y < hraciaHranicaMin.y)
			y = hraciaHranicaMin.y + (unit->pos.y - y)*hranicaY;
		else
			y = hraciaHranicaMax.y - (-unit->pos.y + y)*hranicaY;;
		//if ((closestUnit->pos.x - hraciaHranicaMin.x) / (hraciaHranicaMax.x - hraciaHranicaMin.x) <= 0.5)
		if (closestUnit->pos.x < unit->pos.x)
			x += posun * hranicaY;
		else
			x -= posun * hranicaY;
	}		
	else if (hranicaX != 0 && hranicaY != 0)
	{
		if (hranicaX > hranicaY)
		{
			if (x < hraciaHranicaMin.x)
				x = hraciaHranicaMin.x + 0.5;
			else
				x = hraciaHranicaMax.x - 0.5;
			if (y < hraciaHranicaMin.y)
				y = hraciaHranicaMin.y + 5/*(hranicaX + hranicaY) * posun*/;
			else
				y = hraciaHranicaMax.y- 5/*(hranicaX + hranicaY) * posun*/;

		}
		else
		{
			if (y < hraciaHranicaMin.y)
				y = hraciaHranicaMin.y + 0.5;
			else
				y = hraciaHranicaMax.y - 0.5;
			if (x < hraciaHranicaMin.x)
				x = hraciaHranicaMin.x + 5/*(hranicaX + hranicaY) * posun*/;
			else
				x = hraciaHranicaMax.x - 5/*(hranicaX + hranicaY) * posun*/;
		}
	}
}
