/*
 * Copyright (c) 2006-2007 Ed Schouten <ed@fxq.nl>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/**
 * @file gui_draw.c
 */

#include "config.h"
#include "gui_internal.h"

GMutex *gui_lock;
int gui_draw_colors;

void
gui_draw_init_pre(void)
{
	/* We need the termcap before chroot() */
	initscr();
}

void
gui_draw_init_post(void)
{
	/* Lock playq from main thread */
	gui_lock = g_mutex_new();

#ifdef PDCURSES
	PDC_set_title(APP_NAME);
#endif /* PDCURSES */
	nonl();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	raw();
	refresh();

	gui_draw_colors = config_getopt_bool("gui.color.enabled");
	if (!has_colors())
		gui_draw_colors = 0;

	if (gui_draw_colors) {
		start_color();

		init_pair(GUI_COLOR_BAR,
		    config_getopt_color("gui.color.bar.fg"),
		    config_getopt_color("gui.color.bar.bg"));
		init_pair(GUI_COLOR_BLOCK,
		    config_getopt_color("gui.color.block.fg"),
		    config_getopt_color("gui.color.block.bg"));
		init_pair(GUI_COLOR_SELECT,
		    config_getopt_color("gui.color.select.fg"),
		    config_getopt_color("gui.color.select.bg"));
		init_pair(GUI_COLOR_DESELECT,
		    config_getopt_color("gui.color.deselect.fg"),
		    config_getopt_color("gui.color.deselect.bg"));
	}

	gui_msgbar_init();
	gui_playq_init();
	gui_browser_init();
}

void
gui_draw_init_abort(void)
{
	endwin();
}

void
gui_draw_destroy(void)
{
	GUI_LOCK;
	gui_msgbar_destroy();
	gui_playq_destroy();
	gui_browser_destroy();

	endwin();
	GUI_UNLOCK;
}

void
gui_draw_resize(void)
{
	GUI_LOCK;
	refresh();
	GUI_UNLOCK;
	gui_msgbar_resize();
	gui_playq_resize();
	gui_browser_resize();
	gui_draw_async_done();
}
