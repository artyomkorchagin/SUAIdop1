#include <iostream>
#include "windows.h"
#include <random>
#include <ctime>
#include <list>

using namespace std;

mt19937 engine;

struct Character {              // максимальные значения у персонажей
    int damagePerSec = 50;      // урон в секунду
    int splashDamage = 3;       // сколько противников может задевать за удар
    int maxHP = 1000;           // максимальное количество здоровья
    int stunDuration = 15;      // время на которое выводит из строя противника
    int stunCooldown = 50;      // время перезарядки стана (минимум 3.0 сек (вроде))
    bool stunned = false;       // выведен ли из строя сейчас персонаж
    bool dead = false;
    int lastTimeAttacked = 10;
    int lastTimeStunned = 0;
    int stunnedFor = 0;
};

// будем считать, что сначала атакует игрок, а затем противник.

struct FightingTeam {
    Character strongest;
    Character medium;
    Character weakest;
};

struct winConditions {
    int Time = 0;
    int HP = 0;
    int playerID = -1;
};

Character RandomCharacterGen() {
    Character a;

    a.damagePerSec = (engine() % 50 + engine() % 90);
    a.splashDamage = engine() % 3 + 1;
    a.maxHP = 50+(engine() % 10000 + engine() % 100) / 10;
    a.stunDuration = (engine() % 150 + 100) / 10;
    a.stunCooldown = (engine() % 200 + engine() % 10) / 10 + 29;
    a.lastTimeStunned = a.stunCooldown;
    a.stunned = false;
    return a;
}

Character* TeamCreation(int NumberOfCharacters) {

    Character *team = new Character[NumberOfCharacters];
    for (int i = 0; i < NumberOfCharacters; i++) {
        team[i] = RandomCharacterGen();
    }

    return team;

}

void TeamShowcase(Character*team, int s) {
    system("cls");
    for(int i = 0; i < s; i++) {
        cout << "#" << i+1 << endl;
        cout << "Damage : " << (double)team[i].damagePerSec /10.0 << endl;
        cout << "Splash Damage : " << team[i].splashDamage << endl;
        cout << "HP : " << (double)team[i].maxHP/10.0 << endl;
        cout << "Stun Duration : " << (double)team[i].stunDuration /10.0 << endl;
        cout << "Stun Cooldown : " << (double)team[i].stunCooldown /10.0<< endl << endl;
    }
    cout << "Available characters\n";
    system("pause");
    system("cls");

}

Character* EnemyChoice(Character* team, int &s) {
    int EnemyCharacter = engine() % s;
    cout << "Enemies choice is : #" << EnemyCharacter+1 << endl;
    cout << "Damage : " << (double)team[EnemyCharacter].damagePerSec/10.0 << endl;
    cout << "Splash Damage : " << team[EnemyCharacter].splashDamage << endl;
    cout << "HP : " << (double)team[EnemyCharacter].maxHP/10.0 << endl;
    cout << "Stun Duration : " << (double)team[EnemyCharacter].stunDuration/10.0 << endl;
    cout << "Stun Cooldown : " << (double)team[EnemyCharacter].stunCooldown/10.0 << endl << endl;
    team[s] = team[EnemyCharacter];                                  //перевел персонажа противника в конец
    for (int i = EnemyCharacter;i < s; i++)                          //сдвинул элементы на прошлое место противника
        team[i] = team[i+1];
    s--;
    system("pause");
    return team;
}

void Stun(Character &a){
    if (a.stunned) a.stunned = false;
    else a.stunned = true;
}

void Attack(Character &a, Character b) {
    a.maxHP -= b.damagePerSec;
}

winConditions* victories1v1(Character* team, int s) {
    int SortType;
    winConditions* arrayOfWins = new winConditions[s];
    cout << "Sort by Time(1) or HP left(2)?";
    cin >> SortType;
    for(int i = 0; i < s; i++) {
        Character You = team[i], Enemy = team[s];
        bool win = false;
        int BattleDuration = 0;
        while (!win) {

            if (!You.stunned && (You.stunCooldown - You.lastTimeStunned <= 0)) {
                Stun(Enemy);
                You.lastTimeStunned = 0;
            }

            if (Enemy.stunned && You.stunDuration == You.lastTimeStunned)
                Stun(Enemy);

            if (!Enemy.stunned && (Enemy.stunCooldown - Enemy.lastTimeStunned <= 0)) {
                Stun(You);
                Enemy.lastTimeStunned = 0;
            }

            if (You.stunned && Enemy.stunDuration == Enemy.lastTimeStunned)
                Stun(You);

            if(!You.stunned && (You.lastTimeAttacked >= 10)) {
                Attack(Enemy, You);
                You.lastTimeAttacked = 0;
            }

            if(!Enemy.stunned && (Enemy.lastTimeAttacked >= 10)) {
                Attack(You, Enemy);
                Enemy.lastTimeAttacked = 0;
            }

            if (You.maxHP <= 0)
                break;

            if (Enemy.maxHP <= 0) {
                win = true;
                break;
            }

            You.lastTimeAttacked += 1;   You.lastTimeStunned += 1;
            Enemy.lastTimeAttacked += 1; Enemy.lastTimeStunned += 1;
            BattleDuration += 1;
        }
        if(win) {
            arrayOfWins[i].HP = You.maxHP;
            arrayOfWins[i].Time = BattleDuration;
            arrayOfWins[i].playerID = i;
        }
    }
    winConditions temp;

    for (int i = 0; i < s - 1; i++) {
        for (int j = 0; j < s - i - 1; j++) {
            if(SortType == 2) {          // сортировка по оставшемувся здоровью
                if (arrayOfWins[j].HP < arrayOfWins[j+1].HP) {
                    temp = arrayOfWins[j];
                    arrayOfWins[j] = arrayOfWins[j+1];
                    arrayOfWins[j+1] = temp;
                }
            }
            if(SortType == 1) {          // сортировка по длительности битвы
                if (arrayOfWins[j].Time < arrayOfWins[j+1].Time) {
                    temp = arrayOfWins[j];
                    arrayOfWins[j] = arrayOfWins[j+1];
                    arrayOfWins[j+1] = temp;
                }
            }
        }
    }
    return arrayOfWins;
}

void ChckCondition(FightingTeam &y, FightingTeam &e) {
    if (y.strongest.maxHP <= 0) y.strongest.dead = true;
    if (y.medium.maxHP <= 0) y.medium.dead = true;
    if (y.weakest.maxHP <= 0) y.weakest.dead = true;
    if (e.strongest.maxHP <= 0) e.strongest.dead = true;
    if (e.medium.maxHP <= 0) e.medium.dead = true;
    if (e.weakest.maxHP <= 0) e.weakest.dead = true;
}

FightingTeam SortTeam(FightingTeam t) {
    Character temp;
    if (t.weakest.damagePerSec > t.medium.damagePerSec) {
        if(t.medium.damagePerSec > t.strongest.damagePerSec) {
            temp = t.weakest;
            t.weakest = t.strongest;
            t.strongest = temp;
        }
        else {
            if (t.weakest.damagePerSec > t.strongest.damagePerSec) {
                temp = t.weakest;
                t.weakest = t.medium;
                t.medium = t.strongest;
                t.strongest = temp;
            }
            else {
                temp = t.weakest;
                t.weakest = t.medium;
                t.medium = temp;
            }
        }
    }
    if(t.medium.damagePerSec > t.strongest.damagePerSec) {
        if(t.strongest.damagePerSec > t.weakest.damagePerSec) {
            temp = t.medium;
            t.medium = t.strongest;
            t.strongest = temp;
        }
        else {
            if (t.weakest.damagePerSec > t.medium.damagePerSec) {
                temp = t.weakest;
                t.weakest = t.strongest;
                t.strongest = temp;
            } else {
                temp = t.strongest;
                t.strongest = t.medium;
                t.medium = t.weakest;
                t.weakest = temp;
            }
        }
    }
    return t;
}

void victories3v3(Character* team, int s) {
    FightingTeam Enemy, You;
    for(int i = 0; i < s-2; i++) {
        for(int j = i+1; j < s-1; j++) {
            for (int k = j + 1; k < s; k++) {
                Enemy.strongest = team[s];
                Enemy.medium = team[s+1];
                Enemy.weakest = team[s+2];
                Enemy = SortTeam(Enemy);
                You.strongest = team[i];
                You.medium = team[j];
                You.weakest = team[k];
                You = SortTeam(You);
                bool win = false;
                int BattleDuration = 0;
                while (!win) {

                    if (!You.strongest.dead) {

                        if (You.strongest.stunned && You.strongest.stunnedFor <= 0)
                            Stun(You.strongest);

                        if (!You.strongest.stunned) {

                            if (You.strongest.stunCooldown - You.strongest.lastTimeStunned <= 0) {
                                if(!Enemy.strongest.stunned && Enemy.strongest.maxHP > 0)   {Stun(Enemy.strongest); Enemy.strongest.stunnedFor = You.strongest.stunDuration; You.strongest.lastTimeStunned = 0; continue;}
                                if(!Enemy.medium.stunned && Enemy.medium.maxHP > 0)         {Stun(Enemy.medium); Enemy.medium.stunnedFor = You.strongest.stunDuration; You.strongest.lastTimeStunned = 0; continue;}
                                if(!Enemy.weakest.stunned && Enemy.weakest.maxHP > 0)       {Stun(Enemy.weakest); Enemy.weakest.stunnedFor = You.strongest.stunDuration; You.strongest.lastTimeStunned = 0; continue;}
                            }
                            if((You.strongest.lastTimeAttacked >= 10)) {
                                if (!Enemy.strongest.dead)                                  {Attack(Enemy.strongest, You.strongest); You.strongest.lastTimeAttacked = 0; continue;}
                                if (!Enemy.medium.dead)                                     {Attack(Enemy.medium, You.strongest); You.strongest.lastTimeAttacked = 0; continue;}
                                if (!Enemy.weakest.dead)                                    {Attack(Enemy.weakest, You.strongest); You.strongest.lastTimeAttacked = 0; continue;}
                            }
                        }
                    }

                    ChckCondition(You, Enemy);

                    if (!Enemy.strongest.dead) {

                        if (Enemy.strongest.stunned && Enemy.strongest.stunnedFor <= 0)
                            Stun(Enemy.strongest);

                        if (!Enemy.strongest.stunned) {

                            if (Enemy.strongest.stunCooldown - Enemy.strongest.lastTimeStunned <= 0) {
                                if(!You.strongest.stunned && You.strongest.maxHP > 0)       {Stun(You.strongest); You.strongest.stunnedFor = Enemy.strongest.stunDuration; Enemy.strongest.lastTimeStunned = 0; continue;}
                                if(!You.medium.stunned && You.medium.maxHP > 0)             {Stun(You.medium); You.medium.stunnedFor = Enemy.strongest.stunDuration; Enemy.strongest.lastTimeStunned = 0; continue;}
                                if(!You.weakest.stunned && You.weakest.maxHP > 0)           {Stun(You.weakest); You.weakest.stunnedFor = Enemy.strongest.stunDuration; Enemy.strongest.lastTimeStunned = 0; continue;}
                            }

                            if((Enemy.strongest.lastTimeAttacked >= 10)) {
                                if (!You.strongest.dead)                                    {Attack(You.strongest, Enemy.strongest); Enemy.strongest.lastTimeAttacked = 0; continue;}
                                if (!You.medium.dead)                                       {Attack(You.medium, Enemy.strongest); Enemy.strongest.lastTimeAttacked = 0; continue;}
                                if (!You.weakest.dead)                                      {Attack(You.weakest, Enemy.strongest); Enemy.strongest.lastTimeAttacked = 0; continue;}
                            }
                        }
                    }

                    ChckCondition(You, Enemy);

                    if (!You.medium.dead) {

                        if (You.medium.stunned && You.medium.stunnedFor <= 0)
                            Stun(You.medium);

                        if (!You.medium.stunned) {

                            if (You.medium.stunCooldown - You.medium.lastTimeStunned <= 0) {
                                if(!Enemy.strongest.stunned && Enemy.strongest.maxHP > 0)   {Stun(Enemy.strongest); Enemy.strongest.stunnedFor = You.medium.stunDuration; You.medium.lastTimeStunned = 0; continue;}
                                if(!Enemy.medium.stunned && Enemy.medium.maxHP > 0)         {Stun(Enemy.medium); Enemy.medium.stunnedFor = You.medium.stunDuration; You.medium.lastTimeStunned = 0; continue;}
                                if(!Enemy.weakest.stunned && Enemy.weakest.maxHP > 0)       {Stun(Enemy.weakest); Enemy.weakest.stunnedFor = You.medium.stunDuration; You.medium.lastTimeStunned = 0; continue;}
                            }

                            if((You.medium.lastTimeAttacked >= 10)) {
                                if (!Enemy.strongest.dead)                                  {Attack(Enemy.strongest, You.medium); You.medium.lastTimeAttacked = 0; continue;}
                                if (!Enemy.medium.dead)                                     {Attack(Enemy.medium, You.medium); You.medium.lastTimeAttacked = 0; continue;}
                                if (!Enemy.weakest.dead)                                    {Attack(Enemy.weakest, You.medium); You.medium.lastTimeAttacked = 0; continue;}
                            }
                        }

                    }

                    ChckCondition(You, Enemy);

                    if (!Enemy.medium.dead) {

                        if (Enemy.medium.stunned && Enemy.medium.stunnedFor <= 0)
                            Stun(Enemy.medium);

                        if(!Enemy.medium.stunned) {

                            if (Enemy.medium.stunCooldown - Enemy.medium.lastTimeStunned <= 0) {
                                if(!You.strongest.stunned && You.strongest.maxHP > 0)       {Stun(You.strongest); You.strongest.stunnedFor = Enemy.medium.stunDuration; Enemy.medium.lastTimeStunned = 0; continue;}
                                if(!You.medium.stunned && You.medium.maxHP > 0)             {Stun(You.medium); You.medium.stunnedFor = Enemy.medium.stunDuration; Enemy.medium.lastTimeStunned = 0; continue;}
                                if(!You.weakest.stunned && You.weakest.maxHP > 0)           {Stun(You.weakest); You.weakest.stunnedFor = Enemy.medium.stunDuration; Enemy.medium.lastTimeStunned = 0; continue;}
                            }

                            if((Enemy.medium.lastTimeAttacked >= 10)) {
                                if (!You.strongest.dead)                                    {Attack(You.strongest, Enemy.medium); Enemy.medium.lastTimeAttacked = 0; continue;}
                                if (!You.medium.dead)                                       {Attack(You.medium, Enemy.medium); Enemy.medium.lastTimeAttacked = 0; continue;}
                                if (!You.weakest.dead)                                      {Attack(You.weakest, Enemy.medium); Enemy.medium.lastTimeAttacked = 0; continue;}
                            }
                        }
                    }

                    ChckCondition(You, Enemy);

                    if (!You.weakest.dead) {

                        if (You.weakest.stunned && You.weakest.stunnedFor <= 0)
                            Stun(You.weakest);

                        if (!You.weakest.stunned) {

                            if (You.weakest.stunCooldown - You.weakest.lastTimeStunned <= 0) {
                                if(!Enemy.strongest.stunned && Enemy.strongest.maxHP > 0)   {Stun(Enemy.strongest); Enemy.strongest.stunnedFor = You.weakest.stunDuration; You.weakest.lastTimeStunned = 0; continue;}
                                if(!Enemy.medium.stunned && Enemy.medium.maxHP > 0)         {Stun(Enemy.medium); Enemy.medium.stunnedFor = You.weakest.stunDuration; You.weakest.lastTimeStunned = 0; continue;}
                                if(!Enemy.weakest.stunned && Enemy.weakest.maxHP > 0)       {Stun(Enemy.weakest); Enemy.weakest.stunnedFor = You.weakest.stunDuration; You.weakest.lastTimeStunned = 0; continue;}
                            }

                            if((You.weakest.lastTimeAttacked >= 10)) {
                                if (!Enemy.strongest.dead)                                  {Attack(Enemy.strongest, You.weakest); You.weakest.lastTimeAttacked = 0; continue;}
                                if (!Enemy.medium.dead)                                     {Attack(Enemy.medium, You.weakest); You.weakest.lastTimeAttacked = 0; continue;}
                                if (!Enemy.weakest.dead)                                    {Attack(Enemy.weakest, You.weakest); You.weakest.lastTimeAttacked = 0; continue;}
                            }
                        }
                    }

                    ChckCondition(You, Enemy);

                    if (!Enemy.weakest.dead) {

                        if (Enemy.weakest.stunned && Enemy.weakest.stunnedFor <= 0)
                            Stun(Enemy.weakest);

                        if(!Enemy.weakest.stunned) {

                            if (Enemy.weakest.stunCooldown - Enemy.weakest.lastTimeStunned <= 0) {
                                if(!You.strongest.stunned && You.strongest.maxHP > 0)       {Stun(You.strongest); You.strongest.stunnedFor = Enemy.weakest.stunDuration; Enemy.weakest.lastTimeStunned = 0; continue;}
                                if(!You.medium.stunned && You.medium.maxHP > 0)             {Stun(You.medium); You.medium.stunnedFor = Enemy.weakest.stunDuration; Enemy.weakest.lastTimeStunned = 0; continue;}
                                if(!You.weakest.stunned && You.weakest.maxHP > 0)           {Stun(You.weakest); You.weakest.stunnedFor = Enemy.weakest.stunDuration; Enemy.weakest.lastTimeStunned = 0; continue;}
                            }

                            if((Enemy.weakest.lastTimeAttacked >= 10)) {
                                if (!You.strongest.dead)                                    {Attack(You.strongest, Enemy.weakest); Enemy.weakest.lastTimeAttacked = 0; continue;}
                                if (!You.medium.dead)                                       {Attack(You.medium, Enemy.weakest); Enemy.weakest.lastTimeAttacked = 0; continue;}
                                if (!You.weakest.dead)                                      {Attack(You.weakest, Enemy.weakest); Enemy.weakest.lastTimeAttacked = 0; continue;}
                            }
                        }
                    }

                    ChckCondition(You, Enemy);

                    if (You.strongest.dead && You.medium.dead && You.weakest.dead)
                        break;


                    if (Enemy.strongest.dead && Enemy.medium.dead && Enemy.weakest.dead) {
                        win = true;
                        break;
                    }

                    You.strongest.stunnedFor -= 1;  You.strongest.lastTimeAttacked += 1;    You.strongest.lastTimeStunned += 1;
                    You.medium.stunnedFor -= 1;     You.medium.lastTimeAttacked += 1;       You.medium.lastTimeStunned += 1;
                    You.weakest.stunnedFor -= 1;    You.weakest.lastTimeAttacked += 1;      You.weakest.lastTimeStunned += 1;

                    Enemy.strongest.stunnedFor -= 1;Enemy.strongest.lastTimeAttacked += 1;  Enemy.strongest.lastTimeStunned += 1;
                    Enemy.medium.stunnedFor -= 1;   Enemy.medium.lastTimeAttacked += 1;     Enemy.medium.lastTimeStunned += 1;
                    Enemy.weakest.stunnedFor -= 1;  Enemy.weakest.lastTimeAttacked += 1;    Enemy.weakest.lastTimeStunned += 1;
                    BattleDuration += 1;
                }

                if (win) {
                    cout << "Team members : " << i+1 << " " << j+1 << " " << k+1 << endl;
                    cout <<(double)BattleDuration /10.0 << endl << endl;
                }
            }
        }
    }
}

void WinnersShowcase(winConditions* win, int s) {
    system("cls");
    for(int i = 0; i < s; i++) {
        if (win[i].HP == 0)
            continue;
        cout << "Character #" << win[i].playerID+1 << "\nHP : " << (double)win[i].HP / 10.0 << "\nBattle Duration : " << (double)win[i].Time / 10.0 << endl << endl;
    }
}

int main()
{
    int v;
    cout << "Welcome to \"The Game\"\n";
    cout << "1v1 (1) or 3v3 (2)? ";
    cin >> v;
    engine.seed(time(nullptr));
    int NumberOfCharacters = 10 + engine() % 10;
    Character *FullTeam = TeamCreation(NumberOfCharacters+1);
    TeamShowcase(FullTeam, NumberOfCharacters);
    if (v == 1) {
        FullTeam = EnemyChoice(FullTeam, NumberOfCharacters);
        TeamShowcase(FullTeam, NumberOfCharacters);
        winConditions *Winners = victories1v1(FullTeam, NumberOfCharacters);
        WinnersShowcase(Winners, NumberOfCharacters);
        delete [] FullTeam;
        delete [] Winners;
    }
    if (v == 2) {
        FullTeam = EnemyChoice(FullTeam, NumberOfCharacters);
        FullTeam = EnemyChoice(FullTeam, NumberOfCharacters);
        FullTeam = EnemyChoice(FullTeam, NumberOfCharacters);
        TeamShowcase(FullTeam, NumberOfCharacters);
        victories3v3(FullTeam, NumberOfCharacters);
        delete [] FullTeam;

    }

    return 0;
}
