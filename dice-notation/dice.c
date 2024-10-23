
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdint.h"
#include "math.h"
#include "string.h"
#include "limits.h"

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

        uint16_t offset = amount - keep_highest;
        for(uint8_t i = 0; i < keep_highest; i++){
            high[i] = values[offset + i];
        }

        for(uint8_t i = 0; i < keep_lowest; i++){
            low[i] = values[i];
        }

        for(uint8_t i = 0; i < keep_highest; i++){
            total = total + high[i];
        }

        for(uint8_t i = 0; i < keep_lowest; i++){
            total = total + low[i];
        }

        if(high != NULL){
            free(high);
            high = NULL;
        }

        if(low != NULL){
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
    size_t length = strlen(text);
    if(length > 200){
        printf("don't feel like doing a dice notation with more than 200 characters\n");
        return 0; // why
    }
    PairToken* tokens = calloc(sizeof(PairToken), length);
    if(tokens == NULL){
        printf("failed to allocate space for tokens\n");
        return 0;
    }

    uint16_t dice_count = 0;
    uint16_t keep_count = 0;
    uint16_t math_count = 0;

    uint16_t group_start_count = 0;
    uint16_t group_end_count = 0;

    signed int last_number_index = -1;
    uint8_t processing_number = 0;
    uint16_t number_count = 0;

    uint8_t real_token_count = length;
    for(uint8_t i = 0; i < (uint8_t)length; i++){
        char c = text[i];
        uint8_t handling_number = 0;
        uint8_t value;

        switch(c){
            case 'd':
                tokens[i].type = TYPE_DICE;
                tokens[i].value = i; //define where it's located
                dice_count++;
                break;
            case 'k':
                // tokens[i][0] = TYPE_EMPTY; // keep the TYPE_KEEP, but we'll be placing an empty here for now.
                keep_count++;
                tokens[i+1].type = TYPE_EMPTY; // just a reminder but if we're setting a type here and we don't skip incriment the I then this will automatically be overwritten in the next iteration.
                real_token_count--; // since we're assigning empty for the initial token
                tokens[i].value = 0;
                switch(text[i+1]) {
                    case 'h':
                        tokens[i].type = TYPE_KEEP_HIGH;
                        break;
                    case 'l':
                        tokens[i].type = TYPE_KEEP_LOW;
                        break;
                    default:
                        printf("failed to parse keep statement\n");
                        break;
                }
                break;
            case '(':
                tokens[i].type = TYPE_GROUP_START;
                group_start_count++;
                break;
            case ')':
                tokens[i].type = TYPE_GROUP_END;
                group_end_count++;
                break;
            case '+':
                tokens[i].type = TYPE_ADD;
                tokens[i].value = 0;
                math_count++;
                break;
            case '-':
                tokens[i].type = TYPE_SUB;
                math_count++;
                break;
            case '*':
                tokens[i].type = TYPE_MULT;
                math_count++;
                break;
            case '/':
                tokens[i].type = TYPE_DIV;
                math_count++;
                break;
            case ' ':
                tokens[i].type = TYPE_EMPTY; // keep these seperate just in case
                real_token_count = real_token_count - 1;
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
                tokens[last_number_index].value = (tokens[last_number_index].value * 10) + value;
                tokens[i].type = TYPE_EMPTY;
                real_token_count--;
            } else {
                last_number_index = i;
                processing_number = 1;
                number_count++;
                tokens[i].type = TYPE_NUM;
                tokens[i].value = value;
            }
        } else {
            if(processing_number == 1){
                processing_number = 0;
                last_number_index = -1;
            }
        }
    }

    // dice_notation_print_pairtokens(tokens, length);

    // printf("number count: %d\ntoken count: %d\nreal token count: %d\n", number_count, length, real_token_count);

    // printf("----\n");

    // add a check to see if group start count is equal to group end count.

    if(group_start_count != group_end_count){
        printf("group start count '(' should be equal to group end count ')'\n");
        return -1;
    }

    

    //remove unneccesary tokens
    RealToken* real_tokens = calloc(sizeof(RealToken), real_token_count);
    if(real_tokens == NULL){
        printf("failed to allocate space for real_tokens\n");
        return 0;
    }
    int real_token_pos = 0;

    DiceToken* dice_cache = calloc(sizeof(DiceToken), dice_count);
    MathToken* math_cache = calloc(sizeof(MathToken), math_count);
    NumberToken* number_cache = calloc(sizeof(NumberToken), number_count);
    KeepToken* keep_cache = calloc(sizeof(KeepToken), keep_count);

    uint16_t dice_cache_pos = 0;
    uint16_t math_cache_pos = 0;
    uint16_t number_cache_pos = 0;
    uint16_t keep_cache_pos = 0;

    // organize the tokens to remove dead space and also have it record data on positions of tokens.
    for(int i = 0; i < length; i++){
        TOKEN_TYPE _type = tokens[i].type;
        int _value = tokens[i].value;
        if(_type == TYPE_EMPTY){
            continue;
        }

        int out_ref = -1; // -1 could mean no ref, this could also be a 16 bit int

        switch(_type){
            case TYPE_DICE:
                dice_cache[dice_cache_pos].pos = real_token_pos;
                out_ref = dice_cache_pos;
                dice_cache_pos++;
                break;
            case TYPE_KEEP_LOW:
            case TYPE_KEEP_HIGH:
                keep_cache[keep_cache_pos].pos = real_token_pos;
                keep_cache[keep_cache_pos].type = _type;
                out_ref = keep_cache_pos;
                keep_cache_pos++;
                break;
            case TYPE_NUM:
                number_cache[number_cache_pos].pos = real_token_pos;
                number_cache[number_cache_pos].value = _value;
                out_ref = number_cache_pos;
                number_cache_pos++;
                break;
            case TYPE_SUB:
            case TYPE_DIV:
            case TYPE_MULT:
            case TYPE_ADD:
                math_cache[math_cache_pos].pos = real_token_pos;
                math_cache[math_cache_pos].type = _type;
                out_ref = math_cache_pos;
                math_cache_pos++;
                break;
            default:
                // unhandled but that's fine
                break;
        }

        real_tokens[real_token_pos].ref = out_ref;
        real_tokens[real_token_pos].type = _type;
        real_tokens[real_token_pos].value = _value;
        real_token_pos++;
    }

    // dice_notation_print_realtokens(real_tokens, real_token_pos);
    // dice_notation_print_dicetokens(dice_cache, dice_cache_pos);

    free(tokens);
    tokens = NULL;

    // lets define keep

    for(int i = 0; i < keep_cache_pos; i++){
        int pos = keep_cache[i].pos;
        int next_type = real_tokens[pos+1].type;
        if(real_tokens[pos+1].type == TYPE_NUM){
            int value = real_tokens[pos+1].value;
            keep_cache[i].value = value; // might not be needed
            real_tokens[pos].value = value;
            real_tokens[pos+1].used = 1;
        } else {
            printf("no value found for keep_cache\n");
            keep_cache[i].value = 0; // no value found, we could have this as 0 or 1 depending on how we want to have to debug stuff.
        }
    }

    // lets define dice

    for(int i = 0; i < dice_cache_pos; i ++){
        DiceToken* token = &dice_cache[i];
        const int dice_pos = dice_cache[i].pos;
        int before_pos = dice_pos-1;
        int after_pos = dice_pos+1;

        unsigned int keep_high = 0; // should not be negative
        unsigned int keep_low = 0; // should not be negative
        unsigned int amount = 1;
        unsigned int sides = 0;

        uint8_t ok = 1;

        // looking before dice pos
        for(int i = dice_pos - 1; 0 <= i && ok != 0; i--){
            if(real_tokens[i].used == 1){
                continue; // token already in use
            }

            int real_type = real_tokens[i].type;
            int real_value = real_tokens[i].value;
            switch(real_type){
                case TYPE_KEEP_HIGH:
                    keep_high = real_value;
                    break;
                case TYPE_KEEP_LOW:
                    keep_low = keep_low + real_value;
                    break;
                case TYPE_NUM:
                    amount = real_value;
                    break;
                default:
                    ok = 0;
                    continue; // technically this passes over the part where we define if it was used, but imma leave this here.
                    break;
            }
            if(ok){
                real_tokens[i].used = 1; // bulk check if it was used or not.
            }
        }

        ok = 1;

        // look after

        for(int i = dice_pos+1; i < real_token_pos && ok != 0; i++){
            if(real_tokens[i].used == 1){
                continue; // token already in use
            }

            int real_type = real_tokens[i].type;
            int real_value = real_tokens[i].value;
            switch(real_type){
                case TYPE_KEEP_HIGH:
                    keep_high = real_value;
                    break;
                case TYPE_KEEP_LOW:
                    keep_low = keep_low + real_value;
                    break;
                case TYPE_NUM:
                    sides = sides + real_value;
                    break;
                default:
                    ok = 0;
                    continue;
                    break;
            }
            if(ok){
                real_tokens[i].used = 1; // bulk check if it was used or not.
            }
        }

        // last step
        dice_cache[i].amount = amount;
        dice_cache[i].sides = sides;
        dice_cache[i].keep_high = keep_high;
        dice_cache[i].keep_low = keep_low;
    }


    // // after processing caches
    // dice_notation_print_realtokens(real_tokens, real_token_pos);
    // dice_notation_print_dicetokens(dice_cache, dice_cache_pos);

    int unused_count = real_token_pos;
    for(unsigned int i = 0; i < real_token_pos; i++){
        if(real_tokens[i].used == 1){
            unused_count--;
        }
    }

    int unused_cache[unused_count];
    int unused_cache_pos = 0;

    // try walking through current tokens

    for(unsigned int i = 0; i < real_token_pos; i++){
        if(real_tokens[i].used != 1){
            unused_cache[unused_cache_pos] = i;
            unused_cache_pos = unused_cache_pos + 1;
        }
    }

    // roll some stuff

    // printf("rolling\n");

    long out_value = 0;

    uint8_t current_operation = 0;

    // printf("unusded cache size: %d\n", unused_cache_pos);s

    for(int i = 0; i < unused_cache_pos; i++){
        uint16_t pos = unused_cache[i];
        RealToken* token = &real_tokens[pos];
        if(token == NULL){
            printf("failed to get token\n");
            continue;
        }
        // printf("before value: %ld\n", out_value);
        // printf("got type: %s\n", token_type_as_str(token->type));
        switch(token->type){
            case TYPE_DICE: {
                token->used = 1;
                long rolled_dice = dice_roll(dice_cache[token->ref].amount, dice_cache[token->ref].sides, dice_cache[token->ref].keep_high, dice_cache[token->ref].keep_low);
                // printf("rolled a %ld\n", rolled_dice);
                out_value = out_value + rolled_dice;
            }; break;
            case TYPE_ADD:
                token->used = 1;
                current_operation = TYPE_ADD;
                break;
            case TYPE_NUM:
                if(current_operation != 0){
                    switch(current_operation){
                        case TYPE_ADD:
                            token->used = 1;
                            out_value = out_value + token->value;
                            break;
                        case TYPE_SUB:
                            token->used = 1;
                            out_value = out_value + token->value;
                            break;
                        default:
                            break;
                    }
                }
        }
        // printf("after value: %ld\n", out_value);
    }

    free(dice_cache);
    dice_cache = NULL;

    free(keep_cache);
    keep_cache = NULL;

    free(math_cache);
    math_cache = NULL;

    free(number_cache);
    number_cache = NULL;

    free(real_tokens);
    real_tokens = NULL;

    return out_value;
}

int main(void){
    srand(time(NULL));

    // long result = dice_roll(10, 20, 3, 3);
    // printf("result: %ld\n", result);

    long dice_result;

    for(int i = 0; i < 20; i++){
        dice_result = dice_notation("2kh1d20 + 2 ");
        printf("got dice result of: %ld\n", dice_result);
    }

    return 0;
}