#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#pragma warning(disable : 4996)
using namespace std;

enum paramentry_w {
	max_kolorow = 7,
	max_graczy = 6,
	min_graczy = 2,
	max_kociolkow = 7,
	min_kociolkow = 1,
	max_zielonych = 10,
	min_zielonych = 1,
	max_zielonych_w = 10,
	min_zielonych_w = 1,
	max_innych = 20,
	min_innych = 1,
	min_rund = 1
};

enum bledy {
	brak_bledu,
	blad_zielonych = 21,
	blad_innych = 22,
	blad_kart_na_rece = 23,
	blad_stosow = 24,
	blad_eksplozji = 25
};

enum kolor_w {
	zielony,
	niebieski,
	czerwony,
	brazawy,
	fioletowy,
	zolty,
	bialy,
	czarny
	
};

enum wczytywanie {
	reka,
	zebrane,
	kociol
};

enum punkty{
	tricizna = 2,
	eliksir = 1
};


struct parametr {
	int liczba_rund;
	int liczba_graczy = 0;
	int liczba_kart_talia = 0;
	int eksplozja = 0;
	int liczba_kociolkow = 0;

	int liczba_zielonych = 0;
	int wprowadzona_wartosc_zielonych;
	int* zielone_wartosci;

	int liczba_kart_innych = 0;
	int* wprowadzone_wartosci_innych;

	int* wczytane;
};

struct kolor {
	char zielony[6] = { "green" };
	char niebieski[5] = { "blue" };
	char czerwony[4] = { "red" };
	char fioletowy[7] = { "violet" };
	char zolty[7] = { "yellow" };
	char bialy[6] = { "white" };
	char czarny[6] = { "black" };
	char brazawy[6] = { "brown" };
}kolory;

struct gracz {
	int karty_reka = 0;
	int *na_rece_w;
	int *na_rece_k;

	int karty_zabrane = 0;
	int *zebrane_w = new int[10];
	int *zebrane_k = new int[10];
	int rozmiar = 10;

	int *odpornosc;
	int liczba_zielonych = 0;
	int punkty = 0;
};

struct kociolek {
	int wartosc = 0;
	int kolor = 0;
	int liczba_kart = 0;
	int rozmiar = 10;
	int *karty_w = new int[10];
	int *karty_kolor = new int[10];
};



int rozszerz_tablice(int* tablica, int rozmiar) {
	int r = 2 * rozmiar;
	int* tab = new int[r];

	for (int i = 0; i < rozmiar; i++) {
		tab[i] = tablica[i];
	}

	tablica = tab;

	return r;
}


int f(const void *a, const void *b) {
	int _a = *(int*)a;
	int _b = *(int*)b;
	if (_a < _b) return -1;
	else if (_a == _b) return 0;
	else return 1;
}


//OBS£UGA PLIKU WSPÓ£DZIELONEGO
int arbitr(int nr_gracza) {
	int aktualny = 0;

	while (true) {
		FILE* plik;
		plik = fopen("gracz_aktywny.txt", "r");
		fscanf(plik, "%d", &aktualny);
		fclose(plik);
		if (aktualny != nr_gracza && aktualny < 21) {
			Sleep(1000);
		}else if(aktualny > 20){
			return aktualny;
		}else {
			return 0;
		}
	}
}


void zmien_gracza(int nr_gracza) {
	FILE* plik;
	plik = fopen("gracz_aktywny.txt", "w");
	fprintf(plik, "%d", nr_gracza);
	fclose(plik);
}



//ZAPISYWANIE STANU GRY
void zapisz_kolor_do_pliku(FILE* plik, int kolor) {
	switch (kolor) {
		case zielony:
			fprintf(plik, "%s ", kolory.zielony);
			break;
		case niebieski:
			fprintf(plik, "%s ", kolory.niebieski);
			break;
		case czerwony:
			fprintf(plik, "%s ", kolory.czerwony);
			break;
		case fioletowy:
			fprintf(plik, "%s ", kolory.fioletowy);
			break;
		case zolty:
			fprintf(plik, "%s ", kolory.zolty);
			break;
		case bialy:
			fprintf(plik, "%s ", kolory.bialy);
			break;
		case czarny:
			fprintf(plik, "%s ", kolory.czarny);
			break;
		case brazawy:
			fprintf(plik, "%s ", kolory.brazawy);
			break;
		}
}


void zapisz_stan_gry(struct gracz gracze[], int liczba_graczy, int liczba_kociolkow, struct kociolek koiolki[]) {
	FILE* plik;
	plik = fopen("stan_gry.txt", "w");
	for (int i = 0; i < liczba_graczy; i++) {
		fprintf(plik, "%d player hand cards (%d): ", (i + 1), gracze[i].karty_reka);
		for (int j = 0; j < gracze[i].karty_reka; j ++) {
			fprintf(plik, "%d ", gracze[i].na_rece_w[j]);
			int k = gracze[i].na_rece_k[j];
			zapisz_kolor_do_pliku(plik, k);
		}

		fprintf(plik, "\n%d player deck cards (%d): ", (i + 1), gracze[i].karty_zabrane);
		for (int a = 0; a < gracze[i].karty_zabrane; a++) {
			fprintf(plik, "%d ", gracze[i].zebrane_w[a]);
			int k = gracze[i].zebrane_k[a];
			zapisz_kolor_do_pliku(plik, k);
		}
		fprintf(plik, "\n");
	}

	for (int i = 0; i < liczba_kociolkow; i++) {
		fprintf(plik, "%d pile cards (%d): ", (i+1), koiolki[i].liczba_kart);
		for (int j = 0; j < koiolki[i].liczba_kart; j++) {
			fprintf(plik, "%d ", koiolki[i].karty_w[j]);
			int k = koiolki[i].karty_kolor[j];
			zapisz_kolor_do_pliku(plik, k);
		}
		fprintf(plik, "\n");
	}

	fclose(plik);
}



//WCZYTYWANIE PARAMETRÓW
void zapisz_parametry_do_pliku(struct parametr *parametry) {
	FILE* plik;
	plik = fopen("parametry.txt", "w");
	fprintf(plik, "liczba rund = %d\n", (*parametry).liczba_rund);
	fprintf(plik, "liczba graczy = %d\n", (*parametry).liczba_graczy);
	fprintf(plik, "liczba kociolkow = %d\n", (*parametry).liczba_kociolkow);
	fprintf(plik, "eksplozja = %d\n", (*parametry).eksplozja);
	fprintf(plik, "liczba kart w talii = %d\n", (*parametry).liczba_kart_talia);
	fprintf(plik, "liczba zielonych = %d\nwartosc zielonych = %d\n", (*parametry).liczba_zielonych, (*parametry).wprowadzona_wartosc_zielonych);
	fprintf(plik, "liczba innych = %d\n", (*parametry).liczba_kart_innych);

	for (int i = 0; i < (*parametry).liczba_kart_innych; i++) {
		fprintf(plik, "%d ", (*parametry).wprowadzone_wartosci_innych[i]);
	}
	fclose(plik);
}


void wczytaj_parametry(struct parametr *parametry, int nr_gracza){
	int n, s, k, g, gv, o, e, r;
	if (nr_gracza == 1) {
		int i = 0;
		while (i == 0) {
			cin >> n >> k >> g >> gv >> o >> e >> r;
			s = g + (k * o);
			if (n < min_graczy) {
				cout << "Not enough players\n";
			}else if (n > max_graczy) {
				cout << "Too many players\n";
			}else if (k < min_kociolkow) {
				cout << "Not enough piles\n";
			}else if (k > max_kociolkow) {
				cout << "Too many piles\n";
			}else if (g < min_zielonych) {
				cout << "Not enough green cards\n";
			}else if (o < min_innych) {
				cout << "Not enough cards of other colors\n";
			}else if (s < n) {
				cout << "Not enough cards\n";
			}else if (r < min_rund) {
				cout << "Not enough rounds\n";
			}else {
				i = 1;
			}
		}
	}else {
		FILE* plik;
		plik = fopen("parametry.txt", "r");
		fscanf(plik, "liczba rund = %d\n", &r);
		fscanf(plik, "liczba graczy = %d\n", &n);
		fscanf(plik, "liczba kociolkow = %d\n", &k);
		fscanf(plik, "eksplozja = %d\n", &e);
		fscanf(plik, "liczba kart w talii = %d\n", &s);
		fscanf(plik, "liczba zielonych = %d\n", &g);
		fscanf(plik, "wartosc zielonych = %d\n", &gv);
		fscanf(plik, "liczba innych = %d\n", &o);
		fclose(plik);
	}
	
	(*parametry).liczba_rund = r;
	(*parametry).liczba_graczy = n;
	(*parametry).liczba_kociolkow = k;
	(*parametry).liczba_zielonych = g;
	(*parametry).wprowadzona_wartosc_zielonych = gv;
	(*parametry).liczba_kart_innych = o;
	(*parametry).liczba_kart_talia = s;
	(*parametry).eksplozja = e;
}


void wczytaj_parametry_inne(struct parametr *parametry, int nr_gracza) {
	int o = (*parametry).liczba_kart_innych, k = (*parametry).liczba_kociolkow;
	int* ov = new int[o];

	if (nr_gracza == 1) {
		for (int i = 0; i < o; i++) {
			cin >> ov[i];
		}
	}else {
		int r, n, kk, e, s, g, gv, oo;
		FILE* plik;
		plik = fopen("parametry.txt", "r");
		fscanf(plik, "liczba rund = %d\n", &r);
		fscanf(plik, "liczba graczy = %d\n", &n);
		fscanf(plik, "liczba kociolkow = %d\n", &kk);
		fscanf(plik, "eksplozja = %d\n", &e);
		fscanf(plik, "liczba kart w talii = %d\n", &s);
		fscanf(plik, "liczba zielonych = %d\n", &g);
		fscanf(plik, "wartosc zielonych = %d\n", &gv);
		fscanf(plik, "liczba innych = %d\n", &oo);
		for (int i = 0; i < oo; i++) {
			fscanf(plik, "%d ", &ov[i]);
		}
		fclose(plik);
	}

	for (int i = 0; i < o; i++) {
		(*parametry).wprowadzone_wartosci_innych[i] = ov[i];
	}

	delete ov;

	if (nr_gracza == 1) {
		zapisz_parametry_do_pliku(parametry);
	}
}



//TWORZENIE TALII
void zapisz_talie_do_pliku(int talia[], int talia_k[], int s) {
	FILE* plik;
	plik = fopen("talia.txt", "w");
	for (int i = 0; i < s; i++) {
		fprintf(plik, "%d ", talia[i]);
		int k = talia_k[i];
		switch (k) {
		case zielony:
			fprintf(plik, "%s ", kolory.zielony);
			break;
		case niebieski:
			fprintf(plik, "%s ", kolory.niebieski);
			break;
		case czerwony:
			fprintf(plik, "%s ", kolory.czerwony);
			break;
		case fioletowy:
			fprintf(plik, "%s ", kolory.fioletowy);
			break;
		case zolty:
			fprintf(plik, "%s ", kolory.zolty);
			break;
		case bialy:
			fprintf(plik, "%s ", kolory.bialy);
			break;
		case czarny:
			fprintf(plik, "%s ", kolory.czarny);
			break;
		}
	}
	
	fclose(plik);
}


void utworz_talie(int talia[], int talia_k[], struct parametr *parametry) {
	int s = (*parametry).liczba_kart_talia;
	int g = (*parametry).liczba_zielonych;
	int o = (*parametry).liczba_kart_innych;
	int gv = (*parametry).wprowadzona_wartosc_zielonych;
	
	int kolor = 0;
	for (int i = 0; i < s; i++) {
		if (i < g) {
			talia[i] = gv;
			talia_k[i] = kolor;
		}else {
			if (((i - g) % o) == 0) {
				kolor++;
			}

			int ov = (*parametry).wprowadzone_wartosci_innych[((i - g) % o)];
			talia[i] = ov;
			talia_k[i] = kolor;
		}
	}

	zapisz_talie_do_pliku(talia, talia_k, s);
}



//TASOWANIE TALII
void ustaw(int talia[], int talia_k[], int tasowanie[], int kolory[], int liczba_kart) {
	//ustawianie kart w wylosowanej kolejnoœci
	for (int i = 0; i < liczba_kart; i++) {
		int k = tasowanie[i];
		talia_k[i] = kolory[k];
		tasowanie[i] = talia[k];
	}

	for (int i = 0; i < liczba_kart; i++) {
		talia[i] = tasowanie[i];
	}
}


bool czy_powtorka(int liczba, int tab[], int ile){
	if (ile <= 0){
		return false;
	}

	int i = 0;
	do{
		if (tab[i] == liczba){
			return true;
		}
		i++;
	}while (i < ile);

	return false;
}


void potasuj_talie(int talia[], int talia_k[], int liczba_kart) {

	srand(time(NULL));
	int* tasowanie = new int[liczba_kart];
	int* kolory = new int[liczba_kart];

	for (int i = 0; i < liczba_kart; i++) {
		kolory[i] = talia_k[i];
	}
	int wylosowanych = 0;

	//Losowanie liczb pseudolosowych od 0 do liczby_graczy (indeksy kart)
	do{
		int liczba = rand() % liczba_kart;
		if (czy_powtorka(liczba, tasowanie, wylosowanych) == false){
			tasowanie[wylosowanych] = liczba;
			wylosowanych++;
		}
	} while (wylosowanych < liczba_kart);

	ustaw(talia, talia_k, tasowanie, kolory, liczba_kart);
}



void rozdaj_karty(int talia[], int talia_k[], struct parametr *parametry, struct gracz *gracze) {
	int n = (*parametry).liczba_graczy;
	int s = (*parametry).liczba_kart_talia;
	int k = (*parametry).liczba_kociolkow;

	for (int i = 0; i < n; i++) {
		int z = 0;

		for (int j = i; j < s; j += n) { 
			gracze[i].na_rece_w[z] = talia[j];
			gracze[i].na_rece_k[z] = talia_k[j];
			z++;
			gracze[i].karty_reka++;
		}
	}
}



//WYPISYWANIE STANU GRY
int wypisz_kolor(int kolor) {
	switch (kolor){
	case zielony:
		cout << kolory.zielony << " ";
		break;
	case niebieski:
		cout << kolory.niebieski << " ";
		break;
	case czerwony:
		cout << kolory.czerwony << " ";
		break;
	case fioletowy:
		cout << kolory.fioletowy << " ";
		break;
	case zolty:
		cout << kolory.zolty << " ";
		break;
	case bialy:
		cout << kolory.bialy << " ";
		break;
	case czarny:
		cout << kolory.czarny << " ";
		break;
	case brazawy:
		cout << kolory.brazawy << " ";
		break;
	default:
		break;
	}
	return 0;
}


int wypisz_stan_gry(struct gracz gracze[], struct kociolek kociolki[], int gracz_a, int n, int e, int k) {
	cout << endl << "active player = " << gracz_a << "\nplayers number = " << n << "\nexplosion threshold = " << e << endl;
	
	for (int i = 0; i < n; i++) {
		cout << i+1 << " player hand cards: ";
		for (int j = 0; j < gracze[i].karty_reka; j++) {
			cout << gracze[i].na_rece_w[j] << ' ';
			wypisz_kolor(gracze[i].na_rece_k[j]);
		}

		cout << endl << i + 1 << " player deck cards: ";
		for (int j = 0; j < gracze[i].karty_zabrane; j++) {
			cout << gracze[i].zebrane_w[j] << ' ';
			wypisz_kolor(gracze[i].zebrane_k[j]);
		}
		cout << endl;
	}

	for (int i = 0; i < k; i++) {
		cout << i + 1 << " pile cards: ";
		for (int j = 0; j < kociolki[i].liczba_kart; j++) {
			cout << kociolki[i].karty_w[j] << ' ';
			wypisz_kolor(kociolki[i].karty_kolor[j]);
		}
		cout << endl;
	}

	return 0;
}



//ZAPISYWANIE DO STRUCKTUR
int zapisz_wartosc_z(int a, int wartosc[max_zielonych], int nr_zielonej, int aktywnosc, int nr_karty, struct gracz gracze[], struct kociolek kociolki[], int numer) {
	wartosc[nr_zielonej] = a;

	if (aktywnosc == reka) {
		gracze[numer].na_rece_w[nr_karty] = wartosc[nr_zielonej];
	}else if (aktywnosc == zebrane) {
		gracze[numer].zebrane_w[nr_karty] = wartosc[nr_zielonej];
	}else {
		kociolki[numer].karty_w[nr_karty] = wartosc[nr_zielonej];
		kociolki[numer].wartosc += wartosc[nr_zielonej];
	}

	return 0;
}


int zapisz_wartosc(int wartosc, struct gracz gracze[], struct kociolek kociolki[], int kolor, int nr_karty, int nr, int a, int **wartosci, struct parametr *parametry) {
	kolor -= 1;
	int i = (*parametry).wczytane[kolor];
	wartosci[kolor][i] = wartosc;

	if (a == reka) {
		gracze[nr].na_rece_w[nr_karty] = wartosc;
	}else if (a == zebrane) {
		gracze[nr].zebrane_w[nr_karty] = wartosc;
	}else {
		kociolki[nr].karty_w[nr_karty] = wartosc;
		kociolki[nr].wartosc += wartosc;
		if (kociolki[nr].kolor == 0) {
			kociolki[nr].kolor = kolor + 1;
		}
	}

	(*parametry).wczytane[kolor]++;

	return 0;
}

//nr - numer gracza/kocio³ka; a - zmienna do sprawdzenia czy karty s¹ wczytywane z rêki, zebranych lub kocio³ka
int zapisz_kolor(char kolor[8], struct gracz gracze[], struct kociolek kociolki[], int nr_karty, int nr, int a, struct parametr *parametry) {
	int k = 0;
	if (strcmp(kolor, kolory.zielony) == 0) {
		k = zielony;
	}else if (strcmp(kolor, kolory.niebieski) == 0) {
		k = niebieski;
	}else if (strcmp(kolor, kolory.czerwony) == 0) {
		k = czerwony;
	}else if (strcmp(kolor, kolory.fioletowy) == 0) {
		k = fioletowy;
	}else if (strcmp(kolor, kolory.zolty) == 0) {
		k = zolty;
	}else if (strcmp(kolor, kolory.bialy) == 0) {
		k = bialy;
	}else if (strcmp(kolor, kolory.czarny) == 0) {
		k = czarny;
	}

	if (a == reka) {
		gracze[nr].na_rece_k[nr_karty] = k;
	}else if (a == zebrane) {
		gracze[nr].zebrane_k[nr_karty] = k;
	}
	else if(a == kociol){
		if (kociolki[nr].kolor == 0 && k != 0) {
			kociolki[nr].kolor = k;
		}
		kociolki[nr].karty_kolor[nr_karty] = k;
	}
	return k;
}



// SPRAWDZANIE WCZYTANEGO STANU GRY
int sprawdz_zielone(int wartosc[], int liczba, struct parametr *parametry) {
	int z;
	for (int i = 0; i < liczba; i++) {
		if (wartosc[i] != (*parametry).wprowadzona_wartosc_zielonych) {
			z = blad_zielonych;
			cout << "Different green cards values occurred\n";
			return z;
		}
	}

	if (liczba == 0) {
		cout << "Green cards does not exist\n";
		z = blad_zielonych;
		return z;
	}

	return brak_bledu;
}


int sprawdz_liczbe_innych(int liczba_kart, int liczba_kolorow, struct parametr *parametry) {
	for (int i = 0; i < liczba_kolorow; i++) {
		if ((*parametry).wczytane[i] != liczba_kart) {
			return blad_innych;
		}
	}
	return brak_bledu;
}


int sprawdz_wartosci_innych(int **wartosci, int liczba, int liczba_kolorow) {
	int z = brak_bledu;
	for (int i = 0; i < liczba_kolorow; i++) {
		for (int j = 0; j < liczba; j++) {
			if (wartosci[0][j] != wartosci[i][j]) {
				cout << "The values of cards of all colors are not identical:\n";
				z = blad_innych;
			}
		}
	}

	if (z == blad_innych) {
		for (int i = 0; i < liczba_kolorow; i++) {
			wypisz_kolor(i);
			cout << "cards values: ";
			for (int j = 0; j < liczba; j++) {
				cout << wartosci[i][j];
			}
			cout << endl;
		}
	}
	return z;
}





int sprawdz_poprawnosc_innych(struct parametr *parametry, int **wartosci) {
	for (int i = 0; i < (*parametry).liczba_kociolkow; i++) {
		int* t = new int[(*parametry).wczytane[i]];
		for (int j = 0; j < (*parametry).wczytane[i]; j++) {
			t[j] = wartosci[i][j];
		}

		qsort((void*) t, (*parametry).wczytane[i], sizeof(int), f);

		for (int j = 0; j < (*parametry).liczba_kart_innych; j++) {
			wartosci[i][j] = t[j];
		}
	}
	
	int z = sprawdz_liczbe_innych((*parametry).liczba_kart_innych, (*parametry).liczba_kociolkow, parametry);
	if(z == brak_bledu) {
		z = sprawdz_wartosci_innych(wartosci, (*parametry).wczytane[0], (*parametry).liczba_kociolkow);
	}else {
		cout << "At least two colors with a different number of cards were found:\n";
		for (int i = 1; i <= (*parametry).liczba_kociolkow; i++) {
			wypisz_kolor(i);
			cout << " cards are: " << (*parametry).wczytane[i-1];
		}
	}

	return z;
}


int sprawdz_liczbe_kart_na_rece(struct parametr *parametry, struct gracz gracze[]) {
	for (int i = 1; i < (*parametry).liczba_graczy; i++) {
		if (gracze[0].karty_reka > (gracze[i].karty_reka + 1) || gracze[0].karty_reka < (gracze[i].karty_reka - 1)) {
			return blad_kart_na_rece;
		}
	}
	return brak_bledu;
}


int sprawdz_kolor_kociolka(struct kociolek stos) {
	for (int i = 0; i < stos.liczba_kart; i++) {
		if (stos.kolor != stos.karty_kolor[i] && stos.karty_kolor[i] != 0 && stos.liczba_kart != 1) {
			return 1;
		}
	}
	return 0;
}


int sprawdz_czy_eksplozja(struct kociolek stos, struct parametr *parametry) {
	stos.wartosc = 0;
	for (int i = 0; i < stos.liczba_kart; i++) {
		stos.wartosc += stos.karty_w[i];
	}

	if (stos.wartosc > (*parametry).eksplozja) {
		return blad_eksplozji;
	}

	return brak_bledu;
}


int sprawdz_poprawnosc_gry(struct parametr* parametry, struct gracz gracze[], struct kociolek kociolki[], int *wartosci_zielonych, int liczba_zielonych, int **wartosci_innych) {
	int zielone = sprawdz_zielone(wartosci_zielonych, liczba_zielonych, parametry);
	if (zielone == blad_zielonych) {
		return blad_zielonych;
	}

	int inne = sprawdz_poprawnosc_innych(parametry, wartosci_innych);
	if (inne == blad_innych) {
		return blad_innych;
	}

	int liczba_kart = sprawdz_liczbe_kart_na_rece(parametry, gracze);
	if (liczba_kart == blad_innych) {
		return blad_innych;
	}

	int stosy[max_kociolkow] = { 0 }, eksplozja[max_kociolkow] = { 0 };
	for (int i = 0; i < (*parametry).liczba_kociolkow; i++) {
		stosy[i] = sprawdz_kolor_kociolka(kociolki[i]);
		if (stosy[i] == blad_stosow) {
			cout << "Two different colors were found on the " << i + 1 << " pile\n";
		}

		eksplozja[i] = sprawdz_czy_eksplozja(kociolki[i], parametry);
		if (eksplozja[i] == blad_eksplozji) {
			cout << "Pile number " << i + 1 << " should explode earlier\n";
		}
	}

	for (int i = 0; i < (*parametry).liczba_kociolkow; i++) {
		if (stosy[i] == blad_stosow) {
			return blad_stosow;
		}

		if (eksplozja[i] == blad_eksplozji) {
			return blad_eksplozji;
		}
	}
	
	return brak_bledu;
}




//RUCH GRACZA
int wybierz_kociolek(int *najmniejsze, int liczba_kociolkow, struct kociolek kociolki[], int *najmniejszy_k) {
	int* tab = new int[liczba_kociolkow];

	for (int i = 0; i < liczba_kociolkow; i++) {
		najmniejszy_k[i] = kociolki[i].wartosc;
	}

	qsort((void*)najmniejszy_k, liczba_kociolkow, sizeof(int), f);
	//ustaw_kociolki_roznaco(najmniejszy_k, liczba_kociolkow, kociolki);
	int i = 0;
	while (i <= liczba_kociolkow) {
		for (int j = 0; j < liczba_kociolkow; j++) {
			if (kociolki[j].wartosc == najmniejszy_k[i] && kociolki[j].kolor != 0 && (najmniejsze[kociolki[j].kolor]!=0 || najmniejsze[0] != 0)) {
				return j;
			}else if (kociolki[j].wartosc == najmniejszy_k[i] && kociolki[j].kolor == 0) {
				return j;
			}
			
		}
		i++;
	}
}


void wybierz_najmniejsze(struct gracz gracze[], int* najmniejsze, int liczba_kociolkow, int numer_gracza) {
	for (int i = 0; i < gracze[numer_gracza].karty_reka; i++) {
		int k = gracze[numer_gracza].na_rece_k[i];

		if (najmniejsze[k] == 0) {
			najmniejsze[k] = gracze[numer_gracza].na_rece_w[i];
		}else if (najmniejsze[k] > gracze[numer_gracza].na_rece_w[i]) {
			najmniejsze[k] = gracze[numer_gracza].na_rece_w[i];
		}
	}

	int min = najmniejsze[0];
	for (int i = 1; i <= liczba_kociolkow; i++) {
		if (najmniejsze[i] != 0 && najmniejsze[i] < min) {
			min = najmniejsze[i];
		}else if (min == 0 && najmniejsze[i] != 0) {
			min = najmniejsze[i];
		}
	}

	for (int i = 0; i <= liczba_kociolkow; i++) {
		if (najmniejsze[i] > min) {
			najmniejsze[i] = 0;
		}
	}
}


void wybierz_najwieksze(struct gracz gracze[], int numer_gracza, int liczba_kociolkow, int *najwieksze, struct kociolek kociolki[], int *wartosci_k) {
	for (int i = 0; i < gracze[numer_gracza].karty_reka; i++) {
		int k = gracze[numer_gracza].na_rece_k[i];

		if (najwieksze[k] == 0) {
			najwieksze[k] = gracze[numer_gracza].na_rece_w[i];
		}
		else if (najwieksze[k] < gracze[numer_gracza].na_rece_w[i]) {
			najwieksze[k] = gracze[numer_gracza].na_rece_w[i];
		}
	}

	/*int max = najwieksze[0], kolor = 0;
	for (int i = 1; i <= liczba_kociolkow; i++) {
		if (najwieksze[i] > max) {
			max = najwieksze[i];
			kolor = i;
		}else if (max == 0 && najwieksze[i] != 0) {
			max = najwieksze[i];
			kolor = i;
		}
	}*/

}


int sprawdz_czy_kociolek_jest_najmniejszy(struct gracz gracze[], int numer_gracza, int liczba_kociolkow, int* wartosci_kociolkow, struct kociolek kociolki[], int *najwieksze) {
	for (int j = 0; j <= liczba_kociolkow; j++) {
		for (int i = 0; i < liczba_kociolkow; i++) {
			if (kociolki[i].wartosc == wartosci_kociolkow[j] && najwieksze[kociolki[i].kolor] != 0) {
				return i;
			}
			
		}
	}
	return 0;
}


int sprawdz_ktora_karta(struct gracz gracze[], int nr_gracza, int* najmniejsze, int kolor, int liczba_kociolkow, struct kociolek kociolki[]) {
	if (kolor == zielony) {
		for (int i = 0; i <= liczba_kociolkow; i++) {
			if (najmniejsze[i] != 0) {
				for (int j = 0; j < gracze[nr_gracza].karty_reka; j++) {
					if (gracze[nr_gracza].na_rece_w[j] == najmniejsze[i] && gracze[nr_gracza].na_rece_k[j] == i) {
						return j;
					}
				}
			}
		}
	}else {
		if (najmniejsze[kolor] != 0) {
			for (int j = 0; j < gracze[nr_gracza].karty_reka; j++) {
				if (gracze[nr_gracza].na_rece_k[j] == kolor && gracze[nr_gracza].na_rece_w[j] == najmniejsze[kolor]) {
					return j;
				}
			}
		}else {
			for (int j = 0; j < gracze[nr_gracza].karty_reka; j++) {
				if (gracze[nr_gracza].na_rece_k[j] == zielony && gracze[nr_gracza].na_rece_w[j] == najmniejsze[zielony]) {
					return j;
				}
			}
		}
	}
	
}


void dodaj_do_kociolka(int kolor_karty, int wartosc_karty, struct kociolek kociolki[], int nr_kociolka) {
	kociolki[nr_kociolka].karty_w[kociolki[nr_kociolka].liczba_kart] = wartosc_karty;
	kociolki[nr_kociolka].karty_kolor[kociolki[nr_kociolka].liczba_kart] = kolor_karty;

	if (kociolki[nr_kociolka].kolor == 0) {
		kociolki[nr_kociolka].kolor = kolor_karty;
	}

	kociolki[nr_kociolka].wartosc += wartosc_karty;
	kociolki[nr_kociolka].liczba_kart++;

	while (kociolki[nr_kociolka].liczba_kart >= kociolki[nr_kociolka].rozmiar) {
		int* t = new int[kociolki[nr_kociolka].rozmiar];
		int* k = new int[kociolki[nr_kociolka].rozmiar];

		for (int a = 0; a < kociolki[nr_kociolka].rozmiar; a++) {
			t[a] = kociolki[nr_kociolka].karty_w[a];
			k[a] = kociolki[nr_kociolka].karty_kolor[a];
		}

		int r = kociolki[nr_kociolka].rozmiar;
		kociolki[nr_kociolka].rozmiar = rozszerz_tablice(k, r);
		rozszerz_tablice(t, r);
		kociolki[nr_kociolka].karty_w = t;
		kociolki[nr_kociolka].karty_kolor = k;
	}
}


void przesun_karty(struct gracz gracze[], int nr_gracza, int nr_karty) {
	//usuniêcie wy³o¿onej karty i przesuniêcie kolejnych o jedno miejsce
	for (int i = nr_karty + 1; i < gracze[nr_gracza].karty_reka; i++) {
		int j = i - 1;
		gracze[nr_gracza].na_rece_w[j] = gracze[nr_gracza].na_rece_w[i];
		gracze[nr_gracza].na_rece_k[j] = gracze[nr_gracza].na_rece_k[i];
	}

	gracze[nr_gracza].karty_reka--;
}


void przenies_karty(struct gracz gracze[], int nr_gracza, struct kociolek kociolki[], int nr_kociolka) {
	int j = gracze[nr_gracza].karty_zabrane;
	int k = 0, kolor = kociolki[nr_kociolka].kolor - 1;

	for (int i = 0; i < kociolki[nr_kociolka].liczba_kart; i++) {
		while (gracze[nr_gracza].karty_zabrane >= gracze[nr_gracza].rozmiar) {  //powiêksznie tablicy jeœli liczba kart przekroczy rozmiar tej tablicy
			int* t = new int[gracze[nr_gracza].rozmiar];
			int* k = new int[gracze[nr_gracza].rozmiar];

			for (int a = 0; a < gracze[nr_gracza].rozmiar; a++) {
				t[a] = gracze[nr_gracza].zebrane_w[a];
				k[a] = gracze[nr_gracza].zebrane_k[a];
			}

			int r = gracze[nr_gracza].rozmiar;
			gracze[nr_gracza].rozmiar = rozszerz_tablice(k, r);
			rozszerz_tablice(t, r);

			gracze[nr_gracza].zebrane_k = k;
			gracze[nr_gracza].zebrane_w = t;
		}

		gracze[nr_gracza].zebrane_w[j] = kociolki[nr_kociolka].karty_w[i];
		gracze[nr_gracza].zebrane_k[j] = kociolki[nr_kociolka].karty_kolor[i];

		if (kociolki[nr_kociolka].karty_kolor[i] == kociolki[nr_kociolka].kolor) {
			k++;
		}
		j++;
	}

	gracze[nr_gracza].karty_zabrane = j;
	kociolki[nr_kociolka].liczba_kart = 0;
	kociolki[nr_kociolka].wartosc = 0;
}


void wykonaj_ruch(struct kociolek kociolki[], int liczba_kociolkow, struct gracz gracze[], int nr_gracza, struct parametr *parametry) {
	nr_gracza--;
	int* najmniejsze = new int[liczba_kociolkow + 1];
	int* wartosci_kociolkow = new int[liczba_kociolkow];

	for (int i = 0; i <= liczba_kociolkow; i++) {
		najmniejsze[i] = 0;
	}

	wybierz_najmniejsze(gracze, najmniejsze, liczba_kociolkow, nr_gracza);
	int nr_kociolka = wybierz_kociolek(najmniejsze, (*parametry).liczba_kociolkow, kociolki, wartosci_kociolkow);

	if (kociolki[nr_kociolka].kolor == 0) {
		for (int i = 0; i < liczba_kociolkow; i++) {
			if (kociolki[i].kolor != 0) {
				najmniejsze[kociolki[i].kolor] = 0;
			}
		}
	}

	int nr_karty = sprawdz_ktora_karta(gracze, nr_gracza, najmniejsze, kociolki[nr_kociolka].kolor, liczba_kociolkow, kociolki);
	int wartosc_kociolka = kociolki[nr_kociolka].wartosc + gracze[nr_gracza].na_rece_w[nr_karty];
	cout << nr_karty << " kociolek " << nr_kociolka << endl;
	if ((*parametry).eksplozja < wartosc_kociolka) {
		int *najwieksze = new int[liczba_kociolkow+1];
		for (int i = 0; i <= liczba_kociolkow; i++) {
			najwieksze[i] = 0;
		}
		
		wybierz_najwieksze(gracze, nr_gracza, kociolki[nr_kociolka].kolor, najwieksze, kociolki, wartosci_kociolkow);
		nr_kociolka = sprawdz_czy_kociolek_jest_najmniejszy(gracze, nr_gracza, liczba_kociolkow, wartosci_kociolkow, kociolki, najwieksze);
		nr_karty = sprawdz_ktora_karta(gracze, nr_gracza, najwieksze, kociolki[nr_kociolka].kolor, liczba_kociolkow, kociolki);
		cout << nr_karty << " kociolek::: " << nr_kociolka << endl;
	}

	dodaj_do_kociolka(gracze[nr_gracza].na_rece_k[nr_karty], gracze[nr_gracza].na_rece_w[nr_karty], kociolki, nr_kociolka);
	przesun_karty(gracze, nr_gracza, nr_karty);

	int czy = sprawdz_czy_eksplozja(kociolki[nr_kociolka], parametry);
	if (czy == blad_eksplozji) {
		przenies_karty(gracze, nr_gracza, kociolki, nr_kociolka);
	}
}



//OBS£UGA KOÑCA
int policz_kolory_zebranych(struct gracz gracze[], int liczba_graczy) {
	for (int i = 0; i < liczba_graczy; i++) {
		for (int j = 0; j < gracze[i].karty_zabrane; j++) {
			if (gracze[i].zebrane_k[j] != 0) {
				int kolor = gracze[i].zebrane_k[j] - 1;
				gracze[i].odpornosc[kolor] ++;
			}else {
				gracze[i].liczba_zielonych++;
			}
		}
	}

	return 0;
}


int wyznacz_odpornosc(struct parametr* parametry, struct gracz gracze[], int liczba_graczy, int tab[]) {
	for (int i = 0; i < (*parametry).liczba_kociolkow; i++) {
		int max = gracze[0].odpornosc[i];
		tab[i] = 1;

		for (int j = 1; j < (*parametry).liczba_graczy; j++) {
			if (gracze[j].odpornosc[i] > max) {
				tab[i] = j + 1;
				max = gracze[j].odpornosc[i];
			}else if (gracze[j].odpornosc[i] == max){
				tab[i] = 0;
			}
		} 

		if (tab[i] != 0) {
			cout << "Na kolor ";
			wypisz_kolor(i + 1);
			cout << "odporny jest gracz " << tab[i] << endl;
		}
	}

	return 0;
}


int oblicz_punkty(struct gracz gracze[], int liczba_graczy, int tab[], int liczba_kolorow) {
	for (int i = 0; i < liczba_graczy; i++) {
		int d = i + 1;
		for (int j = 0; j < liczba_kolorow; j++) {
			if (tab[j] != d) {
				int c = gracze[i].odpornosc[j];
				gracze[i].punkty += c;
			}
		}

		int z = (gracze[i].liczba_zielonych) * 2;
		gracze[i].punkty += z;
		cout << "Wynik gracza " << d << " = " << gracze[i].punkty << endl;
	}
	return 0;
}


int koniec_tury(struct parametr *parametry, struct gracz gracze[], int liczba_graczy) {
	int* tab = new int[(*parametry).liczba_kociolkow];

	policz_kolory_zebranych(gracze, (*parametry).liczba_graczy);
	wyznacz_odpornosc(parametry, gracze, liczba_graczy, tab);
	oblicz_punkty(gracze, liczba_graczy, tab, (*parametry).liczba_kociolkow);

	return 0;
}


int sprawdz_czy_koniec(struct gracz gracze[], int liczba_graczy) {
	for (int i = 0; i < liczba_graczy; i++) {
		if (gracze[i].karty_reka != 0) {
			return 0;
		}
	}

	return 1;
}



//WCZYTANIE STANU GRY
int wczytaj_karty_gracz(struct parametr *parametry, struct gracz gracze[], struct kociolek kociolki[], FILE* plik, int wartosci_zielonych[], int **wartosci_innych, int zielone) {
	int nr_gracza, wartosc;
	char kolor[8];

	for (int i = 0; i < (*parametry).liczba_graczy; i++) {
		int karty, karty1;

		fscanf(plik, "%d player hand cards (%d): ", &nr_gracza, &karty);
		gracze[i].karty_reka = karty;
		for (int j = 0; j < gracze[i].karty_reka; j++) {
			fscanf(plik, "%d %s", &wartosc, kolor);
			int k = zapisz_kolor(kolor, gracze, kociolki, j, i, reka, parametry);
			if (k == 0) {
				zapisz_wartosc_z(wartosc, wartosci_zielonych, zielone, reka, j, gracze, kociolki, i);
				zielone++;
			}else {
				zapisz_wartosc(wartosc, gracze, kociolki, k, j, i, reka, wartosci_innych, parametry);
			}
		}

		fscanf(plik, "\n%d player deck cards (%d): ", &nr_gracza, &karty1);
		gracze[i].karty_zabrane = karty1;

		while(gracze[i].karty_zabrane >= gracze[i].rozmiar){ //powiêkszanie tablicy, jeœli liczba kart do wczytania jest wiêksza ni¿ aktualny rozmiar tablicy
			int* t = new int[gracze[i].rozmiar];
			int* k = new int[gracze[i].rozmiar];

			for (int a = 0; a < gracze[i].rozmiar; a++) {
				t[a] = gracze[i].zebrane_w[a];
				k[a] = gracze[i].zebrane_k[a];
			}

			int r = gracze[i].rozmiar;
			gracze[i].rozmiar = rozszerz_tablice(k, r);
			rozszerz_tablice(t, r);
			gracze[i].zebrane_k = k;
			gracze[i].zebrane_w = t;
		}

		for (int j = 0; j < gracze[i].karty_zabrane; j++) {
			fscanf(plik, "%d %s", &wartosc, kolor);
			int k = zapisz_kolor(kolor, gracze, kociolki, j, i, zebrane, parametry);
			if (k == 0) {
				zapisz_wartosc_z(wartosc, wartosci_zielonych, zielone, zebrane, j, gracze, kociolki, i);
				zielone++;
			}else {
				zapisz_wartosc(wartosc, gracze, kociolki, k, j, i, zebrane, wartosci_innych, parametry);
			}
		}
		fscanf(plik, "\n");
	}

	return zielone;
}


int wczytaj_karty_kociolek(struct parametr* parametry, struct gracz gracze[], struct kociolek kociolki[], FILE* plik, int wartosci_zielonych[], int** wartosci_innych, int zielone) {
	int nr_kociolka, wartosc, karty;
	char kolor[8];

	for (int i = 0; i < (*parametry).liczba_kociolkow; i++) {
		fscanf(plik, "%d pile cards (%d): ", &nr_kociolka, &karty);
		kociolki[i].liczba_kart = karty;
		kociolki[i].wartosc = 0;

		while (kociolki[i].liczba_kart >= kociolki[i].rozmiar) {  //powiêkszanie tablicy, jeœli liczba kart do wczytania jest wiêksza ni¿ aktualny rozmiar tablicy
			int* t = new int[kociolki[i].rozmiar];
			int* k = new int[kociolki[i].rozmiar];

			for (int a = 0; a < kociolki[i].rozmiar; a++) {
				t[a] = kociolki[i].karty_w[a];
				k[a] = kociolki[i].karty_kolor[a];
			}

			int r = kociolki[i].rozmiar;
			kociolki[i].rozmiar = rozszerz_tablice(k, r);
			rozszerz_tablice(t, r);
			kociolki[i].karty_w = t;
			kociolki[i].karty_kolor = k;
		}

		for (int j = 0; j < kociolki[i].liczba_kart; j++) {
			fscanf(plik, "%d %s ", &wartosc, kolor);
			int k = zapisz_kolor(kolor, gracze, kociolki, j, i, kociol, parametry);

			if (k == 0) {
				zapisz_wartosc_z(wartosc, wartosci_zielonych, zielone, kociol, j, gracze, kociolki, i);
				zielone++;
			}else {
				zapisz_wartosc(wartosc, gracze, kociolki, k, j, i, kociol, wartosci_innych, parametry);
			}
		}
	}

	return zielone;
}


int wczytaj_stan_gry(struct parametr  *parametry, struct gracz gracze[], struct kociolek kociolki[]) {
	int* zielone_w = new int[(*parametry).liczba_zielonych];
	int** wartosci_innych = new int* [(*parametry).liczba_kart_innych];
	int k = (*parametry).liczba_kociolkow;

	for (int i = 0; i < (*parametry).liczba_kociolkow; i++) {
		wartosci_innych[i] = new int[(*parametry).liczba_kart_innych];
		(*parametry).wczytane[i] = 0;

	}

	FILE* plik;
	plik = fopen("stan_gry.txt", "r");

	int z = 0, zielone;
	zielone = wczytaj_karty_gracz(parametry, gracze, kociolki, plik, zielone_w, wartosci_innych, z);

	z = zielone;
	zielone = wczytaj_karty_kociolek(parametry, gracze, kociolki, plik, zielone_w, wartosci_innych, z);

	fclose(plik);

	int poprawnosc = sprawdz_poprawnosc_gry(parametry, gracze, kociolki, zielone_w, zielone, wartosci_innych);
	if (poprawnosc != brak_bledu) {
		return poprawnosc;
	}

	return 0;
}



int main(int argc, char* argv[]) {	

	int nr_gracza = atoi(argv[1]); //przetworzenie numeru gracza na int
	if (nr_gracza == 1) {
		zmien_gracza(0);
	}
		
	struct parametr parametry;
	wczytaj_parametry(&parametry, nr_gracza);
	
	int max_kart_na_rece = parametry.liczba_kart_talia / parametry.liczba_graczy + 1;
	int* tab = new int[parametry.liczba_zielonych];
	int* wartosci = new int[parametry.liczba_kart_innych];
	int* liczba_innych = new int[parametry.liczba_kociolkow];

	gracz* gracze = new gracz[parametry.liczba_graczy];
	kociolek* kociolki = new kociolek[parametry.liczba_kociolkow];

	for (int i = 0; i < parametry.liczba_graczy; i++) {
		int* na_rece_k = new int[max_kart_na_rece];
		int* na_rece_w = new int[max_kart_na_rece];
		int* odpornosc = new int[parametry.liczba_kociolkow];
		gracze[i].na_rece_k = na_rece_k;
		gracze[i].na_rece_w = na_rece_w;
		gracze[i].odpornosc = odpornosc;
		for (int j = 0; j < parametry.liczba_kociolkow; j++) {
			gracze[i].odpornosc[j] = 0;
		}
	}

	parametry.zielone_wartosci = tab;
	parametry.wprowadzone_wartosci_innych = wartosci;
	parametry.wczytane = liczba_innych;

	for (int i = 0; i < parametry.liczba_kociolkow; i++) {
		parametry.wczytane[i] = 0;
	}
	
	wczytaj_parametry_inne(&parametry, nr_gracza);

	for (int i = 0; i < parametry.liczba_rund; i++){
		if (nr_gracza == 1) {
			int* talia = new int[parametry.liczba_kart_talia];
			int* talia_k = new int[parametry.liczba_kart_talia];

			for (int i = 0; i < parametry.liczba_kociolkow; i++) {
				kociolki[i].liczba_kart = 0;
			}

			for (int i = 0; i < parametry.liczba_graczy; i++) {
				gracze[i].karty_zabrane = 0;
			}

			utworz_talie(talia, talia_k, &parametry);
			potasuj_talie(talia, talia_k, parametry.liczba_kart_talia);
			rozdaj_karty(talia, talia_k, &parametry, gracze);
			zapisz_stan_gry(gracze, parametry.liczba_graczy, parametry.liczba_kociolkow, kociolki);
			wypisz_stan_gry(gracze, kociolki, nr_gracza, parametry.liczba_graczy, parametry.eksplozja, parametry.liczba_kociolkow);

			if (i == 0) {
				cout << "Waiting for other players\n";
			}
		}else {
			wczytaj_stan_gry(&parametry, gracze, kociolki);
		}

		if (nr_gracza == parametry.liczba_graczy) {
			zmien_gracza(1); //ostatni gracz po wczytaniu parametrów i stanu gry daje sygna³ do rozpoczêcia rozgrywki
		}

		cout << "Round " << i + 1 << endl;

		while (gracze[nr_gracza-1].karty_reka > 0) {
			int p = arbitr(nr_gracza);
			if (p != 0) {
				cout << "Error\n";
				return 0;
			}

			int poprawnosc = wczytaj_stan_gry(&parametry, gracze, kociolki); 
			if (poprawnosc != 0) {  //Jeœli wczytany stan gry jest niepoprawny program informuje pozosta³e i siê wy³¹cza
				cout << "Error\n";
				zmien_gracza(poprawnosc);  //Zmiana numeru w pliku wspó³dzielonym (poprawnosc > max_graczy)
				return 0;
			}

			wykonaj_ruch(kociolki, parametry.liczba_kociolkow, gracze, nr_gracza, &parametry);

			int n = (nr_gracza+1)%(parametry.liczba_graczy+1);  //zmienna przechowuj¹ca numer kolejnego gracza
			if (n == 0) {
				n = 1;
			}

			wypisz_stan_gry(gracze, kociolki, n, parametry.liczba_graczy, parametry.eksplozja, parametry.liczba_kociolkow);
			zapisz_stan_gry(gracze, parametry.liczba_graczy, parametry.liczba_kociolkow, kociolki);

			for (int i = 0; i < parametry.liczba_kociolkow; i++) {
				parametry.wczytane[i] = 0;
			}
			
			int czy_koniec = sprawdz_czy_koniec(gracze, parametry.liczba_graczy);
			if (czy_koniec == 1) {
				koniec_tury(&parametry, gracze, parametry.liczba_graczy);
			}else {
				zmien_gracza(n);
			}
		}
		
	}

	return 0;
}