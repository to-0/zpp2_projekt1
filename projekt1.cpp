// projekt1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define N 4
#include <stdlib.h>
#include <string.h>
#include<stdio.h>
#include <ctype.h>
int kontrola_miesta(int n, int* pole, int dlzka) { //vypisem kolko volneho miesta mi zostava v poli pre pismeno
	int i = 0;
	while (*(pole + i) >= 0 && i < dlzka) { //na zaciatku je pole indexov pre pismeno vzdy nastavene na -1
		//printf("Toto je hodnota prvku pola i %d\n", pole[i]);
		i++;
	}
	return i - 1; //vrati index posledneho prvku ktory je vyplneny

}

void nacitaj(char** pole, int m, int n, FILE* f, int* index[], int dlzky[]) { //nacitavam aj pismena aj index a aj dlzky indexu
	char c;
	for (int i = 0; i < m; i++) { //riadok
		for (int j = 0; j < n; j++) { //stlpec
			c = getc(f);
			//if (c == '\n') c = getc(f); // ak nahoodu precitam novy riadok tak preskocim 
			 //priradim polu pismenko ktore som preictal
			while (c<'A' || c>'Z') c = getc(f);
			if (c != EOF) pole[i][j] = c;
			if (dlzky[c - 'A'] == 0) { //ak tam este neni ziadny vyskyt alokujem tomu N miest
				if (dlzky[c - 'A'] == 0) { //ak sme to prvu krat nasli
					if ((index[c - 'A'] = (int*)malloc(N * sizeof(int))) == NULL) {
						exit(1);
					}
					index[c - 'A'][0] = i; //dam na prve miesto poziciu riadku
					index[c - 'A'][1] = j; //druhe miesto pozicia stlpca pismenka
					dlzky[c - 'A'] = N; //zvacsim dlzku index[x]
					for (int t = 2; t < N; t++) index[c - 'A'][t] = -1;
				}
			}
			else {
				int posledny_prvok = kontrola_miesta(dlzky[c - 'A'], index[c - 'A'], dlzky[c - 'A']); //vrati poziciu posledneho nenuloveho prvku
				if (posledny_prvok + 2 >= dlzky[c - 'A']) { //ak sa nam tam uz nezmestia tie 2 prvky 
					index[c - 'A'] = (int*)realloc(index[c - 'A'], (dlzky[c - 'A'] + N) * sizeof(int));//zvacsim pole indexov[x] 
					if (index[c - 'A'] == NULL) exit(1);
					dlzky[c - 'A'] += N; //zvacsim dlzku
				}
				if (index[c - 'A'] != NULL) {
					index[c - 'A'][posledny_prvok + 1] = i; //dam to na poziciu posledny+1 (posledny je posledny nezaporny cize eeste nejaka pozicia)
					index[c - 'A'][posledny_prvok + 2] = j; //posledny+2
					for (int t = posledny_prvok + 2 + 1; t < dlzky[c - 'A']; t++) index[c - 'A'][t] = -1;
				}
			}

		}
	}
}

void vytvor_index(int* index[], int dlzky[]) { //vytvorim pole index a vynulujem jeho hodnoty, to iste spravim pre pole
	//dlzky, kde si ukladam dlzky indexu
	for (int i = 0; i < 26; i++) {
		index[i] = NULL;
		dlzky[i] = 0;
	}

}
void vypis(char** pole, int m, int n) { //vypisem sucasny stav osemsmerovky
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			putchar(pole[i][j]);
		}
		putchar('\n');
	}
	putchar('\n');
}
void vyskrtni(char** pole, int smer, int i, int j, int dlzka) {
	int kj = 0, ki = 0;
	switch (smer) { //musim zistit ktory smer idem vyskrtavat
	case 1: //smer dolu;
		ki = 1;
		break;
	case 2: //doprava
		kj = 1;
		break;
	case 3: //hore
		ki = -1;
		break;
	case 4: //dolava
		kj = -1;
		break;
	case 5:
		ki = 1;
		kj = 1;
		break;
	case 6:
		ki = 1;
		kj = -1;
		break;
	case 7:
		ki = -1;
		kj = 1;
		break;
	case 8:
		ki = -1;
		kj = -1;
		break;
	}
	pole[i][j] = tolower(pole[i][j]);
	for (int k = 1; k < dlzka; k++) {
		pole[i + ki][j + kj] = tolower(pole[i + ki][j + kj]);
		i += ki;
		j += kj;
	}
}
int najdi_slovo(char** pole, int* index[], char slovo[], int m, int n, int dlzky[]) { //m pocet riadkov pola n pocet stlpcov
	int dlzka = strlen(slovo), pom; //pom mi pomaha pri urcovani ci uz som v nejakom smere urcil 
	int* r = index[slovo[0] - 'A'];
	int* s = index[slovo[0] - 'A'];
	s++;
	int nasiel = 0; //ci som nasiel alebo nenasiel slovo aspon raz
	int nasiel_v_smere=0;//pomocna premenna na vyskrtavanie viac krat
	int i = 1;
	int pocet_moznych_vyskytov_pismena = dlzky[slovo[0] - 'A'] / 2; //kolko krat tu to pismeno moze byt, vzdy delim 2 lebo davam 2 suradnice
	int posuny = 0;
	while (r != NULL && s != NULL && posuny<pocet_moznych_vyskytov_pismena 
		&& *r>-1 && *s>-1) {
		i = 1;
		nasiel_v_smere = 0;
		int ki = *r, kj = *s;
		//smer dole
		while (ki + 1 < m && i < dlzka && toupper(pole[ki + 1][kj]) == slovo[i]) { //to upper aby nahodou ked uz je to pismenko vyskrtnute aby sme ho mohli pouzit 
			i++;
			ki++;
			pom = 1;
		}
		if (i < dlzka) {
			ki = *r, kj = *s;
			i = 1;
		}
		else {
			if (nasiel_v_smere == 0) { //este som nenasiel slovo v nejakom smere
				vyskrtni(pole, pom, *r, *s, dlzka);
				nasiel = 1;
				nasiel_v_smere = 1;
			}
			//return 1;
		}
		//smer doprava
		while (kj + 1 < n && i < dlzka && toupper(pole[ki][kj + 1]) == slovo[i]) {
			i++;
			kj++;
			pom = 2;
		}
		if (i < dlzka) {
			ki = *r, kj = *s;
			i = 1;
		}
		else {
			if (nasiel_v_smere == 0) { //este som nenasiel slovo v nejakom smere
				vyskrtni(pole, pom, *r, *s, dlzka);
				nasiel = 1;
				nasiel_v_smere = 1;
			}
			//return 1;
		}
		//hore
		while (ki - 1 >= 0 && i < dlzka && toupper(pole[ki - 1][kj]) == slovo[i]) {
			i++;
			ki--;
			pom = 3;
		}
		if (i < dlzka) {
			ki = *r, kj = *s;
			i = 1;
		}
		else {
			if (nasiel_v_smere == 0) { //este som nenasiel slovo v nejakom smere
				vyskrtni(pole, pom, *r, *s, dlzka);
				nasiel = 1;
				nasiel_v_smere = 1;
			}
			//return 1;
		}
		//dolava
		while (kj - 1 >= 0 && i < dlzka && toupper(pole[ki][kj - 1]) == slovo[i]) {
			kj--;
			i++;
			pom = 4;
		}
		if (i < dlzka) {
			ki = *r, kj = *s;
			i = 1;
		}
		else {
			if (nasiel_v_smere == 0) { //este som nenasiel slovo v nejakom smere
				vyskrtni(pole, pom, *r, *s, dlzka);
				nasiel = 1;
				nasiel_v_smere = 1;
			}
			//return 1;
		}
		// doprava dole
		while (kj + 1 < n && ki + 1 < m && i < dlzka && toupper(pole[ki + 1][kj + 1]) == slovo[i]) {
			i++;
			kj++;
			ki++;
			pom = 5;
		}
		if (i < dlzka) {
			ki = *r, kj = *s;
			i = 1;
		}
		else {
			if (nasiel_v_smere == 0) { //este som nenasiel slovo v nejakom smere
				vyskrtni(pole, pom, *r, *s, dlzka);
				nasiel = 1;
				nasiel_v_smere = 1;
			}
			//return 1;
		}
		//dole dolava
		while (kj - 1 >= 0 && ki + 1 < m && i < dlzka && toupper(pole[ki + 1][kj - 1]) == slovo[i]) {
			i++;
			kj--;
			ki++;
			pom = 6;
		}
		if (i < dlzka) {
			ki = *r, kj = *s;
			i = 1;
		}
		else {
			if (nasiel_v_smere == 0) { //este som nenasiel slovo v nejakom smere
				vyskrtni(pole, pom, *r, *s, dlzka);
				nasiel = 1;
				nasiel_v_smere = 1;
			}
			//return 1;
		}
		//hore doprava
		while (kj + 1 < n && ki - 1 >= 0 && i < dlzka && toupper(pole[ki - 1][kj + 1]) == slovo[i]) {
			i++;
			kj++;
			ki--;
			pom = 7;
		}
		if (i < dlzka) {
			ki = *r, kj = *s;
			i = 1;
		}
		else {
			if (nasiel_v_smere == 0) { //este som nenasiel slovo v nejakom smere
				vyskrtni(pole, pom, *r, *s, dlzka);
				nasiel = 1;
				nasiel_v_smere = 1;
			}
			//return 1;
		}
		//hore dolava
		while (kj - 1 >= 0 && ki - 1 >= 0 && i < dlzka && toupper(pole[ki - 1][kj - 1]) == slovo[i]) {
			i++;
			kj--;
			ki--;
			pom = 8;
		}
		if (i < dlzka) {
			ki = *r, kj = *s;
			i = 1;
		}
		else {
			if (nasiel_v_smere == 0) { //este som nenasiel slovo v nejakom smere
				vyskrtni(pole, pom, *r, *s, dlzka);
				nasiel = 1;
				nasiel_v_smere = 1;
			}
			//return 1;
		}
		s += 2;
		r += 2;
		posuny += 1;
	}
	return nasiel;
}
void uvolni(char** pole, int* index[], int m) {
	for (int i = 0; i < m; i++) {
		free(pole[i]);
	}
	for (int i = 0; i < 26; i++) {
		free(index[i]);
	}
}
void vypis_tajnicku(char** pole, int m, int n) {
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			if (pole[i][j] >= 'A' && pole[i][j] <= 'Z') putchar(pole[i][j]);
		}
	}
	putchar('\n');
}

int main()
{
	int m, n;
	FILE* f;
	if ((f = fopen("osemsmerovka.txt", "r")) == NULL) {
		printf("Nepodarilo sa otvorit subor\n");
		return -2;
	}
	char** pole, slovo[100];
	int(*index[26]), dlzky[26];
	fscanf(f, "%d %d", &m, &n);
	if ((pole = (char**)malloc(m * sizeof(char*))) == NULL) {
		printf("Nepodarilo sa alokovat pamat\n");
		exit(1);
	}
	for (int i = 0; i < m; i++) {
		if ((pole[i] = (char*)malloc(n * sizeof(char))) == NULL) {
			printf("Nepodarilo sa vyhradit pamat\n");
			return -1;
		}//alokovanie toho dvojrozmerneho pola, ktore tvori osemsmerovku
	}
	vytvor_index(index, dlzky);
	nacitaj(pole, m, n, f, index, dlzky);
	//vypis(pole, m, n);
	//vypis_index(index,dlzky);
	while (fscanf(f, "%s", slovo) == 1) {
		if (!najdi_slovo(pole, index, slovo, m, n, dlzky)) printf("Nenaslo sa slovo %s\n", slovo);
		vypis(pole, m, n);
	}
	vypis_tajnicku(pole, m, n);
	uvolni(pole, index, m);
	if (fclose(f) == EOF) {
		printf("Nepodarilo sa zatvorit subor\n");
		return -1;
	}

	return 0;
}