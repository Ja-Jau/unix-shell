/*
Simple calculator using GTK library
Test-project for GTK and shell :)
Still in development

Compile:
gcc calc.c -o calc `pkg-config --cflags --libs gtk+-3.0`
*/

#include <gtk/gtk.h>

GtkWidget *entry;


double eval(const char *expr);

void on_click(GtkButton *button, gpointer user_data) {
    const gchar *text = gtk_button_get_label(button);
    const gchar *current = gtk_entry_get_text(GTK_ENTRY(entry));
    gchar *new_text = g_strconcat(current, text, NULL);
    gtk_entry_set_text(GTK_ENTRY(entry), new_text);
    g_free(new_text);
}

void on_clear(GtkButton *button, gpointer user_data) {
    gtk_entry_set_text(GTK_ENTRY(entry), "");
}

void on_equals(GtkButton *button, gpointer user_data) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    double result = eval(text);

    char result_str[128];
    snprintf(result_str, sizeof(result_str), "%g", result);
    gtk_entry_set_text(GTK_ENTRY(entry), result_str);
}

GtkWidget* create_button(const gchar *label, GCallback callback) {
    GtkWidget *button = gtk_button_new_with_label(label);
    g_signal_connect(button, "clicked", callback, NULL);
    return button;
}

double eval(const char *expr) {
    double result = 0;
    double current = 0;
    char op = '+';
    int offset = 0;

    while (*expr) {
        if (*expr == ' ') {
            expr++;
            continue;
        }

        if (sscanf(expr, "%lf%n", &current, &offset) == 1) {
            switch (op) {
                case '+': result += current; break;
                case '-': result -= current; break;
                case '*': result *= current; break;
                case '/':
                    if (current == 0) {
                        g_warning("Illegal move.. (div by 0)");
                        return 0;
                    }
                    result /= current;
                    break;
            }

            expr += offset;
        } else if (*expr == '+' || *expr == '-' || *expr == '*' || *expr == '/') {
            op = *expr;
            expr++;
        } else {
            expr++;
        }
    }

    return result;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calc :)");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 4, 1);

    // GUI

    // 1
    gtk_grid_attach(GTK_GRID(grid), create_button("1", G_CALLBACK(on_click)), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_button("2", G_CALLBACK(on_click)), 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_button("3", G_CALLBACK(on_click)), 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_button("DEL", G_CALLBACK(on_clear)), 3, 1, 1, 1);

    // 2
    gtk_grid_attach(GTK_GRID(grid), create_button("4", G_CALLBACK(on_click)), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_button("5", G_CALLBACK(on_click)), 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_button("6", G_CALLBACK(on_click)), 2, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_button("=", G_CALLBACK(on_equals)), 3, 2, 1, 1);

    // 3
    gtk_grid_attach(GTK_GRID(grid), create_button("7", G_CALLBACK(on_click)), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_button("8", G_CALLBACK(on_click)), 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_button("9", G_CALLBACK(on_click)), 2, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_button("0", G_CALLBACK(on_click)), 3, 3, 1, 1);

    // 4
    gtk_grid_attach(GTK_GRID(grid), create_button("+", G_CALLBACK(on_click)), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_button("-", G_CALLBACK(on_click)), 1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_button("*", G_CALLBACK(on_click)), 2, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_button("/", G_CALLBACK(on_click)), 3, 4, 1, 1);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}