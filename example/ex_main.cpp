/* SKALE - 2D SKeletal Animation Layer for Entities
 * Copyright (c) 2011 Joshua Larouche
 * 
 *
 * License: (BSD)
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of SKALE nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//Allegro 5 
#include <allegro5/allegro.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_opengl.h>

#include <stdlib.h>
#include <vector> 
#include <string>
#include <ctime>
#include <iostream>
#include <fstream>
#include "SKALE/Skeleton.hpp"
#include "SKALE/IKSolver.hpp"
#include <math.h>

skl::IKSolver solver;

#define FRAME_RATE 60

//Globals
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_EVENT event;
ALLEGRO_EVENT_QUEUE *queue = NULL;
ALLEGRO_BITMAP *rope;
bool done = false;
skl::Skeleton skeleton;
int mouseX;
int mouseY;
int startX;
int startY;


bool Inisde(float x,float y,float l,float t,float r,float b )
{
	return x > l && x < r && y > t && y < b;
}

skl::Bone* findBone(float x, float y, skl::Bone* root)
{
	float x1, y1, x2, y2;

	x1 = root->getFrameX();
	x2 = root->getFrameX();
	y1 = root->getFrameY();
	y2 = root->getFrameY();

	x1 -= 12.0f;
	y1 -= 12.0f;
	x2 += 12.0f;
	y2 += 12.0f;

	if(Inisde(x,y,x1,y1,x2,y2))
	{
		return root;
	}

	skl::Bone* result = NULL;
	for(std::list<skl::Bone>::iterator it = root->begin(); it != root->end(); ++it )
	{
		result = findBone(x,y,&(*it));
		if(result)
		{
			return result;
		}
	}

	return NULL;
}

void initializeAllegro() {
	//Initialize Allegro
	if(!al_init())
	{
		throw std::exception("Allegro failed to initialize");
	}

	if(!al_init_image_addon())
	{
		throw std::exception("Allegro image addon failed to initialize");
	}
	al_init_font_addon();

	if(!al_init_ttf_addon())
	{
		throw std::exception("Allegro ttf addon failed to initialize");
	}

	if(!al_init_primitives_addon())
	{
		throw std::exception("Allegro primitives addon failed to initialize");
	}
	if(!al_install_mouse())
	{
		throw std::exception("Allegro mouse failed to initialize");
	}
	if(!al_install_keyboard())
	{
		throw std::exception("Allegro keyboard failed to initialize");
	}

	// Start a timer to regulate speed
	timer = al_create_timer(1.0/FRAME_RATE);
	al_start_timer(timer);

	//show screen

	al_set_new_display_flags(ALLEGRO_RESIZABLE | ALLEGRO_OPENGL);

	display = al_create_display(640,480);

	if(!display)
	{
		done = true;
	}
	//show the mouse
	al_show_mouse_cursor(display); 



	//Window Title
	al_set_window_title(display,"SKALE - Example");

	queue = al_create_event_queue();
	rope = al_load_bitmap("rope.png");
}

void buildSkeleton()
{
	skeleton.load("Skeleton.txt");

}

void renderSkeleton(skl::Bone* root)
{
	if(root->getParent())
	{

		al_draw_scaled_rotated_bitmap(rope,al_get_bitmap_width(rope) / 2.0f,
			0,
			root->getParent()->getFrameX(), root->getParent()->getFrameY(),0.1f,(root->getLength() * 1.1f) /
			al_get_bitmap_height(rope) ,root->getFrameAngle() - (3.1415f / 2.0f),0);

		al_draw_line(
			root->getParent()->getFrameX(),root->getParent()->getFrameY(),
			root->getFrameX(),root->getFrameY(),al_map_rgb(255,0,0),1.0f);
		al_draw_filled_circle(root->getFrameX(),root->getFrameY(),4.0f,al_map_rgb(50,200,0));

	}
	else
	{
		al_draw_filled_circle(root->getFrameX(),root->getFrameY(),4.0f,al_map_rgb(50,200,0));
	}

	for(std::list<skl::Bone>::iterator it = root->begin(); it != root->end(); ++it)
	{
		renderSkeleton(&(*it));
	}
}

skl::Bone* boneUnderMouse = NULL;

void render()
{
	al_clear_to_color(al_map_rgb(240,240,240));
	skeleton.processAnimation();
	skeleton.updateBones();
	renderSkeleton(skeleton.getRoot());
	al_flip_display();
}

int main(int argc, char *argv[])
{
	initializeAllegro();
	buildSkeleton();

	bool needRedraw = true;
	// Start the event queue to handle keyboard input, mouse and our timer

	al_register_event_source(queue, (ALLEGRO_EVENT_SOURCE*)al_get_keyboard_event_source());
	al_register_event_source(queue, (ALLEGRO_EVENT_SOURCE*)al_get_mouse_event_source());
	al_register_event_source(queue, (ALLEGRO_EVENT_SOURCE*)timer);
	al_register_event_source(queue, (ALLEGRO_EVENT_SOURCE*)display);

	while(!done) {

		// Block until an event enters the queue
		al_wait_for_event(queue, &event);

		//Handle rendering and logic
		if (needRedraw && al_event_queue_is_empty(queue)) {
			render();
			needRedraw = false;
		}

		switch(event.type) {
	case ALLEGRO_EVENT_TIMER:
		if(event.timer.source == timer) {
			needRedraw = true;
		}
		break;
	case ALLEGRO_EVENT_DISPLAY_RESIZE:
		al_acknowledge_resize(event.display.source);
		break;
	case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		mouseX = event.mouse.x;
		mouseY = event.mouse.y;
		startX = mouseX;
		startY = mouseY;
		boneUnderMouse = findBone((float)mouseX,(float)mouseY,skeleton.getRoot());
		if(boneUnderMouse)
		{
			if(boneUnderMouse->getParent())
			{
				startX = boneUnderMouse->getParent()->getFrameX();
				startY = boneUnderMouse->getParent()->getFrameY();
			}
		}
		break;
	case ALLEGRO_EVENT_MOUSE_AXES:
		if(boneUnderMouse) {
			solver.solve(
				&skeleton,boneUnderMouse,(float)event.mouse.x,(float)event.mouse.y);
			skeleton.updateBones();
			//skeleton.save("Skeleton.txt");
		}
		break;
	case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
		boneUnderMouse = NULL;
		break;
	case ALLEGRO_EVENT_DISPLAY_CLOSE:
		return 0;
		break;
		}
	}

	return 0;
}
