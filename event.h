#ifndef EVENT_H_
#define EVENT_H_

int EventsService(SDL_Event* const, Player* const, Render_data* const);
int MainMenuEventsService(SDL_Event* const, Render_data* const, Uint8* const);
int ManageScrollsEventsService(SDL_Event* const, Player* const, Render_data* const);
int MenuEventsService(SDL_Event* const, Player* const, Render_data* const);
int EndingEventsService(SDL_Event* const);
int ShopEventsService(SDL_Event* const, Player* const, Render_data* const);

#endif