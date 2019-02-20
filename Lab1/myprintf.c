#include <stdio.h>
#include <stdarg.h> //To portably implement variadic functions
#include <stdlib.h>

//***** Custom Defined Data Types *****
typedef unsigned int u32;
typedef struct node{
	char type;
	struct node *Next;
}Node;


//***** Function Declarations *****
int rpu(u32 x, int base);
int prints(char *x); //Prints a string
int printu(u32 x); //Prints an unsigned integer
int printd(int x); //Prints an integer (x may be negative)
int printx(u32 x); //Prints in HEX (start with 0x)
int printo(u32 x); //Prints in Octal (start with 0)
int myprintf(char *fmt, ...); //Prints in all types for any amount of arguments
void createPrintStatement(int argc, char *argv[]); 

//***** Defined Constants *****
#define O_BASE 8
#define U_BASE 10
#define H_BASE 16
#define STRING 's'
#define UNSIGNED_INT 'u'
#define INTEGER 'd'
#define OCT 'o'
#define HEX 'x'
#define CHAR 'c'

//***** Global Variables *****
const char *ctable = "0123456789ABCDEF";

int main(int argc, char *argv[], char *env[]){
	/*
		argc - value
		argv- strings
		env - strings	
	*/
	puts("***** Test Input *****");
	myprintf("cha=%c string=%s      dec=%d hex=%x oct=%o neg=%d\n", 
		'A', "this is a test", 100, 100, 100, -100);
	createPrintStatement(argc, argv);
	return 0;
}

void createPrintStatement(int argc, char *argv[]){
	myprintf("\n***** Print argc and argv[] *****\nargc = %d\n", argc);
	for (int i = 0; i < argc; i++){
		myprintf("argv[%d] = %s\n", i+1, argv[i]);
	}
}

int rpu(u32 x, int base)
{  
	int characters = 0;
    char c;
    if (x){
       c = ctable[x % base];
       characters = 1 + rpu(x / base, base);
       putchar(c);
    }
    return characters;
}

int prints(char *x){//String
	if(x[0] == '\0'){
		return 0;
	}
	putchar(x[0]);
	return 1 + prints(++x);
}

int printu(u32 x) //Unsigned Integer
{
   return (x==0)? putchar('0') : rpu(x, U_BASE);
}

int printd(int x){ //Integer
	if(x < 0){
		putchar('-');
		x *= -1;
	}
	return rpu(x, U_BASE);
}

int printx(u32 x){//Hexadecimal
	return (x==0)? puts("0x") : rpu(x, H_BASE);
}

int printo(u32 x){//Octal
	return (x==0)? putchar('0') : rpu(x, O_BASE);
}

int myprintf(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt); // Requires the last fixed parameter (to get the address) 
    int i = 0;
	for (; fmt[i] != '\0' ; i++) {
		if(fmt[i] == '%' && fmt[i+1] != '\0'){
			char type = fmt[++i];
			switch(type){
				case STRING: prints(va_arg(ap, char*));
					break;
				case UNSIGNED_INT: printu(va_arg(ap, u32));
					break;
				case INTEGER: printd(va_arg(ap, int));
					break;
				case OCT: printo(va_arg(ap, u32));
					break;
				case HEX: printx(va_arg(ap, u32));
					break;
				default: putchar(va_arg(ap, int)); //case 'CHAR'
			}
		}
		else{ putchar(fmt[i]); }
	}
    va_end(ap);
    return i;
}
