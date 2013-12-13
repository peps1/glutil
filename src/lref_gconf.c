/*
 * lref_gconf.c
 *
 *  Created on: Dec 10, 2013
 *      Author: reboot
 */

#include <glutil.h>
#include "lref_gconf.h"

#include <str.h>
#include <lref.h>
#include <lref_gen.h>
#include <l_sb.h>

#include <errno.h>

int
gconf_format_block(void *iarg, char *output)
{
  __d_gconf data = (__d_gconf) iarg;

  return print_str("GCONF\x9%s\x9%s\x9%s\x9%s\x9%hhu\x9%hhu\x9%s\x9%s\x9%s\x9%s\x9%s\n",
      data->r_clean, data->r_postproc, data->r_yearm, data->r_sects, data->o_use_shared_mem,
      data->o_exec_on_lookup_fail, data->e_lookup_fail, data->e_match, data->r_skip_basedir,
      data->r_exclude_user, data->r_exclude_user_flags);

}

int
gconf_format_block_batch(void *iarg, char *output)
{
  __d_gconf data = (__d_gconf) iarg;

  return printf("GCONF\x9%s\x9%s\x9%s\x9%s\x9%hhu\x9%hhu\x9%s\x9%s\x9%s\x9%s\x9%s\n",
      data->r_clean, data->r_postproc, data->r_yearm, data->r_sects, data->o_use_shared_mem,
      data->o_exec_on_lookup_fail, data->e_lookup_fail, data->e_match, data->r_skip_basedir,
      data->r_exclude_user, data->r_exclude_user_flags);

}

int
gconf_format_block_exp(void *iarg, char *output)
{
  __d_gconf data = (__d_gconf) iarg;

  return printf("r_path_clean %s\n"
      "r_path_postproc %s\n"
      "r_year_extract %s\n"
      "r_skip_basedir %s\n"
      "paths %s\n"
      "use_shared_mem %hhu\n"
      "execute_on_lookup_fail %hhu\n"
      "path_exec_on_lookup_fail %s\n"
      "path_exec_on_match %s\n"
      "r_exclude_user %s\n"
      "r_exclude_user_flags %s\n\n"
      , data->r_clean, data->r_postproc, data->r_yearm, data->r_skip_basedir, data->r_sects, data->o_use_shared_mem,
      data->o_exec_on_lookup_fail, data->e_lookup_fail, data->e_match, data->r_exclude_user, data->r_exclude_user_flags);

}

void *
ref_to_val_ptr_gconf(void *arg, char *match, int *output)
{
  __d_gconf data = (__d_gconf) arg;
  if (!strncmp(match, _MC_GCONF_O_SHM, 14))
    {
      *output = ~((uint8_t) sizeof(data->o_use_shared_mem));
      return &data->o_use_shared_mem;
    }
  else if (!strncmp(match, _MC_GCONF_E_OLF, 22))
    {
      *output = ~((uint8_t) sizeof(data->o_exec_on_lookup_fail));
      return &data->o_exec_on_lookup_fail;
    }

  return NULL;
}

char *
dt_rval_gconf_shm(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  snprintf(output, max_size, ((__d_drt_h ) mppd)->direc, ((__d_gconf) arg)->o_use_shared_mem);
  return output;
}

char *
dt_rval_gconf_e_olf(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  snprintf(output, max_size, ((__d_drt_h ) mppd)->direc, ((__d_gconf) arg)->o_exec_on_lookup_fail);
  return output;
}

char *
dt_rval_gconf_r_clean(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return ((__d_gconf) arg)->r_clean;
}

char *
dt_rval_gconf_r_postproc(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return ((__d_gconf) arg)->r_postproc;
}

char *
dt_rval_gconf_r_yearm(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return ((__d_gconf) arg)->r_yearm;
}

char *
dt_rval_gconf_r_sects(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return ((__d_gconf) arg)->r_sects;
}

char *
dt_rval_gconf_e_lf(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return ((__d_gconf) arg)->e_lookup_fail;
}

char *
dt_rval_gconf_e_m(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return ((__d_gconf) arg)->e_match;
}

char *
dt_rval_gconf_sbdir(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return ((__d_gconf) arg)->r_skip_basedir;
}

char *
dt_rval_gconf_ex_u(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return ((__d_gconf) arg)->r_exclude_user;
}

char *
dt_rval_gconf_ex_uf(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return ((__d_gconf) arg)->r_exclude_user_flags;
}

void *
ref_to_val_lk_gconf(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  void *ptr;
  if ((ptr = ref_to_val_lk_generic(NULL, match, output, max_size, mppd)))
    {
      return ptr;
    }

  if (!strncmp(match, _MC_GCONF_R_CLEAN, 12))
    {
      return as_ref_to_val_lk(match, dt_rval_gconf_r_clean, (__d_drt_h) mppd, "%s");
    }
  else if (!strncmp(match, _MC_GCONF_R_POSTPROC, 15))
    {
      return as_ref_to_val_lk(match, dt_rval_gconf_r_postproc, (__d_drt_h) mppd, "%s");
    }
  else if (!strncmp(match, _MC_GCONF_R_YEARM, 14))
    {
      return as_ref_to_val_lk(match, dt_rval_gconf_r_yearm, (__d_drt_h) mppd, "%s");
    }
  else if (!strncmp(match, _MC_GCONF_R_SECTS, 5))
    {
      return as_ref_to_val_lk(match, dt_rval_gconf_r_sects, (__d_drt_h) mppd, "%s");
    }
  else if (!strncmp(match, _MC_GCONF_O_SHM, 14))
    {
      return as_ref_to_val_lk(match, dt_rval_gconf_shm , (__d_drt_h) mppd, "%hhu");
    }
  else if (!strncmp(match, _MC_GCONF_E_LF, 24))
    {
      return as_ref_to_val_lk(match, dt_rval_gconf_e_lf , (__d_drt_h) mppd, "%s");
    }
  else if (!strncmp(match, _MC_GCONF_E_OLF, 22))
    {
      return as_ref_to_val_lk(match, dt_rval_gconf_e_olf , (__d_drt_h) mppd, "%hhu");
    }
  else if (!strncmp(match, _MC_GCONF_E_M, 18))
    {
      return as_ref_to_val_lk(match, dt_rval_gconf_e_m , (__d_drt_h) mppd, "%s");
    }
  else if (!strncmp(match, _MC_GCONF_R_SKIPBDIR, 14))
    {
      return as_ref_to_val_lk(match, dt_rval_gconf_sbdir , (__d_drt_h) mppd, "%s");
    }
  else if (!strncmp(match, _MC_GCONF_EX_U, 14))
    {
      return as_ref_to_val_lk(match, dt_rval_gconf_ex_u , (__d_drt_h) mppd, "%s");
    }
  else if (!strncmp(match, _MC_GCONF_EX_UF, 20))
    {
      return as_ref_to_val_lk(match, dt_rval_gconf_ex_uf , (__d_drt_h) mppd, "%s");
    }

  return NULL;
}

int
gcb_gconf(void *buffer, char *key, char *val)
{
  size_t k_l = strlen(key), v_l;
  __d_gconf ptr = (__d_gconf) buffer;
  errno = 0;

  if (k_l == 12 && !strncmp(key, _MC_GCONF_R_CLEAN, 12))
    {
      if (!(v_l = strlen(val)))
        {
          return -1;
        }
      memcpy(ptr->r_clean, val,
          v_l >= GCONF_MAX_REG_EXPR ? GCONF_MAX_REG_EXPR - 1 : v_l);
      return -1;
    }
  else if (k_l == 15 && !strncmp(key, _MC_GCONF_R_POSTPROC, 15))
    {
      if (!(v_l = strlen(val)))
        {
          return -1;
        }
      memcpy(ptr->r_postproc, val,
          v_l >= GCONF_MAX_REG_EXPR ? GCONF_MAX_REG_EXPR - 1 : v_l);
      return -1;
    }
  else if (k_l == 14 && !strncmp(key, _MC_GCONF_R_YEARM, 14))
    {
      if (!(v_l = strlen(val)))
        {
          return -1;
        }
      memcpy(ptr->r_yearm, val,
          v_l >= GCONF_MAX_REG_EXPR ? GCONF_MAX_REG_EXPR - 1 : v_l);
      return -1;
    }
  else if (k_l == 5 && !strncmp(key, _MC_GCONF_R_SECTS, 5))
    {
      if (!(v_l = strlen(val)))
        {
          return 0;
        }
      memcpy(ptr->r_sects, val,
          v_l >= GCONF_MAX_REG_EXPR ? GCONF_MAX_REG_EXPR - 1 : v_l);
      return 1;
    }
  else if (k_l == 14 && !strncmp(key, _MC_GCONF_O_SHM, 14))
    {
      uint8_t v_ui = (uint8_t) strtoul(val, NULL, 10);
      if ( errno == ERANGE)
        {
          return -1;
        }
      ptr->o_use_shared_mem = v_ui;
      return -1;
    }
  else if (k_l == 22 && !strncmp(key, _MC_GCONF_E_OLF, 22))
    {
      uint8_t v_ui = (uint8_t) strtoul(val, NULL, 10);
      if ( errno == ERANGE)
        {
          return -1;
        }
      ptr->o_exec_on_lookup_fail = v_ui;
      return -1;
    }
  else if (k_l == 24 && !strncmp(key, _MC_GCONF_E_LF, 24))
    {
      if (!(v_l = strlen(val)))
        {
          return -1;
        }
      memcpy(ptr->e_lookup_fail, val,
          v_l >= GCONF_MAX_EXEC ? GCONF_MAX_EXEC - 1 : v_l);
      return -1;
    }
  else if (k_l == 18 && !strncmp(key, _MC_GCONF_E_M, 18))
    {
      if (!(v_l = strlen(val)))
        {
          return -1;
        }
      memcpy(ptr->e_match, val,
          v_l >= GCONF_MAX_EXEC ? GCONF_MAX_EXEC - 1 : v_l);
      return -1;
    }
  else if (k_l == 14 && !strncmp(key, _MC_GCONF_R_SKIPBDIR, 14))
    {
      if (!(v_l = strlen(val)))
        {
          return -1;
        }
      memcpy(ptr->r_skip_basedir, val,
          v_l >= GCONF_MAX_EXEC ? GCONF_MAX_EXEC - 1 : v_l);
      return -1;
    }
  else if (k_l == 14 && !strncmp(key, _MC_GCONF_EX_U, 14))
     {
       if (!(v_l = strlen(val)))
         {
           return -1;
         }
       memcpy(ptr->r_exclude_user, val,
           v_l >= GCONF_MAX_REG_USR ? GCONF_MAX_REG_USR - 1 : v_l);
       return -1;
     }
  else if (k_l == 20 && !strncmp(key, _MC_GCONF_EX_UF, 20))
     {
       if (!(v_l = strlen(val)))
         {
           return -1;
         }
       memcpy(ptr->r_exclude_user_flags, val,
           v_l >= GCONF_MAX_UFLAGS ? GCONF_MAX_UFLAGS - 1 : v_l);
       return -1;
     }

  return 0;
}