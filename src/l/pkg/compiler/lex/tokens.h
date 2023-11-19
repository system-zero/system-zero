#define LA_ERROR         -1
#define LA_OK             0

#define TOKEN_EOF         0
#define TOKEN_NL          '\n'
#define TOKEN_WS          ' '
#define TOKEN_LDWS        (' ' + '~' )
#define TOKEN_BANG        '!'
#define TOKEN_DQUOTE      '"'
#define TOKEN_SL_COMMENT  '#'
#define TOKEN_DOLLAR      '$'
#define TOKEN_PERC        '%'
#define TOKEN_AMP         '&'
#define TOKEN_AMP_EQ      ('&' + '=' + '~')
#define TOKEN_LPAREN      '('
#define TOKEN_RPAREN      ')'
#define TOKEN_SQUOTE      '\''
#define TOKEN_STAR        '*'
#define TOKEN_PLUS        '+'
#define TOKEN_PLUS_PLUS   ('+' + '~')
#define TOKEN_PLUS_EQ     ('+' + '=' + '~')
#define TOKEN_COMMA       ','
#define TOKEN_MINUS       '-'
#define TOKEN_MINUS_MINUS ('-' + '~')
#define TOKEN_DOT         '.'
#define TOKEN_SLASH       '/'
#define TOKEN_COLON       ':'
#define TOKEN_SEMICOLON   ';'
#define TOKEN_LESS        '<'
#define TOKEN_LESS_LESS   ('<' + '~')
#define TOKEN_ASSIGN      '='
#define TOKEN_GT          '>'
#define TOKEN_GT_GT       ('>' + '~')
#define TOKEN_LBRACKET    '['
#define TOKEN_RBRACKET    ']'
//#define TOKEN_BINARY    'B'
#define TOKEN_OCTAL       'C'
//#define TOKEN_DECIMAL   'D'
//#define TOKEN_HEX       'E'
#define TOKEN_DQ_STRING   'F'
#define TOKEN_BQ_STRING   'G'
#define TOKEN_SYMBOL      'S'
#define TOKEN_INT         'F'
#define TOKEN_NUMBER      'G'
#define TOKEN_THEN        'K'
#define TOKEN_BSLASH      '\\'
#define TOKEN_CIRCUM      '^'
#define TOKEN_BQUOTE      '`'
#define TOKEN_CONST       'a'
#define TOKEN_CHAR        'c'
#define TOKEN_FOR         'f'
#define TOKEN_FUNC        'g'
#define TOKEN_IF          'i'
#define TOKEN_IFNOT       'j'
#define TOKEN_IMPORT      'k'
#define TOKEN_IN          'l'
#define TOKEN_IS          'm'
#define TOKEN_ISNOT       'n'
#define TOKEN_NULL        'o'
#define TOKEN_VAR         'v'
//#define TOKEN_WHILE     'w'
#define TOKEN_LBRACE      '{'
#define TOKEN_BAR         '|'
#define TOKEN_BAR_EQ      ('|' + '=' + '~')
#define TOKEN_RBRACE      '}'
#define TOKEN_TILDE       '~'
#define TOKEN_UNARY TOKEN_TILDE
