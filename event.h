#ifndef EVENT_H_
#define EVENT_H_

extern Render_data rend_data;

bool EventsService(SDL_Event* const, Player* const, SDL_Window* const);
int MenuEventsService(SDL_Event* const, SDL_Window* const);

#endif