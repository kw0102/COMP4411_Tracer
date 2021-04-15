//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"
#include "../fileio/bitmap.h"

using namespace std;
static bool done;


//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (TraceUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			done=true;	
			
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);

	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v) 
{
	TraceUI* pUI=(TraceUI *)(o->user_data());
	
	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}


void TraceUI::cb_loadHeightField(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);

	if (!pUI->raytracer->sceneLoaded())
	{
		fl_message("Please load the blank.ray scene first.");
		return;
	}

	char* newfile = fl_file_chooser("Load Height Field?", "*.bmp", NULL);

	if (newfile != NULL) {
		int width, height;
		char greyfile[256];
		strncpy(greyfile, newfile, strlen(newfile) - 4);
		strcat(greyfile, "grey_.bmp\0");
		unsigned char* bitmap = readBMP(newfile, width, height);
		unsigned char* greymap = readBMP(greyfile, width, height);
		if (greymap == NULL)
		{
			fl_message("Corresponding grey map not found.");
			return;
		}
		pUI->raytracer->loadHeightField(greymap, bitmap, width, height);
		delete[] bitmap;
  }
}

void TraceUI::cb_load_background(Fl_Menu_* o, void* v) {
	
	TraceUI* pUI = whoami(o);

	char* newfile = fl_file_chooser("Load Background?", "*.bmp", NULL);

	if (newfile != NULL) 
	{
		pUI->raytracer->loadBackground(newfile);
		
	}
	
}

void TraceUI::cb_load_textureMap(Fl_Menu_* o, void* v) {

	TraceUI* pUI = whoami(o);

	char* newfile = fl_file_chooser("Load Texture?", "*.bmp", NULL);

	if (newfile != NULL)
	{
		pUI->raytracer->loadTexture(newfile);
	}
}

void TraceUI::cb_load_bumpMap(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);
	char* newfile = fl_file_chooser("Load Bump?", "*.bmp", NULL);
	if (newfile != NULL) {
		pUI->raytracer->loadBumpping(newfile);

	}
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=(TraceUI*)(o->user_data());
	
	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
	((TraceUI*)(o->user_data()))->raytracer->maxDepth = int(((Fl_Slider*)o)->value());
}

void TraceUI::cb_adaptiveTerminationSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->adaptiveTermination = double(((Fl_Slider*)o)->value());
	cout << pUI->adaptiveTermination << endl;

	
}

void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI=((TraceUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done=false;
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (done) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				pUI->raytracer->tracePixel( x, y );
		
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}
		done=true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done=true;
}

void TraceUI::cb_backgroundButton(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	RayTracer* rt = pUI->raytracer;
	rt->useBackground = !rt->useBackground;
	
}

void TraceUI::cb_textureMappingButton(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	RayTracer* rt = pUI->raytracer;
	rt->useTexture = !rt->useTexture;
	if (rt->useTexture)
		printf("true");
	else
		printf("false");
}

void TraceUI::cb_bumppingButton(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	RayTracer* rt = pUI->raytracer;
	rt->useBump = !rt->useBump;

}

void TraceUI::cb_superSampleButton(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	RayTracer* rt = pUI->raytracer;
	rt->useSuperSampling = !rt->useSuperSampling;
	if (rt->useSuperSampling) {
		pUI->m_adaptiveSampleButton->deactivate();
		pUI->m_adaptiveSampleButton->value(0);
		rt->useAdaptiveSampling = false;
	}
	else 
	{
		pUI->m_adaptiveSampleButton->activate();
	}
}

void TraceUI::cb_adaptiveSampleButton(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	RayTracer* rt = pUI->raytracer;
	rt->useAdaptiveSampling = !rt->useAdaptiveSampling;
	if (rt->useAdaptiveSampling) {
		pUI->m_superSampleButton->deactivate();
		pUI->m_superSampleButton->value(0);
		rt->useSuperSampling = false;
	}
	else
	{
		pUI->m_superSampleButton->activate();
	}
}

void TraceUI::cb_jitterButton(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	RayTracer* rt = pUI->raytracer;
	rt->useJitter = !rt->useJitter;
}

void TraceUI::cb_superSampleSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	RayTracer* rt = pUI->raytracer;
	rt->numSuperSample = int(((Fl_Slider*)o)->value());
}

void TraceUI::cb_depthOfFieldButton(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	RayTracer* rt = pUI->raytracer;
	rt->depthOfField = !rt->depthOfField;
}

void TraceUI::cb_softShadowButton(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	RayTracer* rt = pUI->raytracer;
	rt->softShadow = !rt->softShadow;
}

void TraceUI::cb_glossyReflectionButton(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	RayTracer* rt = pUI->raytracer;
	rt->glossyReflection = !rt->glossyReflection;
}

void TraceUI::cb_motionBlurButton(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	RayTracer* rt = pUI->raytracer;
	rt->motionBlur = !rt->motionBlur;
}

void TraceUI::cb_focalLengthSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	RayTracer* rt = pUI->raytracer;
	rt->focalLength = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_apertureSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	RayTracer* rt = pUI->raytracer;
	rt->aperture = double(((Fl_Slider*)o)->value());
}

void TraceUI::show()
{
	m_mainWindow->show();
}

void TraceUI::setRayTracer(RayTracer *tracer)
{
	raytracer = tracer;
	m_traceGlWindow->setRayTracer(tracer);
}

int TraceUI::getSize()
{
	return m_nSize;
}

int TraceUI::getDepth()
{
	return m_nDepth;
}



// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{ "Load HField Map ",		   0, (Fl_Callback *)TraceUI::cb_loadHeightField },
		{ "&Load Background...", FL_ALT + 'b', (Fl_Callback*)TraceUI::cb_load_background },
		{ "&Load Texture...", FL_ALT + 't', (Fl_Callback*)TraceUI::cb_load_textureMap },
		{ "&Load Bump...", FL_ALT + 't', (Fl_Callback*)TraceUI::cb_load_bumpMap },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 0;
	m_nSize = 150;
	m_mainWindow = new Fl_Window(100, 40, 320, 300, "Ray <Not Loaded>");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 320, 25);
		m_menubar->menu(menuitems);

		// install slider depth
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(0);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install slider size
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(512);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);

		//install Adaptive Termination Slider 
		m_adaptiveTerminationSlider = new Fl_Value_Slider(10, 130, 180, 20, "Threashold");
		m_adaptiveTerminationSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_adaptiveTerminationSlider->type(FL_HOR_NICE_SLIDER);
		m_adaptiveTerminationSlider->labelfont(FL_COURIER);
		m_adaptiveTerminationSlider->labelsize(12);
		m_adaptiveTerminationSlider->minimum(0);
		m_adaptiveTerminationSlider->maximum(1);
		m_adaptiveTerminationSlider->step(0.01);
		m_adaptiveTerminationSlider->value(adaptiveTermination);
		m_adaptiveTerminationSlider->align(FL_ALIGN_RIGHT);
		m_adaptiveTerminationSlider->callback(cb_adaptiveTerminationSlides);


		// button 

		m_renderButton = new Fl_Button(240, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(240, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		m_superSampleButton = new Fl_Light_Button(10, 80, 100, 25, "Super Samp.");
		m_superSampleButton->user_data((void*)(this));
		m_superSampleButton->callback(cb_superSampleButton);

		m_adaptiveSampleButton = new Fl_Light_Button(110, 80, 130, 25, "Adaptive Samp.");
		m_adaptiveSampleButton->user_data((void*)(this));
		m_adaptiveSampleButton->callback(cb_adaptiveSampleButton);

		m_jitterButton = new Fl_Light_Button(240, 80, 70, 25, "Jitter");
		m_jitterButton->user_data((void*)(this));
		m_jitterButton->callback(cb_jitterButton);

		m_backgroundButton = new Fl_Light_Button(10, 150, 120, 25, "Background");
		m_backgroundButton->user_data((void*)(this));
		m_backgroundButton->callback(cb_backgroundButton);

		m_textureMappingButton = new Fl_Light_Button(130, 150, 70, 25, "Texture");
		m_textureMappingButton->user_data((void*)(this));
		m_textureMappingButton->callback(cb_textureMappingButton);

		m_textureMappingButton = new Fl_Light_Button(200, 150, 60, 25, "Bump");
		m_textureMappingButton->user_data((void*)(this));
		m_textureMappingButton->callback(cb_bumppingButton);

		m_superSampleSlider = new Fl_Value_Slider(10, 110, 180, 20, "Num of sub-pixel");
		m_superSampleSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_superSampleSlider->type(FL_HOR_NICE_SLIDER);
		m_superSampleSlider->labelfont(FL_COURIER);
		m_superSampleSlider->labelsize(12);
		m_superSampleSlider->minimum(1);
		m_superSampleSlider->maximum(5);
		m_superSampleSlider->step(1);
		m_superSampleSlider->value(1);
		m_superSampleSlider->align(FL_ALIGN_RIGHT);
		m_superSampleSlider->callback(cb_superSampleSlides);

		m_depthOfFieldButton = new Fl_Light_Button(10, 180, 60, 25, "DOF");
		m_depthOfFieldButton->user_data((void*)(this));
		m_depthOfFieldButton->callback(cb_depthOfFieldButton);

		m_softShadowButton = new Fl_Light_Button(70, 180, 100, 25, "Soft Shadow");
		m_softShadowButton->user_data((void*)(this));
		m_softShadowButton->callback(cb_softShadowButton);

		m_glossyReflectionButton = new Fl_Light_Button(170, 180, 70, 25, "Glossy");
		m_glossyReflectionButton->user_data((void*)(this));
		m_glossyReflectionButton->callback(cb_glossyReflectionButton);

		m_motionBlurButton = new Fl_Light_Button(240, 180, 70, 25, "Motion");
		m_motionBlurButton->user_data((void*)(this));
		m_motionBlurButton->callback(cb_motionBlurButton);

		m_focalLengthSlider = new Fl_Value_Slider(10, 210, 180, 20, "Focal Length");
		m_focalLengthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_focalLengthSlider->type(FL_HOR_NICE_SLIDER);
		m_focalLengthSlider->labelfont(FL_COURIER);
		m_focalLengthSlider->labelsize(12);
		m_focalLengthSlider->minimum(1.0);
		m_focalLengthSlider->maximum(5.0);
		m_focalLengthSlider->step(0.01);
		m_focalLengthSlider->value(2.0);
		m_focalLengthSlider->align(FL_ALIGN_RIGHT);
		m_focalLengthSlider->callback(cb_focalLengthSlides);

		m_apertureSlider = new Fl_Value_Slider(10, 230, 180, 20, "Aperture Size");
		m_apertureSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_apertureSlider->type(FL_HOR_NICE_SLIDER);
		m_apertureSlider->labelfont(FL_COURIER);
		m_apertureSlider->labelsize(12);
		m_apertureSlider->minimum(1);
		m_apertureSlider->maximum(5);
		m_apertureSlider->step(1);
		m_apertureSlider->value(1);
		m_apertureSlider->align(FL_ALIGN_RIGHT);
		m_apertureSlider->callback(cb_apertureSlides);


		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
		m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}