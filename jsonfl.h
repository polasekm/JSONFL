/*-----------------------------------------------------------------------------*/
/*
 * jsonfl.h
 *
 *  Created on: 21.3. 2021
 *      Author: Martin Polasek
 */
/*-----------------------------------------------------------------------------*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef JSONFL_H_INCLUDED
#define JSONFL_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdint.h>

/* Exported functions ------------------------------------------------------- */

/* Defines -------------------------------------------------------------------*/

#define JNFL_BUFF_LEN           256

// Parser state
#define JNFL_STAT_W_START       0
#define JNFL_STAT_W_KV          4
#define JNFL_STAT_KV            5
#define JNFL_STAT_ITEM_SEP      6
#define JNFL_STAT_END           7
#define JNFL_STAT_ERROR         8

// Value types
/*#define JNFL_TYPE_STRING        0
#define JNFL_TYPE_NUMBER        1
#define JNFL_TYPE_OBJECT        2
#define JNFL_TYPE_ARRAY         3
#define JNFL_TYPE_B_TRUE        4
#define JNFL_TYPE_B_FALSE       5
#define JNFL_TYPE_NULL          6
#define JNFL_TYPE_UNKNOWN       7*/

typedef enum
{
  JNFL_TYPE_STRING              = 0,
  JNFL_TYPE_NUMBER,
  JNFL_TYPE_OBJECT,
  JNFL_TYPE_ARRAY,
  JNFL_TYPE_B_TRUE,
  JNFL_TYPE_B_FALSE,
  JNFL_TYPE_NULL,
  JNFL_TYPE_UNKNOWN

} jsonfl_type_t;

//------------------------------------------------------------------------------
//typedef struct jsonfl
typedef struct jsonfl
{
  uint8_t state;                  //state

  uint8_t deep;
  jsonfl_type_t val_type;         //value type
  uint8_t escape;

  char buff[JNFL_BUFF_LEN + 1];   //buff + /0

  char *key;                      //key pointer
  char *val;                      //value pointer

  char *val_p;                    //value actual pointer

  void (*callback)(struct jsonfl*);      //events callback
  void *callback_data;            //for library user

} jsonfl_t;

// Functions -------------------------------------------------------------------

   void jsonfj_init(jsonfl_t *json, void (*callback)(jsonfl_t*)); //init json parser

   uint32_t jsonfl_read(jsonfl_t *json, const char *data, uint32_t len);  //read json data
   void jsonfj_reset(jsonfl_t *json);    //reset json parser
uint8_t jsonfj_state(jsonfl_t *json);    //get parser state

//------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif // JSONFL_H_INCLUDED
