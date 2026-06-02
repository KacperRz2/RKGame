#ifndef EVENT_H_
#define EVENT_H_

enum event{
    event_ok,
    event_quit_game = 1,
    event_menu,
    event_manage_scrolls,
    event_used_pause
};

int EventsService(Game_data* const, Player* const);
unsigned int MenuEventsService(SDL_Event* const, Render_data* const, unsigned int* const, const unsigned int);
int ManageScrollsEventsService(SDL_Event* const, Player* const, Render_data* const);
int EndingEventsService(SDL_Event* const);
int ShopEventsService(SDL_Event* const, Player* const, Render_data* const);

#endif
