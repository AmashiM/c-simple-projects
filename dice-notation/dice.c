
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdint.h"
#include "math.h"
#include "string.h"


long dice_roll(int amount, int sides, uint8_t keep_highest, uint8_t keep_lowest){
    long total = 0;

    uint8_t mode = 0;

    // if keep_highest doesn't equal 0
    if(keep_highest){
        mode = 1;
    }
    // if keep_lowest doesn't equal 0
    else if(keep_lowest){
        mode = 2;
    }

    for(int i = 0; i < amount; i++){
        int result = (int)((rand() % sides) + 1);
        switch(mode){
            case 0:
                total = total + result;
                break;
            case 1:
                if(total < result){
                    total = result;
                }
                break;
            case 2:
                if(total > result){
                    total = result;
                }
                break;
        }
    }

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

/**
 * Next steps:
 * - shorten memory footprint
 * - incorporate some bitwise in here to try and shorten the computation
 * - decrease the amount of loops
 * - maybe replace some of these similar types with bitflags
 * - check the numbers to see if they could be unsigned
 * 
 * - [solved] implement something to actually give a result
 */
long dice_notation(const char* text){
    unsigned int length = strlen(text);
    PairToken tokens[length];

    int dice_count = 0;
    int keep_count = 0;
    int math_count = 0;

    int group_start_count = 0;
    int group_end_count = 0;

    int last_number_index = -1;
    uint8_t processing_number = 0;
    int number_count = 0;

    unsigned int real_token_count = length;
    for(unsigned int i = 0; i < length; i++){
        char c = text[i];
        uint8_t handling_number = 0;
        uint8_t value;

        switch(c){
            case 'd':
                tokens[i].type = TYPE_DICE;
                tokens[i].value = i; //define where it's located
                dice_count = dice_count + 1;
                break;
            case 'k':
                // tokens[i][0] = TYPE_EMPTY; // keep the TYPE_KEEP, but we'll be placing an empty here for now.
                keep_count = keep_count + 1;
                tokens[i+1].type = TYPE_EMPTY; // just a reminder but if we're setting a type here and we don't skip incriment the I then this will automatically be overwritten in the next iteration.
                real_token_count = real_token_count - 1; // since we're assigning empty for the initial token
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
                group_start_count = group_start_count + 1;
                break;
            case ')':
                tokens[i].type = TYPE_GROUP_END;
                group_end_count = group_end_count + 1;
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
                real_token_count = real_token_count - 1;
            } else {
                last_number_index = i;
                processing_number = 1;
                number_count = number_count + 1;
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
    RealToken real_tokens[real_token_count];
    int real_token_pos = 0;

    DiceToken dice_cache[dice_count];
    MathToken math_cache[math_count];
    NumberToken number_cache[number_count];
    KeepToken keep_cache[keep_count]; // it's 3 because we reserve the 3rd value for the defined amount

    int dice_cache_pos = 0;
    int math_cache_pos = 0;
    int number_cache_pos = 0;
    int keep_cache_pos = 0;

    // organize the tokens to remove dead space and also have it record data on positions of tokens.
    for(int i = 0; i < length; i++){
        int _type = tokens[i].type;
        int _value = tokens[i].value;
        if(_type == TYPE_EMPTY){
            continue;
        }

        switch(_type){
            case TYPE_DICE:
                dice_cache[dice_cache_pos].pos = real_token_pos;
                real_tokens[real_token_pos].ref = dice_cache_pos;
                dice_cache_pos++;
                break;
            case TYPE_KEEP_LOW:
            case TYPE_KEEP_HIGH:
                keep_cache[keep_cache_pos].pos = real_token_pos;
                keep_cache[keep_cache_pos].type = _type;
                real_tokens[real_token_pos].ref = keep_cache_pos;
                keep_cache_pos++;
                break;
            case TYPE_NUM:
                number_cache[number_cache_pos].pos = real_token_pos;
                number_cache[number_cache_pos].value = _value;
                real_tokens[real_token_pos].ref = number_cache_pos;
                number_cache_pos++;
                break;
            case TYPE_SUB:
            case TYPE_DIV:
            case TYPE_MULT:
            case TYPE_ADD:
                math_cache[math_cache_pos].pos = real_token_pos;
                math_cache[math_cache_pos].type = _type;
                real_tokens[real_token_pos].ref = math_cache_pos;
                math_cache_pos++;
                break;
            default:
                // unhandled but that's fine
                break;
        }

        real_tokens[real_token_pos].type = _type;
        real_tokens[real_token_pos].value = _value;
        real_token_pos++;
    }

    // dice_notation_print_realtokens(real_tokens, real_token_pos);
    // dice_notation_print_dicetokens(dice_cache, dice_cache_pos);

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

        unsigned int keep_high = 0;
        unsigned int keep_low = 0;
        int amount = 1;
        int sides = 0;

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

    return out_value;
}

int main(void){
    srand(time(NULL));

    long dice_result;

    dice_result = dice_notation("2kh1d20 + 2 ");
    printf("got dice result of: %ld\n", dice_result);

    dice_result = dice_notation("2kh1d20 + 2 ");
    printf("got dice result of: %ld\n", dice_result);

    return 0;
}