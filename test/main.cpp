#include <iostream>
#include <cstdio>

#include "jsonfl/jsonfl.h"
//------------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------------
void json_item_proc(jsonfl_t *json);
//------------------------------------------------------------------------------
int main()
{
  FILE * pFile;
  char read_buff[2048];
  size_t result;
  long lSize;

  //uint32_t i;
  //char ch;

  jsonfl_t json;

  jsonfj_init(&json, json_item_proc);
  pFile = fopen ("bin/Debug/json_example4.json", "r");

  if(pFile != NULL)
  {
    cout << "Soubor otevren!" << endl;

    lSize = 2000;
    result = fread(read_buff, 1, lSize, pFile);
    /*if(result != lSize)
    {
      fputs("Reading error", stderr);
      exit (3);
    }

    for(i = 0; i < result; i++)
    {
      ch = read_buff[i];
      printf("%c", ch);

      read_buff[result] = 0;
      printf("%s len: %u/n", read_buff, result);
    }*/

    read_buff[result] = 0;
    printf(read_buff);

    jsonfl_read(&json, read_buff, result);
    printf("Parser state: %u\n", jsonfj_state(&json));

    fclose (pFile);
  }
  else
  {
    cout << "Soubor se epovedlo otevrit" << endl;
  }

  return 0;
}
//------------------------------------------------------------------------------
void json_item_proc(jsonfl_t *json)
{
  uint8_t i;

  printf("d = %u\t", json->deep);
  for(i = 0; i < json->deep; i++) printf("\t");
  switch(json->val_type)
  {
   case JNFL_TYPE_STRING:
     printf("type = STRING");
     break;

   case JNFL_TYPE_NUMBER:
     printf("type = NUMBER");
     break;

   case JNFL_TYPE_OBJECT:
     printf("type = OBJECT");
     break;

   case JNFL_TYPE_ARRAY:
     printf("type = ARRAY");
     break;

   case JNFL_TYPE_B_TRUE:
     printf("type = BOOL_T");
     break;

   case JNFL_TYPE_B_FALSE:
     printf("type = BOOL_F");
     break;

   case JNFL_TYPE_NULL:
     printf("type = NULL");
     break;

   case JNFL_TYPE_UNKNOWN:
     printf("type = UNKNOWN");
     break;

   default:
     printf("type = ERROR");
     break;
  }

  printf(" \tkey = %s, value = %s\n", json->key, json->val);
}
//------------------------------------------------------------------------------
