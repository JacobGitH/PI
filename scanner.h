
typedef enum {
    TYPE_UNDEF, // Nedefinovaný stav, token začíná s touto hodnotou

    TYPE_ID, // Identifikátor
    TYPE_ID_DOLLAR, // identifikátor s $ ($return, $karel)

    TYPE_ELSE, // Klíčová slova
    TYPE_FLOAT,
    TYPE_FUNCTION,
    TYPE_IF,
    TYPE_INT,
    TYPE_NULL,
    TYPE_RETURN,
    TYPE_STRING,
    TYPE_VOID,
    TYPE_WHILE,

    TYPE_PLUS, // Binární operátory +, -, *, / , =
    TYPE_MINUS,
    TYPE_MULTIPLY,
    TYPE_DIVISION,
    TYPE_EQUAL, 

    TYPE_CONCAT, // Řetězcové operátory ., ===, !==
    TYPE_COMPARE_STR,
    TYPE_COMPARE_STR_NEG,

    TYPE_LOWER, // Relační operátory <, >, <=, >=
    TYPE_HIGHER,
    TYPE_LOWER_EQUAL,
    TYPE_HIGHER_EQUAL,

    TYPE_SEMICOLON,     
    TYPE_BRACKET_LEFT, 
    TYPE_BRACKET_RIGHT,
    TYPE_CURLY_BRACKET_LEFT,
    TYPE_CURLY_BRACKET_RIGHT,
    TYPE_COLON,
    TYPE_COMMA,

    TYPE_INT_VALUE,     //konstantni hodnoty, 100, 100.24/100.25e-5/100e+2/100.245E-20, "AHOJ"
    TYPE_FLOAT_VALUE,
    TYPE_STRING_VALUE,

    TYPE_EOP, // konec programu ?>
    TYPE_EOF // konec souboru

}tokenType;

/**
 * @brief dvousměrně vázaný seznam
 * 
 */
typedef struct token{			
	char *tokenString;          // string pro hodnotu
    tokenType tokenType;        // typ tokenu
    struct token *prevToken;	// předešlý token
	struct token *nextToken;	// nadcházející token
	int linePos;		        // řádek 
	int charLinePos;		    // umístění na řádku
} *tokenList;

//enumeration for finite state machine
typedef enum {
    MOVE_EXPECT_EQUAL,
    MOVE_END,
    MOVE_START,
    MOVE_ID,
    MOVE_FLOAT,
    MOVE_CONCAT,
    MOVE_NUM,
    MOVE_EOP,
    MOVE_STRING_CONTENT,
    MOVE_EXPONENT,
    MOVE_DIVIDE,
    MOVE_EXPECT_EQUAL_NEG,
    MOVE_GREATER_EQUAL,
    MOVE_LOWER_EQUAL,
    MOVE_KEYWORD,
    MOVE_LITERAL,
    MOVE_ESCAPE_SEQ,
    MOVE_ESCAPE_HEX,
    MOVE_ESCAPE_OCT,
    MOVE_LOWER,
    MOVE_HIGHER,
    MOVE_HEADER,
    MOVE_EXPECT_COMMENT,
    MOVE_NESTED_COMMENT,
    MOVE_QUESTIONMARK

}FSMState;


//functions declaration
tokenList FSM();
void printTokens(tokenList tokens);
//start iterate by 1 by call
tokenList getToken();
// decrement token list by 1
void decToken();