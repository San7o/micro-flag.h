// SPDX-License-Identifier: MIT

#define MICRO_FLAG_IMPLEMENTATION
#include "micro-flag.h"

#include <stdio.h>
#include <stdbool.h>

typedef struct {
  bool show_help;
  char* out_name;
  char a_char;
  int a_number;
  double a_double;
} Args;

int main(int argc, char** argv)
{
  Args args;
  // Default values
  args.show_help = false;
  args.out_name = "out";
  args.a_char = 'A';
  args.a_number = 0;
  args.a_double = 123.123;
  
  MicroFlag flags[] =
    {
      { MICRO_FLAG_BOOL, &args.show_help, "-h", "--help",   "show help message" },
      { MICRO_FLAG_STR,  &args.out_name,  "-o", "--output", "set output file"   },
      { MICRO_FLAG_CHAR, &args.a_char  ,  "-c", "--char",   "give me a char!"   },
      { MICRO_FLAG_INT,  &args.a_number,  "-n", "--number", "print this number" },
      { MICRO_FLAG_DOUBLE,  &args.a_double,  "-d", "--double", "print a double" },
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
  printf("A double:    %f\n", args.a_double);
  
  return 0;
}
