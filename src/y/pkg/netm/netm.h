#ifndef NETM_HDR
#define NETM_HDR

typedef struct netm_t {
  string
    *iface,
    *iface_dir;
} netm_t;

netm_t *netm_init (const char *);
void netm_release (netm_t **);

#endif /* NETM_HDR */
