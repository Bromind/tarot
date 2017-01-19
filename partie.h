
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
	struct donne * donne_suivante;
};

struct partie {
	unsigned char nb_joueurs;
	struct donne * donnes;
	struct donne * derniere_donne;
	char** noms;
};

int valeur_donne(const struct donne donne);
struct partie * nouvelle_partie(void);
void desallouer_partie(struct partie * partie);
void nouvelle_donne(struct partie *partie, struct donne *donne);
void set_nb_joueurs(struct partie *partie, unsigned char nb_joueurs);
void set_nom_joueur(struct partie *partie, char* nom, size_t index_joueur);
