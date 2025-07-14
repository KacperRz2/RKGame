#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <event.h>
#include <enum.h>

bool EventsService(SDL_Event* const e, Player* const p) {
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
