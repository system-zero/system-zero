/*
 Simple, STB-style, parser for URL:s as specified by RFC1738 ( http://www.ietf.org/rfc/rfc1738.txt )

 version 1.0, June, 2014

 Copyright (C) 2014- Fredrik Kihlander

 This software is provided 'as-is', without any express or implied
 warranty.  In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

 1. The origin of this software must not be misrepresented; you must not
 claim that you wrote the original software. If you use this software
 in a product, an acknowledgment in the product documentation would be
 appreciated but is not required.
 2. Altered source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
 3. This notice may not be removed or altered from any source distribution.

 Fredrik Kihlander
 */


/**
 * Parse an url specified by RFC1738 into its parts.
 */

#include "url.h"

typedef url_t parsed_url;

typedef struct parse_url_ctx
{
  void* mem;
  size_t memsize;
  size_t memleft;
} parse_url_ctx;

#ifdef URL_USE_INTERNAL_FUNCTIONS
/* additions: string functions [ag] */
static int to_lower (int c) {
  if ('A' <= c and c <= 'Z')
  /* the non magic version */
    return c + ('a' - 'A');
            /* ( c | 32) */
  return c;
}

#define URL_TOLOWER to_lower

static int str_equal (const char *sa, const char *sb) {
  const uchar *spa = (const uchar *) sa;
  const uchar *spb = (const uchar *) sb;
  for (; *spa == *spb; spa++, spb++)
    if (*spa == 0) return 1;

  return 0;
}

#define URL_STR_EQUAL str_equal

static char *url_strchr (const char *s, int c) {
  const char *sp = s;
  while (*sp != c) {
    if (*sp == 0) return 0x0;
    sp++;
  }

  return (char *) sp;
}

#define URL_STRCHR url_strchr

static char *url_strpbrk (const char *s, const char *accept) {
/* algorithm from dietlibc */
  for (; *s; s++)
    for (int i = 0; accept[i]; i++)
      if (*s == accept[i])
        return (char *) s;

  return 0x0;
}

#define URL_STRPBRK url_strpbrk

#endif /* URL_USE_INTERNAL_FUNCTIONS */

static const char* parse_url_strnchr( const char* str, size_t len, int ch )
{
  for( size_t i = 0; i < len; ++i )
    if( str[i] == ch )
      return &str[i];
  return 0x0;
}

static void* parse_url_alloc_mem( parse_url_ctx* ctx, size_t request_size )
{
  if( request_size > ctx->memleft )
    return 0;
  void* res = (char*)ctx->mem + ctx->memsize - ctx->memleft;
  ctx->memleft -= request_size;
  return res;
}

static unsigned int parse_url_default_port_for_scheme( const char* scheme )
{
  if( scheme == 0x0 )
    return 0;

  if( URL_STR_EQUAL( scheme, "http"   )) return 80;
  if( URL_STR_EQUAL( scheme, "https"  )) return 443;
  if( URL_STR_EQUAL( scheme, "ftp"    )) return 21;
  if( URL_STR_EQUAL( scheme, "ssh"    )) return 22;
  if( URL_STR_EQUAL( scheme, "telnet" )) return 23;
  return 0x0;
}

static char* parse_url_alloc_string( parse_url_ctx* ctx, const char* src, size_t len)
{
  char* dst = (char*)parse_url_alloc_mem( ctx, len + 1 );
  if( dst == 0x0 )
    return 0x0;
  memcpy( dst, src, len );
  dst[len] = '\0';
  return dst;
}

static const char* parse_url_alloc_lower_string( parse_url_ctx* ctx, const char* src, size_t len)
{
  char* dst = (char*)parse_url_alloc_mem( ctx, len + 1 );
  if( dst == 0x0 )
    return 0x0;
  // parse_url_strncpy_lower( new_str, src, len );
  for( size_t i = 0; i < len; ++i )
    dst[i] = (char)URL_TOLOWER( src[i] );
  dst[len] = '\0';
  return dst;
}

static const char* parse_url_parse_scheme( const char* url, parse_url_ctx* ctx, parsed_url* out )
{
  const char* schemesep = URL_STRCHR( url, ':' );
  if( schemesep == 0x0 )
    return url;
  else
  {
    // ... is this the user part of a user/pass pair or the separator host:port? ...
    if( schemesep[1] != '/')
      return url;

    if( schemesep[2] != '/' )
      return 0x0;

    out->scheme = parse_url_alloc_lower_string( ctx, url, (size_t)( schemesep - url ) );
    if(out->scheme == 0x0)
      return 0x0;
    return &schemesep[3];
  }
}

static const char* parse_url_parse_user_pass( const char* url, parse_url_ctx* ctx, parsed_url* out )
{
  const char* atpos = URL_STRCHR( url, '@' );
  if( atpos != 0x0 )
  {
    // ... check for a : before the @ ...
    const char* passsep = parse_url_strnchr( url, (size_t)( atpos - url ), ':' );
    if( passsep == 0 )
    {
      out->pass = "";
      out->user = parse_url_alloc_string( ctx, url, (size_t)( atpos - url ) );
    }
    else
    {
      size_t userlen = (size_t)(passsep - url);
      size_t passlen = (size_t)(atpos - passsep - 1);
      out->user = (char*)parse_url_alloc_string( ctx, url, userlen );
      out->pass = (char*)parse_url_alloc_string( ctx, passsep + 1, passlen );
    }

    if(out->user == 0x0 || out->pass == 0x0)
      return 0x0;

    return atpos + 1;
  }

  return url;
}

static bool parse_url_is_hex_char( char c )
{
  return (c >= 'a' && c <= 'f') ||
       (c >= 'A' && c <= 'F') ||
       (c >= '0' && c <= '9');
}

static char parse_url_hex_char_value( char c )
{
  if(c >= '0' && c <= '9') return c - '0';
  if(c >= 'a' && c <= 'f') return c - 'a' + 10;
  if(c >= 'A' && c <= 'F') return c - 'A' + 10;
  return 0;
}

static char* parse_url_unescape_percent_encoding( char* str )
{
  char* read  = str;
  char* write = str;

  while(*read)
  {
    if(*read == '%')
    {
      ++read;
      if(!parse_url_is_hex_char(*read))
        return 0x0;
      char v1 = parse_url_hex_char_value(*read); 

      ++read;
      if(!parse_url_is_hex_char(*read))
        return 0x0;

      char v2 = parse_url_hex_char_value(*read);

      *write = (char)((v1 << 4) | v2);
    }
    else
    {
      *write = *read;
    }
    ++read;
    ++write;
  }
  *write = '\0';
  return str;
}

static const char* parse_url_parse_host_port( const char* url, parse_url_ctx* ctx, parsed_url* out )
{
  out->port = parse_url_default_port_for_scheme( out->scheme );

  size_t hostlen = 0;
  const char* ipv6_end = 0x0;

  if(url[0] == '[')
  {
    // ipv6 host is always enclosed in a [] to handle the : in an ipv6 address.
    ipv6_end = URL_STRCHR( url, ']' );
    if(ipv6_end == 0x0)
      return 0x0;
  }

  const char* portsep = URL_STRCHR( ipv6_end ? ipv6_end + 1 : url, ':' );
  const char* pathsep = URL_STRCHR( ipv6_end ? ipv6_end + 1 : url, '/' );

  if( portsep == 0x0 )
  {
    hostlen = pathsep == 0x0 ? URL_STRLEN( url ) : (size_t)( pathsep - url );
  }
  else
  {
    if(pathsep && portsep && (pathsep < portsep))
    {
      // ... path separator was before port-separator, i.e. the : was not a port-separator! ...
      hostlen = (size_t)( pathsep - url );
    }
    else
    {
      out->port = (unsigned int)atoi( portsep + 1 );
      hostlen = (size_t)( portsep - url );
      pathsep = URL_STRCHR( portsep, '/' );
    }
  }

  if( hostlen > 0 )
  {
    if(ipv6_end)
    {
      // ... we have an ipv6 host, we need to strip of the []
      out->host = parse_url_alloc_lower_string( ctx, url + 1, hostlen - 2 );

      // ... verify that the host is actually a valid ipv6 address... I guess this
      //     might miss one or two checks.
      //     this only checks that it contains numbers or hex-chars or : or .

      for(const char* c = out->host; *c; ++c)
      {
        bool valid = parse_url_is_hex_char(*c) ||
               (*c == ':') ||
               (*c == '.');
        if(!valid)
          return 0x0;
      }
    }
    else
      out->host = parse_url_alloc_lower_string( ctx, url, hostlen );
    if(out->host == 0x0)
      return 0x0;
  }

  // ... parse path ... TODO: extract to own function.
  if( pathsep != 0x0 )
  {
    // ... check if there are any query or fragment to parse ...
    const char* path_end = URL_STRPBRK(pathsep, "?#");

    size_t reslen = 0;
    if(path_end)
      reslen = (size_t)(path_end - pathsep);
    else
      reslen = URL_STRLEN( pathsep );

    char* path = parse_url_alloc_string( ctx, pathsep, reslen );
    if(path == 0x0)
      return 0x0;

    out->path = parse_url_unescape_percent_encoding(path);
    if(out->path == 0x0)
      return 0x0;

    return pathsep + reslen;
  }

  return url;
}

static const char* parse_url_parse_query( const char* url, parse_url_ctx* ctx, parsed_url* out )
{
  // ... do we have a query? ...
  if(*url != '?')
    return url;

  // ... skip '?' ...
  ++url;

  // ... find the end of the query ...
  size_t query_len = 0;

  const char* fragment_start = URL_STRCHR(url, '#');
  if(fragment_start)
    query_len = (size_t)(fragment_start - url);
  else
    query_len = URL_STRLEN(url);

  out->query = parse_url_alloc_string( ctx, url, query_len );
  return out->query == 0x0
        ? 0x0
        : url + query_len;
}

static const char* parse_url_parse_fragment( const char* url, parse_url_ctx* ctx, parsed_url* out )
{
  // ... do we have a fragment? ...
  if(*url != '#')
    return url;

  // ... skip '#' ...
  ++url;

  size_t frag_len = URL_STRLEN(url);
  out->fragment = parse_url_alloc_string( ctx, url, frag_len );

  return out->fragment == 0x0
        ? 0x0
        : url + frag_len;
}

#define URL_PARSE_FAIL_IF( x ) \
  if( x )                      \
  {                            \
    if( usermem == 0x0 )       \
      free( mem );             \
    return 0x0;                \
  }

static size_t parse_url_calc_mem_usage( const char* url )
{
  return sizeof( parsed_url ) + URL_STRLEN( url ) + 7; // 7 == max number of '\0' terminate
}

static parsed_url* parse_url( const char* url, void* usermem, size_t mem_size )
{
  void* mem = usermem;
  if( mem == 0x0 )
  {
    mem_size = parse_url_calc_mem_usage( url );
    mem = URL_ALLOC( mem_size );
    if (!mem) return 0x0;
  }

  parse_url_ctx ctx = {mem, mem_size, mem_size};

  parsed_url* out = (parsed_url*)parse_url_alloc_mem( &ctx, sizeof( parsed_url ) );
  URL_PARSE_FAIL_IF( out == 0x0 );

  // ... set default values ...
  memset(out, 0x0, sizeof(parsed_url));
  out->host = "localhost";
  out->path = "/";

  url = parse_url_parse_scheme   ( url, &ctx, out ); URL_PARSE_FAIL_IF( url == 0x0 );
  url = parse_url_parse_user_pass( url, &ctx, out ); URL_PARSE_FAIL_IF( url == 0x0 );
  url = parse_url_parse_host_port( url, &ctx, out ); URL_PARSE_FAIL_IF( url == 0x0 );
  url = parse_url_parse_query    ( url, &ctx, out ); URL_PARSE_FAIL_IF( url == 0x0 );
  url = parse_url_parse_fragment ( url, &ctx, out ); URL_PARSE_FAIL_IF( url == 0x0 );

  return out;
}

#ifndef URL_EXTENTED

__attribute__((visibility ("default")))
url_t *UrlParse (void *mem, size_t mem_size, int flags, char *url) {
  (void) flags;
  if (!url) return 0x0;
  return parse_url (url, mem, mem_size);
}

#else
#include <stdio.h>
__attribute__((visibility ("default")))
url_t *UrlParse (void *mem, size_t mem_size, int flags, char *url) {
  if (NULL == url) return 0x0;

  url_t *parsedURL = parse_url (url, mem, mem_size);

  if (!parsedURL) return 0x0;

  if (flags & URL_PRINT) {
    fprintf (stdout, "Protocol: %s\n", parsedURL->scheme);
    fprintf (stdout, "  Domain: %s\n", parsedURL->host);
    fprintf (stdout, "    Port: %u\n", parsedURL->port);
    fprintf (stdout, "Fragment: %s\n", parsedURL->fragment);
    fprintf (stdout, "   Query: %s\n", parsedURL->query);
    fprintf (stdout, "    Path: %s\n", parsedURL->path);
    fprintf (stdout, "    User: %s\n", parsedURL->user);
    if (flags & URL_PRINT_PASS)
      fprintf (stdout, "Passwd: %s\n", parsedURL->pass);
  }

  if (flags & URL_RELEASE)
    free (parsedURL);
    parsedURL = 0x0;
  }

  return parsedURL;
}
#endif /* URL_EXTENTED */
