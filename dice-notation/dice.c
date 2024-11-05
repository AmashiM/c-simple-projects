
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdint.h"
#include "math.h"
#include "string.h"
#include "limits.h"
#include <sys/time.h>

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

#ifndef uint16_t
typedef unsigned short uint16_t;
#endif

/**
 * Description
 * function used to roll dice
 * 
 * - [solved] added appropriate unsigned numbers
 * - [solved] accurately tracked keep for multiple numbers
 */
long dice_roll(uint16_t amount, uint16_t sides, uint8_t keep_highest, uint8_t keep_lowest){
    long total = 0;
    uint8_t keep = 0;

    if(keep_highest > 0 || keep_lowest > 0){
        keep = 1;
    }

    uint16_t* values = calloc(sizeof(uint16_t), amount);
    if(values == NULL){
        printf("failed to allocate space for values\n");
        return -1;
    }

    for(int i = 0; i < amount; i++){
        values[i] = (int)((rand() % sides) + 1);
    }

    for(uint16_t i = 0; i < amount; i++){
        for(uint16_t j = i+1; j < amount; j++){
            if(values[i] > values[j]){
                uint16_t temp = values[i];
                values[i] = values[j];
                values[j] = temp;
            }
        }
    }

    if(keep == 0){
        for(int i = 0; i < amount; i++){
            total = total + values[i];
        }
    } else {
        uint16_t* high = NULL;
        uint16_t* low = NULL;
        if(keep_highest > 0){
            high = (uint16_t*)calloc(sizeof(uint16_t), keep_highest);
            if(high == NULL){
                printf("failed to allocate space for high values\n");
                return -1;
            }
        }
        if(keep_lowest > 0){
            low = (uint16_t*)calloc(sizeof(uint16_t), keep_lowest);
            if(low == NULL){
                printf("failed to allocate space for low values\n");
                return -1;
            }
            for(uint8_t i = 0; i < keep_lowest; i++){
                low[i] = (uint16_t)INT_MAX;
            }
        }

        if(high != NULL){
            uint16_t offset = amount - keep_highest;
            for(uint8_t i = 0; i < keep_highest; i++){
                high[i] = values[offset + i];
            }

            for(uint8_t i = 0; i < keep_highest; i++){
                total = total + high[i];
            }

            free(high);
            high = NULL;
        }

        if(low != NULL){
            for(uint8_t i = 0; i < keep_lowest; i++){
                low[i] = values[i];
            }

            for(uint8_t i = 0; i < keep_lowest; i++){
                total = total + low[i];
            }

            free(low);
            low = NULL;
        }
    }

    free(values);
    values = NULL;

    return total;
}


#define TYPE_SPACE 0
#define TYPE_DICE 1
#define TYPE_NUM 2
#define TYPE_KEEP 3
#define TYPE_KEEP_HIGH 4
#define TYPE_KEEP_LOW 5
#define TYPE_GROUP_START 6
#define TYPE_GROUP_END 7
#define TYPE_ADD 8
#define TYPE_SUB 9
#define TYPE_MULT 10
#define TYPE_DIV 11
#define TYPE_EMPTY 12

const char* token_type_as_str(int type){
    switch(type){
        case TYPE_SPACE: return "SPACE";
        case TYPE_DICE: return "DICE";
        case TYPE_NUM: return "NUM";
        case TYPE_KEEP: return "KEEP";
        case TYPE_KEEP_HIGH: return "KEEP HIGH";
        case TYPE_KEEP_LOW: return "KEEP LOW";
        case TYPE_GROUP_START: return "GROUP START";
        case TYPE_GROUP_END: return "GROUP END";
        case TYPE_ADD: return "ADD";
        case TYPE_SUB: return "SUB";
        case TYPE_DIV: return "DIV";
        case TYPE_MULT: return "MULT";
        case TYPE_EMPTY: return "EMPTY";
        default: printf("got something strange when getting a token type as a string\n"); break;
    }
    return NULL;
}

void dice_notation_print_token(int type, int value){
    switch(type){
        case TYPE_SPACE: printf("SPACE\n"); break;
        case TYPE_DICE: printf("DICE\n"); break;
        case TYPE_NUM: printf("NUM: %d\n", value); break;
        case TYPE_KEEP: printf("KEEP\n"); break;
        case TYPE_KEEP_HIGH: printf("KEEP HIGH\n"); break;
        case TYPE_KEEP_LOW: printf("KEEP LOW\n"); break;
        case TYPE_GROUP_START: printf("GROUP START\n"); break;
        case TYPE_GROUP_END: printf("GROUP END\n"); break;
        case TYPE_ADD: printf("ADD\n"); break;
        case TYPE_SUB: printf("SUB\n"); break;
        case TYPE_DIV: printf("DIV\n"); break;
        case TYPE_MULT: printf("MULT\n"); break;
        case TYPE_EMPTY: printf("EMPTY\n"); break;
        default: printf("got something strange\n"); break;
    }
}

void dice_notation_print_tokens(int tokens[][2], const int length){
    for(int i = 0; i < length; i++){
        dice_notation_print_token(tokens[i][0], tokens[i][1]);
    }
}


typedef struct tagPairToken {
    int type;
    int value;
} PairToken; // it's called a pair since there's 2 values

typedef struct tagRealToken {
    int type; // 0
    int value; // 1
    uint8_t used; // 2
    uint8_t ref; // 3
} RealToken;

typedef struct tagMathToken {
    int pos; // 0
    int type; // 1
} MathToken;

typedef struct tagNumberToken {
    int pos; // 0
    int value; // 1
} NumberToken;

typedef struct tagKeepToken {
    int pos; // 0
    int type; // 1
    int value; // 2
} KeepToken;

typedef struct tagDiceToken {
    int pos;
    int amount;
    int sides;
    int keep_high;
    int keep_low;
} DiceToken;

typedef struct tagGroupPlacementToken {
    uint8_t priority;
    uint8_t pos;
    uint8_t used;
} GroupPlacementToken;

typedef struct tagGroupToken {
    uint8_t priority;
    uint8_t start_pos;
    uint8_t end_pos;
    long value;
    uint8_t used;
} GroupToken;

void dice_notation_print_groupplacementtokens(GroupPlacementToken* tokens, int length){
    for(unsigned int i = 0; i < length; i++){
        GroupPlacementToken* token = &tokens[i];
        printf("GroupPlacementToken(pos: %d, priority: %d)\n", token->pos, token->priority);
    }
}

void dice_notation_print_grouptokens(GroupToken* tokens, uint8_t length){
    for(uint8_t i = 0; i < length; i++){
        GroupToken* token = &tokens[i];
        printf("GroupToken(priority: %d, start: %d, end: %d)\n", token->priority, token->start_pos, token->end_pos);
    }
}

void dice_notation_print_dicetokens(DiceToken tokens[], int length){
    for(int i = 0; i < length; i++){
        DiceToken* token = &tokens[i];
        printf("Dice(pos: %d, amount: %d, sides: %d, keep_high: %d, keep_low: %d)\n", token->pos, token->amount, token->sides, token->keep_high, token->keep_low);
    }
}

char math_token_type_as_c(int type){
    switch(type){
        case TYPE_ADD: return '+';
        case TYPE_SUB: return '-';
        case TYPE_MULT: return '*';
        case TYPE_DIV: return '/';
        default: {
            printf("unknown math operation type");
            return 'm';
        }; break;
    }
}

void dice_notation_print_pairtokens(PairToken tokens[], const int length){
    for(int i = 0; i < length; i++){
        dice_notation_print_token(tokens[i].type, tokens[i].value);
    }
}

void dice_notation_print_realtokens(RealToken tokens[], const int length){
    for(int i = 0; i < length; i++){
        printf("(type: %s, value: %d, used: %s)\n", token_type_as_str(tokens[i].type), tokens[i].value, (tokens[i].used == 1) ? "true" : "false");
    }
}

#define TOKEN_TYPE uint8_t


#define DICE_COUNT_TYPE uint8_t
#define KEEP_COUNT_TYPE uint16_t // this should be higher than DICE_COUNT_TYPE since logically we want to allow for 2 per dice at the minimum.

typedef struct tagDiceNotationState {
    size_t length;
    const char* text;

    PairToken* tokens;
} DiceNotationState;

typedef struct tagDiceNotationCounters {
    DICE_COUNT_TYPE dice_count;
    KEEP_COUNT_TYPE keep_count;

    uint16_t math_count;
    uint16_t number_count;
    uint16_t real_token_count;
    uint8_t group_start_count;
    uint8_t group_end_count;
    uint8_t group_count;

    uint8_t dice_cache_pos;
    uint8_t math_cache_pos;
    uint8_t number_cache_pos;
    uint8_t keep_cache_pos;
    unsigned int real_token_pos;

    uint8_t group_priority;
} DiceNotationCounters;

typedef struct tagDiceNotationCache {
    DiceToken* dice;
    MathToken* math;
    NumberToken* number;
    KeepToken* keep;
    RealToken* real_tokens;
    GroupPlacementToken* group_start_cache;
    GroupPlacementToken* group_end_cache;
    GroupToken* group_cache;
} DiceNotationCache;

int dice_notation_parse_text(DiceNotationState* state, DiceNotationCounters* counters) {
    signed int last_number_index = -1;
    uint8_t processing_number = 0;

    // printf("internal - parsing text\n");

    counters->real_token_count = state->length;
    for(uint8_t i = 0; i < (uint8_t)state->length; i++){
        char c = state->text[i];
        uint8_t handling_number = 0;
        uint8_t value = 0;

        PairToken* token = &state->tokens[i];
        if(token == NULL){
            printf("accidentally got null token\n");
            continue;
        }

        // printf("got token: %c\n", c);
        switch(c){
            case 'd':
                token->type = TYPE_DICE;
                token->value = i; //define where it's located
                counters->dice_count++;
                break;
            case 'k':
                // tokens[i][0] = TYPE_EMPTY; // keep the TYPE_KEEP, but we'll be placing an empty here for now.
                counters->keep_count++;
                state->tokens[i+1].type = TYPE_EMPTY; // just a reminder but if we're setting a type here and we don't skip incriment the I then this will automatically be overwritten in the next iteration.
                counters->real_token_count--; // since we're assigning empty for the initial token
                token->value = 0;
                switch(state->text[i+1]) {
                    case 'h':
                        token->type = TYPE_KEEP_HIGH;
                        break;
                    case 'l':
                        token->type = TYPE_KEEP_LOW;
                        break;
                    default:
                        printf("failed to parse keep statement\n");
                        break;
                }
                break;
            case '(':
                token->type = TYPE_GROUP_START;
                counters->group_start_count++;
                break;
            case ')':
                token->type = TYPE_GROUP_END;
                counters->group_end_count++;
                break;
            case '+':
                token->type = TYPE_ADD;
                token->value = 0;
                counters->math_count++;
                break;
            case '-':
                token->type = TYPE_SUB;
                counters->math_count++;
                break;
            case '*':
                token->type = TYPE_MULT;
                counters->math_count++;
                break;
            case '/':
                token->type = TYPE_DIV;
                counters->math_count++;
                break;
            case ' ':
                token->type = TYPE_EMPTY; // keep these seperate just in case
                counters->real_token_count--;
                break;
            default: {
                if('0' <= c && c <= '9'){
                    value = (c - '0');
                    handling_number = 1;
                } else if(c != 'h' && c != 'l') {
                    printf("unexpected token: %c\n", c);
                }
            }; break;
        }

        if(handling_number){
            if(processing_number == 1) {
                state->tokens[last_number_index].value = (state->tokens[last_number_index].value * 10) + value;
                token->type = TYPE_EMPTY;
                counters->real_token_count--;
            } else {
                last_number_index = i;
                processing_number = 1;
                counters->number_count++;
                token->type = TYPE_NUM;
                token->value = value;
            }
        } else {
            if(processing_number == 1){
                processing_number = 0;
                last_number_index = -1;
            }
        }
    }

    return 0;
}

int dice_notation_organize_into_cache(DiceNotationState* state, DiceNotationCounters* counters, DiceNotationCache* cache){

    if(counters->group_start_count != counters->group_end_count){
        printf("start and end group identifiers are not equal\n");
        return -1;
    }

    counters->group_count = counters->group_start_count;

    cache->group_start_cache = calloc(sizeof(GroupPlacementToken), counters->group_start_count);
    cache->group_end_cache = calloc(sizeof(GroupPlacementToken), counters->group_end_count);

    if(cache->group_start_cache == NULL){
        printf("failed to allocate space for group_start_cache\n");
        return -1;
    }

    if(cache->group_end_cache == NULL){
        printf("failed to allocate space for group_end_cache\n");
        return -1;
    }

    counters->group_priority = 0;

    unsigned int real_token_pos = 0;

    uint8_t group_start_pos = 0;
    uint8_t group_end_pos = 0;

    // organize the tokens to remove dead space and also have it record data on positions of tokens.
    for(size_t i = 0; i < state->length; i++){
        TOKEN_TYPE _type = state->tokens[i].type;
        int _value = state->tokens[i].value;
        if(_type == TYPE_EMPTY){
            continue;
        }

        int out_ref = -1; // -1 could mean no ref, this could also be a 16 bit int

        switch(_type){
            case TYPE_GROUP_START:
                cache->group_start_cache[group_start_pos].pos = real_token_pos;
                counters->group_priority++;
                cache->group_start_cache[group_start_pos].priority = counters->group_priority;
                group_start_pos++;
                break;
            case TYPE_GROUP_END:
                cache->group_end_cache[group_end_pos].pos = real_token_pos;
                cache->group_end_cache[group_end_pos].priority = counters->group_priority;
                counters->group_priority--;
                group_end_pos++;
                break;
            case TYPE_DICE:
                cache->dice[counters->dice_cache_pos].pos = real_token_pos;
                out_ref = counters->dice_cache_pos;
                counters->dice_cache_pos++;
                break;
            case TYPE_KEEP_LOW:
            case TYPE_KEEP_HIGH:
                cache->keep[counters->keep_cache_pos].pos = real_token_pos;
                cache->keep[counters->keep_cache_pos].type = _type;
                out_ref = counters->keep_cache_pos;
                counters->keep_cache_pos++;
                break;
            case TYPE_NUM:
                cache->number[counters->number_cache_pos].pos = real_token_pos;
                cache->number[counters->number_cache_pos].value = _value;
                out_ref = counters->number_cache_pos;
                counters->number_cache_pos++;
                break;
            case TYPE_SUB:
            case TYPE_DIV:
            case TYPE_MULT:
            case TYPE_ADD:
                cache->math[counters->math_cache_pos].pos = real_token_pos;
                cache->math[counters->math_cache_pos].type = _type;
                out_ref = counters->math_cache_pos;
                counters->math_cache_pos++;
                break;
            default:
                // unhandled but that's fine
                break;
        }

        RealToken* real_token = &cache->real_tokens[real_token_pos];
        
        real_token->ref = out_ref;
        real_token->type = _type;
        real_token->value = _value;
        real_token_pos++;
    }

    free(state->tokens);
    state->tokens = NULL;

    counters->real_token_pos = real_token_pos;

    return 0;
}

int dice_notation_define_keep(DiceNotationCounters* counters, DiceNotationCache* cache){
    for(KEEP_COUNT_TYPE i = 0; i < counters->keep_cache_pos; i++){
        KeepToken* keep_token = &cache->keep[i];
        uint16_t pos = keep_token->pos;
        RealToken* next_token = &cache->real_tokens[pos+1];
        TOKEN_TYPE next_type = next_token->type;
        if(next_token->type == TYPE_NUM){
            keep_token->value = next_token->value; // might not be needed
            cache->real_tokens[pos].value = next_token->value;
            next_token->used = 1;
        } else {
            printf("no value found for keep_cache\n");
            keep_token->value = 0; // no value found, we could have this as 0 or 1 depending on how we want to have to debug stuff.
        }
    }

    return 0;
}

int dice_notation_define_dice(DiceNotationCounters* counters, DiceNotationCache* cache){

    for(DICE_COUNT_TYPE i = 0; i < counters->dice_cache_pos; i ++){
        DiceToken* dice_token = &cache->dice[i];
        const int dice_pos = dice_token->pos;
        int before_pos = dice_pos-1;
        int after_pos = dice_pos+1;

        unsigned int keep_high = 0; // should not be negative
        unsigned int keep_low = 0; // should not be negative
        unsigned int amount = 1;
        unsigned int sides = 0;

        uint8_t ok = 1;

        // looking before dice pos
        for(int i = dice_pos - 1; 0 <= i && ok != 0; i--){
            if(cache->real_tokens[i].used == 1){
                continue; // token already in use
            }
            RealToken* real_token = &cache->real_tokens[i];

            switch(real_token->type){
                case TYPE_KEEP_HIGH:
                    keep_high = real_token->value;
                    break;
                case TYPE_KEEP_LOW:
                    keep_low = keep_low + real_token->value;
                    break;
                case TYPE_NUM:
                    amount = real_token->value;
                    break;
                default:
                    ok = 0;
                    continue; // technically this passes over the part where we define if it was used, but imma leave this here.
                    break;
            }
            if(ok){
                real_token->used = 1; // bulk check if it was used or not.
            }
        }

        ok = 1;

        // look after

        for(int i = dice_pos+1; i < counters->real_token_pos && ok != 0; i++){
            if(cache->real_tokens[i].used == 1){
                continue; // token already in use
            }

            RealToken* real_token = &cache->real_tokens[i];

            switch(real_token->type){
                case TYPE_KEEP_HIGH:
                    keep_high = real_token->value;
                    break;
                case TYPE_KEEP_LOW:
                    keep_low = keep_low + real_token->value;
                    break;
                case TYPE_NUM:
                    sides = sides + real_token->value;
                    break;
                default:
                    ok = 0;
                    continue;
                    break;
            }
            if(ok){
                real_token->used = 1; // bulk check if it was used or not.
            }
        }

        // last step
        dice_token->amount = amount;
        dice_token->sides = sides;
        dice_token->keep_high = keep_high;
        dice_token->keep_low = keep_low;
    }

    return 0;
}

int dice_notation_define_groups(DiceNotationCounters* counters, DiceNotationCache* cache){
    cache->group_cache = calloc(sizeof(GroupToken), counters->group_count+1);
    if(cache->group_cache == NULL){
        printf("failed to allocate memory for group cache\n");
        return -1;
    }
    counters->group_count++;


    cache->group_cache[0].priority = 0;
    cache->group_cache[0].start_pos = -1;
    cache->group_cache[0].end_pos = counters->real_token_pos;

    // for(uint8_t i = 1; i < (counters->group_count+1); i++){
    //     group_token = &cache->group_cache[i];
    //     // if(group_token == NULL)
    // }

    for(uint8_t i = 0; i < counters->group_start_count; i++){
        GroupPlacementToken* start_token = &cache->group_start_cache[i];
        GroupToken* group_token = &cache->group_cache[i+1];
        group_token->start_pos = start_token->pos;
        group_token->priority = start_token->priority;
        for(uint8_t j = 0; j < counters->group_end_count; j++){
            GroupPlacementToken* end_token = &cache->group_end_cache[j];
            if(end_token->used == 1){
                continue;
            }
            if(end_token->priority != start_token->priority){
                continue;
            }
            if(end_token->pos < start_token->pos){
                continue;
            }
            // there should be a 3rd check here, but this is enough for now.
            group_token->end_pos = end_token->pos;
            end_token->used = 1;
            break;
        }
        start_token->used = 1;
    }


    return 0;
}

long dice_notation_simple_process(DiceNotationCounters* counters, DiceNotationCache* cache){
    // we could have it record data above, but this allows us to not have to go through any confusing information collection while we're processing data.
    int unused_count = counters->real_token_pos;
    for(unsigned int i = 0; i < counters->real_token_pos; i++){
        if(cache->real_tokens[i].used == 1){
            unused_count--;
        }
    }

    int* unused_cache = calloc(sizeof(int), unused_count);
    if(unused_cache == NULL){
        printf("failed to init unused cache\n");
        return 0;
    }
    int unused_cache_pos = 0;

    // try walking through current tokens
    for(unsigned int i = 0; i < counters->real_token_pos; i++){
        if(cache->real_tokens[i].used != 1){
            unused_cache[unused_cache_pos] = i;
            unused_cache_pos = unused_cache_pos + 1;
        }
    }

    // roll some stuff
    
    long out_value = 0;
    uint8_t current_operation = 0;

    // printf("unusded cache size: %d\n", unused_cache_pos);

    for(uint16_t i = 0; i < unused_cache_pos; i++){
        uint16_t pos = unused_cache[i];
        RealToken* token = &cache->real_tokens[pos];
        if(token == NULL){
            printf("failed to get token\n");
            continue;
        }
        int token_value = token->value;
        uint8_t used = token->used;
        
        // printf("before value: %ld\n", out_value);
        // printf("got type: %s\n", token_type_as_str(token->type));
        switch(token->type){
            case TYPE_DICE: {
                used = 1;
                DiceToken* dice_token = &cache->dice[token->ref];
                long rolled_dice = dice_roll(dice_token->amount, dice_token->sides, dice_token->keep_high, dice_token->keep_low);
                // printf("rolled a %ld\n", rolled_dice);
                out_value = out_value + rolled_dice;
            }; break;
            case TYPE_ADD:
                used = 1;
                current_operation = TYPE_ADD;
                break;
            case TYPE_NUM:
                if(current_operation != 0){
                    switch(current_operation){
                        case TYPE_ADD:
                            used = 1;
                            out_value = out_value + token_value;
                            break;
                        case TYPE_SUB:
                            used = 1;
                            out_value = out_value + token_value;
                            break;
                        case TYPE_DIV:
                            used = 1;
                            out_value = out_value / token_value;
                            break;
                        case TYPE_MULT:
                            used = 1;
                            out_value = out_value * token_value;
                        default:
                            break;
                    }
                }
        }
        token->used = used;
        // printf("after value: %ld\n", out_value);
    }

    free(unused_cache);
    unused_cache = NULL;

    return out_value;
}


long dice_notation_advanced_process(DiceNotationCounters* counters, DiceNotationCache* cache){


    // sort the group cache so the group cache is sorted highest to lowest priority.
    for(uint8_t i = 0; i < counters->group_count; i++){
        GroupToken* token_i = &cache->group_cache[i];
        for(uint8_t j = i+1; j < counters->group_count; j++){
            GroupToken* token_j = &cache->group_cache[j];
            if(token_i->priority < token_j->priority){
                GroupToken temp;
                // don't you love copying memory directly :)
                memcpy(&temp, &cache->group_cache[i], sizeof(GroupToken));
                memcpy(&cache->group_cache[i], &cache->group_cache[j], sizeof(GroupToken));
                memcpy(&cache->group_cache[j], &temp, sizeof(GroupToken));
            } else if(token_i->priority == token_j->priority) {
                // this is for cases where we have matching priority levels
                if(token_i->start_pos > token_j->start_pos){
                    GroupToken temp;
                    // don't you love copying memory directly :)
                    memcpy(&temp, &cache->group_cache[i], sizeof(GroupToken));
                    memcpy(&cache->group_cache[i], &cache->group_cache[j], sizeof(GroupToken));
                    memcpy(&cache->group_cache[j], &temp, sizeof(GroupToken));
                }
            }
        }
    }

    // assign references on real tokens so they can be referenced back to their groups
    for(uint8_t i = 0; i < counters->group_count; i++){
        GroupToken* group = &cache->group_cache[i];
        if(group->priority == 0){
            continue;
        }
        cache->real_tokens[group->start_pos].ref = i;
        cache->real_tokens[group->end_pos].ref = i;
    }

    // printf("sorted group cache\n");
    // dice_notation_print_grouptokens(cache->group_cache, counters->group_count);


    for(uint8_t group_pos = 0; group_pos < counters->group_count; group_pos++){
        GroupToken* group_token = &cache->group_cache[group_pos];
        if(group_token == NULL){
            printf("failed to get group token at index: %d\n", group_pos);
            continue;
        }

        uint8_t current_operation = 0;

        long out_value = 0;

        // process real tokens within range
        for(uint8_t i = group_token->start_pos+1; i < group_token->end_pos; i++){
            RealToken* token = &cache->real_tokens[i];
            uint8_t used = token->used;

            // printf("\ttype: %s, pos: %d, value: %d, used: %d\n", token_type_as_str(token->type), i, token->value, token->used);

            if(used == 1){
                continue;
            }

            uint8_t handle_num = 0;
            long number_value = 0;

            if(token->type == TYPE_DICE) {
                handle_num = 1;
                // unsure if i should mark it as used.
                DiceToken* dice_token = &cache->dice[token->ref];
                number_value = dice_roll(dice_token->amount, dice_token->sides, dice_token->keep_high, dice_token->keep_low);
                used = 1;
                // printf("\t> rolled a %ld\n", number_value);
            } else if(token->type == TYPE_GROUP_START){
                handle_num = 1;
                GroupToken* temp_group_token = &cache->group_cache[token->ref];
                number_value = temp_group_token->value;
                temp_group_token->used = 1;
                used = 1;
            } else {
                switch (token->type)
                {
                    case TYPE_SUB:
                    case TYPE_MULT:
                    case TYPE_DIV:
                    case TYPE_ADD:
                        used = 1;
                        current_operation = token->type;
                        break;
                    case TYPE_NUM:
                        handle_num = 1;
                        number_value = token->value;
                    default:
                        break;
                }
            }

            if(handle_num == 1){
                if(current_operation == 0){
                    used = 1;
                    out_value = number_value;
                } else {
                    switch (current_operation){
                        case TYPE_ADD:
                            out_value = out_value + number_value;
                            used = 1;
                            break;
                        case TYPE_SUB:
                            out_value = out_value - number_value;
                            used = 1;
                            break;
                        case TYPE_DIV:
                            out_value = out_value / number_value;
                            used = 1;
                            break;
                        case TYPE_MULT:
                            out_value = out_value * number_value;
                            used = 1;
                            break;
                        default:
                            printf("unknown math operation given\n");
                            break;
                    }
                    current_operation = 0;
                }
            }

            // printf("\tused: %d\n", used);
            token->used = used;
        }

        // printf("group out value: %ld\n", out_value);

        group_token->value = out_value;
    }
    
    // get group 0
    uint8_t group_zero_pos;
    for(uint8_t i = 0; i < counters->group_count; i++){
        if(cache->group_cache[i].priority == 0){
            group_zero_pos = i;
            break;
        }
    }

    return cache->group_cache[group_zero_pos].value;
}


/**
 * Next steps:
 * - shorten memory footprint
 * - incorporate some bitwise in here to try and shorten the computation
 * - decrease the amount of loops
 * - maybe replace some of these similar types with bitflags
 * - check the numbers to see if they could be unsigned
 * 
 * - [solved] make it so the notation has a limit.
 * - [solved] implement something to actually give a result
 */
long dice_notation(const char* text){
    DiceNotationState state;
    DiceNotationCounters counters;
    DiceNotationCache cache;

    // initialize the structs with 0 to clear out any old memory.
    memset(&state, 0, sizeof(DiceNotationState));
    memset(&counters, 0, sizeof(DiceNotationCounters));
    memset(&cache, 0, sizeof(DiceNotationCache));
    
    state.text = text;
    state.length = strlen(text);
    if(state.length > 200){
        printf("don't feel like doing a dice notation with more than 200 characters\n");
        return 0; // why
    }
    state.tokens = calloc(sizeof(PairToken), state.length);
    if(state.tokens == NULL){
        printf("failed to allocate space for tokens\n");
        return 0;
    }

    int retvalue = 0;
    // printf("parsing text\n");
    retvalue = dice_notation_parse_text(&state, &counters);
    if(retvalue != 0){
        printf("failed to parse dice notation text\n");
        return 0;
    }

    // dice_notation_print_pairtokens(tokens, length);

    // printf("number count: %d\ntoken count: %d\nreal token count: %d\n", number_count, length, real_token_count);

    // printf("----\n");

    // add a check to see if group start count is equal to group end count.

    if(counters.group_start_count != counters.group_end_count){
        printf("group start count '(' should be equal to group end count ')'\ngot:\n\tstart: %d\n\tend: %d\n", counters.group_start_count, counters.group_end_count);
        return -1;
    }

    //remove unneccesary tokens
    cache.real_tokens = calloc(sizeof(RealToken), counters.real_token_count);
    if(cache.real_tokens == NULL){
        printf("failed to allocate space for real_tokens\n");
        return 0;
    }

    cache.dice = calloc(sizeof(DiceToken), counters.dice_count);
    cache.math = calloc(sizeof(MathToken), counters.math_count);
    cache.number = calloc(sizeof(NumberToken), counters.number_count);
    cache.keep = calloc(sizeof(KeepToken), counters.keep_count);

    // printf("organizing tokens\n");
    retvalue = dice_notation_organize_into_cache(&state, &counters, &cache);
    if(retvalue != 0){
        printf("failed to organize tokens into cache\n");
        return 0;
    }

    // printf("real_token_count: %d\n", counters.real_token_count);

    // dice_notation_print_groupplacementtokens(cache.group_start_cache, counters.group_start_count);
    // dice_notation_print_groupplacementtokens(cache.group_end_cache, counters.group_end_count);

    // dice_notation_print_realtokens(real_tokens, real_token_pos);
    // dice_notation_print_dicetokens(dice_cache, dice_cache_pos);

    // lets define keep

    retvalue = dice_notation_define_keep(&counters, &cache);
    if(retvalue != 0){
        printf("failed to define keep tokens\n");
        return 0;
    }

    // lets define dice
    retvalue = dice_notation_define_dice(&counters, &cache);
    if(retvalue != 0){
        printf("failed to define dice tokens\n");
        return 0;
    }

    retvalue = dice_notation_define_groups(&counters, &cache);
    if(retvalue != 0){
        printf("failed to define dice tokens\n");
        return 0;
    }

    // dice_notation_print_grouptokens(cache.group_cache, counters.group_count);

    // // after processing caches
    // dice_notation_print_realtokens(real_tokens, real_token_pos);
    // dice_notation_print_dicetokens(dice_cache, dice_cache_pos);

    // long out_value = dice_notation_simple_process(&counters, &cache);
    long out_value = dice_notation_advanced_process(&counters, &cache);
    
    void** pointers[] = {
        (void**)&cache.dice, (void**)&cache.keep, (void**)&cache.math,
        (void**)&cache.number, (void**)&cache.real_tokens, (void**)&cache.group_start_cache,
        (void**)&cache.group_end_cache, (void**)&cache.group_cache
    };

    for(uint8_t i = 0; i < 5; i++){
        if(*pointers[i] != NULL){
            free(*pointers[i]);
            *pointers[i] = NULL;
        }
    }

    return out_value;
}

int main(void){
    srand(time(NULL));

    // long result = dice_roll(10, 20, 3, 3);
    // printf("result: %ld\n", result);

    long dice_result;

    struct timespec start_time, end_time;

    timespec_get(&start_time, TIME_UTC);
    for(int i = 0; i < 10; i++){
        dice_result = dice_notation("(1d4 * (1d4)) + (1d4 * (1d4))"); // 3kh1kl1d4 + (1d4 * (1d4 + 1)) + (1d4+1)
        // sleep(1);
        printf("got dice result of: %ld\n", dice_result);
    }
    timespec_get(&end_time, TIME_UTC);
    double time_spent = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;
    printf("elapsed time: %lf seconds\n", time_spent);

    return 0;
}