# Ultra64 Extensions
List of structs and functions to help on Ultra64 (Nintendo 64 SDK) development.

This library is a port of [Libdragon-Extensions](https://github.com/stefanmielke/libdragon-extensions.git) that aims to target compatibility with the N64 standard (or modern) SDK. I am porting functions as needed, so do not expect full parity with it.

## Installation

You can either [download the code from GitHub](https://github.com/stefanmielke/ultra64-extensions/archive/refs/heads/main.zip) into your project, or [clone the repo](https://github.com/stefanmielke/ultra64-extensions.git) inside your project as a [git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules) (so you can use `git pull` to get the latest changes when needed).

If you are using NuSystem, you have to add this definition to your `LCDEFS` variable:
```
-D_NUSYS_
```
or add this line:
```
LCDEFS += -D_NUSYS_
```

## Extensions Available

### Position
> position.h

```c
typedef struct {
	float x;
	float y;
} Position;
```

### PositionInt
> position_int.h

```c
typedef struct {
	int x;
	int y;
} PositionInt;
```

### Size
>size.h

```c
typedef struct {
	float width;
	float height;
} Size;
```

### Rect
> rect.h

```c
typedef struct {
	Position pos;
	Size size;
} Rect;

bool is_intersecting(Rect a, Rect b);
bool contains(Rect inner, Rect outer);
```

### Random
> random.h

Set of functions to simplify random numbers in a range.

```c
// generates an int from 0 to 10 (inclusive)
int n1 = RANDN(10);
// generates an int from 10 to 20 (inclusive)
int n2 = RANDR(10, 20);
// generates a float from 0 to 10 (inclusive)
float f1 = RANDNF(10);
// generates an int from 10 to 20 (inclusive)
float f2 = RANDRF(10, 20)
```

### Range
> range.h

Struct that contains a range and has functions to get random values from it.

```c
// generates a range from 0 to 10
RangeFloat r1 = { 0, 10 };
RangeInt r2 = { 0, 10 };

// gets a random value from 0 to 10
float value = range_get_from_float(&r1);

// gets a random value from 0 to 10
int value = range_get_from_int(&r2);
```

### Memory Pool
> mem_pool.h | mem_pool.c

Simple memory pool to allocate/free memory during gameplay without causing malloc (slower) calls.

```c
MemZone memory_pool;

// real memory that will be used by the memory pool
char mem_pool[1*1024*1024];

// allocating 1KB to be used
mem_zone_init(&memory_pool, &mem_pool, 1 * 1024);

// gets one int from the pool
int value = mem_zone_alloc(&memory_pool, sizeof(int));
value = 1;

// resets pool to the beggining
mem_zone_free_all(&memory_pool);

// gets another int, but on the same memory space as before
// if no value is set it will be the same set previously (on this example, '1')
int value2 = mem_zone_alloc(&memory_pool, sizeof(int));
```

### Scene Manager

You can use this to manage the transition across different scenes (aka levels).

This allows you to create different files that can represent your scenes and just create callbacks that will be used by the Scene Manager.

```c
// callbacks for a scene
void main_screen_create();
short main_screen_tick();
void main_screen_display();
void main_screen_destroy()

// callback for changing scenes
enum screens {
	SCREEN_MAIN
};
void change_screen(short curr_screen, short next_screen) {
	switch (next_screen) {
		case SCREEN_MAIN:
			scene_manager_set_callbacks(scene_manager, &main_screen_create, &main_screen_tick,
										&main_screen_display, &main_screen_destroy);
			break;
		default:
			abort();
	}
}

// 3 ways to initialize scene manager (options 2 and 3 are currently only available if using NuSystem)

// 1. I have a global and scene memory pools
SceneManager *scene_manager = scene_manager_init(&global_memory_pool, &memory_pool, &change_screen);
// 2. I use malloc or global vars for globals
SceneManager *scene_manager = scene_manager_init(NULL, &memory_pool, &change_screen);
// 3. I want to manage my memory pools myself
SceneManager *scene_manager = scene_manager_init(NULL, NULL, &change_screen);

// set up first screen after initializing
scene_manager_change_scene(scene_manager, SCREEN_MAIN);

// call tick every frame after controller_scan
scene_manager_tick(scene_manager);

// call every tick to draw to the screen after display_lock
scene_manager_display(scene_manager);

// you can call this method at any time during screen tick to set up screen change
// it will only really change the scene at the beggining of the next tick
scene_manager_change_scene(scene_manager, SCREEN_PLAY);

// you can check if the game will change on the next tick using this check after `scene_manager_tick`
if (scene_manager->current_scene_id != scene_manager->next_scene_id) {
	// Screen will change! Draw loading screen instead of calling `scene_manager_display`?
}

// destroy the scene manager if not using a global memory pool
scene_manager_destroy(scene_manager);
```

### Tweening

We support [Tweening](https://en.wikipedia.org/wiki/Inbetweening) and easing through a few functions. Easing functions were based on [MonoGame.Extended Methods](https://www.monogameextended.net/docs/features/tweening/tweening#in-easing-functions).

```c
// example struct
typedef struct {
	int x;
} Player;
Player *my_player;
// callback for value changes
void tween_player_x(void *target_object, float current_value) {
	Player *player = (Player *)target_object;
	player->x = current_value;
}
void tween_player_size(void *target_object, Size current_value);
void tween_player_position(void *target_object, Position current_value);
void tween_player_color(void *target_object, uint32_t current_value);
// example callback for when the tween finishes
void tween_finished_player_x(void *target_object);

Tween *tween;

// initialize the tween with a memory pool
tween = tween_init(&memory_pool);
// initialize without a memory pool (only available when using NuSystem, uses malloc)
tween = tween_init(NULL);

// starting the tween, examples
uint64_t duration_in_ms = 2000; // 2 seconds

// 1. No ending callback, no reverse, no always repeat
tween_start(tween, (void *)my_player, &easing_bounce_in_out, duration_in_ms, NULL, false, false);
// 2. Ending callback, no reverse, no always repeat
tween_start(tween, (void *)my_player, &easing_bounce_in_out, duration_in_ms, &tween_finished_player_x, false, false);
// 3. Ending callback, auto reverse, no always repeat
tween_start(tween, (void *)my_player, &easing_bounce_in_out, duration_in_ms, &tween_finished_player_x, true, false);
// 4. No ending callback, no auto reverse, always repeat (if always repeat, it will never call the ending callback)
tween_start(tween, (void *)my_player, &easing_bounce_in_out, duration_in_ms, NULL, false, true);
// 4. No ending callback, auto reverse, always repeat (if always repeat, it will never call the ending callback)
tween_start(tween, (void *)my_player, &easing_bounce_in_out, duration_in_ms, NULL, true, true);

// set the tween to the desired type
// float tween 
float start_x = 50, end_x = 200;
tween_set_to_float(tween, start_x, end_x, &tween_player_x);
// size tween
Size start_size = new_size_same(1), end_size = new_size_same(4);
tween_set_to_size(tween, start_size, end_size, &tween_player_size);
// position tween
Position start_position = new_position(10, 100), end_position = new_position(10, 300);
tween_set_to_position(tween, start_position, end_position, &tween_player_position);
// color tween (example uses new_color() from 'color.h')
color_t start_color = new_color(10, 100), end_color = new_color(10, 300);
tween_set_to_color(tween, start_color, end_color, &tween_player_color);

// changing duration of the tween to 1 second. use this only when reaching the end of the tween, or it may cause jitter.
tween_change_duration(tween, 1000);

// calling tick every frame. It will call 'tween_player_x' with the new value, and 'tween_finished_player_x' when it finishes
tween_tick(tween);

// destroying the tween (unnecessary but safe when using a memory pool)
tween_destroy(tween);

// Re-using Tweens:

// you can call tween_restart(...) instead of start.
tween_restart(tween, (void *)my_player, &easing_bounce_in_out, duration_in_ms, NULL, true, true);
// remember to call your "set" method again with the new values. after this it should be ready to be used again
float start_x = 50, end_x = 200;
tween_set_to_float(tween, start_x, end_x, &tween_player_x);
```

## Development

Make sure to use the included clang-format before pushing your code.

**Pull Requests are welcome!**
