#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "mgos_bthing_sdk.h"

bool mg_bthing_register(mgos_bthing_t thing) {
  if (!thing) return false;
  struct mg_bthing_enum *things = &s_context.things;
  if (things->thing) {
    struct mg_bthing_enum *new_item = calloc(1, sizeof(struct mg_bthing_enum ));
    new_item->thing = things->thing;
    new_item->next_item = things->next_item;
    things->next_item = new_item;
  }
  things->thing = thing;
  mgos_event_trigger(MGOS_EV_BTHING_CREATED, thing);
  return true;
}

int mg_bthing_scount(const char *str1, const char* str2) {
  int count = 0;
  if (str1 != NULL && str2 != NULL) {
    int string_len = strlen(str2);
    for (const char *p = str1; *p != '\0';) {
      if (strncmp(p, str2, string_len) == 0) {
        ++count;
        p += string_len;
      } else {
        ++p;
      }
    }
  }
  return count;
}

bool mg_bthing_sreplace(const char *src,
                              const char *tag,
                              const char* replace_with,
                              char **out) {
  *out = NULL;

  /* count 'tag' occurrences in 'src' */
  int count = mg_bthing_scount(src, tag);
  if (count == 0) {
    return false;
  }

  int src_len = strlen(src);
  int tag_len = strlen(tag);
  int replace_with_len = (replace_with == NULL ? 0 : strlen(replace_with));

  /* allocate the buffer for the result string */
  int res_len = (src_len + (replace_with_len * count));
  *out = (char *)malloc(res_len + 1);
  strcpy(*out, src);

  /* allocate a temporary buffer */
  char *tmp_buf = (char *)malloc(src_len + 1);
  char *tmp_buf_to_free = tmp_buf;
  char *buf = *out;
      
  for (; *buf != '\0';) {
    if (strncmp(buf, tag, tag_len) == 0) {
      strcpy(tmp_buf, (buf + tag_len));
      strcpy(buf, (replace_with != NULL ? replace_with : ""));
      strcpy((buf + replace_with_len), tmp_buf);
      buf += replace_with_len;
    } else {
      ++buf;
    }
  }

  free(tmp_buf_to_free);
  return true;
}

bool mg_bthing_sreplaces(const char *src,
                               char **out,
                               int count,
                               ...) {
  *out = NULL;

  char *string = (char *)src;
  char *tag = NULL;
  char *tag_value = NULL;
  char *temp_out = NULL;

  va_list ap;
  va_start(ap, count);
  
  for (int i = 0; i < count; ++i) {
    tag = va_arg(ap, char*);
    tag_value = va_arg(ap, char*);
    
    if (mg_bthing_sreplace(string, tag, tag_value, &temp_out)) {
      if (string != src) free (string);
      string = *out = temp_out; 
    }
  }
  
  va_end(ap);
  return (*out != NULL);
}