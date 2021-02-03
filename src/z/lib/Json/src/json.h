#ifndef JSON_HDR
#define JSON_HDR

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */

typedef enum {
  JSMN_UNDEFINED = 0,
  JSMN_OBJECT = 1,
  JSMN_ARRAY = 2,
  JSMN_STRING = 3,
  JSMN_PRIMITIVE = 4
} jsmntype_t;

enum jsmnerr {
  /* Not enough tokens were provided */
  JSMN_ERROR_NOMEM = -1,
  /* Invalid character inside JSON string */
  JSMN_ERROR_INVAL = -2,
  /* The string is not a full JSON packet, more bytes expected */
  JSMN_ERROR_PART = -3
};

/**
 * JSON token description.
 * type		type (object, array, string etc.)
 * start	start position in JSON data string
 * end		end position in JSON data string
 */
typedef struct {
  jsmntype_t type;
  int start;
  int end;
  int size;
#ifdef JSMN_PARENT_LINKS
  int parent;
#endif
} jsmntok_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string.
 */

typedef struct {
  unsigned int pos;     /* offset in the JSON string */
  unsigned int toknext; /* next token to allocate */
  int toksuper;         /* superior token node, e.g. parent object or array */
} jsmn_parser;

#define JSON_NUM_TOKENS 256
#define JSON_ERR_MSG_LEN 256
#define JSON_DEF_DATA_SIZE 256

typedef struct json_t json_t;

typedef int (*JsonParse_cb) (json_t *);
typedef int (*JsonGetData_cb) (json_t *);

struct json_t {
  char
     error_msg[JSON_ERR_MSG_LEN];

  string_t *data;

  int
    retval,
    num_tokens;

  jsmntok_t   *tokens;
  jsmn_parser *jsmn;

  JsonParse_cb parse;
  JsonGetData_cb get_data;

  void *obj;
};

typedef struct json_self {
  void  (*release) (json_t **);
  int   (*parse) (json_t *);
  json_t *(*new) (int, JsonParse_cb, JsonGetData_cb);
} json_self;

typedef struct json_T {
  json_self self;
} json_T;

public json_T __init_json__ (void);

#endif /* JSON_HDR */
