#include <NDL.h>
#include <SDL.h>
#include <string.h>
#define keyname(k) #k,
static const char *keyname[] = {
    "NONE",
    _KEYS(keyname)};
int SDL_PushEvent(SDL_Event *ev)
{
  return 0;
}
static char *key_action, *key_key;
static uint8_t key_buf[sizeof(keyname) / sizeof(char *)] = {0};
static int inline read_keyinfo(uint8_t *type, uint8_t *sym){

}
int SDL_PollEvent(SDL_Event *ev)
{
  uint8_t type = 0, sym = 0;
    char key[64];
  int ret = NDL_PollEvent(key, sizeof(key));
  //printf("%s\n", key_buf);
  key_action = key;
  int i;
  for (i = 0; key[i] != ' '; i++){}
  key[i] = '\0';
  key_key = &key[i + 1]; 
  
  //截断\n
  for (i = 0;  key_key[i] != '\0' && key_key[i] != '\n'; i++){}
  if (key_key[i] == '\n'){
    key_key[i] = '\0';
  }
  
  //strcmp("kd", key_action) == 0
  if (key_action[1] == 'd'){//加速！！
    type = SDL_KEYDOWN;
  }else{
    type = SDL_KEYUP;
  }

  for (i = 0; i < sizeof(keyname) / sizeof(char *); ++i){
    //剪枝掉很多
    if (key_key[0] == keyname[i][0] && strcmp(key_key, keyname[i]) == 0){
      sym = i;
      //printf("%d %d\n", *type, *sym);
      break;
    }
  }
  if (ret)
  {
    ev->type = type;
    ev->key.keysym.sym = sym;

    if (ev->type == SDL_KEYDOWN)
      key_buf[sym] = 1;
    else if (ev->type == SDL_KEYUP)
      key_buf[sym] = 0;
  }
  else
  {
    return 0;
  }

  return 1;
}

int SDL_WaitEvent(SDL_Event *event)
{
  char buf[32];
  // printf("1");
  while (NDL_PollEvent(buf, 32) == 0)
    ;
  // printf("2 %s \n", buf);
  if (strncmp(buf, "kd", 2) == 0)
  {
    event->type = SDL_KEYDOWN;
    printf("3%s in SDL\n", buf);
  }
  else if (strncmp(buf, "ku", 2) == 0)
  {
    event->type = SDL_KEYUP;
    // printf("4%s in SDL\n", buf);
  }
  for (int i = 0; i < (sizeof(keyname) / sizeof(char *)); ++i)
  {
    // printf("%s in SDL\n", buf);
    if (((strlen(buf + 3) - 1) == strlen(keyname[i])) && (strncmp(buf + 3, keyname[i], strlen(keyname[i])) == 0))
    {
      event->key.keysym.sym = i;
      if (event->type == SDL_KEYDOWN)
        key_buf[i] = 1;
      else if (event->type == SDL_KEYUP)
        key_buf[i] = 0;
      break;
    }
  }

  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask)
{
  return 0;
}

uint8_t *SDL_GetKeyState(int *numkeys)
{
  return NULL;
}
