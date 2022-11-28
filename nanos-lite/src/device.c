#include <common.h>
// #include <fs.h>
#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
#define MULTIPROGRAM_YIELD() yield()
#else
#define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
    [AM_KEY_NONE] = "NONE",
    AM_KEYS(NAME)};
int fg_pcb;
size_t serial_write(const void *buf, size_t offset, size_t len)
{
  // return 0;
  char *str = (char *)buf;
  for (size_t i = 0; i < len; ++i)
  {
    putch(str[i]);
  }
  return len;
}

void switch_program_index(int new_index);
size_t events_read(void *buf, size_t offset, size_t len)
{
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE) return 0;
  
  switch (ev.keycode){
  case AM_KEY_F1:
    switch_program_index(1);
    return 0;

  case AM_KEY_F2:
    switch_program_index(2);
    return 0;

  case AM_KEY_F3:
    switch_program_index(3);
    return 0;
  
  default:
    break;
  }

  //int real_length = 4;
  char *tag = ev.keydown ? "kd " : "ku ";
  //if (real_length <= len){
  strcpy(buf, tag);
  // }else {
  //   assert(0);
  //   return 0;
  // }
  
  //real_length += strlen(keyname[ev.keycode]);
  
  //if (real_length<= len){
  strcat(buf, keyname[ev.keycode]);
  // }else {
  //   Log("Need %d for %s%s but got %d", strlen(keyname[ev.keycode]), (char *)buf, keyname[ev.keycode], len);
  //   assert(0);
  //   return 0;
  // }
  Log("Got  (kbd): %s (%d) %s\n", keyname[ev.keycode], ev.keycode, ev.keydown ? "DOWN" : "UP");
  
  return 1;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len)
{
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len)
{
  return 0;
}

void init_device()
{
  Log("Initializing devices...");
  ioe_init();
}
