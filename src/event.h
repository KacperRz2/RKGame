#ifndef EVENT_H_
#define EVENT_H_

enum event{
    event_ok,
    event_quit_game = 1,
    event_menu,
    event_manage_scrolls
};

int EventsService(SDL_Event* const, Player* const, Render_data* const);
int MainMenuEventsService(SDL_Event* const, Render_data* const, unsigned int* const);
int ManageScrollsEventsService(SDL_Event* const, Player* const, Render_data* const);
int MenuEventsService(Game_data* const);
int EndingEventsService(SDL_Event* const);
int ShopEventsService(SDL_Event* const, Player* const, Render_data* const);

#endif
