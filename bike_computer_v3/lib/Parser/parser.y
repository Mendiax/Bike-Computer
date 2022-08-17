%{
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdarg>
#include <cinttypes>
#include <string>


static void yyerror(const char *msg);
static int yyparse(void);
int yylex(void);

extern FILE *yyin;

static Memory vars;
static LabelLinker labels;
static Assembler asem;
static bool grammarError = false;

%}

%code requires
{
    #include <cstdint>
    #include <string>

    struct DefaultToken
    {
        uint64_t line;
    };

    struct NumberToken
    {
        int64_t val;
        uint64_t line;
    };

    struct VariableToken
    {
        uint64_t line;
        std::string* str;
    };

    void checkInitalization(Value *val, uint64_t line);
}

%union
{
    DefaultToken dtoken;
    NumberToken number;
    VariableToken variable;
    Value* valuep;
    Constant* rvaluep;
    Variable* variablep;
    Condition* cond;
}

%token	LEX_DECLARE_VAR LEX_BEGIN LEX_END
        LEX_ASSIGN
        LEX_IF LEX_THEN LEX_ELSE LEX_ENDIF
        LEX_FOR LEX_FROM LEX_TO LEX_DOWNTO LEX_ENDFOR
        LEX_WHILE LEX_DO LEX_ENDWHILE
        LEX_REPEAT LEX_UNTIL
        LEX_READ LEX_WRITE
        LEX_ADD LEX_SUB LEX_DIV LEX_MOD LEX_MUL
        LEX_EQ LEX_NEQ LEX_LE LEX_GE LEX_LEQ LEX_GEQ
        LEX_VARIABLE LEX_NUM
        LEX_L_BRACKET LEX_R_BRACKET LEX_ARRAY_RANGE
        LEX_COMMA LEX_SEMICOLON
        LEX_ERROR


%type <dtoken>	LEX_DECLARE_VAR LEX_BEGIN LEX_END
                LEX_ASSIGN
                LEX_IF LEX_THEN LEX_ELSE LEX_ENDIF
                LEX_FOR LEX_FROM LEX_TO LEX_DOWNTO LEX_ENDFOR
                LEX_WHILE LEX_DO LEX_ENDWHILE
                LEX_REPEAT LEX_UNTIL
                LEX_READ LEX_WRITE
                LEX_ADD LEX_SUB LEX_DIV LEX_MOD LEX_MUL
                LEX_EQ LEX_NEQ LEX_LE LEX_GE LEX_LEQ LEX_GEQ
                LEX_L_BRACKET LEX_R_BRACKET LEX_ARRAY_RANGE
                LEX_COMMA LEX_SEMICOLON
                LEX_ERROR

%type <variable>  LEX_VARIABLE
%type <number> LEX_NUM
%type <cond> cond
%type <valuep> value
%type <rvaluep> num
%type <variablep> identifier


%%

program:
    LEX_DECLARE_VAR declarations LEX_BEGIN commands LEX_END{}
    | LEX_BEGIN commands LEX_END{}
;

declarations:
    declarations LEX_COMMA LEX_VARIABLE
    {
        vars.initVariable(*$3.str, $3.line);
        delete $3.str;

    }
    | declarations LEX_COMMA LEX_VARIABLE LEX_L_BRACKET LEX_NUM LEX_ARRAY_RANGE LEX_NUM LEX_R_BRACKET
    {
        vars.initArray(*$3.str,$5.val,$7.val,$3.line);
        delete $3.str;
    }
    | LEX_VARIABLE
    {
        vars.initVariable(*$1.str, $1.line);
        delete $1.str;

    }
    | LEX_VARIABLE LEX_L_BRACKET LEX_NUM LEX_ARRAY_RANGE LEX_NUM LEX_R_BRACKET
    {
        vars.initArray(*$1.str,$3.val,$5.val, $1.line);
        //std::cerr << "new array " << *$1.str << " address "<< vars.getVariableAddress(*$1.str) << std::endl;
        delete $1.str;

    }
;

commands:
    commands command{}
    | command{}
;

command:
    identifier LEX_ASSIGN expr LEX_SEMICOLON
    {
        /*
        value from expresssion is in a register
        */
        Variable* lval = $1;

        //assertMutuable(lval, $2.line);
        if (!lval->mutuable)
        {
            std::cerr << "Błąd w linii " << $2.line << ":Proba nadpisania zmiennej niemodyfikowalnej " << lval->getName() << std::endl;
            exit(EXIT_FAILURE);
        }
        //std::cout << " asignning val " << lval->name << std::endl;
        Assembler::Register& reg = asem.getFreeReg();
        reg.lock();
        asem.SWAP(reg);
        Assembler::Register& reg2 = asem.getFreeReg(lval->address);
        reg2.lock();
        asem.eLOADADDR(reg2, lval);
        asem.SWAP(reg);
        asem.STORE(reg2);
        reg.unlock();
        reg2.unlock();

        lval->isInitialized = true;
    }
    | LEX_READ identifier LEX_SEMICOLON
    {
        Variable* lval = dynamic_cast<Variable*>($2);
        //assertMutuable(lval, $1.line);
        if (!lval->mutuable)
        {
            std::cerr << "Błąd w linii " << $1.line << ":Proba nadpisania zmiennej " << lval->getName() << " typu const" << std::endl;
            exit(EXIT_FAILURE);
        }
        Assembler::Register& reg = asem.getFreeReg(lval->address);
        reg.lock();
        asem.eLOADADDR(reg, lval);
        asem.GET(); //readed value is in a reg
        asem.STORE(reg);
        reg.unlock();
        //Variable* accessVariable = dynamic_cast<Variable*>(vars.getVariableByName(lval->getName()));
        lval->isInitialized = true;
    }
    | LEX_WRITE value LEX_SEMICOLON
    {
        checkInitalization($2, $1.line);
        asem.eLOAD($2);
        asem.PUT();
    }
    | for_start for_end{}
    | while_cond while_end{}
    | repeat_start repeat_cond{}
    | if_cond if_else if_else_end{}
    | if_cond if_end{}
;

for_start:
    LEX_FOR LEX_VARIABLE LEX_FROM value LEX_TO value LEX_DO
    {
        checkInitalization($4, $1.line);
        checkInitalization($6, $1.line);

        vars.initVariableFor(*$2.str);

        //init for variable
        Variable* iterator = vars.getVariableByName(*$2.str, $2.line);
        iterator->isInitialized = true;

        Assembler::Register& reg2 = asem.getFreeReg();
        reg2.lock();
        asem.eLOADADDR(reg2, iterator);
        asem.eLOAD($4);
        asem.DEC(asem.regs[0]);
        asem.STORE(reg2);
        reg2.unlock();


        Variable* endValue = new Variable("endloop_value");
        endValue->address = vars.malloc(1);
        vars.push(iterator);
        Assembler::Register& reg4 = asem.getFreeReg();
        reg4.lock();
        asem.eLOADADDR(reg4, endValue);
        asem.eLOAD($6);
        asem.STORE(reg4);
        reg4.unlock();


        auto label = labels.generateLabel("FOR_LOOP_START");
        labels.pushOnStack(label);
        asem.eLABEL(label);


        //inc iterator
        Assembler::Register& reg5 = asem.getFreeReg();
        reg5.lock();
        asem.eLOADADDR(reg5, iterator);
        asem.LOAD(reg5);
        asem.INC(asem.regs[0]);
        asem.STORE(reg5);
        asem.SWAP(reg5);


        auto label_end = labels.generateLabel("FOR_LOOP_END");
        labels.pushOnStack(label_end);

        Assembler::Register &reg6 = asem.getFreeReg();
        reg6.lock();
        asem.eLOAD(endValue);
        asem.SUB(reg5);
        reg6.unlock();
        reg5.unlock();
        asem.eJNEG(label_end);


        delete endValue;
        delete $2.str;
    }
    | LEX_FOR LEX_VARIABLE LEX_FROM value LEX_DOWNTO value LEX_DO
    {
        checkInitalization($4, $1.line);
        checkInitalization($6, $1.line);

        vars.initVariableFor(*$2.str); // change it to for type later

        //init for variable
        Variable* iterator = vars.getVariableByName(*$2.str, $2.line);
        iterator->isInitialized = true;
        vars.push(iterator);
        Assembler::Register& reg = asem.getFreeReg();
        reg.lock();
        asem.eLOADADDR(reg, iterator);
        asem.eLOAD($4);
        asem.INC(asem.regs[0]);
        asem.STORE(reg);
        reg.unlock();

        Variable* endValue = new Variable("endloop_value");
        endValue->address = vars.malloc(1);
        Assembler::Register& reg2 = asem.getFreeReg();
        reg2.lock();
        asem.eLOADADDR(reg2, endValue);
        asem.eLOAD($6);
        asem.STORE(reg2);
        reg2.unlock();


        auto label = labels.generateLabel("FOR_LOOP_START");
        labels.pushOnStack(label);
        asem.eLABEL(label);


        //dec iterator
        Assembler::Register& reg4 = asem.getFreeReg();
        reg4.lock();
        asem.eLOADADDR(reg4, iterator);
        asem.LOAD(reg4);
        asem.DEC(asem.regs[0]);
        asem.STORE(reg4);
        asem.SWAP(reg4);


        auto label_end = labels.generateLabel("FOR_LOOP_END");
        labels.pushOnStack(label_end);

        asem.eLOAD(endValue);
        asem.SUB(reg4);
        reg4.unlock();
        asem.eJPOS(label_end);


        delete endValue;
        delete $2.str;
    }
;

for_end:
    commands LEX_ENDFOR
    {
        auto label_end = labels.popFromStack();
        auto label = labels.popFromStack();
        asem.eJUMP(label);
        asem.eLABEL(label_end);
        Variable* iterator = vars.pop();
        vars.deleteVariable(iterator->getName());
        delete iterator;
    }
;

while_cond:
    while_declare cond LEX_DO
    {
        auto label_end = labels.generateLabel("WHILE_LOOP_END");
        labels.pushOnStack(label_end);
        //nie jest optymalnie bo robie 2 jumpy a nie jeden
        $2->writeInstructions(asem, label_end);
        delete $2;
    }
;

while_declare:
    LEX_WHILE
    {
        auto label = labels.generateLabel("WHILE_LOOP_START");
        labels.pushOnStack(label);
        asem.eLABEL(label);
    }
;

while_end:
    commands LEX_ENDWHILE
    {
        auto label_end = labels.popFromStack();
        auto label = labels.popFromStack();
        asem.eJUMP(label);
        asem.eLABEL(label_end);
    }
;

repeat_start:
    LEX_REPEAT
    {
        auto label = labels.generateLabel("REPEAT_LOOP_START");
        labels.pushOnStack(label);
        asem.eLABEL(label);
    }
;

repeat_cond:
    commands LEX_UNTIL cond LEX_SEMICOLON
    {
        auto label = labels.popFromStack();
        $3->writeInstructions(asem, label);
        delete $3;
    }
;

if_cond:
    LEX_IF cond LEX_THEN
    {
        auto label = labels.generateLabel("if_false");
        labels.pushOnStack(label);

        $2->writeInstructions(asem, label);

        delete $2;
    }
;

if_end:
    commands LEX_ENDIF
    {
        asem.eLABEL(labels.popFromStack());
    }
;

if_else:
    commands LEX_ELSE
    {
        //create jump after else end
        auto label = labels.generateLabel("endif");
        asem.eJUMP(label);

        //insert else jump from stack
        asem.eLABEL(labels.popFromStack());
        labels.pushOnStack(label);
    }
;

if_else_end:
    commands LEX_ENDIF
    {
        //insert endif
        asem.eLABEL(labels.popFromStack());
    }
;


expr:
    value
    {
        checkInitalization($1, 0);//don't know line in this place
        auto val = $1;
        asem.eLOAD(val);
    }
    | value LEX_ADD value
    {
        checkInitalization($1, $2.line);
        checkInitalization($3, $2.line);
        if($1->getType() == Value::rvalue && $3->getType() == Value::rvalue &&\
             dynamic_cast<Constant*>($1)->getValue() < (1LL<<62) &&  dynamic_cast<Constant*>($3)->getValue() < (1LL<<62) &&\
             dynamic_cast<Constant*>($1)->getValue() > -(1LL<<62) && dynamic_cast<Constant*>($3)->getValue() > -(1LL<<62)){
            auto val1 = dynamic_cast<Constant*>($1);
            auto val2 = dynamic_cast<Constant*>($3);
            int64_t newVal = val1->getValue() + val2->getValue();
            asem.eMOV_sign(asem.regs[0], newVal);
        }
        else if($3->getType() == Value::rvalue && dynamic_cast<Constant*>($3)->getValue() == 1 ){
            asem.eLOAD($1);
            asem.INC(asem.regs[0]);
        }
        else if($3->getType() == Value::rvalue && dynamic_cast<Constant*>($3)->getValue() == -1 ){
            asem.eLOAD($1);
            asem.DEC(asem.regs[0]);
        }
        else
        {
            asem.eLOAD($3);
            Assembler::Register& reg = asem.getFreeReg();
            reg.lock();
            asem.SWAP(reg);
            asem.eLOAD($1);
            asem.ADD(reg);
            reg.unlock();
        }
    }
    | value LEX_SUB value
    {
        checkInitalization($1, $2.line);
        checkInitalization($3, $2.line);
        if($1->getType() == Value::rvalue && $3->getType() == Value::rvalue &&\
             dynamic_cast<Constant*>($1)->getValue() < (1LL<<62) && dynamic_cast<Constant*>($3)->getValue() < (1LL<<62) &&\
             dynamic_cast<Constant*>($1)->getValue() > -(1LL<<62) && dynamic_cast<Constant*>($3)->getValue() > -(1LL<<62)){
            auto val1 = dynamic_cast<Constant*>($1);
            auto val2 = dynamic_cast<Constant*>($3);
            int64_t newVal = val1->getValue() - val2->getValue();
            asem.eMOV_sign(asem.regs[0], newVal);
        }
        else if($3->getType() == Value::rvalue && dynamic_cast<Constant*>($3)->getValue() == 1 ){
            asem.eLOAD($1);
            asem.DEC(asem.regs[0]);
        }
        else if($3->getType() == Value::rvalue && dynamic_cast<Constant*>($3)->getValue() == -1 ){
            asem.eLOAD($1);
            asem.INC(asem.regs[0]);
        }
        else
        {
            asem.eLOAD($3);
            Assembler::Register& reg = asem.getFreeReg();
            reg.lock();
            asem.SWAP(reg);
            asem.eLOAD($1);
            asem.SUB(reg);
            reg.unlock();
        }
    }
    | value LEX_MUL value
    {
        checkInitalization($1, $2.line);
        checkInitalization($3, $2.line);
        //mnożenie z shiftem ???
        if ($1->getType() == Value::rvalue && $3->getType() == Value::rvalue &&\
             dynamic_cast<Constant*>($1)->getValue() < (1LL<<31) && dynamic_cast<Constant*>($3)->getValue() < (1LL<<31) &&\
             dynamic_cast<Constant*>($1)->getValue() > -(1LL<<31) && dynamic_cast<Constant*>($3)->getValue() > -(1LL<<31)){
            auto val1 = dynamic_cast<Constant*>($1);
            auto val2 = dynamic_cast<Constant*>($3);
            int64_t newVal = val1->getValue() * val2->getValue();
            //std::cout << "optim mul "  << val1->getValue() << " * " <<  val2->getValue() << " -> " << val1->getValue() * val2->getValue() << std::endl;
            asem.eMOV_sign(asem.regs[0], newVal);
        }else{
            asem.eLOAD($3);
            Assembler::Register& reg2 = asem.getFreeReg();
            reg2.lock();
            asem.SWAP(reg2);
            asem.eLOAD($1);
            Assembler::Register& reg = asem.getFreeReg();
            reg.lock();
            asem.SWAP(reg);
            asem.eMULT(reg,reg2);
            reg.unlock();
            reg2.unlock();
        }

        //delete $1;
        //delete $3;
    }
    | value LEX_DIV value
    {
        checkInitalization($1, $2.line);
        checkInitalization($3, $2.line);
        if ($1->getType() == Value::rvalue && $3->getType() == Value::rvalue && \
            dynamic_cast<Constant*>($1)->getValue() > INT64_MIN && \
            dynamic_cast<Constant*>($3)->getValue() > INT64_MIN && true){
            auto val1 = dynamic_cast<Constant*>($1);
            auto val2 = dynamic_cast<Constant*>($3);
            int64_t newVal;
            if( val2->getValue() != 0)
            {
                if (val1->getValue() >= 0 && val2->getValue() > 0)
                {
                    newVal = val1->getValue() / val2->getValue();
                }
                if (val1->getValue() >= 0 && val2->getValue() < 0)
                {
                    newVal = val1->getValue() / (val2->getValue() * -1);
                    if (val1->getValue() % (val2->getValue() * -1))
                        newVal++;
                    newVal *= -1LL;
                }
                if (val1->getValue() < 0 && val2->getValue() > 0)
                {
                    newVal = (val1->getValue() * -1) / val2->getValue();
                    if ((val1->getValue() * -1) % val2->getValue())
                        newVal++;
                    newVal *= -1LL;
                }
                if (val1->getValue() < 0 && val2->getValue() < 0)
                {
                    newVal = (val1->getValue() * -1) / (val2->getValue() * -1);
                }
            }
            else
                newVal = 0;
            asem.eMOV_sign(asem.regs[0], newVal);
        }
        else if($3->getType() == Value::rvalue && dynamic_cast<Constant*>($3)->getValue() == 2)
        {
            Assembler::Register& reg2 = asem.getFreeReg();//-1 ale cos nie pomaga
            reg2.lock();
            asem.RESET(reg2);
            asem.DEC(reg2);
            //asem.eMOV_sign(reg2, -1);
            asem.eLOAD($1);
            asem.SHIFT(reg2);
            reg2.unlock();
        }
        else if($3->getType() == Value::rvalue && dynamic_cast<Constant*>($3)->getValue() == 1)
        {
            asem.eLOAD($1);
        }
        else if($3->getType() == Value::rvalue && dynamic_cast<Constant*>($3)->getValue() == -1)
        {
            Assembler::Register& reg2 = asem.getFreeReg();
            reg2.lock();
            asem.eLOAD($1);
            asem.SWAP(reg2);
            asem.RESET(asem.regs[0]);
            asem.SUB(reg2);
            reg2.unlock();
        }
        else{
            const std::string divPN = labels.generateLabel("yyDIV_p_n");
            const std::string divNP = labels.generateLabel("yyDIV_n_p");
            const std::string divNN = labels.generateLabel("yyDIV_n_n");
            const std::string divPP = labels.generateLabel("yyDIV_p_p");
            const std::string divEnd = labels.generateLabel("yyDIV_end");

            const std::string div_p1 = labels.generateLabel("yyDIV_p1");
            const std::string div_n1 = labels.generateLabel("yyDIV_n1");

            //mod 2 we can do with shift
            asem.eLOAD($3);
            asem.eJZERO(divEnd);
            //maybe check for 0 in second

            Assembler::Register& reg2 = asem.getFreeReg();
            reg2.lock();
            asem.SWAP(reg2);
            asem.eLOAD($1);
            Assembler::Register& reg = asem.getFreeReg();
            reg.lock();
            asem.SWAP(reg);


            // check reg2 for 1 and -1
            asem.SWAP(reg2);

            //check for 1
            asem.DEC(asem.regs[0]);
            asem.eJZERO(div_p1);
            asem.INC(asem.regs[0]);

            //check for -1
            asem.INC(asem.regs[0]);
            asem.eJZERO(div_n1);
            asem.DEC(asem.regs[0]);


            asem.eJNEG(divPN);
            asem.SWAP(reg2);

            asem.SWAP(reg);
            asem.eJNEG(divNP);
            asem.SWAP(reg);

            // _____________ P P _______________
            asem.eDIV(reg,reg2);
            asem.eJUMP(divEnd);

            // _____________ N P _______________
            asem.eLABEL(divNP);
            asem.SWAP(reg);

            // start reg a is free
            asem.RESET(asem.regs[0]);
            //asem.ADD(reg2);
            asem.SUB(reg);
            asem.SWAP(reg);

            const std::string div_negIncNP = labels.generateLabel("yyDIV_negIncNP");
            asem.eDIV(reg,reg2);
            asem.SWAP(reg);
            asem.eJZERO(div_negIncNP);
            asem.INC(reg);
            asem.eLABEL(div_negIncNP);
            asem.RESET(asem.regs[0]);
            asem.SUB(reg);

            asem.eJUMP(divEnd);

            // _____________ P N _______________
            asem.eLABEL(divPN);
            asem.SWAP(reg2);

            asem.SWAP(reg);
            asem.eJNEG(divNN);
            asem.SWAP(reg);


            //reg -= reg2
            //asem.SWAP(reg);
            //asem.SUB(reg2);
            //asem.INC(asem.regs[0]);
            //asem.SWAP(reg);

            //reg2 = |reg2|
            asem.RESET(asem.regs[0]);
            asem.SUB(reg2);
            asem.SWAP(reg2);

            const std::string div_negInc = labels.generateLabel("yyDIV_negInc");
            asem.eDIV(reg,reg2);
            asem.SWAP(reg);
            asem.eJZERO(div_negInc);
            asem.INC(reg);
            asem.eLABEL(div_negInc);
            asem.RESET(asem.regs[0]);
            asem.SUB(reg);

            asem.eJUMP(divEnd);
            //asem.eDIV_p_n(reg,reg2);

            // _____________ N N _______________
            asem.eLABEL(divNN);
            asem.SWAP(reg);

            asem.RESET(asem.regs[0]);
            asem.SUB(reg2);
            asem.SWAP(reg2);

            asem.RESET(asem.regs[0]);
            asem.SUB(reg);
            asem.SWAP(reg);

            asem.eDIV(reg,reg2);
            asem.eJUMP(divEnd);

            // _____________ * 1 _______________
            asem.eLABEL(div_p1);
            asem.SWAP(reg);

            asem.eJUMP(divEnd);

            // _____________ * -1 ______________
            asem.eLABEL(div_n1);
            asem.RESET(asem.regs[0]);
            asem.SUB(reg);

            asem.eLABEL(divEnd);

            reg.unlock();
            reg2.unlock();
        }

    }
    | value LEX_MOD value
    {
        checkInitalization($1, $2.line);
        checkInitalization($3, $2.line);
        if($1->getType() == Value::rvalue && $3->getType() == Value::rvalue && false){
            auto val1 = dynamic_cast<Constant*>($1);
            auto val2 = dynamic_cast<Constant*>($3);
            int64_t newVal;
            if( val2->getValue() != 0)
                newVal = val1->getValue() % val2->getValue();
            else
                newVal = 0;
            asem.eMOV_sign(asem.regs[0], newVal);
        }else{
            //mod 2 we can do with shift
            asem.eLOAD($3);
            Assembler::Register& reg2 = asem.getFreeReg();
            reg2.lock();
            asem.SWAP(reg2);
            asem.eLOAD($1);
            Assembler::Register& reg = asem.getFreeReg();
            reg.lock();
            asem.SWAP(reg);

            const std::string divPN = labels.generateLabel("yyMOD_p_n");
            const std::string divNP = labels.generateLabel("yyMOD_n_p");
            const std::string divNN = labels.generateLabel("yyMOD_n_n");
            const std::string divPP = labels.generateLabel("yyMOD_p_p");
            const std::string divEnd = labels.generateLabel("yyMOD_end");

            const std::string div_p1 = labels.generateLabel("yyMOD_p1");
            const std::string div_n1 = labels.generateLabel("yyMOD_n1");

            asem.SWAP(reg2);

            //check for 1
            asem.DEC(asem.regs[0]);
            asem.eJZERO(div_p1);
            asem.INC(asem.regs[0]);

            //check for -1
            asem.INC(asem.regs[0]);
            asem.eJZERO(div_n1);
            asem.DEC(asem.regs[0]);

            //asem.SWAP(reg2);
            asem.eJNEG(divPN);
            asem.SWAP(reg2);
            
            asem.SWAP(reg);
            asem.eJNEG(divNP);
            asem.SWAP(reg);

            // _____________ P P _______________
            asem.eMOD(reg,reg2);
            asem.eJUMP(divEnd);

            // _____________ N P _______________
            asem.eLABEL(divNP);
            asem.SWAP(reg);

            //reg = |reg|
            asem.RESET(asem.regs[0]);
            asem.SUB(reg);
            asem.SWAP(reg);

            //reg2 = |reg2|

            {
                // reg3 = reg2
                Assembler::Register& reg3 = asem.getFreeReg();
                reg3.lock();
                asem.eMOV(reg3,reg2);

                // reg mod
                const std::string div_r0 = labels.generateLabel("yyMOD_r0");
                asem.eMOD(reg,reg2);
                asem.eJZERO(div_r0);
                asem.SUB(reg3);
                asem.eLABEL(div_r0);
                reg3.unlock();
            }
            
            asem.SWAP(reg);
            asem.RESET(asem.regs[0]);
            asem.SUB(reg);

            asem.eJUMP(divEnd);

            // _____________ * N _______________
            asem.eLABEL(divPN);
            asem.SWAP(reg2);

            asem.SWAP(reg);
            asem.eJNEG(divNN);
            asem.SWAP(reg);

            // _____________ P N _______________

            //reg2 = |reg2|
            asem.RESET(asem.regs[0]);
            asem.SUB(reg2);
            asem.SWAP(reg2);

            {
                // reg3 = reg2
                Assembler::Register& reg3 = asem.getFreeReg();
                reg3.lock();
                asem.eMOV(reg3,reg2);

                // reg mod
                const std::string div_r0 = labels.generateLabel("yyMOD_r0");
                asem.eMOD(reg,reg2);
                asem.eJZERO(div_r0);
                asem.SUB(reg3);
                asem.eLABEL(div_r0);
                reg3.unlock();
            }

            asem.eJUMP(divEnd);

            // _____________ N N _______________
            asem.eLABEL(divNN);
            asem.SWAP(reg);

            asem.RESET(asem.regs[0]);
            asem.SUB(reg2);
            asem.SWAP(reg2);

            asem.RESET(asem.regs[0]);
            asem.SUB(reg);
            asem.SWAP(reg);

            asem.eMOD(reg,reg2);

            asem.SWAP(reg);
            asem.RESET(asem.regs[0]);
            asem.SUB(reg);

            asem.eJUMP(divEnd);

            // _____________ * 1 _______________
            asem.eLABEL(div_p1);
            asem.RESET(asem.regs[0]);

            asem.eJUMP(divEnd);

            // _____________ * -1 ______________
            asem.eLABEL(div_n1);
            asem.RESET(asem.regs[0]);


            asem.eLABEL(divEnd);

            reg.unlock();
            reg2.unlock();
        }
    }
;

cond:
    value LEX_EQ value
    {
        checkInitalization($1, $2.line);
        checkInitalization($3, $2.line);
        $$ = new Condition(Condition::type_t::eq, $1, $3);
    }
    | value LEX_NEQ value
    {
        checkInitalization($1, $2.line);
        checkInitalization($3, $2.line);
        $$ = new Condition(Condition::type_t::neq, $1, $3);
    }
    | value LEX_LE value
    {
        checkInitalization($1, $2.line);
        checkInitalization($3, $2.line);
        $$ = new Condition(Condition::type_t::le, $1, $3);
    }
    | value LEX_LEQ value
    {
        checkInitalization($1, $2.line);
        checkInitalization($3, $2.line);
        $$ = new Condition(Condition::type_t::leq, $1, $3);
    }
    | value LEX_GE value
    {
        checkInitalization($1, $2.line);
        checkInitalization($3, $2.line);
        $$ = new Condition(Condition::type_t::ge, $1, $3);
    }
    | value LEX_GEQ value
    {
        checkInitalization($1, $2.line);
        checkInitalization($3, $2.line);
        $$ = new Condition(Condition::type_t::geq, $1, $3);
    }
;

value:
    num
    {
        $$ = dynamic_cast<Value*>($1);
    }
    | identifier
    {
        $$ = dynamic_cast<Value*>($1);
    }
;

num:
    LEX_NUM
    {
        $$ = new Constant($1.val);
        //asem.eMOV(asem.regs[0], $1.val);
    }
;

identifier:
    LEX_VARIABLE
    {
        Variable* var = dynamic_cast<Variable*>(vars.getVariableByName(*($1.str), $1.line));
        if (var->getType() == Value::lvalueArray)
        {
            std::cerr << "Błąd w linii " << $1.line << ": Użycie tablicy " << *($1.str) << " bez indeksu" << std::endl;
            exit(EXIT_FAILURE);
        }
        //$$ = var;
        $$ = dynamic_cast<Variable*>(vars.getVariableByName(*($1.str), $1.line));
        delete $1.str;
    }
    | LEX_VARIABLE LEX_L_BRACKET LEX_VARIABLE LEX_R_BRACKET
    {
        VariableArray* var = dynamic_cast<VariableArray*>(vars.getVariableByName(*($1.str), $1.line));
        if (var->getType() == Value::lvalue)
        {
            std::cerr << "Błąd w linii " << $1.line << ": Użycie zmiennej " << *($1.str) << " jako tablicy" << std::endl;
            exit(EXIT_FAILURE);
        }

        Variable* accessVariable = dynamic_cast<Variable*>(vars.getVariableByName(*($3.str), $3.line));
        if (accessVariable->getType() == Value::lvalueArray)
        {
            std::cerr << "Błąd w linii " << $3.line << ": Użycie tablicy " << *($3.str) << " jako index dostępu" << std::endl;
            exit(EXIT_FAILURE);
        }


        //checkInitalization(accessVariable, $3.line);
        // if (!vars.isInit(*($3.str)))
        // {
        //     std::cerr << "BLAD w linii " << $3.line << ": Użycie niezadeklarowanej zmiennej " << std::endl;
        //     exit(EXIT_FAILURE);
        // }

        VariableArray* val = new VariableArray(*var);
        val->setAccessVal(accessVariable);

        $$ = val;

        delete $1.str;
        delete $3.str;
    }
    | LEX_VARIABLE LEX_L_BRACKET num LEX_R_BRACKET
    {
        VariableArray* var = (VariableArray*)vars.getVariableByName(*($1.str), $1.line);
        if (var->getType() == Value::lvalue)
        {
            std::cerr << "Błąd w linii " << $1.line << ": Użycie zmiennej " << *($1.str) << " jako tablicy" << std::endl;
            exit(EXIT_FAILURE);
        }

        var->setAccessVal($3);
        VariableArray* val = new VariableArray(*var);
        val->setAccessVal($3);

        $$ = val;

        delete $1.str;
    }
;

%%

static void yyerror(const char* msg)
{
    std::cerr << "Błąd w linii " <<  yylval.dtoken.line << ":" << msg << " błąd składni " << std::endl;
    grammarError = true;
}

void checkInitalization(Value *val, uint64_t line)
{
    if (val->getType() == Value::lvalue)
    {
        Variable *lval = vars.getVariableByName(dynamic_cast<Variable *>(val)->getName(), line);
        if (!lval->isInitialized)
        {
            std::cerr << "Błąd w linii " << line << ": Uzycie zmiennej " << lval->getName() << " bez inicjalizacji" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, const char* argv[])
{
    if (argc < 3)
    {
        std :: cout << "Za mało argumentów. Poprawne użycie:\n" << "./kompilator kod kod_assemblera" << std :: endl;
        return 1;
    }

    asem.labels = &labels;

    if (!(yyin = fopen (argv[1], "r")))
    {
      std::cerr << "Nie można otworzyć pliku: " << argv[1] << '\n';
      exit(EXIT_FAILURE);
    }
    yyparse();
    fclose(yyin);

    if(!grammarError){
        labels.linkLabels(asem.code);

        std::ofstream file(argv[2]);
        asem.HALT();
    
        for (auto &&instr : asem.code)
        {
            if (instr[0] != '.')
                file << instr << std::flush;
        }
    }

    return 0;
}
