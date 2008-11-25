#include "map_widget.h"

#include "game/units/units.h"

#include "game/config.h"
#include "sprites.h"
#include "game/gui/paths.h"

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

namespace aw
{
	namespace gui
	{
		namespace gtk
		{
			map_widget::map_widget()
			{
				this->set_size_request(480, 320);
				this->set_events(Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK);

				m_background = Cairo::ImageSurface::create_from_png(aw::config().get<std::string>("/config/dirs/data") + "/background.png");
			}

			void map_widget::write_to_png(const std::string &filename)
			{
				Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 30*16, 20*16);
				Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);

				cr->set_source(m_background, 0, 0);
				cr->paint();
				if(m_scene)
					this->update_display(cr);

				surface->write_to_png(filename);
			}
			
			void map_widget::disable()
			{
				this->set_sensitive(false);
			}
			
			void map_widget::enable()
			{
				this->set_sensitive(true);
			}

			bool map_widget::on_expose_event(GdkEventExpose* event)
			{
				bool ret = Gtk::Widget::on_expose_event(event);

				Cairo::RefPtr<Cairo::Context> cr = this->get_window()->create_cairo_context();

				cr->set_source(m_background, 0, 0);
				cr->paint();

				if(m_scene)
					this->update_display(cr);

				if(!this->is_sensitive())
				{
					cr->set_source_rgba(0.7, 0.7, 0.7, 0.5);
					cr->paint();
				}

				return ret;
			}


			bool map_widget::on_button_press_event(GdkEventButton* event)
			{
				if(m_scene)
				{
					int x = event->x/16;
					int y = event->y/16;

					if(x >= 30 || y >= 20)
						return false;

					if(event->type == GDK_BUTTON_PRESS)
						m_signal_click(coord(x, y), event->button);

					return true;
				}
				else
				{
					return false;
				}
			}


			bool map_widget::on_motion_notify_event(GdkEventMotion *event)
			{
				static int old_x;
				static int old_y;

				const int x = static_cast<int>(event->x/16);
				const int y = static_cast<int>(event->y/16);

				if((x != old_x || y != old_y) && (x < 30 && y < 20))
				{
					m_signal_focus_changed(coord(x, y));

					old_x = x;
					old_y = y;
				}

				return Gtk::Widget::on_motion_notify_event(event);
			}


			void map_widget::draw(Cairo::RefPtr<Cairo::Context> &context, int x, int y, const std::string &filename)
			{
				sprites::ptr surface = aw::sprite().get_sprite(filename);
				context->set_source(surface, (x*16)+(16-surface->get_width()), (y*16)+(16-surface->get_height()));
				context->paint();
			}

			void map_widget::queue_draw()
			{
				Gtk::Widget::queue_draw();
			}


			void map_widget::update_display(Cairo::RefPtr<Cairo::Context>& cr)
			{
				for(int x = 0; x < 30; x++)
				{
					for(int y = 0; y < 20; y++)
					{
						const terrain::types type = m_scene->get_terrain(x, y)->type();
						const terrain::extras extra = m_scene->get_terrain(x, y)->extra();

						if(type == terrain::WATER)
						{
							if(y > 0 && x > 0 && terrain::is_ground(m_scene->get_terrain(x-1, y-1)->type()))
								this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::UL, terrain::CORNER)); //Ecke oben Links
							if(y < 19 && x > 0 && terrain::is_ground(m_scene->get_terrain(x-1, y+1)->type()))
								this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::LD, terrain::CORNER)); //Ecke unten links
							if(y > 0 && x < 29 && terrain::is_ground(m_scene->get_terrain(x+1, y-1)->type()))
								this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::UR, terrain::CORNER)); //Ecke oben rechts
							if(y < 19 && x < 29 && terrain::is_ground(m_scene->get_terrain(x+1, y+1)->type()))
								this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::DR, terrain::CORNER)); //Ecke unten rechts

							if(x > 0 && terrain::is_ground(m_scene->get_terrain(x-1, y)->type()))
								this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::L)); //Küste links
							if(x < 29 && terrain::is_ground(m_scene->get_terrain(x+1, y)->type()))
								this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::R)); //Küste rechts
							if(y > 0 && terrain::is_ground(m_scene->get_terrain(x, y-1)->type()))
								this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::U)); //Küste oben
							if(y < 19 && terrain::is_ground(m_scene->get_terrain(x, y+1)->type()))
								this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::D)); //Küste unten

							if(x > 0 && y < 19 && y > 0 && m_scene->get_terrain(x-1, y)->type() == terrain::RIVER &&
									(m_scene->get_terrain(x-1, y+1)->type() != terrain::RIVER && m_scene->get_terrain(x-1, y-1)->type() != terrain::RIVER))
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::L, terrain::MOUTH)); //Flussmündung links

							if(x < 29 && y < 19 && y > 0 && m_scene->get_terrain(x+1, y)->type() == terrain::RIVER &&
									(m_scene->get_terrain(x+1, y-1)->type() != terrain::RIVER && m_scene->get_terrain(x+1, y+1)->type() != terrain::RIVER))
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::R, terrain::MOUTH)); //Flussmündung rechts

							if(y > 0 && x < 29 && x > 0 && m_scene->get_terrain(x, y-1) ->type()== terrain::RIVER &&
									(m_scene->get_terrain(x-1, y-1)->type() != terrain::RIVER && m_scene->get_terrain(x+1, y-1)->type() != terrain::RIVER))
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::U, terrain::MOUTH)); //Flussmündung oben

							if(y < 19 && x < 29 && x > 0 && m_scene->get_terrain(x, y+1)->type() == terrain::RIVER &&
									(m_scene->get_terrain(x-1, y+1)->type() != terrain::RIVER && m_scene->get_terrain(x+1, y+1)->type() != terrain::RIVER))
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::D, terrain::MOUTH)); //Flussmündung unten

							if(x > 0 && terrain::is_ground(m_scene->get_terrain(x-1, y)->type()))
							{
								if(y > 0 && terrain::is_ground(m_scene->get_terrain(x, y-1)->type()))
									this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::UL)); //Nach innen gebogene Küste (terrain::UL)

								if(y < 19 && terrain::is_ground(m_scene->get_terrain(x, y+1)->type()))
									this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::LD)); //Nach innen gebogene Küste (LD)
							}

							if(x < 29 && terrain::is_ground(m_scene->get_terrain(x+1, y)->type()))
							{
								if(y > 0 && terrain::is_ground(m_scene->get_terrain(x, y-1)->type()))
									this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::UR)); //Nach innen gebogene Küste (UR)
								else if(y < 19 && terrain::is_ground(m_scene->get_terrain(x, y+1)->type()))
									this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::DR)); //Nach innen gebogene Küste (DR)
							}

							if(x > 0 && x < 29 && terrain::is_ground(m_scene->get_terrain(x-1, y)->type()) && terrain::is_ground(m_scene->get_terrain(x+1, y)->type()))
							{
								if(y > 0 && terrain::is_ground(m_scene->get_terrain(x, y-1)->type()))
									this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::ULR)); //Bucht nach unten offen
								else if(y < 19 && terrain::is_ground(m_scene->get_terrain(x, y+1)->type()))
									this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::LDR)); //Bucht nach oben offen
							}

							if(y > 0 && y < 19 && terrain::is_ground(m_scene->get_terrain(x, y-1)->type()) && terrain::is_ground(m_scene->get_terrain(x, y+1)->type()))
							{
								if(x > 0 && terrain::is_ground(m_scene->get_terrain(x-1, y)->type()))
									this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::ULD)); //Bucht nach rechts offen
								else if(x < 29 && terrain::is_ground(m_scene->get_terrain(x+1, y)->type()))
									this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::UDR)); //Bucht nach links offen
							}

							if(y > 0 && y < 19 && x > 0 && x < 29)
							{
								if(terrain::is_ground(m_scene->get_terrain(x, y-1)->type()) && //Oben
										terrain::is_ground(m_scene->get_terrain(x, y+1)->type()) && //Unten
										terrain::is_ground(m_scene->get_terrain(x+1, y)->type()) && //Rechts
										terrain::is_ground(m_scene->get_terrain(x-1, y)->type())) //Links
									this->draw(cr, x, y, get_path(terrain::SEASIDE, terrain::ULDR)); //See (oder was das sein soll)
							}
						}
						else if(type == terrain::RIVER)
						{
							if(y > 0 && y < 19 && x > 0 && x < 29 &&
									is_connectable(terrain::RIVER, m_scene->get_terrain(x, y-1)->type()) && //Oben
									is_connectable(terrain::RIVER, m_scene->get_terrain(x, y+1)->type()) && //Unten
									is_connectable(terrain::RIVER, m_scene->get_terrain(x+1, y)->type()) && //Rechts
									is_connectable(terrain::RIVER, m_scene->get_terrain(x-1, y)->type())) //Links)
									{
										this->draw(cr, x, y, get_path(terrain::RIVER, terrain::ULDR)); //Flusskreuzung (ULDR)
									}
							else if(y > 0 && y < 19 && x > 0 &&
									is_connectable(terrain::RIVER, m_scene->get_terrain(x, y-1)->type()) && //Oben
									is_connectable(terrain::RIVER, m_scene->get_terrain(x, y+1)->type()) && //Unten
									is_connectable(terrain::RIVER, m_scene->get_terrain(x-1, y)->type())) //Links
							{
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::ULD)); //Fluss mündet in anderen Fluss (terrain::ULD)
							}
							else if(y > 0 && y < 19 && x < 29 &&
									is_connectable(terrain::RIVER, m_scene->get_terrain(x, y-1)->type()) && //Oben
									is_connectable(terrain::RIVER, m_scene->get_terrain(x, y+1)->type()) && //Unten
									is_connectable(terrain::RIVER, m_scene->get_terrain(x+1, y)->type())) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::UDR)); //Fluss mündet in anderen Fluss (UDR)
							}
							else if(y < 19 && x > 0 && x < 29 &&
									is_connectable(terrain::RIVER, m_scene->get_terrain(x, y+1)->type()) && //Unten
									is_connectable(terrain::RIVER, m_scene->get_terrain(x-1, y)->type()) &&//Links
									is_connectable(terrain::RIVER, m_scene->get_terrain(x+1, y)->type())) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::LDR)); //Fluss mündet in anderen Fluss (LDR)
							}
							else if(y > 0 && x > 0 && x < 29 &&
									is_connectable(terrain::RIVER, m_scene->get_terrain(x, y-1)->type()) && //Oben
									is_connectable(terrain::RIVER, m_scene->get_terrain(x-1, y)->type()) &&//Links
									is_connectable(terrain::RIVER, m_scene->get_terrain(x+1, y)->type())) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::ULR)); //Fluss mündet in anderen Fluss (ULR)
							}
							else if(y > 0 && x > 0 &&
									is_connectable(terrain::RIVER, m_scene->get_terrain(x, y-1)->type()) && //Oben
									is_connectable(terrain::RIVER, m_scene->get_terrain(x-1, y)->type())) //Links
							{
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::UL)); //Kurve (terrain::UL)
							}
							else if(y < 19 && x > 0 &&
									is_connectable(terrain::RIVER, m_scene->get_terrain(x, y+1)->type()) && //Unten
									is_connectable(terrain::RIVER, m_scene->get_terrain(x-1, y)->type())) //Links
							{
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::LD)); //Kurve (LD)
							}
							else if(y > 0 && x < 29 &&
									is_connectable(terrain::RIVER, m_scene->get_terrain(x, y-1)->type()) && //Oben
									is_connectable(terrain::RIVER, m_scene->get_terrain(x+1, y)->type())) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::UR)); //Kurve (UR)
							}
							else if(y < 19 && x < 29 &&
									is_connectable(terrain::RIVER, m_scene->get_terrain(x, y+1)->type()) && //Unten
									is_connectable(terrain::RIVER, m_scene->get_terrain(x+1, y)->type())) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::DR)); //Kurve (DR)
							}
							else if((y > 0 && is_connectable(terrain::RIVER, m_scene->get_terrain(x, y-1)->type())) || //Oben
									(y < 19 && is_connectable(terrain::RIVER, m_scene->get_terrain(x, y+1)->type())))
							{
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::UD)); //Gerade (UD)
							}
							else if((x > 0 && is_connectable(terrain::RIVER, m_scene->get_terrain(x-1, y)->type())) ||
									(x < 29 && is_connectable(terrain::RIVER, m_scene->get_terrain(x+1, y)->type())))
							{
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::LR)); //Gerade (UD)
							}
							else if((y > 0 && m_scene->get_terrain(x, y-1)->type() == terrain::WATER) ||
									(y < 19 && m_scene->get_terrain(x, y+1)->type() == terrain::WATER)) //Um einzelne Flusstücke mit Flussmündungen zu verbinden
							{
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::UD)); //Gerade (UD)
							}
							else if((x > 0 && m_scene->get_terrain(x-1, y)->type() == terrain::WATER) ||
									(x < 29 && m_scene->get_terrain(x+1, y)->type() == terrain::WATER)) //Um einzelne Flusstücke mit Flussmündungen zu verbinden
							{
								this->draw(cr, x, y, get_path(terrain::RIVER, terrain::LR)); //Gerade (UD)
							}
						}
						else if(type == terrain::STREET)
						{
							if(y > 0 && y < 19 && x > 0 && x < 29 &&
									is_connectable(terrain::STREET, m_scene->get_terrain(x, y-1)->type()) && //Oben
									is_connectable(terrain::STREET, m_scene->get_terrain(x, y+1)->type()) && //Unten
									is_connectable(terrain::STREET, m_scene->get_terrain(x+1, y)->type()) && //Rechts
									is_connectable(terrain::STREET, m_scene->get_terrain(x-1, y)->type())) //Links)
									{
										this->draw(cr, x, y, get_path(terrain::STREET, terrain::ULDR)); //Kreuzung (ULDR)
									}
							else if(y > 0 && y < 19 && x > 0 &&
									is_connectable(terrain::STREET, m_scene->get_terrain(x, y-1)->type()) && //Oben
									is_connectable(terrain::STREET, m_scene->get_terrain(x, y+1)->type()) && //Unten
									is_connectable(terrain::STREET, m_scene->get_terrain(x-1, y)->type())) //Links
							{
								this->draw(cr, x, y, get_path(terrain::STREET, terrain::ULD)); //Strasse trifft auf querstrasse (terrain::ULD)
							}
							else if(y > 0 && y < 19 && x < 29 &&
									is_connectable(terrain::STREET, m_scene->get_terrain(x, y-1)->type()) && //Oben
									is_connectable(terrain::STREET, m_scene->get_terrain(x, y+1)->type()) && //Unten
									is_connectable(terrain::STREET, m_scene->get_terrain(x+1, y)->type())) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::STREET, terrain::UDR)); //Strasse trifft auf querstrasse (UDR)
							}
							else if(y < 19 && x > 0 && x < 29 &&
									is_connectable(terrain::STREET, m_scene->get_terrain(x, y+1)->type()) && //Unten
									is_connectable(terrain::STREET, m_scene->get_terrain(x-1, y)->type()) &&//Links
									is_connectable(terrain::STREET, m_scene->get_terrain(x+1, y)->type())) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::STREET, terrain::LDR)); //Strasse trifft auf querstrasse (LDR)
							}
							else if(y > 0 && x > 0 && x < 29 &&
									is_connectable(terrain::STREET, m_scene->get_terrain(x, y-1)->type()) && //Oben
									is_connectable(terrain::STREET, m_scene->get_terrain(x-1, y)->type()) &&//Links
									is_connectable(terrain::STREET, m_scene->get_terrain(x+1, y)->type())) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::STREET, terrain::ULR)); //Strasse trifft auf querstrasse (ULR)
							}
							else if(y > 0 && x > 0 &&
									is_connectable(terrain::STREET, m_scene->get_terrain(x, y-1)->type()) && //Oben
									is_connectable(terrain::STREET, m_scene->get_terrain(x-1, y)->type())) //Links
							{
								this->draw(cr, x, y, get_path(terrain::STREET, terrain::UL)); //Kurve (terrain::UL)
							}
							else if(y < 19 && x > 0 &&
									is_connectable(terrain::STREET, m_scene->get_terrain(x, y+1)->type()) && //Unten
									is_connectable(terrain::STREET, m_scene->get_terrain(x-1, y)->type())) //Links
							{
								this->draw(cr, x, y, get_path(terrain::STREET, terrain::LD)); //Kurve (LD)
							}
							else if(y > 0 && x < 29 &&
									is_connectable(terrain::STREET, m_scene->get_terrain(x, y-1)->type()) && //Oben
									is_connectable(terrain::STREET, m_scene->get_terrain(x+1, y)->type())) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::STREET, terrain::UR)); //Kurve (UR)
							}
							else if(y < 19 && x < 29 &&
									is_connectable(terrain::STREET, m_scene->get_terrain(x, y+1)->type()) && //Unten
									is_connectable(terrain::STREET, m_scene->get_terrain(x+1, y)->type())) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::STREET, terrain::DR)); //Kurve (DR)
							}
							else if((y > 0 && is_connectable(terrain::STREET, m_scene->get_terrain(x, y-1)->type())) || //Oben
									(y < 19 && is_connectable(terrain::STREET, m_scene->get_terrain(x, y+1)->type()))) //unten
							{
								this->draw(cr, x, y, get_path(terrain::STREET, terrain::UD)); //Gerade (UD)
							}
							else if((x > 0 && is_connectable(terrain::STREET, m_scene->get_terrain(x-1, y)->type())) || //Links
									(x < 29 && is_connectable(terrain::STREET, m_scene->get_terrain(x+1, y)->type()))) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::STREET, terrain::LR)); //Gerade (UD)
							}
							else
							{
								this->draw(cr, x, y, get_path(terrain::STREET, terrain::LR)); //Gerade (LR)
							}
						}
						else if(type == terrain::BRIDGE)
						{
							if((y > 0 && is_connectable(terrain::BRIDGE, m_scene->get_terrain(x, y-1)->type())) || //Oben
									(y < 19 && is_connectable(terrain::BRIDGE, m_scene->get_terrain(x, y+1)->type()))) //unten
							{
								this->draw(cr, x, y, get_path(terrain::BRIDGE, terrain::UD)); //Gerade (UD)
							}
							else if((x > 0 && is_connectable(terrain::BRIDGE, m_scene->get_terrain(x-1, y)->type())) || //Links
									(x < 29 && is_connectable(terrain::BRIDGE, m_scene->get_terrain(x+1, y)->type()))) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::BRIDGE, terrain::LR)); //Gerade (LR)
							}
						}
						else if(type == terrain::PIPE)
						{
							if(y > 0 && x > 0 &&
									is_connectable(terrain::PIPE, m_scene->get_terrain(x, y-1)->type()) && //Oben
									is_connectable(terrain::PIPE, m_scene->get_terrain(x-1, y)->type())) //Links
							{
								this->draw(cr, x, y, get_path(terrain::PIPE, terrain::UL)); //Kurve (terrain::UL)
							}
							else if(y < 19 && x > 0 &&
									is_connectable(terrain::PIPE, m_scene->get_terrain(x, y+1)->type()) && //Unten
									is_connectable(terrain::PIPE, m_scene->get_terrain(x-1, y)->type())) //Links
							{
								this->draw(cr, x, y, get_path(terrain::PIPE, terrain::LD)); //Kurve (LD)
							}
							else if(y > 0 && x < 29 &&
									is_connectable(terrain::PIPE, m_scene->get_terrain(x, y-1)->type()) && //Oben
									is_connectable(terrain::PIPE, m_scene->get_terrain(x+1, y)->type())) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::PIPE, terrain::UR)); //Kurve (UR)
							}
							else if(y < 19 && x < 29 &&
									is_connectable(terrain::PIPE, m_scene->get_terrain(x, y+1)->type()) && //Unten
									is_connectable(terrain::PIPE, m_scene->get_terrain(x+1, y)->type())) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::PIPE, terrain::DR)); //Kurve (DR)
							}
							else if(x > 0 && is_connectable(terrain::PIPE, m_scene->get_terrain(x-1, y)->type()) && //Links
									x < 29 && is_connectable(terrain::PIPE, m_scene->get_terrain(x+1, y)->type())) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::PIPE, terrain::LR)); //Gerade (LR)
							}
							else if(y > 0 && is_connectable(terrain::PIPE, m_scene->get_terrain(x, y-1)->type()) && //Oben
									y < 19 && is_connectable(terrain::PIPE, m_scene->get_terrain(x, y+1)->type())) //unten
							{
								this->draw(cr, x, y, get_path(terrain::PIPE, terrain::UD)); //Gerade (UD)
							}
							else if(y > 0 && is_connectable(terrain::PIPE, m_scene->get_terrain(x, y-1)->type())) //Oben
							{
								this->draw(cr, x, y, get_path(terrain::PIPE, terrain::U)); //Endstück (terrain::U)
							}
							else if(y < 19 && is_connectable(terrain::PIPE, m_scene->get_terrain(x, y+1)->type())) //Unten
							{
								this->draw(cr, x, y, get_path(terrain::PIPE, terrain::D)); //Endstück (terrain::D)
							}
							else if(x > 0 && is_connectable(terrain::PIPE, m_scene->get_terrain(x-1, y)->type())) //Links
							{
								this->draw(cr, x, y, get_path(terrain::PIPE, terrain::L)); //Endstück (terrain::L)
							}
							else if(x < 29 && is_connectable(terrain::PIPE, m_scene->get_terrain(x+1, y)->type())) //Links
							{
								this->draw(cr, x, y, get_path(terrain::PIPE, terrain::R)); //Endstück (terrain::R)
							}
							else
							{
								this->draw(cr, x, y, get_path(terrain::PIPE, terrain::LR)); //Wenn keine Verbindungen möglich sind (LR)
							}
						}
						else if(type == terrain::SEGMENT_PIPE)
						{
							if(x > 0 && is_connectable(terrain::SEGMENT_PIPE, m_scene->get_terrain(x-1, y)->type()) && //Links
									x < 29 && is_connectable(terrain::SEGMENT_PIPE, m_scene->get_terrain(x+1, y)->type())) //Rechts
							{
								this->draw(cr, x, y, get_path(terrain::SEGMENT_PIPE, terrain::LR)); //Gerade (LR)
							}
							else if(y > 0 && is_connectable(terrain::SEGMENT_PIPE, m_scene->get_terrain(x, y-1)->type()) && //Oben
									y < 19 && is_connectable(terrain::SEGMENT_PIPE, m_scene->get_terrain(x, y+1)->type())) //unten
							{
								this->draw(cr, x, y, get_path(terrain::SEGMENT_PIPE, terrain::UD)); //Gerade (UD)
							}
						}
						else if(type == terrain::WRECKAGE)
						{
							if((y > 0 && is_connectable(type, m_scene->get_terrain(x, y-1)->type())) ||
									(y < 19 && is_connectable(type, m_scene->get_terrain(x, y+1)->type())))
							{
								this->draw(cr, x, y, get_path(terrain::WRECKAGE, terrain::UD));
							}
							else if((x > 0 && is_connectable(type, m_scene->get_terrain(x-1, y)->type())) ||
									(x < 29 && is_connectable(type, m_scene->get_terrain(x+1, y)->type())))
							{
								this->draw(cr, x, y, get_path(terrain::WRECKAGE, terrain::LR));
							}
							else
							{
								//Trümmer
								this->draw(cr, x, y, get_path(terrain::WRECKAGE, terrain::UNDEF));
							}
						}
						else if(type == terrain::MOUNTAIN)
						{
							if(y > 0)
							{
								this->draw(cr, x, y, get_path(terrain::MOUNTAIN, terrain::UNDEF, terrain::BIG));
							}
							else
							{
								this->draw(cr, x, y, get_path(terrain::MOUNTAIN, terrain::UNDEF, terrain::SMALL));
							}
						}
						else if(type == terrain::BEACH)
						{

							if(x > 0 && terrain::is_ground(m_scene->get_terrain(x-1, y)->type()))
								this->draw(cr, x, y, get_path(terrain::BEACH, terrain::L)); //Küste links
							if(x < 29 && terrain::is_ground(m_scene->get_terrain(x+1, y)->type()))
								this->draw(cr, x, y, get_path(terrain::BEACH, terrain::R)); //Küste rechts
							if(y > 0 && terrain::is_ground(m_scene->get_terrain(x, y-1)->type()))
								this->draw(cr, x, y, get_path(terrain::BEACH, terrain::U)); //Küste oben
							if(y < 19 && terrain::is_ground(m_scene->get_terrain(x, y+1)->type()))
								this->draw(cr, x, y, get_path(terrain::BEACH, terrain::D)); //Küste unten


							if(x > 0 && terrain::is_ground(m_scene->get_terrain(x-1, y)->type()))
							{
								if(y > 0 && terrain::is_ground(m_scene->get_terrain(x, y-1)->type()))
									this->draw(cr, x, y, get_path(terrain::BEACH, terrain::UL)); //Nach innen gebogene Küste (terrain::UL)

								if(y < 19 && terrain::is_ground(m_scene->get_terrain(x, y+1)->type()))
									this->draw(cr, x, y, get_path(terrain::BEACH, terrain::LD)); //Nach innen gebogene Küste (LD)
							}

							if(x < 29 && terrain::is_ground(m_scene->get_terrain(x+1, y)->type()))
							{
								if(y > 0 && terrain::is_ground(m_scene->get_terrain(x, y-1)->type()))
									this->draw(cr, x, y, get_path(terrain::BEACH, terrain::UR)); //Nach innen gebogene Küste (UR)

								if(y < 19 && terrain::is_ground(m_scene->get_terrain(x, y+1)->type()))
									this->draw(cr, x, y, get_path(terrain::BEACH, terrain::DR)); //Nach innen gebogene Küste (DR)
							}

							if(y > 0 && y < 19 &&
									terrain::is_ground(m_scene->get_terrain(x, y-1)->type()) &&
									terrain::is_ground(m_scene->get_terrain(x, y+1)->type()))
							{
								if(x > 0 && terrain::is_ground(m_scene->get_terrain(x-1, y)->type()))
									this->draw(cr, x, y, get_path(terrain::BEACH, terrain::ULD)); //Bucht mit Strand
								else if(x < 29 && terrain::is_ground(m_scene->get_terrain(x+1, y)->type()))
									this->draw(cr, x, y, get_path(terrain::BEACH, terrain::UDR)); //Bucht mit Strand
							}

							if(x > 0 && x < 29 &&
									terrain::is_ground(m_scene->get_terrain(x-1, y)->type()) &&
									terrain::is_ground(m_scene->get_terrain(x+1, y)->type()))
							{
								if(y > 0 && terrain::is_ground(m_scene->get_terrain(x, y-1)->type()))
									this->draw(cr, x, y, get_path(terrain::BEACH, terrain::ULR)); //Bucht mit Strand
								else if(y < 19 && terrain::is_ground(m_scene->get_terrain(x, y+1)->type()))
									this->draw(cr, x, y, get_path(terrain::BEACH, terrain::LDR)); //Bucht mit Strand
							}
						}
						else if(type != terrain::WATER)
						{
							this->draw(cr, x, y, get_path(m_scene->get_terrain(x, y)->type(), terrain::UNDEF, extra));
						}

						//          if(terrain::is_building(m_scene->get_terrain(x, y)->type()))
						//          {
						//            int points = m_scene->get_terrain(x, y)->capture_points();
						//
						//            if(points < 20 && points > 0)
						//              this->draw(cr, x, y, get_path(unit::LIVE, points));
						//          }

						const unit::ptr &u = m_scene->get_unit(x, y);

						if(u)
						{
							this->draw(cr, x, y, get_path(u->type(), u->color()));

							if(u->moved())
							{
								//              Cairo::RefPtr<Cairo::SurfacePattern> sp = Cairo::SurfacePattern::create(aw::sprite().get_sprite(get_path(u->type(), u->color())));

								cr->set_source_rgba(0, 0, 0, 0.3);
								cr->mask(aw::sprite().get_sprite(get_path(u->type(), u->color())), static_cast<double>(x*16), static_cast<double>(y*16));
							}
						}
					}
				}


				//NOTE: Zweiter Durchgang könnte durch eine umgedrehte Zeichenreihenfolge vermieden werden
				for(int x = 0; x < 30; x++)
				{
					for(int y = 0; y < 20; y++)
					{
						const unit::ptr &u = m_scene->get_unit(x, y);
						if(u && !u->is_dummy())
						{
							int life = u->get_hp();
							if(life < 10 && life > 0)
								this->draw(cr, x, y, get_path(unit::LIVE, life));

							if(u->low_ammo())
								this->draw(cr, x, y, get_path(unit::LOW_AMMO));

							if(u->low_fuel())
								this->draw(cr, x, y, get_path(unit::LOW_FUEL));

							if(u->is_hidden())
								this->draw(cr, x, y, get_path(unit::HIDDEN));

							if(u->is_transporter() && boost::dynamic_pointer_cast<transporter>(u)->loaded_units_count() > 0)
								this->draw(cr, x, y, get_path(unit::LOADED));

							if(u->can_capture())
							{
								const terrain::ptr &t(m_scene->get_terrain(x, y));

								if(t->is_building() && boost::dynamic_pointer_cast<building>(t)->capture_points() < 20)
								{
									this->draw(cr, x, y, get_path(unit::CAPTURE));
								}
							}
						}

						static const std::string range(aw::config().get<std::string>("/config/dirs/pixmaps") + "/misc/range.png");
						static const std::string path(aw::config().get<std::string>("/config/dirs/pixmaps") + "/misc/path.png");

						if(m_scene->highlighted(x, y))
							this->draw(cr, x, y, range);

						if(m_scene->path(x, y))
							this->draw(cr, x, y, path);
					}
				}

				if(m_scene->highlighted_unit())
				{
					const coord &hu = m_scene->highlighted_unit();

					cr->save();
					cr->set_source_rgba(0.5, 0.5, 0.5, 0.75);
					cr->mask(aw::sprite().get_sprite(aw::config().get<std::string>("/config/dirs/pixmaps") + "/misc/range.png"), static_cast<double>(hu.x*16), static_cast<double>(hu.y*16));
					cr->restore();
				}
			}
		}
	}
}
