/*

Author: Ted Meyer

Yes, I know there is a GNU library for reading flags, but I wanted to write my
own for the sake of reducing the number of includes, and for my own edification

This software is licensed under the The Happy Fun Ball License

*/


#ifndef FLAGS
#define FLAGS

// STRUCTS
struct flag
{
	char* flagname; // the name of the flag, ie, 'r' in '-r'
	char* flagarg;  // the value of the flag, ie, 'out' in '-o out'
	int   is_parameter_flag; // whether this flag should be parsed with params
	int   is_true_flag;      // whether this is a flag, or just a normal parameter
};

struct flaglist // dont use externally
{
	struct flag *first;    // yes this is modeled after scheme lists
	struct flaglist *rest;
};

struct arglist // dont use externally
{
	char* first;    // yes this is modeled after scheme lists
	struct arglist *rest;
};

struct paramlist
{
	int count;
	char** params;
};



// FUNCTION PROTOTYPES
int parse_flags(char** args, int count); // 1 less than argc from main!!
int set_flags_with_args(char** flags_with_args, int count);
char* get_flag(char* flagname); //returns 0 if no match, 1 if set with no value
struct paramlist* get_real_params();  //gets all the flags that arent flags, ie no -- or -

#endif