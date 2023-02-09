
//struct sigset {
//  unsigned long word[2];
//};

#define _SIGSET_WORDS	(1024 / (8 * sizeof (unsigned long int)))

typedef struct sigset {
  unsigned long sig[_SIGSET_WORDS];
} sigset_t;
