#include <iostream>
#include "sc2api/sc2_interfaces.h"
#include "sc2lib/sc2_lib.h"
#include <limits>
#include "qllib/QInitZealot.h"
#include "sc2api/sc2_score.h"
#include "qllib/Stav.h"
#include "../../tests/feature_layers_shared.h"
#include "qlbot/bot_ql_test.h"
#include <fstream>


QlBot::QlBot(int width, int height, float square_size, bool is_learning) :
    restarts_(0), pi(atan(1) * 4)
{
    //todo: parametricke prepinanie ucenia a cesta k csv s q hodnotami
    buffer_pointer = buffer;
    buffer_size = 0;


    stepNum = 0;
    GAMMA = 0.9;
    ALPHA = 0.1;
    EPSILON = 0.7;

    this->width = width;
    this->height = height;
    this->is_learning = is_learning;
    this->square_size = square_size;
    x = 0;
    y = 0;

    zstav_ = new TestState();
    state_ = new Stav(new vector<int>(2, 0));//TODO zatial napevno .. asi odstranim z parametra take a nejako to oriesim v kniznici
    ql_ = new QL(state_, 2, 4, new QInitZealot());
    ql_->SetHyperparemeters(ALPHA, GAMMA, EPSILON);

    ql_path = "minihraJamy_ql_w_" + to_string(this->width) + "_h_" + to_string(this->height) + "_l_" + to_string(this->is_learning) +".csv";
    res_path = "minihraJamy_res_w_" + to_string(this->width) + "_h_" + to_string(this->height) + "_l_" + to_string(this->is_learning) + ".csv";
    try
    {
       
        
        ql_->Load(ql_path);
        printf("NACITANE");
        try
        {
            this->Load();
        }
        catch(const std::exception& e)
        {
            win_count = 0;
            game_count = 0;
            win_percentage = 0;
        }
    }
    catch (const std::exception& e)
    {
        win_count = 0;
        game_count = 0;
        win_percentage = 0;
        printf("NENACITANE -> uci sa od zaciatku");
       
    }

    

    time(&lastUpdate);
}


void QlBot::OnGameStart()
{
    std::cout << "Starting a new game (" << restarts_ << " restarts)" << std::endl;
    reward = 0;
    global_reward = reward;
    //Control()->SaveReplay()
    Debug()->DebugCreateUnit(sc2::UNIT_TYPEID::PROTOSS_ZEALOT, *start_point, Observation()->GetPlayerID());
    Debug()->DebugEnemyControl();
    Debug()->DebugShowMap();
    Debug()->SendDebug();
    /*auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(2s);
    auto end = std::chrono::high_resolution_clock::now();
    auto upper_left = new sc2::Point3D(units[0]->pos.x, units[0]->pos.y, units[0]->pos.z);
    auto lower_right = new sc2::Point3D(upper_left->x + 1, upper_left->y + 1, upper_left->z);
    Debug()->DebugBoxOut(*upper_left, *lower_right, sc2::Colors::Red);
    */
}

void QlBot::OnStep()
{
    uint32_t game_loop = Observation()->GetGameLoop();
    if (game_loop % 20 != 0)
    {
        return;
    }
    reward = 0;
    if (!draw && Observation()->GetGameLoop() < 10)
    {
        return;
    }
    stepNum++;
    if (stepNum % 200 == 0)
    {
        this->ql_->Save(ql_path);
        printf("ulozene\n");

        //meranie casu medzi ulozeniami
        time_t actualTime;
        time(&actualTime);
        actualTime = actualTime - lastUpdate;
        std::cout << "Cas od posledneho ulozenia: " << actualTime << std::endl;
        time(&lastUpdate);
    }
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
    //cout << units.size();
    if (!draw)
    {
        float x = units[0]->pos.x - square_size / 2;
        float y = units[0]->pos.y + square_size / 2;
        float z = units[0]->pos.z;
        for (int i = width - 1; i >= 0; i--)
        {
            for (int j = height - 1; j >= 0; j--)
            {
                auto upper_left = new sc2::Point3D(x + i * square_size, y - j * square_size, z);
                auto lower_right = new sc2::Point3D(x + (i + 1)* square_size, y - (j + 1) * square_size, z + 0.01);
                if (j == 0)
                {
                    if (i == width - 1)
                    {
                        Debug()->DebugBoxOut(*upper_left, *lower_right, sc2::Colors::Green);
                    }
                    else if (i > 0)
                    {
                        Debug()->DebugBoxOut(*upper_left, *lower_right, sc2::Colors::Red);
                    }
                    else
                    {
                        Debug()->DebugBoxOut(*upper_left, *lower_right, sc2::Colors::White);
                    }

                }
                else
                {
                    Debug()->DebugBoxOut(*upper_left, *lower_right, sc2::Colors::White);
                }


            }
        }
        Debug()->DebugMoveCamera(*new sc2::Point2D(x + width * square_size / 2, y - (height + 3) * square_size / 2));
        Debug()->SendDebug();
        draw = true;
    }
    //cout << Observation()->GetCameraPos().x << "   " << Observation()->GetCameraPos().y;
    if (units.empty())
    {
        return;
    }
    if (units[0]->orders.empty())
    {

        if (y == 0 && x != 0)
        {
            if (x == width - 1)
            {
                reward = 1;
                win_count++;
            }
            else
            {
                reward = -10;
            }
            game_count++;
            win_percentage = static_cast<double>(win_count) / static_cast<double>(game_count);

            

            x = 0;
            y = 0;
            zstav_->set_x(y);
            zstav_->set_y(x);
            Debug()->DebugKillUnit(units[0]);
            Debug()->SendDebug();

            Debug()->DebugCreateUnit(sc2::UNIT_TYPEID::PROTOSS_ZEALOT, *start_point, Observation()->GetPlayerID());
            Debug()->SendDebug();
            ql_->Learn(reward, new Stav(zstav_->to_array()), lastAction, false);
            global_reward += reward;
            reward_now += reward - 0.001*reward_now;

            (*buffer_pointer).reward = reward;
            (*buffer_pointer).reward_now = reward_now;
            (*buffer_pointer).game_count = game_count;
            (*buffer_pointer).win_count = win_count;
            (*buffer_pointer).win_percentage = win_percentage;
            //cout << "game" << game_count << endl;
            buffer_pointer++;
            buffer_size++;
            if (buffer_size > 19)
            {
                this->Save();
                buffer_size = 0;
                buffer_pointer = buffer;
            }

            printf("score %6.3f %6.3f\n", global_reward, reward_now);
            return;
        }
        ql_->Learn(reward, new Stav(zstav_->to_array()), lastAction, false);


        if (units.empty())
        {
            //this->Vypis(" Nemame jednotky.");
            return;//ak nemame vojakov step sa nedeje (padlo)
        }
        auto stav = new Stav(zstav_->to_array());
        int akcia = ql_->ChooseAction(true, stav);
        //cout << stav->toCSV() << endl;
        sc2::Units jednotkyNepriatelov = Observation()->GetUnits(sc2::Unit::Enemy);
        /*cout << "x:" << zstav_->get_x() << " y: " << zstav_->get_y() << endl;*/
        int x_new = x, y_new = y;
        //cout << akcia << endl;
        switch (akcia)
        {
        case 0: y_new = y - 1; /*cout << "hore" << endl;*/  break; //hore
        case 1: x_new = x + 1; /*cout << "doprava" << endl;*/ break; //doprava
        case 2: y_new = y + 1; /*cout << "dole" << endl; */break; //dole
        case 3: x_new = x - 1; /*cout << "dolava" << endl; */break; //dolava

        default: break;
        }
        lastAction = akcia;

        if (x_new < 0)
            return;
        if (x_new > width - 1)
            return;

        if (y_new < 0)
            return;
        if (y_new > height - 1)
            return;


        x = x_new;
        y = y_new;
        zstav_->set_x(y);
        zstav_->set_y(x);
        global_reward += reward;
        reward_now += reward - 0.001*reward_now;
        Actions()->UnitCommand(units[0], sc2::ABILITY_ID::MOVE, *new sc2::Point2D(start_point->x + x * square_size, start_point->y - y * square_size), false);
        printf("\nscore %6.3f %6.3f\n", global_reward, reward_now);


    }


}

void QlBot::UlozNaucene()
{
    this->ql_->Save("saveQL.csv");
}


void QlBot::OnGameEnd()
{
    ++restarts_;
    if (restarts_ % 1 == 0)
    {
        this->UlozNaucene();
        //Priebezne uklada to je asi len docasne alebo sa potom zvacsi interval
        //ak by padlo a podobne
        cout << "Ukladam." << endl;
    }
    //cout << Observation()->GetUnitTypeData().at(static_cast<int>(sc2::UNIT_TYPEID::PROTOSS_ZEALOT)).l;


    std::cout << "Game ended after: " << Observation()->GetGameLoop() << " loops " << std::endl;
}


string QlBot::ReportNaKonciHry()
{
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
    float friendly_hp = 0;
    for (auto unit : units)
    {
        friendly_hp += unit->health;
    }
    sc2::Units enemy_units = Observation()->GetUnits(sc2::Unit::Enemy);
    float enemy_hp = 0;
    for (auto unit : enemy_units)
    {
        enemy_hp += unit->health;
    }
    string ret_string = "Replikacia: " + to_string(restarts_) + " Pocet uceni: " + to_string(ql_->DajPocetUceni()) + "\n";
    ret_string += "Ziskana odmena: " + to_string(global_reward) + "\n";
    return ret_string;
}



QlBot::~QlBot()
{
    delete ql_;
    ql_ = nullptr;
    delete state_;
    state_ = nullptr;
    delete zstav_;
    zstav_ = nullptr;
}

QlBot::TestState::TestState()
{
    x = 0;
    y = 0;
}

QlBot::TestState::~TestState()
{
}

void QlBot::TestState::set_x(int x)
{
    this->x = x;
}

int QlBot::TestState::get_x()
{
    return x;
}

void QlBot::TestState::set_y(int y)
{
    this->y = y;
}

int QlBot::TestState::get_y()
{
    return y;
}

vector<int>* QlBot::TestState::to_array()
{
    vector<int>* pole = new vector<int>();
    pole->resize(2);
    (*pole)[0] = x;
    (*pole)[1] = y;
    return pole;
}

void QlBot::GetState() const
{
    zstav_->set_x(x);
    zstav_->set_y(y);
}

void QlBot::Triangulate(float speed, float degree, float& x, float& y)
{
    x = cos(degree * pi / 180) * speed;
    y = sin(degree * pi / 180) * speed;
}

void QlBot::Load()
{
    //todo toto nefungiruje :D
    bool prvyKoniec = false;
    ifstream file;
    file.open(res_path);
    string line;
    if (file.is_open())
    {
        //Got to the last character before EOF
        file.seekg(-1, std::ios_base::end);
        if (file.peek() == '\n')
        {
            //Start searching for \n occurrences
            file.seekg(-1, std::ios_base::cur);
            int i = file.tellg();
            for (i; i > 0; i--)
            {
                if (file.peek() == '\n')
                {
                    //Found
                    file.get();
                    break;
                }
                //Move one character back
                file.seekg(i, std::ios_base::beg);
            }
        }
        std::string lastline;
        getline(file, lastline);
        
        string buff{ "" };
        vector<string> tokens;

        for (auto n : lastline)
        {
            if (n != ',') buff += n; else
                if (n == ',' && buff != "") { tokens.push_back(buff); buff = ""; }
        }
        if (buff != "") tokens.push_back(buff);


        reward_now = stof(tokens.at(1));
        win_count = stoi(tokens.at(2));
        game_count = stoi(tokens.at(3));
        win_percentage = stod(tokens.at(4));
       /* cout << reward_now << endl;
        cout << win_count << endl;
        cout << game_count << endl;
        cout << win_percentage << endl;*/
        

        file.close();
        

    }
}

void QlBot::Save()
{
    ofstream file;
    std::ifstream infile(res_path);
    
    if (infile.good())
    {
        file.open(res_path, std::ios_base::app);
    } else
    {
        file.open(res_path, std::ios_base::app);
        file << "sep=, \n";
    }
    string pomocna;
    file << pomocna;
    auto iterator = buffer;
    for (int i = 0; i < buffer_size ; i++)
    {
        ResultRecord result = *iterator;
        pomocna = to_string(result.reward) + "," + to_string(result.reward_now) + "," + to_string(result.win_count) + "," + to_string(result.game_count) + "," + to_string(result.win_percentage) + "," + "\n";
        file << pomocna;
        iterator++;
    }

    file.close();
}






