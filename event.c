#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <event.h>
#include <enum.h>
#include <render.h>

bool EventsService(SDL_Event* const e, Player* const p, Render_data* const rend_data){
	while(SDL_PollEvent(e)){
		if(e->type == SDL_EVENT_KEY_DOWN){
			switch(e->key.scancode){
			case KEY_MOVE_FORWARD:
				p->flags |= forward; break;
			case KEY_MOVE_BACK:
				p->flags |= back; break;
			case KEY_MOVE_RIGHT:
				p->flags |= right; break;
			case KEY_MOVE_LEFT:
				p->flags |= left; break;
			case KEY_DODGE:
				p->flags |= dodge; break;
			case KEY_RUN:
				p->flags |= run; break;
			case KEY_ACTION:
				p->flags |= action; break;
			case KEY_SWITCH_RANGE:
				p->flags ^= range_mode; break;
			case KEY_TMP:
				p->flags ^= tmp0; break;
			case SDL_SCANCODE_1:
				p->flags &= ~(range_mode);
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
				p->flags |= range_mode;
				p->selected_scroll = *(p->scrolls_quick_access + (int)e->key.scancode - 31);
				break;
			default: break;
			}
		}else if(e->type == SDL_EVENT_KEY_UP){
			switch (e->key.scancode){
			case KEY_MOVE_FORWARD:
				p->flags &= ~(forward); break;
			case KEY_MOVE_BACK:
				p->flags &= ~(back); break;
			case KEY_MOVE_RIGHT:
				p->flags &= ~(right); break;
			case KEY_MOVE_LEFT:
				p->flags &= ~(left); break;
			case KEY_DODGE:
				p->flags &= ~(dodge); break;
			case KEY_RUN:
				p->flags &= ~(run); break;
			case KEY_ACTION:
				p->flags &= ~(action); break;
			case SDL_SCANCODE_ESCAPE:
				return true;
			default: break;
			}
		}else if(e->type == SDL_EVENT_MOUSE_BUTTON_DOWN){
			switch (e->button.button){
				case BUTTON_ATTACK:
					p->flags |= attack;
					break;
				case BUTTON_BLOCK:
					p->flags |= block; break;
				default: break;
			}
		}else if(e->type == SDL_EVENT_MOUSE_BUTTON_UP){
			switch (e->button.button){
				case BUTTON_ATTACK:
					p->flags &= ~(attack);
					if((p->flags & range_mode) && p->selected_scroll != scroll_empty){
						p->flags |= cast;
					}
					break;
				case BUTTON_BLOCK:
					p->flags &= ~(block); break;
				default: break;
			}
		}else if(e->type == SDL_EVENT_MOUSE_MOTION){
			p->direction += e->motion.xrel * ((e->motion.y / (float)rend_data->window_h + 0.125F) * ROTATION_SPEED);
		}else if(e->type == SDL_EVENT_QUIT){
			return true;
		}
	}
	return false;
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