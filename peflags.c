/*
 * Copyright (c) 2009 Charles Wilson
 * Based on rebase.c by Jason Tishler
 * Significant contributions by Dave Korn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * A copy of the GNU General Public License can be found at
 * http://www.gnu.org/
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>

//#include <windows.h>
#include <inttypes.h>
#define MAX_PATH (260)
#define IMAGE_FILE_RELOCS_STRIPPED	1
#define IMAGE_FILE_EXECUTABLE_IMAGE	2
#define IMAGE_FILE_AGGRESIVE_WS_TRIM 	16
#define IMAGE_FILE_LARGE_ADDRESS_AWARE	32
#define IMAGE_FILE_DEBUG_STRIPPED	512
#define IMAGE_FILE_DLL	8192

#define IMAGE_NT_SIGNATURE 0x00004550

#define IMAGE_DLL_CHARACTERISTICS_DYNAMIC_BASE		0x0040
#define IMAGE_DLL_CHARACTERISTICS_FORCE_INTEGRITY	0x0080
#define IMAGE_DLL_CHARACTERISTICS_NX_COMPAT		0x0100
#define IMAGE_DLLCHARACTERISTICS_NO_ISOLATION		0x0200
#define IMAGE_DLLCHARACTERISTICS_NO_SEH			0x0400
#define IMAGE_DLLCHARACTERISTICS_NO_BIND		0x0800
#define IMAGE_DLLCHARACTERISTICS_WDM_DRIVER		0x2000
#define IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE	0x8000

#define O_BINARY 0
#define TRUE 1
#define FALSE 0

static uint16_t coff_characteristics_set;
static uint16_t coff_characteristics_clr;
static uint16_t coff_characteristics_show;
static uint16_t pe_characteristics_set;
static uint16_t pe_characteristics_clr;
static uint16_t pe_characteristics_show;

typedef struct {
  long          flag;
  const char *  name;
  uint32_t      len;
} symbolic_flags_t;

#define CF(flag,name) { flag, #name, sizeof(#name) - 1 }
static const symbolic_flags_t coff_symbolic_flags[] = {
  CF(0x0001, relocs_stripped),
  CF(0x0002, executable_image),
  CF(0x0004, line_nums_stripped),
  CF(0x0008, local_syms_stripped),
  CF(0x0010, wstrim),
  CF(0x0020, bigaddr),
/*CF(0x0040, unspec_0x0040),*/
  CF(0x0080, bytes_reversed_lo),
  CF(0x0100, 32bit_machine),
  CF(0x0200, sepdbg),
  CF(0x0400, removable_run_from_swap),
  CF(0x0800, net_run_from_swap),
  CF(0x1000, system),
  CF(0x2000, dll),
  CF(0x4000, uniprocessor_only),
  CF(0x8000, bytes_reversed_hi),
  {0, 0, 0}
};

static const symbolic_flags_t pe_symbolic_flags[] = {
/*CF(0x0001, reserved_0x0001),*/
/*CF(0x0002, reserved_0x0002),*/
/*CF(0x0004, reserved_0x0004),*/
/*CF(0x0008, reserved_0x0008),*/
/*CF(0x0010, unspec_0x0010),*/
/*CF(0x0020, unspec_0x0020),*/
  CF(0x0040, dynamicbase),
  CF(0x0080, forceinteg),
  CF(0x0100, nxcompat),
  CF(0x0200, no-isolation),
  CF(0x0400, no-seh),
  CF(0x0800, no-bind),
/*CF(0x1000, reserved_0x1000),*/
  CF(0x2000, wdmdriver),
/*CF(0x4000, reserved_0x4000),*/
  CF(0x8000, tsaware),
  {0, 0, 0}
};


#define OPTION_FORCE_INTEGRITY			150
#define OPTION_NO_ISOLATION			OPTION_FORCE_INTEGRITY+1
#define OPTION_NO_SEH				OPTION_NO_ISOLATION+1
#define OPTION_NO_BIND				OPTION_NO_SEH+1
#define OPTION_WDM_DRIVER			OPTION_NO_BIND+1
#define OPTION_AGGRESIVE_WS_TRIM		OPTION_WDM_DRIVER+1
#define OPTION_LARGE_ADDRESS_AWARE		OPTION_AGGRESIVE_WS_TRIM+1
#define OPTION_DEBUG_STRIPPED			OPTION_LARGE_ADDRESS_AWARE+1

static struct option long_options[] = {
  {"dynamicbase",  optional_argument, NULL, 'd'},
  {"forceinteg",   optional_argument, NULL, OPTION_FORCE_INTEGRITY},
  {"nxcompat",     optional_argument, NULL, 'n'},
  {"no-isolation", optional_argument, NULL, OPTION_NO_ISOLATION},
  {"no-seh",       optional_argument, NULL, OPTION_NO_SEH},
  {"no-bind",      optional_argument, NULL, OPTION_NO_BIND},
  {"wdmdriver",    optional_argument, NULL, OPTION_WDM_DRIVER},
  {"tsaware",      optional_argument, NULL, 't'},
  {"wstrim",       optional_argument, NULL, OPTION_AGGRESIVE_WS_TRIM},
  {"bigaddr",      optional_argument, NULL, OPTION_LARGE_ADDRESS_AWARE},
  {"sepdbg",       optional_argument, NULL, OPTION_DEBUG_STRIPPED},
  {"filelist",     no_argument, NULL, 'T'},
  {"verbose",      no_argument, NULL, 'v'},
  {"help",         no_argument, NULL, 'h'},
  {"version",      no_argument, NULL, 'V'},
  {NULL, no_argument, NULL, 0}
};

static void short_usage (FILE *f);
static void help (FILE *f);
static void version (FILE *f);

int do_mark (const char *pathname);
int get_characteristics(const char *pathname,
                        uint16_t* coff_characteristics,
                        uint16_t* pe_characteristics);
int set_coff_characteristics(const char *pathname,
                             uint16_t coff_characteristics);
int set_pe_characteristics(const char *pathname,
                           uint16_t pe_characteristics);
static int pe_get16 (int fd, off_t offset, uint16_t* value);
static int pe_get32 (int fd, off_t offset, uint32_t* value);
static int pe_set16 (int fd, off_t offset, uint16_t value);

static void display_flags (const char *field_name, const symbolic_flags_t *syms,
                           long show_symbolic, long old_flag_value, long new_flag_value);
static char *symbolic_flags (const symbolic_flags_t *syms, long show, long value);
static void append_and_decorate (char **str, int is_set, const char *name, int len);
static void *xmalloc (size_t num);
#define XMALLOC(type, num)      ((type *) xmalloc ((num) * sizeof(type)))
static void handle_coff_flag_option (const char *option_name,
                                     const char *option_arg,
                                     uint16_t   flag_value);
static void handle_pe_flag_option (const char *option_name,
                                   const char *option_arg,
                                   uint16_t   flag_value);
void parse_args (int argc, char *argv[]);
int string_to_bool  (const char *string, int *value);
int string_to_ulong (const char *string, unsigned long *value);
FILE *file_list_fopen (const char *file_list);
char *file_list_fgets (char *buf, int size, FILE *file);
int file_list_fclose (FILE *file);


int args_index = 0;
int verbose = 0;
const char *file_list = 0;
const char *stdin_file_list = "-";
int mark_any = 0;

int
main (int argc, char *argv[])
{
  int files_attempted = 0;
  int i = 0;

  parse_args (argc, argv);

  /* Operate on files in file list, if specified. */
  if (file_list)
    {
      int status = 0;
      char filename[MAX_PATH + 2];
      FILE *file = file_list_fopen (file_list);

      if (!file)
	exit (2);

      while (file_list_fgets (filename, MAX_PATH + 2, file))
	{
          files_attempted++;
	  if ((status = do_mark (filename)) != 0)
	    break;
	}

      file_list_fclose (file);

      if (files_attempted == 0)
        {
          /* warn the user */
          fprintf (stderr,
             "Error: Could not find any filenames in %s\n",
             (strcmp(file_list, stdin_file_list) == 0 ? "<stdin>" : file_list));
          exit (2);
        }
      if (status != 0)
	exit (2);
    }

  /* Operate on files listed as command line arguments.
   * Don't reset files_attempted because we do not require
   * any args if -T filelist
   */
  for (i = args_index; i < argc; i++)
    {
      const char *filename = argv[i];
      files_attempted++;
      if (do_mark (filename) != 0)
	exit (2);
    }

  if (files_attempted == 0)
    {
      /* warn the user */
      fprintf (stderr, "Error: no files to process\n");
      short_usage (stderr);
      exit (2);
    }

  exit (0);
}
int
do_mark (const char *pathname)
{
  int has_relocs;
  int is_executable;
  int is_dll;
  uint16_t old_coff_characteristics;
  uint16_t new_coff_characteristics;
  uint16_t old_pe_characteristics;
  uint16_t new_pe_characteristics;
  int i;

  /* Skip if file does not exist */
  if (access (pathname, F_OK) == -1)
    {
      fprintf (stderr, "%s: skipped because nonexistent\n", pathname);
      return 0;
    }

  if (mark_any)
    {
      /* Skip if not writable. */
      if (access (pathname, W_OK) == -1)
        {
          fprintf (stderr, "%s: skipped because not writable\n", pathname);
          return 0;
        }
    }

  if (get_characteristics (pathname,
                           &old_coff_characteristics,
                           &old_pe_characteristics) != 0)
    {
      fprintf (stderr,
               "%s: skipped because could not read file characteristics\n",
               pathname);
      return 0;
    }
  new_coff_characteristics = old_coff_characteristics;
  new_coff_characteristics |= coff_characteristics_set;
  new_coff_characteristics &= ~coff_characteristics_clr;

  new_pe_characteristics = old_pe_characteristics;
  new_pe_characteristics |= pe_characteristics_set;
  new_pe_characteristics &= ~pe_characteristics_clr;

  is_executable = ((new_coff_characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) > 0);
  is_dll        = ((new_coff_characteristics & IMAGE_FILE_DLL) > 0);
  has_relocs    = ((new_coff_characteristics & IMAGE_FILE_RELOCS_STRIPPED) == 0);

  /* validation and warnings about things that are
     problematic, but that we are not explicitly
     changing */
  if (!has_relocs)
    {
      if (verbose
         && (new_pe_characteristics & IMAGE_DLL_CHARACTERISTICS_DYNAMIC_BASE)
         && (old_pe_characteristics & IMAGE_DLL_CHARACTERISTICS_DYNAMIC_BASE))
        {
          fprintf (stderr,
                   "Warning: file has no relocation info but has dynbase set (%s).\n",
                   pathname);
        }
    }
  if (!is_executable || is_dll)
    {
      if (verbose
         && (new_pe_characteristics & IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE)
         && (old_pe_characteristics & IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE))
        {
          fprintf (stderr,
                   "Warning: file is non-executable but has tsaware set (%s).\n",
                   pathname);
        }
    }
 
  if (mark_any)
    {
      /* validation and warnings about things we are changing */
      if (!has_relocs)
        {
          if (   (new_pe_characteristics & IMAGE_DLL_CHARACTERISTICS_DYNAMIC_BASE)
             && !(old_pe_characteristics & IMAGE_DLL_CHARACTERISTICS_DYNAMIC_BASE))
            {
              fprintf (stderr,
                       "Warning: setting dynbase on file with no relocation info (%s).\n",
                       pathname);
            }
        } 

      if (!is_executable || is_dll)
        {
          if (   (new_pe_characteristics & IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE)
             && !(old_pe_characteristics & IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE))
            {
              fprintf (stderr,
                       "Warning: setting tsaware on non-executable (%s).\n",
                       pathname);
            }
        }

      /* setting */
      if (new_coff_characteristics != old_coff_characteristics)
        if (set_coff_characteristics (pathname,new_coff_characteristics) != 0)
          {
            fprintf (stderr,
                     "Error: could not update coff characteristics (%s): %s\n",
                      pathname, strerror (errno));
            return 1;
          }
      if (new_pe_characteristics != old_pe_characteristics)
        if (set_pe_characteristics (pathname,new_pe_characteristics) != 0)
          {
            fprintf (stderr,
                     "Error: could not update pe characteristics (%s): %s\n",
                      pathname, strerror (errno));
            return 1;
          }
    }

  /* Display characteristics. */
  if (verbose
      || !mark_any
      || coff_characteristics_show
      || pe_characteristics_show)
    {
      printf ("%s: ", pathname);
      display_flags ("coff", coff_symbolic_flags,
                     coff_characteristics_show,
                     old_coff_characteristics,
                     new_coff_characteristics);
      display_flags ("pe", pe_symbolic_flags,
                     pe_characteristics_show,
                     old_pe_characteristics,
                     new_pe_characteristics);
      puts ("");
    }

  return 0;
}

static void
display_flags (const char *field_name, const symbolic_flags_t *syms,
               long show_symbolic, long old_flag_value, long new_flag_value)
{
  if (show_symbolic)
    {
      if (old_flag_value != new_flag_value)
        {
          char * old_symbolic = symbolic_flags (syms, show_symbolic, old_flag_value);
          char * new_symbolic = symbolic_flags (syms, show_symbolic, new_flag_value);
          printf ("%s(0x%04x%s==>0x%04x%s) ", field_name,
              old_flag_value, old_symbolic,
              new_flag_value, new_symbolic);
          free (old_symbolic);
          free (new_symbolic);
        }
      else
        {
          char * old_symbolic = symbolic_flags (syms, show_symbolic, old_flag_value);
          printf ("%s(0x%04x%s) ", field_name,
              old_flag_value, old_symbolic);
          free (old_symbolic);
        }
    }
  else
    {
      if (old_flag_value != new_flag_value)
        printf ("%s(0x%04x==>0x%04x) ", field_name,
            old_flag_value, new_flag_value);
      else
        printf ("%s(0x%04x) ", field_name, old_flag_value);
    }
}

static char *
symbolic_flags (const symbolic_flags_t *syms, long show, long value)
{
  int i;
  char * rVal = NULL;
  for (i = 0; syms[i].name; i++)
    {
      if (syms[i].flag & show)
        {
          append_and_decorate (&rVal,
            (value & syms[i].flag),
            syms[i].name,
            syms[i].len);
        }
    }
  if (rVal)
    {
      size_t len = strlen (rVal);
      char *tmp = XMALLOC (char, len + 3);
      *tmp = '[';
      memcpy (tmp+1, rVal, len);
      tmp[len+1] = ']';
      tmp[len+2] = '\0';
      free (rVal);
      rVal = tmp;
    }
  return rVal;
}

static void
append_and_decorate (char **str, int is_set, const char *name, int len)
{
  char *tmp;
  int slen;
  if (!*str)
    {
      *str = XMALLOC (char, len + 2);
      (*str)[0] = (is_set ? '+' : '-');
      memcpy ((*str)+1, name, len);
      (*str)[len+1] = '\0';
      return; 
    }
  else
    { 
      slen = strlen (*str);
      tmp = XMALLOC (char, slen + 2 + len + 1);
      memcpy (tmp, *str, slen);
      free (*str);
      *str = tmp;
      tmp = *str + slen;
      *tmp++ = ',';
      *tmp++ = (is_set ? '+' : '-');
      memcpy (tmp, name, len);
      tmp[len] = '\0';
    } 
}

static void *
xmalloc (size_t num)
{
  void *p = (void *) malloc (num);
  if (!p)
    {
      fputs ("Memory exhausted", stderr);
      exit (2);
    }
  return p;
}

static void
handle_pe_flag_option (const char *option_name,
                       const char *option_arg,
                       uint16_t   flag_value)
{
  int bool_value;
  if (!option_arg)
    {
      pe_characteristics_show |= flag_value;
    }
  else
    {
      if (string_to_bool (option_arg, &bool_value) != 0)
        {
          fprintf (stderr, "Invalid argument for %s: %s\n", 
                   option_name, option_arg);
          short_usage (stderr);
          exit (1);
        }
      if (bool_value)
        pe_characteristics_set |= flag_value;
      else
        pe_characteristics_clr |= flag_value;
    }
}
static void
handle_coff_flag_option (const char *option_name,
                         const char *option_arg,
                         uint16_t   flag_value)
{
  int bool_value;
  if (!option_arg)
    {
      coff_characteristics_show |= flag_value;
    }
  else
    {
      if (string_to_bool (option_arg, &bool_value) != 0)
        {
          fprintf (stderr, "Invalid argument for %s: %s\n", 
                   option_name, option_arg);
          short_usage (stderr);
          exit (1);
        }
      if (bool_value)
        coff_characteristics_set |= flag_value;
      else
        coff_characteristics_clr |= flag_value;
    }
}
void
parse_args (int argc, char *argv[])
{
  int c;

  while (1)
    {
      int option_index = 0;
      c = getopt_long (argc, argv, "d::t::n::T:vhV", long_options, &option_index);
      if (c == -1)
        break;

      switch (c)
	{
	case 'h':
	  help (stdout);
          exit (0);
          break;
	case 'V':
	  version (stdout);
	  exit (0);
	  break;

	case 'd':
	  handle_pe_flag_option (long_options[option_index].name,
	                         optarg,
	                         IMAGE_DLL_CHARACTERISTICS_DYNAMIC_BASE);
	  break;
	case 'n':
	  handle_pe_flag_option (long_options[option_index].name,
	                         optarg,
	                         IMAGE_DLL_CHARACTERISTICS_NX_COMPAT);
	  break;
	case 't':
	  handle_pe_flag_option (long_options[option_index].name,
	                         optarg,
	                         IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE);
	  break;
	case OPTION_FORCE_INTEGRITY:
	  handle_pe_flag_option (long_options[option_index].name,
	                         optarg,
	                         IMAGE_DLL_CHARACTERISTICS_FORCE_INTEGRITY);
	  break;
	case OPTION_NO_ISOLATION:
	  handle_pe_flag_option (long_options[option_index].name,
	                         optarg,
	                         IMAGE_DLLCHARACTERISTICS_NO_ISOLATION);
	  break;
	case OPTION_NO_SEH:
	  handle_pe_flag_option (long_options[option_index].name,
	                         optarg,
	                         IMAGE_DLLCHARACTERISTICS_NO_SEH);
	  break;
	case OPTION_NO_BIND:
	  handle_pe_flag_option (long_options[option_index].name,
	                         optarg,
	                         IMAGE_DLLCHARACTERISTICS_NO_BIND);
	  break;
	case OPTION_WDM_DRIVER:
	  handle_pe_flag_option (long_options[option_index].name,
	                         optarg,
	                         IMAGE_DLLCHARACTERISTICS_WDM_DRIVER);
	  break;
	case OPTION_AGGRESIVE_WS_TRIM:
	  handle_coff_flag_option (long_options[option_index].name,
	                           optarg,
	                           IMAGE_FILE_AGGRESIVE_WS_TRIM);
	  break;
	case OPTION_LARGE_ADDRESS_AWARE:
	  handle_coff_flag_option (long_options[option_index].name,
	                           optarg,
	                           IMAGE_FILE_LARGE_ADDRESS_AWARE);
	  break;
	case OPTION_DEBUG_STRIPPED:
	  handle_coff_flag_option (long_options[option_index].name,
	                           optarg,
	                           IMAGE_FILE_DEBUG_STRIPPED);
	  break;
	case 'T':
	  file_list = optarg;
	  break;
	case 'v':
	  verbose = TRUE;
	  break;
        case '?':
          break;
	default:
	  short_usage (stderr);
	  exit (1);
	  break;
	}
    }

  args_index = optind;
  mark_any =   pe_characteristics_set
             | pe_characteristics_clr
             | coff_characteristics_set
             | coff_characteristics_clr;
}

int
string_to_bool (const char *string, int *value)
{
  unsigned long number = 0;
  if (!string || !*string)
    return 1;

  if (string_to_ulong (string, &number) != 0)
    {
      size_t len = strlen (string);
      if ( (len == 4 && strcasecmp (string, "true") == 0)
         ||(len == 3 && strcasecmp (string, "yes") == 0) 
         ||(len == 1 && strcasecmp (string, "t") == 0)
         ||(len == 1 && strcasecmp (string, "y") == 0)) 
        {
          *value = TRUE;
        }
      else if ( (len == 5 && strcasecmp (string, "false") == 0)
         ||(len == 2 && strcasecmp (string, "no") == 0) 
         ||(len == 1 && strcasecmp (string, "f") == 0) 
         ||(len == 1 && strcasecmp (string, "n") == 0))
        {
          *value = FALSE;
        }
      else
        {
          return 1;
        }
    }
  else
    {
      *value = (number != 0);
    }
  return 0; 
}

int
string_to_ulong (const char *string, unsigned long *value)
{
  unsigned long number = 0;
  char * endp;
  errno = 0;

  /* null or empty input */
  if (!string || !*string)
    return 1;

  number = strtoul (string, &endp, 0);

  /* out of range */
  if (ERANGE == errno)
    return 1;

  /* no valid numeric input */
  if (endp == string)
    return 1;

  /* non-numeric trailing characters */
  if (*endp != '\0')
    return 1;

  *value = number;
  return 0;
}

int
get_characteristics(const char *pathname,
                    uint16_t* coff_characteristics,
                    uint16_t* pe_characteristics)
{
  uint32_t pe_header_offset, opthdr_ofs;
  int status = 1;
  int fd, size;
  uint32_t pe_sig;

  fd = open (pathname, O_RDONLY|O_BINARY);
  if (fd == -1)
    return status;

  if (pe_get32 (fd, 0x3c, &pe_header_offset) != 0)
    goto done;
  opthdr_ofs = pe_header_offset + 4 + 20;

  pe_sig = 0;
  if (pe_get32 (fd, pe_header_offset, &pe_sig) != 0)
    goto done;
  if (pe_sig != IMAGE_NT_SIGNATURE)
    goto done;

  if (pe_get16 (fd, pe_header_offset + 4 + 18, coff_characteristics) != 0)
    goto done;

  if (pe_get16 (fd, opthdr_ofs + 70, pe_characteristics) != 0)
    goto done;

  status = 0;

done:
  close (fd);
  return status;
}

int
set_coff_characteristics(const char *pathname,
                         uint16_t coff_characteristics)
{
  uint32_t pe_header_offset;
  int status = 1;
  int fd, size;

  /* no extra checking of file's contents below, because
     get_characteristics already did that */
  fd = open (pathname, O_RDWR|O_BINARY);
  if (fd == -1)
    return status;

  if (pe_get32 (fd, 0x3c, &pe_header_offset) != 0)
    goto done;

  if (pe_set16 (fd, pe_header_offset + 4 + 18, coff_characteristics) != 0)
    {
      fprintf (stderr,
               "CATASTROPIC ERROR: attempt to write to file failed! %s could be corrupted; HALTING.\n",
               pathname);
      close (fd);
      exit(2);
    }

  status = 0;

done:
  close (fd);
  return status;
}

int
set_pe_characteristics(const char *pathname,
                       uint16_t pe_characteristics)
{
  uint32_t pe_header_offset, opthdr_ofs;
  int status = 1;
  int fd, size;

  /* no extra checking of file's contents below, because
     get_characteristics already did that */
  fd = open (pathname, O_RDWR|O_BINARY);
  if (fd == -1)
    return status;

  if (pe_get32 (fd, 0x3c, &pe_header_offset) != 0)
    goto done;
  opthdr_ofs = pe_header_offset + 4 + 20;

  if (pe_set16 (fd, opthdr_ofs + 70, pe_characteristics) != 0)
    {
      fprintf (stderr,
               "CATASTROPIC ERROR: attempt to write to file failed! %s could be corrupted; HALTING.\n",
               pathname);
      close (fd);
      exit(2);
    }

  status = 0;

done:
  close (fd);
  return status;
}

static int 
pe_get16 (int fd, off_t offset, uint16_t* value)
{
  unsigned char b[2];
  if (lseek (fd, offset, SEEK_SET) == -1)
    return 1;
  if (read (fd, b, 2) != 2)
    return 1;
  *value = b[0] + (b[1]<<8);
  return 0;
}

static int 
pe_get32 (int fd, off_t offset, uint32_t* value)
{
  unsigned char b[4];
  if (lseek (fd, offset, SEEK_SET) == -1)
    return 1;
  if (read (fd, b, 4) != 4)
    return 1;
  *value = b[0] + (b[1]<<8) + (b[2]<<16) + (b[3]<<24);
  return 0;
}

static int 
pe_set16 (int fd, off_t offset, uint16_t value)
{
  unsigned char b[2];
  b[0] = (unsigned char) (value & 0x00ff);
  b[1] = (unsigned char) ((value>>8) & 0x00ff);
  if (lseek (fd, offset, SEEK_SET) == -1)
    return 1;
  if (write (fd, b, 2) != 2)
    return 1;
  return 0;
}

FILE *
file_list_fopen (const char *file_list)
{
  FILE *file = stdin;
  if (strcmp(file_list, stdin_file_list) != 0)
    {
      file = fopen (file_list, "r");
      if (!file)
	fprintf (stderr, "cannot read %s\n", file_list);
    }
  return file;
}

char *
file_list_fgets (char *buf, int size, FILE *file)
{
  char *status = fgets (buf, size, file);
  if (status)
    {
      size_t length = strlen (buf);
      if (buf[length - 1] == '\n')
	buf[length - 1] = '\0';
    }
  return status;
}

int
file_list_fclose (FILE *file)
{
  int status = 0;
  if (strcmp(file_list, stdin_file_list) != 0)
    status = fclose (file);
  return status;
}

static void
short_usage (FILE *f)
{
  fputs ("Usage: peflags [OPTIONS] file(s)...\n", f);
  fputs ("Sets or clears various flags in PE files (that is, exes and dlls)\n", f);
  fputs ("Use --help for full help text\n", f);
}

static void
help (FILE *f)
{
  fputs ("Usage: peflags [OPTIONS] file(s)...\n", f);
  fputs ("Sets, clears, or displays various flags in PE files (that is,\n", f);
  fputs ("exes and dlls).  For each flag, if an argument is given, then\n", f);
  fputs ("the specified flag will be set or cleared; if no argument is\n", f);
  fputs ("given, then the current value of that flag will be displayed.\n", f);
  fputs ("\n", f);
  fputs ("  -d, --dynamicbase  [BOOL]   Image base address may be relocated using\n", f);
  fputs ("                              address space layout randomization (ASLR)\n", f);
  fputs ("      --forceinteg   [BOOL]   Code integrity checks are enforced\n", f);
  fputs ("  -n, --nxcompat     [BOOL]   Image is compatible with data execution\n", f);
  fputs ("                              prevention\n", f);
  fputs ("      --no-isolation [BOOL]   Image understands isolation but do not\n", f);
  fputs ("                              isolate the image\n", f);
  fputs ("      --no-seh       [BOOL]   Image does not use SEH. No SE handler may\n", f);
  fputs ("                              be called in this image\n", f);
  fputs ("      --no-bind      [BOOL]   Do not bind this image\n", f);
  fputs ("      --wdmdriver    [BOOL]   Driver uses the WDM model\n", f);
  fputs ("  -t, --tsaware      [BOOL]   Image is Terminal Server aware\n", f);
  fputs ("      --wstrim       [BOOL]   Aggressively trim the working set.\n", f);
  fputs ("      --bigaddr      [BOOL]   The application can handle addresses\n", f);
  fputs ("                              larger than 2 GB\n", f);
  fputs ("      --sepdbg       [BOOL]   Debugging information was removed and\n", f);
  fputs ("                              stored separately in another file.\n", f);
  fputs ("  -T, --filelist FILE         Indicate that FILE contains a list\n", f);
  fputs ("                              of PE files to process\n", f);
  fputs ("  -v, --verbose               Display diagnostic information\n", f);
  fputs ("  -V, --version               Display version information\n", f);
  fputs ("  -h, --help                  Display this help\n", f);
  fputs ("\n", f);
  fputs ("BOOL: may be 1, true, or yes - indicates that the flag should be set\n", f);
  fputs ("          if 0, false, or no - indicates that the flag should be cleared\n", f);
  fputs ("          if not present, then display symbolicly the value of the flag\n", f);
  fputs ("Valid forms for short options: -d, -d0, -d1, -dfalse, etc\n", f);
  fputs ("Valid forms for long options :  --tsaware, --tsaware=true, etc\n", f);
  fputs ("To set a value, and display the results symbolic, repeat the option:\n", f);
  fputs ("  --tsaware=true --tsaware -d0 -d\n", f);
  fputs ("\n", f);
}

static void
version (FILE *f)
{
  fprintf (f, "peflags version %s\n", "Geoffroy Couprie");
  fprintf (f, "Copyright (c) 2009  Charles Wilson, Dave Korn, Jason Tishler\n");
}

