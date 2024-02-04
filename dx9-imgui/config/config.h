#ifndef CONFIG_H
#define CONFIG_H


class Config {
public:
	void load();
	void save();

	struct {
		bool enabled = true;
		bool show_demo_window = true;
		bool show_watermark = true;
	} overlay;
	struct {
		bool enabled = true;
		int fov = 100;
		bool draw_fov = true;
	} aimbot;
};


inline Config config;

#endif
