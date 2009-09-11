/*	toolbar.c
	Copyright (C) 2006 Mark Tyler and Dmitry Groshev

	This file is part of mtPaint.

	mtPaint is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	mtPaint is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with mtPaint in the file COPYING.
*/

#include <gtk/gtk.h>


#include "global.h"

#include "memory.h"
#include "inifile.h"
#include "mainwindow.h"
#include "otherwindow.h"
#include "canvas.h"
#include "mygtk.h"
#include "toolbar.h"
#include "layer.h"
#include "viewer.h"
#include "png.h"
#include "channels.h"
#include "csel.h"


#include "graphics/xpm_paint.xpm"

#include "graphics/xbm_square.xbm"
#include "graphics/xbm_square_mask.xbm"

#include "graphics/xbm_circle.xbm"
#include "graphics/xbm_circle_mask.xbm"

#include "graphics/xbm_horizontal.xbm"
#include "graphics/xbm_horizontal_mask.xbm"

#include "graphics/xbm_vertical.xbm"
#include "graphics/xbm_vertical_mask.xbm"

#include "graphics/xbm_slash.xbm"
#include "graphics/xbm_slash_mask.xbm"

#include "graphics/xbm_backslash.xbm"
#include "graphics/xbm_backslash_mask.xbm"

#include "graphics/xbm_spray.xbm"
#include "graphics/xbm_spray_mask.xbm"

#include "graphics/xpm_shuffle.xpm"
#include "graphics/xbm_shuffle.xbm"
#include "graphics/xbm_shuffle_mask.xbm"

#include "graphics/xpm_flood.xpm"
#include "graphics/xbm_flood.xbm"
#include "graphics/xbm_flood_mask.xbm"

#include "graphics/xpm_line.xpm"
#include "graphics/xbm_line.xbm"
#include "graphics/xbm_line_mask.xbm"

#include "graphics/xpm_select.xpm"
#include "graphics/xbm_select.xbm"
#include "graphics/xbm_select_mask.xbm"

#include "graphics/xpm_smudge.xpm"
#include "graphics/xbm_smudge.xbm"
#include "graphics/xbm_smudge_mask.xbm"

#include "graphics/xpm_polygon.xpm"
#include "graphics/xbm_polygon.xbm"
#include "graphics/xbm_polygon_mask.xbm"

#include "graphics/xpm_clone.xpm"
#include "graphics/xbm_clone.xbm"
#include "graphics/xbm_clone_mask.xbm"

#include "graphics/xbm_move.xbm"
#include "graphics/xbm_move_mask.xbm"

#include "graphics/xpm_brcosa.xpm"
#include "graphics/xpm_flip_vs.xpm"
#include "graphics/xpm_flip_hs.xpm"
#include "graphics/xpm_rotate_cs.xpm"
#include "graphics/xpm_rotate_as.xpm"
#include "graphics/xpm_text.xpm"
#include "graphics/xpm_lasso.xpm"

#include "graphics/xpm_ellipse.xpm"
#include "graphics/xpm_ellipse2.xpm"
#include "graphics/xpm_rect1.xpm"
#include "graphics/xpm_rect2.xpm"
#include "graphics/xpm_pan.xpm"

#include "graphics/xpm_new.xpm"
#include "graphics/xpm_open.xpm"
#include "graphics/xpm_save.xpm"
#include "graphics/xpm_cut.xpm"
#include "graphics/xpm_copy.xpm"
#include "graphics/xpm_paste.xpm"
#include "graphics/xpm_undo.xpm"
#include "graphics/xpm_redo.xpm"

#include "graphics/xpm_up.xpm"
#include "graphics/xpm_down.xpm"
#include "graphics/xpm_centre.xpm"
#include "graphics/xpm_close.xpm"

#include "graphics/xpm_mode_cont.xpm"
#include "graphics/xpm_mode_opac.xpm"
#include "graphics/xpm_mode_tint.xpm"
#include "graphics/xpm_mode_tint2.xpm"
#include "graphics/xpm_mode_csel.xpm"
#include "graphics/xpm_mode_mask.xpm"


GtkWidget *icon_buttons[TOTAL_ICONS_TOOLS], *icon_buttons2[TOTAL_ICONS_MAIN];

gboolean toolbar_status[TOOLBAR_MAX];			// True=show
GtkWidget *toolbar_boxes[TOOLBAR_MAX]			// Used for showing/hiding
		= {NULL, NULL, NULL, NULL, NULL, NULL},
	*toolbar_menu_widgets[TOOLBAR_MAX],		// Menu widgets
	*drawing_col_prev = NULL;

GdkCursor *move_cursor;
GdkCursor *m_cursor[32];		// My mouse cursors



static GtkWidget *toolbar_zoom_main = NULL, *toolbar_zoom_view,
	*toolbar_labels[2],		// Colour A & B details
	*ts_spinslides[3],		// Size, flow, opacity
	*tb_label_opacity		// Opacity label
	;
static unsigned char *mem_prev = NULL;		// RGB colours, tool, pattern preview


GtkWidget *layer_iconbar(GtkWidget *window, GtkWidget *box, GtkWidget **icons)
{		// Create iconbar for layers window
	char **icon_list[7] = {
		xpm_new_xpm, xpm_up_xpm, xpm_down_xpm, xpm_copy_xpm, xpm_centre_xpm,
		xpm_cut_xpm, xpm_close_xpm
		};

	char *hint_text[7] = {
		_("New Layer"), _("Raise"), _("Lower"), _("Duplicate Layer"), _("Centralise Layer"),
		_("Delete Layer"), _("Close Layers Window")
		};

	gint i, offset[7] = {3, 1, 2, 4, 6, 5, 8};//0, 0, 7, 8};

	GtkWidget *toolbar, *iconw;
	GdkPixmap *icon, *mask;

// we need to realize the window because we use pixmaps for 
// items on the toolbar in the context of it
	gtk_widget_realize( window );

#if GTK_MAJOR_VERSION == 1
	toolbar = gtk_toolbar_new( GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS );
#endif
#if GTK_MAJOR_VERSION == 2
	toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style( GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS );
#endif

	gtk_box_pack_start ( GTK_BOX (box), toolbar, FALSE, FALSE, 0 );

	for (i=0; i<7; i++)
	{
		icon = gdk_pixmap_create_from_xpm_d ( main_window->window, &mask,
			NULL, icon_list[i] );
		iconw = gtk_pixmap_new ( icon, mask );
		gdk_pixmap_unref( icon );
		gdk_pixmap_unref( mask );

		icons[ offset[i] ] =
			gtk_toolbar_append_element( GTK_TOOLBAR(toolbar),
			GTK_TOOLBAR_CHILD_BUTTON, NULL, "None", hint_text[i],
			"Private", iconw, GTK_SIGNAL_FUNC(layer_iconbar_click), (gpointer) i);
	}
	gtk_widget_show ( toolbar );

	return toolbar;
}

static gint expose_preview( GtkWidget *widget, GdkEventExpose *event )
{
	int rx, ry, rw, rh;

	rx = event->area.x;
	ry = event->area.y;
	rw = event->area.width;
	rh = event->area.height;

	if ( ry < PREVIEW_HEIGHT )
	{
		if ( (ry+rh) >= PREVIEW_HEIGHT )
		{
			rh = PREVIEW_HEIGHT - ry;
		}
		gdk_draw_rgb_image( widget->window, widget->style->black_gc,
				rx, ry, rw, rh,
				GDK_RGB_DITHER_NONE,
				mem_prev + 3*( rx + PREVIEW_WIDTH*ry ),
				PREVIEW_WIDTH*3
				);
	}

	return FALSE;
}


static gint click_colours( GtkWidget *widget, GdkEventButton *event )
{
	if (mem_img[CHN_IMAGE])
	{
		if ( event->y > 31 ) choose_pattern(0);
		else
		{
			if ( event->x < 48 ) choose_colours();
			else choose_pattern(1);
		}
	}

	return FALSE;
}

static GtkWidget *toolbar_add_zoom(GtkWidget *box)		// Add zoom combo box
{
	int i;
	char *txt[] = { "10%", "20%", "25%", "33%", "50%", "100%", "200%",
		"400%", "800%", "1200%", "1600%", "2000%", NULL };
	GtkWidget *combo, *combo_entry;
	GList *combo_list = NULL;


	combo = gtk_combo_new ();
	gtk_combo_set_value_in_list (GTK_COMBO (combo), FALSE, FALSE);
	gtk_widget_show (combo);
	gtk_box_pack_start (GTK_BOX (box), combo, FALSE, FALSE, 0);
	combo_entry = GTK_COMBO (combo)->entry;
	GTK_WIDGET_UNSET_FLAGS (combo_entry, GTK_CAN_FOCUS);
	gtk_widget_set_usize(GTK_COMBO(combo)->button, 18, -1);


#if GTK_MAJOR_VERSION == 1
	gtk_widget_set_usize(combo, 75, -1);
#endif
#if GTK_MAJOR_VERSION == 2
	gtk_entry_set_width_chars( GTK_ENTRY(combo_entry), 6);
#endif

	gtk_entry_set_editable( GTK_ENTRY(combo_entry), FALSE );

	for ( i=0; txt[i]; i++ ) combo_list = g_list_append( combo_list, txt[i] );

	gtk_combo_set_popdown_strings( GTK_COMBO(combo), combo_list );
	g_list_free( combo_list );
	gtk_entry_set_text( GTK_ENTRY(combo_entry), "100%" );

	return combo;
}

static float toolbar_get_zoom( GtkWidget *combo )
{
	int i = 0;
	float res = 0;
	char *txt = (char *) gtk_entry_get_text( GTK_ENTRY(GTK_COMBO (combo)->entry) );

	if ( strlen(txt) > 2)		// Weed out bogus calls
	{
		sscanf(txt, "%i%%", &i);
		res = ((float) i ) / 100;
		if ( res<1 )
		{
			res = mt_round( 1/res );
			res = 1/res;
		}
	}

	return res;
}

void toolbar_viewzoom(gboolean visible)
{
	if ( visible )
		gtk_widget_show( toolbar_zoom_view );
	else
		gtk_widget_hide( toolbar_zoom_view );
}

void toolbar_zoom_update()			// Update the zoom combos to reflect current zoom
{
	char txt[32];

	if ( toolbar_zoom_main == NULL ) return;
	if ( toolbar_zoom_view == NULL ) return;

	snprintf( txt, 30, "%i%%", (int)(can_zoom*100) );
	gtk_entry_set_text( GTK_ENTRY(GTK_COMBO(toolbar_zoom_main)->entry), txt );

	snprintf( txt, 30, "%i%%", (int)(vw_zoom*100) );
	gtk_entry_set_text( GTK_ENTRY(GTK_COMBO(toolbar_zoom_view)->entry), txt );
}

static void toolbar_zoom_main_change()
{
	float new = toolbar_get_zoom( toolbar_zoom_main );

	if ( new > 0 ) align_size( new );
}

static void toolbar_zoom_view_change()
{
	float new = toolbar_get_zoom( toolbar_zoom_view );

	if ( new > 0 ) vw_align_size( new );
}

static int *vars_settings[TOTAL_ICONS_SETTINGS] = {
	&mem_continuous, &mem_undo_opacity,
	&tint_mode[0], &tint_mode[1],
	&mem_cselect, &mem_unmask
};

void toolbar_mode_change(GtkWidget *widget, gpointer data)
{
	gint j = (gint) data;

	*(vars_settings[j]) = GTK_TOGGLE_BUTTON(widget)->active;

	switch (j)
	{
	case SETB_CONT:
		inifile_set_gboolean( "continuousPainting", mem_continuous );
		break;
	case SETB_OPAC:
		inifile_set_gboolean( "opacityToggle", mem_undo_opacity );
		break;
	case SETB_CSEL:
		if (mem_cselect && !csel_data)
		{
			csel_init();
			mem_cselect = !!csel_data;
		}
		break;
	}
}

static int set_flood(GtkWidget *box, gpointer fdata)
{
	GtkWidget *spin, *toggle;
	GList *chain = GTK_BOX(box)->children;

	spin = ((GtkBoxChild*)chain->data)->widget;
	gtk_spin_button_update(GTK_SPIN_BUTTON(spin));
	flood_step = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(spin));
	chain = chain->next;
	toggle = ((GtkBoxChild*)chain->data)->widget;
	flood_cube = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle));
	chain = chain->next;
	toggle = ((GtkBoxChild*)chain->data)->widget;
	flood_img = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle));
	chain = chain->next;
	toggle = ((GtkBoxChild*)chain->data)->widget;
	flood_slide = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle));

	return TRUE;
}

static int set_smudge(GtkWidget *box, gpointer fdata)
{
	GtkWidget *toggle;

	toggle = ((GtkBoxChild*)GTK_BOX(box)->children->data)->widget;
	smudge_mode = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle));

	return TRUE;
}

static gboolean toolbar_rclick(GtkWidget *widget, GdkEventButton *event,
	gpointer user_data)
{
	GtkWidget *spin, *box;

	/* Handle only right clicks */
	if ((event->type != GDK_BUTTON_PRESS) || (event->button != 3))
		return (FALSE);

	switch ((gint)user_data)
	{
	case SETB_CSEL:
		colour_selector(COLSEL_EDIT_CSEL);
		break;
	case (TTB_0 + TTB_FLOOD): /* Flood fill step */
		spin = add_a_spin(0, 0, 200);
		gtk_spin_button_set_digits(GTK_SPIN_BUTTON(spin), 2);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), flood_step);
		box = gtk_vbox_new(FALSE, 5);
		gtk_widget_show(box);
		gtk_box_pack_start(GTK_BOX(box), spin, FALSE, FALSE, 0);
		add_a_toggle(_("RGB Cube"), box, flood_cube);
		add_a_toggle(_("By image channel"), box, flood_img);
		add_a_toggle(_("Gradient-driven"), box, flood_slide);
		filter_window(_("Fill settings"), box, set_flood, NULL, GTK_WIN_POS_MOUSE);
		break;
	case (TTB_0 + TTB_SMUDGE): /* Smudge opacity mode */
		box = gtk_vbox_new(FALSE, 5);
		gtk_widget_show(box);
		add_a_toggle(_("Respect opacity mode"), box, smudge_mode);
		filter_window(_("Smudge settings"), box, set_smudge, NULL, GTK_WIN_POS_MOUSE);
		break;
	default: /* For other buttons, do nothing */
		return (FALSE);
	}
	return (TRUE);
}


static void ts_update_spinslides()
{
	mt_spinslide_set_value(ts_spinslides[0], tool_size);
	mt_spinslide_set_value(ts_spinslides[1], tool_flow);
	mt_spinslide_set_value(ts_spinslides[2], mem_channel == CHN_IMAGE ?
		tool_opacity : channel_col_A[mem_channel]);
}


static void ts_spinslide_moved(GtkAdjustment *adj, gpointer user_data)
{
	int n, i;

	n = ADJ2INT(adj);
	switch ((int)user_data)
	{
	case 0: tool_size = n;
		break;
	case 1:	tool_flow = n;
		break;
	case 2:	i = mem_channel == CHN_IMAGE ? tool_opacity :
			channel_col_A[mem_channel];
		if (n != i) pressed_opacity(n);
		break;
	}
}


static void toolbar_settings_exit()
{
	toolbar_status[TOOLBAR_SETTINGS] = FALSE;
	gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(toolbar_menu_widgets[TOOLBAR_SETTINGS]),
			FALSE );
	toolbar_exit();
}


static void toolbar_settings_init()
{
	int i, vals[] = {tool_size, tool_flow, tool_opacity};
	char *ts_titles[] = { _("Size"), _("Flow"), _("Opacity") },
	**icon_list_settings[TOTAL_ICONS_SETTINGS] = {
		xpm_mode_cont_xpm, xpm_mode_opac_xpm, xpm_mode_tint_xpm, xpm_mode_tint2_xpm,
		xpm_mode_csel_xpm, xpm_mode_mask_xpm
		},
	*hint_text_settings[TOTAL_ICONS_SETTINGS] = {
		_("Continuous Mode"),  _("Opacity Mode"), _("Tint Mode"), _("Tint +-"),
		_("Colour-Selective Mode"), _("Disable All Masks")
		};

	GtkWidget *iconw, *label, *vbox, *table, *toolbar_settings, *but, *labels[3];
	GdkPixmap *icon, *mask;

	if ( toolbar_boxes[TOOLBAR_SETTINGS] )
	{
		gtk_widget_show( toolbar_boxes[TOOLBAR_SETTINGS] );	// Used when Home key is pressed
		return;
	}

///	SETTINGS TOOLBAR

#if GTK_MAJOR_VERSION == 1
	toolbar_settings = gtk_toolbar_new( GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS );
#endif
#if GTK_MAJOR_VERSION == 2
	toolbar_settings = gtk_toolbar_new();
	gtk_toolbar_set_style( GTK_TOOLBAR(toolbar_settings), GTK_TOOLBAR_ICONS );
#endif

	for (i=0; i<TOTAL_ICONS_SETTINGS; i++)
	{
		icon = gdk_pixmap_create_from_xpm_d ( main_window->window, &mask,
			NULL, icon_list_settings[i] );
		iconw = gtk_pixmap_new ( icon, mask );
		gdk_pixmap_unref( icon );
		gdk_pixmap_unref( mask );

		but = gtk_toolbar_append_element( GTK_TOOLBAR(toolbar_settings),
			GTK_TOOLBAR_CHILD_TOGGLEBUTTON, NULL, "None", hint_text_settings[i],
			"Private", iconw,
			GTK_SIGNAL_FUNC(toolbar_mode_change),
			(gpointer) i);

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(but), !!*(vars_settings[i]));

		if (i == SETB_CSEL)
		{
			gtk_signal_connect(GTK_OBJECT(but), "button_press_event",
				GTK_SIGNAL_FUNC(toolbar_rclick), (gpointer)i);
		}
	}


	toolbar_boxes[TOOLBAR_SETTINGS] = add_a_window( GTK_WINDOW_TOPLEVEL, _("Settings Toolbar"),
			GTK_WIN_POS_NONE, FALSE );

	gtk_widget_set_uposition( toolbar_boxes[TOOLBAR_SETTINGS],
		inifile_get_gint32("toolbar_settings_x", 0 ),
		inifile_get_gint32("toolbar_settings_y", 0 ) );

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show(vbox);
	gtk_container_add (GTK_CONTAINER (toolbar_boxes[TOOLBAR_SETTINGS]), vbox);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);

	gtk_box_pack_start ( GTK_BOX (vbox), toolbar_settings, FALSE, FALSE, 0 );
	gtk_widget_show(toolbar_settings);

	label = gtk_label_new("");
	toolbar_labels[0] = label;
	gtk_misc_set_alignment( GTK_MISC(label), 0, 0.5 );
	gtk_widget_show (label);
	gtk_misc_set_padding (GTK_MISC (label), 5, 2);
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

	label = gtk_label_new("");
	toolbar_labels[1] = label;
	gtk_misc_set_alignment( GTK_MISC(label), 0, 0.5 );
	gtk_widget_show (label);
	gtk_misc_set_padding (GTK_MISC (label), 5, 2);
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

	if (mem_channel != CHN_IMAGE) vals[2] = channel_col_A[mem_channel];
	table = add_a_table(3, 2, 5, vbox);
	for (i = 0; i < 3; i++)
	{
		labels[i] = add_to_table(ts_titles[i], table, i, 0, 0,
			GTK_JUSTIFY_LEFT, 0, 0.5);
		ts_spinslides[i] = mt_spinslide_new(150, -1);
		gtk_table_attach(GTK_TABLE(table), ts_spinslides[i], 1, 2,
			i, i + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
		mt_spinslide_set_range(ts_spinslides[i], i == 2 ? 0 : 1, 255);
		mt_spinslide_connect(ts_spinslides[i],
			GTK_SIGNAL_FUNC(ts_spinslide_moved), (gpointer)i);
	}
	tb_label_opacity = labels[2];

	gtk_signal_connect_object (GTK_OBJECT (toolbar_boxes[TOOLBAR_SETTINGS]), "delete_event",
		GTK_SIGNAL_FUNC (toolbar_settings_exit), NULL);
	gtk_window_set_transient_for( GTK_WINDOW(toolbar_boxes[TOOLBAR_SETTINGS]),
		GTK_WINDOW(main_window) );

	toolbar_update_settings();

	gtk_widget_show( toolbar_boxes[TOOLBAR_SETTINGS] );
}

void toolbar_init(GtkWidget *vbox_main)
{
	char txt[32];
	int i;

	GtkToolbarChildType child_type;
	GdkPixmap *icon, *mask;
	GtkWidget *iconw, *toolbar_main, *toolbar_tools, *previous = NULL, *hbox;
	GdkColor cfg = { -1, -1, -1, -1 }, cbg = { 0, 0, 0, 0 };

	char **icon_list_main[TOTAL_ICONS_MAIN] = {
		xpm_new_xpm, xpm_open_xpm, xpm_save_xpm, xpm_cut_xpm,
		xpm_copy_xpm, xpm_paste_xpm, xpm_undo_xpm, xpm_redo_xpm,
		xpm_brcosa_xpm, xpm_pan_xpm
		},
	*hint_text_main[TOTAL_ICONS_MAIN] = {
		_("New Image"), _("Load Image File"), _("Save Image File"), _("Cut"),
		_("Copy"), _("Paste"), _("Undo"), _("Redo"),
		_("Transform Colour"), _("Pan Window")
		 },
	**icon_list_tools[TOTAL_ICONS_TOOLS] = {
		xpm_paint_xpm, xpm_shuffle_xpm, xpm_flood_xpm,
		xpm_line_xpm, xpm_smudge_xpm, xpm_clone_xpm, xpm_select_xpm,
		xpm_polygon_xpm, xpm_lasso_xpm, xpm_text_xpm,
		xpm_ellipse2_xpm, xpm_ellipse_xpm, xpm_rect1_xpm, xpm_rect2_xpm,
		xpm_flip_vs_xpm, xpm_flip_hs_xpm, xpm_rotate_cs_xpm, xpm_rotate_as_xpm
		},
	*hint_text_tools[TOTAL_ICONS_TOOLS] = {
		_("Paint"), _("Shuffle"), _("Flood Fill"),
		_("Straight Line"), _("Smudge"), _("Clone"), _("Make Selection"),
		_("Polygon Selection"), _("Lasso Selection"),_("Paste Text"),
		_("Ellipse Outline"), _("Filled Ellipse"), _("Outline Selection"), _("Fill Selection"),
		_("Flip Selection Vertically"), _("Flip Selection Horizontally"),
		_("Rotate Selection Clockwise"), _("Rotate Selection Anti-Clockwise")
		},
	*xbm_list[TOTAL_CURSORS] = { xbm_square_bits, xbm_circle_bits,
		xbm_horizontal_bits, xbm_vertical_bits, xbm_slash_bits, xbm_backslash_bits,
		xbm_spray_bits, xbm_shuffle_bits, xbm_flood_bits, xbm_select_bits, xbm_line_bits,
		xbm_smudge_bits, xbm_polygon_bits, xbm_clone_bits
		},
	*xbm_mask_list[TOTAL_CURSORS] = { xbm_square_mask_bits, xbm_circle_mask_bits,
		xbm_horizontal_mask_bits, xbm_vertical_mask_bits, xbm_slash_mask_bits,
		xbm_backslash_mask_bits, xbm_spray_mask_bits, xbm_shuffle_mask_bits,
		xbm_flood_mask_bits, xbm_select_mask_bits, xbm_line_mask_bits,
		xbm_smudge_mask_bits, xbm_polygon_mask_bits, xbm_clone_mask_bits
		};

	for ( i=1; i<TOOLBAR_MAX; i++ )
	{
		sprintf(txt, "toolbar%i", i);
		toolbar_status[i] = inifile_get_gboolean( txt, TRUE );

		gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(toolbar_menu_widgets[i]),
				toolbar_status[i] );	// Menu toggles = status
	}

///	MAIN TOOLBAR

	hbox = gtk_hbox_new (FALSE, 0);
	toolbar_boxes[TOOLBAR_MAIN] = hbox;
	if ( toolbar_status[TOOLBAR_MAIN] ) gtk_widget_show (hbox);	// Only show if user wants
	gtk_box_pack_start ( GTK_BOX (vbox_main), hbox, FALSE, FALSE, 0 );


#if GTK_MAJOR_VERSION == 1
	toolbar_main = gtk_toolbar_new( GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS );
	toolbar_tools = gtk_toolbar_new( GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS );
#endif
#if GTK_MAJOR_VERSION == 2
	toolbar_main = gtk_toolbar_new();
	gtk_toolbar_set_style( GTK_TOOLBAR(toolbar_main), GTK_TOOLBAR_ICONS );
	toolbar_tools = gtk_toolbar_new();
	gtk_toolbar_set_style( GTK_TOOLBAR(toolbar_tools), GTK_TOOLBAR_ICONS );
#endif

	gtk_box_pack_start ( GTK_BOX (hbox), toolbar_main, FALSE, FALSE, 0 );

	toolbar_zoom_main = toolbar_add_zoom( hbox );
	gtk_signal_connect_object (GTK_OBJECT (GTK_COMBO (toolbar_zoom_main)->entry), "changed",
		GTK_SIGNAL_FUNC (toolbar_zoom_main_change), NULL);
	toolbar_zoom_view = toolbar_add_zoom( hbox );
	gtk_signal_connect_object (GTK_OBJECT (GTK_COMBO (toolbar_zoom_view)->entry), "changed",
		GTK_SIGNAL_FUNC (toolbar_zoom_view_change), NULL);
	toolbar_viewzoom(FALSE);

	for (i=0; i<TOTAL_CURSORS; i++)
	{
		icon = gdk_bitmap_create_from_data (NULL, xbm_list[i], 20, 20);
		mask = gdk_bitmap_create_from_data (NULL, xbm_mask_list[i], 20, 20);

		if ( i <= TOOL_SHUFFLE || i == TOOL_LINE || i == TOOL_SMUDGE || i == TOOL_CLONE )
			m_cursor[i] = gdk_cursor_new_from_pixmap (icon, mask, &cfg, &cbg, 0, 0);
		else
		{
			if ( i == TOOL_FLOOD)
				m_cursor[i] = gdk_cursor_new_from_pixmap (icon, mask, &cfg, &cbg, 2, 19);
			else
				m_cursor[i] = gdk_cursor_new_from_pixmap (icon, mask, &cfg, &cbg, 10, 10);
		}

		gdk_pixmap_unref( icon );
		gdk_pixmap_unref( mask );
	}
	icon = gdk_bitmap_create_from_data (NULL, xbm_move_bits, 20, 20);
	mask = gdk_bitmap_create_from_data (NULL, xbm_move_mask_bits, 20, 20);
	move_cursor = gdk_cursor_new_from_pixmap (icon, mask, &cfg, &cbg, 10, 10);
	gdk_pixmap_unref( icon );
	gdk_pixmap_unref( mask );

	previous = NULL;
	for (i=0; i<TOTAL_ICONS_TOOLS; i++)
	{
		icon = gdk_pixmap_create_from_xpm_d ( main_window->window, &mask,
			NULL, icon_list_tools[i] );
		iconw = gtk_pixmap_new ( icon, mask );
		gdk_pixmap_unref( icon );
		gdk_pixmap_unref( mask );

		if (i > TTB_POLY)
		{
			child_type = GTK_TOOLBAR_CHILD_BUTTON;
			previous = NULL;
		}
		else
		{
			child_type = GTK_TOOLBAR_CHILD_RADIOBUTTON;
			previous = i ? icon_buttons[i - 1] : NULL;
		}

		icon_buttons[i] = gtk_toolbar_append_element( GTK_TOOLBAR(toolbar_tools),
			child_type, previous, "None", hint_text_tools[i],
			"Private", iconw, GTK_SIGNAL_FUNC(toolbar_icon_event), (gpointer) i);
		if ((i == TTB_FLOOD) || (i == TTB_SMUDGE))
			gtk_signal_connect(GTK_OBJECT(icon_buttons[i]),
				"button_press_event", GTK_SIGNAL_FUNC(toolbar_rclick),
				(gpointer)(i + TTB_0));

	}


	gtk_toolbar_insert_space(GTK_TOOLBAR(toolbar_tools), TTB_SELFV);
	gtk_toolbar_insert_space(GTK_TOOLBAR(toolbar_tools), TTB_ELLIPSE);
	gtk_toolbar_insert_space(GTK_TOOLBAR(toolbar_tools), TTB_LASSO);

	men_dis_add(icon_buttons[TTB_SMUDGE], menu_only_24);
	men_dis_add(icon_buttons[TTB_LASSO], menu_lasso);

	men_dis_add(icon_buttons[TTB_ELLIPSE], menu_need_selection);
	men_dis_add(icon_buttons[TTB_FELLIPSE], menu_need_selection);

	men_dis_add(icon_buttons[TTB_OUTLINE], menu_need_selection);
	men_dis_add(icon_buttons[TTB_FILL], menu_need_selection);
	men_dis_add(icon_buttons[TTB_OUTLINE], menu_lasso);
	men_dis_add(icon_buttons[TTB_FILL], menu_lasso);

	men_dis_add(icon_buttons[TTB_SELFV], menu_need_clipboard );	// Flip sel V
	men_dis_add(icon_buttons[TTB_SELFH], menu_need_clipboard );	// Flip sel H
	men_dis_add(icon_buttons[TTB_SELRCW], menu_need_clipboard );	// Rot sel clock
	men_dis_add(icon_buttons[TTB_SELRCCW], menu_need_clipboard );	// Rot sel anti


	for (i=0; i<TOTAL_ICONS_MAIN; i++)
	{
		icon = gdk_pixmap_create_from_xpm_d ( main_window->window, &mask,
			NULL, icon_list_main[i] );
		iconw = gtk_pixmap_new ( icon, mask );
		gdk_pixmap_unref( icon );
		gdk_pixmap_unref( mask );

		icon_buttons2[i] = gtk_toolbar_append_element( GTK_TOOLBAR(toolbar_main),
			GTK_TOOLBAR_CHILD_BUTTON, NULL, "None", hint_text_main[i],
			"Private", iconw, GTK_SIGNAL_FUNC(toolbar_icon_event2), (gpointer) i);
	}
	gtk_toolbar_insert_space( GTK_TOOLBAR(toolbar_main), 8 );
	gtk_toolbar_insert_space( GTK_TOOLBAR(toolbar_main), 6 );
	gtk_toolbar_insert_space( GTK_TOOLBAR(toolbar_main), 3 );

	men_dis_add( icon_buttons2[3], menu_need_selection );	// Cut
	men_dis_add( icon_buttons2[3], menu_lasso );		// Cut
	men_dis_add( icon_buttons2[4], menu_need_selection );	// Copy
	men_dis_add( icon_buttons2[4], menu_lasso );		// Copy
	men_dis_add( icon_buttons2[5], menu_need_clipboard );	// Paste
	men_dis_add( icon_buttons2[6], menu_undo );		// Undo
	men_dis_add( icon_buttons2[7], menu_redo );		// Undo

	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(icon_buttons[PAINT_TOOL_ICON]), TRUE );
	gtk_widget_show ( toolbar_main );



///	TOOLS TOOLBAR

	hbox = gtk_hbox_new (FALSE, 0);
	toolbar_boxes[TOOLBAR_TOOLS] = hbox;
	if ( toolbar_status[TOOLBAR_TOOLS] ) gtk_widget_show (hbox);	// Only show if user wants
	gtk_box_pack_start ( GTK_BOX (vbox_main), hbox, FALSE, FALSE, 0 );

	gtk_box_pack_start ( GTK_BOX (hbox), toolbar_tools, FALSE, FALSE, 0 );
	gtk_widget_show ( toolbar_tools );
}

void toolbar_update_settings()
{
	char txt[32];

	if ( toolbar_boxes[TOOLBAR_SETTINGS] == NULL ) return;

	ts_update_spinslides();		// Update tool settings

	if ( mem_img_bpp == 1 )
		snprintf(txt, 30, "A [%i] = {%i,%i,%i}", mem_col_A, mem_pal[mem_col_A].red,
			mem_pal[mem_col_A].green, mem_pal[mem_col_A].blue );
	else	snprintf(txt, 30, "A = {%i,%i,%i}", mem_col_A24.red,
			mem_col_A24.green, mem_col_A24.blue );
	gtk_label_set_text( GTK_LABEL(toolbar_labels[0]), txt );

	if ( mem_img_bpp == 1 )
		snprintf(txt, 30, "B [%i] = {%i,%i,%i}", mem_col_B, mem_pal[mem_col_B].red,
			mem_pal[mem_col_B].green, mem_pal[mem_col_B].blue );
	else	snprintf(txt, 30, "B = {%i,%i,%i}", mem_col_B24.red,
			mem_col_B24.green, mem_col_B24.blue );
	gtk_label_set_text( GTK_LABEL(toolbar_labels[1]), txt );

	if ( mem_channel == CHN_IMAGE )
		gtk_label_set_text( GTK_LABEL(tb_label_opacity), _("Opacity") );
	else
		gtk_label_set_text( GTK_LABEL(tb_label_opacity), channames[mem_channel] );
}

static gint expose_palette( GtkWidget *widget, GdkEventExpose *event )
{
	gdk_draw_rgb_image( widget->window, widget->style->black_gc,
				event->area.x, event->area.y, event->area.width, event->area.height,
				GDK_RGB_DITHER_NONE,
				mem_pals + 3*( event->area.x + PALETTE_WIDTH*event->area.y ),
				PALETTE_WIDTH*3
				);

	return FALSE;
}


static gint motion_palette( GtkWidget *widget, GdkEventMotion *event )
{
	GdkModifierType state;
	int x, y, px, py, pindex;

	px = event->x;
	py = event->y;
	pindex = (py-39+34)/16;

	mtMAX(pindex, pindex, 0)
	mtMIN(pindex, pindex, mem_cols-1)

	if (event->is_hint) gdk_window_get_pointer (event->window, &x, &y, &state);
	else
	{
		x = event->x;
		y = event->y;
		state = event->state;
	}

	if ( drag_index && drag_index_vals[1] != pindex )
	{
		mem_pal_index_move( drag_index_vals[1], pindex );
		init_pal();
		drag_index_vals[1] = pindex;
	}

	return TRUE;
}

static gint release_palette( GtkWidget *widget, GdkEventButton *event )
{
	if (drag_index)
	{
		drag_index = FALSE;
		gdk_window_set_cursor( drawing_palette->window, NULL );
		if ( drag_index_vals[0] != drag_index_vals[1] )
		{
			mem_pal_copy( mem_pal, brcosa_pal );		// Get old values back
			mem_undo_next(UNDO_XPAL);			// Do undo stuff
			mem_pal_index_move( drag_index_vals[0], drag_index_vals[1] );

			if ( mem_img_bpp == 1 )
				mem_canvas_index_move( drag_index_vals[0], drag_index_vals[1] );

			canvas_undo_chores();
		}
	}

	return FALSE;
}

static gint click_palette( GtkWidget *widget, GdkEventButton *event )
{
	int px, py, pindex;

	px = event->x;
	py = event->y;
	pindex = (py-2)/16;
	if (py < 2) pindex = -1;

	if (pindex>=0 && pindex<mem_cols)
	{
		if ( px<22 ) pressed_allcol( NULL, NULL );
		if ( px>22 && px <53 )		// Colour A or B changed
		{
			if ( event->button == 1 )
			{
				if ( (event->state & GDK_CONTROL_MASK) )
				{
					mem_col_B = pindex;
					mem_col_B24 = mem_pal[mem_col_B];
				}
				else if ( (event->state & GDK_SHIFT_MASK) )
				{
					mem_pal_copy( brcosa_pal, mem_pal );
					drag_index = TRUE;
					drag_index_vals[0] = pindex;
					drag_index_vals[1] = pindex;
					gdk_window_set_cursor( drawing_palette->window, move_cursor );
				}
				else
				{
					mem_col_A = pindex;
					mem_col_A24 = mem_pal[mem_col_A];
				}
			}
			if ( event->button == 3 )
			{
				mem_col_B = pindex;
				mem_col_B24 = mem_pal[mem_col_B];
			}

			repaint_top_swatch();
			init_pal();
			gtk_widget_queue_draw( drawing_col_prev );	// Update widget
		}
		if ( px>=53 )			// Mask changed
		{
			mem_prot_mask[pindex] ^= 255;

			repaint_swatch(pindex);				// Update swatch
			gtk_widget_queue_draw_area( widget, 0, event->y-16,
				PALETTE_WIDTH, 32 );			// Update widget

			mem_mask_init();		// Prepare RGB masks
		}
	}

	return TRUE;
}

void toolbar_palette_init(GtkWidget *box)		// Set up the palette area
{
	GtkWidget *vbox, *hbox, *scrolledwindow_palette, *viewport_palette;


	vbox = gtk_vbox_new (FALSE, 0);
	if ( toolbar_status[TOOLBAR_PALETTE] ) gtk_widget_show (vbox);
	gtk_box_pack_start (GTK_BOX (box), vbox, FALSE, TRUE, 0);

	toolbar_boxes[TOOLBAR_PALETTE] = vbox;		// Hide palette area

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 5);


	drawing_col_prev = gtk_drawing_area_new ();
#if GTK_MAJOR_VERSION == 2
	viewport_palette = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport_palette);
	gtk_viewport_set_shadow_type( GTK_VIEWPORT(viewport_palette), GTK_SHADOW_IN );
	gtk_box_pack_start( GTK_BOX(hbox), viewport_palette, TRUE, FALSE, 0 );

	gtk_container_add (GTK_CONTAINER (viewport_palette), drawing_col_prev);
#endif
#if GTK_MAJOR_VERSION == 1
	gtk_box_pack_start( GTK_BOX(hbox), drawing_col_prev, TRUE, FALSE, 0 );
#endif
	gtk_widget_set_usize( drawing_col_prev, PREVIEW_WIDTH, PREVIEW_HEIGHT );

	gtk_widget_show( drawing_col_prev );
	gtk_signal_connect_object( GTK_OBJECT(drawing_col_prev), "button_release_event",
		GTK_SIGNAL_FUNC (click_colours), GTK_OBJECT(drawing_col_prev) );
	gtk_signal_connect_object( GTK_OBJECT(drawing_col_prev), "expose_event",
		GTK_SIGNAL_FUNC (expose_preview), GTK_OBJECT(drawing_col_prev) );
	gtk_widget_set_events (drawing_col_prev, GDK_ALL_EVENTS_MASK);

	scrolledwindow_palette = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow_palette);
	gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow_palette, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_palette),
		GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	viewport_palette = gtk_viewport_new (NULL, NULL);

	gtk_widget_set_usize( viewport_palette, PALETTE_WIDTH, 64 );
	gtk_widget_show (viewport_palette);
	gtk_container_add (GTK_CONTAINER (scrolledwindow_palette), viewport_palette);

	drawing_palette = gtk_drawing_area_new ();
	gtk_widget_set_usize( drawing_palette, PALETTE_WIDTH, 64 );
	gtk_container_add (GTK_CONTAINER (viewport_palette), drawing_palette);
	gtk_widget_show( drawing_palette );
	gtk_signal_connect_object( GTK_OBJECT(drawing_palette), "expose_event",
		GTK_SIGNAL_FUNC (expose_palette), GTK_OBJECT(drawing_palette) );
	gtk_signal_connect_object( GTK_OBJECT(drawing_palette), "button_press_event",
		GTK_SIGNAL_FUNC (click_palette), GTK_OBJECT(drawing_palette) );
	gtk_signal_connect_object( GTK_OBJECT(drawing_palette), "motion_notify_event",
		GTK_SIGNAL_FUNC (motion_palette), GTK_OBJECT(drawing_palette) );
	gtk_signal_connect_object( GTK_OBJECT(drawing_palette), "button_release_event",
		GTK_SIGNAL_FUNC (release_palette), GTK_OBJECT(drawing_palette) );

	gtk_widget_set_events (drawing_palette, GDK_ALL_EVENTS_MASK);
}

void toolbar_exit()				// Remember toolbar settings on program exit
{
	int i, x, y;
	char txt[32];

	for ( i=1; i<TOOLBAR_MAX; i++ )		// Remember current show/hide status
	{
		sprintf(txt, "toolbar%i", i);
		inifile_set_gboolean( txt, toolbar_status[i] );
	}

	if ( toolbar_boxes[TOOLBAR_SETTINGS] == NULL ) return;

	gdk_window_get_root_origin( toolbar_boxes[TOOLBAR_SETTINGS]->window, &x, &y );
	
	inifile_set_gint32("toolbar_settings_x", x );
	inifile_set_gint32("toolbar_settings_y", y );

	gtk_widget_destroy(toolbar_boxes[TOOLBAR_SETTINGS]);

	toolbar_boxes[TOOLBAR_SETTINGS] = NULL;
}


void toolbar_showhide()				// Show/Hide all 4 toolbars
{
	int i, bar[] = { TOOLBAR_MAIN, TOOLBAR_TOOLS, TOOLBAR_PALETTE, TOOLBAR_STATUS };

	if ( toolbar_boxes[TOOLBAR_MAIN] == NULL ) return;		// Grubby hack to avoid segfault

	for ( i=0; i<4; i++ )
	{
		if ( toolbar_status[ bar[i] ] )
			gtk_widget_show( toolbar_boxes[ bar[i] ] );
		else
			gtk_widget_hide( toolbar_boxes[ bar[i] ] );
	}

	if ( !toolbar_status[TOOLBAR_SETTINGS] )
	{
		toolbar_exit();
	}
	else
	{
		toolbar_settings_init();
		gdk_window_raise( main_window->window );
	}
}


void pressed_toolbar_toggle( GtkMenuItem *menu_item, gpointer user_data, gint item )
{						// Menu toggle for toolbars
	toolbar_status[item] = GTK_CHECK_MENU_ITEM(menu_item)->active;;
	toolbar_showhide();
}



///	PATTERNS/TOOL PREVIEW AREA


void toolbar_preview_init()		// Initialize memory for preview area
{
	mem_prev = grab_memory( 3*PREVIEW_WIDTH*PREVIEW_HEIGHT, 0 );
}


void mem_set_brush(int val)			// Set brush, update size/flow/preview
{
	int offset, i, j, k, o, o2;

	tool_type = mem_brush_list[val][0];
	tool_size = mem_brush_list[val][1];
	if ( mem_brush_list[val][2]>0 ) tool_flow = mem_brush_list[val][2];

	offset = 3*( 2 + 36*(val % 9) + 36*PATCH_WIDTH*(val / 9) + 2*PATCH_WIDTH );
			// Offset in brush RGB
	for ( j=0; j<32; j++ )
	{
		o = 3*(40 + PREVIEW_WIDTH*j);		// Preview offset
		o2 = offset + 3*PATCH_WIDTH*j;		// Offset in brush RGB
		for ( i=0; i<32; i++ )
		{
			for ( k=0; k<3; k++ )
				mem_prev[o + 3*i + k] = mem_brushes[o2 + 3*i + k];
		}
	}

	if ( drawing_col_prev ) gtk_widget_queue_draw( drawing_col_prev );
}

void mem_pat_update()			// Update indexed and then RGB pattern preview
{
	int i, j, i2, j2, c, offset;
	png_color c24;

	if ( mem_img_bpp == 1 )
	{
		mem_col_A24 = mem_pal[mem_col_A];
		mem_col_B24 = mem_pal[mem_col_B];
	}

	for ( j=0; j<8; j++ )
	{
		for ( i=0; i<8; i++ )
		{
			if ( mem_patterns[tool_pat][j][i] == 1 )
			{
				c = mem_col_A;
				c24 = mem_col_A24;
			}
			else
			{
				c = mem_col_B;
				c24 = mem_col_B24;
			}

			mem_col_pat[i + j*8] = c;

			mem_col_pat24[ 3*(i + j*8) ] = c24.red;
			mem_col_pat24[ 1 + 3*(i + j*8) ] = c24.green;
			mem_col_pat24[ 2 + 3*(i + j*8) ] = c24.blue;

			for ( j2=0; j2<2; j2++ )		// 16 pixels high (8x2)
			{
				for ( i2=0; i2<(PREVIEW_WIDTH / 8); i2++ )
				{
					offset = 3*(i+i2*8 + (j+j2*8+32)*PREVIEW_WIDTH);
					mem_prev[ 0 + offset ] = c24.red;
					mem_prev[ 1 + offset ] = c24.green;
					mem_prev[ 2 + offset ] = c24.blue;
				}
			}
		}
	}
}

void repaint_top_swatch()			// Update selected colours A & B
{
	int i, j, r[2], g[2], b[2], nx, ny;

	if ( mem_img_bpp == 1 )
	{
		mem_col_A24 = mem_pal[mem_col_A];
		mem_col_B24 = mem_pal[mem_col_B];
	}
	r[0] = mem_col_A24.red;
	g[0] = mem_col_A24.green;
	b[0] = mem_col_A24.blue;
	r[1] = mem_col_B24.red;
	g[1] = mem_col_B24.green;
	b[1] = mem_col_B24.blue;

	for ( j=0; j<20; j++ )
	{
		for ( i=0; i<20; i++ )
		{
			nx = i+1; ny = j+1;
			mem_prev[ 0 + 3*( nx + ny*PREVIEW_WIDTH) ] = r[0];
			mem_prev[ 1 + 3*( nx + ny*PREVIEW_WIDTH) ] = g[0];
			mem_prev[ 2 + 3*( nx + ny*PREVIEW_WIDTH) ] = b[0];

			nx = i+11; ny = j+11;
			mem_prev[ 0 + 3*( nx + ny*PREVIEW_WIDTH) ] = r[1];
			mem_prev[ 1 + 3*( nx + ny*PREVIEW_WIDTH) ] = g[1];
			mem_prev[ 2 + 3*( nx + ny*PREVIEW_WIDTH) ] = b[1];
		}
	}

	if ( drawing_col_prev ) gtk_widget_queue_draw( drawing_col_prev );
}
