#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include "partie.h"

struct gui_nouvelle_donne_data {
	struct partie ** partie;
	GtkTreeView * view;
	GtkWindow* parent;
};

struct gui_nouvelle_donne_creee_data {
	GtkTreeView* view;
	struct partie ** partie;
	GtkWidget * parent;
	GtkBuilder * builder;
};

struct gui_nouvelle_partie_data {
	struct partie ** partie;
	GtkWidget * view;
};

struct gui_nouvelle_partie_creee_data {
	struct partie ** partie;
	GtkBuilder * builder;
	GtkWidget * view;
};

enum enchere get_enchere(GtkBuilder * builder) {
	GObject * combo = gtk_builder_get_object(builder, "enchere");
	gint active = gtk_combo_box_get_active(GTK_COMBO_BOX(combo));
	switch(active) {
		case 0: return PRISE;
		case 1: return GARDE;
		case 2: return GARDE_SANS;
		case 3: return GARDE_CONTRE;
		default: {
				 fprintf(stderr, "Erreur d'enchere");
				 return PRISE;
			 }
	}
}

enum prime_multipliable get_prime_multipliable(GtkBuilder * builder) {
	GObject * combo = gtk_builder_get_object(builder, "petit");
	gint active = gtk_combo_box_get_active(GTK_COMBO_BOX(combo));
	switch(active) {
		case 0: return RIEN;
		case 1: return PETIT_AU_BOUT_ATTAQUE;
		case 2: return PETIT_AU_BOUT_DEFENSE;
		default: {
				 fprintf(stderr, "Erreur de prime");
				 return RIEN;
			 }
	}
}

int get_primes_non_multipliables(GtkBuilder * builder, enum prime_non_multipliable ** primes) {
	GObject * combo_poignee = gtk_builder_get_object(builder, "poignee");
	GObject * combo_chelem = gtk_builder_get_object(builder, "chelem");
	gint active_poignee = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_poignee));
	gint active_chelem = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_chelem));

	int nb_primes = 0;
	if(active_poignee != 0) 
		nb_primes++;
	if(active_chelem != 0)
		nb_primes++;

	if(nb_primes != 0){
		size_t i = 0;
		*primes = malloc(nb_primes * sizeof(enum prime_non_multipliable));
		switch(active_poignee) {
			case 1: *primes[i] = SIMPLE_POIGNEE;
				i++;
				break;
			case 2: *primes[i] = DOUBLE_POIGNEE;
				i++;
				break;
			case 3: *primes[i] = TRIPLE_POIGNEE;
				i++;
		}

		switch(active_chelem) {
			case 1: *primes[i] = CHELEM_ANNONCE_PASSE;
				break;
			case 2: *primes[i] = CHELEM_ANNONCE_CHUTE;
				break;
			case 3: *primes[i] = CHELEM_NON_ANNONCE;
		}
	} else {
		primes = NULL;
	}
	return nb_primes;
}

int get_nb_bouts(GtkBuilder * builder) {
	return 0;
}

int get_score(GtkBuilder * builder) {
	return 0;
}

int get_preneur(GtkBuilder * builder){
	return 0;
}

void gui_nouvelle_donne_creee(struct gui_nouvelle_donne_creee_data *data) {
	struct partie * partie = *(data->partie);
	struct donne * donne = malloc(sizeof(struct donne));
	donne->enchere = get_enchere(data->builder);
	donne->prime = get_prime_multipliable(data->builder);
	donne->nb_primes = get_primes_non_multipliables(data->builder, &(donne->primes));
	donne->nb_bouts = get_nb_bouts(data->builder);
	donne->score = get_score(data->builder);
	donne->preneur = get_preneur(data->builder);
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

	gtk_widget_destroy(data->parent);
}

void gui_nouvelle_donne(const struct gui_nouvelle_donne_data * data) {
	struct partie * partie = *(data->partie);
	if(partie == NULL) {
		GtkWidget * dialog = gtk_message_dialog_new(data->parent, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "La partie n'a pas été créée.");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	} else {
		GtkBuilder *builder;
		GObject * button;
		GObject * window;

		builder = gtk_builder_new();
		gtk_builder_add_from_file(builder, "ecran_nouvelle_donne.ui", NULL);
		button = gtk_builder_get_object(builder, "button_valider_nouvelle_donne");
		window = gtk_builder_get_object(builder, "window");

		struct gui_nouvelle_donne_creee_data *n_d_c_data = malloc(sizeof(struct gui_nouvelle_donne_creee_data));
		if(n_d_c_data == NULL) {
			fprintf(stderr, "Erreur d'allocation mémoire");
		} else {
			n_d_c_data->view = data->view;
			n_d_c_data->partie = data->partie;
			n_d_c_data->parent = GTK_WIDGET(window);
			n_d_c_data->builder = builder;
			g_signal_connect_swapped(button, "clicked", G_CALLBACK(gui_nouvelle_donne_creee), n_d_c_data);
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

void gui_nouvelle_partie_creee(struct gui_nouvelle_partie_creee_data * data) {
	struct partie * partie = *(data->partie);
	GtkBuilder *builder = data->builder;
	GtkEntry *entry;
	char* nom = NULL;
	set_nb_joueurs(partie, 4);

	entry = GTK_ENTRY(gtk_builder_get_object(builder, "nom1"));
	nom = malloc(sizeof(char) * (1 + gtk_entry_get_text_length(entry)));
	if(nom == NULL) {
		fprintf(stderr, "Erreur d'allocation mémoire");
	}
	strcpy(nom, gtk_entry_get_text(entry));
	set_nom_joueur(partie, nom, 0);

	entry = GTK_ENTRY(gtk_builder_get_object(builder, "nom2"));
	nom = malloc(sizeof(char) * (1 + gtk_entry_get_text_length(entry)));
	if(nom == NULL) {
		fprintf(stderr, "Erreur d'allocation mémoire");
	}
	strcpy(nom, gtk_entry_get_text(entry));
	set_nom_joueur(partie, nom, 1);

	entry = GTK_ENTRY(gtk_builder_get_object(builder, "nom3"));
	nom = malloc(sizeof(char) * (1 + gtk_entry_get_text_length(entry)));
	if(nom == NULL) {
		fprintf(stderr, "Erreur d'allocation mémoire");
	}
	strcpy(nom, gtk_entry_get_text(entry));
	set_nom_joueur(partie, nom, 2);

	entry = GTK_ENTRY(gtk_builder_get_object(builder, "nom4"));
	nom = malloc(sizeof(char) * (1 + gtk_entry_get_text_length(entry)));
	if(nom == NULL) {
		fprintf(stderr, "Erreur d'allocation mémoire");
	}
	strcpy(nom, gtk_entry_get_text(entry));
	set_nom_joueur(partie, nom, 3);

	afficher_score(data->view, partie);

	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	gtk_widget_destroy(window);
	free(data);
}

void gui_nouvelle_partie(struct gui_nouvelle_partie_data *data) {
	struct partie * partie = *(data->partie);
	desallouer_partie(partie);

	partie = nouvelle_partie();

	*(data->partie) = partie;

	GtkBuilder * builder;
	GObject *button;
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "ecran_nouvelle_partie.ui", NULL);

	button = gtk_builder_get_object(builder, "button_valider_nouvelle_partie");

	struct gui_nouvelle_partie_creee_data *n_p_c_data = malloc(sizeof(struct gui_nouvelle_partie_creee_data));
	n_p_c_data->partie = data->partie;
	n_p_c_data->builder = builder;
	n_p_c_data->view = data->view;

	g_signal_connect_swapped(button, "clicked", G_CALLBACK (gui_nouvelle_partie_creee), n_p_c_data);
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
	gtk_builder_add_from_file(builder, "ecran_principal.ui", NULL);

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
