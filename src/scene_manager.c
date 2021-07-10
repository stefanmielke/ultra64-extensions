#include "../include/scene_manager.h"

#include "../include/memory_alloc.h"

SceneManager *scene_manager_init(MemZone *global_memory_pool, MemZone *scene_memory_pool,
								 fnSMSceneChangeCallback change_scene_callback) {
	SceneManager *scene_manager = MEM_ALLOC(sizeof(SceneManager), global_memory_pool);

	scene_manager->change_scene_callback = change_scene_callback;
	scene_manager->current_scene_id = -1;
	scene_manager->scene_memory_pool = scene_memory_pool;

	return scene_manager;
}

void scene_manager_change_scene(SceneManager *scene_manager, short scene_id) {
	scene_manager->next_scene_id = scene_id;
}

void scene_manager_set_callbacks(SceneManager *scene_manager, fnSMCreateCallback create_callback,
								 fnSMTickCallback tick_callback,
								 fnSMDisplayCallback display_callback,
								 fnSMDestroyCallback destroy_callback) {
	scene_manager->scene_callbacks.create = create_callback;
	scene_manager->scene_callbacks.tick = tick_callback;
	scene_manager->scene_callbacks.display = display_callback;
	scene_manager->scene_callbacks.destroy = destroy_callback;
}

void scene_manager_tick(SceneManager *scene_manager) {
	// change scene if needed
	if (scene_manager->current_scene_id != scene_manager->next_scene_id) {
		if (scene_manager->current_scene_id >= 0) {
			if (scene_manager->scene_callbacks.destroy)
				scene_manager->scene_callbacks.destroy();
			if (scene_manager->scene_memory_pool)
				mem_zone_free_all(scene_manager->scene_memory_pool);
		}

		scene_manager->change_scene_callback(scene_manager->current_scene_id,
											 scene_manager->next_scene_id);

		scene_manager->current_scene_id = scene_manager->next_scene_id;

		if (scene_manager->scene_callbacks.create)
			scene_manager->scene_callbacks.create();
	}

	scene_manager->next_scene_id = scene_manager->scene_callbacks.tick();
}

void scene_manager_display(SceneManager *scene_manager) {
	scene_manager->scene_callbacks.display();
}

void scene_manager_destroy(SceneManager *scene_manager) {
#ifdef _NUSYS_
	free(scene_manager);
#endif
}
