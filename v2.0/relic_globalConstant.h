#ifndef relic_global_constant
#define relic_global_constant
    
#include<stdio.h>
// #include<stdlib.h>
#include<string.h>
#include<assert.h>

//enum / code 
typedef enum PIECE{
    PIECE_HEAD,
    PIECE_HANDS,
    PIECE_BODY,
    PIECE_FEET,
    PIECE_PLANAR,
    PIECE_ROPE
} PIECE;
typedef enum STAT{
    STAT_HP = 0,
    STAT_ATK = 1,
    STAT_DEF = 2,
    STAT_HPP = 3,
    STAT_ATKP = 4,
    STAT_DEFP = 5,
    STAT_SPD = 6,
    STAT_CR = 7,
    STAT_CD = 8,
    STAT_EHR = 9,
    STAT_ERS = 10,
    STAT_BE = 11,
    STAT_DMG = -1,
    STAT_HEAL = -2,
    STAT_ER = -3
} STAT;
char *STATname[]={
    "HP"
    ,"ATK"
    ,"DEF"
    ,"HP%"
    ,"ATK%"
    ,"DEF%"
    ,"SPD"
    ,"CR"
    ,"CD"
    ,"EHR"
    ,"ERS"
    ,"BE"
};


// global variable
struct COST{
    double create;
    double upgrade[5];
    double accept;
} cost;
struct CHANCE{
    double startFour;//chance for starting with 4 substat

    double mainstat;//chance for the mainstat to be correct
    int substatWeightTotal;
    int substatWeight[12];//each susbtat weight, 0 if mainstat
    
    int substatGoodCount;
    STAT substatGood[12];//array containing all good substat
    int substatBadCount;
    STAT substatBad[12];//array containing all bad substat
} chance;
int threshold;


//change setting here
void initGlobalVariable(PIECE piece, STAT mainstat, STAT *substat, int substat_len, int minimumSubstat){
    threshold = minimumSubstat;
    //cost
        //initial
        cost.create = 100*195;// 1 remain is 195 exp
        cost.upgrade[0] = 2400;//from lvl 0 to 3 need # exp
        cost.upgrade[1] = 5100;//from lvl 3 to 6need # exp
        cost.upgrade[2] = 10000;
        cost.upgrade[3] = 19500;
        cost.upgrade[4] = 39000;
        //refund 
        cost.create -= 1500;
        cost.accept = 1500;//all sum of refund
        for(int i = 0; i<5; i++){
            cost.upgrade[i] /= 5;//refunded 80%
            cost.accept += cost.upgrade[i]*4;
        } 
    //

    //chance.startFour
    chance.startFour = .175f;

    //chance.mainstat
    switch (piece){
        case PIECE_HEAD:
            chance.mainstat = 1;
        break;
        case PIECE_HANDS:
            chance.mainstat = 1;
        break;
        case PIECE_BODY:
            switch (mainstat){
                case STAT_HPP:chance.mainstat = 0.20f;break;
                case STAT_ATKP:chance.mainstat = 0.20f;break;
                case STAT_DEFP:chance.mainstat = 0.20f;break;
                case STAT_CD:chance.mainstat = 0.10f;break;
                case STAT_CR:chance.mainstat = 0.10f;break;
                case STAT_EHR:chance.mainstat = 0.10f;break;
                case STAT_HEAL:chance.mainstat = 0.10f;break;
                default :chance.mainstat = 0;break;
            }
        break;
        case PIECE_FEET:
            switch (mainstat){
                case STAT_HPP:chance.mainstat = 0.28f;break;
                case STAT_ATKP:chance.mainstat = 0.30f;break;
                case STAT_DEFP:chance.mainstat = 0.30f;break;
                case STAT_SPD:chance.mainstat = 0.12f;break;
                default :chance.mainstat = 0;break;
            }
        break;
        case PIECE_PLANAR:
            switch (mainstat){
                case STAT_HPP:chance.mainstat = 0.12f;break;
                case STAT_ATKP:chance.mainstat = 0.13f;break;
                case STAT_DEFP:chance.mainstat = 0.12f;break;
                case STAT_DMG:chance.mainstat = 0.09f;break;
                default :chance.mainstat = 0;break;
            }
        break;
        case PIECE_ROPE:
            switch (mainstat){
                case STAT_HPP:chance.mainstat = 0.26f;break;
                case STAT_ATKP:chance.mainstat = 0.27f;break;
                case STAT_DEFP:chance.mainstat = 0.24f;break;
                case STAT_BE:chance.mainstat = 0.16f;break;
                case STAT_ER:chance.mainstat = 0.5f;break;
                default :chance.mainstat = 0;break;
            }
        break;
        default: printf("invalid piece"); break;
    }
    
    //chance.substatWeight and chance.substatweightTotal 
    const int substatWeight[] = {10,10,10,10,10,10,4,6,6,8,8,8};
    chance.substatWeightTotal = 0;
    for(int i = 0; i<sizeof(substatWeight)/sizeof(int); i++){
        chance.substatWeight[i] = 
        substatWeight[i]*(mainstat != i);
        chance.substatWeightTotal += chance.substatWeight[i];
    }

    //chance.substatGood and chance.substatBad, with their count
    //prep: foreach chance.substatGood entry, its index be treated as STAT and value is (goodsub?),
    int *substat_is_Good = chance.substatGood;//borrowing 
    memset(substat_is_Good,0,sizeof(chance.substatGood));
    for(int i = 0 ; i<substat_len; i++){
        assert(substat[i]>=0);
        substat_is_Good[substat[i]] = 1;
    }
    
    chance.substatGoodCount = 0;
    chance.substatBadCount = 0;
    for(int i = 0; i < 12; i++){//for every substat
        if(i != mainstat){
            if(!substat_is_Good[i])chance.substatBad[chance.substatBadCount++] = i;    
            else chance.substatGood[chance.substatGoodCount++] = i;
        } else continue;
    }
    
    
    
    
}


// // initialize constant variable
// STAT substat[] = {STAT_CD,STAT_ATKP};
// iniGlobalVariable(PIECE_BODY,STAT_CR,substat,sizeof(substat)/sizeof(int)); 
// struct COST cost2;
// struct CHANCE chance2;
// memcpy(&cost2,&cost,sizeof(struct COST));
// memcpy(&chance2,&chance,sizeof(struct CHANCE));
#endif
