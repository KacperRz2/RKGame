#include <SDL3/SDL.h>
#include <macros.h>
#include <enum.h>
#include <types.h>
#include <event.h>
#include <render.h>
#include <Player.h>
#include <game.h>

int EventsService(SDL_Event* const e, Player* const pc, Render_data* const rend_data){
	while(SDL_PollEvent(e)){
		if(e->type == SDL_EVENT_KEY_DOWN){
			switch(e->key.scancode){
			case KEY_MOVE_FORWARD:
				pc->flags |= forward; break;
			case KEY_MOVE_BACK:
				pc->flags |= back; break;
			case KEY_MOVE_RIGHT:
				pc->flags |= right; break;
			case KEY_MOVE_LEFT:
				pc->flags |= left; break;
			case KEY_DODGE:
				pc->flags |= dodge; break;
			case KEY_RUN:
				pc->flags |= run; break;
			case KEY_ACTION:
				pc->flags |= action; break;
			case KEY_SWITCH_RANGE:
				pc->flags ^= range_mode; break;
			case KEY_TMP1:
				pc->flags ^= tmp; break;
			case SDL_SCANCODE_1:
				pc->flags &= ~(range_mode);
				break;
			case SDL_SCANCODE_2:
			case SDL_SCANCODE_3:
			case SDL_SCANCODE_4:
			case SDL_SCANCODE_5:
			case SDL_SCANCODE_6:
			case SDL_SCANCODE_7:
			case SDL_SCANCODE_8:
			case SDL_SCANCODE_9:
			case SDL_SCANCODE_0:
				pc->flags |= range_mode;
				pc->selected_scroll = *(pc->scrolls_quick_access + (int)e->key.scancode - 31);
				break;
			case KEY_MANAGE_SCROLLS:
				SDL_SetWindowRelativeMouseMode(rend_data->window, false);
				return event_manage_scrolls;
			case KEY_SHOW_MAP:
				pc->flags ^= map_look; break;
			case SDL_SCANCODE_ESCAPE:
				SDL_SetWindowRelativeMouseMode(rend_data->window, false);
				return event_menu;
			default: break;
			}
		}else if(e->type == SDL_EVENT_KEY_UP){
			switch (e->key.scancode){
			case KEY_MOVE_FORWARD:
				pc->flags &= ~(forward); break;
			case KEY_MOVE_BACK:
				pc->flags &= ~(back); break;
			case KEY_MOVE_RIGHT:
				pc->flags &= ~(right); break;
			case KEY_MOVE_LEFT:
				pc->flags &= ~(left); break;
			case KEY_DODGE:
				pc->flags &= ~(dodge); break;
			case KEY_RUN:
				pc->flags &= ~(run); break;
			case KEY_ACTION:
				pc->flags &= ~(action); break;
			default: break;
			}
		}else if(e->type == SDL_EVENT_MOUSE_BUTTON_DOWN){
			switch (e->button.button){
				case BUTTON_ATTACK:
					pc->flags |= attack;
					break;
				case BUTTON_BLOCK:
					pc->flags |= block; break;
				default: break;
			}
		}else if(e->type == SDL_EVENT_MOUSE_BUTTON_UP){
			switch (e->button.button){
				case BUTTON_ATTACK:
					if(pc->flags & attack){
						pc->flags &= ~(attack);
						if((pc->flags & range_mode) && pc->selected_scroll != scroll_empty){
							pc->flags |= cast;
						}
					}
					break;
				case BUTTON_BLOCK:
					pc->flags &= ~(block); break;
				default: break;
			}
		}else if(e->type == SDL_EVENT_MOUSE_MOTION){
			pc->direction += e->motion.xrel * ((e->motion.y / (float)rend_data->window_h + 0.125F) * ROTATION_SPEED);
		}else if(e->type == SDL_EVENT_QUIT){
			return 1;
		}
	}
	return 0;
}

int MainMenuEventsService(SDL_Event* const e, Render_data* const rend_data, unsigned int* const menu_position){
	while(SDL_PollEvent(e)){
		if(e->type == SDL_EVENT_KEY_DOWN){
			switch(e->key.scancode){
			case KEY_SELECT:
			case SDL_SCANCODE_RETURN:
				return *menu_position;
				break;
			case KEY_MOVE_BACK:
			case SDL_SCANCODE_DOWN:
			case KEY_MOVE_RIGHT:
			case SDL_SCANCODE_RIGHT:
				++(*menu_position); break;
			case KEY_MOVE_FORWARD:
			case SDL_SCANCODE_UP:
			case KEY_MOVE_LEFT:
			case SDL_SCANCODE_LEFT:
				--(*menu_position); break;
			case SDL_SCANCODE_ESCAPE:
				*menu_position = menu_quit;
				break;
			default: break;
			}
		}else if(e->type == SDL_EVENT_MOUSE_BUTTON_DOWN){
			return *menu_position;
		}else if(e->type == SDL_EVENT_MOUSE_MOTION){
			SetPointedOptionMouseSelection(rend_data, menu_position); break;
		}
	}
	return menu_unknown;
}

int ManageScrollsEventsService(SDL_Event* const e, Player* const pc, Render_data* const rend_data){
	while(SDL_PollEvent(e)){
		if(e->type == SDL_EVENT_KEY_DOWN){
			switch(e->key.scancode){
			case KEY_SELECT:
			case SDL_SCANCODE_RETURN:
				pc->selected_scroll = pc->help_data.menu_position; break;
			case KEY_MOVE_RIGHT:
			case SDL_SCANCODE_RIGHT:
				++pc->help_data.menu_position; break;
			case KEY_MOVE_LEFT:
			case SDL_SCANCODE_LEFT:
				--pc->help_data.menu_position; break;
			case KEY_MOVE_BACK:
			case SDL_SCANCODE_DOWN:
				pc->help_data.menu_position += ICONS_IN_VIEWF_ROW; break;
			case KEY_MOVE_FORWARD:
			case SDL_SCANCODE_UP:
				pc->help_data.menu_position -= ICONS_IN_VIEWF_ROW; break;
			case SDL_SCANCODE_2:
			case SDL_SCANCODE_3:
			case SDL_SCANCODE_4:
			case SDL_SCANCODE_5:
			case SDL_SCANCODE_6:
			case SDL_SCANCODE_7:
			case SDL_SCANCODE_8:
			case SDL_SCANCODE_9:
			case SDL_SCANCODE_0:
				SetQuickScroll(pc, (int)e->key.scancode - 31); break;
			case KEY_MANAGE_SCROLLS:
				SDL_SetWindowRelativeMouseMode(rend_data->window, true);
				return 0;
			default: break;
			}
		}else if(e->type == SDL_EVENT_KEY_UP){
			switch (e->key.scancode){
			case SDL_SCANCODE_ESCAPE:
				SDL_SetWindowRelativeMouseMode(rend_data->window, true);
				return 0;
			default: break;
			}
		}else if(e->type == SDL_EVENT_MOUSE_BUTTON_DOWN){
			SetSelectetScrollMouseSelection(rend_data, pc); break;
		}else if(e->type == SDL_EVENT_MOUSE_MOTION){
			SetPointedScrollMouseSelection(rend_data, pc); break;
		}
	}
	return event_manage_scrolls;
}

int MenuEventsService(Game_data* const gd){
	Player* const pc = human(gd);
	while(SDL_PollEvent(gd->ev_ptr)){
		if(gd->ev_ptr->type == SDL_EVENT_KEY_DOWN){
			switch(gd->ev_ptr->key.scancode){
			case KEY_SELECT:
			case SDL_SCANCODE_RETURN:
				return ActivateMenuOption(pc->help_data.menu_position, gd);
				break;
			case KEY_MOVE_BACK:
			case SDL_SCANCODE_DOWN:
			case KEY_MOVE_RIGHT:
			case SDL_SCANCODE_RIGHT:
				++pc->help_data.menu_position; break;
			case KEY_MOVE_FORWARD:
			case SDL_SCANCODE_UP:
			case KEY_MOVE_LEFT:
			case SDL_SCANCODE_LEFT:
				--pc->help_data.menu_position; break;
			case SDL_SCANCODE_ESCAPE:
				SDL_SetWindowRelativeMouseMode(gd->rend_data_ptr->window, true);
				return 0;
			default: break;
			}
		}else if(gd->ev_ptr->type == SDL_EVENT_MOUSE_BUTTON_DOWN){
			return ActivateMenuOption(pc->help_data.menu_position, gd);
		}else if(gd->ev_ptr->type == SDL_EVENT_MOUSE_MOTION){
			SetPointedOptionMouseSelection(gd->rend_data_ptr, &pc->help_data.menu_position); break;
		}
	}
	return event_menu;
}

int EndingEventsService(SDL_Event* const e){
	while(SDL_PollEvent(e)){
		if(e->type == SDL_EVENT_KEY_UP || e->type == SDL_EVENT_MOUSE_BUTTON_UP){
			return 1;
		}
	}
	return 0;
}

int ShopEventsService(SDL_Event* const e, Player* const pc, Render_data* const rend_data){
	while(SDL_PollEvent(e)){
		if(e->type == SDL_EVENT_KEY_DOWN){
			switch(e->key.scancode){
			case KEY_SELECT:
			case SDL_SCANCODE_RETURN:
				return 1; break;
			default: break;
			}
		}else if(e->type == SDL_EVENT_KEY_UP){
			switch (e->key.scancode){
			case SDL_SCANCODE_ESCAPE:
				return -1;
			default: break;
			}
		}else if(e->type == SDL_EVENT_MOUSE_BUTTON_DOWN){
			return 1; break;
		}else if(e->type == SDL_EVENT_MOUSE_MOTION){
			pc->help_data.menu_position = GetMouseShopSelection(rend_data); break;
		}
	}
	return 0;
}