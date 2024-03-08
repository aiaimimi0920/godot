#ifndef GIF_EXPORTER_H
#define GIF_EXPORTER_H

#include "core/io/image.h"
#include "core/config/project_settings.h"
#include "core/object/ref_counted.h"
#include "gifanim/gifanim.h"


class GifExporter : public RefCounted {
	GDCLASS(GifExporter, RefCounted);

	GifAnim ganim;
	GifWriter gwriter;

protected:
	static void _bind_methods();

public:
	bool begin_export(const String file_path, int width, int height, float frame_delay, int loop_count = 0, int32_t bit_depth = 8, bool dither = false);

	bool end_export();

	bool write_frame(const Ref<Image> frame, Color background_color, float frame_delay, int32_t bit_depth = 8, bool dither = false);

	GifExporter();
	~GifExporter();
};

#endif // GIF_EXPORTER_H
