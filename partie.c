#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "partie.h"

int valeur_donne(const struct donne donne)
{
	int nb_points;
	int passe;
	int valeur;
	switch(donne.nb_bouts) {
		case 0: nb_points = 56;
			break;
		case 1: nb_points = 51;
			break;
		case 2: nb_points = 41;
			break;
		case 3: nb_points = 36;
			break;
		default: 
			nb_points = 0;
	}
	if(nb_points > donne.score) {
		passe = -1;
	} else {
		passe = 1;
	}

	valeur = ((donne.score - nb_points + donne.prime)*passe + 25)*donne.enchere;

	{
		size_t i = 0;
		for(; i < donne.nb_primes; i++)
		{
			valeur += donne.primes[i];
		}
	}

	return valeur*passe;
}

void print_partie(const struct partie partie)
{
	int scores[4] = {0, 0, 0, 0};
	size_t i = 0;
	printf("%s\t%s\t%s\t%s\n", partie.noms[0], partie.noms[1], partie.noms[2], partie.noms[3]);
	struct donne * donne = partie.donnes;
	while(donne != NULL)
	{
		size_t j = 0;
		int valeur = valeur_donne(partie.donnes[i]);
		for(; j < 4 ;j++)
			if(j == partie.donnes[i].preneur)
			{
				scores[j] += 3*valeur;
				printf("%i\t", 3*valeur);
			} else {
				scores[j] -= valeur;
				printf("%i\t", -1*valeur);
			}
		printf("\n");
		donne = donne->donne_suivante;
	}

	printf("\n%i\t%i\t%i\t%i\n", scores[0], scores[1], scores[2], scores[3]);
}

void set_nb_joueurs(struct partie *partie, unsigned char nb_joueurs) {
	if(nb_joueurs < 3 || nb_joueurs > 5) {
		fprintf(stderr, "Le nombre de joueurs doit être compris entre 3 et 5 (inclus): %i", nb_joueurs);
		return;
	}
	partie->nb_joueurs = nb_joueurs;
	partie->noms = malloc(nb_joueurs * sizeof(char *));

	if(partie->noms == NULL) {
		fprintf(stderr, "L'allocation de mémoire pour les noms de joureurs a échoué.");
	}
}

void set_nom_joueur(struct partie *partie, char *nom, size_t index_joueur) {
	if(index_joueur >= partie->nb_joueurs) {
		fprintf(stderr, "L'index joueur (%i) doit être inferieur au nombre de joueurs (%i).", (int)index_joueur, partie->nb_joueurs);
		return;
	}
	partie->noms[index_joueur] = nom;
}

void nouvelle_donne(struct partie * partie, struct donne *donne) {
	if(partie->donnes == NULL) { // Cas 1ère donne
		partie->donnes = donne;
		partie->derniere_donne = donne;
	} else {
		partie->derniere_donne->donne_suivante = donne;
		partie->derniere_donne = donne;
	}
	partie->derniere_donne->donne_suivante = NULL;
}

struct partie * nouvelle_partie(void) {
	struct partie * partie = malloc(sizeof(struct partie));
	if(partie == NULL) {
		fprintf(stderr, "L'allocation de mémoire pour la partie a échoué.");
		return NULL;
	}
	partie->nb_joueurs = 0;
	partie->donnes = NULL;
	partie->derniere_donne = NULL;
	partie->noms = NULL;

	return partie;
}

void desallouer_partie (struct partie * partie) {
	if(partie != NULL) {
		struct donne * donne = partie->donnes;
		struct donne * n_donne = NULL;
		while(donne != NULL){
			n_donne = donne->donne_suivante;
			free(donne);
			donne = n_donne;
		}
		if(partie->noms != NULL) {
				free(partie->noms);
		}
		free(partie);
	}
}
