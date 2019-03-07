#include "qlbot/zealot_state.h"


ZealotState::ZealotState() : hp(0), vazenaPrvaVzdialenost(0), vazenaDruhaVzdialenost(0), pocetNepriatelov(0), ciel(0)
{
}

ZealotState::~ZealotState()
{
}

void ZealotState::set_hp(float hpPer)
{
    //pod 0.1 sa mu celkom tazko dostava vela vyhrava :D 
    if (hpPer < 0.2)
    {
        hp = 1;
    }
    else if (hpPer < 0.35)
    {
        hp = 2;
    }
    else if (hpPer < 0.55)
    {
        hp = 3;
    }
    else if (hpPer < 0.85)
    {
        hp = 4;
    }
    else
    {
        hp = 5;
    }
}

int ZealotState::get_hp()
{
    return hp;
}



void ZealotState::set_najblizsi_prvy_vzd(float distance, float hp)
{
    //3.5-5 nejako malo
    auto result = distance * hp;
    if (result < 0.5)
    {
        vazenaPrvaVzdialenost = 1;
    }
    else if (result < 1)
    {
        vazenaPrvaVzdialenost = 2;
    }
    else if (result < 2)
    {
        vazenaPrvaVzdialenost = 3;
    }
    else if (result < 3)
    {
        vazenaPrvaVzdialenost = 4;
    }
    else
    {
        vazenaPrvaVzdialenost = 5;
    }
}

int ZealotState::get_najblizsi_prvy_vzd()
{
    return vazenaPrvaVzdialenost;
}

void ZealotState::set_najblizsi_druhy_vzd(float distance, float hp)
{
    //pod 5 ide celkom tazko
    auto result = distance * hp;
    if (result < 0.5)
    {
        vazenaDruhaVzdialenost = 1;
    }
    else if (result < 1)
    {
        vazenaDruhaVzdialenost = 2;
    }
    else if (result < 2)
    {
        vazenaDruhaVzdialenost = 3;
    }
    else if (result < 3)
    {
        vazenaDruhaVzdialenost = 4;
    }
    else
    {
        vazenaDruhaVzdialenost = 5;
    }
}

int ZealotState::get_najblizsi_druhy_vzd()
{
    return vazenaDruhaVzdialenost;
}

void ZealotState::set_pocet_nepriatelov(int count)
{
    pocetNepriatelov = count;
}

int ZealotState::get_pocet_nepriatelov()
{
    return pocetNepriatelov;
}

void ZealotState::set_ciel(int jednotka)
{
    ciel = jednotka;
}
int ZealotState::get_ciel()
{
    return ciel;
}

vector<int>* ZealotState::to_array()
{
    vector<int>* pole = new vector<int>();
    pole->resize(5);
    (*pole)[0] = hp;
    (*pole)[1] = vazenaPrvaVzdialenost;
    (*pole)[2] = vazenaDruhaVzdialenost;
    (*pole)[3] = pocetNepriatelov;
    (*pole)[4] = ciel;
    return pole;
}

