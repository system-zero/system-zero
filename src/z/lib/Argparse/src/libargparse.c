#define LIBRARY "Argparse"

#define REQUIRE_STDIO
#define REQUIRE_STDARG

#define REQUIRE_STRING_TYPE   DONOT_DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_IO_TYPE       DECLARE
#define REQUIRE_ERROR_TYPE    DECLARE
#define REQUIRE_ARGPARSE_TYPE DONOT_DECLARE

#include <z/cenv.h>

/* Argparse:
  https://github.com/cofyc/argparse

  forked commit: fafc503d23d077bda40c29e8a20ea74707452721
  (HEAD -> master, origin/master, origin/HEAD)
*/

/**
 * Copyright (C) 2012-2015 Yecheng Fu <cofyc.jackson at gmail dot com>
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define OPT_UNSET 1
#define OPT_LONG  (1 << 1)

static const char *argparse_prefix_skip (const char *str, const char *prefix) {
  size_t len = bytelen (prefix);
  return Cstring.cmp_n (str, prefix, len) ? NULL : str + len;
}

static int argparse_error (argparse_t *self, const argparse_option_t *opt,
                                            const char *reason, int flags) {
  (void) self;
  if (flags & OPT_LONG) {
    Stderr.print_fmt ("error: option `--%s` %s\n", opt->long_name, reason);
  } else {
    Stderr.print_fmt("error: option `-%c` %s\n", opt->short_name, reason);
  }
  return -1;
}

static int argparse_getvalue (argparse_t *self, const argparse_option_t *opt, int flags) {
  const char *s = NULL;
  if (!opt->value)
    goto skipped;

  switch (opt->type) {
    case ARGPARSE_OPT_BOOLEAN:
      if (flags & OPT_UNSET) {
        *(int *)opt->value = *(int *)opt->value - 1;
      } else {
        *(int *)opt->value = *(int *)opt->value + 1;
      }
      if (*(int *)opt->value < 0) {
        *(int *)opt->value = 0;
      }
      break;

    case ARGPARSE_OPT_BIT:
      if (flags & OPT_UNSET) {
        *(int *)opt->value &= ~opt->data;
      } else {
        *(int *)opt->value |= opt->data;
      }
      break;

    case ARGPARSE_OPT_STRING:
      if (self->optvalue) {
        *(const char **)opt->value = self->optvalue;
        self->optvalue             = NULL;
      } else if (self->argc > 1) {
        self->argc--;
        *(const char **)opt->value = *++self->argv;
      } else {
        return argparse_error (self, opt, "requires a value", flags);
      }
      break;

    case ARGPARSE_OPT_INTEGER:
      errno = 0;
      if (self->optvalue) {
        *(int *)opt->value = strtol(self->optvalue, (char **)&s, 0);
        self->optvalue     = NULL;
      } else if (self->argc > 1) {
        self->argc--;
        *(int *)opt->value = strtol (*++self->argv, (char **)&s, 0);
      } else {
        return argparse_error (self, opt, "requires a value", flags);
      }
      if (errno)
        return argparse_error (self, opt, Error.errno_string (errno), flags);
      if (s[0] != '\0')
        return argparse_error (self, opt, "expects an integer value", flags);
      break;

    case ARGPARSE_OPT_FLOAT:
      errno = 0;
      if (self->optvalue) {
        *(float *)opt->value = strtof (self->optvalue, (char **)&s);
        self->optvalue       = NULL;
      } else if (self->argc > 1) {
        self->argc--;
        *(float *)opt->value = strtof (*++self->argv, (char **)&s);
      } else {
        return argparse_error (self, opt, "requires a value", flags);
        }
      if (errno)
        return argparse_error (self, opt, Error.errno_string (errno), flags);
      if (s[0] != '\0')
        return argparse_error (self, opt, "expects a numerical value", flags);
      break;

    default:
      exit (0);
    }

skipped:
  if (opt->callback) {
    return opt->callback(self, opt);
  }

  return 0;
}

static void argparse_options_check (const argparse_option_t *options) {
  for (; options->type != ARGPARSE_OPT_END; options++) {
    switch (options->type) {
    case ARGPARSE_OPT_END:
    case ARGPARSE_OPT_BOOLEAN:
    case ARGPARSE_OPT_BIT:
    case ARGPARSE_OPT_INTEGER:
    case ARGPARSE_OPT_FLOAT:
    case ARGPARSE_OPT_STRING:
    case ARGPARSE_OPT_GROUP:
      continue;
    default:
      Stderr.print_fmt ("wrong option type: %d", options->type);
      break;
    }
  }
}

static int argparse_short_opt (argparse_t *self, const argparse_option_t *options) {
  for (; options->type != ARGPARSE_OPT_END; options++) {
    if (options->short_name == *self->optvalue) {
      self->optvalue = self->optvalue[1] ? self->optvalue + 1 : NULL;
      return argparse_getvalue (self, options, 0);
    }
  }
  return -2;
}

static int argparse_long_opt (argparse_t *self, const argparse_option_t *options) {
  for (; options->type != ARGPARSE_OPT_END; options++) {
    const char *rest;
    int opt_flags = 0;
    if (!options->long_name)
      continue;

    rest =  argparse_prefix_skip (self->argv[0] + 2, options->long_name);
    if (!rest) {
      // negation disabled?
      if (options->flags & OPT_NONEG) {
        continue;
      }
      // only OPT_BOOLEAN/OPT_BIT supports negation
      if (options->type != ARGPARSE_OPT_BOOLEAN && options->type !=  ARGPARSE_OPT_BIT) {
        continue;
      }

      ifnot (Cstring.eq_n (self->argv[0] + 2, "no-", 3)) {
        continue;
      }

      rest = argparse_prefix_skip (self->argv[0] + 2 + 3, options->long_name);
      if (!rest)
        continue;

      opt_flags |= OPT_UNSET;
    }

    if (*rest) {
      if (*rest != '=')
        continue;
      self->optvalue = rest + 1;
    }
    return argparse_getvalue (self, options, opt_flags | OPT_LONG);
  }

  return -2;
}

static int argparse_init (argparse_t *self, argparse_option_t *options,
                                  const char *const *usages, int flags) {
  memset (self, 0, sizeof(*self));
  self->options     = options;
  self->usages      = usages;
  self->flags       = flags;
  self->description = NULL;
  self->epilog      = NULL;
  return 0;
}

/*
static void argparse_describe (argparse_t *self, const char *description,
                                                 const char *epilog) {
  self->description = description;
  self->epilog      = epilog;
}
*/

static void argparse_print_usage (argparse_t *self) {
  if (self->usages) {
    Stdout.print_fmt ("Usage: %s\n", *self->usages++);
    while (*self->usages && **self->usages)
      Stdout.print_fmt ("   or: %s\n", *self->usages++);
  } else {
    Stdout.print_fmt ("Usage:\n");
  }

  // print description
  if (self->description)
    Stdout.print_fmt ("%s\n", self->description);

  Stdout.print ("\n");

  const argparse_option_t *options;

  // figure out best width
  size_t usage_opts_width = 0;
  size_t len;
  options = self->options;
  for (; options->type != ARGPARSE_OPT_END; options++) {
    len = 0;
    if ((options)->short_name) {
      len += 2 - ((options)->flags & SHORT_OPT_HAS_NO_DASH);
    }
    if ((options)->short_name && (options)->long_name) {
      len += 2;           // separator ", "
    }
    if ((options)->long_name) {
      len += bytelen ((options)->long_name) + 2;
    }
    if (options->type == ARGPARSE_OPT_INTEGER) {
      len += bytelen ("=<int>");
    }
    if (options->type == ARGPARSE_OPT_FLOAT) {
      len += bytelen ("=<flt>");
    } else if (options->type == ARGPARSE_OPT_STRING) {
      len += bytelen ("=<str>");
    }
    len = (len + 3) - ((len + 3) & 3);
    if (usage_opts_width < len) {
      usage_opts_width = len;
    }
  }
  usage_opts_width += 4;      // 4 spaces prefix

  options = self->options;
  for (; options->type != ARGPARSE_OPT_END; options++) {
    size_t pos = 0;
    int pad    = 0;
    if (options->type == ARGPARSE_OPT_GROUP) {
      Stdout.print_fmt ("\n%s\n", options->help);
      continue;
    }
    pos = Stdout.print ("    ");
    if (options->short_name) {
      pos += Stdout.print_fmt ("%s%c", // extension
        (options)->flags & SHORT_OPT_HAS_NO_DASH ? "" : "-",
         options->short_name);
    }
    if (options->long_name && options->short_name) {
      pos += Stdout.print (", ");
    }
    if (options->long_name) {
      pos += Stdout.print_fmt ("--%s", options->long_name);
    }
    if (options->type == ARGPARSE_OPT_INTEGER) {
      pos += Stdout.print ("=<int>");
    } else if (options->type == ARGPARSE_OPT_FLOAT) {
      pos += Stdout.print ("=<flt>");
    } else if (options->type == ARGPARSE_OPT_STRING) {
      pos += Stdout.print ("=<str>");
    }
    if (pos <= usage_opts_width) {
      pad = usage_opts_width - pos;
    } else {
      Stdout.print ("\n");
      pad = usage_opts_width;
    }
    Stdout.print_fmt ("%*s%s\n", pad + 2, "", options->help);
  }

  // print epilog
  if (self->epilog)
    Stdout.print_fmt ("%s\n", self->epilog);
}

static int argparse_parse (argparse_t *self, int argc, const char **argv) {
  self->argc = argc - 1;
  self->argv = argv + 1;
  self->out  = argv;

  argparse_options_check (self->options);

  for (; self->argc; self->argc--, self->argv++) {
    const char *arg = self->argv[0];

    int opt_has_no_dash = 0;
    if (arg[0] != '-' || !arg[1]) {
      if (self->flags & ARGPARSE_STOP_AT_NON_OPTION) {
        goto end;
      }


      for (int i = 0; self->options[i].type != ARGPARSE_OPT_END; i++) {
        if (self->options[i].short_name == arg[0]) {
          if (self->options[i].flags & SHORT_OPT_HAS_NO_DASH)
            opt_has_no_dash = 1;
          break;
        }
      }

      if (!opt_has_no_dash) {
        // if it's not option or is a single char '-', copy verbatim
        self->out[self->cpidx++] = self->argv[0];
        continue;
      }
    }

    // short option
    if (arg[1] != '-') {
      self->optvalue = arg + 1 - opt_has_no_dash;
      switch (argparse_short_opt (self, self->options)) {
        case -1:
          return -1;

        case -2:
          if (self->flags & ARGPARSE_DONOT_EXIT_ON_UNKNOWN) {
            self->out[self->cpidx++] = self->argv[0];
            continue;
          }

          goto unknown;
      }

      while (self->optvalue) {
        switch (argparse_short_opt (self, self->options)) {
          case -1:
            return -1;

          case -2:
            if (self->flags & ARGPARSE_DONOT_EXIT_ON_UNKNOWN) {
             self->out[self->cpidx++] = self->argv[0];
             continue;
            }

            goto unknown;
        }
      }

      continue;
    }

    // if '--' presents
    if (!arg[2]) {
      self->argc--;
      self->argv++;
      break;
    }

    // long option
    switch (argparse_long_opt (self, self->options)) {
      case -1:
        return -1;

      case -2:
        if (self->flags & ARGPARSE_DONOT_EXIT_ON_UNKNOWN) {
          self->out[self->cpidx++] = self->argv[0];
          continue;
        }

        goto unknown;
    }
    continue;

unknown:
    Stderr.print_fmt ("error: unknown option `%s`\n", self->argv[0]);
    argparse_print_usage (self);
    return -1;
  }

end:
  memmove (self->out + self->cpidx, self->argv,
           self->argc * sizeof(*self->out));
  self->out[self->cpidx + self->argc] = NULL;

  return self->cpidx + self->argc;
}

public int argparse_help_cb (argparse_t *self, const argparse_option_t *option) {
  (void) option;
  argparse_print_usage (self);
  return -1;
}

public argparse_T __init_argparse__ (void) {
  __INIT__ (io);
  __INIT__ (error);
  __INIT__ (cstring);

  return (argparse_T) {
    .self = (argparse_self) {
      .init = argparse_init,
      .exec =  argparse_parse,
      .print_usage = argparse_print_usage
    }
  };
}
