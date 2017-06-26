#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>

/**
  main(){
  if a+9<=s {
       if e>0 {
           d=1.32e-7;
       } else {
           do{
               y=0;
           }while r==3;
       };
   } else {
       if d==12.3 {
           do{
               y=0;
           }while r==3;
       };
   };
   }#
 */
/*
 * syn为单词种别码，token为存放单词自身字符串，num为整型常数,rwtab为关键字组
 */
char arr[2000],token[20];
char ch;
int syn,str_index,n;
double num;
int token_index = 0;
char *rwtab[9] = {"main","int","float","double","char","if","else","do","while"};

std::string expr();
std::string expr2(std::string);
std::string term();
std::string term2(std::string);
std::string factor();
void match();
std::string rela();
std::string cond();
void sta();
void stas();
void stas2();
void stablock();
void prog();
void loop();
void condsta();
void condsta2(std::string);
void setsta();

int scaner();

int main() {
    str_index = 0;
    printf("\n please input string \n");
    //将所有字符串放入prog这个数组中
    do{
        ch = getchar();
        arr[str_index++] = ch;
    }while(ch!='#');
    str_index = 0;
    /*
    do
    {  scaner();
        switch(syn) {
            case 20: printf("(%2d,%+10.9e)\n", syn, num); break;    //输出数字
            case -1: printf("input error\n"); break;        //输入语法错误
            default: printf("(%2d,%8s)\n", syn, token);     //输出对应的文法
        }
    } while (syn!=0);
     */
    printf("\n");
    scaner();
    prog();
    printf("success");
}

// ----------- 语法分析器部分开始 --------------
std::string new_tmp() {
    static int i = 0;
    return "t" + std::to_string(++i);
}

std::string new_label(){
    static int i = 0;
    return "l" + std::to_string(++i);
}

void match(int flag) {
    if(syn != flag) throw std::runtime_error("Syntax error");
    scaner();
}

// E → TE’
std::string expr() {
    std::string t = term();
    std::string tmp = expr2(t);
    return tmp;
}

// E’ → +TE’|-TE’|ε
std::string expr2(std::string t) {
    std::string t2, tmp;
    switch (syn) {
        case 22:
            match(22);
            t2 = term();
            tmp = new_tmp();
            std::cout << tmp << " = " << t << " + " << t2 << std::endl;
            expr2(tmp);
            return tmp;
        case 23:
            match(23);
            t2 = term();
            tmp = new_tmp();
            std::cout << tmp << " = " << t << " - " << t2 << std::endl;
            expr2(tmp);
            return tmp;
        default:
            return t;
    }
}

// T → FT’
std::string term() {
    std::string f = factor();
    std::string tmp = term2(f);
    return tmp;
}

// T’ → *FT’|/FT’|ε
std::string term2(std::string f) {
    std::string f2, tmp;
    switch (syn) {
        case 24:
            match(24);
            f2 = factor();
            tmp = new_tmp();
            std::cout << tmp << " = " << f << " * " << f2 << std::endl;
            term2(tmp);
            return tmp;
        case 25:
            match(25);
            f2 = factor();
            tmp = new_tmp();
            std::cout << tmp << " = " << f << " / " << f2 << std::endl;
            term2(tmp);
            return tmp;
        default:
            return f;
    }
}

// F → (E)|ID|num
std::string factor() {
    std::string token2;
    switch (syn) {
        case 26:
            match(26);
            expr();
            match(27);
            break;
        case 10:
            token2 = token;
            scaner();
            return token2;
            //break;
        case 20:
            char buffer[30];
            sprintf(buffer, "%+10.9e", num);
            //token2 = std::to_string(buffer);
            scaner();
            //std::string str(buffer);
            return buffer;
        default:
            throw std::runtime_error("Syntax error");
    }
}

// <关系运算符> ::= <|<=|>|>=|==|!=
std::string rela() {
    switch (syn) {
        case 34:
            match(34);
            return " < ";
        case 35:
            match(35);
            return " <= ";
        case 32:
            match(32);
            return " > ";
        case 33:
            match(33);
            return " >= ";
        case 36:
            match(36);
            return " == ";
        case 37:
            match(37);
            return " != ";
        default:
            throw std::runtime_error("Syntax error");
    }
}


// <条件>cond::=<表达式><关系运算符><表达式>
std::string cond() {
    std::string expr1 = expr();
    std::string expr2 = rela();
    std::string expr3 = expr();
    //std::cout << expr1 << expr2 << expr3 << std::endl;
    std::string str = expr1 + expr2 +expr3;
    return str;
}

// <语句>::=<赋值语句>|<条件语句>|<循环语句>
void sta() {
    switch (syn) {
        case 10:
            setsta();
            break;
        case 6:
            condsta();
            break;
        case 8:
            loop();
            break;
        default:
            throw std::runtime_error("Syntax error");
    }
}

//<语句串>::=<语句>;{<语句>};
void stas() {
    sta();
    match(31);
    stas2();
}

void stas2() {
    switch (syn) {
        case 10:
        case 6:
        case 8:
            sta();
            match(31);
            stas2();
    }
}

//<语句块> ::= ‘{‘<语句串>’}’
void stablock() {
    match(28);
    stas();
    match(29);
}

//<程序> ::= main()<语句块>
void prog() {
    match(1);
    match(26);
    match(27);
    stablock();
}

// <循环语句>::=do <语句块>while <条件>
void loop() {
    match(8);
    std::string label = new_label();
    std::cout << label << ':' << std::endl;
    stablock();
    match(9);
    std::string cond1 = cond();
    std::cout << "ifTrue " << cond1 << " goto " << label << std::endl;
}

//<条件语句>::=if<条件><语句块>[else <语句块>]
void condsta() {
    match(6);
    std::string label = new_label();
    std::string cond1 = cond();
    std::cout << "ifFalse " << cond1 << " goto " << label << std::endl;
    stablock();
    condsta2(label);
}

void condsta2(std::string inh) {
    switch (syn) {
        case 7: {
            match(7);
            std::string label2 = new_label();
            std::cout << "goto " << label2 << std::endl;
            std::cout << inh << ":" << std::endl;
            stablock();
            std::cout << label2 << ":" << std::endl;
            break;
        }
        default:
            std::cout << inh << ":" << std::endl;
    }
}

//<赋值语句>::=ID=<表达式>
void setsta() {
    std::string id = token;
    match(10);
    match(21);
    std::string expr1 = expr();
    std::cout << id << '=' << expr1 << std::endl;
}

// ----------- 语法分析器部分结束 --------------

bool isCha(char ch){
    if(ch >= 'a' && ch <= 'z' ||  ch >= 'A' && ch<= 'Z'){
        return true;
    }
    else return false;
}

bool isNum(char ch){
    if(ch >= '0' && ch <= '9'){
        return true;
    }
    else return false;
}

// 数字计算的函数
double cal(){
    num = 0;
    do {
        num = num * 10	+ (ch - '0');
        double x = num;
        ch=arr[str_index];
        str_index++;
    } while (isNum(ch));

    if(ch == '.') {
        int cx = 1; //cx为小数点阶乘
        double num2 = 0;
        ch=arr[str_index];
        str_index++;
        if(!isNum(ch)) {
            syn = -1;
            return 0;
        }
        do {
            num2 = num2 * 10 + (ch - '0');
            ch=arr[str_index];
            str_index++;
            cx *= 10;
        } while (isNum(ch));
        num2 /= cx;
        num = num2 + num;
    }

    if(ch == 'e') {
        ch=arr[str_index];
        str_index++;
        int mult = 1;
        int num3 = 0;
        if(ch == '-') {
            mult = -1;
            ch=arr[str_index];
            str_index++;
            if(!isNum(ch)) {
                //倒退一格
                ch=arr[str_index];
                str_index--;
                syn = -1;
                return 0;
            }
        } else if (ch == '+') {
            ch=arr[str_index];
            str_index++;
            if(!isNum(ch)) {
                //倒退一格
                ch=arr[str_index];
                str_index--;
                syn = -1;
                return 0;
            }
        } else {
            if(!isNum(ch)) {
                syn = -1;
                return 0;
            }
        }
        do {
            num3 = num3 * 10 + (ch - '0');
            ch=arr[str_index];
            str_index++;
        } while (isNum(ch));
        num3 *= mult;
        num = num * pow(10, num3);
    }
    str_index = str_index-1;
    syn = 20;
    double a = num;
    return num;
}

int scaner(){
    for (n = 0; n<8; n++) token[n] = NULL;
    //ch = arr[str_index++];
    //ch是空格或者注释

    while (1){
        ch = arr[str_index];
        str_index = str_index + 1;
        if(ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
            continue;
        } else if (ch == '/') {
            ch = arr[str_index];
            if(ch == '/') {
                str_index++;
                while(1) {
                    ch = arr[str_index];
                    str_index = str_index + 1;
                    if(ch == '\n') {
                        break;
                    }
                }
            } else if (ch == '*') {
                str_index++;
                while(1) {
                    ch = arr[str_index];
                    str_index = str_index + 1;
                    if(ch == '*') {
                        ch = arr[str_index];
                        if(ch == '/') {
                            str_index++;
                            break;
                        }
                    }
                }
            } else {
                ch = '/';
                break;
            }
        } else {
            break;
        }
    }
    //ch是数字
    if(ch >='0' && ch<='9'){
        num = cal();
        return 0;
        //syn = 20;
    }

    //ch是字母
    if( isCha(ch) ) {
        token_index = 0;
        while (isNum(ch) || isCha(ch))
        {
            token[token_index] = ch;
            ch = arr[str_index];
            token_index = token_index + 1;
            str_index = str_index + 1;
        }
        token[token_index++] = '\0';   str_index--;   syn = 10;
        //判断该字符串是否为关键字
        int judge;
        for (int n = 0; n < 9; n++)
        {
            judge = strcmp(token, rwtab[n]);
            if (judge == 0)
            {
                syn = n + 1;//表示是关键字
                break;
            }
        }
    }

    else {
        switch(ch) {
            case '=':
                token_index = 0;
                token[token_index++] = ch;
                ch = arr[str_index++];
                if (ch == '=') {
                    syn = 36;
                    token[token_index++] = ch;
                }
                else {
                    syn = 21;
                    str_index--;
                }
                break;
            case '+':
                if(isNum(arr[str_index]) && syn != 10 && syn != 20 ) {
                    ch = arr[str_index];
                    str_index++;
                    num = cal();
                    return 0;
                } else {
                    // 加号
                    syn = 22;
                    token[0] = ch;
                }
                break;
            case '-':
                //前面不是字符串也不是数字，后面是数字，则是负号
                if(isNum(arr[str_index]) && syn != 10 && syn != 20 ) {
                    // 正号
                    ch = arr[str_index];
                    str_index++;
                    num = - cal();
                    return 0;
                } else {
                    // 加号
                    syn = 23;
                    token[0] = ch;
                }
                break;
            case '*':
                syn = 24;
                token[0] = ch;
                break;
            case '/':
                syn = 25;
                token[0] = ch;
                break;
            case '(':
                syn = 26;
                token[0] = ch;
                break;
            case ')':
                syn = 27;
                token[0] = ch;
                break;
            case '{':
                syn = 28;
                token[0] = ch;
                break;
            case '}':
                syn = 29;
                token[0] = ch;
                break;
            case ',':
                syn = 30;
                token[0] = ch;
                break;
            case ';':
                syn = 31;
                token[0] = ch;
                break;
            case '>':
                token_index = 0;
                token[token_index++] = ch;
                ch = arr[str_index++];
                if (ch == '=') {
                    syn = 33;
                    token[token_index++] = ch;
                }

                else {
                    syn = 32;
                    str_index--;
                }
                break;
            case '<':
                token_index = 0;
                token[token_index++] = ch;
                ch = arr[str_index++];
                if (ch == '=') {
                    syn = 35;
                    token[token_index++] = ch;
                }
                else {
                    syn = 34;
                    str_index--;
                }
                break;
            case '!':
                token_index = 0;
                token[token_index++] = ch;
                ch = arr[str_index++];
                if (ch == '=') {
                    syn = 37;
                    token[token_index++] = ch;
                }
                break;
            case '#':
                syn = 0;
                token[0] = ch;
                break;
            default:
                syn = -1;
        }
    }
}

