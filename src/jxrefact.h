/*
 * jxrefact.h
 *
 * Copyright (C) 2011, Chris Harding.
 *
 * This file contains definitions for the enum factory. It requires enums
 * to be declared as follows:
 * 
 *    #define SOME_ENUM(XX) \
 *      XX(FirstValue,) \
 *      XX(SecondValue,) \
 *      XX(SomeOtherValue,=50) \
 *      XX(OneMoreValue,=100) \
 *    DECLARE_ENUM(SomeEnum,SOME_ENUM)
 *
 * The string representation can then be returned using:
 * 
 *    DEFINE_ENUM(SomeEnum,SOME_ENUM)
 * 
 * and then calling GetString_SomeEnum(value).
 * 
 */
 
// for "dynamic" function names (not really since still compile time)
#define PASTER(x,y) x ## _ ## y
#define EVALUATOR(x,y)  PASTER(x,y)

 
 // expansion macro for enum value definition
#define ENUM_VALUE(name,assign) name assign,

// expansion macro for enum to string conversion
#define ENUM_CASE(name,assign) case name: return #name;

// expansion macro for enum to string conversion
#define ENUM_STRCMP(name,assign) if (!strcmp(str,#name)) return name;

/// declare the access function and define enum values
#define DECLARE_ENUM(EnumType,ENUM_DEF) \
  typedef enum { \
    ENUM_DEF(ENUM_VALUE) \
  } EnumType; \
  const char * EVALUATOR(GetString, EnumType) (EnumType dummy); \
  EnumType EVALUATOR(GetValue, EnumType) (const char *string); \

/// define the access function names
#define DEFINE_ENUM(EnumType,ENUM_DEF) \
  const char * EVALUATOR(GetString, EnumType) (EnumType value) \
  { \
    switch(value) \
    { \
      ENUM_DEF(ENUM_CASE) \
      default: return "[*ENUM NOT DEFINED*]"; /* handle input error */ \
    } \
  } \
  EnumType EVALUATOR(GetValue, EnumType) (const char *str) \
  { \
    ENUM_DEF(ENUM_STRCMP) \
    return (EnumType)0; /* handle input error */ \
  } \
