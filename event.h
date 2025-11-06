#ifndef EVENT_H_
#define EVENT_H_

int EventsService(SDL_Event* const, Player* const, Render_data* const);
int MenuEventsService(SDL_Event* const, Render_data* const);
int ManageScrollsEventsService(SDL_Event* const, Player* const, Render_data* const);

#endif