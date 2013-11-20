#ifndef _GAMERULES_H_
#define _GAMERULES_H_
#include <string.h>
bool Battle(Unit &A, Unit &B); //Build a battle animation and calculate outcome
//Function calculates battle, saves the outcome to the progressive chapter file then adds animation state
bool BattleEx(Unit &A, Unit &B, std::string script); //Build a battle by a script
//Uses script logic to determine battle outcome and set animation information
void VisualBattle(Unit &A, Unit &B, std::string script); //Build the battle graphic
//Builds a battle animation script using the unit information
int GetAtkSpeed(Unit &A, Unit &B); //Attack Speed = Spd + Con - Weapon Weight  (Attack Speed is never greater than Speed)
//Double Attacks: If Attack Speed differs by four or more, then the fast unit double attacks. 
int GetDamage(Unit &A, Tile &T, int misc); //Pow + Effective Weapon Might - Target Effective Defence + Bonuses 
int GetHitRate(Unit &A, Unit &B); //Weapon Hit Rate + 2*Skl + 0.5*Luc - Target Avoid + Bonuses
//(Bonuses: +5 if S in weapon, 0-25 from HIT Support, +30, +15, +0, -15, or -30 from the Weapon Triangle, and 1-9 from your Tactician Rank for units of the tactician's element)
//Avoid = 2*Attack Speed + Luc + Bonuses
//(Bonuses: 0-25 from EVA Support, 0-40 from Terrain, +10 if dance Set's Litany is used, and 1-9 from your Tactician Rank for units of the tactician's element) 
int GetWeaponMight(WeaponEntry &A, Unit &T); //(Might + (2, 1, 0, -1, or -2 from Weapon Triangle))*2 if weapon is effective against Target 
//(Bonuses: 0-5 from POW Support, and +10 if dance Fillia's Might is used) 
int GetDefence(Unit &A, int misc); // Def or Res + Bonuses
//(Bonuses: 0-5 from DEF Support, 0-2 from Terrain, and +10 if dance Nini's Grace is used) 
int GetCritialRate(Unit &A, Unit &B); //Weapon Critical +0.5*Skl - Target Crit Evade + Bonuses 
//(Bonuses: +5 if S in weapon, +15 if Berserker or Swordmaster, 0-25 from CRT Support, and +10 if dance Thor's Ire is used). 
//Critical Evade = Luc + Bonuses
//(Bonuses: 0-25 from CEV Support, and 1-9 from Tactician Rank (regardless of element))
//Criticals deal 3x damage (after defence has been factored in, so characters who did 0 before will still deal 0).
//You must hit in order to crit.
//(Magic Swords can not critical when casting a spell). 
int GetStaffAcc(Unit &A, Unit &B); //30 + Pow*5 + Skl - Enemy Staff Evade + Bonuses 
//(Bonuses: +5 if S in Staves, and 0-25 from POW Support)
//Staff Evade = Res*5 + Distance From Target*2 + Bonuses
//(Bonuses: 0-25 from Support, and +50 if dance Nini's Grace is used)
bool TryPath(int *moves, int length, Map *map);

#endif