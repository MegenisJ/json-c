#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "../useful-c/dynamic_array.h"


#define FOREACH_TOKENNAME(TOKEN) \
   TOKEN( TOKEN_OPEN_OBJECT)\
   TOKEN( TOKEN_CLOSE_OBJECT)\
   TOKEN( TOKEN_ASSIGNMENT)\
   TOKEN( TOKEN_DELIMETER)\
   TOKEN( TOKEN_OPEN_ARRAY)\
   TOKEN( TOKEN_CLOSE_ARRAY) \
   TOKEN( TOKEN_NUMBER) \
   TOKEN( TOKEN_STRING) \
   TOKEN( TOKEN_TRUE) \
   TOKEN( TOKEN_FALSE)\
   TOKEN( TOKEN_NULL)\

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum tokenName {
    FOREACH_TOKENNAME(GENERATE_ENUM)
};
static const char *TOKENNAME_STRING[] = {
    FOREACH_TOKENNAME(GENERATE_STRING)
};

typedef struct {
    enum tokenName name;
    void* value;
} token;

typedef struct {
    token *items;
    size_t count;
    size_t capacity;
} tokens;

char* t_print(token t){
    if(t.name == TOKEN_STRING){
        return t.value;
    }
    if(t.name == TOKEN_NUMBER){
        return t.value;
    }
    return "";
}

//gets position of final char in next token. Returns -1 if does not exit. Returns len if no tokens found.
int getNextToken(char *json,int len, token* next){
    //printf("%s\n",json); //for debugging 
    for(int i=0;i<=len-1;i++){
        switch(json[i]){
            //TODO: Check for string
            case ' ' : 
            case '\n': 
                continue;        
            case '"': {
                if(i+1 == len){break;}
                const char* result = strchr(json+i+1, '"');
                if(result != NULL){ 
                    int pos = result - json ;
                    next ->name = TOKEN_STRING;
                    int len = pos-i-1;
                    char *temp = malloc(len + 1);
                    strncpy(temp, json+i+1,len);
                    temp[len] = '\0';
                    //printf("String token value: [%s] \n",temp);
                    next -> value = temp; 
                    return pos;
                }
                printf("Invalid JSON, string contains no closing brace");
            }
            case '{':
                next->name = TOKEN_OPEN_OBJECT;
                return i;
            case '}':
                next->name = TOKEN_CLOSE_OBJECT;
                return i;
            case ',':
                next->name = TOKEN_DELIMETER;
                return i;
            case '[':
                next->name = TOKEN_OPEN_ARRAY;
                return i;
            case ']':
                next->name = TOKEN_CLOSE_ARRAY;
                return i;
            case ':':
                next->name = TOKEN_ASSIGNMENT;
                return i;
            //TODO: Check for number 
            case 'N':
                if(strncmp(json + i + 1 , "ULL",3) == 0 ){
                    next->name=TOKEN_FALSE;
                    return i+3;
                }
                break;
            case 'n':
                if(strncmp(json + i + 1 , "ull",3) == 0 ){
                    next->name=TOKEN_FALSE;
                    return i+3;
                }
                break;
            case 'F':
                if(strncmp(json + i + 1 , "ALSE", 4) == 0 ){
                    next->name=TOKEN_FALSE;
                    return i+4;
                }
                break;
            case 'f':
                if(strncmp(json + i + 1, "alse", 4) == 0){
                    next->name=TOKEN_FALSE;
                    return i+4;
                }
                break;
            case 't':
                if(strncmp(json + i + 1, "rue",3) == 0){
                    next->name=TOKEN_FALSE;
                    return i+3;
                }
                break;
            case 'T':
                if(strncmp(json + i + 1, "RUE",3) == 0){
                    next->name=TOKEN_FALSE;
                    return i+3;
                }
                break;

        }
        if(isdigit(json[i])){
            next->name=TOKEN_NUMBER;
            
            for(int j=i;j<=len-1;j++){
                //TODO: ADD RULES FOR NUMBERS 
                //Check for type - int /decimal 
                if(isdigit(json[j]) == 0 && json[j] != '.' ){
                     
                    char *temp = malloc(j + 1);
                    strncpy(temp, json+i,j-1);
                    temp[j+1] = '\0';
                    //printf("Number token value: [%s] \n",temp);
                    next -> value = temp; 
                    return j;

                }
            }
        }
                    
        printf("Unknown character found at pos %d.[%c] \n", i,json[i]);
        //TODO: Update error handling to return position of error
        return -1;
    }
    //TODO: Update error handling    
    //No tokens found in string (not an error?!) 
    return 999;
}

int getTokens(char *json, int len, tokens *tokens){
    token next;
    next.name = TOKEN_NULL;
    int nextpos; 
    int size = len;
    //TODO :: Refactor to allow for position in starting string to be returned
    while((nextpos = getNextToken(json,  size, &next)) >= 0 && nextpos != 999){
        da_append((*tokens) ,next); 
        json = json + nextpos+1; 
        size = size - nextpos - 1;
    }
    if(nextpos == 999){
        return 1;
    }
    if(nextpos < 0){
        return -1;
    }
    return 0;
    //TODO :: Handle negative return values properly to indicate in the console the location of the error.
}

int main(int argc, char * argv[]){
    FILE *fp = fopen(argv[1], "r");    
    assert(fp != NULL);
    char line[256]; 
    tokens tokens = {0};
    int ERROR_CODE;

    while (fgets(line, sizeof(line), fp)) { 
        if((ERROR_CODE = getTokens(line, strlen(line), &tokens)) < 0){
            printf("Failure to read json\n");
            return ERROR_CODE; 
        };
    } 

    printf("Token count: %zu\n",tokens.count);
    for(int x = 0; x<tokens.count; x++){
        printf("%s", TOKENNAME_STRING [tokens.items[x].name]);
        printf(" : %s\n",t_print(tokens.items[x]));
    } 

    free(tokens.items);
    return 0;
}
