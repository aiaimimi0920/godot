#include "gif_exporter.h"


void GifExporter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("begin_export", "file_path", "width", "height", "frame_delay", "loop_count", "bit_depth", "dither"), &GifExporter::begin_export, DEFVAL(Variant(0)), DEFVAL(Variant(8)), DEFVAL(Variant(false)));
	ClassDB::bind_method(D_METHOD("write_frame", "frame", "background_color", "frame_delay", "bit_depth", "dither"), &GifExporter::write_frame, DEFVAL(Variant(8)), DEFVAL(Variant(false)));
	ClassDB::bind_method(D_METHOD("end_export"), &GifExporter::end_export);
}

bool GifExporter::begin_export(const String file_path, int width, int height, float frame_delay, int loop_count, int32_t bit_depth, bool dither) {
	String abs_path = ProjectSettings::get_singleton()->globalize_path(file_path);
	// opens a new gif file
	return ganim.GifBegin(&gwriter, abs_path.utf8().get_data(), width, height, frame_delay, loop_count, bit_depth, dither);
}

bool GifExporter::end_export() {
	return ganim.GifEnd(&gwriter);
}

bool GifExporter::write_frame(const Ref<Image> frame, Color background_color, float frame_delay, int32_t bit_depth, bool dither) {
	// get raw bytes from frame
	PackedByteArray pool = frame->get_data();

	uint8_t *data = new uint8_t[pool.size()];
	for (size_t i = 0; i < pool.size(); i += 4) {
		// blend color with the background color because gif doesn't support alpha channel
		uint8_t red = pool[i];
		uint8_t green = pool[i + 1];
		uint8_t blue = pool[i + 2];
		uint8_t alpha = pool[i + 3];

		// background always has to have a solid alpha
		data[i + 3] = alpha + 255 * (255 - alpha);
		data[i] = (red * alpha + background_color.get_r8() * 255 * (255 - alpha)) / data[i + 3];
		data[i + 1] = (green * alpha + background_color.get_g8() * 255 * (255 - alpha)) / data[i + 3];
		data[i + 2] = (blue * alpha + background_color.get_b8() * 255 * (255 - alpha)) / data[i + 3];
	}
	bool _result = ganim.GifWriteFrame(&gwriter, data, frame->get_width(), frame->get_height(), frame_delay, bit_depth, dither);
	delete data;
	data = nullptr;

	return _result;
}

GifExporter::GifExporter() {}

GifExporter::~GifExporter() {
	end_export();
}
