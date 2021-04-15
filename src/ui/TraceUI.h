//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "../fileio/bitmap.h"


#include "TraceGLWindow.h"

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;
	Fl_Slider*			m_adaptiveTerminationSlider;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	Fl_Light_Button*	m_backgroundButton;
	Fl_Light_Button*	m_textureMappingButton;
	Fl_Light_Button*	m_bumppingButton;
	Fl_Light_Button*	m_superSampleButton;
	Fl_Light_Button*	m_adaptiveSampleButton;
	Fl_Light_Button*	m_jitterButton;
	Fl_Slider*			m_superSampleSlider;

	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	void		setRayTracer(RayTracer *tracer);

	int			getSize();
	int			getDepth();
	
	void		setAdaptiveTermination(float tresh){ this->adaptiveTermination = tresh; this->m_adaptiveTerminationSlider->value(tresh); }
	float		getAdaptiveTermination() { return this->adaptiveTermination; }
	


private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
	float		adaptiveTermination=0.0;
	

// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);
	static void cb_load_background(Fl_Menu_* o, void* v);
	static void cb_load_textureMap(Fl_Menu_* o, void* v);
	static void cb_load_bumpMap(Fl_Menu_* o, void* v);
	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_adaptiveTerminationSlides(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);

	static void cb_backgroundButton(Fl_Widget* o, void* v);
	static void cb_textureMappingButton(Fl_Widget* o, void* v);
	static void cb_bumppingButton(Fl_Widget* o, void* v);
	static void cb_superSampleButton(Fl_Widget* o, void* v);
	static void cb_adaptiveSampleButton(Fl_Widget* o, void* v);
	static void cb_jitterButton(Fl_Widget* o, void* v);
	static void cb_superSampleSlides(Fl_Widget* o, void* v);
};

#endif
