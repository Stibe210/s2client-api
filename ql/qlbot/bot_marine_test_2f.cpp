#include "qlbot/bot_marine_test_2f.h"
#include "qllib/Stav.h"
#include "qllib/QInit.h"

#include "sc2api/sc2_interfaces.h"
#include "sc2lib/sc2_lib.h"

#include <iostream>
#include <cfloat>
#include "sc2api/sc2_score.h"
#include <fstream>

using namespace sc2;
using namespace std;

MarineBotTest2F::MarineBotTest2F() : restarts_(0), reward(0), radiusQuadrant(5), lastAction(0), step(0)
{
	double const GAMMA = 0.9;
	double const ALPHA = 0.05;
	double const EPSILON = 0.75;
	int const featureCount = 2;
	int const actionCount = 2;

	feature_ = new MarineTestFeature();
	state_ = new Stav(new vector<int>(featureCount, 0));///TODO NATVRDO nasraaaaaat com to tu ide - zaujimavy koment
	ql_ = new QL(state_, featureCount, actionCount, new QInit());
	ql_->SetHyperparemeters(ALPHA, GAMMA, EPSILON);
	//ql_->Load("marine_saveQL.csv");
    srand(time(nullptr)); ///HALO, CO TO TU ROBI TOTO?
    statistics.insert({ "uspenost", new Statistic(30) });
    statistics.insert({ "reward", new Statistic(30) });
    statistics.insert({ "dmg", new Statistic(30) });
}

void MarineBotTest2F::OnGameStart()
{
    std::cout << "Starting a new game (" << restarts_ << " restarts)" << std::endl;
	auto units = Observation()->GetUnits(Unit::Alliance::Self);
	SetFeatures(units[0]);
	Debug()->DebugMoveCamera(*new Point2D(
		Observation()->GetGameInfo().playable_min.x + (Observation()->GetGameInfo().playable_max.x - Observation()->GetGameInfo().playable_min.x) / 2,
		Observation()->GetGameInfo().playable_min.y + (Observation()->GetGameInfo().playable_max.y - Observation()->GetGameInfo().playable_min.y) / 2 - 4
	));
	Debug()->SendDebug();
}

void MarineBotTest2F::OnStep()
{	
    auto alliedUnits = Observation()->GetUnits(Unit::Alliance::Self);
    if (alliedUnits.empty()) return;    
	auto enemyUnits = Observation()->GetUnits(Unit::Enemy);
	if (enemyUnits.empty()) return;
	//cout << "Step" << endl;
	for (auto unit : alliedUnits)
	{			
		/*float pomocna = Observation()->GetScore().score_details.total_damage_dealt.life;
		pomocna += Observation()->GetScore().score_details.total_damage_dealt.shields;
		pomocna += Observation()->GetScore().score_details.total_damage_dealt.energy;
		reward = pomocna - dmg;
		global_reward += reward;
		*/
		//*
		//cout << "Tag jednotky je: " << unit->tag << endl;
		reward = 0;
		ql_->Learn(reward, new Stav(feature_->to_array()), lastAction, false);
		//TODO Pozor momentalne to je spravene na tu minihru s dierou ktoru ma obchadzat (natvrdo bohuzial) takze velkosti stavou nesed
		SetFeatures(unit);		
		const int action = ql_->ChooseAction(false, this->state_);		
		switch (action)
		{
		case 0:
			//Vypis("Akcia USTUP");
			//step = 30;
			this->ActionMoveBack(unit);
			break;
		case 1:
			//Vypis("Akcia POHYB DOPREDU");
			//step = 100;
			this->ActionMoveForward(unit);
			break;
		case 2:
			//Vypis("Akcia UTOC");
			//step = 100;
			this->ActionAttack(unit);
			break;
		case 3:
			//Vypis("  Strategia POHYB DO KVADRANTU");
			//step = 30;
			this->ActionMoveToQuadrant(unit);
			break;
		default:
			break;
		}
		lastAction = action;
	}
}

void MarineBotTest2F::OnGameEnd()
{
	++restarts_;
	if (restarts_ % 5 == 0)
	{
		this->ql_->Save("marine_saveQL_2f.csv");
        save_statistics();
		cout << "Ukladam po " << restarts_ << "hrach." << endl;
	}
	
	auto vysledky = Observation()->GetResults();
	for (auto player_result : vysledky)
	{
		if (player_result.player_id == Observation()->GetPlayerID())
		{
            double pomocna = Observation()->GetScore().score_details.total_damage_dealt.life;
            pomocna += Observation()->GetScore().score_details.total_damage_dealt.shields;
            statistics["dmg"]->add(pomocna);
			if (player_result.result == 0)
			{
                statistics["uspenost"]->add(1);
			    reward += 1000;
                statistics["reward"]->add(reward);
				cout << "Vyhral som. " /*<< this->ReportNaKonciHry()*/ << endl;
                ql_->Learn(reward, new Stav(feature_->to_array()), lastAction, true);
				break;
			} 
            statistics["uspenost"]->add(0);
            statistics["reward"]->add(reward);
			cout << "Prehral som. " << endl /*<< this->ReportNaKonciHry()*/ << endl;
			break;
		}
	}
	std::cout << "Game ended after: " << Observation()->GetGameLoop() << " loops " << std::endl;
}

/**
 * Akcia ustupu od najblizsej jednotky.
 */
void MarineBotTest2F::ActionMoveBack(const Unit* unit)
{
    Unit* closest_unit = nullptr;
	Units units = Observation()->GetUnits(Unit::Ally);
	float distanceFromEnemy = GetClosestEnemy(unit, closest_unit);
    auto weaponRange = Observation()->GetUnitTypeData().at((*unit).unit_type).weapons[0].range;
	
	/*
    if (distance == pomocna[0].range)
    {
        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, unit->pos);
    }
    else
    {*/
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
            x = closest_unit->pos.x - abs(closest_unit->pos.x - unit->pos.x) * (weaponRange / distanceFromEnemy);
            y = closest_unit->pos.y - abs(closest_unit->pos.y - unit->pos.y) * (weaponRange / distanceFromEnemy);
        }
        else
        {
            x = closest_unit->pos.x - abs(closest_unit->pos.x - unit->pos.x) * (weaponRange / distanceFromEnemy);
            y = closest_unit->pos.y + abs(closest_unit->pos.y - unit->pos.y) * (weaponRange / distanceFromEnemy);
        }
    }
    else
    {
		if (closest_unit->pos.y > unit->pos.y)
		{
			x = closest_unit->pos.x + abs(closest_unit->pos.x - unit->pos.x) * (weaponRange / distanceFromEnemy);
			y = closest_unit->pos.y - abs(closest_unit->pos.y - unit->pos.y) * (weaponRange / distanceFromEnemy);
		}
        else
        {
            x = closest_unit->pos.x + abs(closest_unit->pos.x - unit->pos.x) * (weaponRange / distanceFromEnemy);
            y = closest_unit->pos.y + abs(closest_unit->pos.y - unit->pos.y) * (weaponRange / distanceFromEnemy);
        }
    }
	this->MoveBorderBend(unit, closest_unit, x, y, 5);		
	/*cout << "X kam by som mal ist: " << x << endl;
    cout << "Y kam by som mal ist: " << y << endl;
    cout << "Povodny distance " << distance << endl;*/
    Actions()->UnitCommand(unit, ABILITY_ID::MOVE, *new Point2D(x, y), false);
    //}
}

/**
 * Pohyb dopredu, teda k najblizsiemu nepriatelovi.
 */
void MarineBotTest2F::ActionMoveForward(const Unit* unit)
{
	//to iste ako Attack +-, prerobit alebo vyhodit?
    Unit* closest_unit = nullptr;
    GetClosestEnemy(unit, closest_unit);
    Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, closest_unit);
}

/**
 * Akcia na utok.
 */
void MarineBotTest2F::ActionAttack(const Unit* unit)
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
void MarineBotTest2F::ActionMoveToQuadrant(const Unit* unit)
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
 * Momentalne je ako hodnota zvolena vzdialenost ovahovana medzi 0 a 5 (ak je blizko ku stredu, ma vyssiu vahu)
 * Allied jendotky davaju kladne cislo, nepriatelske jednotky davaju zaporne cislo * 3 (kedze cca 3x marine = 1 zealot)
 */
float* MarineBotTest2F::GetFeatureQuadrant(const Unit* unit)
{	
	Units alliedUnits = Observation()->GetUnits(Unit::Ally);
	Units enemyUnits = Observation()->GetUnits(Unit::Enemy);
	float *quadrant = new float[4];
	for (auto alliedUnit : alliedUnits)
	{
		float distance = abs(Distance2D(alliedUnit->pos, unit->pos));
		//float distance = sqrt(pow(abs(alliedUnit->pos.x - unit->pos.x), 2.0) + pow(abs(alliedUnit->pos.y - unit->pos.y), 2.0));
		//if (distance <= radiusQuadrant / 2)
		//quadrant[this->GetQuadrantIndex(unit, alliedUnit)] += (radiusQuadrant / 2 - distance) / radiusQuadrant / 2;
		quadrant[this->GetQuadrantIndex(unit, alliedUnit)] += (radiusQuadrant - distance) / radiusQuadrant;
	}
	for (auto enemyUnit : enemyUnits)
	{
		float distance = abs(Distance2D(enemyUnit->pos, unit->pos));
		//float distance = sqrt(pow(abs(enemyUnit->pos.x - unit->pos.x), 2.0) + pow(abs(enemyUnit->pos.y - unit->pos.y), 2.0));
		//if (distance >= radiusQuadrant / 2)
		//quadrant[this->GetQuadrantIndex(unit, enemyUnit)] -= (radiusQuadrant / 2 -distance) / radiusQuadrant / 2;
		quadrant[this->GetQuadrantIndex(unit, enemyUnit)] -= 3*((radiusQuadrant - distance) / radiusQuadrant);
	}
	return quadrant;
}

/*
 * Vrati index kvadrantu (1. kvadrant ma index 0)
 *  4 | 1
 *  -----
 *  3 | 2
*/
int MarineBotTest2F::GetQuadrantIndex(const Unit* middle, const Unit* unit)
{	
	int index;
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

void MarineBotTest2F::save_statistics()
{
    for (auto statistic : statistics)
    {
        ofstream file;
        auto filename = statistic.first + ".csv";
        std::ifstream ifile(filename);
        if(!static_cast<bool>(ifile))
        {
            file.open(filename, std::ios_base::app);
            file << "sep=; \n";
        } else
        {
            file.open(filename, std::ios_base::app);
        }
        
        
        file << statistic.second->to_csv_string();
        file.close();
    }
}

/*
 * Funkcia vracia vzdialenost od najblizsieho nepriatela a taktiez ho vlozi na adresu premennej closest_unit.
 */
float MarineBotTest2F::GetClosestEnemy(const Unit* source_unit, Unit*& closest_unit)
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
 * Nastavi vsetky featury danej jednotky.
 */
void MarineBotTest2F::SetFeatures(const Unit* unit)
{  
	float health;
	float max_health;
	float distance = FLT_MAX;
	auto enemy_units = Observation()->GetUnits(sc2::Unit::Alliance::Enemy);
	feature_->set_hp(unit->health / unit->health_max);

	for (auto enemy : enemy_units)
	{
		if (Distance2D(enemy->pos, unit->pos) < distance)
			distance = Distance2D(enemy->pos, unit->pos);
	}
	feature_->set_enemy_dist(distance);
}

/**
 * Kerovanie pri hranici mapy.
 * Meni parametre x a y.
 */
void MarineBotTest2F::MoveBorderBend(const Unit* unit, const Unit* closestUnit, float& x, float& y, const int movementSize)
{	
	auto const borderMax = Observation()->GetGameInfo().playable_max;
	auto const borderMin = Observation()->GetGameInfo().playable_min;
	
	float xOffLength = 0;//kolko z vzdialenosti medzi X a poziciou jednotky je za hraciou hranicu. <0,1>
	float yOffLength = 0;//kolko z vzdialenosti medzi Y a poziciou jednotky je za hraciou hranicu. <0,1>

	if (x < borderMin.x)
		xOffLength = (borderMin.x - x) / (unit->pos.x - x);
	else if (x > borderMax.x)
		xOffLength = (-borderMax.x + x) / (-unit->pos.x + x);

	if (y < borderMin.y)
		yOffLength = (borderMin.y - y) / (unit->pos.y - y);
	else if (y > borderMax.y)
		yOffLength = (-borderMax.y + y) / (-unit->pos.y + y);

	if (xOffLength != 0 && yOffLength == 0)
	{
		if (x < borderMin.x)
			x = borderMin.x + (unit->pos.x - x)*xOffLength;
		else
			x = borderMax.x - (-unit->pos.x + x)*xOffLength;;
		//if ((closestUnit->pos.y - hraciaHranicaMin.y) / (hraciaHranicaMax.y - hraciaHranicaMin.y) <= 0.5)
		if (closestUnit->pos.y < unit->pos.y)
			y += movementSize * xOffLength;
		else
			y -= movementSize * xOffLength;
	}		
	else if (xOffLength == 0 && yOffLength != 0)
	{
		if (y < borderMin.y)
			y = borderMin.y + (unit->pos.y - y)*yOffLength;
		else
			y = borderMax.y - (-unit->pos.y + y)*yOffLength;;
		//if ((closestUnit->pos.x - hraciaHranicaMin.x) / (hraciaHranicaMax.x - hraciaHranicaMin.x) <= 0.5)
		if (closestUnit->pos.x < unit->pos.x)
			x += movementSize * yOffLength;
		else
			x -= movementSize * yOffLength;
	}		
	else if (xOffLength != 0 && yOffLength != 0)
	{
		if (xOffLength > yOffLength)
		{
			if (x < borderMin.x)
				x = borderMin.x + 0.5;
			else
				x = borderMax.x - 0.5;
			if (y < borderMin.y)
				y = borderMin.y + 5/*(hranicaX + hranicaY) * movementSize*/;
			else
				y = borderMax.y- 5/*(hranicaX + hranicaY) * movementSize*/;

		}
		else
		{
			if (y < borderMin.y)
				y = borderMin.y + 0.5;
			else
				y = borderMax.y - 0.5;
			if (x < borderMin.x)
				x = borderMin.x + 5/*(hranicaX + hranicaY) * movementSize*/;
			else
				x = borderMax.x - 5/*(hranicaX + hranicaY) * movementSize*/;
		}
	}

}
