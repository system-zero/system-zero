#ifndef URL_HDR
#define URL_HDR

/* Just a bit of adjustment for this environment, not even one addition though.
 * This is one of the best code i've seen.
 * Also this is the first library, that is indepented from this environment.
 */

#ifndef URL_ALLOC
  #define URL_ALLOC(__v__) malloc (__v__)
  #include <stdlib.h>
#endif

#ifndef URL_USE_INTERNAL_FUNCTIONS
  #include <stdbool.h>
  #include <ctype.h>
  #define URL_TOLOWER tolower

  #include <string.h>
  #define URL_STRLEN strlen
  #define URL_STRCHR strchr
  #define URL_STR_EQUAL(_a_, _b_) (0 == strcmp (_a_, _b_))
  #define URL_STRPBRK strpbrk
  #define URL_MEMSET memset
  #define URL_MEMCPY memcpy
#endif

/*
 * Struct describing a parsed url.
 *
 * @example <scheme>://<user>:<pass>@<host>:<port>/<path>?<query>#<fragment>
 */

typedef struct url_t {
  /**
   * scheme part of url or 0x0 if not present.
   * @note the scheme will be lower-cased!
   */
  const char*  scheme;

  /**
   * user part of url or 0x0 if not present.
   */
  const char*  user;

  /**
   * password part of url or 0x0 if not present.
   */
  const char*  pass;

  /**
   * host part of url or "localhost" if not present.
   * if the host is an ipv6 address, i.e. enclosed by [] such as
   * http://[::1]/whoppa host will be the string in question.
   * It will also be verified that it is a valid ipv6 address, parsing
   * will have failed if anything that is not an ipv6 address was found
   * within a []
   * @note the scheme will be lower-cased!
   */
  const char*  host;

  /**
   * port part of url.
   * if not present a default depending on scheme is used, if no default is
   * available for scheme, 0 will be used.
   *
   * supported defaults:
   * "http"   - 80
   * "https"  - 443
   * "ftp"    - 21
   * "ssh"    - 22
   * "telnet" - 23
   */
  unsigned int port;

  /**
   * path part of url.
   * if the path part of the url is not present, it will default to "/"
   * @note percent-encoded values will get decoded during parse, i.e. %21 will be translated
   *       to '!' etc.
   *       see: https://en.wikipedia.org/wiki/Percent-encoding
   */
  const char*  path;

  /**
   * query part of url, default to 0x0 if not present in url.
   * as this is not standardized it is not parsed for the user.
   */
   const char*  query;

  /**
   * fragment part of url, default to 0x0 if not present in url.
   */
   const char*  fragment;
} url_t;

/* a tiny wrapper */

#ifdef URL_EXTENTED
  #define URL_PRINT      (1 << 0)
  #define URL_PRINT_PASS (1 << 1)
  #define URL_RELEASE    (1 << 2)
#endif

__attribute__((visibility ("default")))
  url_t *UrlParse (void *, size_t, int flags, char *);

#endif /* URL_HDR */
