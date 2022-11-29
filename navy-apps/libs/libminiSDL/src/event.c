#include <NDL.h>
#include <SDL.h>

#define keyname(k) #k,
#define NR_KEYS (sizeof(keyname) / sizeof(keyname[0]))
static const char *keyname[] = {
    "NONE",
    _KEYS(keyname)};
static uint8_t keysnap[NR_KEYS] = {0};
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
  memset(buf, 0, 32);
  printf("1event is %s in SDL\n", buf);
  while (NDL_PollEvent(buf, 32) == 0)
  {
  };
  printf("event is %s in SDL\n", buf);
  if (strncmp(buf, "kd", 2) == 0)
    event->type = SDL_KEYDOWN;
  else if (strncmp(buf, "ku", 2) == 0)
    event->type = SDL_KEYUP;

  for (size_t i = 0; i < (sizeof(keyname) / sizeof(keyname[0])); ++i)
  {
    if (((strlen(buf + 3) - 1) == strlen(keyname[i])) && (strncmp(buf + 3, keyname[i], strlen(keyname[i])) == 0))
    {
      event->key.keysym.sym = i;
      keysnap[i] = (event->type == SDL_KEYDOWN) ? 1 : 0;
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
