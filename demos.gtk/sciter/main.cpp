#include <stdlib.h>
#include <gtk/gtk.h>
#include <cairo.h>

#include "sciter-x.h"

#if 1

static void do_drawing(cairo_t *);


static void tran_setup(GtkWidget *win)
{
  GdkScreen *screen;
  GdkVisual *visual;

  gtk_widget_set_app_paintable(win, TRUE);
  screen = gdk_screen_get_default();
  visual = gdk_screen_get_rgba_visual(screen);

  if (visual != NULL && gdk_screen_is_composited(screen)) {
      gtk_widget_set_visual(win, visual);
  }
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr,
    gpointer user_data)
{
  do_drawing(cr);

  return FALSE;
}

static void do_drawing(cairo_t *cr)
{
  cairo_set_source_rgba(cr, 0.2, 0.2, 0.2, 0.4);
  cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);
}

int main (int argc, char *argv[])
{
  //GtkWidget *window;
  //GtkWidget *darea;

  gtk_init(&argc, &argv);

  //window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  //tran_setup(window);
  //darea = gtk_drawing_area_new();

  RECT frame; frame.left = 0; frame.top = 0; frame.right = 300; frame.bottom = 150;
  GtkWidget *sciter = SAPI()->SciterCreateWindow(0,&frame, nullptr, nullptr, nullptr);

  GtkWidget *window = gtk_widget_get_toplevel (sciter);
  //tran_setup(window);

  //gtk_container_add(GTK_CONTAINER (window), darea);

  //g_signal_connect(G_OBJECT(darea), "draw",
  //    G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  //gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);
  //gtk_window_set_title(GTK_WINDOW(window), "Transparent window");

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}

#else

static void helloWorld (GtkWidget *wid, GtkWidget *win)
{
  GtkWidget *dialog = NULL;

  dialog = gtk_message_dialog_new (GTK_WINDOW (win), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "Hello World!");
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

int main (int argc, char *argv[])
{
  GtkWidget *button = NULL;
  GtkWidget *win = NULL;
  GtkWidget *vbox = NULL;

  /* Initialize GTK+ */
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
  gtk_init (&argc, &argv);
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

  /* Create the main window */
  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (win), 8);
  gtk_window_set_title (GTK_WINDOW (win), "Hello World");
  gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);
  gtk_widget_realize (win);
  g_signal_connect (win, "destroy", gtk_main_quit, NULL);

  /* Create a vertical box with buttons */
  vbox = gtk_vbox_new (TRUE, 6);
  gtk_container_add (GTK_CONTAINER (win), vbox);

  button = gtk_button_new_from_stock (GTK_STOCK_DIALOG_INFO);
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (helloWorld), (gpointer) win);
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

  button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  g_signal_connect (button, "clicked", gtk_main_quit, NULL);
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

  /* Enter the main loop */
  gtk_widget_show_all (win);
  gtk_main ();
  return 0;
}
#endif
