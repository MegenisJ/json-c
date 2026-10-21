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

enum ValueType{
   VALUETYPE_OBJECT,
   VALUETYPE_STRING,
   VALUETYPE_NUMBER,
   VALUETYPE_ARRAY,
   VALUETYPE_NULL,
   VALUETYPE_BOOL
};

typedef struct {
    char* key; //I guess optional? in the case where the entire file is wrapped in array?
    void* value;
    enum ValueType type;
} JSON_VALUE;

typedef struct {
    JSON_VALUE *items;
    size_t count;
    size_t capacity;
} JSON_VALUES;

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
int get_next_token(char *json,int len, token* next){
    //printf("%s\n",json); //for debugging 
    for(int i=0;i<=len-1;i++){
        switch (json[i]) {
            case ' ' : 
            case '\n': 
                continue;        
            case '"': {

                if (i+1 == len) 
                    break;

                const char* result = strchr(json+i+1, '"');
                if (result != NULL) { 
                    int pos = result - json ;
                    next ->name = TOKEN_STRING;
                    int len = pos-i-1;
                    char *temp = malloc(len + 1);
                    strncpy(temp, json+i+1,len);
                    temp[len] = '\0';
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
            //TODO Fix janky case checking
            case 'N':
                if (strncmp(json + i + 1 , "ULL",3) == 0 ) {
                    next->name=TOKEN_FALSE;
                    return i+3;
                }
                break;
            case 'n':
                if (strncmp(json + i + 1 , "ull",3) == 0 ) {
                    next->name=TOKEN_FALSE;
                    return i+3;
                }
                break;
            case 'F':
                if (strncmp(json + i + 1 , "ALSE", 4) == 0 ) {
                    next->name=TOKEN_FALSE;
                    return i+4;
                }
                break;
            case 'f':
                if (strncmp(json + i + 1, "alse", 4) == 0) {
                    next->name=TOKEN_FALSE;
                    return i+4;
                }
                break;
            case 't':
                if (strncmp(json + i + 1, "rue",3) == 0) {
                    next->name=TOKEN_FALSE;
                    return i+3;
                }
                break;
            case 'T':
                if (strncmp(json + i + 1, "RUE",3) == 0) {
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

int get_tokens(char *json, int len, tokens *tokens){
    token next;
    next.name = TOKEN_NULL;
    int nextpos; 
    int size = len;
    //TODO :: Refactor to allow for position in starting string to be returned
    while((nextpos = get_next_token(json,  size, &next)) >= 0 && nextpos != 999){
        da_append((*tokens) ,next); 
        json = json + nextpos + 1; 
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

int parse_object (int startingpos, tokens *tokens, JSON_VALUES *values){
    //CHECKING THROUGH UNTIL WE GET TOKEN_END_OBJECT?  
    //can this be recursive
    return 0;
}


typedef struct 
{
    char* value;
} ArrayItem;

typedef struct {
    size_t count;
    size_t capacity;
    ArrayItem *items;
} ArrayItems;

int parse_array (int startingpos, tokens *tokens, JSON_VALUES *values){
    //Checking through until we get an TOKEN_END_ARRAY
    enum tokenName type = tokens -> items[startingpos + 1].name; //I guess we should verify the valid types for an array
    printf("Parsing array of type %s", TOKENNAME_STRING[type]);
    ArrayItems items = {0};// TODO: I dont know if I need to malloc / free
    JSON_VALUE array;
    array.key = "batman"; //TODO:: Add names
    array.type = VALUETYPE_ARRAY;
     
    //Create an array
    //Add until we find end array or anything not of the same type
    //Add to values
    enum tokenName Prev = TOKEN_DELIMETER;
    for(int x = startingpos + 1; x <= tokens->count; ++x){
        token next = tokens -> items[x];

        if(next.name == TOKEN_DELIMETER)
        {
            continue;
        }

        if(next.name == TOKEN_CLOSE_ARRAY)
        {
            //add to values, 
            array.value = &items;
            da_append((*values) , (JSON_VALUE)array);
            printf("Array items: ");
            for(int i = 0; i < items.count; ++i){
                printf("%s ", items.items[i].value);
            }
            printf("\n");
            return x+1; // return the end of the array
        }
        if(next.name != type) {
            printf("Array items not of matching type %sVALUE%s, \n",TOKENNAME_STRING[next.name], t_print(next)); //add all items and which type they are not?
            return -1; //
        }

        ArrayItem item = {next.value};
        da_append(items,item);

    }
    return 0;
}

//Check for correct order of tokens, outputs values which is a valid json object 
//PARSE INTO A TREE AS JSON IS NOT ORDER SPECIFIC 
//Returns negative values for errors 
int parse_tokens(tokens *tokens, JSON_VALUES *values){

    for(int x = 0; x < tokens -> count; x++){
        //printf("%s", TOKENNAME_STRING [tokens -> items[x].name]);
        //printf(" : %s\n",t_print(tokens -> items[x]));
        if(tokens -> items[x].name == TOKEN_OPEN_OBJECT){
            JSON_VALUE v;
            v.type = VALUETYPE_OBJECT;
            parse_object(x,tokens, values);
        }
        if(tokens -> items[x].name == TOKEN_OPEN_ARRAY){
            parse_array(x,tokens, values);
        }
    } 
    return 0;
    //Implement a tree data structure
    //Parse json tokens into the tree (recursively?) 
    //Other than around the entire object every value must contain a key
    //
    //
    //Object = ARRAY of JSON_VALUE
    //ARRAY = 
}

int main(int argc, char * argv[]){
    FILE *fp = fopen(argv[1], "r");    
    assert(fp != NULL);
    char line[256]; 
    tokens tokens = {0};
    int ERROR_CODE;

    while (fgets(line, sizeof(line), fp)) { 
        if((ERROR_CODE = get_tokens(line, strlen(line), &tokens)) < 0){
            printf("Invalid tokens found \n");
            return ERROR_CODE; 
        };
    } 
    
   // printf("Token count: %zu\n",tokens.count);
   // for(int x = 0; x<tokens.count; x++){
   //     printf("%s", TOKENNAME_STRING [tokens.items[x].name]);
   //     printf(" : %s\n",t_print(tokens.items[x]));
   // } 

    JSON_VALUE init;
    init.key = "batman";
    init.type = VALUETYPE_OBJECT;

    JSON_VALUES values = {0};
    da_append(values,init);

    if((ERROR_CODE = parse_tokens(&tokens, &values)) < 0){
            printf("Tokens in invalid order\n");
            return ERROR_CODE; 
    }

    free(tokens.items);
    return 0;
}
