#pragma once
#include "Stav.h"


class QInit;

class QL
{
private:
    int pocetUceni;
    void UpravHyperparametre();

    Stav* zaciatocnyStav;
    Stav* minulyStav;
    int minulaAkcia;

    int algaritmus = 1; //0 sarsa 1 q learning

    int pocetAkcii;//2 stavy -> 0-1 .. magic numbers mozno prerobit na enum ale boh vie ako to tu vsetko funguje :D :D 
    int pocetFeatur;


    float GAMMA = 0.9;//zlava
    float EPSILON = 0.75;
    float ALPHA = 0.05;//learning rate
    float zmenaALPHA = 1;// 0.9999;
    float zmenaEPSILON = 0.9998;// 0.999;
    QInit* qInit = nullptr;

    int lcgM = 2147483647;
    int lcgA = 48271;
    int lcgC = 0;
    unsigned long int lcgX = 0;





    //QL::QStav stav;
    

public:
	QL(Stav* stav, int pocFeatur, int pocetAkcii, QInit* qin);
	~QL();
	void Learn(int odmena,Stav* stav,int akcia, bool koniec);
	int ChooseAction(bool vybratNajlepsie, Stav* aktualnyStav);
    void Save(string path);
    void Load(string path);
    void SetHyperparemeters(float alpha, float gamma, float epsilon);
    int DajPocetUceni();
    void PremazMinulyStav();
    void ZmenAlgaritmus(int algaritmus);
    double VypocitajNahodneCislo();
    
	class QStav
	{
	private:
		//Obsahuje Q hodnoty akcii v danom stave
		vector<float>* qHodnoty;
        vector<int> pristupy;
		public:
            QStav(int pocetAkcii, QInit* qin);
            QStav(vector<float>* qHodnoty, vector<int> prist);
            ~QStav();
            vector<float>* DajQHodnoty();		
			int DajNajlepsiuAkciu();
            int VyberNahodnuAkciu();
			float DajNajvyssieQ();
			void UpdateQ(int akcia, float cieloveQ, float gamma, int mod);
			string toString();
            string toCSV() const;
            
		
	};

    
private:
    std::map<Stav*, QL::QStav*, CompareStav>* stavy = new map <Stav*, QL::QStav*, CompareStav>();
};

