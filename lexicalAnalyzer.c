// Test
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOK_WHILE 256
#define TOK_BREAK 257
#define TOK_CASE 258
#define TOK_DEFAULT 259
#define TOK_ELSE 260
#define TOK_FOR 261
#define TOK_ID 262
#define TOK_IF 263
#define TOK_INTCONST 264
#define TOK_SWITCH 265
#define TOK_NOTEQ 266
#define TOK_RELEQ 267
#define TOK_BITRSHFT 268
#define TOK_BITLSHFT 269
#define TOK_GREQ 270
#define TOK_LSEQ 271
#define TOK_INC 272
#define TOK_DEC 273

FILE *yyin;
char *yytext;
int yylen;

enum States{WhiteSpaceState, TextState, NumericState, UnderscoreState, SpecialState, SuperSpecialState, InvalidState, EndState} states;
int currentState = -1;
int prevState = -1;
int prevNonWhiteState = -1;
char prevChar;
int flag = -1;
int clearFlag = -1;

int yylex();

int main(int argc, char *argv[]) {
    int token;
    yytext = malloc(256);
    yylen = 0;
    if(argc!=2) {
        fprintf(stderr,"Usage \n");
        exit(1);
    } else {
        yyin = fopen(argv[1],"r");
        while(!feof(yyin)) {
            token = yylex();
            printf("%d\t%s\n",token, yytext);
        }
    }
    return 0;
}

int yylex() {
    char ch;
    prevState = currentState;
    if(prevState != WhiteSpaceState)
        prevNonWhiteState = prevState;
    if(yyin == 0)
        yyin = stdin;

    ch = getc(yyin);

    switch(ch) {
        case ' ':
        case '\t':
        case '\n': currentState = WhiteSpaceState;
        break;

        case '{' :
        case '}' :
        case '(' :
        case ')' :
        case '*' :
        case ';' :
        case '/' : currentState = SpecialState;
        break;
        
        case '+' :
        case '-' :
        case '!' :
        case '>' :
        case '<' :  
        case '=' : currentState = SuperSpecialState;
        break;

        case '_' : currentState = UnderscoreState;
        break;

        case 48 ... 57 : currentState = NumericState;
        break;
        
        case 97 ... 122 :
        case 65 ... 90 : currentState = TextState;
        break;

        case -1 : currentState = EndState;
        break;
        
        default : currentState = InvalidState;
    }
    // yytext[yylen] = ch;
    // yylen++;
    // prevChar = ch;
    if(clearFlag == 1) {
        yylen = 0;
        //yytext = "";
        free(yytext);
        yytext = malloc(256);
        clearFlag = 0;
    }

    if(currentState == InvalidState) {
        fprintf(stderr, "Invalid! Language not recognized\t%c\n", ch);
        clearFlag = 1;
        return yylex();
    }

    if(currentState == TextState) {
        if(flag == 2) {
            fprintf(stderr, "Invalid identifier\n");
            exit(1);
        }
        flag = 1;
        yytext[yylen] = ch;
        yylen++;
        prevChar = ch;
        return yylex();
    } else if(currentState == NumericState) {
        if(prevState == TextState || prevState == UnderscoreState) {
            flag = 1;
        } else {
            flag = 2;
        }
        yytext[yylen] = ch;
        yylen++;
        prevChar = ch;
        return yylex();
    } else if(currentState == UnderscoreState) {
        if(flag == 2) {
            fprintf(stderr, "Invalid identifier\n");
            exit(1);
        }
        flag = 1;
        yytext[yylen] = ch;
        yylen++;
        prevChar = ch;
        return yylex();
    } else {
        if(flag == 1) {
            int token = -1;
            if(strcmp(yytext, "while") == 0) {
                token =  TOK_WHILE;
            } else if(strcmp(yytext, "if") == 0) {
                token =  TOK_IF;
            } else if(strcmp(yytext, "else") == 0) {
                token =  TOK_ELSE;
            } else if(strcmp(yytext, "for") == 0) {
                token =  TOK_FOR;
            } else if(strcmp(yytext, "switch") == 0) {
                token =  TOK_SWITCH;
            } else if(strcmp(yytext, "case") == 0) {
                token =  TOK_CASE;
            } else if(strcmp(yytext, "break") == 0) {
                token =  TOK_BREAK;
            } else if(strcmp(yytext, "default") == 0) {
                token =  TOK_DEFAULT;
            } else {
                token = TOK_ID;
            }

            fseek(yyin, -1, SEEK_CUR);
            flag = -1;
            clearFlag = 1;
            // yylen = 0;
            // yytext = "";
            // yytext = malloc(256);
            return token;
        } else if(flag == 2) {
            fseek(yyin, -1, SEEK_CUR);
            flag = -1;
            clearFlag = 1;
            return TOK_INTCONST;
        } else {
            if(currentState != WhiteSpaceState) {
                yytext = "";
                yytext = malloc(256);
                yytext[0] = ch;
                yylen = 1;
                prevChar = ch;

                if((prevNonWhiteState == SpecialState || prevNonWhiteState == SuperSpecialState) && ch == '-') {
                    char ch2;
                    int i=0;
                    while((ch2 = getc(yyin)) == ' ') {
                        i++;
                    }
                    fseek(yyin, -i - 1, SEEK_CUR);
                    if(ch2 >= 48 && ch2<= 57){
                        return yylex();
                    }
                }
                clearFlag = 1;

                if(currentState == SuperSpecialState) {
                    clearFlag = 1;
                    char nextChar = getc(yyin);
                    if(ch == '!' && nextChar == '=') {
                        yytext[yylen] = nextChar;
                        yylen++;
                        return TOK_NOTEQ;
                    } else if(ch == '=' && nextChar == '=') {
                        yytext[yylen] = nextChar;
                        yylen++;
                        return TOK_RELEQ;
                    } else if(ch == '>' && nextChar == '=') {
                        yytext[yylen] = nextChar;
                        yylen++;
                        return TOK_GREQ;
                    } else if(ch == '<' && nextChar == '=') {
                        yytext[yylen] = nextChar;
                        yylen++;
                        return TOK_LSEQ;
                    } else if(ch == '>' && nextChar == '>') {
                        yytext[yylen] = nextChar;
                        yylen++;
                        return TOK_BITRSHFT;
                    } else if(ch == '<' && nextChar == '<') {
                        yytext[yylen] = nextChar;
                        yylen++;
                        return TOK_BITLSHFT;
                    } else if(ch == '+' && nextChar == '+') {
                        yytext[yylen] = nextChar;
                        yylen++;
                        return TOK_INC;
                    } else if(ch == '-' && nextChar == '-') {
                        yytext[yylen] = nextChar;
                        yylen++;
                        return TOK_DEC;
                    } else {
                        fseek(yyin, -1, SEEK_CUR);
                        return ch;
                    }
                }
            } else {
            if(ch == -1)
                return ch;
            return yylex();
        }
            return ch;
        } 
    }
    //return ch;
}
