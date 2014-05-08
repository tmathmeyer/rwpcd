/*

Author: Ted Meyer

Yes, I know there is a GNU library for reading flags, but I wanted to write my
own for the sake of reducing the number of includes, and for my edification.

This software is licensed under the The Happy Fun Ball License

*/

#include "flags.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*
STRUCTS:
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



FUNCTION PROTOTYPES:
	flaglist* parse_flags(char** args, int arg_count); // 1 less than argc from main!!
	int set_flags_with_args(char** flags_with_args, int count);
	char* get_flag(char* flagname); // returns 0 if no match, 1 if set with no value
	char** get_real_params();  // gets all the flags that arent flags, ie no -- or -

*/

//globals, dont mess with!!
struct flaglist* __flaglist__  = 0;
struct arglist* __arglist__ = 0;
int __flags_not_flags = 0;
int _init_set_ = 0;

int error(void);
int has_args(char* arg);


int parse_flags(char** args, int count)
{
	int arg_count = 0;
    _init_set_ = 1;
	for(; arg_count<count; arg_count++)
	{
		struct flaglist* flaglist = (struct flaglist*)(malloc(sizeof(flaglist)));
		struct flag* flag = (struct flag*)(malloc(sizeof(flag)));

		flaglist -> first = flag;
		flag -> flagname = *(args + arg_count);

		if ( *(flag -> flagname) == '-')
		{ // is a flag, checking for args
			flag -> is_true_flag = 1;
			flag -> flagname += 1; // clean - for searching purposes

			if (has_args(flag -> flagname))
			{ // it has args!
				flag -> is_parameter_flag = 1;
				arg_count++;
				flag -> flagarg = *(args + arg_count);
			}
			else
			{
				flag -> is_parameter_flag = 0;
			}
		}
		else
		{
			flag -> is_true_flag = 0;
			__flags_not_flags += 1;
		}


		flaglist -> rest = __flaglist__;
		__flaglist__ = flaglist;
	}

	return count;
}



int set_flags_with_args(char** flags_with_args, int count)
{
	if (__flaglist__)
	{
		perror("you have already parsed the flags!");
		return -1;
	}

	int arg_count = 0;
	for(; arg_count<count; arg_count++)
	{
		struct arglist* arglist = (struct arglist*)(malloc(sizeof(arglist)));
		arglist -> first = *(flags_with_args + arg_count);
		arglist -> rest = __arglist__;
		__arglist__ = arglist;
	}

	return count;
}


char* get_flag(char* flagname)
{
	if (error())
	{
		return 0;
	}

	struct flaglist* flaglist = __flaglist__;
	while(flaglist)
	{
		if (strcmp(flagname, flaglist->first->flagname) == 0)
		{ // match
			if (flaglist -> first -> is_parameter_flag)
			{
				return flaglist -> first -> flagarg;
			}
			else
			{
				return (char*)1;
			}
		}
		flaglist = flaglist -> rest;
	}
	return 0;
}


int error(void)
{
	if (_init_set_)
	{
		return 0;
	}
	else
	{
		puts("You havent parsed the flags yet!");
		return 1;
	}
}


int has_args(char* arg)
{
	struct arglist* arglist = __arglist__;
	while(arglist)
	{
		if (strcmp(arg, arglist -> first) == 0)
		{
			return 1;
		}
		arglist = arglist -> rest;
	}
	return 0;
}



struct paramlist* get_real_params()
{
	if (error())
	{
		return 0;
	}

	int count = 0;
	char** real = (char**)(malloc(sizeof(char*) * __flags_not_flags));
	struct paramlist* result = (struct paramlist*)
							   (malloc(sizeof(struct paramlist)));
	result -> params = real;
	result -> count = 0;
	struct flaglist* flaglist = __flaglist__;
	while(flaglist)
	{
		if (!(flaglist->first -> is_true_flag))
		{
			real[__flags_not_flags - (++count)] = flaglist->first->flagname;
			result -> count ++;
		}
		flaglist = flaglist -> rest;
	}
	return result;
}
