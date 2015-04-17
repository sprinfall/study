/* 
 * try the usage of these functions:
 * g_signal_connect
 * g_signal_handler_disconnect
 * g_signal_handler_block
 * g_signal_handler_unblock
 * g_signal_handlers_block_by_func
 * g_signal_handlers_unblock_by_func
 */

#include <gtk/gtk.h>

static void hello(GtkWidget *widget, gpointer data)
{
    g_print("Hello World\n");
}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    g_print("delete event occurred\n");
    return FALSE; /* FALSE: the main window will be destroyed */
}

static void destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *button;

    gtk_init(&argc, &argv);

    /* window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(delete_event), NULL);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    /* button */
    button = gtk_button_new_with_label("Hello World");
    gulong cbhello = g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(hello), NULL);

    /* TEST signal handler disconnet */
    g_signal_handler_disconnect(G_OBJECT(button), cbhello);

    /* TEST signal handler block & unblock */
    g_signal_handler_block(G_OBJECT(button), cbhello);
    g_signal_handler_unblock(G_OBJECT(button), cbhello);

    /* TEST signal handlers block_by_func & unblock_by_func */
    g_signal_handlers_block_by_func(G_OBJECT(button), G_CALLBACK(hello), NULL);
    g_signal_handlers_unblock_by_func(G_OBJECT(button), G_CALLBACK(hello), NULL);

    gtk_container_add(GTK_CONTAINER(window), button);
    gtk_widget_show(button);
    gtk_widget_show(window);

    gtk_main();
    return 0;
} 

