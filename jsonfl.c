#include "jsonfl.h"

//------------------------------------------------------------------------------
void jsonfj_init(jsonfl_t *json, void (*callback)(jsonfl_t*))     //init json parser
{
  json->state = JNFL_STAT_W_START;

  json->deep = 0;
  json->escape = 0;
  json->val_type = JNFL_TYPE_UNKNOWN;

  json->buff[0] = 0;
  json->buff[JNFL_BUFF_LEN] = 0;

  json->key = json->buff;
  json->val = json->buff;

  //val_p = val;

  json->callback = callback;
}
//------------------------------------------------------------------------------
void jsonfj_reset(jsonfl_t *json)    //reset json parser
{
  json->state = JNFL_STAT_W_START;

  json->deep = 0;
  json->escape = 0;
  json->val_type = JNFL_TYPE_NULL;
}
//------------------------------------------------------------------------------
uint8_t jsonfj_state(jsonfl_t *json)    //get parser state
{
  return json->state;
}
//------------------------------------------------------------------------------
uint32_t jsonfl_read(jsonfl_t *json, const char *data, uint32_t len) //read json data
{
  uint32_t i;
  char ch;

  for(i = 0; i < len; i++)
  {
    ch = data[i];

    switch(json->state)
    {
    case JNFL_STAT_W_START:
      if(ch == '{')
      {
        json->val   = json->buff;
        json->val_p = json->buff;

        json->state = JNFL_STAT_W_KV;
      }
      else if(ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') continue;
      else
      {
        json->state = JNFL_STAT_ERROR;
        return i;
      }
      break;

    case JNFL_STAT_W_KV:
      if(ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') continue;
      if(ch == ',') continue;

      json->val_type = JNFL_TYPE_UNKNOWN;
      json->state = JNFL_STAT_KV;

    case JNFL_STAT_KV:
      if(json->val_type == JNFL_TYPE_UNKNOWN)
      {
        if(ch == '"')
        {
          json->val_type = JNFL_TYPE_STRING;
          continue;
        }
        else if((ch >= '0' && ch <= '9') || (ch=='-')) json->val_type = JNFL_TYPE_NUMBER;
        else if(ch == 't') json->val_type = JNFL_TYPE_B_TRUE;
        else if(ch == 'f') json->val_type = JNFL_TYPE_B_FALSE;
        else if(ch == '{')
        {
          *(json->val_p) = 0;
          json->val_type = JNFL_TYPE_OBJECT;

          json->callback(json);

          json->val   = json->buff;
          json->val_p = json->buff;

          json->state = JNFL_STAT_W_KV;
          json->deep++;
          continue;
        }
        else if(ch == '[')
        {
          *(json->val_p) = 0;
          json->val_type = JNFL_TYPE_ARRAY;

          json->callback(json);

          json->val   = json->buff;
          json->val_p = json->buff;

          json->state = JNFL_STAT_W_KV;
          json->deep++;
          continue;
        }
        else if(ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') continue;
        else if(ch == 'n') json->val_type = JNFL_TYPE_NULL;
        else if(ch == '}' || ch == ']')
        {
          // Duplicita z casti separace, ale bez callbacku
          *(json->val_p) = 0;

          json->val   = json->buff;
          json->val_p = json->buff;

          json->state = JNFL_STAT_W_KV;

          if(json->deep == 0) json->state = JNFL_STAT_END;
          else json->deep--;
          continue;
        }
        else
        {
          json->state = JNFL_STAT_ERROR;
          return i;
        }

        *(json->val_p++) = ch;
        continue;
      }
      else if(json->val_type == JNFL_TYPE_STRING)
      {
        if(ch == '"')
        {
          *(json->val_p) = 0;
          json->state = JNFL_STAT_ITEM_SEP;
        }
        else
        {
          if(json->val_p - json->val < JNFL_BUFF_LEN) *(json->val_p++) = ch;
          else
          {
            json->state = JNFL_STAT_ERROR;
            return i;
          }
        }
        continue;
      }
      else if(json->val_type == JNFL_TYPE_NUMBER)
      {
        if((ch >= '0' && ch <= '9') || ch == '.')
        {
          if(json->val_p - json->val < JNFL_BUFF_LEN) *(json->val_p++) = ch;
          else
          {
            json->state = JNFL_STAT_ERROR;
            return i;
          }
          continue;
        }
        else
        {
          *(json->val_p) = 0;
          json->state = JNFL_STAT_ITEM_SEP;
        }
      }
      else if(json->val_type == JNFL_TYPE_B_TRUE)
      {
        if(json->val_p - json->val < strlen("true") - 1) *(json->val_p++) = ch;
        else
        {
          *(json->val_p++) = ch;
          *(json->val_p) = 0;
          if(strcmp(json->val, "true") == 0) json->state = JNFL_STAT_ITEM_SEP;
          else
          {
            json->state = JNFL_STAT_ERROR;
            return i;
          }
        }
        continue;
      }
      else if(json->val_type == JNFL_TYPE_B_FALSE)
      {
        if(json->val_p - json->val < strlen("false") - 1) *(json->val_p++) = ch;
        else
        {
          *(json->val_p++) = ch;
          *(json->val_p) = 0;
          if(strcmp(json->val, "false") == 0) json->state = JNFL_STAT_ITEM_SEP;
          else
          {
            json->state = JNFL_STAT_ERROR;
            return i;
          }
        }
        continue;
      }
      else if(json->val_type == JNFL_TYPE_NULL)
      {
        if(json->val_p - json->val < strlen("null") - 1) *(json->val_p++) = ch;
        else
        {
          *(json->val_p++) = ch;
          *(json->val_p) = 0;
          if(strcmp(json->val, "null") == 0) json->state = JNFL_STAT_ITEM_SEP;
          else
          {
            json->state = JNFL_STAT_ERROR;
            return i;
          }
        }
        continue;
      }
      else
      {
        json->state = JNFL_STAT_ERROR;
        return i;
      }

    case JNFL_STAT_ITEM_SEP:
      if(ch == ':')
      {
        json->val_p++;
        json->val = json->val_p;
        *(json->val_p) = 0;

        json->state = JNFL_STAT_W_KV;
      }
      else if(ch == ',')
      {
        *(json->val_p) = 0;

        json->callback(json);

        json->val   = json->buff;
        json->val_p = json->buff;

        json->state = JNFL_STAT_W_KV;
      }
      else if(ch == '}' || ch == ']')
      {
        *(json->val_p) = 0;

        json->callback(json);

        json->val   = json->buff;
        json->val_p = json->buff;

        json->state = JNFL_STAT_W_KV;

        if(json->deep == 0) json->state = JNFL_STAT_END;
        else json->deep--;
      }
      else if(ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') continue;
      else
      {
        json->state = JNFL_STAT_ERROR;
        return i;
      }
      break;

    case JNFL_STAT_END:
      if(ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t' || ch == '\0' || ch == '\xFF') continue;
      else
      {
        json->state = JNFL_STAT_ERROR;
        return i;
      }
      break;

    case JNFL_STAT_ERROR:
      break;

    default:
      break;
    }
  }
  return len;
}
//------------------------------------------------------------------------------
