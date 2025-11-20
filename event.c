#include <SDL3/SDL.h>
#include <macros.h>
#include <enum.h>
#include <types.h>
#include <event.h>
#include <render.h>
#include <Player.h>

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
			case KEY_FULLSCR:
				SDL_SetWindowFullscreen(rend_data->window, true);
				SDL_SyncWindow(rend_data->window);
				ResetRenderData(rend_data);
				break;
			case KEY_WINDOWED:
				SDL_SetWindowFullscreen(rend_data->window, false);
				SDL_SyncWindow(rend_data->window);
				ResetRenderData(rend_data);
				break;
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
			case SDL_SCANCODE_ESCAPE:
				return 1;
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

int MenuEventsService(SDL_Event* const e, Render_data* const rend_data){
	while (SDL_PollEvent(e)){
		if(e->type == SDL_EVENT_KEY_UP){
			switch (e->key.scancode){
			case SDL_SCANCODE_ESCAPE:
				return menu_quit;
			case SDL_SCANCODE_SPACE:
				return menu_start;
				break;
			default: break;
			}
		}else if(e->type == SDL_EVENT_QUIT){
			return menu_quit;
		}
	}
	return menu_unknown;
}

int ManageScrollsEventsService(SDL_Event* const e, Player* const p, Render_data* const rend_data){
	while(SDL_PollEvent(e)){
		if(e->type == SDL_EVENT_KEY_DOWN){
			switch(e->key.scancode){
			case KEY_SELECT:
			case SDL_SCANCODE_RETURN:
				p->selected_scroll = p->help_data.menu_position; break;
			case KEY_MOVE_RIGHT:
			case SDL_SCANCODE_RIGHT:
				++p->help_data.menu_position; break;
			case KEY_MOVE_LEFT:
			case SDL_SCANCODE_LEFT:
				--p->help_data.menu_position; break;
			case KEY_MOVE_BACK:
			case SDL_SCANCODE_DOWN:
				p->help_data.menu_position += ICONS_IN_VIEWF_ROW; break;
			case KEY_MOVE_FORWARD:
			case SDL_SCANCODE_UP:
				p->help_data.menu_position -= ICONS_IN_VIEWF_ROW; break;
			case SDL_SCANCODE_2:
			case SDL_SCANCODE_3:
			case SDL_SCANCODE_4:
			case SDL_SCANCODE_5:
			case SDL_SCANCODE_6:
			case SDL_SCANCODE_7:
			case SDL_SCANCODE_8:
			case SDL_SCANCODE_9:
			case SDL_SCANCODE_0:
				SetQuickScroll(p, (int)e->key.scancode - 31); break;
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
			SetSelectetScrollMouseSelection(rend_data, p); break;
		}else if(e->type == SDL_EVENT_MOUSE_MOTION){
			SetPointedScrollMouseSelection(rend_data, p); break;
		}
	}
	return event_manage_scrolls;
}