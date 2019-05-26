#include "qlbot/bot_marine.h"
#include "qllib/Stav.h"
#include "qllib/QInit.h"

#include "sc2api/sc2_interfaces.h"
#include "sc2lib/sc2_lib.h"

#include <iostream>
#include <cfloat>
#include <time.h>
#include "sc2api/sc2_score.h"
#include <fstream>
#include <direct.h>
#include <sstream>
#include <iomanip>

using namespace sc2;
using namespace std;





string MarineBot::CreateSaveFileParameterPart(double number, string prefix)
{
	std::ostringstream streamObj;
	streamObj << std::fixed;
	streamObj << std::setprecision(2);
	streamObj << number;
	std::string strObj = prefix + streamObj.str();
	return strObj;
}

MarineBot::MarineBot(double paAlpha, double paGamma, double paEpsilon, bool vszealot) : restarts_(0), radiusQuadrant(5), lastAction(0), step(100) {
	is_restarting = false;
	vsZealot = false;
	unitCount = 3;
	enemyUnitCount = 1;
	experimentGameCount = 0;
	alpha = paAlpha;
	gamma = paGamma;
	epsilon = paEpsilon;
	int const featureCount = 7;
	int const actionCount = 3;
	startTime = time(nullptr);
	vsZealot = vszealot;

	char* directory = "experiments";
	mkdir(directory);
	string directoryName(directory);

	saveFileName = directoryName + "/marine_ql_";
	saveFileName += CreateSaveFileParameterPart(alpha, "_a");
	saveFileName += CreateSaveFileParameterPart(gamma, "_g");
	saveFileName += CreateSaveFileParameterPart(epsilon, "_e");

	feature_ = *new std::unordered_map<unsigned long long, MarineFeature*>;
	state_ = new Stav(new vector<int>(featureCount, 0));///TODO NATVRDO nasraaaaaat com to tu ide - zaujimavy koment
	ql_ = new QL(state_, featureCount, actionCount, new QInit());
	ql_->SetHyperparemeters(alpha, gamma, epsilon);
	ql_->Load(saveFileName + ".csv");
	statistics.insert({ "winRate", new Statistic(30) });
	statistics.insert({ "reward", new Statistic(30) });
	statistics.insert({ "remainingHP", new Statistic(30) });
}

MarineBot::MarineBot() : MarineBot(0.05,0.90,0.75, true)
{
}

//Overload prepisany tusim metody OnGameStart
//Vola GameEnd
void MarineBot::OnGameStart()
{
    std::cout << "Starting a new game (" << restarts_ << " restarts)" << std::endl;
	auto units = Observation()->GetUnits(Unit::Alliance::Self);
	feature_.clear();
	for (auto unit : units)
	{
		MarineFeature* ftr = new MarineFeature();
		SetFeatures(unit, ftr);
		ftr->set_lastAction(1);
		feature_.insert(std::make_pair(unit->tag,ftr));		
	}
	Debug()->DebugMoveCamera(*new Point2D(
		Observation()->GetGameInfo().playable_min.x + (Observation()->GetGameInfo().playable_max.x - Observation()->GetGameInfo().playable_min.x) / 2,
		Observation()->GetGameInfo().playable_min.y + (Observation()->GetGameInfo().playable_max.y - Observation()->GetGameInfo().playable_min.y) / 2 - 4
	));
	Debug()->SendDebug();
}

//Metoda na spustenie hry, respektive
//ak zomru vsetky jednotky jednej strany, spusti sa znova
void MarineBot::GameStart()
{
	feature_.clear();
	sc2::Units allUnits = Observation()->GetUnits();
	if (vsZealot)
		allUnits = Observation()->GetUnits(Unit::Alliance::Self);
	if (restarts_ != 0)
	{
			for (auto unit : allUnits)
			{
				Debug()->DebugKillUnit(unit);
			}
				
		//auto start = Observation()->GetStartLocation();
		
		
			/*for (int i = 0; i < unitCount; i++)
			{

				int x = 13.5 + (rand()/ double(RAND_MAX)) * 4;
				int y = 9 + (rand() / double(RAND_MAX)) * 4;
				auto start = new const Point2D(x, y);
				Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARINE, *start, Observation()->GetPlayerID());
			}*/
			auto start = new const Point2D(15.5, 11);
			Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARINE, *start, Observation()->GetPlayerID(), 3);
		Debug()->SendDebug();
		if (!vsZealot)
		{
			auto enemyPoint = new const Point2D(15.5, 18);
			for (int i = 0; i < enemyUnitCount; i++)
			{
				
				int x = 13.5 + (rand() / double(RAND_MAX)) * 4;
				int y = 16 + (rand() / double(RAND_MAX)) * 4;
				auto start = new const Point2D(x, y);
				Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_ZEALOT, *start, 2);
			}
			Debug()->SendDebug();				
		}
	}
	
	std::cout << "Starting a new game (" << restarts_ << " restarts)" << std::endl;
	
	Debug()->DebugMoveCamera(*new Point2D(
		Observation()->GetGameInfo().playable_min.x + (Observation()->GetGameInfo().playable_max.x - Observation()->GetGameInfo().playable_min.x) / 2,
		Observation()->GetGameInfo().playable_min.y + (Observation()->GetGameInfo().playable_max.y - Observation()->GetGameInfo().playable_min.y) / 2 - 4
	));
}

//Kazdy step v hre - 
//
void MarineBot::OnStep()
{	
	if (is_restarting)
		return;	
	auto enemyUnits = Observation()->GetUnits(Unit::Enemy);
	auto alliedUnits = Observation()->GetUnits(Unit::Alliance::Self);
	auto game_loop = Observation()->GetGameLoop();
	if (enemyUnits.empty() || alliedUnits.empty())
	{
		if (game_loop % 10 == 0)
		{
			is_restarting = true;
			GameEnd();
			GameStart();
			is_restarting = false;
			return;
		}		
	}	
    
    if (game_loop % step != 0)
        return;
    if (alliedUnits.empty()) return;
	if (enemyUnits.empty()) return;

	if (feature_.empty())
	{
		auto units = Observation()->GetUnits(Unit::Alliance::Self);
		//feature_.clear();
		for (auto unit : units)
		{
			MarineFeature* ftr = new MarineFeature();
			SetFeatures(unit, ftr);
			ftr->set_lastAction(1);
			feature_.insert(std::make_pair(unit->tag, ftr));
		}
	}
	for (auto unit : alliedUnits)
	{
		auto feature = feature_[unit->tag];
		/*
		reward = 0;
		if (unit->weapon_cooldown > feature->get_weaponCDLastReward())
		{
			reward += Observation()->GetUnitTypeData().at((*unit).unit_type).weapons[0].damage_;
			feature->set_weaponCDLastReward(unit->weapon_cooldown);
			lastAction++;
		}
		reward += unit->health - feature->get_hpValue(); //rozdiel HPciek - momentalne - v minulom stave (tj negativna odmena)
		*/
		float reward = GetLocalReward(); //zakomentovany predchadzajuci kod a skuska davat globalnu odmenu ako lokalnu
		ql_->Learn(reward, new Stav(feature->to_array()), feature->get_lastAction(), false);
		SetFeatures(unit, feature);		
		const int action = ql_->ChooseAction(false, new Stav(feature->to_array()));
		switch (action)
		{
		case 0:
			step = 30;
			this->ActionMoveBack(unit);
			break;
		case 1:
			step = 100;
			this->ActionMoveForward(unit);
			break;
		case 2:
			step = 30;
			this->ActionMoveToQuadrant(unit);
			break;			
		case 3:
			step = 100;
			this->ActionAttack(unit);
			break;
		default:
			break;
		}
		feature->set_lastAction(action);
	}
}

//Overload prepisany metody OnGameEnd
//Vola GameEnd
void MarineBot::OnGameEnd()
{
	GameEnd();
}

//Ukonci hru
//Ak zomrie jedna strana - jednotky
void MarineBot::GameEnd()
{
	experimentGameCount++;
	++restarts_;
	if (restarts_ % 5 == 0)
	{
		//je mozne ze je to windows only prikaz na vytvorenie directory
		
		this->ql_->Save(saveFileName + ".csv");
		save_statistics();
		cout << "Ukladam po " << restarts_ << " hrach." << endl;
	}
	float reward = GetGlobalReward();
	
	auto units = Observation()->GetUnits(Unit::Alliance::Self);
	auto remainingHP = 0;
	for (auto unit : units)
	{
		remainingHP += unit->health;
		auto feature = feature_[unit->tag];		
		SetFeatures(unit, feature);
		ql_->Learn(reward, new Stav(feature->to_array()), feature->get_lastAction(), true);
	}
	if (units.empty())
	{
		statistics["winRate"]->add(0);		
		cout << "Prehral som. MARINAK. Odmena: " << reward << endl /*<< this->ReportNaKonciHry()*/ << endl;
	} else
	{
		statistics["winRate"]->add(1);
		cout << "Vyhral som. MARINAK. Odmena: " << reward /*<< this->ReportNaKonciHry()*/ << endl;
	}
	statistics["remainingHP"]->add(remainingHP);
	statistics["reward"]->add(reward);
	std::cout << "Game ended after: " << Observation()->GetGameLoop() << " loops " << std::endl;
}

/**
 * Akcia ustupu od najblizsej jednotky.
 */
void MarineBot::ActionMoveBack(const Unit* unit)
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
void MarineBot::ActionMoveForward(const Unit* unit)
{
	//to iste ako Attack +-, prerobit alebo vyhodit?
    Unit* closest_unit = nullptr;
    GetClosestEnemy(unit, closest_unit);
    Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, closest_unit);
}

/**
 * Akcia na utok.
 */
void MarineBot::ActionAttack(const Unit* unit)
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
void MarineBot::ActionMoveToQuadrant(const Unit* unit)
{
	vector<float> *quadrant = GetFeatureQuadrant(unit);
	float max = FLT_MIN;
	int index = -1;
	for (int i = 0; i < 4; i++)
	{
		if ((*quadrant)[i] >= max)
		{
			max = (*quadrant)[i];
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
vector<float>* MarineBot::GetFeatureQuadrant(const Unit* unit)
{	
	Units alliedUnits = Observation()->GetUnits(Unit::Alliance::Self);
	Units enemyUnits = Observation()->GetUnits(Unit::Enemy);
	vector<float> *quadrant = new vector<float>(4);
	for (auto alliedUnit : alliedUnits)
	{
		float distance = abs(Distance2D(alliedUnit->pos, unit->pos));
		//float distance = sqrt(pow(abs(alliedUnit->pos.x - unit->pos.x), 2.0) + pow(abs(alliedUnit->pos.y - unit->pos.y), 2.0));
		//if (distance <= radiusQuadrant / 2)
		//quadrant[this->GetQuadrantIndex(unit, alliedUnit)] += (radiusQuadrant / 2 - distance) / radiusQuadrant / 2;
		(*quadrant)[this->GetQuadrantIndex(unit, alliedUnit)] += (radiusQuadrant - distance) / radiusQuadrant;
	}
	for (auto enemyUnit : enemyUnits)
	{
		float distance = abs(Distance2D(enemyUnit->pos, unit->pos));
		//float distance = sqrt(pow(abs(enemyUnit->pos.x - unit->pos.x), 2.0) + pow(abs(enemyUnit->pos.y - unit->pos.y), 2.0));
		//if (distance >= radiusQuadrant / 2)
		//quadrant[this->GetQuadrantIndex(unit, enemyUnit)] -= (radiusQuadrant / 2 -distance) / radiusQuadrant / 2;
		(*quadrant)[this->GetQuadrantIndex(unit, enemyUnit)] -= 3*((radiusQuadrant - distance) / radiusQuadrant);
	}
	return quadrant;
}

/*
 * Vrati index kvadrantu (1. kvadrant ma index 0)
 *  4 | 1
 *  -----
 *  3 | 2
*/
int MarineBot::GetQuadrantIndex(const Unit* middle, const Unit* unit)
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
 * Nastavi vsetky featury danej jednotky.
 */
void MarineBot::SetFeatures(const Unit* unit, MarineFeature*& feature)
{  
	//pridal som ratanie totalneho hp vojakov a posielam namiesto aktualneho
	Unit* closest_unit = nullptr;
	float distanceFromEnemy = GetClosestEnemy(unit, closest_unit);
	auto alliedUnits = Observation()->GetUnits(Unit::Alliance::Self);
	int unitCounter = 0;
	float totalHP = 0;
	float totalHPmax = 0;
	for (auto unit : alliedUnits)
	{
		unitCounter++;
		totalHP += unit->health;
		totalHPmax += unit->health_max;
	}
		
	feature->set_marineCount(unitCounter);
	feature->set_distanceFromClosestEnemy(distanceFromEnemy);
	//feature->set_hp(unit->health / unit->health_max, unit->health);
	feature->set_hp(totalHP / totalHPmax, totalHP);
	feature->set_quadrantSafety(GetFeatureQuadrant(unit));
	feature->set_weaponCD(unit->weapon_cooldown);
	if (unit->weapon_cooldown == 0)
		feature->set_weaponCDLastReward(0);
}

/**
 * Kerovanie pri hranici mapy.
 * Meni parametre x a y.
 */
void MarineBot::MoveBorderBend(const Unit* unit, const Unit* closestUnit, float& x, float& y, const int movementSize)
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
				x = borderMin.x + 0.5f;
			else
				x = borderMax.x - 0.5f;
			if (y < borderMin.y)
				y = borderMin.y + movementSize;
			else
				y = borderMax.y- movementSize;

		}
		else
		{
			if (y < borderMin.y)
				y = borderMin.y + 0.5f;
			else
				y = borderMax.y - 0.5f;
			if (x < borderMin.x)
				x = borderMin.x + movementSize;
			else
				x = borderMax.x - movementSize;
		}
	}

}

void MarineBot::save_statistics()
{
	for (auto statistic : statistics)
	{
		//na vytvorenie directory
		char* directory = "experiments";
		mkdir(directory);
		string directoryName(directory);

		ofstream file;
		auto filename = saveFileName + "_" + statistic.first + ".csv";
		std::ifstream ifile(filename);
		if (!static_cast<bool>(ifile))
		{
			file.open(filename, std::ios_base::app);
			file << "sep=; \n";
		}
		else
		{
			file.open(filename, std::ios_base::app);
		}
		file << statistic.second->to_csv_string();
		file.close();
	}
}

//Vrati globalnu odmenu
//momentalne 50*lokalna
float MarineBot::GetGlobalReward()
{
	return GetLocalReward() * 50;
}

//Vrati lokalnu odmenu
//momentalne 
float MarineBot::GetLocalReward()
{	
	float rewardToReturn = 0;
	auto alliedUnits = Observation()->GetUnits(Unit::Alliance::Self);
	for (auto unit : alliedUnits)
		//rewardToReturn += unit->health; //marine ma 45 hp, konstantou prikladavame vacsiu dolezitost na to, ci je marine zivy, ako to, kolko ma hp
		rewardToReturn += 10;
	auto enemyUnits = Observation()->GetUnits(Unit::Enemy);
	for (auto unit : enemyUnits)
	{
		//rewardToReturn -= (unit->health); //zealot ma cca 150 hp aj so shieldom, tak trosku menej nech neni su zaporne rewardy
		//rewardToReturn -= unit->shield;
		rewardToReturn -= 30;
	}
	return rewardToReturn;
}

string MarineBot::ToCSV() {
	string ret = to_string(alpha) + ";" + to_string(gamma) + ";" + to_string(epsilon) + ";";
	for (auto stat : statistics) {
		ret += stat.second->GetLastGamesResult();
	}
	return ret;
}

MarineBot::~MarineBot() {
	delete ql_;
	ql_ = nullptr;
	delete state_;
	state_ = nullptr;
	statistics.clear();
	feature_.clear();
}