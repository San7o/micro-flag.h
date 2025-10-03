// SPDX-License-Identifier: MIT
//
// micro-flag.h
// -----------
//
// Header-only tiny C99 library to parse command line arguments.
//
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// License: MIT
//
//
// Usage
// -----
//
// Do this:
//
//   #define MICRO_FLAG_IMPLEMENTATION
//
// before you include this file in *one* C or C++ file to create the
// implementation.
//
// i.e. it should look like this:
//
//   #include ...
//   #include ...
//   #include ...
//   #define MICRO_FLAG_IMPLEMENTATION
//   #include "micro-flag.h"
//
// To use micro-flag, create first an array of MicroFlags, like so:
//
// ```
// typedef struct {
//   bool show_help;
//   char* out_name;
//   char a_char;
//   int a_number;
// } Args;                   // This is just for convenience
//
// int main(int argc, char** argv)
// {
//   Args args;
//   // Default values
//   args.show_help = false;
//   args.out_name = "out";
//   args.a_char = 'A';
//   args.a_number = 0;
//
//  // Create an array of flags
//  MicroFlag flags[] =
//    {
//       { MICRO_FLAG_BOOL, &args.show_help, "-h",
//                          "--help", "show help message" },
//       { MICRO_FLAG_STR,  &args.out_name,  "-o",
//                          "--output", "set output file"   },
//       { MICRO_FLAG_CHAR, &args.a_char  ,  "-c",
//                          "--char",   "give me a char!"   },
//       { MICRO_FLAG_INT,  &args.a_number,  "-n",
//                          "--number", "print this number" },
//    };
//  size_t num_flags = sizeof(flags) / sizeof(flags[0]);
// ...
// ```
//
// Now you can call `micro_flag_parse` which will set the variables
// with the parsed valued from the arguments:
//
// ```
// if (micro_flag_parse(flags, num_flags, argc, argv) != MICRO_FLAG_OK)
//    return 1;
// ```
//
// You can use `micro_flag_print_help` to print the help message:
//
// ```
// 
// if (args.show_help)
// {
//   micro_flag_print_help("example",
//                         "A sample application to showcase the library",
//                         flags,
//                         num_flags);
//    return 0;
//  }
// ```
//
// Check out the full example at the end of the header.
//
//
// Code
// ----
//
// The official git repository of micro-flag.h is hosted at:
//
//     https://github.com/San7o/micro-flag.h
//
// This is part of a bigger collection of header-only C99 libraries
// called "micro-headers", contributions are welcome:
//
//     https://github.com/San7o/micro-headers
//

#ifndef _MICRO_FLAG_H_
#define _MICRO_FLAG_H_

#define MICRO_FLAG_MAJOR 0
#define MICRO_FLAG_MINOR 1

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef enum {
  MICRO_FLAG_OK = 0,
  MICRO_FLAG_ERROR_UNKNOWN_TYPE,
  MICRO_FLAG_ERROR_MISSING_CHAR,
  MICRO_FLAG_ERROR_MISSING_STR,
  MICRO_FLAG_ERROR_MISSING_INT,
  MICRO_FLAG_ERROR_MISSING_DOUBLE,
  MICRO_FLAG_ERROR_CHAR_WRONG_ARG,
  MICRO_FLAG_ERROR_UNKNOWN_FLAG,
  MICRO_FLAG_ERROR_NOT_AN_INT,
  MICRO_FLAG_ERROR_NOT_A_DOUBLE,
  _MICRO_FLAG_ERROR_MAX,
} MicroFlagError;

typedef enum {
  MICRO_FLAG_BOOL = 0,
  MICRO_FLAG_CHAR,
  MICRO_FLAG_STR,
  MICRO_FLAG_INT,
  MICRO_FLAG_DOUBLE,
  _MICRO_FLAG_MAX,
} MicroFlagType;

// A single flag
typedef struct {
  // The type of value that should be set
  MicroFlagType type;
  // An opaque pointer to the value that should be set
  // For example, if type is MICRO_FLAG_INT, then value should
  // contain a pointer to an integer variable. The value of the pointer
  // will be set to the parsed argument.
  void *value;
  // Short flag (like "-h", "-o")
  char *short_name;
  // Long flag (line "--help", "--output")
  char *long_name;
  // A short description of this flag
  char *description;
} MicroFlag;
  
//
// Declarations
//

// Parse [num_flags] [flags] from [argc] [argv]
//
// Returns: MICRO_FLAG_OK on success, or an error and prints and error
// message in case parsing was not successful
MicroFlagError micro_flag_parse(MicroFlag *flags,
                                unsigned int num_flags,
                                int argc,
                                char **argv);

// Print the help message with [flags] information
//
// Args:
//  - prog_name: name of the program
//  - description: short description of the program
//  - flags: pointer to an array of flags
//  - num_flags: the number of flags
//
// Returns: MICRO_FLAG_OK on success, or an error.
MicroFlagError micro_flag_print_help(const char* prog_name,
                                     const char* description,
                                     MicroFlag *flags,
                                     unsigned int num_flags);

// String representation of MicoFlagType
extern const char* micro_flag_type_str[_MICRO_FLAG_MAX];

//
// Implementation
//
  
#ifdef MICRO_FLAG_IMPLEMENTATION

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

const char *micro_flag_type_str[] = { "", "<char>", "<str>", "<int>", "<double>" };

MicroFlagError micro_flag_parse(MicroFlag *flags,
                                unsigned int num_flags,
                                int argc,
                                char **argv)
{
  for (int i = 1; i < argc; ++i)
  {
    bool found = false;
    char *endptr;
    for (unsigned int flag = 0; flag < num_flags; ++flag)
    {
      if ((flags[flag].short_name && strcmp(flags[flag].short_name, argv[i]) == 0)
          || (flags[flag].long_name && strcmp(flags[flag].long_name, argv[i]) == 0))
      {
        found = true;
        switch (flags[flag].type)
        {
        case MICRO_FLAG_BOOL:
          *((bool*) flags[flag].value) = true;
          break;
        case MICRO_FLAG_CHAR:
          if (i + 1 >= argc)
          {
            printf("Usage: %s,%s <char>\n",
                   flags[flag].short_name,
                   flags[flag].long_name);
            return MICRO_FLAG_ERROR_MISSING_CHAR;
          }
          if (strlen(argv[i+1]) != 1)
          {
            printf("Usage: %s,%s <char>\n",
                   flags[flag].short_name,
                   flags[flag].long_name);
            return MICRO_FLAG_ERROR_CHAR_WRONG_ARG;
          }
          *((char*) flags[flag].value) = *argv[i+1];
          i++;
          break;
        case MICRO_FLAG_STR:
          if (i + 1 >= argc)
          {
            printf("Usage: %s,%s <string>\n",
                   flags[flag].short_name,
                   flags[flag].long_name);
            return MICRO_FLAG_ERROR_MISSING_STR;
          }
          *((char**) flags[flag].value) = argv[i+1];
          i++;
          break;
        case MICRO_FLAG_INT:
          if (i + 1 >= argc)
          {
            printf("Usage: %s,%s <integer>\n",
                   flags[flag].short_name,
                   flags[flag].long_name);
            return MICRO_FLAG_ERROR_MISSING_INT;
          }

          errno = 0;
          long val_int = strtol(argv[i+1], &endptr, 10);
          if (endptr == argv[i+1] || errno == ERANGE
              || val_int > INT_MAX || val_int < INT_MIN)
          {
            printf("Usage: %s,%s <integer>\n",
                   flags[flag].short_name,
                   flags[flag].long_name);
            return MICRO_FLAG_ERROR_NOT_AN_INT;
          }
          *((int*) flags[flag].value) = val_int;
          i++;
          break;
        case MICRO_FLAG_DOUBLE:
          if (i + 1 >= argc)
          {
            printf("Usage: %s,%s <double>\n",
                   flags[flag].short_name,
                   flags[flag].long_name);
            return MICRO_FLAG_ERROR_MISSING_DOUBLE;
          }

          errno = 0;
          double val_double = strtod(argv[i+1], &endptr);
          if (endptr == argv[i+1] || errno == ERANGE)
          {
            printf("Usage: %s,%s <double>\n",
                   flags[flag].short_name,
                   flags[flag].long_name);
            return MICRO_FLAG_ERROR_NOT_A_DOUBLE;
          }
          *((double*) flags[flag].value) = val_double;
          i++;
          break;
        default:
          return MICRO_FLAG_ERROR_UNKNOWN_TYPE;
        }
      }
    }
    if (!found)
    {
      printf("Error parsing flags: unknown flag \"%s\"\n", argv[i]);
      return MICRO_FLAG_ERROR_UNKNOWN_FLAG;
    }
  }
  
  return MICRO_FLAG_OK;
}

MicroFlagError micro_flag_print_help(const char* prog_name,
                                     const char* description,
                                     MicroFlag *flags,
                                     unsigned int num_flags)
{
  printf("%s\n", prog_name);
  printf("%s\n", description);
  printf("\n");
  printf("Options:\n");

  for (unsigned int i = 0; i < num_flags; ++i)
  {
    bool has_short = (flags[i].short_name != NULL);
    bool has_long = (flags[i].long_name != NULL);
    printf("    %s%s%s %s\n        %s\n",
           (has_short ? flags[i].short_name : ""),
           (has_long && has_short) ? "," : "",
           (has_long ? flags[i].long_name : ""),
           micro_flag_type_str[flags[i].type],
           flags[i].description);
  }
  
  return MICRO_FLAG_OK;
}

#endif // MICRO_FLAG_IMPLEMENTATION

//
// Example
//

#if 0

#define MICRO_FLAG_IMPLEMENTATION
#include "micro-flag.h"

#include <stdio.h>
#include <stdbool.h>

typedef struct {
  bool show_help;
  char* out_name;
  char a_char;
  int a_number;
} Args;

int main(int argc, char** argv)
{
  Args args;
  // Default values
  args.show_help = false;
  args.out_name = "out";
  args.a_char = 'A';
  args.a_number = 0;
  
  MicroFlag flags[] =
    {
      { MICRO_FLAG_BOOL, &args.show_help, "-h", "--help",   "show help message" },
      { MICRO_FLAG_STR,  &args.out_name,  "-o", "--output", "set output file"   },
      { MICRO_FLAG_CHAR, &args.a_char  ,  "-c", "--char",   "give me a char!"   },
      { MICRO_FLAG_INT,  &args.a_number,  "-n", "--number", "print this number" },
    };

  size_t num_flags = sizeof(flags) / sizeof(flags[0]);
  if (micro_flag_parse(flags, num_flags, argc, argv) != MICRO_FLAG_OK)
    return 1;
  
  if (args.show_help)
  {
    micro_flag_print_help("example",
                          "A sample application to showcase the library",
                          flags,
                          num_flags);
    return 0;
  }

  printf("Output file: %s\n", args.out_name);
  printf("A char:      %c\n", args.a_char);
  printf("A number:    %d\n", args.a_number);
  
  return 0;
}

#endif

#ifdef __cplusplus
}
#endif
  
#endif // _MICRO_FLAG_H_
