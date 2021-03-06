/*
 * x_f.c
 *
 *  Created on: Dec 4, 2013
 *      Author: reboot
 */

#include <glutil.h>
#include <t_glob.h>
#include <xref.h>
#include <x_f.h>
#include <l_error.h>
#include <str.h>

#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <zlib.h>

static uint32_t crc_32_tab[] =
  { 0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
      0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
      0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
      0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
      0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
      0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
      0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
      0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
      0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
      0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
      0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
      0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
      0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
      0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
      0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
      0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
      0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
      0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
      0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
      0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
      0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
      0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
      0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
      0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
      0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
      0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
      0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
      0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
      0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
      0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
      0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
      0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
      0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
      0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
      0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
      0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
      0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
      0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
      0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
      0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
      0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
      0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
      0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D };

off_t
get_file_size (char *file)
{
  struct stat st;

  if (stat (file, &st) == -1)
    return 0;

  return (off_t) st.st_size;
}

time_t
get_file_creation_time (struct stat *st)
{
  return (time_t) birthtime(st);
}

int
check_path (char *path)
{
  struct stat sb;
  return stat (path, &sb);
}

int
file_exists (char *file)
{
  int r = get_file_type (file);

  if (r == DT_REG)
    {
      return 0;
    }

  return 1;
}
int
dir_exists (char *dir)
{
  int r = get_file_type (dir);

  if (r == DT_DIR)
    {
      return 0;
    }
  return 1;
}

int
get_file_type (char *file)
{
  struct stat sb;

  if (stat (file, &sb) == -1)
    {
      return DT_UNKNOWN;
    }

  return IFTODT(sb.st_mode);
}

off_t
file_crc32 (char *file, uint32_t *crc_out)
{
  g_setjmp (0, "file_crc32", NULL, NULL);
  FILE *fp;

  size_t r;

  *crc_out = 0x0;

  if ((fp = fopen (file, "rb")) == NULL)
    {
      return 0;
    }

  uint8_t buffer[CRC_FILE_READ_BUFFER_SIZE], *ptr = buffer;

  uint32_t crc = MAX_uint32_t;

  for (;; ptr = buffer)
    {
      if ((r = fread (buffer, 1, CRC_FILE_READ_BUFFER_SIZE, fp)) < 1)
	{
	  break;
	}

      for (; r; r--, ptr++)
	{
	  crc = UPDC32(*ptr, crc);
	}
    }

  if (ferror (fp))
    {
      fclose (fp);
      return 0;
    }

  *crc_out = ~crc;

  fclose (fp);

  return 1;
}

int
find_absolute_path (char *exec, char *output)
{
  char *env = getenv ("PATH");

  if (!env)
    {
      return 1;
    }

  mda s_p =
    { 0 };

  md_init (&s_p, 64);

  int p_c = split_string (env, 0x3A, &s_p);

  if (p_c < 1)
    {
      return 2;
    }

  p_md_obj ptr = md_first (&s_p);

  while (ptr)
    {
      snprintf (output, PATH_MAX, "%s/%s", (char*) ptr->ptr, exec);
      if (!access (output, R_OK | X_OK))
	{
	  md_free (&s_p);
	  return 0;
	}
      ptr = ptr->next;
    }

  md_free (&s_p);

  return 3;
}

int
self_get_path (char *out)
{
  g_setjmp (0, "self_get_path", NULL, NULL);

  char path[PATH_MAX];
  path[0] = 0x0;
  int r;

  snprintf (path, PATH_MAX, "/proc/%d/exe", getpid ());

  if (access (path, R_OK))
    {
      snprintf (path, PATH_MAX, "/compat/linux/proc/%d/exe", getpid ());
    }
  else
    {
      goto read;
    }

  if (access (path, R_OK))
    {
      snprintf (path, PATH_MAX, "/proc/%d/file", getpid ());
    }

  read:

  if ((r = readlink (path, out, PATH_MAX)) < 1)
    {
      if ((r = find_absolute_path (_p_argv[0], out)))
	{
	  snprintf (out, PATH_MAX, "%s", _p_argv[0]);
	}
      return 0;
    }

  out[r] = 0x0;
  return 0;
}

int
delete_file (char *name, unsigned char type, void *arg, __g_eds eds)
{
  g_setjmp (0, "delete_file", NULL, NULL);
  char *match = (char*) arg;

  if (type != DT_REG)
    {
      return 1;
    }

  if (!reg_match (match, name, 0))
    {
      return remove (name);
    }

  return 2;
}

ssize_t
file_copy (char *source, char *dest, char *mode, uint32_t flags)
{
  g_setjmp (0, "file_copy", NULL, NULL);

  if (gfl & F_OPT_NOWRITE)
    {
      return 1;
    }

  struct stat st_s, st_d;
  mode_t st_mode = 0;

  if (stat (source, &st_s))
    {
      return -9;
    }

  off_t ssize = st_s.st_size;

  if (ssize < 1)
    {
      return -1;
    }

  FILE *fh_s = fopen (source, "rb");

  if (!fh_s)
    {
      return -2;
    }

  if (!strncmp (mode, "a", 1) && (flags & F_FC_MSET_DEST))
    {
      if (file_exists (dest))
	{
	  st_mode = st_s.st_mode;
	}
      else
	{
	  if (stat (source, &st_d))
	    {
	      return -10;
	    }
	  st_mode = st_d.st_mode;
	}
    }
  else if (flags & F_FC_MSET_SRC)
    {
      st_mode = st_s.st_mode;
    }

  FILE *fh_d = fopen (dest, mode);

  if (!fh_d)
    {
      return -3;
    }

  size_t r = 0, t = 0, w;
  char *buffer = malloc (V_MB);

  while ((r = fread (buffer, 1, V_MB, fh_s)) > 0)
    {
      if ((w = fwrite (buffer, 1, r, fh_d)))
	{
	  t += w;
	}
      else
	{
	  free (buffer);
	  return -4;
	}
    }

  free (buffer);
  fclose (fh_d);
  fclose (fh_s);

  if (st_mode)
    {
      chmod (dest, st_mode);
    }

  return t;
}

off_t
read_file (char *file, void *buffer, size_t read_max, off_t offset, FILE *_fp)
{
  g_setjmp (0, "read_file", NULL, NULL);
  size_t read;
  int r;
  FILE *fp;

  if (file && file[0] == 0x2D && file[1] == 0x0)
    {
      fp = stdin;
    }
  else
    {
      if (!_fp)
	{
	  if (!file)
	    {
	      return 0;
	    }

	  off_t a_fsz = get_file_size (file);

	  if (!a_fsz)
	    return 0;

	  if (read_max > a_fsz)
	    {
	      read_max = a_fsz;
	    }

	  if ((fp = fopen (file, "rb")) == NULL)
	    {
	      return 0;
	    }
	}
      else
	{
	  fp = _fp;
	}

      if (offset)
	{
	  fseek (fp, (off_t) offset, SEEK_SET);
	}
    }

  for (read = 0; !feof (fp) && !ferror (fp) && read < read_max;)
    {
      if ((r = fread (&((unsigned char*) buffer)[read], 1, read_max - read, fp))
	  < 1)
	{
	  break;
	}
      read += r;
    }

  if (!_fp && fp != stdin)
    {
      fclose (fp);
    }

  return read;
}

off_t
enum_readline (char *file, void *buffer, size_t read_max, off_t max_l,
	       FILE *_fp, int
	       (*call_b) (char *b, void *a),
	       void *arg)
{
  g_setjmp (0, "read_file", NULL, NULL);

  FILE *fp;

  if (file && file[0] == 0x2D && file[1] == 0x0)
    {
      fp = stdin;
    }
  else
    {
      if ((fp = fopen (file, "rb")) == NULL)
	{
	  return 0;
	}
    }

  off_t lc = 0;

  while (fgets (buffer, read_max, fp) && lc < max_l && !ferror (fp)
      && !feof (fp))
    {
      char *s_b = (char*) buffer;
      size_t l_len = strlen (s_b) - 1;
      if (s_b[l_len] == 0xA)
	{
	  s_b[l_len] = 0x0;
	}
      if (!call_b (buffer, arg))
	{
	  lc++;
	}
    }

  if (!_fp && fp != stdin)
    {
      fclose (fp);
    }

  return lc;
}

int
write_file_text (char *data, char *file)
{
  g_setjmp (0, "write_file_text", NULL, NULL);
  int r;
  FILE *fp;

  if ((fp = fopen (file, "a")) == NULL)
    return 0;

  r = fwrite (data, 1, strlen (data), fp);

  fclose (fp);

  return r;
}

size_t
exec_and_redirect_output (char *command, FILE *output)
{
  size_t r = 0, w = 0;
  FILE *pipe = NULL;

  if (!(pipe = popen (command, "r")))
    {
      return 0;
    }

  size_t read = 0;

  char r_b[0x2000];

  while (!feof (pipe))
    {
      if ((r = fread (r_b, 1, 0x2000, pipe)) <= 0)
	{
	  break;
	}
      w += fwrite (r_b, r, 1, output);
      read += r;
    }

  pclose (pipe);

  return w;
}

#ifdef HAVE_ZLIB_H
int
gz_feof(void *p)
  {
    return gzeof((gzFile) p);
  }
#endif

int
g_feof (void *p)
{
  return feof ((FILE*) p);
}

int
load_guid_info (pmda md, char *path)
{
  FILE *fh = fopen (path, "r");

  if (NULL == fh)
    {
      return 1;
    }

  errno = 0;

  char buffer[PGF_MAX_LINE_SIZE + 1];
  uint32_t lc = 0;

  while (fgets (buffer, PGF_MAX_LINE_SIZE, fh) && lc < PGF_MAX_LINE_PROC
      && !ferror (fh) && !feof (fh))
    {
      if (!strlen (buffer) || buffer[0] == 0xA)
	{
	  break;
	}

      mda spl =
	{ 0 };
      md_init (&spl, 5);

      if (split_string (buffer, 0x3A, &spl) < 3)
	{
	  md_free (&spl);
	  return 2;
	}

      p_md_obj ptr = md_first (&spl);

      p_gu_n gu = md_alloc (md, sizeof(gu_n));

      strcp_s (gu->name, sizeof(gu->name), (char *) ptr->ptr);

      errno = 0;
      gu->id = (uint32_t) strtoul (ptr[2].ptr, NULL, 10);

      if ( errno == ERANGE || errno == EINVAL)
	{
	  return 3;
	}

      md_free (&spl);

      lc++;
    }

  fclose (fh);

  return 0;
}

p_gu_n
search_xuid_id (pmda md, uint32_t id)
{
  p_md_obj ptr = md_first (md);

  while (ptr)
    {
      p_gu_n gu = (p_gu_n) ptr->ptr;

      if (gu->id == id)
	{
	  return gu;
	}

      ptr = ptr->next;
    }

  return NULL;
}

p_gu_n
search_xuid_name (pmda md, char *name)
{
  p_md_obj ptr = md_first (md);
  size_t name_l = strlen (name);

  while (ptr)
    {
      p_gu_n gu = (p_gu_n) ptr->ptr;

      size_t p_name_l = strlen (gu->name);
      if (name_l == p_name_l && !strncmp (gu->name, name, p_name_l))
	{
	  return gu;
	}

      ptr = ptr->next;
    }

  return NULL;
}

#ifdef HAVE_ZLIB_H
int
g_is_file_compressed(char *file)
  {
    gzFile gz_fh;

    if ((gz_fh = gzopen(file, "r")) == NULL)
      {
	return 2;
      }

    int r = gzdirect(gz_fh);
    gzclose(gz_fh);

    return r;
  }
#endif

int
r_preload_guid_data (pmda md, char *path)
{
  if (md->offset)
    {
      return 0;
    }

  char buffer[PATH_MAX];
  snprintf (buffer, PATH_MAX, "%s%s", GLROOT, path);
  md_init (md, 16);

  int r = load_guid_info (md, buffer);

  return r;
}
