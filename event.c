#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <event.h>
#include <enum.h>
#include <render.h>

bool EventsService(SDL_Event* const e, Player* const p, SDL_Window* window) {
	while (SDL_PollEvent(e)) {
		if (e->type == SDL_EVENT_QUIT) {
			return true;
		}
		else if (e->type == SDL_EVENT_KEY_DOWN) {
			switch (e->key.scancode) {
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
			case SDL_SCANCODE_LALT:
				p->control_flags |= run; break;
			case SDL_SCANCODE_H:
				p->control_flags ^= tmp0; break;
			// case SDL_SCANCODE_1:
			// 	p->control_flags &= ~(1 << 9); break;
			// case SDL_SCANCODE_2:
			// 	p->control_flags |= 1 << 9; break;
			case SDL_SCANCODE_ESCAPE:
				return true;
			default: break;
			}
		}
		else if (e->type == SDL_EVENT_KEY_UP) {
			switch (e->key.scancode) {
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
			case SDL_SCANCODE_LALT:
				p->control_flags &= ~(run); break;
			//case SDL_SCANCODE_H:
			//	p->control_flags &= ~(1 << 6); break;
			case SDL_SCANCODE_KP_PLUS:
				SetRenderData(rend_data.window_w + 10, rend_data.window_h + 10);
				SDL_SetWindowSize(window, rend_data.window_w, rend_data.window_h);
				SDL_SyncWindow(window); break;
			case SDL_SCANCODE_KP_MINUS:
				SetRenderData(rend_data.window_w - 10, rend_data.window_h - 10);
				SDL_SetWindowSize(window, rend_data.window_w, rend_data.window_h);
				SDL_SyncWindow(window);  break;
			case SDL_SCANCODE_KP_8:
				int w; int h;
				SDL_SetWindowFullscreen(window, true);
				SDL_SyncWindow(window); 
				SDL_GetWindowSize(window, &w, &h);
				SetRenderData(w, h); break;
			case SDL_SCANCODE_KP_2:
				SDL_SetWindowFullscreen(window, false);
				SDL_SyncWindow(window); 
				SDL_GetWindowSize(window, &w, &h);
				SetRenderData(w, h); break;
			default: break;
			}
		}
		else if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			switch (e->button.button) {
				case SDL_BUTTON_LEFT:
					p->control_flags ^= attack; break;
				case SDL_BUTTON_RIGHT:
					p->control_flags ^= fire; break;
				default: break;
			}
		}
		else if (e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
			switch (e->button.button) {
				case SDL_BUTTON_LEFT:
					p->control_flags &= ~(attack); break;
				case SDL_BUTTON_RIGHT:
					p->control_flags &= ~(fire); break;
				default: break;
			}
		}
		else if (e->type == SDL_EVENT_MOUSE_MOTION) {
			p->direction += e->motion.xrel * ((e->motion.y / (float)WINDOW_H + 0.125F) * ROTATION_SPEED);
		}
	}
	return false;
}
