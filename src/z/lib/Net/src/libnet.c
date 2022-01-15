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

/* tested with libressl, it owes to work with openssl too */
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

  NetOutputCallback outputCallback;
  int outputToCallback;

  url_t *parsedURL;

  int contentLength;

  SSL *sslConnection;

  struct hostent *he;
  struct sockaddr_in server_addr;
};

static struct openSSL *init_openssl (net_t *this, struct openSSL *openssl) {
  OpenSSL_add_all_algorithms ();
  SSL_load_error_strings ();
  SSL_library_init ();

  SSL_CTX *ctx = SSL_CTX_new (SSLv23_client_method ());
  SSL* conn = SSL_new (ctx);
  SSL_set_fd (conn, this->socketFD);

  int err = SSL_connect (conn);

  if (err isnot 1) {
    Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
      "SSL: could not connect, %s", SSL_get_error (conn, err));
    return NULL;
  }

  if (this->verbose > NET_VERBOSE_LEVEL_ONE)
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

  #if 0
  if (this->verbose > NET_VERBOSE_LEVEL_TWO) {
    fprintf (stdout, "Protocol: %s\n", parsedURL->scheme);
    fprintf (stdout, "  Domain: %s\n", parsedURL->host);
    fprintf (stdout, "Fragment: %s\n", parsedURL->fragment);
    fprintf (stdout, "   Query: %s\n", parsedURL->query);
    fprintf (stdout, "    Path: %s\n", parsedURL->path);
    ifnot (NULL is this->outputFile)
      fprintf (stdout, "  Output: %s\n", this->outputFile);
  }
  #endif

  return OK;
}

static int net_assign_output_filename (net_t *this) {
  if (this->outputToFile and NULL is this->outputFile) {
    size_t len = bytelen (this->parsedURL->path);
    if ((1 is len and *this->parsedURL->path is '/') or
         0 is len) {
      Cstring.cp (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "nothing to fetch", MAXLEN_ERROR_MSG);
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
    return NOTOK;
  }

  this->socketFD = sockfd;

  struct hostent *he;

  he = gethostbyname (this->parsedURL->host);

  if (he is NULL) {
    Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "%s, Unreachable or wrong URL, %s", this->url, Error.errno_string (errno));
    return NOTOK;
  }

  this->he = he;

  struct in_addr **addr_list = (struct in_addr **) he->h_addr_list;

  for (int i = 0; addr_list[i] != NULL; i++) {
    char *ip = inet_ntoa (*(addr_list[i]));
    Cstring.cp (this->ip, MAXLEN_IP + 1, ip, MAXLEN_IP);
    break;
   }

  if (this->verbose > NET_VERBOSE_LEVEL_ONE)
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

  if (this->verbose > NET_VERBOSE_LEVEL_ONE)
    fprintf (stdout, "Connecting ...\n");

  if (connect (this->socketFD, (struct sockaddr *) &this->server_addr, sizeof(struct sockaddr)) is -1){
    Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
      "failed to connect to socket, %s", Error.errno_string (errno));
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
      ifnot (is_ssl)
        Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
          "error while reading HTTP status, %s", Error.errno_string (errno));
      else
        Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
          "error while reading HTTPS status, %s",
            SSL_get_error (this->sslConnection, nread));

      return -1;
    }

    if (*(ptr - 1) is '\r' and *ptr is '\n') break;

    ptr++;
  }

  *(ptr + 1) = '\0';

  int bytes_received = ptr - buf;

  if (this->verbose > NET_VERBOSE_LEVEL_TWO)
    fprintf (stdout, "HTTP%s: header bytes received: %d\n", (is_ssl ? "S" : ""),
       bytes_received);

  ifnot (bytes_received) return -1;

  ptr = buf + 1;

  this->statusCode = NOTOK;

  if (EOF is sscanf (ptr, "%*s %d ", &this->statusCode)) {
    Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
      "error while scanning HTTP header, %s", Error.errno_string (errno));
    return NOTOK;
  }

  if (this->verbose > NET_VERBOSE_LEVEL_THREE)
    fprintf (stdout, "%s, StatusCode: %d\nPtr\n|%s|\n", __func__, this->statusCode, ptr);

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

  long bytes_received = ptr - buf - 3;

  if (this->verbose > NET_VERBOSE_LEVEL_TWO)
    fprintf (stdout, "HTTP%s: header bytes received: %ld\n", (is_ssl ? "s" : ""),
       bytes_received);

  ifnot (bytes_received) return 0;

  ptr = Cstring.bytes_in_str (buf + 4, "Content-Length:");
  if (NULL is ptr) return -1;

  sscanf (ptr, "%*s %ld", &bytes_received);

  return bytes_received ;
}

static int net_fetch_from_http (net_t *this) {
  int retval = NOTOK;

  url_t *url = this->parsedURL;

  char sendb[3000];
  char recvb[1024];

  Cstring.cp_fmt (sendb, sizeof (sendb),
      "GET /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: libnet\r\n"
      "Accept: text/xml,application/xml,application/xhtml+xml,text/html*/*\r\n"
      "Accept-Language: en-us\r\n"
      "Accept-Charset: ISO-8859-1,utf-8\r\n"
      "Connection: close\r\n\r\n",
      url->path, url->host);

  if (this->verbose > NET_VERBOSE_LEVEL_ONE)
    fprintf (stdout, "Sending data ...\n");

  if(send (this->socketFD, sendb, bytelen (sendb), 0) is -1) {
    Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "Failed to send data, %s", Error.errno_string (errno));
    goto theend;
  }

  if (this->verbose > NET_VERBOSE_LEVEL_ONE) {
    fprintf (stdout, "Data sent\n");
    fprintf (stdout, "Receiving data ...\n");
  }

  ifnot (net_read_statusCode (this)) goto theend;

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

  while ((nread = recv (this->socketFD, recvb, 1024, 0))) {
    if (nread is -1) {
      Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
          "Error receiving data, %s", Error.errno_string (errno));
      ifnot (NULL is fp) fclose (fp);
      goto theend;
    }

    ifnot (NULL is this->outputFile) {
      fwrite (recvb, 1, nread, fp);
    } else
      this->outputCallback (this, NULL, recvb, nread);

    num_bytes += nread;

    if (num_bytes is contentLength) break;
  }

  if (this->verbose > NET_VERBOSE_LEVEL_ONE)
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

  url_t *url = this->parsedURL;

  if (NULL is init_openssl (this, &openssl)) goto theend;

  char sendb[3000];
  char recvb[3000];

  Cstring.cp_fmt (sendb, sizeof (sendb),
      "GET /%s HTTP/1.1\r\n"
      "Host: %s\r\nUser-Agent: libnet\r\n"
      "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n"
      "Accept-Language: en-us\r\n"
      "Accept-Charset: ISO-8859-1,utf-8\r\n"
      "Connection: close"
      "\r\n\r\n",
      url->path, url->host);

  this->sslConnection = openssl.conn;
  int err;

  if (this->verbose > NET_VERBOSE_LEVEL_ONE)
    fprintf (stdout, "Sending data ...\n");

  if ((err = SSL_write (this->sslConnection, sendb, bytelen (sendb))) is -1) {
    Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "SSL: Failed to send data, %s", SSL_get_error (this->sslConnection, err));
    goto theend;
  }

  if (this->verbose > NET_VERBOSE_LEVEL_ONE) {
    fprintf (stdout, "SSL: Data sent\n");
    fprintf (stdout, "SSL: Receiving data ...\n");
  }

  int status = 0;
  ifnot ((status = net_read_statusCode (this))) goto theend;

  if (this->verbose > NET_VERBOSE_LEVEL_ONE) {
    fprintf (stdout, "SSL status: %d\n", status);
  }

  long contentLength = net_parse_header (this);
  ifnot (contentLength) goto theend;

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

  while ((nread = SSL_read (this->sslConnection, recvb, 3000))) {
    if (nread is -1) {
      Cstring.cp_fmt (this->errorMsg, MAXLEN_ERROR_MSG + 1,
        "SSL: Error receiving data, %s",
        SSL_get_error (this->sslConnection, nread));

      ifnot (NULL is fp) fclose (fp);
      goto theend;
    }

    //fprintf (stdout, "nread: %d, total %ld contentlne %ld\n", nread, num_bytes,
    //    contentLength);
    ifnot (nread) break;

    ifnot (NULL is this->outputFile)
      fwrite (recvb, 1, nread, fp);
    else
      this->outputCallback (this, NULL, recvb, nread);

    num_bytes += nread;
    memset (recvb, 0, 3000);
    if (num_bytes is contentLength) break;
  }

  if (this->verbose > NET_VERBOSE_LEVEL_ONE)
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

static void net_release (net_t **thisp) {
  net_t *this = *thisp;
  if (NULL is this) return;

  ifnot (NULL is this->url) free (this->url);
  ifnot (NULL is this->parsedURL) free (this->parsedURL);
  ifnot (NULL is this->outputFile) free (this->outputFile);

  free (this);
  *thisp = NULL;
}

static net_t *net_new (netOptions opts) {
  net_t *this = Alloc (sizeof (net_t));
  this->url = NULL;
  this->outputFile = (NULL is opts.outputFile
    ? NULL : Cstring.dup (opts.outputFile, bytelen (opts.outputFile)));
  this->outputToFile = opts.outputToFile;
  this->outputCallback = opts.outputCallback;
  this->outputToCallback = opts.outputToCallback;
  this->verbose = NET_VERBOSE_LEVEL_THREE + 1;
  //opts.verbose;
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

static int net_get_statusCode (net_t *this) {
  return this->statusCode;
}

static const char *net_get_statusCodeAsString (net_t *this) {
  return HttpStatus_reasonPhrase (this->statusCode);
}

static void net_set_outputCallback (net_t *this, NetOutputCallback cb) {
  this->outputCallback = cb;
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
        .statusCode = net_get_statusCode,
        .statusCodeAsString = net_get_statusCodeAsString
      },
      .set = (net_set_self) {
        .outputCallback = net_set_outputCallback
      }
    }
  };
}

public void __deinit_net__ (net_T **thisp) {
  (void) thisp;
}
