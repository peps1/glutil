/*
 * i_math.c
 *
 *  Created on: Dec 4, 2013
 *      Author: reboot
 */

#include <glob.h>
#include <str.h>

#include <i_math.h>

#include <l_error.h>

#include <errno.h>

int
g_math_res(void *d_ptr, pmda mdm, void *res)
{
  p_md_obj ptr = mdm->objects;
  __g_math math = (__g_math ) ptr->ptr, p_math = NULL;
  void *c_ptr = NULL;

  p_math = (__g_math ) ptr->ptr;
  ptr = ptr->next;

  uint8_t v_b[8];

  if ((math->flags & F_MATH_VAR_KNOWN))
    {
      c_ptr = &math->vstor;
    }
  else
    {
      bzero((void*) v_b, 8);
      memcpy((void*) v_b, d_ptr + math->l_off, math->vb);
      c_ptr = (void*) v_b;
    }

  memcpy(res, c_ptr, math->vb);

  while (ptr)
    {
      math = (__g_math ) ptr->ptr;

      if ((math->flags & F_MATH_VAR_KNOWN))
        {
          c_ptr = &math->vstor;
        }
      else
        {
          bzero((void*) v_b, 8);
          memcpy((void*) v_b, d_ptr + math->l_off, math->vb);
          c_ptr = (void*) v_b;
        }

      p_math->op_t(res, c_ptr, res);

      p_math = math;
      ptr = ptr->next;

    }

  return 0;
}

int
g_process_math_string(__g_handle hdl, char *string, pmda mdm, int *ret,
    uint32_t flags)
{
  g_setjmp(0, "g_process_math_string", NULL, NULL);
  char *ptr = string, *left, oper = 0x0;
  size_t i;
  while (ptr[0])
    {
      if (ptr[0] == 0x0 || ptr[0] == 0x23 || ptr[0] == 0x7D || ptr[0] == 0x29
          || ptr[0] == 0x3A)
        {
          break;
        }

      while (ptr[0] == 0x20)
        {
          ptr++;
        }

      i = 0;
      left = ptr;

      if (ptr[0] == 0x2B || ptr[0] == 0x2D)
        {
          ptr++;
        }

      while (is_ascii_arith_bin_oper(ptr[0]) && ptr[0] && ptr[0] != 0x23
          && ptr[0] != 0x7D && ptr[0] != 0x29 && ptr[0] != 0x3A)
        {
          i++;
          ptr++;
        }

      if (!i)
        {
          return 1;
        }

      if (ptr[0] && ptr[0] != 0x23 && ptr[0] != 0x7D && ptr[0] != 0x29
          && ptr[0] != 0x3A)
        {
          if (ptr[0] == 0x3C || ptr[0] == 0x3E)
            {
              if (ptr[0] != ptr[1])
                {
                  return 2;
                }
              ptr++;
            }
          oper = ptr[0];
          ptr++;
        }
      else
        {
          oper = 0x0;
        }

      *ret = g_build_math_packet(hdl, left, oper, mdm,
      NULL, flags);

      if (*ret)
        {
          return 6;
        }
    }

  return 0;
}

int
g_build_math_packet(__g_handle hdl, char *field, char oper, pmda mdm,
    __g_math *ret, uint32_t flags)
{
  g_setjmp(0, "g_build_math_packet", NULL, NULL);
  int rt = 0;
  md_init(mdm, 16);

  __g_math p_math = (__g_math ) mdm->pos->ptr;
  __g_math math = (__g_math ) md_alloc(mdm, sizeof(_g_math));

  if (!math)
    {
      rt = 1;
      goto end;
    }

  if (p_math)
    {
      math->flags |= (p_math->flags & F_MATH_TYPES);
    }

  if ((rt = g_get_math_g_t_ptr(hdl, field, math, 0)))
    {
      goto end;
    }

  if (!(math->flags & F_MATH_TYPES))
    {
      rt = 6;
      goto end;
    }

  if (math->_m_p)
    {
      if (oper == 0x2B)
        {
          math->op_t = math->_m_p[0];

        }
      else if (oper == 0x2D)
        {
          math->op_t = math->_m_p[1];
        }
      else if (oper == 0x2A)
        {
          math->op_t = math->_m_p[2];
        }
      else if (oper == 0x2F)
        {
          math->op_t = math->_m_p[3];
        }
      else if (oper == 0x25)
        {
          if ((math->flags & F_MATH_FLOAT))
            {
              rt = 14;
              goto end;
            }
          math->op_t = math->_m_p[4];
        }
      else if (oper == 0x26)
        {
          if ((math->flags & F_MATH_FLOAT))
            {
              rt = 15;
              goto end;
            }
          math->op_t = math->_m_p[5];
        }
      else if (oper == 0x7C)
        {
          if ((math->flags & F_MATH_FLOAT))
            {
              rt = 16;
              goto end;
            }
          math->op_t = math->_m_p[6];
        }
      else if (oper == 0x5E)
        {
          if ((math->flags & F_MATH_FLOAT))
            {
              rt = 17;
              goto end;
            }
          math->op_t = math->_m_p[7];
        }
      else if (oper == 0x3C)
        {
          if ((math->flags & F_MATH_FLOAT))
            {
              rt = 17;
              goto end;
            }
          math->op_t = math->_m_p[8];
        }
      else if (oper == 0x3E)
        {
          if ((math->flags & F_MATH_FLOAT))
            {
              rt = 17;
              goto end;
            }
          math->op_t = math->_m_p[9];
        }
      else
        {
          math->op_t = NULL;
        }
    }

  if (ret)
    {
      *ret = math;
    }

  end:

  if (rt)
    {
      md_unlink(mdm, mdm->pos);
    }

  return rt;
}

int
g_get_math_g_t_ptr(__g_handle hdl, char *field, __g_math math, uint32_t flags)
{
  g_setjmp(0, "g_get_math_g_t_ptr", NULL, NULL);

  int vb = 0;

  size_t off = (size_t) hdl->g_proc2(hdl->_x_ref, field, &vb);

  if (!vb)
    {
      errno = 0;
      uint32_t t_f = 0;
      int base = 10;

      if (field[0] == 0x30 && (field[1] == 0x78 || field[1] == 0x58))
        {
          if (math->flags & F_MATH_FLOAT)
            {
              return 201;
            }
          field = &field[2];
          base = 16;
        }

      if (!(math->flags & F_MATH_TYPES))
        {
          if (field[0] == 0x2D || field[0] == 0x2B)
            {
              math->flags |= F_MATH_INT_S;
            }
          else if (base == 10 && s_char(field, 0x2E, g_floatstrlen(field)))
            {
              math->flags |= F_MATH_FLOAT;
            }
          else
            {
              math->flags |= F_MATH_INT;
            }
        }

      void *v_stor = &math->vstor;

      switch (math->flags & F_MATH_TYPES)
        {
      case F_MATH_INT:
        *((uint64_t*) v_stor) = (uint64_t) strtoull(field, NULL, base);
        math->_m_p = _m_u64;
        math->vb = sizeof(uint64_t);
        break;
      case F_MATH_INT_S:
        *((int64_t*) v_stor) = (int64_t) strtoll(field, NULL, base);
        math->_m_p = _m_s64;
        math->vb = sizeof(int64_t);
        break;
      case F_MATH_FLOAT:
        *((float*) v_stor) = (float) strtof(field, NULL);
        math->_m_p = _m_f;
        math->vb = sizeof(float);
        break;
        }

      t_f |= F_MATH_VAR_KNOWN;

      if (errno == ERANGE)
        {
          return 4;
        }

      math->flags |= t_f;

      return 0;
    }

  if (off > hdl->block_sz)
    {
      return 601;
    }

  switch (vb)
    {
  case -32:
    math->_m_p = _m_f;
    math->flags |= F_MATH_FLOAT;
    math->vb = sizeof(float);
    break;
  case -2:
    math->_m_p = _m_s64;
    math->flags |= F_MATH_INT_S;
    math->vb = sizeof(int8_t);
    break;
  case -3:
    math->_m_p = _m_s64;
    math->flags |= F_MATH_INT_S;
    math->vb = sizeof(int16_t);
    break;
  case -5:
    math->_m_p = _m_s64;
    math->flags |= F_MATH_INT_S;
    math->vb = sizeof(int32_t);
    break;
  case -9:
    math->_m_p = _m_s64;
    math->flags |= F_MATH_INT_S;
    math->vb = sizeof(int64_t);
    break;
  case 1:
    math->_m_p = _m_u64;
    math->flags |= F_MATH_INT;
    math->vb = sizeof(uint8_t);
    break;
  case 2:
    math->_m_p = _m_u64;
    math->flags |= F_MATH_INT;
    math->vb = sizeof(uint16_t);
    break;
  case 4:
    math->_m_p = _m_u64;
    math->flags |= F_MATH_INT;
    math->vb = sizeof(uint32_t);
    break;
  case 8:
    math->_m_p = _m_u64;
    math->flags |= F_MATH_INT;
    math->vb = sizeof(uint64_t);
    break;
  default:
    return 608;
    break;
    }

  math->l_off = off;

  return 0;

}

/*
 void

 g_arith_add_u8(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint8_t*) s) + *((uint8_t*) d);
 }

 void
 g_arith_rem_u8(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint8_t*) s) - *((uint8_t*) d);
 }

 void
 g_arith_mult_u8(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint8_t*) s) * *((uint8_t*) d);
 }

 void
 g_arith_div_u8(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint8_t*) s) / *((uint8_t*) d);
 }

 void
 g_arith_mod_u8(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint8_t*) s) % *((uint8_t*) d);
 }

 static void *_m_u8[] =
 { g_arith_add_u8, g_arith_rem_u8, g_arith_mult_u8, g_arith_div_u8,
 g_arith_mod_u8 };

 void
 g_arith_add_u16(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint16_t*) s) + *((uint16_t*) d);
 }

 void
 g_arith_rem_u16(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint16_t*) s) - *((uint16_t*) d);
 }

 void
 g_arith_mult_u16(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint16_t*) s) * *((uint16_t*) d);
 }

 void
 g_arith_div_u16(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint16_t*) s) / *((uint16_t*) d);
 }

 void
 g_arith_mod_u16(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint16_t*) s) % *((uint16_t*) d);
 }

 static void *_m_u16[] =
 { g_arith_add_u16, g_arith_rem_u16, g_arith_mult_u16, g_arith_div_u16,
 g_arith_mod_u16 };

 void
 g_arith_add_u32(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint32_t*) s) + *((uint32_t*) d);
 }

 void
 g_arith_rem_u32(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint32_t*) s) - *((uint32_t*) d);
 }

 void
 g_arith_mult_u32(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint32_t*) s) * *((uint32_t*) d);
 }

 void
 g_arith_div_u32(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint32_t*) s) / *((uint32_t*) d);
 }

 void
 g_arith_mod_u32(void * s, void * d, void *o)
 {
 *((uint64_t*) o) = *((uint32_t*) s) % *((uint32_t*) d);
 }

 static void *_m_u32[] =
 { g_arith_add_u32, g_arith_rem_u32, g_arith_mult_u32, g_arith_div_u32,
 g_arith_mod_u32 };

 */

void
g_arith_add_u64(void * s, void * d, void *o)
{
  *((uint64_t*) o) = *((uint64_t*) s) + *((uint64_t*) d);
}

void
g_arith_rem_u64(void * s, void * d, void *o)
{
  *((uint64_t*) o) = *((uint64_t*) s) - *((uint64_t*) d);
}

void
g_arith_mult_u64(void * s, void * d, void *o)
{
  *((uint64_t*) o) = *((uint64_t*) s) * *((uint64_t*) d);
}

void
g_arith_div_u64(void * s, void * d, void *o)
{
  *((uint64_t*) o) = *((uint64_t*) s) / *((uint64_t*) d);
}

void
g_arith_mod_u64(void * s, void * d, void *o)
{
  *((uint64_t*) o) = *((uint64_t*) s) % *((uint64_t*) d);
}

void
g_arith_bin_and_u64(void * s, void * d, void *o)
{
  *((uint64_t*) o) = *((uint64_t*) s) & *((uint64_t*) d);
}
void
g_arith_bin_or_u64(void * s, void * d, void *o)
{
  *((uint64_t*) o) = *((uint64_t*) s) | *((uint64_t*) d);
}

void
g_arith_bin_xor_u64(void * s, void * d, void *o)
{
  *((uint64_t*) o) = *((uint64_t*) s) ^ *((uint64_t*) d);
}

void
g_arith_bin_lshift_u64(void * s, void * d, void *o)
{
  *((uint64_t*) o) = *((uint64_t*) s) << *((uint64_t*) d);
}

void
g_arith_bin_rshift_u64(void * s, void * d, void *o)
{
  *((uint64_t*) o) = *((uint64_t*) s) >> *((uint64_t*) d);
}

static void *_m_u64[] =
  { g_arith_add_u64, g_arith_rem_u64, g_arith_mult_u64, g_arith_div_u64,
      g_arith_mod_u64, g_arith_bin_and_u64, g_arith_bin_or_u64,
      g_arith_bin_xor_u64, g_arith_bin_lshift_u64, g_arith_bin_rshift_u64 };

/*

 void
 g_arith_add_s8(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int8_t*) s) + *((int8_t*) d);
 }

 void
 g_arith_rem_s8(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int8_t*) s) - *((int8_t*) d);
 }

 void
 g_arith_mult_s8(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int8_t*) s) * *((int8_t*) d);
 }

 void
 g_arith_div_s8(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int8_t*) s) / *((int8_t*) d);
 }

 void
 g_arith_mod_s8(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int8_t*) s) % *((int8_t*) d);
 }

 static void *_m_s8[] =
 { g_arith_add_s8, g_arith_rem_s8, g_arith_mult_s8, g_arith_div_s8,
 g_arith_mod_s8 };

 void
 g_arith_add_s16(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int16_t*) s) + *((int16_t*) d);
 }

 void
 g_arith_rem_s16(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int16_t*) s) - *((int16_t*) d);
 }

 void
 g_arith_mult_s16(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int16_t*) s) * *((int16_t*) d);
 }

 void
 g_arith_div_s16(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int16_t*) s) / *((int16_t*) d);
 }

 void
 g_arith_mod_s16(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int16_t*) s) % *((int16_t*) d);
 }

 static void *_m_s16[] =
 { g_arith_add_s16, g_arith_rem_s16, g_arith_mult_s16, g_arith_div_s16,
 g_arith_mod_s16 };

 void
 g_arith_add_s32(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int32_t*) s) + *((int32_t*) d);
 }

 void
 g_arith_rem_s32(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int32_t*) s) - *((int32_t*) d);
 }

 void
 g_arith_mult_s32(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int32_t*) s) * *((int32_t*) d);
 }

 void
 g_arith_div_s32(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int32_t*) s) / *((int32_t*) d);
 }

 void
 g_arith_mod_s32(void * s, void * d, void *o)
 {
 *((int64_t*) o) = *((int32_t*) s) % *((int32_t*) d);
 }

 static void *_m_s32[] =
 { g_arith_add_s32, g_arith_rem_s32, g_arith_mult_s32, g_arith_div_s32,
 g_arith_mod_s32 };*/

void
g_arith_add_s64(void * s, void * d, void *o)
{
  *((int64_t*) o) = *((int64_t*) s) + *((int64_t*) d);
}

void
g_arith_rem_s64(void * s, void * d, void *o)
{
  *((int64_t*) o) = *((int64_t*) s) - *((int64_t*) d);
}

void
g_arith_mult_s64(void * s, void * d, void *o)
{
  *((int64_t*) o) = *((int64_t*) s) * *((int64_t*) d);
}

void
g_arith_div_s64(void * s, void * d, void *o)
{
  *((int64_t*) o) = *((int64_t*) s) / *((int64_t*) d);
}

void
g_arith_mod_s64(void * s, void * d, void *o)
{
  *((int64_t*) o) = *((int64_t*) s) % *((int64_t*) d);
}

void
g_arith_bin_and_s64(void * s, void * d, void *o)
{
  *((int64_t*) o) = *((int64_t*) s) & *((int64_t*) d);
}

void
g_arith_bin_or_s64(void * s, void * d, void *o)
{
  *((int64_t*) o) = *((int64_t*) s) | *((int64_t*) d);
}

void
g_arith_bin_xor_s64(void * s, void * d, void *o)
{
  *((int64_t*) o) = *((int64_t*) s) ^ *((int64_t*) d);
}

void
g_arith_bin_lshift_s64(void * s, void * d, void *o)
{
  *((int64_t*) o) = *((int64_t*) s) << *((int64_t*) d);
}

void
g_arith_bin_rshift_s64(void * s, void * d, void *o)
{
  *((int64_t*) o) = *((int64_t*) s) << *((int64_t*) d);
}

static void *_m_s64[] =
  { g_arith_add_s64, g_arith_rem_s64, g_arith_mult_s64, g_arith_div_s64,
      g_arith_mod_s64, g_arith_bin_and_s64, g_arith_bin_or_s64,
      g_arith_bin_xor_s64, g_arith_bin_lshift_s64, g_arith_bin_rshift_s64 };


void
g_arith_add_f(void * s, void * d, void *o)
{
  *((float*) o) = *((float*) s) + *((float*) d);
}

void
g_arith_rem_f(void * s, void * d, void *o)
{
  *((float*) o) = *((float*) s) - *((float*) d);
}

void
g_arith_mult_f(void * s, void * d, void *o)
{
  *((float*) o) = *((float*) s) * *((float*) d);
}

void
g_arith_div_f(void * s, void * d, void *o)
{
  *((float*) o) = *((float*) s) / *((float*) d);
}

void
g_arith_dummy(void * s, void * d, void *o)
{
  *((float*) o) = 1.0;
}

static void *_m_f[] =
      { g_arith_add_f, g_arith_rem_f, g_arith_mult_f, g_arith_div_f,
          g_arith_dummy, g_arith_dummy, g_arith_dummy, g_arith_dummy,
          g_arith_dummy, g_arith_dummy };