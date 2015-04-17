/*用g_signal_connect()绑定数据到callback

gtk.org给的教程里，传给callback的是一个字符串常量，我这里改用一个gint32 *型的指针。
*/
#include <gtk/gtk.h>

static gint32 count = 0;
static void callback(GtkWidget *widget, gpointer msg)
{
    (*(gint32 *)msg)++;
    g_print("callback here - the data is: %d\n", *(gint32 *)msg);
}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    g_print("delete event occurred\n");
    return FALSE;
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
    button = gtk_button_new_with_label ("Hello World");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(callback), (gpointer)&count);

    gtk_container_add(GTK_CONTAINER(window), button);

    gtk_widget_show(button);
    gtk_widget_show(window);

    gtk_main();
    return 0;
} 

