#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <event.h>
#include <enum.h>
#include <render.h>

bool EventsService(SDL_Event* const e, Player* const p, Render_data* const rend_data){
	while (SDL_PollEvent(e)){
		if(e->type == SDL_EVENT_QUIT){
			return true;
		}else if(e->type == SDL_EVENT_KEY_DOWN){
			switch (e->key.scancode){
			case SDL_SCANCODE_W:
				p->control_flags |= forward; break;
			case SDL_SCANCODE_S:
				p->control_flags |= back; break;
			case SDL_SCANCODE_D:
				p->control_flags |= right; break;
			case SDL_SCANCODE_A:
				p->control_flags |= left; break;
			case SDL_SCANCODE_SPACE:
				p->control_flags |= dodge; break;
			// case SDL_SCANCODE_LSHIFT:
			// 	p->control_flags |= cast; break;
			case SDL_SCANCODE_LALT:
				p->control_flags |= run; break;
			case SDL_SCANCODE_E:
				p->control_flags |= action; break;
			case SDL_SCANCODE_Q:
				p->control_flags ^= range_mode; break;
			case SDL_SCANCODE_H:
				p->control_flags ^= tmp0; break;
			case SDL_SCANCODE_1:
				p->control_flags &= ~(range_mode); break;
			case SDL_SCANCODE_2:
				p->selected_scroll = scroll_empty; break;
			case SDL_SCANCODE_3:
				p->selected_scroll = *(p->scrolls_quick_access); break;
			case SDL_SCANCODE_4:
				p->selected_scroll = *(p->scrolls_quick_access + 1); break;
			case SDL_SCANCODE_5:
				p->selected_scroll = *(p->scrolls_quick_access + 2); break;
			case SDL_SCANCODE_6:
				p->selected_scroll = *(p->scrolls_quick_access + 3); break;
			case SDL_SCANCODE_7:
				p->selected_scroll = *(p->scrolls_quick_access + 4); break;
			case SDL_SCANCODE_8:
				p->selected_scroll = *(p->scrolls_quick_access + 5); break;
			case SDL_SCANCODE_9:
				p->selected_scroll = *(p->scrolls_quick_access + 6); break;
			case SDL_SCANCODE_0:
				p->selected_scroll = *(p->scrolls_quick_access + 7); break;
			default: break;
			}
		}else if(e->type == SDL_EVENT_KEY_UP){
			switch (e->key.scancode){
			case SDL_SCANCODE_W:
				p->control_flags &= ~(forward); break;
			case SDL_SCANCODE_S:
				p->control_flags &= ~(back); break;
			case SDL_SCANCODE_D:
				p->control_flags &= ~(right); break;
			case SDL_SCANCODE_A:
				p->control_flags &= ~(left); break;
			case SDL_SCANCODE_SPACE:
				p->control_flags &= ~(dodge); break;
			// case SDL_SCANCODE_LSHIFT:
			// 	p->control_flags |= cast; break;
			case SDL_SCANCODE_LALT:
				p->control_flags &= ~(run); break;
			case SDL_SCANCODE_E:
				p->control_flags &= ~(action); break;
			case SDL_SCANCODE_ESCAPE:
				return true;
			default: break;
			}
		}else if(e->type == SDL_EVENT_MOUSE_BUTTON_DOWN){
			switch (e->button.button){
				case SDL_BUTTON_LEFT:
					if(!(p->control_flags & range_mode)){
						p->control_flags |= attack;
					}else if((p->scrolls + p->selected_scroll)->num == 0U){
						p->control_flags |= attack;
						rend_data->render_flags |= rend_casting;
					}else{
						rend_data->render_flags |= rend_casting;
					}
					break;
				case SDL_BUTTON_RIGHT:
					p->control_flags |= block; break;
				default: break;
			}
		}else if(e->type == SDL_EVENT_MOUSE_BUTTON_UP){
			switch (e->button.button){
				case SDL_BUTTON_LEFT:
					if(!(p->control_flags & range_mode)){
						p->control_flags &= ~(attack);
					}else if((p->scrolls + p->selected_scroll)->num == 0U){
						p->control_flags &= ~(attack);
						rend_data->render_flags &= ~(rend_casting);
					}else{
						p->control_flags |= cast;
						rend_data->render_flags &= ~(rend_casting);
					}
					break;
				case SDL_BUTTON_RIGHT:
					p->control_flags &= ~(block); break;
				default: break;
			}
		}else if(e->type == SDL_EVENT_MOUSE_MOTION){
			p->direction += e->motion.xrel * ((e->motion.y / (float)rend_data->window_h + 0.125F) * ROTATION_SPEED);
		}
	}
	return false;
}

int MenuEventsService(SDL_Event* const e, Render_data* const rend_data){
	while (SDL_PollEvent(e)){
		if(e->type == SDL_EVENT_QUIT){
			return menu_quit;
		}else
		 if(e->type == SDL_EVENT_KEY_UP){
			switch (e->key.scancode){
			case SDL_SCANCODE_ESCAPE:
				return menu_quit;
			case SDL_SCANCODE_SPACE:
				return menu_start;
				break;
			default: break;
			}
		}
	}
	return menu_unknown;
}