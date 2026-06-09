#include <common.h>
#include <event.h>
#include <sound.h>

int EventsService(Game_data *const gd, Player *const pc){
	SDL_Event *const ev = gd->ev_ptr;
	Render_data* const rend_data = gd->rend_data_ptr;
	while(SDL_PollEvent(ev)){
		if(ev->type == SDL_EVENT_KEY_DOWN){
			switch(ev->key.scancode){
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
				pc->flags ^= range_mode;
				if(!(pc->flags & range_mode)){
					PlaySoundRand(gd->snd_data_ptr, snd_draw0, snd_draw_last);
				}else{
					PlaySoundRand(gd->snd_data_ptr, snd_paper0, snd_paper_last);
				}
				break;
			case SDL_SCANCODE_1:
				if(pc->flags & range_mode){
					PlaySoundRand(gd->snd_data_ptr, snd_draw0, snd_draw_last);
					pc->flags &= ~(range_mode);
				}
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
				{const Uint8 new_selection = *(pc->scrolls_quick_access + (int)ev->key.scancode - SDL_SCANCODE_2);
				if(!(pc->flags & range_mode)){
					PlaySoundRand(gd->snd_data_ptr, snd_paper0, snd_paper_last);
					pc->flags |= range_mode;
				}else if(new_selection != pc->selected_scroll){
					PlaySoundRand(gd->snd_data_ptr, snd_paper0, snd_paper_last);
				}
				pc->selected_scroll = new_selection;}
				break;
			case KEY_MANAGE_SCROLLS:
				ToMenuMouseMode(rend_data);
				return event_manage_scrolls;
			case KEY_SHOW_MAP:
				pc->flags ^= map_look; break;
			case SDL_SCANCODE_ESCAPE:
				ToMenuMouseMode(rend_data);
				return event_menu;
			default: break;
			}
		}else if(ev->type == SDL_EVENT_KEY_UP){
			switch (ev->key.scancode){
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
		}else if(ev->type == SDL_EVENT_MOUSE_BUTTON_DOWN){
			switch (ev->button.button){
			case BUTTON_ATTACK:
				pc->flags |= attack;
				break;
			case BUTTON_BLOCK:
				pc->flags |= block; break;
			default: break;
			}
		}else if(ev->type == SDL_EVENT_MOUSE_BUTTON_UP){
			switch (ev->button.button){
			case BUTTON_ATTACK:
				if(pc->flags & attack){
					pc->flags &= ~(attack);
					if((pc->flags & range_mode) && pc->selected_scroll != scroll_empty){
						pc->flags |= cast;
					}
				} break;
			case BUTTON_BLOCK:
				pc->flags &= ~(block); break;
			default: break;
			}
		}else if(ev->type == SDL_EVENT_MOUSE_MOTION){
			UpdateMouse(gd, ev->motion.xrel, ev->motion.yrel);
		}else if(ev->type == SDL_EVENT_QUIT){
			return event_quit_game;
		}
	}
	return event_ok;
}

unsigned int MenuEventsService(Game_data *const gd, unsigned int *const menu_position, const unsigned int options_num){
	SDL_Event *const ev = gd->ev_ptr;
	Render_data *const rend_data = gd->rend_data_ptr;
	while(SDL_PollEvent(ev)){
		if(ev->type == SDL_EVENT_KEY_DOWN){
			switch(ev->key.scancode){
			case KEY_SELECT:
			case SDL_SCANCODE_RETURN:
				PlayGameSound(gd->snd_data_ptr, snd_menu_select_last);
				return *menu_position;
				break;
			case KEY_MOVE_BACK:
			case SDL_SCANCODE_DOWN:
			case KEY_MOVE_RIGHT:
			case SDL_SCANCODE_RIGHT:
				if(*menu_position < options_num - 1U){
					PlayGameSound(gd->snd_data_ptr, snd_menu_move_last);
					++(*menu_position);
				}
				break;
			case KEY_MOVE_FORWARD:
			case SDL_SCANCODE_UP:
			case KEY_MOVE_LEFT:
			case SDL_SCANCODE_LEFT:
				if(*menu_position > 0U){
					PlayGameSound(gd->snd_data_ptr, snd_menu_move_last);
					--(*menu_position);
				}
				break;
			case SDL_SCANCODE_ESCAPE:
				if(options_num - 1U != *menu_position){
					PlayGameSound(gd->snd_data_ptr, snd_menu_move_last);
					*menu_position = options_num - 1U;
				}
				return options_num + 1U;
				break;
			default: break;
			}
		}else if(ev->type == SDL_EVENT_MOUSE_BUTTON_UP){
			if(BUTTON_ATTACK == ev->button.button){
				PlayGameSound(gd->snd_data_ptr, snd_menu_select_last);
				SetPointedOptionMouseSelection(rend_data, menu_position, options_num);
				return *menu_position;
			}
		}else if(ev->type == SDL_EVENT_MOUSE_MOTION || ev->type == SDL_EVENT_MOUSE_BUTTON_DOWN){
			UpdateMouse(gd, ev->motion.xrel, ev->motion.yrel);
			const unsigned int old_position = *menu_position;
			SetPointedOptionMouseSelection(rend_data, menu_position, options_num);
			if(old_position != *menu_position){
				PlayGameSound(gd->snd_data_ptr, snd_menu_move_last);
			}
		}
	}
	return options_num;
}

int ManageScrollsEventsService(Game_data *const gd){
	SDL_Event *const ev = gd->ev_ptr;
	Render_data *const rend_data = gd->rend_data_ptr;
	Player *const pc = host(gd);
	while(SDL_PollEvent(ev)){
		if(ev->type == SDL_EVENT_KEY_DOWN){
			switch(ev->key.scancode){
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
				SetQuickScroll(pc, (int)ev->key.scancode - SDL_SCANCODE_2); break;
			case KEY_MANAGE_SCROLLS:
				ToGameMouseMode(rend_data);
				return event_ok;
			default: break;
			}
		}else if(ev->type == SDL_EVENT_KEY_UP){
			switch (ev->key.scancode){
			case SDL_SCANCODE_ESCAPE:
				ToGameMouseMode(rend_data);
				return event_ok;
			default: break;
			}
		}else if(ev->type == SDL_EVENT_MOUSE_BUTTON_UP){
			SetSelectetScrollMouseSelection(rend_data, pc);
		}else if(ev->type == SDL_EVENT_MOUSE_MOTION){
			UpdateMouse(gd, ev->motion.xrel, ev->motion.yrel);
			SetPointedScrollMouseSelection(rend_data, pc);
		}
	}
	pc->help_data.menu_position += SCROLLS_NUM;
	pc->help_data.menu_position %= SCROLLS_NUM;
	return event_manage_scrolls;
}

int EndingEventsService(SDL_Event* const ev){
	while(SDL_PollEvent(ev)){
		if(ev->type == SDL_EVENT_KEY_UP || ev->type == SDL_EVENT_MOUSE_BUTTON_UP){
			return 1;
		}
	}
	return 0;
}

int ShopEventsService(Game_data *const gd){
	SDL_Event *const ev = gd->ev_ptr;
	Render_data *const rend_data = gd->rend_data_ptr;
	Player *const pc = host(gd);
	while(SDL_PollEvent(ev)){
		if(ev->type == SDL_EVENT_KEY_DOWN){
			switch(ev->key.scancode){
			case KEY_SELECT:
			case SDL_SCANCODE_RETURN:
				return opt_select;
			default: break;
			}
		}else if(ev->type == SDL_EVENT_KEY_UP){
			switch (ev->key.scancode){
			case SDL_SCANCODE_ESCAPE:
				return opt_exit;
			default: break;
			}
		}else if(ev->type == SDL_EVENT_MOUSE_BUTTON_UP){
			return opt_select;
		}else if(ev->type == SDL_EVENT_MOUSE_MOTION){
			UpdateMouse(gd, ev->motion.xrel, ev->motion.yrel);
			pc->help_data.menu_position = GetMouseShopSelection(rend_data);
		}
	}
	return opt_void;
}
