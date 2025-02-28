
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#define MAX 100

/*
S_START: Trạng thái bắt đầu
S_OPERAND: Trạng thái nhận toán hạng
S_OPERATOR: Trạng thái nhận toán tử
S_OPEN: Trạng thái nhận dấu mở ngoặc
S_CLOSE: Trạng thái nhận dấu đóng ngoặc
S_ERROR: Trạng thái lỗi
S_END: Trạng thái kết thúc
*/
typedef enum {S_START, S_OPERAND, S_OPERATOR, S_OPEN, S_CLOSE, S_ERROR, S_END} state_t;

typedef enum {
    OPERAND,
    OPERATOR,
    VARIABLE
} TokenType;

typedef struct {
    TokenType type;
    union {
        float operand;
        char operator;
        float variable;
    } value;
} Token;

int isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}

int precedence(char op) {
    switch(op) {
        case '+':
        case '-': return 1;
        case '*':
        case '/': return 2;
        case '^': return 3;
        default: return 0;
    }
}

Token *infixToPostfix(char* myFunction){
    state_t current_state = S_START;
    Token *output = (Token *)malloc(MAX * sizeof(Token));
    int outputIndex = 0;
    char stack[MAX];
    int stackTop = -1;

    while (1){
        switch (current_state){
            case S_START:
                if (isdigit(*myFunction) || *myFunction == '.' || *myFunction == 'x') {
                    current_state = S_OPERAND;
                }
                else if (*myFunction == '(') {
                    current_state = S_OPEN;
                }
                else if (*myFunction == 0) {
                    current_state = S_END;
                }
                else {
                    current_state = S_ERROR;
                }
                break;

            case S_OPERAND: {
                float operand = 0.0;
                int decimal_flag = 0;
                float decimal_divisor = 1.0;
                if (isdigit(*myFunction) || *myFunction == '.'){
                    while (isdigit(*myFunction) || *myFunction == '.') {
                        if (*myFunction == '.') {
                            decimal_flag = 1;
                        }
                        else {
                            if (decimal_flag == 0) {
                                operand = operand * 10 + (*myFunction - '0');
                            }
                            else {
                                decimal_divisor *= 10;
                                operand = operand + (*myFunction - '0') / decimal_divisor;
                            }
                        }
                        myFunction++;
                    }
                    output[outputIndex].type = OPERAND;
                    output[outputIndex].value.operand = operand;
                    outputIndex++;
                }
                else if (*myFunction == 'x') {
                    myFunction++;
                    output[outputIndex].type = VARIABLE;
                    output[outputIndex].value.variable = 0.0;
                    outputIndex++;                    
                }       
                if (isOperator(*myFunction)) {
                    current_state = S_OPERATOR;
                }
                else if (*myFunction == ')') {
                    current_state = S_CLOSE;
                }
                else if (*myFunction == 0) {
                    current_state = S_END;
                }
                else {
                    current_state = S_ERROR;
                }
                break;
            }

            case S_OPERATOR:
                while (stackTop >= 0 && isOperator(stack[stackTop]) && 
                       precedence(stack[stackTop]) >= precedence(*myFunction)) {
                    output[outputIndex].type = OPERATOR;
                    output[outputIndex].value.operator = stack[stackTop];
                    outputIndex++;
                    stackTop--;
                }
                stack[++stackTop] = *myFunction;
                myFunction++;
                current_state = S_START;
                break;

            case S_OPEN:
                stack[++stackTop] = *myFunction;
                myFunction++;
                current_state = S_START;
                break;

            case S_CLOSE:
                while (stackTop >= 0 && stack[stackTop] != '(') {
                    output[outputIndex].type = OPERATOR;
                    output[outputIndex].value.operator = stack[stackTop];
                    outputIndex++;
                    stackTop--;
                }
                if (stackTop >= 0) stackTop--; 
                myFunction++;
                if (isOperator(*myFunction)) {
                    current_state = S_OPERATOR;
                }
                else if (*myFunction == ')') {
                    current_state = S_CLOSE;
                }
                else if (*myFunction == 0) {
                    current_state = S_END;
                }
                else if (isdigit(*myFunction) || *myFunction == '.') {
                    current_state = S_OPERAND;
                }
                else {
                    current_state = S_ERROR;
                }
                break;

            case S_END:
                while (stackTop >= 0) {
                    output[outputIndex].type = OPERATOR;
                    output[outputIndex].value.operator = stack[stackTop];
                    outputIndex++;
                    stackTop--;
                }
                output[outputIndex].type = OPERATOR;
                output[outputIndex].value.operator = 'E';
                outputIndex++;
                return output;
                break;

            case S_ERROR:
            default:
                printf("Input function error!!!\n");
                return NULL;
        }
    }
}

float evaluatePostfix(Token *postfix, float x_value) {
    int counter = 0;
    float stack[100];
    float result = 0.0;
    int stackPointer = 0; //-1 mean stack empty;
    while (postfix->type != OPERATOR || postfix->value.operator != 'E'){
        if(postfix->type == OPERAND){
            stack[stackPointer] = postfix->value.operand;
            stackPointer += 1;
            if(stackPointer == 100){
                printf("Stack overflow");
            }
        } else if(postfix->type == OPERATOR){
            float tempOp1, tempOp2, tempResult;
            tempOp1 = stack[stackPointer-1];
            stackPointer --;
            tempOp2 = stack[stackPointer-1];
            stackPointer --;
            if(stackPointer < 0) printf("Stack Underflow");
            if(postfix->value.operator == '^'){
                tempResult = pow(tempOp2, tempOp1);
            } else if(postfix->value.operator == '*'){
                tempResult = tempOp2*tempOp1;
            } else if(postfix->value.operator == '/'){
                tempResult = tempOp2/tempOp1;
            } else if(postfix->value.operator == '+'){
                tempResult = tempOp2+tempOp1;
            } else if(postfix->value.operator == '-'){
                tempResult = tempOp2-tempOp1;
            }
            stack[stackPointer] = tempResult;
            stackPointer ++;
        } else if(postfix->type == VARIABLE){
            postfix->value.variable = x_value;
            stack[stackPointer] = postfix->value.variable;
            stackPointer += 1;
            if(stackPointer == 100){
                printf("Stack overflow");
            }
        }
        postfix ++;
    }
    return stack[0];
}

float findx(Token *postfix){
    float result = 1.2;
    float newResult = 0.0;
    while (evaluatePostfix(postfix, result) >= 0.000001 || evaluatePostfix(postfix, result) <= -0.000001){
        float diff = (evaluatePostfix(postfix, result+0.00001) - evaluatePostfix(postfix, result-0.00001)) / 0.00002;
        if (diff == 0){
            printf("Can not find X\n");
            break;
        }
        newResult = result - evaluatePostfix(postfix, result) / diff;
        printf("Next x= %f, f(x) = %f\n", newResult, evaluatePostfix(postfix, newResult));
        if(newResult == result){
            if(evaluatePostfix(postfix, result) != 0){
                printf("Unable to find x more correctlly");
                break;
            }
        }
        result = newResult;
    }
    result = newResult;
    return result;
}

void printTokens(Token *output) {
    int i = 0;
    printf("Output Tokens: ");
    while (output[i].type != OPERATOR || output[i].value.operator != 'E') {
        if (output[i].type == OPERAND) {
            printf("%.2f ", output[i].value.operand);
        } else if (output[i].type == OPERATOR) {
            printf("%c ", output[i].value.operator);
        } else if (output[i].type == VARIABLE) {
            printf("x ");
        }
        i++;
    }
    printf("\n");
}

int main(){
    Token *output;
    float x;
    char str[MAX];
    printf("Enter your equal (ex: 2.3x^2-6*x-8): ");
    fgets(str, MAX, stdin);
    str[strcspn(str, "\n")] = 0;

    output = infixToPostfix(str);
    printf("Enter X value: ");
    scanf("%f", &x);

    if (output != NULL) {
        printTokens(output);
        float result = evaluatePostfix(output, x);
        printf("Value of equal if x = %.2f is: %.2f\n", x, result);
    }
    float Ng = 0.0;
    Ng = findx(output);
    printf("F(x) = 0 => x = %f, double check: f(x=%f)= %f\n", Ng, Ng, evaluatePostfix(output, Ng));
    return 0;
}
