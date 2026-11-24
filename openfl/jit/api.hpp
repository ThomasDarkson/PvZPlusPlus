#pragma once
int pvz_init(const char* dataDirectory);
int pvz_frame(double deltaSeconds);
int pvz_frame_width();
const unsigned char* pvz_pixels();
void* pvz_gpu_probe_device();
void* pvz_gpu_probe_frame(int frame);
void pvz_gpu_probe_end();
void* pvz_gpu_output_frame();
void pvz_gpu_output_end();
const char* pvz_error();
const char* pvz_status();
void pvz_mouse(int kind, int x, int y, int button);
void pvz_key(int kind, int key, int character);
void pvz_focus(bool focused);
void pvz_shutdown();
const char* pvz_import_error();
const char* pvz_scene();

int pvz_menu_ready();
int pvz_update_warning();
int pvz_all_unlocked();
int pvz_menu_action(int action);

int pvz_window_size();
void pvz_set_window_size(int index);

int pvz_sharpness();
void pvz_set_sharpness(int percent);

int pvz_fps_limit();
void pvz_set_fps_limit(int limit);
void pvz_host_cycle_fps_limit();

int pvz_display_flags();
void pvz_set_display_flags(int flags);
void pvz_host_set_fullscreen(bool enabled);
void pvz_host_set_vsync(bool enabled);


