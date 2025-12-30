#include <SDL3/SDL.h>
#include <macros.h>
#include <render_macros.h>
#include <enum.h>
#include <types.h>
#include <render.h>
#include <function.h>
#include <World.h>
#include <Scroll.h>
#include <Being.h>
#include <game.h>

int GraphicsInitiation(Render_data* const rend_data){
	SDL_Surface* surface = NULL;
	char* bmp_path = NULL;
	const char* const texture_files[] = TEXTURE_FILES_NAMES;
	SDL_SetAppMetadata("KacApp", "1.0", NULL);
	if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}
	if(!SDL_CreateWindowAndRenderer("KacWindow", rend_data->window_w, rend_data->window_h, SDL_WINDOW_BORDERLESS, &rend_data->window, &rend_data->renderer)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
		return 3;
	}
	for(int i = 0; i < SDL_arraysize(texture_files); ++i){
		SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), *(texture_files + i));
		surface = SDL_LoadBMP(bmp_path);
		if(!surface){
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
			return 3;
		}
		texture(i) = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
		if(!texture(i)){
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
			return 3;
		}
	}
	SDL_DestroySurface(surface);
	DrawBeings(rend_data, surface, bmp_path);
	SDL_DestroySurface(surface);
	DrawColouredThings(rend_data, surface, bmp_path);
	SDL_DestroySurface(surface);
	SDL_WarpMouseInWindow(rend_data->window, half(rend_data->window_w), half(rend_data->window_h));
	rend_data->mouse_y = half(rend_data->window_h);
	SetMouseBarrier(rend_data);
	DrawBackgroud(rend_data, surface, bmp_path);
	SDL_free(bmp_path);
	SDL_DestroySurface(surface);
	SDL_SetTextureBlendMode(texture(tx_barrier), SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(texture(tx_bonus_effect), SDL_BLENDMODE_ADD);
	SDL_SetRenderDrawBlendMode(rend_data->renderer, SDL_BLENDMODE_BLEND);
	return 0;
}

extern inline void AddVisalEffect(Visual_effects* const ves, const Visual_effect* const ve){
	if(ves->num < MAX_VISUAL_EFFECTS_NUM){
		*(ves->array + ves->num++) = *ve;
	}else{
		*(ves->array + SDL_rand(MAX_VISUAL_EFFECTS_NUM)) = *ve;
	}
}

static inline void RemoveVisalEffect(Visual_effects* const ves, const unsigned int indx){
	if(indx != ves->num - 1U){
		*(ves->array + indx) = *(ves->array + ves->num - 1);
	}
	--ves->num;
}

static void RenderVisualEffectsType0(Visual_effect* const ve, Game_data* const gd){
	Render_data* const rend_data = gd->rend_data_ptr;
	SDL_FPoint rend_point;
	Segment* seg = GetSegmentUnsafe(&gd->world, ve->position.x, ve->position.y);
	if(!(seg && (seg->flags & segment_in_sight) && GetExtendedRenderPointFromTrue(rend_data, ve->position.x, ve->position.y, half(ve->data.d0.size), human(gd), &rend_point))){
		return;
	}
	if(ve->ticks_left < ve->data.d0.start_ticks * 3U / 4U){
		SDL_SetTextureAlphaModFloat(texture(ve->data.d0.tx_num), ve->ticks_left / (ve->data.d0.start_ticks * 0.75F));
	}else{
		SDL_SetTextureAlphaModFloat(texture(ve->data.d0.tx_num), 1.0F - ((float)ve->ticks_left - ve->data.d0.start_ticks * 0.75F) / (ve->data.d0.start_ticks * 0.5F));
	}
	const float size = ve->data.d0.size * (1.5F - ve->ticks_left / (float)ve->data.d0.start_ticks);
	SDL_RenderTexture(rend_data->renderer, texture(ve->data.d0.tx_num), NULL, &(SDL_FRect){
		rend_point.x - half(size),
		rend_point.y - half(size),
		size,
		size
	});
	SDL_SetTextureAlphaModFloat(texture(ve->data.d0.tx_num), 1.0F);
}

static void RenderVisualEffectsType1(Visual_effect* const ve, Game_data* const gd){
	Render_data* const rend_data = gd->rend_data_ptr;
	SDL_FPoint rend_point;
	Segment* seg = GetSegmentUnsafe(&gd->world, ve->position.x, ve->position.y);
	if(!(seg && (seg->flags & segment_in_sight) && GetExtendedRenderPointFromTrue(rend_data, ve->position.x, ve->position.y, half(ve->data.d0.size), human(gd), &rend_point))){
		return;
	}
	float size;
	if(ve->ticks_left < ve->data.d0.start_ticks / 2U){
		SDL_SetTextureAlphaModFloat(texture(ve->data.d0.tx_num), ve->ticks_left / (ve->data.d0.start_ticks * 0.5F));
		size = ve->data.d0.size * (ve->ticks_left / (ve->data.d0.start_ticks * 0.5F));
	}else{
		const float level = 2.0F - ve->ticks_left / (ve->data.d0.start_ticks * 0.5F);
		SDL_SetTextureAlphaModFloat(texture(ve->data.d0.tx_num), level);
		size = ve->data.d0.size * level;
	}
	SDL_RenderTextureRotated(rend_data->renderer, texture(ve->data.d0.tx_num), NULL, &(SDL_FRect){
		rend_point.x - half(size),
		rend_point.y - half(size),
		size,
		size
	}, (double)(rend_data->counter * VORTEX_ROTAT_SPEED), NULL, SDL_FLIP_NONE);
	SDL_SetTextureAlphaModFloat(texture(ve->data.d0.tx_num), 1.0F);
}

static void RenderVisualEffectsType2(Visual_effect* const ve, Game_data* const gd){
	Render_data* const rend_data = gd->rend_data_ptr;
	SDL_FPoint rend_point;
	Segment* seg = GetSegmentUnsafe(&gd->world, ve->position.x, ve->position.y);
	if(!(seg && (seg->flags & segment_in_sight) && GetExtendedRenderPointFromTrue(rend_data, ve->position.x, ve->position.y, half(ve->data.d0.size), human(gd), &rend_point))){
		return;
	}
	if(ve->ticks_left > ve->data.d0.start_ticks - 2U){
		SDL_SetTextureColorModFloat(texture(ve->data.d0.tx_num), 0.625F, 0.5F, 1.0F);
	}else if(ve->ticks_left >= ve->data.d0.start_ticks * 3U / 4U){
		const float red = ((float)ve->ticks_left - ve->data.d0.start_ticks * 0.75F) / (ve->data.d0.start_ticks * 0.25F);
		SDL_SetTextureColorModFloat(texture(ve->data.d0.tx_num), red, pow2(red) * 0.625F, 0.0F);
	}else{
		const float level = 1.0F - (float)ve->ticks_left / (ve->data.d0.start_ticks * 0.75F);
		SDL_SetTextureColorModFloat(texture(ve->data.d0.tx_num), level, level, level);
	}
	SDL_SetTextureAlphaModFloat(texture(ve->data.d0.tx_num), (float)ve->ticks_left / (float)ve->data.d0.start_ticks * 0.75F);
	const float size = (float)ve->data.d0.size * (1.5F - (float)ve->ticks_left / (float)ve->data.d0.start_ticks);
	SDL_RenderTexture(rend_data->renderer, texture(ve->data.d0.tx_num), NULL, &(SDL_FRect){
		rend_point.x - half(size),
		rend_point.y - half(size),
		size,
		size
	});
}

static void RenderVisualEffectsTimer(Visual_effect* const ve, Game_data* const gd){
	if(ve->ticks_left == 1U){
		ve->ticks_left = ve->data.d0.start_ticks + 1U;
		--(ve->type);
	}
}

static void RenderVisualEffectsBolt(Visual_effect* const ve, Game_data* const gd){
	Render_data* const rend_data = gd->rend_data_ptr;
	SDL_FPoint rend_point;
	SDL_FPoint rend_point1;
	Segment* seg = GetSegmentUnsafe(&gd->world, ve->position.x, ve->position.y);
	if(!(seg && (seg->flags & segment_in_sight) && GetExtendedRenderPointFromTrue(rend_data, ve->position.x, ve->position.y, SEGMENT_SIZE, human(gd), &rend_point)
		&& GetExtendedRenderPointFromTrue(rend_data, ve->data.d1.start_position.x, ve->data.d1.start_position.y, SEGMENT_SIZE, human(gd), &rend_point1))){
		return;
	}
	SDL_SetRenderDrawBlendMode(rend_data->renderer, SDL_BLENDMODE_ADD);
	int count = 6 + (int)ve->position.x % 2U;
	const float angle = ve->data.d1.angle / (float)UINT8_MAX * FULL_ANGLE - human(gd)->direction;
	const float sine_angle = SineSafe(angle);
	const float cosine_angle = CosiSafe(angle);
	const float parts = count - 1.0F;
	const float distance_x = rend_point.x - rend_point1.x;
	const float distance_y = rend_point.y - rend_point1.y;
	const float distance_squared = pow2(distance_x) + pow2(distance_y);
	const float x_part = distance_x / parts;
	const float y_part = distance_y / parts;
	const float shift0 = distance_squared / pow2(VIEWFINDER_SIZE) * 64.0F;
	float shift_x = shift0 * cosine_angle;
	float shift_y = shift0 * sine_angle;
	const float width = (float)count * 0.5F;
	const float width_x = width * cosine_angle;
	const float width_y = width * sine_angle;
	const float width_step_x = width_x / (float)count;
	const float width_step_y = width_y / (float)count;
	if(count % 2){
		shift_x = -shift_x;
		shift_y = -shift_y;
	}
	if(ve->ticks_left > BOLT_TICKS - (count - 2U)){
		count = BOLT_TICKS - ve->ticks_left + 2;
		const int side_p_num = count - 3 > 0 ? count - 3 : 0;
		int num_verts = count * 2 + side_p_num;
		int num_indices = ((count - 1) * 2 + side_p_num) * 3;
		SDL_Vertex verts[num_verts];
		for(unsigned int i = 0U; i < num_verts; ++i){
			(verts + i)->color = (SDL_FColor){BOLT_RGB_F, SDL_ALPHA_OPAQUE_FLOAT};
		}
		int indices[num_indices];
		for(int i = 0; i < (count - 1) * 2; ++i){
			for(int j = 0; j < 3; ++j){
				*(indices + i * 3 + j) = i + j;
			}
		}
		for(int i = 0; i < side_p_num; ++i){
			*(indices + (count - 1) * 6 + i * 3) = (count - 1) * 2 + 2 + i;
			*(indices + (count - 1) * 6 + i * 3 + 1) = (i + 1) * 2;
			*(indices + (count - 1) * 6 + i * 3 + 2) = (i + 1) * 2 + 1;
		}
		verts->position = (SDL_FPoint){
			rend_point1.x - width_x,
			rend_point1.y - width_y
		};
		(verts + 1)->position = (SDL_FPoint){
			rend_point1.x + width_x,
			rend_point1.y + width_y
		};
		for(unsigned int i = 1U; i < count; i += 2U){
			(verts + i * 2)->position = (SDL_FPoint){
				rend_point1.x + x_part * i + shift_x - (width_x - i * width_step_x),
				rend_point1.y + y_part * i + shift_y - (width_y - i * width_step_y)
			};
			(verts + i * 2 + 1)->position = (SDL_FPoint){
				rend_point1.x + x_part * i + shift_x + (width_x - i * width_step_x),
				rend_point1.y + y_part * i + shift_y + (width_y - i * width_step_y)
			};
		}
		for(unsigned int i = 2U; i < count; i += 2U){
			(verts + i * 2)->position = (SDL_FPoint){
				rend_point1.x + x_part * i - shift_x - (width_x - i * width_step_x),
				rend_point1.y + y_part * i - shift_y - (width_y - i * width_step_y)
			};
			(verts + i * 2 + 1)->position = (SDL_FPoint){
				rend_point1.x + x_part * i - shift_x + (width_x - i * width_step_x),
				rend_point1.y + y_part * i - shift_y + (width_y - i * width_step_y)
			};
		}
		for(unsigned int i = 2U; i < count - 1U; i += 2U){
			(verts + count * 2 + (i - 2))->position = (SDL_FPoint){
				rend_point1.x + x_part * (i - 0.5F) + shift_x * 2.0F,
				rend_point1.y + y_part * (i - 0.5F) + shift_y * 2.0F
			};
		}
		for(unsigned int i = 3U; i < count - 1U; i += 2U){
			(verts + count * 2 + (i - 2))->position = (SDL_FPoint){
				rend_point1.x + x_part * (i - 0.5F) - shift_x * 2.0F,
				rend_point1.y + y_part * (i - 0.5F) - shift_y * 2.0F
			};
		}
		SDL_RenderGeometry(rend_data->renderer, NULL, verts, num_verts, indices, num_indices);
	}else{
		const float alpha = ve->ticks_left / (float)BOLT_TICKS;
		const int side_p_num = count - 3;
		const int num_verts = count * 2 + side_p_num;
		const int num_indices = ((count - 1) * 2 + side_p_num) * 3;
		SDL_Vertex verts[num_verts];
		for(unsigned int i = 0U; i < num_verts; ++i){
			(verts + i)->color = (SDL_FColor){BOLT_RGB_F, alpha};
		}
		int indices[num_indices];
		for(int i = 0; i < (count - 1) * 2; ++i){
			for(int j = 0; j < 3; ++j){
				*(indices + i * 3 + j) = i + j;
			}
		}
		for(int i = 0; i < side_p_num; ++i){
			*(indices + (count - 1) * 6 + i * 3) = (count - 1) * 2 + 2 + i;
			*(indices + (count - 1) * 6 + i * 3 + 1) = (i + 1) * 2;
			*(indices + (count - 1) * 6 + i * 3 + 2) = (i + 1) * 2 + 1;
		}
		verts->position = (SDL_FPoint){
			rend_point1.x - width_x,
			rend_point1.y - width_y
		};
		(verts + 1)->position = (SDL_FPoint){
			rend_point1.x + width_x,
			rend_point1.y + width_y
		};
		for(unsigned int i = 1U; i < count - 1U; i += 2U){
			(verts + i * 2)->position = (SDL_FPoint){
				rend_point1.x + x_part * i + shift_x - (width_x - i * width_step_x),
				rend_point1.y + y_part * i + shift_y - (width_y - i * width_step_y)
			};
			(verts + i * 2 + 1)->position = (SDL_FPoint){
				rend_point1.x + x_part * i + shift_x + (width_x - i * width_step_x),
				rend_point1.y + y_part * i + shift_y + (width_y - i * width_step_y)
			};
		}
		for(unsigned int i = 2U; i < count - 1U; i += 2U){
			(verts + i * 2)->position = (SDL_FPoint){
				rend_point1.x + x_part * i - shift_x - (width_x - i * width_step_x),
				rend_point1.y + y_part * i - shift_y - (width_y - i * width_step_y)
			};
			(verts + i * 2 + 1)->position = (SDL_FPoint){
				rend_point1.x + x_part * i - shift_x + (width_x - i * width_step_x),
				rend_point1.y + y_part * i - shift_y + (width_y - i * width_step_y)
			};
		}
		for(unsigned int i = 2U; i < count - 1U; i += 2U){
			(verts + count * 2 + (i - 2))->position = (SDL_FPoint){
				rend_point1.x + x_part * (i - 0.5F) + shift_x * 2.0F,
				rend_point1.y + y_part * (i - 0.5F) + shift_y * 2.0F
			};
		}
		for(unsigned int i = 3U; i < count - 1U; i += 2U){
			(verts + count * 2 + (i - 2))->position = (SDL_FPoint){
				rend_point1.x + x_part * (i - 0.5F) - shift_x * 2.0F,
				rend_point1.y + y_part * (i - 0.5F) - shift_y * 2.0F
			};
		}
		(verts + (count - 1) * 2)->position = (SDL_FPoint){
			rend_point.x - 1.0F,
			rend_point.y
		};
		(verts + (count - 1) * 2 + 1)->position = (SDL_FPoint){
			rend_point.x + 1.0F,
			rend_point.y
		};
		SDL_RenderGeometry(rend_data->renderer, NULL, verts, num_verts, indices, num_indices);
	}
	SDL_SetRenderDrawBlendMode(rend_data->renderer, SDL_BLENDMODE_BLEND);
}

static void RenderVisualEffects(Render_data* const rend_data, Game_data* const gd){
	const void (*render[])(Visual_effect* const, Game_data* const) = VISUAL_EFFECT_FUNCS;
	for(unsigned int i = 0U; i < rend_data->visual_effects.num; ++i){
		Visual_effect* ve = rend_data->visual_effects.array + i;
		(*(render + ve->type))(ve, gd);
		if(--ve->ticks_left < 1U){
			RemoveVisalEffect(&rend_data->visual_effects, i--);
		}
	}
}

extern inline void AddDamageVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &DAMAGE_VIS_EFFECT(*position));
}

extern inline void AddBonusVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &BONUS_VIS_EFFECT(*position));
}

extern inline void AddCurseVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &CURSE_VIS_EFFECT(*position));
}

extern inline void AddDeadVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &DEAD_VIS_EFFECT(*position));
}

extern inline void AddPortalVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &PORTAL_VIS_EFFECT(*position));
}

extern inline void AddProjectileVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &PROJE_VIS_EFFECT(*position));
}

extern inline void AddBigBurnVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &BURN_VIS_EF(*position, SDL_rand(BIG_BURN_SIZE / 2) + BIG_BURN_SIZE / 2 + 1U));
}

extern inline void AddSmallBurnVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &BURN_VIS_EF(*position, SDL_rand(SMALL_BURN_SIZE) + 1U));
}

extern inline void AddBoomVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &BURN_VIS_EF(*position, SDL_rand(BOOM_SIZE / 2) + BOOM_SIZE * 7U / 2U + 1U));
}

extern inline void AddBigBurnVisualEffectTimer(Visual_effects* const ves, const SDL_FPoint* const position, const unsigned int delay){
	AddVisalEffect(ves, &BURN_EF_TIM(*position, SDL_rand(BIG_BURN_SIZE / 2) + BIG_BURN_SIZE / 2 + 1U, delay));
}

extern inline void AddSmallBurnVisualEffectTimer(Visual_effects* const ves, const SDL_FPoint* const position, const unsigned int delay){
	AddVisalEffect(ves, &BURN_EF_TIM(*position, SDL_rand(SMALL_BURN_SIZE) + 1U, delay));
}

extern inline void AddBoomVisualEffectTimer(Visual_effects* const ves, const SDL_FPoint* const position, const unsigned int delay){
	AddVisalEffect(ves, &BURN_EF_TIM(*position, SDL_rand(BOOM_SIZE / 2) + BOOM_SIZE * 7U / 2U + 1U, delay));
}

extern inline void AddBoltVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position, const position16 start_position){
	Visual_effect ve = {
		*position,
		BOLT_TICKS
	};
	ve.data.d1.start_position = start_position;
	float angle = GetAngle(&(SDL_FPoint){start_position.x, start_position.y}, position);
	if(angle < 0.0F){
		angle += FULL_ANGLE;
	}
	ve.data.d1.angle = angle / FULL_ANGLE * UINT8_MAX;
	ve.type = visual_effect_bolt;
	AddVisalEffect(ves, &ve);
}

static void DrawBeings(Render_data* const rend_data, SDL_Surface* surface, char* bmp_path){
	texture(tx_pc) = SDL_CreateTexture(rend_data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BEING_TEXURE_SIZE, BEING_TEXURE_SIZE);
	if(!texture(tx_pc)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError());
		exit(-1);
	}
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), PC_TX0_FILE_NAME);
	surface = SDL_LoadBMP(bmp_path);
	if(!surface){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		exit(-1);
	}
	SDL_Texture* tx_pc_0 = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	if(!tx_pc_0){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		exit(-1);
	}
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), PC_TX1_FILE_NAME);
	SDL_DestroySurface(surface);
	surface = SDL_LoadBMP(bmp_path);
	if(!surface){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		exit(-1);
	}
	SDL_Texture* tx_pc_1 = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	if(!tx_pc_1){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		exit(-1);
	}
	const unsigned int beings_textures[] = BEINGS_TEXTURES;
	for(unsigned int i = 0U; i < SDL_arraysize(beings_textures); ++i){
		texture(*(beings_textures + i)) = SDL_CreateTexture(rend_data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BEING_TEXURE_SIZE, BEING_TEXURE_SIZE);
		if(!texture(*(beings_textures + i))){
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError());
			exit(-1);
		}
	}
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), BEING_TEXTURE0_FILE_NAME);
	SDL_DestroySurface(surface);
	surface = SDL_LoadBMP(bmp_path);
	if(!surface){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		exit(-1);
	}
	SDL_Texture* tx_being_0 = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	if(!tx_being_0){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		exit(-1);
	}
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), BEING_TEXTURE1_FILE_NAME);
	SDL_DestroySurface(surface);
	surface = SDL_LoadBMP(bmp_path);
	if(!surface){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		exit(-1);
	}
	SDL_Texture* tx_being_1 = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	if(!tx_being_1){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		exit(-1);
	}
	DrawBeing(rend_data, texture(tx_pc), tx_pc_0, tx_pc_1, PC_RGB_0, PC_RGB_1);
	DrawBeing(rend_data, texture(tx_being_weak), tx_being_0, tx_being_1, BEING0_RGB_0, BEING0_RGB_1);
	DrawBeing(rend_data, texture(tx_being_ordinary), tx_being_0, tx_being_1, BEING1_RGB_0, BEING1_RGB_1);
	DrawBeing(rend_data, texture(tx_being_ranger), tx_being_0, tx_being_1, BEING2_RGB_0, BEING2_RGB_1);
	DrawBeing(rend_data, texture(tx_being_elite), tx_being_0, tx_being_1, BEING3_RGB_0, BEING3_RGB_1);
	DrawBeing(rend_data, texture(tx_being_elite_ranger), tx_being_0, tx_being_1, BEING4_RGB_0, BEING4_RGB_1);
	DrawBeing(rend_data, texture(tx_being_commander), tx_being_0, tx_being_1, BEING5_RGB_0, BEING5_RGB_1);
	DrawBeing(rend_data, texture(tx_being_warlock), tx_being_0, tx_being_1, BEING6_RGB_0, BEING6_RGB_1);
	DrawBeing(rend_data, texture(tx_being_ally_ordinary), tx_being_0, tx_being_1, ALLY0_RGB_0, ALLY0_RGB_1);
	SDL_DestroyTexture(tx_pc_0);
	SDL_DestroyTexture(tx_pc_1);
	SDL_DestroyTexture(tx_being_0);
	SDL_DestroyTexture(tx_being_1);
}

static void DrawColouredThings(Render_data* const rend_data, SDL_Surface* surface, char* bmp_path){
	const unsigned int projectile_textures[] = PROJECTILE_TEXTURES;
	for(unsigned int i = 0U; i < SDL_arraysize(projectile_textures); ++i){
		texture(*(projectile_textures + i)) = SDL_CreateTexture(rend_data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, PROJECTILE_TEXURE_SIZE, PROJECTILE_TEXURE_SIZE);
		if(!texture(*(projectile_textures + i))){
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError());
			exit(-1);
		}
	}
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), PROJECTILE_TX_FILE_NAME);
	surface = SDL_LoadBMP(bmp_path);
	if(!surface){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		exit(-1);
	}
	SDL_Texture* tx_projectile_0 = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	if(!tx_projectile_0){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		exit(-1);
	}
	DrawColouredThing(rend_data, texture(tx_projectile), tx_projectile_0, PROJECTILE0_RGBA);
	DrawColouredThing(rend_data, texture(tx_h_projectile), tx_projectile_0, PROJECTILE1_RGBA);
	SDL_DestroyTexture(tx_projectile_0);
	SDL_DestroySurface(surface);
	surface = SDL_CreateSurface(3, 3, SDL_PIXELFORMAT_RGBA8888);
	SDL_WriteSurfacePixel(surface, 1, 1, 255U, 255U, 255U, 255U);
	texture(tx_pixel) = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	if(!texture(tx_pixel)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		exit(-1);
	}
}

static void DrawBeing(Render_data* const rend_data, SDL_Texture* const being_tx, SDL_Texture* const tx_0, SDL_Texture* const tx_1, const Uint8* const RGB_0, const Uint8* const RGB_1){
	SDL_SetRenderTarget(rend_data->renderer, being_tx);
	SDL_SetTextureColorMod(tx_0, *RGB_0, *(RGB_0 + 1), *(RGB_0 + 2));
	SDL_SetTextureColorMod(tx_0, *RGB_0, *(RGB_0 + 1), *(RGB_0 + 2));
	SDL_SetTextureColorMod(tx_1, *RGB_1, *(RGB_1 + 1), *(RGB_1 + 2));
	SDL_RenderTexture(rend_data->renderer, tx_1, NULL, NULL);
	SDL_RenderTexture(rend_data->renderer, tx_0, NULL, NULL);
	SDL_SetRenderTarget(rend_data->renderer, NULL);
}

static void DrawColouredThing(Render_data* const rend_data, SDL_Texture* const target, SDL_Texture* const tx, const Uint8* const RGBA){
	SDL_SetRenderTarget(rend_data->renderer, target);
	SDL_SetTextureColorMod(tx, *RGBA, *(RGBA + 1), *(RGBA + 2));
	SDL_SetTextureAlphaMod(tx, *(RGBA + 3));
	SDL_RenderTexture(rend_data->renderer, tx, NULL, NULL);
	SDL_SetRenderTarget(rend_data->renderer, NULL);
}

void SetRenderData(Render_data* const rend_data){
	rend_data->counter = 0U;
	rend_data->window = NULL;
	rend_data->renderer = NULL;
	rend_data->window_w = WINDOW_START_W;
	rend_data->window_h = WINDOW_START_H;
	rend_data->viewfinder = VIEWFINDER_SIZE;
	rend_data->viewfinder_rect = (SDL_Rect)VIEWFINDER_RECT;
	rend_data->visible_rect = (SDL_FRect)VISIBLE_RECT;
}

void ResetRenderData(Render_data* const rend_data){
	int w, h;
	SDL_GetWindowSizeInPixels(rend_data->window, &w, &h);
	rend_data->window_w = w;
	rend_data->window_h = h;
	rend_data->viewfinder = VIEWFINDER_SIZE;
	rend_data->viewfinder_rect = (SDL_Rect)VIEWFINDER_RECT;
	rend_data->visible_rect = (SDL_FRect)VISIBLE_RECT;
	SDL_WarpMouseInWindow(rend_data->window, half(rend_data->window_w), half(rend_data->window_h));
	rend_data->mouse_y = half(rend_data->window_h);
	SDL_SetWindowMouseRect(rend_data->window, &(SDL_Rect)MOUSE_RECT);
	SDL_Surface* surface = NULL;
	char* bmp_path = NULL;
	DrawBackgroud(rend_data, surface, bmp_path);
	SDL_free(bmp_path);
	SDL_DestroySurface(surface);
}

static void RenderHumanPlayerBlade(Render_data* const rend_data, Blade* const blade){
	const SDL_FRect rect_blade = {
		(VIEWFINDER_CENTER - half(BLADE_SIZE)) + blade->placement.position.x,
		(VIEWFINDER_CENTER - BLADE_SIZE * BLADE_HANDLER_POSITION + PLAYER_REND_Y_SHIFT) - blade->placement.position.y,
		BLADE_SIZE,
		BLADE_SIZE
	};
	SDL_RenderTextureRotated(rend_data->renderer, texture(tx_pc_blade), NULL, &rect_blade, (double)RadToDeg(blade->placement.direction), &(SDL_FPoint)WEAPON_ROTATION_POINT, SDL_FLIP_NONE);
}

static void RenderProjectiles(Render_data* const rend_data, Game_data* const gd){
	const unsigned int projectile_textures[] = PROJECTILE_TEXTURES;
	for(Projectile* pr = gd->projectiles.array; pr != (gd->projectiles.array + gd->projectiles.num); ++pr){
		SDL_FPoint point;
		if(GetRenderPointFromTrue(rend_data, pr->position.x, pr->position.y, human(gd), &point, &gd->world)){
			const SDL_FRect rect = { 
				point.x - half(BULLET_SIZE),
				point.y - half(BULLET_SIZE),
				BULLET_SIZE,
				BULLET_SIZE
			};
			SDL_RenderTexture(rend_data->renderer, texture(*(projectile_textures + pr->type_id)), NULL, &rect);
		}
	}
}

static inline void RenderBeing(Render_data* const rend_data, Game_data* const gd, Being* const bg, const unsigned int tex_body, const unsigned int tex_weapon){
	SDL_FPoint point;
	if(GetRenderPointFromTrueWithKnownSegmentVisibility(rend_data, bg->position.x, bg->position.y, human(gd), &point)){
		const SDL_FRect rect = {
			point.x - half(BeingSize(bg)),
			point.y - half(BeingSize(bg)),
			BeingSize(bg),
			BeingSize(bg)
		};
		if(bg->status == being_fly && bg->rend_fly_help_data.ticks != 0){
			bg->direction = bg->rend_fly_help_data.start_angle + FULL_ANGLE * (1.0F - (float)bg->status_ticks_left / (float)bg->rend_fly_help_data.ticks);
		}else if(bg->status <= being_strike){
			bg->direction = arctan2(bg->position.y - bg->target.player->position.y, bg->position.x - bg->target.player->position.x) - SDL_PI_F * 0.5F;
		}else if(bg->status <= being_strike_being){
			bg->direction = arctan2(bg->position.y - bg->target.being->position.y, bg->position.x - bg->target.being->position.x) - SDL_PI_F * 0.5F;
		}else if(bg->status == being_walk && gd->enemy_morale > 0){
			bg->direction = arctan2(bg->position.y - human(gd)->position.y, bg->position.x - human(gd)->position.x) - SDL_PI_F * 0.5F;
		}else if(bg->status == being_search || (bg->status == being_walk && gd->enemy_morale <= 0)){
			bg->direction = arctan2(-bg->special_move_shift.y, -bg->special_move_shift.x) - SDL_PI_F * 0.5F;
		}
		const float being_direction = bg->direction - human(gd)->direction;
		const float sine = SineSafe(being_direction);
		const float cosine = CosiSafe(being_direction);
		const Placement weapon = GetBeingWeaponPlacement(bg);
		const SDL_FRect rect_blade = {
			point.x + (weapon.position.x * cosine + weapon.position.y * sine) - half(BLADE_SIZE),
			point.y + (weapon.position.x * sine - weapon.position.y * cosine) - BLADE_SIZE * BLADE_HANDLER_POSITION,
			BLADE_SIZE,
			BLADE_SIZE
		};
		SDL_RenderTextureRotated(rend_data->renderer, texture(tex_weapon), NULL, &rect_blade, (double)RadToDeg(weapon.direction + being_direction), &(SDL_FPoint)WEAPON_ROTATION_POINT, SDL_FLIP_NONE);
		SDL_RenderTextureRotated(rend_data->renderer, texture(tex_body), NULL, &rect, (double)RadToDeg(being_direction), NULL, SDL_FLIP_NONE);
		if(bg->status == being_stunned){
			SDL_RenderTextureRotated(rend_data->renderer, texture(tx_stun), NULL, &rect, (double)(rend_data->counter * TEXTURE_ROTA_SPEED), NULL, SDL_FLIP_NONE);
		}
	}
}

static void RenderBeings(Render_data* const rend_data, Game_data* const gd, Segment** const segs, const unsigned int segs_num){
	const unsigned int beings_textures[] = BEINGS_TEXTURES;
	const unsigned int beings_weapon_textures[] = BEINGS_WEAPON_TEXTURES;
    for(unsigned int k = 0U; k < segs_num; ++k){
		const Segment* const seg = *(segs + k);
		for(unsigned int i = 0U; i < seg->ally_beings.num; ++i){
			Being* const bg = (gd->beings.array + *(seg->ally_beings.beings_ind + i));
			RenderBeing(rend_data, gd, bg, *(beings_textures + bg->type_id), *(beings_weapon_textures + bg->type_id));
		}
		for(unsigned int i = 0U; i < seg->beings.num; ++i){
			Being* const bg = (gd->beings.array + *(seg->beings.beings_ind + i));
			RenderBeing(rend_data, gd, bg, *(beings_textures + bg->type_id), *(beings_weapon_textures + bg->type_id));
		}
	}
}

static void RenderMap(Render_data* const rend_data, const Game_data* const gd, Player* const pc){
	const SDL_FRect pc_rect = {
		pc->position.x * (VIEWFINDER_SIZE / WORLD_W) - half(MINIMAP_PC_SIZE),
		pc->position.y * (VIEWFINDER_SIZE / WORLD_H) - half(MINIMAP_PC_SIZE),
		MINIMAP_PC_SIZE,
		MINIMAP_PC_SIZE
	};
	SDL_RenderTexture(rend_data->renderer, texture(tx_map), NULL, NULL);
	SDL_SetRenderDrawColor(rend_data->renderer, 255, 242, 0, 255);
	for(unsigned int i = 0U; i < gd->needed_keys; ++i){
		if(*(gd->keys_status + i) != key_location_unknown){
			const SDL_FRect rect = {
				(gd->world.key_locations + i)->x * (VIEWFINDER_SIZE / 255.0F) - half(MAP_KEY_SIZE),
				(gd->world.key_locations + i)->y * (VIEWFINDER_SIZE / 255.0F) - half(MAP_KEY_SIZE),
				MAP_KEY_SIZE,
				MAP_KEY_SIZE
			};
			SDL_RenderFillRect(rend_data->renderer, &rect);
			if(*(gd->keys_status + i) == key_owned){
				SDL_RenderTexture(rend_data->renderer, texture(tx_owned), NULL, &rect);
			}
		}
	}
	SDL_SetRenderDrawColor(rend_data->renderer, 192, 0, 0, 255);
	SDL_RenderFillRect(rend_data->renderer, &pc_rect);
}

static inline bool GetRenderPointFromTrue(Render_data* const rend_data, const float true_point_x, const float true_point_y, const Player* const human_player, SDL_FPoint* const rend_point, World* const w){
	const float dx = true_point_x - human_player->position.x;
	if(SDL_fabsf(dx) > rend_data->viewfinder) return false;
	const float dy = true_point_y - human_player->position.y;
	if(SDL_fabsf(dy) > rend_data->viewfinder) return false;
	if(!(GetSegmentUnsafe(w, true_point_x, true_point_y)->flags & segment_in_sight)) return false;
	*rend_point = (SDL_FPoint){
		VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction),
		PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction)
	};
	if(SDL_PointInRectFloat(rend_point, &rend_data->visible_rect)) return true;
	return false;
}

static inline bool GetRenderPointFromTrueWithYShift(Render_data* const rend_data, const float true_point_x, const float true_point_y, const float y_shift, const Player* const human_player, SDL_FPoint* const rend_point, World* const w){
	const float dx = true_point_x - human_player->position.x;
	if(SDL_fabsf(dx) > rend_data->viewfinder) return false;
	float dy = true_point_y - human_player->position.y;
	if(SDL_fabsf(dy) > rend_data->viewfinder) return false;
	if(!(GetSegmentUnsafe(w, true_point_x, true_point_y)->flags & segment_in_sight)) return false;
	const SDL_FPoint point = {
		VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction),
		PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction)
	};
	dy = true_point_y + y_shift - human_player->position.y;
	*rend_point = (SDL_FPoint){
		VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction),
		PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction)
	};
	if(SDL_PointInRectFloat(&point, &rend_data->visible_rect)) return true;
	return false;
}

static inline bool GetRenderPointFromTrueWithKnownSegmentVisibility(Render_data* const rend_data, const float true_point_x, const float true_point_y, const Player* const human_player, SDL_FPoint* const rend_point){
	const float dx = true_point_x - human_player->position.x;
	const float dy = true_point_y - human_player->position.y;
	*rend_point = (SDL_FPoint){
		VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction),
		PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction)
	};
	if(SDL_PointInRectFloat(rend_point, &rend_data->visible_rect)) return true;
	return false;
}

void RenderTextInfo(Render_data* const rend_data, const Uint64 tps, Game_data* const gd){
	SDL_Renderer* const rend = rend_data->renderer;
	const float x = rend_data->viewfinder_rect.x + 2.0F;
	SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
	SDL_RenderDebugTextFormat(rend, x, 10, "position: %.2f %.2f", human(gd)->position.x, human(gd)->position.y);
	SDL_RenderDebugTextFormat(rend, x, 20, "direction: %.2f", human(gd)->direction);

	SDL_RenderDebugTextFormat(rend, x, 30, "ticks per sec.: %d", tps);
	SDL_RenderDebugTextFormat(rend, x, 40, "max FPS: ~%u", (1000000000ULL / FRAME_TIME));

	SDL_RenderDebugTextFormat(rend, x, 50, "beings: %u", gd->beings.num);
	SDL_RenderDebugTextFormat(rend, x, 60, "projectiles: %u", gd->projectiles.num);
	SDL_RenderDebugTextFormat(rend, x, 70, "blade charge: %f", 1.0F - human(gd)->blade.charge);
	SDL_RenderDebugTextFormat(rend, x, 80, "fatigue: %d", human(gd)->fatigue_points);
	SDL_RenderDebugTextFormat(rend, x, 90, "hp: %d", human(gd)->hit_points);
	SDL_RenderDebugTextFormat(rend, x, 100, "visual effects: %u", rend_data->visual_effects.num);
	SDL_RenderDebugTextFormat(rend, x, 110, "boxes: %u", gd->boxes.num);

	SDL_RenderDebugTextFormat(rend, x, 120, "enemy morale: %d", gd->enemy_morale);

	SDL_RenderDebugTextFormat(rend, x, 130, "keys needed: %u", gd->needed_keys);
	SDL_RenderDebugTextFormat(rend, x, 140, "player segment: x: %u y: %u", human(gd)->segment->indx.x, human(gd)->segment->indx.y);
	SDL_RenderDebugTextFormat(rend, x, 150, "selected scroll: %u", human(gd)->selected_scroll);
	SDL_RenderDebugTextFormat(rend, x, 160, "selected scroll num: %u", *(human(gd)->scrolls + human(gd)->selected_scroll));
	SDL_RenderDebugTextFormat(rend, x, 170, "selected scroll cost: %d", ScrollCost(human(gd)->selected_scroll));
	SDL_RenderDebugTextFormat(rend, x, 190, "Segment beings: %u", human(gd)->segment->beings.num);
}

static void RenderPlayerStatus(Render_data* const rend_data, Player* const p, const Game_data* const gd){
	const float BAR_H = BAR_H_CALC;
	const SDL_FRect rect_armo = {
		BAR_X,
		AREAA_Y + BAR_H + FRAME_W * 2.0F,
		((1.0F - p->armour.multipl) / (1.0F - p->max_armour.multipl)) * BAR_W,
		SMALL_BAR_H
	};
	const SDL_FRect rect_marmo = {
		BAR_X,
		rect_armo.y + SMALL_BAR_H,
		((1.0F - p->armour.magic_multipl) / (1.0F - p->max_armour.magic_multipl)) * BAR_W,
		SMALL_BAR_H
	};
	const SDL_FRect rectHP = {
		BAR_X,
		rect_armo.y + BAR_H * 2.0F + FRAME_W * 5.0F,
		(float)p->hit_points / (float)p->max_h_p * BAR_W,
		BAR_H
	};
	const SDL_FPoint MP_position = {
		RIGHT_AREA_X + FRAME_W * 2.0F,
		AREA_MP_Y + BAR_H + FRAME_W * 2.0F
	};
	const SDL_FRect rectFP = {
		BAR_X,
		rectHP.y + BAR_H * 2.0F + FRAME_W * 5.0F,
		(float)p->fatigue_points / (float)p->max_fatigue * BAR_W,
		BAR_H
	};
	SDL_SetTextureColorMod(texture(tx_bar), 128U, 128U, 128U);
	SDL_RenderTexture(rend_data->renderer, texture(tx_bar), NULL, &rect_armo);
	SDL_SetTextureColorMod(texture(tx_bar), 128U, 128U, 255U);
	SDL_RenderTexture(rend_data->renderer, texture(tx_bar), NULL, &rect_marmo);
	SDL_SetTextureColorMod(texture(tx_bar), 255U, 0U, 0U);
	SDL_RenderTexture(rend_data->renderer, texture(tx_bar), NULL, &rectHP);
	SDL_SetTextureColorMod(texture(tx_bar), 0U, 255U, 0U);
	SDL_RenderTexture(rend_data->renderer, texture(tx_bar), NULL, &rectFP);
	SDL_SetTextureColorMod(texture(tx_chars), MP_TEXT_RGB);
	RenderIntCentered(rend_data, MP_position.x, MP_position.y, MP_BAR_H, p->magic_points, BAR_W);
	SDL_SetTextureColorMod(texture(tx_chars), MP_COST_TEXT_RGB);
	RenderIntCentered(rend_data, MP_position.x, MP_position.y + MP_BAR_H, MP_BAR_H, ScrollCost(p->selected_scroll), BAR_W);
	SDL_SetTextureColorMod(texture(tx_chars), WHITE_RGB);
	RenderIntCentered(rend_data, KEYS_NUM_X, KEYS_NUM_Y, BAR_H, gd->keys, BAR_W - BAR_H - FRAME_W * 2.0F);
	RenderIntCentered(rend_data, KEYS_NUM_X, COINS_NUM_Y, BAR_H, p->coins, BAR_W - BAR_H - FRAME_W * 2.0F);
	if(p->blade.charge != PC_BLADE_CHARGE_BASE){
		const SDL_FRect rect_cha = {
			half(rend_data->window_w) - half(PLAYER_SIZE),
			PLAYER_REND_Y + PLAYER_SIZE,
			PLAYER_SIZE,
			10.0F
		};
		const SDL_FRect rect_chb = {
			half(rend_data->window_w) - (half(PLAYER_SIZE) - 1.0F),
			PLAYER_REND_Y + PLAYER_SIZE + 1.0F,
			(1.0F - p->blade.charge) * (rect_cha.w - 2.0F),
			8.0F
		};
		const unsigned int red = (unsigned int)((1.0F - p->blade.charge) * 255.0F);
		const unsigned int green = (unsigned int)(p->blade.charge * 255.0F);
		SDL_SetRenderDrawColor(rend_data->renderer, red / 4, green / 4, 0U, 255U);
		SDL_RenderRect(rend_data->renderer, &rect_cha);
		SDL_SetTextureColorMod(texture(tx_bar), red, green, 0U);
		SDL_RenderTexture(rend_data->renderer, texture(tx_bar), NULL, &rect_chb);
	}
}

void RenderMainMenu(Render_data* const rend_data, const unsigned int menu_position){
	SDL_SetRenderDrawColor(rend_data->renderer, 0U, 0U, 0U, 255U);
	SDL_RenderClear(rend_data->renderer);
	SDL_SetRenderViewport(rend_data->renderer, &rend_data->viewfinder_rect);
	const Uint8* texts[] = MAIN_MENU_TEXTS;
	const unsigned int texts_chars_num[] = MAIN_MENU_TEXTS_SIZES;
	const float icon_size = SCROLLS_MANAG_ICON_SIZE;
	const float text_height = icon_size * 0.5F;
	SDL_FPoint dst_point_text = {
		icon_size,
		icon_size * 1.25F + FRAME_W * 3.0F
	};
	const float shift = icon_size + FRAME_W;
	const SDL_FRect menu_ptr_rect = {
		FRAME_W,
		icon_size + FRAME_W * 3.0F + shift * menu_position,
		rend_data->viewfinder_rect.w - FRAME_W * 2.0F,
		icon_size
	};
	for(unsigned int i = 0U; i < OPTIONS_NUM; ++i){
		RenderTextCentered(rend_data, 0.0F, dst_point_text.y, text_height, *(texts + i), *(texts_chars_num + i), rend_data->viewfinder);
		dst_point_text.y += shift;
	}
	SDL_RenderTexture(rend_data->renderer, texture(tx_menu_ptr), NULL, &menu_ptr_rect);
	SDL_SetRenderViewport(rend_data->renderer, NULL);
	SDL_RenderPresent(rend_data->renderer);
}

void RenderGame(Render_data* const rend_data, Game_data* const gd, const int event_code){
	Player* const pc = human(gd);
	++rend_data->counter;
	SetSineCosine(rend_data, pc);
	RenderBackground(rend_data);
	SDL_SetRenderViewport(rend_data->renderer, &rend_data->viewfinder_rect);
	Segment* beings_segs[MAX_UNSEEN_SEG];
	unsigned int beings_segs_num = 0U;
	RenderTerrain(rend_data, gd, beings_segs, &beings_segs_num);
	RenderStaticThings(rend_data, gd);
	if(!(pc->flags & range_mode)){
		RenderHumanPlayerBlade(rend_data, &pc->blade);
	}else if((pc->flags & attack)){
		RenderHumanPlayerScrollUnrolled(rend_data, pc->selected_scroll);
	}else{
		RenderHumanPlayerScroll(rend_data);
	}
	Player* players_to_rend[gd->champions.num - 1U];
	SDL_FPoint players_rend_points[gd->champions.num - 1U];
	unsigned int indx = 0U;
	for(unsigned int i = 0U; i < gd->champions.num; ++i){
		if(i == gd->human_indx){
			continue;
		}
		SDL_FPoint point;
		if(GetRenderPointFromTrue(rend_data, (gd->champions.array + i)->position.x, (gd->champions.array + i)->position.y, pc, &point, &gd->world)){
			*(players_to_rend + indx) = gd->champions.array + i;
			*(players_rend_points + indx++) = point;
		}
	}
	RenderPlayersBladesAndScrolls(rend_data, pc->direction, players_to_rend, players_rend_points, indx);
	RenderBeings(rend_data, gd, beings_segs, beings_segs_num);
	RenderProjectiles(rend_data, gd);
	RenderHumanPlayer(rend_data);
	if(human(gd)->flags & stunned){
		SDL_RenderTextureRotated(rend_data->renderer, texture(tx_stun), NULL, &(SDL_FRect)PC_RECT, (double)(rend_data->counter * TEXTURE_ROTA_SPEED), NULL, SDL_FLIP_NONE);
	}
	RenderPlayers(rend_data, pc->direction, players_to_rend, players_rend_points, indx);
	if(pc->flags & block){
		RenderHumanPlayerBarrier(rend_data, pc);
	}
	RenderBarriers(rend_data, pc->direction, players_to_rend, players_rend_points, indx);
	RenderDoors(rend_data, gd);
	RenderVisualEffects(rend_data, gd);
	SDL_RenderTexture(rend_data->renderer, texture(tx_viewfinder), NULL, NULL);
	if(event_code == event_menu){
		RenderMenu(rend_data, pc);
	}else if(event_code == event_manage_scrolls){
		RenderScrollsManagement(rend_data, pc);
	}else{
		RenderGunSight(rend_data);
		if(pc->flags & map_look){
			RenderMap(rend_data, gd, pc);
		}
	}
	if(gd->flags & gamef_horde_attack){
		RenderText(rend_data, 1.0F, 1.0F, 16.0F, (Uint8[])HORDE_ALERT);
	}
	SDL_SetRenderViewport(rend_data->renderer, NULL);
	RenderDirectionArrow(rend_data, RadToDeg(pc->direction));
	RenderPlayerStatus(rend_data, pc, gd);
	RenderQuickScrolls(rend_data, pc);
}

static inline void SetSineCosine(Render_data* const rend_data, Player* const p){
	rend_data->sin_player_direction = SineUnsafe(p->direction);
	rend_data->cos_player_direction = CosiUnsafe(p->direction);
}

void ClearRenderData(Render_data* const rend_data){
	for(int i = 0; i < txs_num; ++i){
		SDL_DestroyTexture(texture(i));
	}
	SDL_DestroyRenderer(rend_data->renderer);
	SDL_DestroyWindow(rend_data->window);
	SDL_Quit();
}

static void RenderTerrain(Render_data* const rend_data, Game_data* const gd, Segment** const beings_segs, unsigned int* const beings_segs_num){
	const SDL_FPoint corner_first = {human(gd)->position.x - (rend_data->viewfinder + SEGMENT_SIZE), human(gd)->position.y - (rend_data->viewfinder + SEGMENT_SIZE)};
	const SDL_FPoint corner_last = {human(gd)->position.x + (rend_data->viewfinder + SEGMENT_SIZE), human(gd)->position.y + (rend_data->viewfinder + SEGMENT_SIZE)};
	SDL_FPoint point = corner_first;
	float shift_y = SDL_fmodf(point.y, SEGMENT_SIZE) + half(SEGMENT_SIZE);
	point.x -= SDL_fmodf(point.x, SEGMENT_SIZE) + half(SEGMENT_SIZE);
	point.y -= shift_y;
	SDL_FPoint unseen_seg_points[MAX_UNSEEN_SEG];
	unsigned int unseen_seg_num = 0U;
	while(point.x < corner_last.x){
		while(point.y < corner_last.y){
			SDL_FPoint rend_point;
			if(GetExtendedRenderPointFromTrue(rend_data, point.x, point.y, SEGMENT_SIZE * SQRT2DIV2, human(gd), &rend_point)){
				if(point.x > 0.0F && point.y > 0.0F && point.x < WORLD_SIZE && point.y < WORLD_SIZE){
					Segment* seg = GetSegmentUnsafe(&gd->world, point.x, point.y);
					if(seg){
						if(SegmentInSight(&human(gd)->position, &point, seg, &gd->world)){
							seg->flags |= segment_in_sight | segment_known;
							if(seg->beings.num > 0U || seg->ally_beings.num > 0U){
								*(beings_segs + (*beings_segs_num)++) = seg;
							}
							SDL_RenderTextureRotated(rend_data->renderer, texture(tx_terrain), NULL, &(SDL_FRect){
								rend_point.x - half(SEGMENT_TX_SIZE),
								rend_point.y - half(SEGMENT_TX_SIZE),
								SEGMENT_TX_SIZE,
								SEGMENT_TX_SIZE
							}, -RadToDeg(human(gd)->direction), NULL, SDL_FLIP_NONE);
						}else{
							if(seg->flags & segment_known){
								*(unseen_seg_points + unseen_seg_num++) = (SDL_FPoint){rend_point.x - half(SEGMENT_TX_SIZE), rend_point.y - half(SEGMENT_TX_SIZE)};
							}
							seg->flags &= ~(segment_in_sight);
						}
					}
				}
			}
			point.y += SEGMENT_SIZE;
		}
		point.x += SEGMENT_SIZE;
		point.y = corner_first.y - shift_y;
	}
	SDL_SetTextureColorMod(texture(tx_terrain), 127, 111, 111);
	for(unsigned int i = 0; i < unseen_seg_num; ++i){
		SDL_RenderTextureRotated(rend_data->renderer, texture(tx_terrain), NULL, &(SDL_FRect){
			(unseen_seg_points + i)->x,
			(unseen_seg_points + i)->y,
			SEGMENT_TX_SIZE,
			SEGMENT_TX_SIZE
		}, -RadToDeg(human(gd)->direction), NULL, SDL_FLIP_NONE);
	}
	SDL_SetTextureColorMod(texture(tx_terrain), 255, 255, 255);
}

static inline bool GetExtendedRenderPointFromTrue(Render_data* const rend_data, const float true_point_x, const float true_point_y, const float extension, const Player* const pc, SDL_FPoint* const rend_point){
	const SDL_FRect extended_view = {
		rend_data->visible_rect.x - extension,
		rend_data->visible_rect.y - extension,
		rend_data->visible_rect.w + extension * 2.0F,
		rend_data->visible_rect.h + extension * 2.0F
	};
	const float dx = true_point_x - pc->position.x;
	if(SDL_fabsf(dx) > rend_data->viewfinder + extension) return false;
	const float dy = true_point_y - pc->position.y;
	if(SDL_fabsf(dy) > rend_data->viewfinder + extension) return false;
	*rend_point = (SDL_FPoint){
		VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction),
		PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction)
	};
	if(SDL_PointInRectFloat(rend_point, &extended_view)) return true;
	return false;
}

static void RenderGunSight(Render_data* const rend_data){
	const float spread = (PLAYER_REND_Y - rend_data->mouse_y) / (float)rend_data->viewfinder * GUN_SIGHT_SPREAD_RANGE + GUN_SIGHT_SPREAD_MIN;
	SDL_FRect rect = {
		VIEWFINDER_CENTER - half(GUN_SIGHT_SIZE),
		rend_data->mouse_y - half(GUN_SIGHT_SIZE) - spread,
		(float)GUN_SIGHT_SIZE,
		(float)GUN_SIGHT_SIZE
	};
	SDL_RenderTexture(rend_data->renderer, texture(tx_gunsightpart), NULL, &rect);
	rect.y += spread * 2.0F;
	SDL_RenderTexture(rend_data->renderer, texture(tx_gunsightpart), NULL, &rect);
	rect.x -= spread;
	rect.y = rend_data->mouse_y - half(GUN_SIGHT_SIZE);
	SDL_RenderTextureRotated(rend_data->renderer, texture(tx_gunsightpart), NULL, &rect, 90.0, NULL, SDL_FLIP_NONE);
	rect.x += spread * 2.0F;
	SDL_RenderTextureRotated(rend_data->renderer, texture(tx_gunsightpart), NULL, &rect, 90.0, NULL, SDL_FLIP_NONE);
}

static void RenderDoors(Render_data* const rend_data, Game_data* const gd){
	RenderStaticThing(rend_data, gd->world.door.x, gd->world.door.y, human(gd), DOOR_SIZE, tx_door, &gd->world);
	for(unsigned int i = 0U; i < SHOPS_NUM; ++i){
		SDL_FPoint point;
		if(GetRenderPointFromTrueWithYShift(rend_data, (gd->world.shops + i)->location.x, (gd->world.shops + i)->location.y, half(SHOP_SIZE) + 1.0F, human(gd), &point, &gd->world)){
			const SDL_FRect rect = {
				point.x - half(SHOP_SIZE),
				point.y - half(SHOP_SIZE),
				SHOP_SIZE,
				SHOP_SIZE
			};
			SDL_RenderTextureRotated(rend_data->renderer, texture(tx_door), NULL, &rect, (double)(-RadToDeg(human(gd)->direction)), NULL, SDL_FLIP_NONE);
		}
	}
	if(gd->flags & gamef_horde_attack){
		for(unsigned int i = 0U; i < HORDE_ATTACK_POINTS; ++i){
			const int ticks_left = (gd->effects + HasEffect(gd->effects, gd->effects_num, game_effect_horde_attack))->ticks_left;
			if(ticks_left > HORDE_ATTACK_START_TICKS / 8 * 7){
				RenderStaticThingRotating(rend_data, (gd->horde_data.creation_points + i)->x, (gd->horde_data.creation_points + i)->y, human(gd), CREATION_POINT_SIZE * ((float)(HORDE_ATTACK_START_TICKS - ticks_left) / (HORDE_ATTACK_START_TICKS / 8)), tx_creation_point, &gd->world, VORTEX_ROTAT_SPEED);
			}else if(ticks_left < HORDE_ATTACK_START_TICKS / 8){
				RenderStaticThingRotating(rend_data, (gd->horde_data.creation_points + i)->x, (gd->horde_data.creation_points + i)->y, human(gd), CREATION_POINT_SIZE * ((float)ticks_left / (HORDE_ATTACK_START_TICKS / 8)), tx_creation_point, &gd->world, VORTEX_ROTAT_SPEED);
			}else{
				RenderStaticThingRotating(rend_data, (gd->horde_data.creation_points + i)->x, (gd->horde_data.creation_points + i)->y, human(gd), CREATION_POINT_SIZE, tx_creation_point, &gd->world, VORTEX_ROTAT_SPEED);
			}
		}
	}
}

static void RenderStaticThings(Render_data* const rend_data, Game_data* const gd){
	RenderStaticThing(rend_data, gd->world.portalA.x, gd->world.portalA.y, human(gd), DOOR_SIZE, tx_portal, &gd->world);
	RenderStaticThing(rend_data, gd->world.portalB.x, gd->world.portalB.y, human(gd), DOOR_SIZE, tx_portal, &gd->world);
	RenderStaticThing(rend_data, gd->world.door.x, gd->world.door.y, human(gd), DOOR_SIZE, tx_door, &gd->world);
	for(unsigned int i = 0U; i < gd->boxes.num; ++i){
		RenderStaticThing(rend_data, (gd->boxes.array + i)->location.x, (gd->boxes.array + i)->location.y, human(gd), BOX_SIZE, tx_box, &gd->world);
	}
	if(gd->flags & gamef_horde_attack){
		for(unsigned int i = 0U; i < HORDE_ATTACK_POINTS; ++i){
			const int ticks_left = (gd->effects + HasEffect(gd->effects, gd->effects_num, game_effect_horde_attack))->ticks_left;
			if(ticks_left > HORDE_ATTACK_START_TICKS / 8 * 7){
				RenderStaticThingRotating(rend_data, (gd->horde_data.creation_points + i)->x, (gd->horde_data.creation_points + i)->y, human(gd), CREATION_POINT_SIZE * ((float)(HORDE_ATTACK_START_TICKS - ticks_left) / (HORDE_ATTACK_START_TICKS / 8)), tx_creation_point, &gd->world, VORTEX_ROTAT_SPEED);
			}else if(ticks_left < HORDE_ATTACK_START_TICKS / 8){
				RenderStaticThingRotating(rend_data, (gd->horde_data.creation_points + i)->x, (gd->horde_data.creation_points + i)->y, human(gd), CREATION_POINT_SIZE * ((float)ticks_left / (HORDE_ATTACK_START_TICKS / 8)), tx_creation_point, &gd->world, VORTEX_ROTAT_SPEED);
			}else{
				RenderStaticThingRotating(rend_data, (gd->horde_data.creation_points + i)->x, (gd->horde_data.creation_points + i)->y, human(gd), CREATION_POINT_SIZE, tx_creation_point, &gd->world, VORTEX_ROTAT_SPEED);
			}
		}
	}
}

static void RenderStaticThing(Render_data* const rend_data, const float pos_x, const float pos_y, Player* const p, const float size, const int tx_num, World* const w){
	SDL_FPoint point;
	if(GetRenderPointFromTrue(rend_data, pos_x, pos_y, p, &point, w)){
		const SDL_FRect rect = {
			point.x - half(size),
			point.y - half(size),
			size,
			size
		};
		SDL_RenderTextureRotated(rend_data->renderer, texture(tx_num), NULL, &rect, (double)(-RadToDeg(p->direction)), NULL, SDL_FLIP_NONE);
	}
}

static void RenderStaticThingRotating(Render_data* const rend_data, const float pos_x, const float pos_y, Player* const p, const float size, const int tx_num, World* const w, const float speed){
	SDL_FPoint point;
	if(GetRenderPointFromTrue(rend_data, pos_x, pos_y, p, &point, w)){
		const SDL_FRect rect = {
			point.x - half(size),
			point.y - half(size),
			size,
			size
		};
		SDL_RenderTextureRotated(rend_data->renderer, texture(tx_num), NULL, &rect, (double)(-RadToDeg(p->direction) + rend_data->counter * speed), NULL, SDL_FLIP_NONE);
	}
}

void DrawMap(Render_data* const rend_data, const World* const wld){
	if(texture(tx_map) != NULL){
		SDL_DestroyTexture(texture(tx_map));
	}
	texture(tx_map) = SDL_CreateTexture(rend_data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BIG_SEGMENTS_X, BIG_SEGMENTS_X);
	SDL_SetRenderTarget(rend_data->renderer, texture(tx_map));
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, 127);
	SDL_RenderFillRect(rend_data->renderer, NULL);
	for(unsigned int c = 0U; c < BIG_SEGMENTS_X; ++c){
		for(unsigned int r = 0U; r < BIG_SEGMENTS_X; ++r){
			if(IsInUncoveredBigSeg(wld->plan, c, r)){
				SDL_SetRenderDrawColor(rend_data->renderer, 255, 255, 255, 255);
				SDL_RenderPoint(rend_data->renderer, c, r);
			}else if(IsInPopulatedBigSeg(wld->plan, c, r)){
				SDL_SetRenderDrawColor(rend_data->renderer, 127, 127, 127, 255);
				SDL_RenderPoint(rend_data->renderer, c, r);
			}
		}
	}
	SDL_SetTextureScaleMode(texture(tx_map), SDL_SCALEMODE_NEAREST);
	SDL_SetRenderTarget(rend_data->renderer, NULL);
}

extern inline void UpdateMap(Render_data* const rend_data, const Uint8 red, const Uint8 green, const Uint8 blue, const unsigned int col, const unsigned int row){
	SDL_SetRenderTarget(rend_data->renderer, texture(tx_map));
	SDL_SetRenderDrawColor(rend_data->renderer, red, green, blue, 255);
	SDL_RenderPoint(rend_data->renderer, col, row);
	SDL_SetRenderTarget(rend_data->renderer, NULL);
}

static inline void RenderHumanPlayer(Render_data* const rend_data){
	SDL_RenderTexture(rend_data->renderer, texture(tx_pc), NULL, &(SDL_FRect)PC_RECT);
}

static inline void RenderHumanPlayerBarrier(Render_data* const rend_data, const Player* const p){
	if(p->block_times.push < 1){
		SDL_RenderTexture(rend_data->renderer, texture(tx_barrier), NULL, &(SDL_FRect)PC_SHIELD_RECT);
	}else{
		const float size = BARRIER_SIZE + (float)p->block_times.push * BARRIER_MAGNIFICATION;
		SDL_RenderTexture(rend_data->renderer, texture(tx_barrier), NULL,
			&(SDL_FRect){
				VIEWFINDER_CENTER - half(size),
				VIEWFINDER_CENTER - half(size) + PLAYER_REND_Y_SHIFT,
				size,
				size
			}
		);
	}
}

static inline void RenderHumanPlayerScroll(Render_data* const rend_data){
	const SDL_FRect src_rect = SRC_SCROLL_RECT;
	SDL_RenderTexture(rend_data->renderer, texture(tx_scroll), &src_rect, &(SDL_FRect)PC_SCROLL_RECT);
}

static inline void RenderHumanPlayerScrollUnrolled(Render_data* const rend_data, unsigned int scroll_num){
	SDL_FRect src_rect = SRC_UNR_SCROLL_RECT;
	SDL_RenderTexture(rend_data->renderer, texture(tx_scroll), &src_rect, &(SDL_FRect)PC_SCROLL_RECT);
	src_rect = GetScrollTextureSrcRect(scroll_num);
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &src_rect, &(SDL_FRect)PC_SCROLL_ICON_RECT);
}

static void	RenderPlayersBladesAndScrolls(Render_data* const rend_data, const float human_player_direction, Player** plys, SDL_FPoint* points, const unsigned int num){
	SDL_FRect rect_blade = {
		0.0F, 0.0F,
		BLADE_SIZE, BLADE_SIZE
	};
	SDL_FRect rect_scroll = {
		0.0F, 0.0F,
		SCROLL_SIZE, SCROLL_SIZE
	};
	SDL_FRect rect_scroll_icon = {
		0.0F, 0.0F,
		SCROLL_ICON_SIZE, SCROLL_ICON_SIZE
	};
	for(unsigned int i = 0U; i < num; ++i){
		const float player_direction = (*(plys + i))->direction - human_player_direction;
		const float sine = SineSafe(player_direction);
		const float cosine = CosiSafe(player_direction);
		if(!((*(plys + i))->flags & range_mode)){
			rect_blade.x = (points + i)->x + ((*(plys + i))->blade.placement.position.x * cosine + (*(plys + i))->blade.placement.position.y * sine) - half(BLADE_SIZE);
			rect_blade.y = (points + i)->y + ((*(plys + i))->blade.placement.position.x * sine - (*(plys + i))->blade.placement.position.y * cosine) - BLADE_SIZE * BLADE_HANDLER_POSITION;
			SDL_RenderTextureRotated(rend_data->renderer, texture(tx_pc_blade), NULL, &rect_blade, (double)RadToDeg(player_direction + (*(plys + i))->blade.placement.direction), &(SDL_FPoint)WEAPON_ROTATION_POINT, SDL_FLIP_NONE);
		}else{
			rect_scroll.x = (points + i)->x + (half(SCROLL_SIZE) * cosine + half(SCROLL_SIZE) * sine) - half(SCROLL_SIZE);
			rect_scroll.y = (points + i)->y + (half(SCROLL_SIZE) * sine - half(SCROLL_SIZE) * cosine) - half(SCROLL_SIZE);
			if((*(plys + i))->flags & attack){
				rect_scroll_icon.x = (points + i)->x + (half(SCROLL_SIZE) * cosine + half(SCROLL_SIZE) * sine) - half(SCROLL_ICON_SIZE);
				rect_scroll_icon.y = (points + i)->y + (half(SCROLL_SIZE) * sine - half(SCROLL_SIZE) * cosine) - half(SCROLL_ICON_SIZE);
				SDL_FRect src_rect = SRC_UNR_SCROLL_RECT;
				SDL_RenderTextureRotated(rend_data->renderer, texture(tx_scroll), &src_rect, &rect_scroll, (double)RadToDeg(player_direction), NULL, SDL_FLIP_NONE);
				src_rect = GetScrollTextureSrcRect((*(plys + i))->selected_scroll);
				SDL_RenderTextureRotated(rend_data->renderer, texture(tx_icons), &src_rect, &rect_scroll_icon, (double)RadToDeg(player_direction), NULL, SDL_FLIP_NONE);
			}else{
				const SDL_FRect src_rect = SRC_SCROLL_RECT;
				SDL_RenderTextureRotated(rend_data->renderer, texture(tx_scroll), &src_rect, &rect_scroll, (double)RadToDeg(player_direction), NULL, SDL_FLIP_NONE);
			}
		}
	}
}

static void	RenderPlayers(Render_data* const rend_data, const float human_player_direction, Player** plys, SDL_FPoint* points, const unsigned int num){
	SDL_FRect rect_player = {
		0.0F,
		0.0F,
		(float)PLAYER_SIZE,
		(float)PLAYER_SIZE
	};
	for(unsigned int i = 0U; i < num; ++i){
		rect_player.x = (points + i)->x - half(PLAYER_SIZE);
		rect_player.y = (points + i)->y - half(PLAYER_SIZE);
		SDL_RenderTextureRotated(rend_data->renderer, texture(tx_pc), NULL, &rect_player, (double)RadToDeg((*(plys + i))->direction - human_player_direction), NULL, SDL_FLIP_NONE);
		if((*(plys + i))->flags & stunned){
			SDL_RenderTextureRotated(rend_data->renderer, texture(tx_stun), NULL, &rect_player, (double)(rend_data->counter * TEXTURE_ROTA_SPEED), NULL, SDL_FLIP_NONE);
		}
	}
}

static void	RenderBarriers(Render_data* const rend_data, const float human_player_direction, Player** plys, SDL_FPoint* points, const unsigned int num){
	SDL_FRect rect_barrier = {
		0.0F,
		0.0F,
		BARRIER_SIZE,
		BARRIER_SIZE
	};
	for(unsigned int i = 0U; i < num; ++i){
		if(!((*(plys + i))->flags & block)){
			continue;
		}
		rect_barrier.x = (points + i)->x - half(BARRIER_SIZE);
		rect_barrier.y = (points + i)->y - half(BARRIER_SIZE);
		if((*(plys + i))->block_times.push < 1){
			SDL_RenderTextureRotated(rend_data->renderer, texture(tx_barrier), NULL, &rect_barrier, (double)RadToDeg((*(plys + i))->direction - human_player_direction), NULL, SDL_FLIP_NONE);
		}else{
			const float size = BARRIER_SIZE + (float)(*(plys + i))->block_times.push * BARRIER_MAGNIFICATION;
			SDL_RenderTextureRotated(rend_data->renderer, texture(tx_barrier), NULL,
				&(SDL_FRect){
					(points + i)->x - half(size),
					(points + i)->y - half(size),
					size,
					size
				}, (double)RadToDeg((*(plys + i))->direction - human_player_direction), NULL, SDL_FLIP_NONE
			);
		}
	}
}

static inline Placement GetBeingWeaponPlacement(const Being* const b){
    if(b->status == being_strike || b->status == being_strike_being){
        if(b->status_ticks_left <= BEING_ATTACK_STEPS){
            const int step = BEING_ATTACK_STEPS - b->status_ticks_left;
			return GetWeaponPlacement(&(Placement)BEING_WEAPON_ATTACK_PLCMNT, &(Placement)BEING_WEAPON_BASE_PLCMNT, step, BEING_ATTACK_STEPS);
        }
        if(b->status_ticks_left <= BEING_ATTACK_STEPS * 2){
            const int step = BEING_ATTACK_STEPS * 2 - b->status_ticks_left;
            return GetWeaponPlacement(&(Placement)BEING_WEAPON_PREPARE_PLCMNT, &(Placement)BEING_WEAPON_ATTACK_PLCMNT, step, BEING_ATTACK_STEPS);
        }
        const int step = BEING_ATTACK_STEPS * 3 - b->status_ticks_left;
		return GetWeaponPlacement(&(Placement)BEING_WEAPON_BASE_PLCMNT, &(Placement)BEING_WEAPON_PREPARE_PLCMNT, step, BEING_ATTACK_STEPS);
    }else if(b->status == being_shoot || b->status == being_shoot_being){
		if(b->status_ticks_left < BEING_ATTACK_STEPS / 2){
			const int step = BEING_ATTACK_STEPS / 2 - b->status_ticks_left;
			return GetWeaponPlacement(&(Placement)BEING_WEAPON_ATTACK_PLCMNT, &(Placement)BEING_WEAPON_BASE_PLCMNT, step, BEING_ATTACK_STEPS / 2);
		}
		if(b->status_ticks_left < BEING_ATTACK_STEPS * 3){
        	return (Placement)BEING_WEAPON_ATTACK_PLCMNT;
		}
		if(b->status_ticks_left < (int)(BEING_ATTACK_STEPS * 4)){
			const int step = (int)(BEING_ATTACK_STEPS * 4) - b->status_ticks_left;
			return GetWeaponPlacement(&(Placement)BEING_WEAPON_BASE_PLCMNT, &(Placement)BEING_WEAPON_ATTACK_PLCMNT, step, BEING_ATTACK_STEPS);
		}
        return (Placement)BEING_WEAPON_BASE_PLCMNT;
    }else{
        return (Placement)BEING_WEAPON_BASE_PLCMNT;
    }
}

static inline Placement GetWeaponPlacement(Placement* const start, Placement* const end, const int step, const int steps){
	return (Placement){
		{
			start->position.x + (end->position.x - start->position.x) / (float)steps * step,
			start->position.y + (end->position.y - start->position.y) / (float)steps * step
		},
		start->direction + (end->direction - start->direction) / (float)steps * step
	};
}

static inline void RenderBackground(Render_data* const rend_data){
	SDL_RenderTexture(rend_data->renderer, texture(tx_background), NULL, NULL);
}

static void RenderFrame(Render_data* const rend_data, SDL_Texture* const tx_backgr1, const SDL_FRect* const frame, const float width){
	const SDL_FRect rects[8] = {
		{
			frame->x,
			frame->y,
			width,
			width
		}, {
			frame->x + frame->w - width,
			frame->y,
			width,
			width
		}, {
			frame->x,
			frame->y + frame->h - width,
			width,
			width
		}, {
			frame->x + frame->w - width,
			frame->y + frame->h - width,
			width,
			width
		}, {//
			frame->x + width,
			frame->y,
			frame->w - width * 2.0F,
			width
		}, {
			frame->x + width,
			frame->y + frame->h - width,
			frame->w - width * 2.0F,
			width
		}, {
			frame->x,
			frame->y + width,
			width,
			frame->h - width * 2.0F
		}, {
			frame->x + frame->w - width,
			frame->y + width,
			width,
			frame->h - width * 2.0F
		}
	};
	const SDL_FRect src_rects[3] = {
		{0.0F, 0.0F, 8.0F, 8.0F},
		{8.0F, 0.0F, 8.0F, 8.0F},
		{0.0F, 8.0F, 8.0F, 8.0F}
	};
	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects, rects);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects, rects + 1, 0.0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects, rects + 2, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects, rects + 3, 180.0, NULL, SDL_FLIP_NONE);
	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects + 1, rects + 4);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 1, rects + 5, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects + 2, rects + 6);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 2, rects + 7, 0.0, NULL, SDL_FLIP_HORIZONTAL);
}

static void RenderDirectionArrow(Render_data* const rend_data, const double rotation){
	SDL_RenderTextureRotated(rend_data->renderer, texture(tx_arrow), NULL, &(SDL_FRect){
		RIGHT_AREA_X + FRAME_W,
		FRAME_W * 2.0F,
		50.0F,
		50.0F
	}, rotation, NULL, SDL_FLIP_NONE);
}

static void RenderQuickScrolls(Render_data* const rend_data, const Player* const p){
	const float BAR_H = BAR_H_CALC;
	const float Q_SCROLL_SIZE = Q_SCROLL_SIZE_CALC;
	const float shift_scroll = Q_SCROLL_SIZE + FRAME_W * 2.5F;
	SDL_FRect scroll_rect = {
		RIGHT_AREA_X + FRAME_W * 1.5F,
		AREA_Q_SCROLLS_Y,
		Q_SCROLL_SIZE,
		Q_SCROLL_SIZE
	};
	SDL_SetTextureColorMod(texture(tx_chars), 255U, 0U, 0U);
	for(unsigned int i = 0U; i < 3U; ++i){
		scroll_rect.x = RIGHT_AREA_X + FRAME_W * 1.5F;
		for(unsigned int j = 0U; j < 3U; ++j){
			const unsigned int scroll_id = *(p->scrolls_quick_access + i * 3 + j);
			const int scrolls_num = *(p->scrolls + scroll_id);
			const SDL_FRect src_rect = GetScrollTextureSrcRect(scroll_id);
			if(scrolls_num < 1){
				SDL_SetTextureAlphaMod(texture(tx_icons), 31U);
				SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &src_rect, &scroll_rect);
			}else{
				SDL_SetTextureAlphaMod(texture(tx_icons), 255U);
				SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &src_rect, &scroll_rect);
				SDL_SetTextureColorMod(texture(tx_chars), WHITE_RGB);
				RenderIntFromRight(rend_data, scroll_rect.x + Q_SCROLL_SIZE - 1.0F, scroll_rect.y + Q_SCROLL_SIZE - 15.0F, 16.0F, scrolls_num);
				SDL_SetTextureColorMod(texture(tx_chars), 255U, 0U, 0U);
			}
			RenderInt(rend_data, scroll_rect.x + 1.0F, scroll_rect.y - 1.0F, 16.0F, (i * 3 + j + 2) % 10);
			scroll_rect.x += shift_scroll;
		}
		scroll_rect.y += shift_scroll;
	}
	SDL_SetTextureColorMod(texture(tx_chars), WHITE_RGB);
	for(unsigned int i = 0U; i < 9U; ++i){
		if(p->selected_scroll == *(p->scrolls_quick_access + i)){
			SDL_SetRenderDrawColor(rend_data->renderer, 255, 0, 0, 255);
			SDL_RenderRect(rend_data->renderer, &(SDL_FRect){
				RIGHT_AREA_X + FRAME_W * 1.5F + shift_scroll * (i % 3),
				AREA_Q_SCROLLS_Y + shift_scroll * (i / 3),
				Q_SCROLL_SIZE,
				Q_SCROLL_SIZE
			});
			break;
		}
	}
}

static void RenderScrollsManagement(Render_data* const rend_data, const Player* const pc){
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, 127);
	SDL_RenderFillRect(rend_data->renderer, NULL);
	const float icon_size = SCROLLS_MANAG_ICON_SIZE;
	const float scrolls_num_text_height = icon_size * 0.25F;
	SDL_FRect dst_rect = {
		FRAME_W,
		icon_size + FRAME_W * 3.0F,
		icon_size,
		icon_size
	};
	const float shift = icon_size + FRAME_W;
	const SDL_FRect menu_ptr_rect = {
		FRAME_W + shift * (pc->help_data.menu_position % ICONS_IN_VIEWF_ROW),
		icon_size + FRAME_W * 3.0F + shift * (pc->help_data.menu_position / ICONS_IN_VIEWF_ROW),
		icon_size,
		icon_size
	};
	SDL_FRect quick_scroll_rect = {
		FRAME_W + shift * (*pc->scrolls_quick_access % ICONS_IN_VIEWF_ROW),
		icon_size + FRAME_W * 3.0F + shift * (*pc->scrolls_quick_access / ICONS_IN_VIEWF_ROW),
		icon_size,
		icon_size
	};
	const SDL_FRect selection_rect = {
		FRAME_W + shift * (pc->selected_scroll % ICONS_IN_VIEWF_ROW),
		icon_size + FRAME_W * 3.0F + shift * (pc->selected_scroll / ICONS_IN_VIEWF_ROW),
		icon_size,
		icon_size
	};
	for(unsigned int i = 0U; i < SCROLLS_NUM; ++i){
		const unsigned int col = i % ICONS_IN_VIEWF_ROW;
		const unsigned int row = i / ICONS_IN_VIEWF_ROW;
		dst_rect.x = FRAME_W + shift * col;
		dst_rect.y = icon_size + FRAME_W * 3.0F + shift * row;
		const int scrolls_num = *(pc->scrolls + i);
		const SDL_FRect src_rect = GetScrollTextureSrcRect(i);
		if(scrolls_num < 1){
			SDL_SetTextureAlphaMod(texture(tx_icons), 31U);
			SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &src_rect, &dst_rect);
		}else{
			SDL_SetTextureAlphaMod(texture(tx_icons), 255U);
			SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &src_rect, &dst_rect);
			const SDL_FPoint dst_point_scrolls_num = {
				4.0F + icon_size + shift * col,
				icon_size * 1.75F + FRAME_W * 3.0F + shift * row 
			};
			RenderIntFromRight(rend_data, dst_point_scrolls_num.x, dst_point_scrolls_num.y, scrolls_num_text_height, scrolls_num);
		}
	}
	SDL_SetTextureColorMod(texture(tx_menu_ptr), 31U, 127U, 255U);
	SDL_SetTextureColorMod(texture(tx_chars), 255U, 0U, 0U);
	for(unsigned int i = 0U; i < 9U; ++i){
		quick_scroll_rect.x = FRAME_W + shift * (*(pc->scrolls_quick_access + i) % ICONS_IN_VIEWF_ROW);
		quick_scroll_rect.y = icon_size + FRAME_W * 3.0F + shift * (*(pc->scrolls_quick_access + i) / ICONS_IN_VIEWF_ROW);
		SDL_RenderTexture(rend_data->renderer, texture(tx_menu_ptr), NULL, &quick_scroll_rect);
		RenderInt(rend_data, quick_scroll_rect.x + 4.0F, quick_scroll_rect.y, scrolls_num_text_height, (i + 2) % 10);
	}
	SDL_SetTextureColorMod(texture(tx_menu_ptr), WHITE_RGB);
	SDL_SetTextureColorMod(texture(tx_chars), WHITE_RGB);
	SDL_RenderTexture(rend_data->renderer, texture(tx_menu_ptr), NULL, &menu_ptr_rect);
	SDL_SetRenderDrawColor(rend_data->renderer, 255, 0, 0, 255);
	SDL_RenderRect(rend_data->renderer, &selection_rect);
}

static void RenderMenu(Render_data* const rend_data, const Player* const pc){
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, 127);
	SDL_RenderFillRect(rend_data->renderer, NULL);
	const Uint8* texts[] = MENU_TEXTS;
	const unsigned int texts_chars_num[] = MENU_TEXTS_SIZES;
	const float icon_size = SCROLLS_MANAG_ICON_SIZE;
	const float text_height = icon_size * 0.5F;
	SDL_FPoint dst_point_text = {
		icon_size,
		icon_size * 1.25F + FRAME_W * 3.0F
	};
	const float shift = icon_size + FRAME_W;
	const SDL_FRect menu_ptr_rect = {
		FRAME_W,
		icon_size + FRAME_W * 3.0F + shift * pc->help_data.menu_position,
		rend_data->viewfinder_rect.w - FRAME_W * 2.0F,
		icon_size
	};
	for(unsigned int i = 0U; i < OPTIONS_NUM; ++i){
		RenderTextCentered(rend_data, 0.0F, dst_point_text.y, text_height, *(texts + i), *(texts_chars_num + i), rend_data->viewfinder);
		dst_point_text.y += shift;
	}
	SDL_RenderTexture(rend_data->renderer, texture(tx_menu_ptr), NULL, &menu_ptr_rect);
}

static void DrawBackgroud(Render_data* const rend_data, SDL_Surface* surface, char* bmp_path){
	if(texture(tx_background) != NULL){
		SDL_DestroyTexture(texture(tx_background));
	}
	texture(tx_background) = SDL_CreateTexture(rend_data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rend_data->window_w, rend_data->window_h);
	if(!(texture(tx_background))){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError());
		exit(-1);
	}
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), BACKGROUND_TX0_FILE_NAME);
	surface = SDL_LoadBMP(bmp_path);
	if(!surface){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		exit(-1);
	}
	SDL_Texture* tx_backgr0 = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	if(!tx_backgr0){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		exit(-1);
	}
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), BACKGROUND_TX1_FILE_NAME);
	SDL_DestroySurface(surface);
	surface = SDL_LoadBMP(bmp_path);
	if(!surface){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		exit(-1);
	}
	SDL_Texture* tx_backgr1 = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	if(!tx_backgr1){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		exit(-1);
	}
	SDL_free(bmp_path);
	SDL_DestroySurface(surface);
	SDL_SetRenderTarget(rend_data->renderer, texture(tx_background));
	const SDL_FRect nesw_rect = {
		RIGHT_AREA_X + FRAME_W,
		FRAME_W * 2.0F,
		50.0F,
		50.0F
	};
	const SDL_FRect rects[16] = {
		{
			FRAME_W,
			FRAME_W,
			SIDE_AREA_WIDTH,
			rend_data->window_h - FRAME_W * 2.0
		}, {
			RIGHT_AREA_X,
			FRAME_W,
			SIDE_AREA_WIDTH,
			rend_data->window_h - FRAME_W * 2.0
		}, {//
			SIDE_AREA_WIDTH,
			0.0F,
			FRAME_W,
			FRAME_W
		}, {
			RIGHT_AREA_X,
			0.0F,
			FRAME_W,
			FRAME_W
		}, {
			SIDE_AREA_WIDTH,
			rend_data->viewfinder_rect.y + rend_data->viewfinder_rect.h,
			FRAME_W,
			FRAME_W
		}, {
			RIGHT_AREA_X,
			rend_data->viewfinder_rect.y + rend_data->viewfinder_rect.h,
			FRAME_W,
			FRAME_W
		}, {//
			0.0F,
			0.0F,
			FRAME_W,
			FRAME_W
		}, {
			0.0F + rend_data->window_w - FRAME_W,
			0.0F,
			FRAME_W,
			FRAME_W
		}, {
			0.0F,
			0.0F + rend_data->window_h - FRAME_W,
			FRAME_W,
			FRAME_W
		}, {
			0.0F + rend_data->window_w - FRAME_W,
			0.0F + rend_data->window_h - FRAME_W,
			FRAME_W,
			FRAME_W
		}, {//
			FRAME_W,
			0.0F,
			rend_data->viewfinder_rect.x - FRAME_W * 3.0F,
			FRAME_W
		}, {
			RIGHT_AREA_X + FRAME_W,
			0.0F,
			rend_data->viewfinder_rect.x - FRAME_W * 3.0F,
			FRAME_W
		}, {
			FRAME_W,
			rend_data->window_h - FRAME_W,
			rend_data->viewfinder_rect.x - FRAME_W * 3.0F,
			FRAME_W
		}, {
			RIGHT_AREA_X + FRAME_W,
			rend_data->window_h - FRAME_W,
			rend_data->viewfinder_rect.x - FRAME_W * 3.0F,
			FRAME_W
		}, {//
			0.0F,
			FRAME_W,
			FRAME_W,
			rend_data->window_h - FRAME_W * 2.0F
		}, {
			rend_data->window_w - FRAME_W,
			FRAME_W,
			FRAME_W,
			rend_data->window_h - FRAME_W * 2.0F
		}
	};
	const SDL_FRect src_rects[6] = {
		{0.0F, 0.0F, 512.0F, 1024.0F},
		{512.0F, 0.0F, 512.0F, 1024.0F},
		{0.0F, 0.0F, 8.0F, 8.0F},
		{8.0F, 0.0F, 8.0F, 8.0F},
		{0.0F, 8.0F, 8.0F, 8.0F},
		{8.0F, 8.0F, 8.0F, 8.0F}
	};
	const float BAR_H = BAR_H_CALC;
	const float Q_SCROLL_SIZE = Q_SCROLL_SIZE_CALC;
	SDL_FRect frames[] = {
		{
			ICON_X - FRAME_W,
			AREAA_Y - FRAME_W,
			BAR_H + FRAME_W * 2.0F,
			BAR_H + FRAME_W * 2.0F
		}, {
			BAR_X - FRAME_W,
			AREAA_Y + BAR_H + FRAME_W,
			BAR_W + FRAME_W * 2.0F,
			BAR_H + FRAME_W * 2.0F
		}, {
			RIGHT_AREA_X + FRAME_W,
			AREA_MP_Y - FRAME_W,
			BAR_H + FRAME_W * 2.0F,
			BAR_H + FRAME_W * 2.0F
		}, {
			RIGHT_AREA_X + FRAME_W,
			AREA_MP_Y + BAR_H + FRAME_W,
			BAR_W + FRAME_W * 2.0F,
			MP_BAR_H * 2.0F + FRAME_W * 2.0F
		}, {
			RIGHT_AREA_X + FRAME_W * 0.5F,
			AREA_Q_SCROLLS_Y - FRAME_W,
			Q_SCROLL_SIZE + FRAME_W * 2.0F,
			Q_SCROLL_SIZE + FRAME_W * 2.0F
		}, {
			nesw_rect.x - FRAME_W,
			nesw_rect.y - FRAME_W,
			nesw_rect.w + FRAME_W * 2.0F,
			nesw_rect.h + FRAME_W * 2.0F
		}, {
			ICON_X + BAR_H + FRAME_W,
			KEY_ICON_FRAME_Y,
			BAR_W - BAR_H,
			BAR_H + FRAME_W * 2.0F
		}
	};
	SDL_FRect empty_areas[] = {
		{
			ICON_X,
			AREAA_Y,
			BAR_H,
			BAR_H
		}, {
			RIGHT_AREA_X + FRAME_W * 2.0F,
			AREA_MP_Y,
			BAR_H,
			BAR_H
		}, {
			RIGHT_AREA_X + FRAME_W * 2.0F,
			AREA_MP_Y + BAR_H + FRAME_W * 2.0F,
			BAR_W,
			MP_BAR_H
		}, {
			RIGHT_AREA_X + FRAME_W * 1.5F,
			AREA_Q_SCROLLS_Y,
			Q_SCROLL_SIZE,
			Q_SCROLL_SIZE
		}
	};
	const SDL_FRect empty_bar_rects[] = {
		{
			BAR_X,
			AREAA_Y + BAR_H + FRAME_W * 2.0F,
			BAR_W,
			SMALL_BAR_H
		}, {
			BAR_X,
			AREAA_Y + BAR_H + FRAME_W * 2.0F + SMALL_BAR_H,
			BAR_W,
			SMALL_BAR_H
		}, {
			BAR_X,
			AREAA_Y + BAR_H * 3.0F + FRAME_W * 7.0F,
			BAR_W,
			BAR_H
		}, {
			BAR_X,
			AREAA_Y + BAR_H * 5.0F + FRAME_W * 12.0F,
			BAR_W,
			BAR_H
		}, {
			BAR_X,
			KEY_ICON_FRAME_Y + FRAME_W,
			BAR_W ,
			BAR_H
		}, {
			BAR_X,
			KEY_ICON_FRAME_Y + FRAME_W * 4.0F + BAR_H,
			BAR_W,
			BAR_H
		}
	};
	SDL_FRect icon_scr_rect = FIRST_ICON_SRC_RECT;
	SDL_RenderTexture(rend_data->renderer, tx_backgr0, src_rects, rects);
	SDL_RenderTexture(rend_data->renderer, tx_backgr0, src_rects + 1, rects + 1);

	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 2, rects + 2, 180.0, NULL, SDL_FLIP_NONE);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 2, rects + 3, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 2, rects + 4, 0.0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects + 2, rects + 5);

	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects + 5, rects + 6);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 5, rects + 7, 0.0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 5, rects + 8, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 5, rects + 9, 180.0, NULL, SDL_FLIP_NONE);

	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 3, rects + 10, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 3, rects + 11, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects + 3, rects + 12);
	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects + 3, rects + 13);

	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 4, rects + 14, 0.0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects + 4, rects + 15);

	RenderFrame(rend_data, tx_backgr1, &(SDL_FRect)VIEWFINDER_FRAME, FRAME_W);
	RenderPortrait(rend_data, tx_backgr1);
	const float shift_bar = BAR_H * 2.0F + FRAME_W * 5.0F;
	const float shift_scroll = Q_SCROLL_SIZE + FRAME_W * 2.5F;
	icon_scr_rect.x = ICON_TX_SIZE * ic_arm;
	for(unsigned int i = 0U; i < 3U; ++i){
		RenderFrame(rend_data, tx_backgr1, frames, FRAME_W);
		RenderFrame(rend_data, tx_backgr1, frames + 1, FRAME_W);
		SDL_SetTextureColorMod(tx_backgr1, 127U, 127U, 127U);
		SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_areas, 0.0F, NULL, SDL_FLIP_VERTICAL);
		SDL_SetTextureColorMod(tx_backgr1, WHITE_RGB);
		SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &icon_scr_rect, empty_areas);
		icon_scr_rect.x += ICON_TX_SIZE;
		frames->y += shift_bar;
		(frames + 1)->y += shift_bar;
		empty_areas->y += shift_bar;
	}
	SDL_SetTextureColorMod(tx_backgr1, 127U, 127U, 127U);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_bar_rects + 4, 0.0F, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_bar_rects + 5, 0.0F, NULL, SDL_FLIP_VERTICAL);
	empty_areas->y = KEY_ICON_FRAME_Y + FRAME_W;
	icon_scr_rect.x = ICON_TX_SIZE * ic_key;
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &icon_scr_rect, empty_areas);
	empty_areas->y += BAR_H + FRAME_W * 3.0F;
	icon_scr_rect.x = ICON_TX_SIZE * ic_coin;
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &icon_scr_rect, empty_areas);
	SDL_SetTextureColorMod(tx_backgr1, WHITE_RGB);
	frames->y = KEY_ICON_FRAME_Y;
	RenderFrame(rend_data, tx_backgr1, frames, FRAME_W);
	RenderFrame(rend_data, tx_backgr1, frames + 6, FRAME_W);
	(frames + 6)->y += BAR_H + FRAME_W * 3.0F;
	RenderFrame(rend_data, tx_backgr1, frames + 6, FRAME_W);
	frames->y +=  BAR_H + FRAME_W * 3.0F;
	RenderFrame(rend_data, tx_backgr1, frames, FRAME_W);
	RenderFrame(rend_data, tx_backgr1, frames + 2, FRAME_W);
	RenderFrame(rend_data, tx_backgr1, frames + 3, FRAME_W);
	RenderFrame(rend_data, tx_backgr1, frames + 5, FRAME_W);
	SDL_SetTextureColorMod(tx_backgr1, 127U, 127U, 127U);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_areas + 1, 0.0F, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_areas + 2, 0.0F, NULL, SDL_FLIP_VERTICAL);
	(empty_areas + 2)->y += MP_BAR_H;
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_areas + 2, 0.0F, NULL, SDL_FLIP_VERTICAL);
	SDL_SetTextureColorMod(tx_backgr1, WHITE_RGB);
	icon_scr_rect.x = ICON_TX_SIZE * ic_mp;
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &icon_scr_rect, empty_areas + 1);
	for(unsigned int i = 0U; i < 3U; ++i){
		(frames + 4)->x = RIGHT_AREA_X + FRAME_W * 0.5F;
		(empty_areas + 3)->x = RIGHT_AREA_X + FRAME_W * 1.5F;
		for(unsigned int j = 0U; j < 3U; ++j){
			RenderFrame(rend_data, tx_backgr1, frames + 4, FRAME_W);
			SDL_SetTextureColorMod(tx_backgr1, 127U, 127U, 127U);
			SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_areas + 3, 0.0F, NULL, SDL_FLIP_VERTICAL);
			SDL_SetTextureColorMod(tx_backgr1, WHITE_RGB);
			(frames + 4)->x += shift_scroll;
			(empty_areas + 3)->x += shift_scroll;
		}
		(frames + 4)->y += shift_scroll;
		(empty_areas + 3)->y += shift_scroll;
	}
	for(unsigned int i = 0U; i < 4U; ++i){
		SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_bar_rects + i, 0.0F, NULL, SDL_FLIP_VERTICAL);
	}
	SDL_SetTextureColorMod(tx_backgr0, 15U, 0U, 0U);
	SDL_RenderTexture(rend_data->renderer, tx_backgr0, NULL, &(SDL_FRect){
		rend_data->viewfinder_rect.x,
		rend_data->viewfinder_rect.y,
		rend_data->viewfinder_rect.w,
		rend_data->viewfinder_rect.h
	});
	SDL_SetTextureColorMod(tx_backgr0, WHITE_RGB);
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(rend_data->renderer, &nesw_rect);
	icon_scr_rect.x = ICON_TX_SIZE * ic_directions;
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &icon_scr_rect, &nesw_rect);
	SDL_SetRenderTarget(rend_data->renderer, NULL);
	SDL_DestroyTexture(tx_backgr0);
	SDL_DestroyTexture(tx_backgr1);
}

static void RenderPortrait(Render_data* const rend_data, SDL_Texture* const tx_backgr1){
	const SDL_FRect portrait_frame_rect = {
		SIDE_AREA_WIDTH < PORTRAIT_H ? PORTRAIT_Y - FRAME_W : FRAME_W + half(SIDE_AREA_WIDTH) - half(PORTRAIT_H),
		PORTRAIT_Y - FRAME_W,
		SIDE_AREA_WIDTH < PORTRAIT_H ? SIDE_AREA_WIDTH - FRAME_W * 2.0F : PORTRAIT_H,
		SIDE_AREA_WIDTH < PORTRAIT_H ? SIDE_AREA_WIDTH - FRAME_W * 2.0F : PORTRAIT_H
	};
	const SDL_FRect portrait_rect = {
		portrait_frame_rect.x + FRAME_W,
		portrait_frame_rect.y + FRAME_W,
		portrait_frame_rect.w - FRAME_W * 2.0F,
		portrait_frame_rect.h - FRAME_W * 2.0F,
	};
	RenderFrame(rend_data, tx_backgr1, &portrait_frame_rect, FRAME_W);
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(rend_data->renderer, &portrait_rect);
	SDL_RenderTexture(rend_data->renderer, texture(tx_icon), NULL, &portrait_rect);
}

static SDL_FPoint GetCharacterXPositionAndWidth(const int character_num){
	const SDL_FPoint x_positions[] = CHARACTERS_X_AND_W;
	return *(x_positions + character_num);
}

static inline unsigned int GetCharacterRow(const int character_num){
	if(character_num >= ROW_5_CHARS){
		return 6U;
	}
	if(character_num >= ROW_4_CHARS){
		return 5U;
	}
	if(character_num >= ROW_3_CHARS){
		return 4U;
	}
	if(character_num >= ROW_2_CHARS){
		return 3U;
	}
	if(character_num >= ROW_1_CHARS){
		return 2U;
	}
	if(character_num >= ROW_0_CHARS){
		return 1U;
	}
	return 0U;
}

static void RenderText(Render_data* const rend_data, float x, const float y, const float size, const Uint8* char_nums){
	const float size_factor = size / CHARS_ROW_HEIGHT;
	do{
		const SDL_FPoint x_and_width = GetCharacterXPositionAndWidth(*char_nums);
		const float char_width = x_and_width.y * size_factor;
		SDL_RenderTexture(rend_data->renderer, texture(tx_chars), &(SDL_FRect){
			x_and_width.x,
			GetCharacterRow(*char_nums) * CHARS_ROW_HEIGHT,
			x_and_width.y,
			CHARS_ROW_HEIGHT
		}, &(SDL_FRect){
			x,
			y,
			char_width,
			CHARS_ROW_HEIGHT * size_factor
		});
		x += char_width + 1.0F;
		++char_nums;
	}while(*char_nums != char_end);
}

static void RenderTextWithKnownCharsXAndWidth(Render_data* const rend_data, float x, const float y, const float size_factor, const Uint8* const char_nums, const unsigned int chars_num, const SDL_FPoint* const chars_x_and_width){
	for(unsigned int i = 0U; i < chars_num; ++i){
		const SDL_FPoint x_and_width = *(chars_x_and_width + i);
		const float char_width = x_and_width.y * size_factor;
		SDL_RenderTexture(rend_data->renderer, texture(tx_chars), &(SDL_FRect){
			x_and_width.x,
			GetCharacterRow(*(char_nums + i)) * CHARS_ROW_HEIGHT,
			x_and_width.y,
			CHARS_ROW_HEIGHT
		}, &(SDL_FRect){
			x,
			y,
			char_width,
			CHARS_ROW_HEIGHT * size_factor
		});
		x += char_width + 1.0F;
	}
}

static void RenderTextCentered(Render_data* const rend_data, float x, const float y, const float size, const Uint8* char_nums, const unsigned int chars_num, const float space){
	SDL_FPoint chars_x_and_width[chars_num];
	float width = -1.0F;
	const float size_factor = size / CHARS_ROW_HEIGHT;
	for(unsigned int i = 0U; i < chars_num; ++i){
		*(chars_x_and_width + i) = GetCharacterXPositionAndWidth(*(char_nums + i));
		width += (chars_x_and_width + i)->y * size_factor + 1.0F;
	}
	x += half(space - width);
	RenderTextWithKnownCharsXAndWidth(rend_data, x, y, size_factor, char_nums, chars_num, chars_x_and_width);
}

static void RenderTextFromRight(Render_data* const rend_data, float x, const float y, const float size, const Uint8* char_nums, const unsigned int chars_num){
	SDL_FPoint chars_x_and_width[chars_num];
	float width = -1.0F;
	const float size_factor = size / CHARS_ROW_HEIGHT;
	for(unsigned int i = 0U; i < chars_num; ++i){
		*(chars_x_and_width + i) = GetCharacterXPositionAndWidth(*(char_nums + i));
		width += (chars_x_and_width + i)->y * size_factor + 1.0F;
	}
	x -= width;
	RenderTextWithKnownCharsXAndWidth(rend_data, x, y, size_factor, char_nums, chars_num, chars_x_and_width);
}

static void RenderInt(Render_data* const rend_data, const float x, const float y, const float size, int num){
	const Uint8 cipher_nums[] = CIPHER_NUMS;
	unsigned int text_len = 1U;
	int tmp = 10;
	while(num >= tmp){
		++text_len;
		tmp *= 10;
	}
	Uint8 char_nums[text_len + 1U];
	for(unsigned int i = 0U; i < text_len; ++i){
		tmp /= 10;
		unsigned int cipher = num / tmp;
		*(char_nums + i) = *(cipher_nums + cipher);
		num %= tmp;
	}
	*(char_nums + text_len) = char_end;
	RenderText(rend_data, x, y, size, char_nums);
}

static void RenderIntCentered(Render_data* const rend_data, const float x, const float y, const float size, int num, const float space){
	const Uint8 cipher_nums[] = CIPHER_NUMS;
	unsigned int text_len = 1U;
	int tmp = 10;
	while(num >= tmp){
		++text_len;
		tmp *= 10;
	}
	Uint8 char_nums[text_len];
	for(unsigned int i = 0U; i < text_len; ++i){
		tmp /= 10;
		unsigned int cipher = num / tmp;
		*(char_nums + i) = *(cipher_nums + cipher);
		num %= tmp;
	}
	RenderTextCentered(rend_data, x, y, size, char_nums, text_len, space);
}

static void RenderSignedIntCentered(Render_data* const rend_data, const float x, const float y, const float size, int num, const float space){
	const Uint8 cipher_nums[] = CIPHER_NUMS;
	Uint8 sign;
	if(num < 0){
		num = -num;
		sign = s_minus;
	}else{
		sign = s_plus;
	}
	unsigned int text_len = 2U;
	int tmp = 10;
	while(num >= tmp){
		++text_len;
		tmp *= 10;
	}
	Uint8 char_nums[text_len + 1U];
	*(char_nums) = sign;
	for(unsigned int i = 1U; i < text_len; ++i){
		tmp /= 10;
		unsigned int cipher = num / tmp;
		*(char_nums + i) = *(cipher_nums + cipher);
		num %= tmp;
	}
	RenderTextCentered(rend_data, x, y, size, char_nums, text_len, space);
}

static void RenderIntFromRight(Render_data* const rend_data, const float x, const float y, const float size, int num){
	const Uint8 cipher_nums[] = CIPHER_NUMS;
	unsigned int text_len = 1U;
	int tmp = 10;
	while(num >= tmp){
		++text_len;
		tmp *= 10;
	}
	Uint8 char_nums[text_len];
	for(unsigned int i = 0U; i < text_len; ++i){
		tmp /= 10;
		unsigned int cipher = num / tmp;
		*(char_nums + i) = *(cipher_nums + cipher);
		num %= tmp;
	}
	RenderTextFromRight(rend_data, x, y, size, char_nums, text_len);
}

extern inline void ResetTextTextureAlpha(Render_data* const rend_data){
	SDL_SetTextureAlphaMod(texture(tx_chars), 255U);
}

void SetSelectetScrollMouseSelection(const Render_data* const rend_data, Player* const p){
	const int num = GetMouseScrollManagPositionNum(rend_data);
	if(num >= 0){
		p->selected_scroll = num;
	}
}

void SetPointedScrollMouseSelection(const Render_data* const rend_data, Player* const pc){
	const int num = GetMouseScrollManagPositionNum(rend_data);
	if(num >= 0){
		pc->help_data.menu_position = num;
	}
}

unsigned int GetMouseShopSelection(const Render_data* const rend_data){
	const float width = (float)rend_data->window_w / SHOP_COLS;
	const float height = (float)rend_data->window_h / SHOP_ROWS;
	SDL_FPoint mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);
	mouse.x -= half(width);
	if(mouse.x <= 0.0F || mouse.x >= (float)rend_data->window_w - width){
		return SHOP_POSITIONS;
	}
	const unsigned int col = (unsigned int)(mouse.x / width);
	const unsigned int row = (unsigned int)(mouse.y / height);
	if(col != SHOP_SIDE_COLS + 1U && col != SHOP_SIDE_COLS + 3U){
		if(
			(row < FIRST_SHOP_ROW || (row == SHOP_ROWS - 1U && col != SHOP_SIDE_COLS + 2U))
		 	|| (col == SHOP_SIDE_COLS || (col == SHOP_SIDE_COLS + 2U && row < SHOP_ROWS - 1U) || col == SHOP_SIDE_COLS + 4U)
			|| (row < FIRST_SHOP_ROW + 1U && col > SHOP_SIDE_COLS + 4U)
		){
			return SHOP_POSITIONS;
		}
	}
	return col + row * SHOP_COLS;
}

void SetPointedOptionMouseSelection(const Render_data* const rend_data, unsigned int* const menu_position){
	const int num = GetMouseMenuPositionNum(rend_data);
	if(num >= 0){
		*menu_position = num;
	}
}

int GetMouseScrollManagPositionNum(const Render_data* const rend_data){
	const float icon_size = SCROLLS_MANAG_ICON_SIZE;
	const float shift = icon_size + FRAME_W;
	const SDL_FRect manage_scrolls_rect = MANAGE_SCROLLS_RECT;
	SDL_FPoint mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);
	if(SDL_PointInRectFloat(&mouse, &manage_scrolls_rect)){
		const unsigned int num = (unsigned int)((mouse.x - manage_scrolls_rect.x) / shift) + (unsigned int)((mouse.y - manage_scrolls_rect.y) / shift) * ICONS_IN_VIEWF_ROW;
		if(num < SCROLLS_NUM){
			return num;
		}
		return SCROLLS_NUM - 1U;
	}
	return -1;
}

static int GetMouseMenuPositionNum(const Render_data* const rend_data){
	const float icon_size = SCROLLS_MANAG_ICON_SIZE;
	const float shift = icon_size + FRAME_W;
	const SDL_FRect menu_rect = MENU_RECT;
	SDL_FPoint mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);
	if(SDL_PointInRectFloat(&mouse, &menu_rect)){
		const unsigned int num = (unsigned int)((mouse.y - menu_rect.y) / shift);
		if(num < OPTIONS_NUM){
			return num;
		}
		return OPTIONS_NUM - 1U;
	}
	return -1;
}

void RenderDefeatedScreen(Render_data* const rend_data){
	SDL_SetRenderDrawColor(rend_data->renderer, 0U, 0U, 0U, 255U);
	SDL_RenderClear(rend_data->renderer);
	SDL_SetTextureColorMod(texture(tx_chars), 255U, 0U, 0U);
	const float text_height = rend_data->window_h * 0.125F;
	RenderTextCentered(rend_data, 0.0F, WINDOW_CENTER_Y - half(text_height), text_height, (Uint8[])DEFEAT_TEXT, DEFEAT_TEXT_LEN, rend_data->window_w);
	SDL_SetTextureColorMod(texture(tx_chars), WHITE_RGB);
	SDL_RenderPresent(rend_data->renderer);
}

void RenderVictoryScreen(Render_data* const rend_data){
	SDL_SetRenderDrawColor(rend_data->renderer, 0U, 0U, 0U, 255U);
	SDL_RenderClear(rend_data->renderer);
	SDL_SetTextureColorMod(texture(tx_chars), 0U, 255U, 0U);
	const float text_height = rend_data->window_h * 0.125F;
	RenderTextCentered(rend_data, 0.0F, WINDOW_CENTER_Y - half(text_height), text_height, (Uint8[])VICTORY_TEXT, VICTORY_TEXT_LEN, rend_data->window_w);
	SDL_SetTextureColorMod(texture(tx_chars), WHITE_RGB);
	SDL_RenderPresent(rend_data->renderer);
}

extern inline void SetMouseBarrier(Render_data* const rend_data){
	SDL_SetWindowMouseRect(rend_data->window, &(SDL_Rect)MOUSE_RECT);
}

static inline SDL_FRect GetScrollTextureSrcRect(unsigned int scroll_num){
	return (SDL_FRect){
		(scroll_num % TX_ICONS_IN_ROW) * ICON_TX_SIZE,
		ICON_SCRL_0_Y + (scroll_num / TX_ICONS_IN_ROW) * ICON_TX_SIZE,
		ICON_TX_SIZE,
		ICON_TX_SIZE
	};
}

static void RenderShopElement(Render_data* const rend_data, const float x, const float y, const float size, const int price, const unsigned int texture_num, const SDL_FRect* const src_rect){
	const float texture_x = x + size * 0x1.0p-4F;
	const float texture_size = size * 0x7.0p-3F;
	const float text_y = y + size * 0x3.0p-2F;
	SDL_RenderTexture(rend_data->renderer, texture(texture_num), src_rect, &(SDL_FRect){texture_x, y, texture_size, texture_size});
	RenderIntCentered(rend_data, x, text_y, size * 0x1.0p-2F, price, size);
}

void RenderShop(Render_data* const rend_data, const Player* const pc, const Shop* const sh, const Uint8* const items_to_sell, const unsigned int items_to_sell_num, const Uint8* const items_to_get, const unsigned int items_to_get_num, const int profit){
	SDL_SetRenderDrawColor(rend_data->renderer, 0U, 0U, 0U, 255U);
	SDL_RenderClear(rend_data->renderer);
	const float width = (float)rend_data->window_w / SHOP_COLS;
	const float height = (float)rend_data->window_h / SHOP_ROWS;
	for(unsigned int i = 0U; i < scroll_empty; ++i){
		if(*(pc->scrolls + i) > 0U){
			SDL_FRect src_rect = GetScrollTextureSrcRect(i);
			RenderShopElement(rend_data, width * (0.5F + (i % SHOP_SIDE_COLS)), height * ((i / SHOP_SIDE_COLS) + FIRST_SHOP_ROW), width, ItemPrice(i) / SELL_DIVIDER, tx_icons, &src_rect);
		}
	}
	for(unsigned int i = 0U; i < items_to_sell_num; ++i){
		SDL_FRect src_rect = GetScrollTextureSrcRect(*(items_to_sell + i));
		RenderShopElement(rend_data, width * (0.5F + SHOP_SIDE_COLS + 1.0F), height * i, width, ItemPrice(*(items_to_sell + i)) / SELL_DIVIDER, tx_icons, &src_rect);
	}
	for(unsigned int i = 0U; i < items_to_get_num; ++i){
		if(*(items_to_get + i) < shop_item_invalid){
			SDL_FRect src_rect = GetScrollTextureSrcRect(*(items_to_get + i));
			RenderShopElement(rend_data, width * (0.5F + SHOP_SIDE_COLS + 3.0F), height * i, width, ItemPrice(*(items_to_get + i)), tx_icons, &src_rect);
		}else{
			RenderShopElement(rend_data, width * (0.5F + SHOP_SIDE_COLS + 3.0F), height * i, width, ItemPrice(*(items_to_get + i)), tx_icon, NULL);
		}
	}
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &(SDL_FRect){ICON_TX_SIZE * ic_mp, 0.0F, ICON_TX_SIZE, ICON_TX_SIZE}, &(SDL_FRect){
		width * 0.5F,
		height * (FIRST_SHOP_ROW - 1.0F),
		width,
		width
	});
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &(SDL_FRect){ICON_TX_SIZE * ic_coin, 0.0F, ICON_TX_SIZE, ICON_TX_SIZE}, &(SDL_FRect){
		width * (0.5F + SHOP_SIDE_COLS - 3.0F),
		height * (FIRST_SHOP_ROW - 1.0F),
		width,
		width
	});
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &(SDL_FRect){ICON_TX_SIZE * ic_mp, 0.0F, ICON_TX_SIZE, ICON_TX_SIZE}, &(SDL_FRect){
		width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS - 1.0F)),
		height * FIRST_SHOP_ROW,
		width,
		width
	});
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &(SDL_FRect){ICON_TX_SIZE * ic_key, 0.0F, ICON_TX_SIZE, ICON_TX_SIZE}, &(SDL_FRect){
		width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS)),
		height * FIRST_SHOP_ROW,
		width,
		width
	});
	SDL_RenderTexture(rend_data->renderer, texture(tx_icon), NULL, &(SDL_FRect){
		width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS + 1.0F)),
		height * FIRST_SHOP_ROW,
		width,
		width
	});
	SDL_RenderTexture(rend_data->renderer, texture(tx_icon), NULL, &(SDL_FRect){
		width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS - 1.0F)),
		height * (FIRST_SHOP_ROW + 4.0F),
		width,
		width
	});
	SDL_RenderTexture(rend_data->renderer, texture(tx_icon), NULL, &(SDL_FRect){
		width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS)),
		height * (FIRST_SHOP_ROW + 4.0F),
		width,
		width
	});
	SDL_SetTextureColorMod(texture(tx_chars), MP_TEXT_RGB);
	RenderIntCentered(rend_data, width * 1.5F, height * (FIRST_SHOP_ROW - 1.0F) + width * 0.25F, half(width), pc->magic_points, width * 2.0F);
	SDL_SetTextureColorMod(texture(tx_chars), WHITE_RGB);
	RenderIntCentered(rend_data, width * (0.5F + SHOP_SIDE_COLS - 2.0F), height * (FIRST_SHOP_ROW - 1.0F) + width * 0.25F, half(width), pc->coins, width * 2.0F);
	if(profit){
		RenderSignedIntCentered(rend_data, 0.0F, height * (SHOP_ROWS - 1.0F), height, profit, rend_data->window_w);
	}else if(items_to_sell_num){
		RenderTextCentered(rend_data, 0.0F, height * (SHOP_ROWS - 1.0F), height, (Uint8[]){O,K}, 2U, rend_data->window_w);
	}
	for(unsigned int i = 0U; i < SHOP_SCROLLS_NUM; ++i){
		if(*(sh->scrolls + i) < scroll_empty){
			SDL_FRect src_rect = GetScrollTextureSrcRect(*(sh->scrolls + i));
			RenderShopElement(rend_data, width * (0.5F + SHOP_COLS - SHOP_SCROLLS_COLS - 1.0F + (i % SHOP_SCROLLS_COLS)), height * (FIRST_SHOP_ROW + 1.0F + (i / SHOP_SCROLLS_COLS)), width, ItemPrice(*(sh->scrolls + i)), tx_icons, &src_rect);
		}
	}
	RenderShopElement(rend_data, width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS - 1.0F)), height * (FIRST_SHOP_ROW + 1.0F), width, ItemPrice(shop_item_mp10), tx_icon, NULL);
	RenderShopElement(rend_data, width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS - 1.0F)), height * (FIRST_SHOP_ROW + 2.0F), width, ItemPrice(shop_item_mp100), tx_icon, NULL);
	RenderShopElement(rend_data, width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS)), height * (FIRST_SHOP_ROW + 1.0F), width, ItemPrice(shop_item_key), tx_icon, NULL);
	RenderShopElement(rend_data, width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS + 1.0F)), height * (FIRST_SHOP_ROW + 1.0F), width, ItemPrice(shop_item_key_location), tx_icon, NULL);
	RenderShopElement(rend_data, width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS - 1.0F)), height * (FIRST_SHOP_ROW + 5.0F), width, ItemPrice(shop_item_armour), tx_icon, NULL);
	RenderShopElement(rend_data, width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS)), height * (FIRST_SHOP_ROW + 5.0F), width, ItemPrice(shop_item_dodge_speed), tx_icon, NULL);
	if(pc->help_data.menu_position < SHOP_POSITIONS){
		const unsigned int row = pc->help_data.menu_position / SHOP_COLS;
		if(row == SHOP_ROWS - 1U){
			if(profit || items_to_sell_num){
				SDL_RenderTexture(rend_data->renderer, texture(tx_menu_ptr), NULL, &(SDL_FRect){
					width * (0.5F + SHOP_SIDE_COLS + 1U),
					height * row,
					width * 3.0F,
					height
				});
			}
		}else{
			const SDL_FRect menu_ptr_rect = {
				width * (0.5F + (pc->help_data.menu_position % SHOP_COLS)),
				height * row,
				width,
				width
			};
			SDL_RenderTexture(rend_data->renderer, texture(tx_menu_ptr), NULL, &menu_ptr_rect);
		}
	}
	SDL_RenderPresent(rend_data->renderer);
}

void ToggleFullscreen(Render_data* const rend_data){
	const bool fullscreen = (SDL_GetWindowFlags(rend_data->window) & SDL_WINDOW_FULLSCREEN);
	if(fullscreen){
		SDL_SetWindowFullscreen(rend_data->window, false);
	}else{
		SDL_SetWindowFullscreen(rend_data->window, true);
	}
	SDL_SyncWindow(rend_data->window);
	ResetRenderData(rend_data);
}

extern inline SDL_FPoint GetMouseWorldPosition(const Game_data* const gd){
	const Render_data* const rend_data = gd->rend_data_ptr;
	const float distance = PLAYER_REND_Y - rend_data->mouse_y;
	return (SDL_FPoint){
		human(gd)->position.x + rend_data->sin_player_direction * distance,
		human(gd)->position.y - rend_data->cos_player_direction * distance
	};
}