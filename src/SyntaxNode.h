typedef enum{
    LEX_INT=0,
    LEX_FLOAT=1,
    LEX_ID=2,
    LEX_TYPE=3,
    LEX_OTHER_TOKEN=4,
    SYNTACTIC_UNIT=5,
    SYNTACTIC_UNIT_EMPTY
}NODE_TYPE;

struct Node{
    struct Node* child;
    struct Node* next_sib;
    char name[32];
    union{
        int int_constant;
        double float_constant;
        char str_constant[32];
    };
    int lineno;
    NODE_TYPE type;
};