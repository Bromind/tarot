#include <gtk/gtk.h>
#include <stdlib.h>
#include "partie.h"

struct gui_nouvelle_donne_data {
	struct partie ** partie;
	GtkTreeView * view;
	GtkWindow * parent;
};

struct gui_nouvelle_partie_data {
	struct partie ** partie;
	GtkWidget * view;
};

void gui_nouvelle_donne(const struct gui_nouvelle_donne_data * data) {
	struct partie * partie = *(data->partie);
	if(partie == NULL) {
		GtkWidget * dialog = gtk_message_dialog_new(data->parent, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "La partie n'a pas été créée.");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	} else {
		struct donne * donne = malloc(sizeof(struct donne));
		donne->enchere = PRISE;
		donne->prime = RIEN;
		donne->primes = NULL;
		donne->nb_primes = 0;
		donne->nb_bouts = 0;
		donne->score = 0;
		donne->preneur = 0;
		donne->donne_suivante = NULL;

		nouvelle_donne(partie, donne);

		GtkListStore * model = GTK_LIST_STORE(gtk_tree_view_get_model(data->view));
		GtkTreeIter iter;
		gtk_list_store_append(model, &iter);
		
		int i;
		int valeur = valeur_donne(*donne);
		for(i = 0; i < partie->nb_joueurs; i++) {
			gint points_joueurs;
			if(i == donne->preneur) {
				points_joueurs = 3*valeur;
			} else {
				points_joueurs = -1*valeur;
			}
			gtk_list_store_set(model, &iter, i, points_joueurs, -1);
		}
	}
}

void afficher_score(GtkWidget *view, const struct partie *partie) {

	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;

	GType * column_types = malloc(partie->nb_joueurs * sizeof(GType));
	int i;
	for(i = 0; i < partie->nb_joueurs; i++)
	{
		col = gtk_tree_view_column_new();
		gtk_tree_view_column_set_title(col, partie->noms[i]);
		gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

		renderer = gtk_cell_renderer_text_new();

		gtk_tree_view_column_pack_start(col, renderer, TRUE);
		gtk_tree_view_column_add_attribute(col, renderer, "text", i);
		column_types[i] = G_TYPE_INT;
	}


	GtkListStore *list_model;
	list_model = gtk_list_store_newv(4, column_types);
	gtk_tree_view_set_model(GTK_TREE_VIEW(view), GTK_TREE_MODEL(list_model));
	g_object_unref(list_model);
}

void gui_nouvelle_partie(struct gui_nouvelle_partie_data *data) {
	struct partie * partie = *(data->partie);
	g_print("Créé une nouvelle partie\n");
	desallouer_partie(partie);

	partie = nouvelle_partie();

	*(data->partie) = partie;

	set_nb_joueurs(partie, 4);
	set_nom_joueur(partie, "nom1", 0);
	set_nom_joueur(partie, "nom2", 1);
	set_nom_joueur(partie, "nom3", 2);
	set_nom_joueur(partie, "nom4", 3);

	afficher_score(data->view, partie);
}

int main (int argc, char **argv)
{
	GtkBuilder *builder;
	GObject *window;
	GObject *button;
	GObject *treeview;

	struct partie * partie = NULL;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new ();
	gtk_builder_add_from_file(builder, "builder.ui", NULL);

	window = gtk_builder_get_object(builder, "window");
	g_signal_connect(window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	treeview = gtk_builder_get_object(builder, "treeview");

	struct gui_nouvelle_partie_data n_p_data = {
		.partie = &partie,
		.view = GTK_WIDGET(treeview),
	};

	button = gtk_builder_get_object(builder, "button_nouvelle_partie");
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gui_nouvelle_partie), &n_p_data);

	struct gui_nouvelle_donne_data n_d_data = {
		.partie = &partie,
		.view = GTK_TREE_VIEW(treeview),
		.parent = GTK_WINDOW(window),
	};

	button = gtk_builder_get_object(builder, "button_nouvelle_donne");
	g_signal_connect_swapped(button, "clicked", G_CALLBACK (gui_nouvelle_donne), &n_d_data);

	button = gtk_builder_get_object(builder, "button_quitter");
	g_signal_connect(button, "clicked", G_CALLBACK (gtk_main_quit), NULL);

	gtk_main();

	return 0;
}
