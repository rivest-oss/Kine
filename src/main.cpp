/* Kine: A very simple "AI" toy.
** Copyright (C) 2026  Rivest Osz
** This program is free software: you can redistribute it and/or modify
** it under the terms of the Creative Commons "0" License.
**/

#include <cmath>

namespace Raylib {
	#include <raylib.h>
};

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

float rand_f32(void) {
	return Raylib::GetRandomValue(0, 65535) / 65536.f;
};

Raylib::Image map;
Raylib::Texture2D map_tex;

typedef struct worm_t {
	Raylib::Vector2 position;
	float rotation;
	float speed;
	float comfort;

	const float min_speed = 0.05f;
	const float max_speed = 0.5f;

	float mod_rotation(float r) {
		while(r < 0.f) r += 6.28318530f;
		while(r >= 6.28318530f) r -= 6.28318530f;
		return r;
	};

	float get_random_rotation(void) {
		float r = rand_f32() * 6.28318529f;
		return r;
	};

	void apply_movement(void) {
		float dx = std::cos(rotation) * speed;
		float dy = std::sin(rotation) * speed;

		position.x += dx;
		position.y += dy;

		if(position.x < 0.f) position.x = 0.f;
		if(position.y < 0.f) position.y = 0.f;
		if(position.x >= 1.f) position.x = 0.99999f;
		if(position.y >= 1.f) position.y = 0.99999f;
	};

	void begin(void) {
		position.x = (float)Raylib::GetRandomValue(0, map.width - 1) / map.width;
		position.y = (float)Raylib::GetRandomValue(0, map.height - 1) / map.height;
		rotation = get_random_rotation();
		speed = 0.001f;
	};

	// actual "AI".
	void update(double dt) {
		(void)dt;

		int ix = map.width * position.x;
		int iy = map.height * position.y;

		Raylib::Color food_color = Raylib::GetImageColor(map, ix, iy);

		int food_i = food_color.r;
		float food = food_i / 256.f;

		// Update worm's level of comfort.
		comfort = food;

		// Calculate how the comfort-ness affects the speed.
		float speed_weight;
		if(food >= 0.5f) speed_weight = 0.f;
		else speed_weight = (food - 0.5f) / 0.5f;

		float new_speed = (min_speed + (max_speed - min_speed) * speed_weight) * dt;

		// Apply new speed and rotation.
		speed = new_speed;
		rotation = get_random_rotation();

		apply_movement();

		// Eat food.
		if(food_i > 0) {
			food_i--;
			food_color.r = food_color.g = food_color.b = food_i;
			Raylib::ImageDrawPixel(&map, ix, iy, food_color);
		}
	};

	void draw(double dt) {
		(void)dt;

		float min_size;
		if(map.width > map.height)
			min_size = map.height;
		else
			min_size = map.width;

		float worm_radius = (1.f / 200.f) * min_size;

		Raylib::Color worm_color = Raylib::ColorLerp(Raylib::RED, Raylib::GREEN, comfort);

		Raylib::DrawCircle(position.x * map.width, position.y * map.height, worm_radius + 1, Raylib::BLACK);
		Raylib::DrawCircle(position.x * map.width, position.y * map.height, worm_radius, worm_color);
	};
} worm_t;

int map_mode = 0;
void generate_map(void);

worm_t worms[100];

int go_fast = 0;
int iterations;

void update(double dt) {
	int function_pressed = -1;
	if(Raylib::IsKeyPressed(Raylib::KEY_F1)) function_pressed = 1;
	else if(Raylib::IsKeyPressed(Raylib::KEY_F2)) function_pressed = 2;
	else if(Raylib::IsKeyPressed(Raylib::KEY_F3)) function_pressed = 3;
	else if(Raylib::IsKeyPressed(Raylib::KEY_F4)) function_pressed = 4;
	else if(Raylib::IsKeyPressed(Raylib::KEY_F5)) function_pressed = 5;
	else if(Raylib::IsKeyPressed(Raylib::KEY_F6)) function_pressed = 6;
	else if(Raylib::IsKeyPressed(Raylib::KEY_F7)) function_pressed = 7;
	else if(Raylib::IsKeyPressed(Raylib::KEY_F8)) function_pressed = 8;
	else if(Raylib::IsKeyPressed(Raylib::KEY_F9)) function_pressed = 9;
	else if(Raylib::IsKeyPressed(Raylib::KEY_F10)) function_pressed = 10;
	else if(Raylib::IsKeyPressed(Raylib::KEY_F11)) function_pressed = 11;
	else if(Raylib::IsKeyPressed(Raylib::KEY_F12)) function_pressed = 12;

	bool dropped_something = Raylib::IsFileDropped();

	if(dropped_something) {
		Raylib::FilePathList dropped_files = Raylib::LoadDroppedFiles();

		for(unsigned int i = 0; i < dropped_files.count; i++) {
			Raylib::Image img0 = Raylib::LoadImage(dropped_files.paths[i]);

			if(Raylib::IsImageValid(img0) == false)
				continue;

			int sw = Raylib::GetScreenWidth();
			int sh = Raylib::GetScreenHeight();

			float ratio = (float)img0.width / img0.height;
			sw = ratio * sh;

			Raylib::ImageColorGrayscale(&img0);

			Raylib::Image img1 = Raylib::GenImageColor(sw, sh, Raylib::WHITE);
			Raylib::ImageDraw(&img1, img0, Raylib::Rectangle {
				0.f,
				0.f,
				(float)img0.width,
				(float)img0.height,
			}, Raylib::Rectangle {
				0.f,
				0.f,
				(float)img1.width,
				(float)img1.height,
			}, Raylib::WHITE);

			Raylib::UnloadImage(map);
			Raylib::UnloadImage(img0);
			map = img1;

			Raylib::SetWindowSize(sw, sh);
		};

		Raylib::UnloadDroppedFiles(dropped_files);
	} else {
		if(function_pressed > 0) {
			map_mode = function_pressed - 1;
			Raylib::UnloadImage(map);
			generate_map();
		}
	
		if(Raylib::IsKeyPressed(Raylib::KEY_I))
			Raylib::ImageColorInvert(&map);
	
		if(Raylib::IsKeyPressed(Raylib::KEY_S))
			go_fast++;
		if(Raylib::IsKeyPressed(Raylib::KEY_A))
			go_fast--;
	}

	switch(go_fast) {
		case 1: iterations = 2; break;
		case 2: iterations = 5; break;
		case 3: iterations = 10; break;
		case 4: iterations = 50; break;
		case 5: iterations = 100; break;
		case 6: iterations = 1000; break;
		default:
			go_fast = 0;
			iterations = 1;
			break;
	};

	for(int i = 0; i < (int)(sizeof(worms) / sizeof(worm_t)); i++)
		for(int j = 0; j < iterations; j++)
			worms[i].update(dt);
};

void draw(double dt) {
	map_tex = Raylib::LoadTextureFromImage(map);
	Raylib::DrawTexture(map_tex, 0, 0, Raylib::WHITE);

	for(int i = 0; i < (int)(sizeof(worms) / sizeof(worm_t)); i++)
		worms[i].draw(dt);

	if(iterations > 1) {
		Raylib::DrawText(Raylib::TextFormat("x%d", iterations), 16, 16, 32, Raylib::RED);
	}
};

void update_and_draw(void) {
	Raylib::BeginDrawing();
	Raylib::ClearBackground(Raylib::BLACK);
	
	double dt = Raylib::GetFrameTime();
	update(dt);
	draw(dt);
	
	Raylib::EndDrawing();
	Raylib::SwapScreenBuffer();

	Raylib::UnloadTexture(map_tex);
};

void generate_map(void) {
	// 1. Perlin noise.
	if(map_mode == 1) {
		map = Raylib::GenImagePerlinNoise(Raylib::GetScreenWidth(), Raylib::GetScreenHeight(), 1.f, 1.f, 2.f);
		return;
	}

	// 2. Gradient linear.
	if(map_mode == 2) {
		map = Raylib::GenImageGradientLinear(Raylib::GetScreenWidth(), Raylib::GetScreenHeight(), 0, Raylib::BLACK, Raylib::WHITE);
		return;
	}

	// 3. Gradient radial.
	if(map_mode == 3) {
		map = Raylib::GenImageGradientRadial(Raylib::GetScreenWidth(), Raylib::GetScreenHeight(), 1.f, Raylib::BLACK, Raylib::WHITE);
		return;
	}

	// 4. Gradient radial.
	if(map_mode == 4) {
		map = Raylib::GenImageCellular(Raylib::GetScreenWidth(), Raylib::GetScreenHeight(), 100);
		return;
	}

	// Default (split half).
	map = Raylib::GenImageColor(Raylib::GetScreenWidth(), Raylib::GetScreenHeight(), Raylib::BLACK);
	Raylib::ImageDrawRectangle(&map, 0, 0, map.width / 2, map.height, Raylib::WHITE);
	return;
};

int main(void) {
	Raylib::SetConfigFlags(Raylib::FLAG_WINDOW_RESIZABLE);
	Raylib::SetTargetFPS(50);
	Raylib::SetExitKey(Raylib::KEY_NULL);

	Raylib::InitWindow(640, 360, "kine");
	while(Raylib::IsWindowReady() == false);

	Raylib::InitAudioDevice();
	while(Raylib::IsAudioDeviceReady() == false) {};

	int mon_index = Raylib::GetCurrentMonitor();
	int mon_refresh = Raylib::GetMonitorRefreshRate(mon_index);
	int mon_w = Raylib::GetMonitorWidth(mon_index);
	int mon_h = Raylib::GetMonitorHeight(mon_index);

	Raylib::SetTargetFPS(mon_refresh);

	int sw = mon_w;
	int sh = mon_h;

	if(sw < 16) sw = 16;
	if(sh < 16) sh = 16;
	if(sw > 32'768) sw = 32'768;
	if(sh > 32'768) sh = 32'768;
	sw *= 0.9;
	sh *= 0.9;

	Raylib::SetWindowSize(sw, sh);
	Raylib::SetWindowPosition((mon_w - sw) / 2, (mon_h - sh) / 2);

	go_fast = 0;
	iterations = 1;
	map_mode = 1;
	generate_map();

	map_tex = Raylib::LoadTextureFromImage(map);

	for(int i = 0; i < (int)(sizeof(worms) / sizeof(worm_t)); i++) {
		worms[i].begin();
	};
	
	#ifdef PLATFORM_WEB
		emscripten_set_main_loop(update_and_draw, 0, 1);
	#else
		while(Raylib::WindowShouldClose() == false) {
			update_and_draw();
		};
	#endif
	
	Raylib::CloseAudioDevice();
	Raylib::CloseWindow();
	
	return 0;
};

