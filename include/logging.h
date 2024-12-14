#ifndef __LOGGING__
#define __LOGGING__

#define __mpf_mul(...)               \
  do                                 \
  {                                  \
    mpf_mul(__VA_ARGS__);            \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_add_ui(...)            \
  do                                 \
  {                                  \
    mpf_add_ui(__VA_ARGS__);         \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_mul_ui(...)            \
  do                                 \
  {                                  \
    mpf_mul_ui(__VA_ARGS__);         \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_ui_sub(...)            \
  do                                 \
  {                                  \
    mpf_ui_sub(__VA_ARGS__);         \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_pow_ui(...)            \
  do                                 \
  {                                  \
    mpf_pow_ui(__VA_ARGS__);         \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_add(...)               \
  do                                 \
  {                                  \
    mpf_add(__VA_ARGS__);            \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_div(...)               \
  do                                 \
  {                                  \
    mpf_div(__VA_ARGS__);            \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_ui_div(...)            \
  do                                 \
  {                                  \
    mpf_ui_div(__VA_ARGS__);         \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_mul_si(...)            \
  do                                 \
  {                                  \
    mpf_mul_si(__VA_ARGS__);         \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_ui_pow_ui(...)         \
  do                                 \
  {                                  \
    mpf_ui_pow_ui(__VA_ARGS__);      \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_div_ui(...)            \
  do                                 \
  {                                  \
    mpf_div_ui(__VA_ARGS__);         \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_sub(...)               \
  do                                 \
  {                                  \
    mpf_sub(__VA_ARGS__);            \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_swap(...)              \
  do                                 \
  {                                  \
    mpf_swap(__VA_ARGS__);           \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_qtrt(...)              \
  do                                 \
  {                                  \
    mpf_qtrt(__VA_ARGS__);           \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_set_z(...)             \
  do                                 \
  {                                  \
    mpf_set_z(__VA_ARGS__);          \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_neg(...)               \
  do                                 \
  {                                  \
    mpf_neg(__VA_ARGS__);            \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_init2(...)             \
  do                                 \
  {                                  \
    mpf_init2(__VA_ARGS__);          \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_inits(...)             \
  do                                 \
  {                                  \
    mpf_inits(__VA_ARGS__);          \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_clears(...)            \
  do                                 \
  {                                  \
    mpf_clears(__VA_ARGS__);         \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_clear(...)             \
  do                                 \
  {                                  \
    mpf_clear(__VA_ARGS__);          \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpz_clear(...)             \
  do                                 \
  {                                  \
    mpz_clear(__VA_ARGS__);          \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpz_init(...)              \
  do                                 \
  {                                  \
    mpz_init(__VA_ARGS__);           \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpz_mul_ui(...)            \
  do                                 \
  {                                  \
    mpz_mul_ui(__VA_ARGS__);         \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpz_ui_pow_ui(...)         \
  do                                 \
  {                                  \
    mpz_ui_pow_ui(__VA_ARGS__);      \
    PB(name, &exec_time, times_ptr); \
  } while (0);
#define __mpf_init(...)              \
  do                                 \
  {                                  \
    mpf_init(__VA_ARGS__);           \
    PB(name, &exec_time, times_ptr); \
  } while (0);

#endif // __LOGGING__