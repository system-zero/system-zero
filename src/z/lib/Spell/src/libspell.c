#define REQUIRE_STDIO
#define REQUIRE_UNISTD

#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_VSTRING_TYPE  DECLARE
#define REQUIRE_USTRING_TYPE  DECLARE
#define REQUIRE_IMAP_TYPE     DECLARE
#define REQUIRE_FILE_TYPE     DECLARE
#define REQUIRE_ERROR_TYPE    DECLARE
#define REQUIRE_SPELL_TYPE    DONOT_DECLARE

#include <z/cenv.h>

spell_T __SPELL__;

/* The algorithms for transforming the `word', except the case handling,
 * are based on the checkmate_spell project at:
 * https://github.com/syb0rg/checkmate
 *
 * Almost same code at: https://github.com/marcelotoledo/spelling_corrector
 *
 * Copyright  (C)  2007  Marcelo Toledo <marcelo@marcelotoledo.com>
 *
 * Version: 1.0
 * Keywords: spell corrector
 * Author: Marcelo Toledo <marcelo@marcelotoledo.com>
 * Maintainer: Marcelo Toledo <marcelo@marcelotoledo.com>
 * URL: http://marcelotoledo.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

 /* The original idea is from Peter Norvig at: http://norvig.com/ */

 /* The word database i used for a start, is from:
  * https://github.com/first20hours/google-10000-english
  * Data files are derived from the Google Web Trillion Word Corpus, as described
  * by Thorsten Brants and Alex Franz
  * http://googleresearch.blogspot.com/2006/08/all-our-n-gram-are-belong-to-you.html
  * and distributed by the Linguistic Data Consortium:
  * http://www.ldc.upenn.edu/Catalog/CatalogEntry.jsp?catalogId=LDC2006T13.
  * Subsets of this corpus distributed by Peter Novig:
  * http://norvig.com/ngrams/
  * Corpus editing and cleanup by Josh Kaufman.
  */

static string_t *spell_get_dictionary (void) {
  return __SPELL__.dic_file;
}

static void spell_set_dictionary (char *file) {
  if (NULL isnot __SPELL__.dic_file)
    String.replace_with (__SPELL__.dic_file, file);
  else
    __SPELL__.dic_file = String.new_with (file);
}

static int spell_get_num_entries (void) {
  return __SPELL__.num_entries;
}

static spelldic_t *spell_get_current_dic (void) {
  return __SPELL__.current_dic;
}

static void spell_set_current_dic (spelldic_t *dic) {
  ifnot (dic is __SPELL__.current_dic)
    Imap.release (__SPELL__.current_dic);

  __SPELL__.current_dic = dic;
}

static void spell_clear (spell_t *spell, int clear_ignored) {
  if (NULL is spell) return;
  String.clear (spell->tmp);
  Vstring.clear (spell->words);
  Vstring.clear (spell->guesses);
  Vstring.clear (spell->messages);
  if (clear_ignored) Imap.clear (spell->ign_words);
}

static void spell_release (spell_t *spell, int clear_dic) {
  if (NULL is spell) return;
  String.release (spell->tmp);
  Vstring.release (spell->words);
  Vstring.release (spell->guesses);
  Vstring.release (spell->messages);

  Imap.release (spell->ign_words);

  if (SPELL_CLEAR_DICTIONARY is clear_dic) {
    Imap.release (spell->dic);
    spell_set_current_dic (NULL);
  } else
    spell_set_current_dic (spell->dic);

  free (spell);
}

static void spell_deletion (spell_t *spell) {
  for (size_t i = 0; i < spell->word_len; i++) {
    String.clear (spell->tmp);
    size_t ii = 0;
    for (; ii < i; ii++) String.append_byte (spell->tmp, spell->word[ii]);

    ssize_t len = spell->word_len - (i + 1);
    for (int i_ = 0; i_ < len; i_++)
      String.append_byte (spell->tmp, spell->word[++ii]);

    Vstring.add.sort_and_uniq (spell->words, spell->tmp->bytes);
  }
}

static void spell_transposition (spell_t *spell) {
  for (size_t i = 0; i < spell->word_len - 1; i++) {
    String.clear (spell->tmp);
    size_t ii = 0;
    for (; ii < i; ii++) String.append_byte (spell->tmp, spell->word[ii]);

    String.append_byte (spell->tmp, spell->word[i+1]);
    String.append_byte (spell->tmp, spell->word[i]);
    ii++;

    ssize_t len = spell->word_len - (i + 2);
    for (int i_ = 0; i_ <= len; i_++)
      String.append_byte (spell->tmp, spell->word[++ii]);

    Vstring.add.sort_and_uniq (spell->words, spell->tmp->bytes);
  }
}

static void spell_alteration(spell_t *spell) {
  for (size_t i = 0; i < spell->word_len; i++) {
    for (size_t j = 0; j < 26; j++) {
      String.clear (spell->tmp);
      size_t ii = 0;
      for (; ii < i; ii++) String.append_byte (spell->tmp, spell->word[ii]);

      String.append_byte (spell->tmp, 'a' + j);

      ssize_t len = spell->word_len - (i + 1);

      for (int i_ = 0; i_ < len; i_++)
        String.append_byte (spell->tmp, spell->word[++ii]);

      Vstring.add.sort_and_uniq (spell->words, spell->tmp->bytes);
    }
  }
}

static void spell_insertion (spell_t *spell) {
  for (size_t i = 0; i <= spell->word_len; i++) {
    for (size_t j = 0; j < 26; j++) {
      String.clear (spell->tmp);
      size_t ii = 0;
      for (; ii < i; ii++) String.append_byte (spell->tmp, spell->word[ii]);

      String.append_byte (spell->tmp, 'a' + j);
      for (size_t i_ = 0; i_ < spell->word_len - i; i_++)
        String.append_byte (spell->tmp, spell->word[i + i_]);

      Vstring.add.sort_and_uniq (spell->words, spell->tmp->bytes);
    }
  }
}

static int spell_case (spell_t *spell) {
  char buf[spell->word_len + 1];
  int retval = Ustring.change_case (buf, spell->word, spell->word_len, TO_LOWER);
  ifnot (retval) return SPELL_WORD_ISNOT_CORRECT;
  if (Imap.key_exists (spell->dic, buf)) return SPELL_WORD_IS_CORRECT;
  Vstring.add.sort_and_uniq (spell->words, buf);
  return SPELL_WORD_ISNOT_CORRECT;
}

static int spell_guess (spell_t *spell) {
  if (SPELL_WORD_IS_CORRECT is spell_case (spell))
    return SPELL_WORD_IS_CORRECT;

  spell_clear (spell, SPELL_DONOT_CLEAR_IGNORED);
  spell_deletion (spell);
  spell_transposition (spell);
  spell_alteration (spell);
  spell_insertion (spell);

  vstring_t *that = spell->words->head;
  while (that) {
    if (Imap.key_exists (spell->dic, that->data->bytes))
      Vstring.current.append_with (spell->guesses, that->data->bytes);
    that = that->next;
  }

  return SPELL_WORD_ISNOT_CORRECT;
}

static int spell_correct (spell_t *spell) {
  if (spell->word_len < spell->min_word_len or
      spell->word_len >= MAXLEN_WORD)
    return SPELL_WORD_IS_IGNORED;

  if (Imap.key_exists (spell->dic, spell->word)) return SPELL_WORD_IS_CORRECT;
  if (Imap.key_exists (spell->ign_words, spell->word)) return SPELL_WORD_IS_IGNORED;

  return spell_guess (spell);
}

static void spell_add_word_to_dictionary (spell_t *spell, char *word) {
  FILE *fp = fopen (spell->dic_file->bytes, "a+");
  if (NULL is fp) return;

  fprintf (fp, "%s\n", word);
  fclose (fp);
}

static int spell_file_readlines_cb (Vstring_t *unused, char *line, size_t len,
                                                         int lnr, void *obj) {
  (void) unused; (void) lnr; (void) len;
  spell_t *spell = (spell_t *) obj;
  Imap.set_with_keylen (spell->dic, Cstring.trim.end (line, '\n'));
                    // this untill an inner getline()
  spell->num_dic_words++;
  return 0;
}

static int spell_read_dictionary (spell_t *spell) {
  Imap.clear (spell->dic);
  Vstring_t unused;
  File.readlines (spell->dic_file->bytes, &unused, spell_file_readlines_cb,
      (void *) spell);
  return spell->retval;
}

static int spell_init_dictionary (spell_t *spell, string_t *dic, int num_words, int force) {
  if (NULL is dic) return SPELL_ERROR;

  spelldic_t *current_dic = spell_get_current_dic ();

  if (current_dic isnot NULL and 0 is force) {
    spell->dic = current_dic;
    spell->dic_file = dic;
    return SPELL_OK;
  }

  if (-1 is access (dic->bytes, F_OK|R_OK)) {
    spell->retval = SPELL_ERROR;
    Vstring.append_with_fmt (spell->messages,
        "dictionary is not readable: |%s|\n" "errno: %d, error: %s",
        dic->bytes, errno, Error.errno_string (errno));
    return spell->retval;
  }

  spell->dic_file = dic;
  spell->dic = Imap.new (num_words);
  spell_read_dictionary (spell);
  spell_set_current_dic (spell->dic);
  return SPELL_OK;
}

static spell_t *spell_new (void) {
  spell_t *spell = Alloc (sizeof (spell_t));
  spell->tmp = String.new_with ("");
  spell->words = Vstring.new ();
  spell->ign_words = Imap.new (100);
  spell->guesses = Vstring.new ();
  spell->messages = Vstring.new ();
  spell->min_word_len = SPELL_MIN_WORD_LEN;
  return spell;
}

public spell_T __init_spell__ (void) {
  __INIT__ (file);
  __INIT__ (imap);
  __INIT__ (error);
  __INIT__ (string);
  __INIT__ (cstring);
  __INIT__ (vstring);
  __INIT__ (ustring);

  __SPELL__ = (spell_T) {
    .self = (spell_self) {
      .new = spell_new,
      .clear = spell_clear,
      .correct = spell_correct,
      .release = spell_release,
      .init_dictionary = spell_init_dictionary,
      .add_word_to_dictionary = spell_add_word_to_dictionary,
      .get = (spell_get_self) {
        .dictionary = spell_get_dictionary,
        .num_entries = spell_get_num_entries
      },
      .set = (spell_set_self) {
        .dictionary = spell_set_dictionary
      }
    },
   .current_dic = NULL,
   .dic_file = NULL,
   .num_entries = 10000
  };

  return __SPELL__;
}

public void __deinit_spell__ (spell_T *this) {
  ifnot (NULL is this->current_dic)
    Imap.release (this->current_dic);

  String.release (this->dic_file);

  //if (__SPELL__ is *this) return;

  ifnot (NULL is __SPELL__.current_dic)
    Imap.release (__SPELL__.current_dic);

  String.release (__SPELL__.dic_file);
}
