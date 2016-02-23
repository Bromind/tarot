#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <cdk.h>


enum enchere {
	PRISE = 1, 
	GARDE = 2,
	GARDE_SANS = 4,
	GARDE_CONTRE = 6,
};

enum prime_multipliable {
	RIEN = 0,
	PETIT_AU_BOUT_ATTAQUE = 10,
	PETIT_AU_BOUT_DEFENSE = -10,
};

enum prime_non_multipliable {
	SIMPLE_POIGNEE = 20,
	DOUBLE_POIGNEE = 30,
	TRIPLE_POIGNEE = 40,
	CHELEM_ANNONCE_PASSE = 400,
	CHELEM_ANNONCE_CHUTE = -200,
	CHELEM_NON_ANNONCE = 200,
};

struct donne {
	enum enchere enchere;
	enum prime_multipliable prime;
	enum prime_non_multipliable *primes;
	size_t nb_primes;
	size_t nb_bouts;
	size_t score;
	size_t preneur;
};

struct partie {
	size_t nb_donnes;
	struct donne * donnes;
	char* noms[4];
};

WINDOW* initCurses(void){
	WINDOW* w = initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	return w;
}

void closeCurses(void) {
	endwin();
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);
	wrefresh(local_win);

	return local_win;
}

void destroy_win(WINDOW *local_win)
{	
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}

int valeur_donne(struct donne donne)
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

void print_partie(struct partie partie, WINDOW *w)
{
	int scores[4] = {0, 0, 0, 0};
	size_t i = 0;
	wprintw(w, "%s\t%s\t%s\t%s\n", partie.noms[0], partie.noms[1], partie.noms[2], partie.noms[3]);
	for(; i < partie.nb_donnes ; i++)
	{
		size_t j = 0;
		int valeur = valeur_donne(partie.donnes[i]);
		for(; j < 4 ;j++)
			if(j == partie.donnes[i].preneur)
			{
				scores[j] += 3*valeur;
			} else {
				scores[j] -= valeur;
			}
		wprintw(w, "%i\t%i\t%i\t%i\n", scores[0], scores[1], scores[2], scores[3]);
	}
	wrefresh(w);
}


int main(void)
{
	enum prime_non_multipliable primes[1] = {SIMPLE_POIGNEE};

	char* noms1 = "Val";
	char* noms2 = "Hector";
	char* noms3 = "Arnaud";
	char* noms4 = "Moi";

	struct donne donne = {
		.enchere = PRISE,
		.primes = primes,
		.prime = PETIT_AU_BOUT_ATTAQUE,
		.nb_primes = 1,
		.nb_bouts = 3,
		.score = 50,
		.preneur = 0,
	};

	struct partie partie = {
		.nb_donnes = 1,
		.donnes = &donne,
		.noms = {noms1, noms2, noms3, noms4},
	};


	WINDOW *w = initCurses();
	//print_partie(partie, w);


	CDKSCREEN *cdk_screen = initCDKScreen(w);



	char ch = getch();
	while(ch != 'q')
	{
		ch = getch();
	}
	closeCurses();

	return 0;
}

