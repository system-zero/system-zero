#define REQUIRE_ZALLOC
#define REQUIRE_FORMAT

#define tostdout(_fmt_, ...) format_to_fd (1, _fmt_, ##__VA_ARGS__)

#include <libc.h>

int main (int argc, char **argv) {
  (void) argv;
  int num_failed = 0;
  int num_dbgs = 0;
  int verbose = argc - 1;

  mem_init (1000);
  if (verbose) tostdout ("#### Init \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);

  char *s = (char *) Alloc (20);
  if (verbose) tostdout ("#### Alloc \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);

#ifdef MEMDBG
  tostdout ("is |s| who asked for 20 bts %p\n", mem_get_chunk_from_ptr (s));
  char *sp = s;
  for (int i = 0; i < 10; i++) {
    *sp++ = i + '0';
  }
  s[9] = '\0';
  tostdout ("|%s|\n", s);
#endif

  char *sa = Alloc (888);
  if (verbose) tostdout ("#### Alloc \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);

#ifdef MEMDBG
  tostdout ("is |sa| who asked for 20 bts %p\n", mem_get_chunk_from_ptr (sa));
  sp = sa;
  for (int i = 0; i < 10; i++) {
    *sp++ = i + 'a';
  }

  sa[9] = '\0';
  tostdout ("|%s|\n", sa);
#endif

//tostdout ("is |s| who ask for release\n", s - HEADER_SIZE + 1);
  Release (s);
  if (verbose) tostdout ("#### Release \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
//tostdout ("is |sa| who ask for release\n", sa - HEADER_SIZE + 1);
  Release (sa);
  if (verbose) tostdout ("#### Release \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);

  char *saa = Alloc (44);
  if (verbose) tostdout ("#### Alloc \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);

#ifdef MEMDBG
  tostdout ("is |saa| who asked for 20 bts %p\n", mem_get_chunk_from_ptr (saa));
  sp = saa;
  for (int i = 0; i < 10; i++) {
    *sp++ = i + 'A';
  }

  saa[9] = '\0';
  tostdout ("|%s|\n", saa);
#endif

  Release (saa);
  if (verbose) tostdout ("#### Release \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);

  char *o = Alloc (1);
  if (verbose) tostdout ("#### Alloc \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);

#ifdef MEMDBG
  sp = o;
  for (int i = 0; i < 10; i++) {
    *sp++ = 'a' + i;
  }

  o[9] = '\0';
  tostdout ("|%s|\n", o);
#endif

  Release (o);
  if (verbose) tostdout ("#### Release \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);

  o = Alloc (990);
  if (verbose) tostdout ("#### Alloc \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  s  = Alloc (1000);
  if (verbose) tostdout ("#### Alloc \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  Release (o);
  if (verbose) tostdout ("#### Release \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  sa = Alloc (3000);
  if (verbose) tostdout ("#### Alloc \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  Release (s);
  if (verbose) tostdout ("#### Release \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  o = Alloc (111);
  if (verbose) tostdout ("#### Alloc \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  Release (sa);
  if (verbose) tostdout ("#### Release \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  saa = Alloc (666);
  if (verbose) tostdout ("#### Alloc \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  Release (saa);
  if (verbose) tostdout ("#### Release \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  saa = Alloc (777);
  if (verbose) tostdout ("#### Alloc \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  Release (saa);
  if (verbose) tostdout ("#### Release \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  saa = Realloc (NULL, 777);
  if (verbose) tostdout ("#### Realloc \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  saa = Realloc (saa, 888);
  if (verbose) tostdout ("#### Realloc \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  saa = Realloc (saa, 188);
  if (verbose) tostdout ("#### Realloc \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  saa = Realloc (saa, 2);
tostdout ("here\n");
  if (verbose) tostdout ("#### Realloc \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  Release (saa);
  if (verbose) tostdout ("#### Release \033[%%33mDebug %d \033[m####\n", num_dbgs++);
  num_failed += mem_debug_all (verbose);
  if (num_failed)
    tostdout ("Failed tESTS %d\n", num_failed);
  else
    tostdout ("all ok\n");

  return 0;
}
