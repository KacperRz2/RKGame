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
				p->control_flags |= forward; break;
			case KEY_MOVE_BACK:
				p->control_flags |= back; break;
			case KEY_MOVE_RIGHT:
				p->control_flags |= right; break;
			case KEY_MOVE_LEFT:
				p->control_flags |= left; break;
			case KEY_DODGE:
				p->control_flags |= dodge; break;
			case KEY_RUN:
				p->control_flags |= run; break;
			case KEY_ACTION:
				p->control_flags |= action; break;
			case KEY_SWITCH_RANGE:
				p->control_flags ^= range_mode;
				p->control_flags &= ~(attack);
				rend_data->render_flags &= ~(rend_casting);
				break;
			case KEY_TMP:
				p->control_flags ^= tmp0; break;
			case SDL_SCANCODE_1:
				p->control_flags &= ~(range_mode);
				p->control_flags &= ~(attack);
				rend_data->render_flags &= ~(rend_casting);
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
				p->control_flags |= range_mode;
				p->selected_scroll = *(p->scrolls_quick_access + (int)e->key.scancode - 31);
				p->control_flags &= ~(attack);
				rend_data->render_flags &= ~(rend_casting);
				break;
			default: break;
			}
		}else if(e->type == SDL_EVENT_KEY_UP){
			switch (e->key.scancode){
			case KEY_MOVE_FORWARD:
				p->control_flags &= ~(forward); break;
			case KEY_MOVE_BACK:
				p->control_flags &= ~(back); break;
			case KEY_MOVE_RIGHT:
				p->control_flags &= ~(right); break;
			case KEY_MOVE_LEFT:
				p->control_flags &= ~(left); break;
			case KEY_DODGE:
				p->control_flags &= ~(dodge); break;
			case KEY_RUN:
				p->control_flags &= ~(run); break;
			case KEY_ACTION:
				p->control_flags &= ~(action); break;
			case SDL_SCANCODE_ESCAPE:
				return true;
			default: break;
			}
		}else if(e->type == SDL_EVENT_MOUSE_BUTTON_DOWN){
			switch (e->button.button){
				case BUTTON_ATTACK:
					if(!(p->control_flags & range_mode)){
						p->control_flags |= attack;
					}else if(*(p->scrolls + p->selected_scroll) == 0U){
						p->control_flags |= attack;
						rend_data->render_flags |= rend_casting;
					}else{
						rend_data->render_flags |= rend_casting;
					}
					break;
				case BUTTON_BLOCK:
					p->control_flags |= block; break;
				default: break;
			}
		}else if(e->type == SDL_EVENT_MOUSE_BUTTON_UP){
			switch (e->button.button){
				case BUTTON_ATTACK:
					if(!(p->control_flags & range_mode)){
						p->control_flags &= ~(attack);
					}else if(*(p->scrolls + p->selected_scroll) == 0U){
						p->control_flags &= ~(attack);
						rend_data->render_flags &= ~(rend_casting);
					}else{
						p->control_flags |= cast;
						rend_data->render_flags &= ~(rend_casting);
					}
					break;
				case BUTTON_BLOCK:
					p->control_flags &= ~(block); break;
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