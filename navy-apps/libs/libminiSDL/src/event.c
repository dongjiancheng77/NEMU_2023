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

int SDL_PollEvent(SDL_Event *ev)
{
  return 0;
}

int SDL_WaitEvent(SDL_Event *event)
{
  char buf[32];
  // printf("1");
  while (NDL_PollEvent(buf, 32) == 0)
    ;
  printf("2 %s \n", buf);
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
