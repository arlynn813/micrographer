/* Program to take a function in the form
 * of a character string and convert it
 * to Reverse Polish Notation in the form
 * of an array of integers.
 *
 * NOTES: Function takes one argument in
 * the form of a function. Signs include
 * +,-,*,/, and $ for exponent. Compiler
 * didn't like me using '^' for some
 * reason. Brackets, '(' and ')', should
 * be fully functional, but error testing
 * isn't fully fleshed out. Function first
 * prints raw input, then input as an
 * array of integers, then the shunted
 * array. All values in equation are stored
 * as integers, so don't use things larger
 * than int MAX or smaller than int MIN.
 *
 * I tried commenting a lot, but I tend to
 * get sucked in and just blast out a dozen
 * lines before going back. Everything has
 * a purpose, probably.
 *
 * Resources:
 * https://en.wikipedia.org/wiki/Shunting-yard_algorithm
 * https://en.wikipedia.org/wiki/Reverse_Polish_notation
 *
 * All constants are represented as positives
 * and all signs are represented by negative
 * values:
 *
 *      +                       -7
 *      -                       -6
 *
 *      *                       -4
 *      '/'                     -3
 *
 *      ^($)            -1
 *
 *      (                       -9
 *      )                       -10
 *
 *      x                       -11
 *      y                       -12
 *
 *      terminate       -15
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int new_arr[30];
int shunted[30];

#define STRLEN 14       //Max length of input string + 1 for terminating int

int* string_to_array(char*);
int* shunt(int*);
float rpn_eval(int, int, int*);

//function to convert array of characters to useable array of integers
int* string_to_array(char* in_arr) {
        int spare_space = 0;
        int closeflag = 0;                                                                                                      //flag to represent need for closing bracket
        int i = 0;                                                                                                                      //location in string (old)
        int k = 0;                                                                                                                      //location in array (new)
        int str_lngth_tmp = 0;                                                                                          //establishes temporary string length for later
        while (in_arr[str_lngth_tmp] != 0)
                str_lngth_tmp++;
//        int* new_arr = malloc(sizeof(int) * str_lngth_tmp);                       //allocates new array of integers
        while(in_arr[i] != 0 && i < str_lngth_tmp) {                                            //while there are tokens in the string
                switch(in_arr[i]) {                                                                                             //behavior changes based on character
                        case '+':
                                new_arr[k++] = -7;
                                break;
                        case '-':
                                if((i == 0) || ((in_arr[i - 1] <= '0' || in_arr[i - 1] >= '9') && (in_arr[i - 1] != 'x' && in_arr[i - 1] != 'y'))) {            //If first char is '-' or the preceding char is non-numeric
                                        str_lngth_tmp += 3;                                                                     //increases temporary length of array
                                        //new_arr = (int*)realloc(new_arr, sizeof(int) * str_lngth_tmp);          //Extends array to account for added (0...)
                                        new_arr[k++] = -9;                                                                      //'('
                                        new_arr[k++] = 0;                                                                       //'0'
                                        new_arr[k++] = -6;                                                                      //'-'
                                        closeflag = 2;                                                                          //set to 2 to let one iteration pass before adding bracket
                                }
                                else {
                                        new_arr[k++] = -6;
                                }
                                break;
                        case '*':
                                new_arr[k++] = -4;
                                break;
                        case '/':
                                new_arr[k++] = -3;
                                break;
                        case '^':
                                new_arr[k++] = -1;
                                break;
                        case '(':
                                new_arr[k++] = -9;
                                break;
                        case ')':
                                new_arr[k++] = -10;
                                break;
                        case 'x':
                                new_arr[k++] = -11;
                                break;
                        case 'y':
                                new_arr[k++] = -12;
                                break;
                        default :

                                ;int tempVal = in_arr[i] - '0';                                                  //Assign the current value to a temporary int
                                while((in_arr[i+1] >= '0')  && (in_arr[i+1] <= '9')) {  //While the next value is a digit between 0 and 9
                                        tempVal = 10 * tempVal + (in_arr[i+1] - '0');           //Interpret all numbers as base 10 values
                                        spare_space++;                                                                          //Every character that's rolled into a single integer frees up a byte of space
                                        i++;
                                }
                                new_arr[k++] = tempVal;
                }
                if (closeflag > 0) {
                        if (closeflag == 1) {                                                                           //takes one iteration of the code to trigger
                                new_arr[k++] = -10;                                                                             //tacks on a bracket as a result of negative number conversion
                        }
                        closeflag--;
                }
                i++;
        }
        if(spare_space != 0) {
//                new_arr = (int*)realloc(new_arr, sizeof(int) * (str_lngth_tmp - spare_space));  //Reallocate the new array so no space is wasted
        }
        new_arr[k++] = -15;
        return new_arr;
}

//function to rearrange array of integers in reverse polish notation
int* shunt(int* in_arr) {
        int str_lngth = 0;
        int j = 0;
        while (in_arr[j] != -15) {
                j++;
                str_lngth++;
        }
        int op_stack[30];
        j = 0;                                                                                                                          //Position in output array
        int k = 0;                                                                                                                      //Position in operator stack
        for(int i = 0; i < str_lngth; i++) {                                                            //Position in input array, WHILE THERE ARE TOKENS TO BE READ
                if(in_arr[i] >= 0 || in_arr[i] == -11 || in_arr[i] == -12) {    //Case for integers
                        shunted[j++] = in_arr[i];                                                                       //just shove them in the output
                }
                else if(in_arr[i] < 0 && in_arr[i] > -8) {                                              //Case for Operators
                        while((k > 0) && (op_stack[k-1] >= in_arr[i] + 1) && (op_stack[k-1] != -9)) {   //while operator on stack has higher precedence
                            shunted[j++] = op_stack[--k];
                        }
                        op_stack[k++] = in_arr[i];                                                                      //then onto the operator stack they go
                }
                else if((in_arr[i] == -9) || (in_arr[i] == -10)) {                              //Case for brackets
                        if(in_arr[i] == -9) {                                                                           //If left bracket
                                op_stack[k++] = in_arr[i];                                                              //onto the operator stack it goes
                        }
                        else{                                                                                                           //If right bracket
                                while(op_stack[--k] != -9) {                                                    //pop operators until left bracket is found
                                    shunted[j++] = op_stack[k];
                                        if(k == 0) {                                                                            //Error handling for if mismatched brackets
                                                k = j;
                                                break;
                                        }
                                }
                        }
                }
        }
        while(k > 0) {                                                                                                          //If you run out of in_arr, toss the remaining operators on the out array
                if(op_stack[k-1] != -9) {
                    shunted[j++] = op_stack[--k];
                }
                else {
                        k--;                                                                                                            //Error handling for mismatched brackets
                }
        }
        //free(op_stack);
        shunted[j++] = -15;
        return shunted;
}

//function to take x and y to output z
float rpn_eval(int x,int y,int* rpn_arr) {
        int j = 0;
        int rpn_lngth = 0;
        while (rpn_arr[j] != -15) {
                j++;
                rpn_lngth++;
        }

        if (rpn_lngth == 1) {                   //Handles single character inputs
            if ( rpn_arr[0] >= 0)
              return rpn_arr[0];
            else if ( rpn_arr[0] == -11)
              return x;
            else if ( rpn_arr[0] == -12)
              return y;
        }

        float temp_op[30];      //make operand stack
        j = 0;                                                                                                                          //location in operand stack
        float result = 0.0;                                                                                                             //final value to return
        for(int i = 0; i < rpn_lngth; i++) {                                                            //for every token in rpn array
                if(rpn_arr[i] < 0 && rpn_arr[i] > -11) {                                                //If token is operator
                        float operand_1 = temp_op[--j];                                                         //Get operand 1
                        float operand_2 = temp_op[--j];                                                         //Get operand 2
                        switch(rpn_arr[i]) {                                                                            //Carry out operation
                                case -7:                                                                                                //If +
                                        result = operand_2 + operand_1;
                                        break;
                                case -6:                                                                                                //If -
                                        result = operand_2 - operand_1;
                                        break;
                                case -4:                                                                                                //If *
                                        result = operand_2 * operand_1;
                                        break;
                                case -3:                                                                                                //If /
                                        result = operand_2 / operand_1;                                         //Put '/0' error handling here
                                        break;
                                case -1:                                                                                                //If ^
                                        result = powf(operand_2, operand_1);
                                        break;
                        }
                        temp_op[j++] = result;                                                                          //Store result back on stack
                }
                else {                                                                                                                  //If token is operand
                        if (rpn_arr[i] == -11) {                                                                        //If value indicating x appears
                                temp_op[j++] = (float)x;                                                                //Store current x as operand
                        }
                        else if (rpn_arr[i] == -12) {                                                           //If value indicating y appears
                                temp_op[j++] = (float)y;                                                                //store current y as operand
                        }
                        else {                                                                                                          //If it's a straight number
                                temp_op[j++] = (float)rpn_arr[i];                                               //store it as an operand
                        }
                }
        }
        return result;
}

int* string_to_rpn(char* str) {
        int* arr = string_to_array(str);
        arr = shunt(arr);
        return arr;
}
