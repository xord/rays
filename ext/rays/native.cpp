#include "defs.h"


void Init_rays ();
void Init_exception ();

void Init_point ();
void Init_bounds ();
void Init_color ();
void Init_color_space ();
void Init_matrix ();

void Init_painter ();
void Init_polyline ();
void Init_polygon_line ();
void Init_polygon ();
void Init_bitmap ();
void Init_image ();
void Init_font ();
void Init_shader ();
void Init_camera ();

void Init_noise ();


extern "C" void
#ifdef COCOAPODS
	Init_rays_native ()
#else
	Init_native ()
#endif
{
	RUCY_TRY

	Rucy::init();

	Init_rays();
	Init_exception();

	Init_point();
	Init_bounds();
	Init_color();
	Init_color_space();
	Init_matrix();

	Init_painter();
	Init_polyline();
	Init_polygon_line();
	Init_polygon();
	Init_bitmap();
	Init_image();
	Init_font();
	Init_shader();
	Init_camera();

	Init_noise();

	RUCY_CATCH
}
