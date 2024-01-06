#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_SOCKET
#define REQUIRE_SYS_TYPES
#define REQUIRE_ARPA_INET
#define REQUIRE_NETINET_IN
#define REQUIRE_NETDB

#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_ERROR_TYPE   DECLARE
#define REQUIRE_NET_TYPE     DONOT_DECLARE
#define REQUIRE_URL_TYPE

#include <z/cenv.h>

#include "HttpStatusCodes_C.h"

#define NET_PROTO_NONE       0
#define NET_PROTO_HTTP_TYPE  1
#define NET_PROTO_HTTPS_TYPE 2

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

struct openSSL {
  SSL *conn;
  SSL_CTX *ctx;
};

#define MAXLEN_IP 255

struct net_t {
  char *url;
  char *outputFile;

  char errorMsg[MAXLEN_ERROR_MSG + 1];
  char ip[MAXLEN_IP + 1];

  int outputToFile;
  int statusCode;
  int socketFD;
  int protoType; // is better fitted in url.h, and when setting default port num
  int verbose;
  int debug;

  NetOutputCallback outputCallback;
  int outputToCallback;

  void *userData;

  url_t *parsedURL;

  int contentLength;

  SSL *sslConnection;

  struct hostent *he;
  struct sockaddr_in server_addr;
};

static void net_clear (net_t *);
static int net_fetch (net_t *, char *);

static struct openSSL *init_openssl (net_t *this, struct openSSL *openssl) {
  OpenSSL_add_all_algorithms ();
  SSL_load_error_strings ();
  SSL_library_init ();

  //SSL_CTX *ctx = SSL_CTX_new (SSLv23_client_method ());
  SSL_CTX *ctx = SSL_CTX_new (TLS_client_method ());
  SSL* conn = SSL_new (ctx);
  SSL_set_fd (conn, this->socketFD);

  int err = SSL_connect (conn);

  if (err <= 0) {
    int sslerr = SSL_get_error (conn, err);
    if (sslerr is SSL_ERROR_SSL) {
      char msg[1024];
      ERR_error_string_n(ERR_get_error(), msg, sizeof(msg));
      Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "SSL: could not connect, %s", msg);
    } else
      Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "SSL: could not connect, %d", SSL_get_error (conn, err));

    fprintf (stderr, "%s\n", this->errorMsg);
    return NULL;
  }

  if (this->debug)
    fprintf (stdout, "Connected\n");

  openssl->ctx = ctx;
  openssl->conn = conn;
  return openssl;
}

static int net_parse_url (net_t *this) {
  url_t *parsedURL = UrlParse (0x0, 0x0, 0, this->url);

  ifnot (parsedURL) {
    Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
      "failed to parse url, %s\n", this->url);
    return NOTOK;
  }

  if (Cstring.eq (parsedURL->scheme, "http"))
    this->protoType = NET_PROTO_HTTP_TYPE;
  else if (Cstring.eq (parsedURL->scheme, "https"))
    this->protoType = NET_PROTO_HTTPS_TYPE;

  this->parsedURL = parsedURL;

  if (this->debug) {
    fprintf (stdout, "Protocol: %s\n", parsedURL->scheme);
    fprintf (stdout, "  Domain: %s\n", parsedURL->host);
    fprintf (stdout, "Fragment: %s\n", parsedURL->fragment);
    fprintf (stdout, "   Query: %s\n", parsedURL->query);
    fprintf (stdout, "    Path: %s\n", parsedURL->path);
    fprintf (stdout, "  Output: %s\n", this->outputFile);
  }

  return OK;
}

static int net_assign_output_filename (net_t *this) {
  if (this->outputToFile and NULL is this->outputFile) {
    size_t len = bytelen (this->parsedURL->path);
    if ((1 is len and *this->parsedURL->path is '/') or
         0 is len) {
      Cstring.cp (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "nothing to fetch because of empty path in the url", MAXLEN_ERROR_MSG);
      return NOTOK;
    }

    char *fname = (char *) this->parsedURL->path + len;
    while (fname > this->parsedURL->path and *fname isnot '/') fname--;
    if (*fname is '/') fname++;
    this->outputFile = Cstring.dup (fname, len - (fname - this->parsedURL->path));
    return OK;
  }

  if (NULL is this->outputFile and NULL is this->outputCallback) {
    Cstring.cp (this->errorMsg, MAXLEN_ERROR_MSG + 1,
      "neither an output filename or a callback function has been supplied", MAXLEN_ERROR_MSG);
    return NOTOK;
  }

  return OK;
}

static int net_assign_url (net_t *this, const char *url) {
  if (NULL is url) return NOTOK;
  size_t len = bytelen (url);
  ifnot (len) return NOTOK;

  if (this->url isnot NULL) free (this->url);

  this->url = Cstring.dup (url, len);
  return OK;
}

static int net_new_socket (net_t *this, int port) {
  int sockfd;

  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) is -1){
    Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
       "Fail to create socket creation, %s", Error.errno_string (errno));
    fprintf (stderr, "%s\n", this->errorMsg);
    return NOTOK;
  }

  this->socketFD = sockfd;

  struct hostent *he;

  he = gethostbyname (this->parsedURL->host);

  if (he is NULL) {
    Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "%s, Unreachable or wrong URL, %s", this->url, Error.errno_string (errno));
    fprintf (stderr, "%s\n", this->errorMsg);
    return NOTOK;
  }

  this->he = he;

  struct in_addr **addr_list = (struct in_addr **) he->h_addr_list;

  for (int i = 0; addr_list[i] != NULL; i++) {
    char *ip = inet_ntoa (*(addr_list[i]));
    Cstring.cp (this->ip, MAXLEN_IP + 1, ip, MAXLEN_IP);
    break;
   }

  if (this->debug)
    fprintf (stdout, "     Ip: %s\n", this->ip);

  struct sockaddr_in server_addr;
  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons (port);
  server_addr.sin_addr = *((struct in_addr *) he->h_addr_list[0]);
  this->server_addr = server_addr;

  return OK;
}

static int net_connect_to_socket (net_t *this) {
  if (this->socketFD is -1) return NOTOK;

  if (this->debug)
    fprintf (stdout, "Connecting ...\n");

  if (connect (this->socketFD, (struct sockaddr *) &this->server_addr, sizeof(struct sockaddr)) is -1){
    Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
      "failed to connect to socket, %s", Error.errno_string (errno));
    fprintf (stderr, "%s\n", this->errorMsg);
    return NOTOK;
  }

  return OK;
}

static int net_read_statusCode (net_t *this) {
  char buf[MAXLEN_STATUS_MSG];

  char *ptr = buf + 1; // advance the pointer, so we can perfom the check with safety

  int is_ssl = this->sslConnection isnot NULL;
  int nread = 0;

  while (1) {
    ifnot (is_ssl)
      nread = recv (this->socketFD, ptr, 1, 0);
    else
      nread = SSL_read (this->sslConnection, ptr, 1);

    ifnot (nread) break;

    if (nread is -1) {
      ifnot (is_ssl) {
        Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
          "error while reading http status, %s", Error.errno_string (errno));
      } else {
        char msg[1024];
        ERR_error_string_n(ERR_get_error(), msg, sizeof(msg));
        Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "error while reading https status, %s", msg);
      }

      fprintf (stderr, "%s\n", this->errorMsg);
      return -1;
    }

    if (*(ptr - 1) is '\r' and *ptr is '\n') break;

    ptr++;
  }

  *(ptr + 1) = '\0';

  int bytes_received = ptr - buf;

  ifnot (bytes_received) return 0;

  if (this->debug) {
    size_t len = bytelen (buf + 1) - 2;
    char out[len + 1];
    Cstring.cp (out, len + 1, buf + 1, len);
    fprintf(stdout, "Getting status code ...\n%s\n", out);
  }

  ptr = buf + 1;

  this->statusCode = 0;

  if (EOF is sscanf (ptr, "%*s %d ", &this->statusCode)) {
    Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
      "error while scanning HTTP header, %s", Error.errno_string (errno));
    fprintf (stderr, "%s\n", this->errorMsg);
    return 0;
  }

  if (this->debug)
    fprintf (stdout, "StatusCode: %d\n", this->statusCode);

  return this->statusCode;
}

static long net_parse_header (net_t *this) {
  char buf[MAXLEN_HEADER_MSG];
  char *ptr = buf + 4;

  int is_ssl = this->sslConnection isnot NULL;
  int nread = 0;

  while (1) {
    ifnot (is_ssl)
      nread = recv (this->socketFD, ptr, 1, 0);
    else
      nread = SSL_read (this->sslConnection, ptr, 1);

    ifnot (nread) break;

    if (nread is -1) {
      ifnot (is_ssl)
        Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
            "error while parsing HTTP header, %s", Error.errno_string (errno));
      else
        Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
          "error while parsing HTTPS header, %d",
            SSL_get_error (this->sslConnection, nread));

      return -1;
    }

    if (*(ptr - 3) is '\r' and *(ptr - 2) is '\n' and
        *(ptr - 1) is '\r' and *ptr  is '\n')
      break;

    ptr++;
  }

  *(ptr + 1) = '\0';

  if (this->debug) {
    size_t len = bytelen (buf + 4) - 4;
    char out[len + 1];
    Cstring.cp (out, len + 1, buf + 4, len);
    fprintf(stdout, "Getting headers ...\n%s\n", out);
  }

  long bytes_received = ptr - buf - 3;

  ifnot (bytes_received) return 0;

  ptr = Cstring.bytes_in_str (buf + 4, "Content-Length:");
  if (NULL is ptr) return -1;

  sscanf (ptr, "%*s %ld", &bytes_received);

  if (this->outputToFile) {
    const char disp[] = "content-disposition: attachment; filename=";
    ptr = Cstring.bytes_in_str (buf + 4, disp);
    ifnot (NULL is ptr) {
      ifnot (NULL is this->outputFile) free (this->outputFile);
      char *fname = ptr + sizeof (disp) - 1;
      ptr = fname;
      while (*(ptr + 1) isnot '\r') ptr++;
      size_t len = ptr - fname + 1;
      this->outputFile = Alloc (len + 1);
      Cstring.cp (this->outputFile, len + 1, fname, len);
      if (this->debug)
        fprintf (stdout, "Saving as %s\n", this->outputFile);
    }
  }

  return bytes_received ;
}

static int net_send_request (net_t *this, const char *path, const char *host) {
  char sendb[4096];
  Cstring.cp_fmt (sendb, sizeof (sendb),
      "GET %s%s HTTP/1.1\r\n"
      "Host: %s\r\nUser-Agent: libnet\r\n"
      "Accept-Language: en-us\r\n"
      "Accept-Charset: ISO-8859-1,utf-8\r\n"
      "Accept: */*\r\n"
      "Connection: Keep-Alive\r\n"
      "Accept-Encoding: identity\r\n\r\n",
      *path == '/' ? "" : "/", path, host);

 //   "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n"
 //   "Connection: close"

  if (this->debug) {
    size_t len = bytelen (sendb) - 4;
    char buf[len + 1];
    Cstring.cp (buf, len + 1, sendb, len);
    fprintf(stdout, "Sending request ...\n%s\n", buf);
  }

  int is_ssl = this->sslConnection isnot NULL;
  int err = OK;

  if (is_ssl) {
    if ((err = SSL_write (this->sslConnection, sendb, bytelen (sendb))) is -1) {
      char msg[1024];
      ERR_error_string_n(ERR_get_error(), msg, sizeof(msg));
      Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "Failed to send request, %s", msg);
    }

  } else {
    if ((err = send (this->socketFD, sendb, bytelen (sendb), 0)) is -1) {
      Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "Failed to send request, %s", Error.errno_string (errno));
    }
  }

  if (err is -1)
    fprintf (stderr, "%s\n", this->errorMsg);

  return err;
}

static int net_handle_badrequest (net_t *this) {
  size_t path_len  = bytelen (this->parsedURL->path);
  ifnot (path_len) return NOTOK;
  size_t query_len = bytelen (this->parsedURL->query);
  ifnot (query_len) return NOTOK;
  char path[path_len + query_len + 2];
  char *ptr = Cstring.bytes_in_str (this->url, this->parsedURL->query);
  char sep = *(ptr - 1);
  Cstring.cp_fmt (path, path_len + query_len + 2, "%s%c%s",
    this->parsedURL->path, sep, this->parsedURL->query);

  return net_send_request (this, path, this->parsedURL->host);
}

static int net_parse_redirection_url (net_t *this) {
  char buf[MAXLEN_HEADER_MSG];
  char *ptr = buf + 4;

  int is_ssl = this->sslConnection isnot NULL;
  int nread = 0;

  while (1) {
    ifnot (is_ssl)
      nread = recv (this->socketFD, ptr, 1, 0);
    else
      nread = SSL_read (this->sslConnection, ptr, 1);

    ifnot (nread) break;

    if (nread is -1) {
      ifnot (is_ssl)
        Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
            "error while parsing HTTP header, %s", Error.errno_string (errno));
      else
        Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
          "error while parsing HTTPS header, %s",
            SSL_get_error (this->sslConnection, nread));

      return -1;
    }

    if (*(ptr - 3) is '\r' and *(ptr - 2) is '\n' and
        *(ptr - 1) is '\r' and *ptr  is '\n')
      break;

    ptr++;
  }

  *(ptr + 1) = '\0';

  if (this->debug) {
    size_t len = bytelen (buf + 4) - 4;
    char out[len + 1];
    Cstring.cp (out, len + 1, buf + 4, len);
    fprintf(stdout, "Getting headers ...\n%s\n", out);
  }

  ptr = Cstring.bytes_in_str (buf + 4, "Location:");
  if (NULL is ptr) return NOTOK;

  char *location = ptr += 10;
  while (*(ptr + 1) isnot '\r') ptr++;
  size_t len = ptr - location + 1;
  char url[len + 1];
  Cstring.cp (url, len + 1, location, len);
  net_clear (this);
  return net_fetch (this, url);
}

static int net_fetch_from_http (net_t *this) {
  int retval = NOTOK;

  url_t *url = this->parsedURL;

  if (NOTOK is net_send_request (this, url->path, url->host))
    goto theend;

  int status = 0;
  ifnot ((status = net_read_statusCode (this))) goto theend;

  switch (status) {
    case HttpStatus_Found:
      return net_parse_redirection_url (this);

    case HttpStatus_BadRequest:
      if (NOTOK is net_handle_badrequest (this))
        goto theend;

      ifnot ((status = net_read_statusCode (this))) goto theend;
  }

  int contentLength = net_parse_header (this);
  ifnot (contentLength) goto theend;

  int num_bytes = 0;
  int nread;

  FILE* fp = NULL;

  ifnot (NULL is this->outputFile) {
    fp = fopen (this->outputFile, "wb");

    if (fp is NULL) {
      Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "%s, failed to open file, %s", this->outputFile, Error.errno_string (errno));
      goto theend;
    }
  }

  char recvb[4096];
  while ((nread = recv (this->socketFD, recvb, 1024, 0))) {
    if (nread is -1) {
      Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
          "Error receiving data, %s", Error.errno_string (errno));
      ifnot (NULL is fp) fclose (fp);
      goto theend;
    }

    ifnot (nread) break;

    if (this->verbose is NET_VERBOSE_LEVEL_ONE or this->outputToCallback) {
      ifnot (NULL is this->outputCallback) {
        if (NOTOK is this->outputCallback (this, fp, recvb, nread, num_bytes, contentLength)) {
          ifnot (NULL is fp)
            fclose (fp);
          retval = NOTOK;
          goto theend;
        }

        goto next;
      }
    }

    ifnot (NULL is this->outputFile)
      fwrite (recvb, 1, nread, fp);

next:
    num_bytes += nread;

    if (num_bytes is contentLength) break;
  }

  if (this->debug)
    fprintf (stdout, "Saving data\n");

  ifnot (NULL is this->outputFile)
    retval = fclose (fp);
  else
    retval = OK;

  if (retval is NOTOK)
    Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "%s, failed to close file, %s", this->outputFile, Error.errno_string (errno));

theend:
  return retval;
}

static int net_fetch_from_https (net_t *this) {
  int retval = NOTOK;

  struct openSSL openssl = {NULL, NULL};
  if (NULL is init_openssl (this, &openssl)) goto theend;
  this->sslConnection = openssl.conn;

  url_t *url = this->parsedURL;

  if (NOTOK is net_send_request (this, url->path, url->host))
    goto theend;

  int status = 0;

  ifnot ((status = net_read_statusCode (this))) goto theend;

  switch (status) {
    case HttpStatus_Found:
      return net_parse_redirection_url (this);

    case HttpStatus_BadRequest:
      if (NOTOK is net_handle_badrequest (this))
        goto theend;

      ifnot ((status = net_read_statusCode (this))) goto theend;
  }

  long contentLength = net_parse_header (this);
  if (contentLength <= 0) goto theend;

  FILE* fp = NULL;

  ifnot (NULL is this->outputFile) {
    fp = fopen (this->outputFile, "wb");

    if (fp is NULL) {
      Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "%s, failed to open file, %s", this->outputFile, Error.errno_string (errno));
      goto theend;
    }
  }

  long num_bytes = 0;
  int nread;

  char recvb[4096];

  while ((nread = SSL_read (this->sslConnection, recvb, 3000))) {
    if (nread is -1) {
      char msg[1024];
      ERR_error_string_n(ERR_get_error(), msg, sizeof(msg));
      Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "Error receiving data, %s", msg);

      ifnot (NULL is fp) fclose (fp);
      goto theend;
    }

    ifnot (nread) break;

    if (this->verbose is NET_VERBOSE_LEVEL_ONE or this->outputToCallback) {
      ifnot (NULL is this->outputCallback) {
        if (NOTOK is this->outputCallback (this, fp, recvb, nread, num_bytes, contentLength)) {
           ifnot (NULL is fp)
             fclose (fp);
           retval = NOTOK;
           goto theend;
        }

        goto next;
      }
    }

    ifnot (NULL is this->outputFile)
      fwrite (recvb, 1, nread, fp);

next:
    num_bytes += nread;
    memset (recvb, 0, 3000);
    if (num_bytes is contentLength) break;
  }

  if (this->debug)
    fprintf (stdout, "Saving data\n");

  ifnot (NULL is this->outputFile)
    retval = fclose (fp);
  else
    retval = OK;

  if (retval is -1)
    Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "%s, failed to close file, %s", this->outputFile, Error.errno_string (errno));

theend:
  ifnot (NULL is this->sslConnection)
    SSL_free (this->sslConnection);

  ifnot (NULL is openssl.ctx)
    SSL_CTX_free (openssl.ctx);

  return retval;
}

static int net_fetch (net_t *this, char *url) {
  int retval = NOTOK;

  if (NOTOK is net_assign_url (this, url)) goto theend;

  if (NOTOK is net_parse_url (this)) goto theend;

  if (this->protoType is NET_PROTO_NONE) goto theend;

  if (NOTOK is net_assign_output_filename (this)) goto theend;

  if (NOTOK is net_new_socket (this, this->parsedURL->port)) goto theend;

  if (NOTOK is net_connect_to_socket (this)) goto theend;

  switch (this->protoType) {
    case NET_PROTO_HTTP_TYPE: retval = net_fetch_from_http (this);
      break;
    case NET_PROTO_HTTPS_TYPE: retval = net_fetch_from_https (this);
  }

theend:
  if (this->socketFD isnot -1) {
    close (this->socketFD);
    this->socketFD = -1;
  }

  return retval;
}

static void net_clear (net_t *this) {
  if (NULL is this) return;
  ifnot (NULL is this->url) {
    free (this->url); this->url = NULL;
  }

  ifnot (NULL is this->parsedURL) {
    free (this->parsedURL); this->parsedURL = NULL;
  }

  ifnot (NULL is this->outputFile) {
    free (this->outputFile); this->outputFile = NULL;
  }

  if (this->socketFD isnot -1) {
    close (this->socketFD);
    this->socketFD = -1;
  }
}

static void net_release (net_t **thisp) {
  net_t *this = *thisp;
  net_clear (this);
  free (this);
  *thisp = NULL;
}

static int net_default_output_callback (net_t *this, FILE *fp, char *recvb, size_t nread, size_t num_bytes, size_t contentLength) {
  (void) this;

  if (NULL is fp) return NOTOK;

   if (nread isnot fwrite (recvb, 1, nread, fp))
     return NOTOK;

   ulong fractiondownloaded = (ulong) (((ulong) (num_bytes + nread) * 100) / (ulong) contentLength);
   fprintf (stdout, "\033[?25l%zdKB (%lu%%) of %zdMB\r\033[?25h", (num_bytes + nread) / 1024, fractiondownloaded,
        contentLength / 1024);
   return OK;
}

static net_t *net_new (netOptions opts) {
  net_t *this = Alloc (sizeof (net_t));
  this->url = NULL;
  this->outputFile = (NULL is opts.outputFile
    ? NULL : Cstring.dup (opts.outputFile, bytelen (opts.outputFile)));
  this->outputToFile = opts.outputToFile;
  this->outputCallback = (NULL is opts.outputCallback ? net_default_output_callback : opts.outputCallback);
  this->outputToCallback = opts.outputToCallback;
  this->verbose = opts.verbose;
  this->debug = opts.debug;
  this->userData = opts.userData;
  this->statusCode = HttpStatus_OK;
  this->contentLength = 0;
  this->socketFD = -1;
  this->protoType = NET_PROTO_NONE;
  this->sslConnection = NULL;
  this->parsedURL = 0x0;
  return this;
}

static char *net_get_errorMsg (net_t *this) {
  return this->errorMsg;
}

static void *net_get_userData (net_t *this) {
  return this->userData;
}

static int net_get_statusCode (net_t *this) {
  return this->statusCode;
}

static const char *net_get_statusCodeAsString (net_t *this) {
  return HttpStatus_reasonPhrase (this->statusCode);
}

static void net_set_outputCallback (net_t *this, NetOutputCallback cb) {
  this->outputCallback = cb;
}

static void net_set_userData (net_t *this, void *user_data) {
  this->userData = user_data;
}

public net_T __init_net__ (void) {
  __INIT__ (cstring);
  __INIT__ (error);

  return (net_T) {
    .self = (net_self) {
      .fetch = net_fetch,
      .new = net_new,
      .release = net_release,
      .get = (net_get_self) {
        .errorMsg = net_get_errorMsg,
        .userData = net_get_userData,
        .statusCode = net_get_statusCode,
        .statusCodeAsString = net_get_statusCodeAsString
      },
      .set = (net_set_self) {
        .outputCallback = net_set_outputCallback,
        .userData = net_set_userData
      }
    }
  };
}

public void __deinit_net__ (net_T **thisp) {
  (void) thisp;
}
