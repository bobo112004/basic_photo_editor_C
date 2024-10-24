//Popescu Bogdan Stefan 312CAa 2023-2024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//functia clamp pentru a aduce pixelul intre 0 si 255
int clamp(int x)
{
	if (x < 0)
		return 0;
	if (x > 255)
		return 255;
	return x;
}

//functie de alocare a memoriei pentru o matrice
void stoc(int m, int n, int ***a_ptr)
{
	int **a;
	//alocam exact m linii si n coloane si verificam daca exista sau nu memorie
	a = malloc(m * sizeof(int *));
	if (!a) {
		*a_ptr = NULL;
		return;
	}
	for (int i = 0; i < m; i++) {
		a[i] = calloc(n, sizeof(int));
		if (!a[i]) {
			for (int j = 0; j < i; j++)
				free(a[i]);
			free(a);
			*a_ptr = NULL;
			return;
			//daca nu am gasit memorie, am eliberat ce am alocat inainte
		}
	}
	*a_ptr = a;
}

//functia verifica daca 
int check(int **p, char *name)
{
	if (!p) {
		printf("Failed to load %s\n", name);
		return 1;
	}
	return 0;
}

void task_load(char **name, char *type, int ***pic, int *m_ptr, int *n_ptr,
			   int *loaded, char ***v, int *x1, int *x2, int *y1, int *y2)
{
	int m, n, max;
	//eliberam matricea din trecut daca incarcam o noua imagine
	if (*loaded) {
		for (int i = 0; i < *m_ptr; i++)
			free((*pic)[i]);
		free(*pic);
	}
	//la fel si la celelalte resurse, cum ar fi numele
	if (*name)
		free(*name);
	*name = malloc((strlen((*v)[1]) + 1) * sizeof(char));
	strcpy(*name, (*v)[1]);
	//deschidem fisierul si verificam daca exista sau nu
	FILE *f = fopen(*name, "rb");
	if (!f) {
		printf("Failed to load %s\n", *name);
		(*loaded) = 0;
		return;
	}
	//extragem tipul, dimensiunile si valoarea maxima (mereu 255)
	fscanf(f, "%s", type);
	fscanf(f, "%d%d", &n, &m);
	fscanf(f, "%d", &max);
	//daca tipul este P2, citim normal din fisier
	if (strcmp(type, "P2") == 0) {
		stoc(m, n, pic);
		if (check(*pic, *name))
			return;
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n; j++)
				fscanf(f, "%d", &(*pic)[i][j]);
		}
	} else if (strcmp(type, "P5") == 0) {
		//daca tipul este P5, atunci va trebui sa citim matricea binar, cate
		//un octet (deoarece valorile sunt cuprinse intre 0 si 255)
		stoc(m, n, pic);
		if (check(*pic, *name))
			return;
		int l;
		fread(&l, 1, 1, f);
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n; j++)
				fread(&(*pic)[i][j], 1, 1, f);
		}
	} else if (strcmp(type, "P3") == 0) {
		//analog si la P3 respectiv P6, insa matricea va avea de 3 ori mai
		//multe coloane din cauza celor 3 culori RGB
		stoc(m, 3 * n, pic);
		if (check(*pic, *name))
			return;
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < 3 * n; j++)
				fscanf(f, "%d", &(*pic)[i][j]);
		}
	} else if (strcmp(type, "P6") == 0) {
		stoc(m, 3 * n, pic);
		if (check(*pic, *name))
			return;
		int l;
		fread(&l, 1, 1, f);
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < 3 * n; j++)
				fread(&(*pic)[i][j], 1, 1, f);
		}
	}
	//ne-am asigurat de fiecare data ca a existat destul spatiu pentru matricea
	//de pixeli si in caz contrar afisam eroarea
	fflush(f);
	fclose(f);
	//actualizam variabilele de selectie si dimensiunile cand incarcam ceva nou
	*x1 = 0;
	*x2 = n;
	*y1 = 0;
	*y2 = m;
	printf("Loaded %s\n", *name);
	(*loaded) = 1;
	*m_ptr = m;
	*n_ptr = n;
}

//functia verifiva daca un string e format doar din cifre, adica daca este
//un numar
int check_num(char *s)
{
	int l = strlen(s);
	for (int i = 0; i < l; i++) {
		if (strchr("-0123456789", s[i]) == 0)
			return 0;
	}
	return 1;
}

//functia pentru a selecta o zona in matrice
void task_select(int *loaded, int *m, int *n, char ***v, int *nrp,
				 int *x1_p, int *x2_p, int *y1_p, int *y2_p)
{
	int x1, x2, y1, y2, aux;
	if ((*loaded) == 0) {
		printf("No image loaded\n");
	} else {
		if (*nrp == 2 && strcmp((*v)[1], "ALL") == 0) {
			//daca selectia este ALL, variabilele de selectie devin 0 respectiv
			// dimensiunea liniei/coloanei
			*x1_p = 0;
			*x2_p = *n;
			*y1_p = 0;
			*y2_p = *m;
			printf("Selected ALL\n");
		} else {
			if (*nrp == 5) {
				//daca toti parametri sunt numere, atunci putem sa ii verificam
				if (check_num((*v)[1]) && check_num((*v)[2]) &&
				check_num((*v)[3]) && check_num((*v)[4])) {
					x1 = atoi((*v)[1]);
					y1 = atoi((*v)[2]);
					x2 = atoi((*v)[3]);
					y2 = atoi((*v)[4]);
					//ordonam coordonatele
					if (x1 > x2) {
						aux = x1;
						x1 = x2;
						x2 = aux;
					}
					if (y1 > y2) {
						aux = y1;
						y1 = y2;
						y2 = aux;
					}
					//punema conditia ca selectia sa fie valida in functie de
					//dimensiunile matricei
					if (x1 >= 0 && x1 <= *n && y1 >= 0 && y1 <= *m &&
						x2 >= 0 && x2 <= *n && y2 >= 0 && y2 <= *m &&
						x1 != x2 && y1 != y2) {
						*x1_p = x1;
						*x2_p = x2;
						*y1_p = y1;
						*y2_p = y2;
						printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
					} else {
						printf("Invalid set of coordinates\n");
					}
				} else {
					printf("Invalid command\n");
				}
			} else {
				printf("Invalid command\n");
			}
		}
	}
}

//functia ce va lua toata comanda si o va parsa in mai multi parametri
//si ne va da si numarul de parametri, util atunci cand testam daca o 
//comanda este valida sau nu
void parsare(char *s, char ***v_ptr, int *nrp)
{
	//ne vom folosi de un "vector" de cuvinte
	char **v;
	int nrv = 1, l, nr = 0;
	v = malloc(sizeof(char *));
	if (!v) {
		printf("Nu s-a putut aloca memorie!");
		exit(-1);
	}
	if (!strchr(s, ' ')) {
		l = strlen(s);
		v[0] = malloc((l + 1) * sizeof(char));
		if (!v[0]) {
			printf("Nu s-a putut aloca memorie!");
			exit(-1);
		}
		//verificam daca comanda contine unul sau mai multe cuvinte
		//pentru a putea parcurge cu parsarea
		strcpy(v[0], s);
		nr++;
	} else {
		char *p = strtok(s, " ");
		//parcurgem cu strtok si salvam parametrul in vectorul de parametri
		//alocat dinamic si realocat atunci cand este nevoie
		while (p) {
			l = strlen(p);
			nr++;
			if (nr > nrv) {
				v = realloc(v, 2 * nrv * sizeof(char *));
				if (!v) {
					printf("Nu s-a putut aloca memorie!");
					exit(-1);
				}
				nrv = 2 * nrv;
			}
			v[nr - 1] = malloc((l + 1) * sizeof(char));
			if (!v[nr - 1]) {
				printf("Nu s-a putut aloca memorie!");
				exit(-1);
			}
			strcpy(v[nr - 1], p);
			p = strtok(NULL, " ");
		}
	}
	*v_ptr = v;
	*nrp = nr;
}

//functia verifica daca un numar este putere a lui 2
int check_pow(int y)
{
	if (y >= 2 && y <= 256) {
		//cat timp numarul se poate imparti fara rest, il impartim
		while (y % 2 == 0)
			y = y / 2;
		if (y == 1)
		//in functie de ce ne-a ramas, stim sau nu daca este putere a lui 2
			return 1;
		else
			return 0;
	} else {
		return 0;
	}
}

void task_histogram(int ***pic, char *type, int loaded, int m, int n,
					char ***v, int nrp)
{
	int x, y;
	int *fv, grupe, fmax = 0, fcur, nrs;
	//vom avea nevoie de un vector de frecventa de fix 265 de elemente
	//pe care il vom aloca (si elibera) dinamic
	fv = calloc(256, sizeof(int));
	if (!fv) {
		printf("Nu s-a putut aloca memorie!\n");
		exit(-1);
	}
	if (loaded == 0) {
		printf("No image loaded\n");
	} else {
		if(nrp<3) {
			printf("Invalid command\n");
		} else {
			//histograma se aplica doar daca avem o imagine grayscale
		if (strcmp(type, "P2") == 0 || strcmp(type, "P5") == 0) {
				//trebuie sa verificam ca x si y sa fie numere si ca y sa fie
				//putere a lui 2 pentru a continua cu histograma
				if (check_num((*v)[1]) && check_num((*v)[2])) {
					x = atoi((*v)[1]);
					y = atoi((*v)[2]);
					if (check_pow(y) == 0) {
						printf("Invalid set of parameters\n");
					} else {
						//parcurgem matricea si umplem vectorul de frecventa
						for (int i = 0; i < m; i++)
							for (int j = 0; j < n; j++)
								fv[(*pic)[i][j]]++;
						grupe = 256 / y;
						//aplicam formula impartind vectorul in mai multe grupe
						//si calculam frecventa maxima pentru a o putea folosi
						//in formula
						for (int i = 0; i < 256; i = i + grupe) {
							fcur = 0;
							for (int j = i; j < i + grupe; j++)
								fcur = fcur + fv[j];
							if (fcur > fmax)
								fmax = fcur;
						}
						//dupa ce aplicam formula trebuie doar sa afisam
						//conform cerintei
						for (int i = 0; i < 256; i = i + grupe) {
							fcur = 0;
							for (int j = i; j < i + grupe; j++)
								fcur = fcur + fv[j];
							nrs = floor((double)(fcur * x / fmax));
							printf("%d\t|\t", nrs);
							for (int k = 0; k < nrs; k++)
								printf("%c", '*');
							printf("\n");
						}
					}
				} else {
					printf("Inlavid command\n");
				}
		} else {
			printf("Black and white image needed\n");
		}
		}
		
	}
	free(fv);
}

void task_equalize(int ***pic, char *type, int loaded, int m, int n)
{
	if (loaded == 0) {
		printf("No image loaded\n");
	} else {
		//egalizarea se aplica doar pentru grayscale si verificam
		if (strcmp(type, "P2") == 0 || strcmp(type, "P5") == 0) {
			int *fv, sum = 0, **copy;
			stoc(m, n, &copy);
			if (!copy) {
				printf("Nu s-a putut aloca memorie!\n");
				exit(-1);
			}
			//utilizam un alt vector de frecventa alocat dinamic
			fv = calloc(256, sizeof(int));
			if (!fv) {
				printf("Nu s-a putut aloca memorie!\n");
				exit(-1);
			}
			//umplem vectorul de frecventa
			for (int i = 0; i < m; i++)
				for (int j = 0; j < n; j++)
					fv[(*pic)[i][j]]++;
			for (int i = 0; i < m; i++) {
				for (int j = 0; j < n; j++) {
					sum = 0;
					for (int k = 0; k <= (*pic)[i][j]; k++)
						sum = sum + fv[k];
					copy[i][j] = clamp(round((double)(255 * sum / (n * m))));
					//am aplicat formula fiecarui pixel si am copiat pixelul
					//intr-o copie a pozei curente
				}
			}
			//eliberam poza curenta si o inlocuim (mai bine zic inlocuim
			// pointerul) cu copia noastra
			for (int i = 0; i < m; i++)
				free((*pic)[i]);
			free((*pic));
			free(fv);
			*pic = copy;
			printf("Equalize done\n");
		} else {
			printf("Black and white image needed\n");
		}
	}
}

//functie de swap
void swap(int *x, int *y)
{
	int aux;
	aux = *x;
	*x = *y;
	*y = aux;
}

//functia mai intai va transpune matricea, apoi va interschimba coloanele
//simetrice fata de mijloc
void rotate90_simplu(int ***a, int x1, int x2, int y1, int y2)
{
	for (int i = y1; i < y2; i++) {
		for (int j = i + 1; j < x2; j++)
			swap(&(*a)[i][j], &(*a)[j][i]);
	}
	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < (x2 - x1) / 2; j++)
			swap(&(*a)[i][j], &(*a)[i][x1 + x2 - j - 1]);
	}
}

//acelasi rationament ca la functia anterioara, transpunem matricea si dupa
//interschimbam coloanele simetrice fata de mijloc
//algoritmul difera deoarece avem cate 3 culori pentru un pixel
void rotate90_rgb(int ***a, int x1, int x2, int y1, int y2)
{
	for (int i = y1; i < y2; i++) {
		for (int j = i + 1; j < x2; j++) {
			swap(&(*a)[i][3 * j], &(*a)[j][3 * i]);
			swap(&(*a)[i][3 * j + 1], &(*a)[j][3 * i + 1]);
			swap(&(*a)[i][3 * j + 2], &(*a)[j][3 * i + 2]);
		}
	}
	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < (x2 - x1) / 2; j++) {
			swap(&(*a)[i][3 * j], &(*a)[i][(x2 - j) * 3]);
			swap(&(*a)[i][3 * j + 1], &(*a)[i][(x2 - j) * 3 + 1]);
			swap(&(*a)[i][3 * j + 2], &(*a)[i][(x2 - j) * 3 + 2]);
		}
	}
}

//functia pentru rotirea unei zone selectate sau a intregii imagini
void task_rotate(int ***a, char *type, int loaded, int x1, int x2, int y1,
				 int y2, char ***v, int n, int m)
{
	if (loaded == 0) {
		printf("No image loaded\n");
	} else {
		if (x1 == 0 && x2 == n && y1 == 0 && y2 == m) {
			//retinem unghiul pentru rotire ca sa il putem afisa ;la urma
			int r = atoi((*v)[1]), r1;
				r1 = r;
				if (r % 90) {
					printf("Unsuported rotation angle\n");
				} else {
					if(r==0 || r==360 || r==-360) {
						//daca avem 0 sau +-360, imaginea este aceeasi
						printf("Rotated %d\n", r);
					} else {
						//avem doua functii, una pentru rotirea imaginii grayscale
						//iar alta pentru imagini color
						if (strcmp(type, "P2") == 0 || strcmp(type, "P5") == 0) {
							if (r > 0) {
								//daca avem unghi negativ, adica rotire la stanga,
								//vom roti de fapt de 3 ori la dreapta
								while (r) {
									rotate90_simplu(a, x1, x2, y1, y2);
									r = r - 90;
								}
							} else {
								while (r) {
									rotate90_simplu(a, x1, x2, y1, y2);
									rotate90_simplu(a, x1, x2, y1, y2);
									rotate90_simplu(a, x1, x2, y1, y2);
									r = r + 90;
								}
							}
						} else {
							if (r > 0) {
								//aici apelam functia pentru rotirea imaginii
								//color, asemanator ca inainte
								while (r) {
									rotate90_rgb(a, x1, x2, y1, y2);
									r = r - 90;
								}
							} else {
								while (r) {
									rotate90_rgb(a, x1, x2, y1, y2);
									rotate90_rgb(a, x1, x2, y1, y2);
									rotate90_rgb(a, x1, x2, y1, y2);
									r = r + 90;
								}
							}
						}
						printf("Rotated %d\n", r1);
					}
					
				}
		} else {
			if ((x2 - x1) != (y2 - y1)) {
				printf("The selection must be square\n");
			} else {
				//acelasi rationament ca la rotirea intregii imagini
				//doar ca este aplicata unei zone patrate (am verificat)
				int r = atoi((*v)[1]), r1;
				r1 = r;
				if (r % 90) {
					printf("Unsuported rotation angle\n");
				} else {
					if(r==0 || r==360 || r==-360) {
						//daca avem 0 sau +-360, imaginea este aceeasi
						printf("Rotated %d\n", r);
					} else {
						//avem doua functii, una pentru rotirea imaginii grayscale
						//iar alta pentru imagini color
						if (strcmp(type, "P2") == 0 || strcmp(type, "P5") == 0) {
							if (r > 0) {
								//daca avem unghi negativ, adica rotire la stanga,
								//vom roti de fapt de 3 ori la dreapta
								while (r) {
									rotate90_simplu(a, x1, x2, y1, y2);
									r = r - 90;
								}
							} else {
								while (r) {
									rotate90_simplu(a, x1, x2, y1, y2);
									rotate90_simplu(a, x1, x2, y1, y2);
									rotate90_simplu(a, x1, x2, y1, y2);
									r = r + 90;
								}
							}
						} else {
							if (r > 0) {
								//aici apelam functia pentru rotirea imaginii
								//color, asemanator ca inainte
								while (r) {
									rotate90_rgb(a, x1, x2, y1, y2);
									r = r - 90;
								}
							} else {
								while (r) {
									rotate90_rgb(a, x1, x2, y1, y2);
									rotate90_rgb(a, x1, x2, y1, y2);
									rotate90_rgb(a, x1, x2, y1, y2);
									r = r + 90;
								}
							}
						}
						printf("Rotated %d\n", r1);
					}
				}
			}
		}
	}
}

void task_apply(int ***pic, char *type, int loaded, char ***v,
				int x1, int x2, int y1, int y2, int m, int n, int nrp)
{
	if (loaded == 0) {
		printf("No image loaded\n");
	} else {
		if (nrp == 2) {
			//pentru imaginile grayscale, vom pune intr-o copie pixelii
			//care nu sutn afectati de efect, adica cei din afara selectiei
			//si cei de pa margine
			int ok1=0;
			//folosim ok1 ca sa contorizam faptul ca APPLY s-a efectuat bine
			if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0) {
				int **copy;
				stoc(m, n * 3, &copy);
				for (int i = 0; i < m; i++) {
					for (int j = 0; j < n * 3; j++) {
						if (i < y1 || i >= y2 || j < x1 * 3 || j >= x2 * 3 ||
							i == 0 || i == m - 1 || j < 3 || j >= 3 * (n - 1))
							copy[i][j] = (*pic)[i][j];
					}
				}
				//pentru functia EDGE, aplicam formula la pixelii aflati in 
				//selectie, in timpul parcurgerii matricei, folosindu-ne
				//de vecini
				if (strcmp((*v)[1], "EDGE") == 0) {
					ok1=1;
					int sum;
					for (int i = 1; i < m - 1; i++) {
						for (int j = 3; j < 3 * (n - 1); j++) {
							if (i >= y1 && i < y2 && j >= 3 * x1 && j < 3 * x2) {
								sum = 0;
								sum = sum - (*pic)[i - 1][j] - (*pic)[i + 1][j] -
									  (*pic)[i][j - 3] - (*pic)[i][j + 3] -
									  (*pic)[i - 1][j - 3] - (*pic)[i - 1][j + 3] -
									  (*pic)[i + 1][j - 3] - (*pic)[i + 1][j + 3] +
									  8 * (*pic)[i][j];
								copy[i][j] = clamp(sum);
								//folosim functia clamp deoarece rezultatul
								//nu se afla obligatoriu in intervalul dorit
							}
						}
					}
				}
				//pentru sharpen rationamentul este identic, parcurgem si
				//aplicam formula fiecarui pixel folosindu-ne de vecini
				if (strcmp((*v)[1], "SHARPEN") == 0) {
					ok1=1;
					int sum;
					for (int i = 1; i < m - 1; i++) {
						for (int j = 3; j < 3 * (n - 1); j++) {
							if (i >= y1 && i < y2 && j >= 3 * x1 && j < 3 * x2) {
								sum = 0;
								sum = sum - (*pic)[i - 1][j] -
									  (*pic)[i + 1][j] -
									  (*pic)[i][j - 3] - (*pic)[i][j + 3] +
									  5 * (*pic)[i][j];
								copy[i][j] = clamp(sum);
								//utilizam iarasi sharpen
							}
						}
					}
				}
				//pentru functia blur, ne folosim si de functia round, iar 
				//variabila sum este acum de tip double deoarece avem si
				//impartiri
				if (strcmp((*v)[1], "BLUR") == 0) {
					ok1=1;
					double sum;
					for (int i = 1; i < m - 1; i++) {
						for (int j = 3; j < 3 * (n - 1); j++) {
							if (i >= y1 && i < y2 && j >= 3 * x1 &&
								j < 3 * x2) {
								sum = 0;
								sum = sum + ((*pic)[i - 1][j] +
								(*pic)[i + 1][j] + (*pic)[i][j - 3] +
								(*pic)[i][j + 3] + (*pic)[i - 1][j - 3] +
								(*pic)[i - 1][j + 3] + (*pic)[i + 1][j - 3] +
								(*pic)[i + 1][j + 3] + (*pic)[i][j]) / 9;
								copy[i][j] = clamp(round(sum));
							}
						}
					}
				}
				//gaussian_blur este asemanator cu blur-ul normal, ne folosim
				//de vecini si de o suma de tip double ca sa putem calcula
				//noua valoare a unui pixel
				if (strcmp((*v)[1], "GAUSSIAN_BLUR") == 0) {
					ok1=1;
					double sum;
					for (int i = 1; i < m - 1; i++) {
						for (int j = 3; j < 3 * (n - 1); j++) {
							if (i >= y1 && i < y2 && j >= 3 * x1 && j < 3 * x2) {
								sum = 0;
								sum = sum + (2 * (*pic)[i - 1][j] + 2 * (*pic)[i + 1][j] +
											 2 * (*pic)[i][j - 3] + 2 * (*pic)[i][j + 3] +
											 (*pic)[i - 1][j - 3] + (*pic)[i - 1][j + 3] +
											 (*pic)[i + 1][j - 3] + (*pic)[i + 1][j + 3] +
											 4 * (*pic)[i][j]) /
												16;
								copy[i][j] = clamp(round(sum));
							}
						}
					}
				}
				if(ok1) {
					for (int i = 0; i < m; i++)
					free((*pic)[i]);
				free((*pic));
				*pic = copy;
				printf("APPLY %s done\n", (*v)[1]);
				} else {
					printf("APPLY parameter invalid\n");
					for(int i=0;i<m;i++)
						free(copy[i]);
					free(copy);
				}	
			} else {
				printf("Easy, Charlie Chaplin\n");
			}
		} else {
			printf("Invalid command\n");
		}
		//mai sus avem tratate celelalte cazuri cand tipul de imagine e
		//incorecta sau cand comanda in sine este invalida
	}
}

void task_save(int ***pic, int loaded, int nrp, char ***v, int m, int n,
			   char *type)
{
	if (loaded == 0) {
		printf("No image loaded\n");
	} else {
		FILE *f;
		//aici ne folosim de numarul de parametri si verificam daca al 3-lea
		//parametru (daca exista) este "ascii"
		if (nrp == 2) {
			//daca nu exista, deschidem in format binar si scriem mai intai
			//antetul intalnit si cand am citit din fisier, insa schimbam
			//tipul pozei in functie de cerinta (daca este binar, color etc)
			f = fopen((*v)[1], "wb");
			if (strcmp(type, "P2") == 0)
				fprintf(f, "%s\n", "P5");
			else if (strcmp(type, "P3") == 0)
				fprintf(f, "%s\n", "P6");
			else
				fprintf(f, "%s\n", type);
			fprintf(f, "%d %d\n", n, m);
			fprintf(f, "255\n");
			//scriem elementele din matrice (in functie daca este color sau nu)
			//cate un octet
			if (strcmp(type, "P2") == 0 || strcmp(type, "P5") == 0) {
				for (int i = 0; i < m; i++) {
					for (int j = 0; j < n; j++)
						fwrite(&(*pic)[i][j], 1, 1, f);
				}
			} else {
				for (int i = 0; i < m; i++) {
					for (int j = 0; j < 3 * n; j++)
						fwrite(&(*pic)[i][j], 1, 1, f);
				}
			}
			fflush(f);
			fclose(f);
		} else {
			//daca ne cere sa il salvam ascii deschidem fisierul in modul text
			f = fopen((*v)[1], "w");
			if (strcmp(type, "P5") == 0)
				fprintf(f, "%s\n", "P2");
			else if (strcmp(type, "P6") == 0)
				fprintf(f, "%s\n", "P3");
			else
				fprintf(f, "%s\n", type);
			fprintf(f, "%d %d\n", n, m);
			fprintf(f, "255\n");
			if (strcmp((*v)[2], "ascii") == 0) {
				//scriem element cu element in fisier
				if (strcmp(type, "P2") == 0 || strcmp(type, "P5") == 0) {
					for (int i = 0; i < m; i++) {
						for (int j = 0; j < n; j++)
							fprintf(f, "%d ", (*pic)[i][j]);
						fprintf(f, "\n");
					}
				} else {
					for (int i = 0; i < m; i++) {
						for (int j = 0; j < 3 * n; j++)
							fprintf(f, "%d ", (*pic)[i][j]);
						fprintf(f, "\n");
					}
				}
			} else {
				printf("Invalid command\n");
			}
			fclose(f);
		}
		printf("Saved %s\n", (*v)[1]);
	}
}

void task_exit(int ***pic, int m, char **name, char **type, int loaded)
{
	//verificam daca imaignea este incarcata si daca da, eliberam tot
	if (loaded) {
		//deoarece unele variabile pot fi eliberate in alte functii (pentru)
		//a avea un program cat mai optim, verificam daca sunt sau nu eliberate
		if ((*pic) != NULL) {
			for (int i = 0; i < m; i++)
				free((*pic)[i]);
			free((*pic));
		}
		if (*type)
			free(*type);
		if (*name)
			free(*name);
	} else {
		printf("No image loaded\n");
	}
}

void task_crop(int ***pic, int loaded, int *x1_p, int *x2_p, int *y1_p, int *y2_p, char *type, int *m, int *n)
{
	if (loaded) {
		int **copy;
		int m1, n1, x1, x2, y1, y2;
		x1 = *x1_p; x2 = *x2_p;	y1 = *y1_p;	y2 = *y2_p;
		m1 = y2 - y1; n1 = x2 - x1;
		//m1 si n1 vor fi noile dimensiuni dupa cropare
		//daca avem o imagine grayscale, copiem element cu element intr-o
		//matrice copie
		if (strcmp(type, "P2") == 0 || strcmp(type, "P5") == 0) {
			stoc(m1, n1, &copy);
			for (int i = y1; i < y2; i++) {
				for (int j = x1; j < x2; j++)
					copy[i - y1][j - x1] = (*pic)[i][j];
			}
		} else {
			//daca avem o imagine color, vom tine cont de faptul ca avem de 3
			//ori mai multe elemente pe linii si vom copia iarasi
			stoc(m1, 3 * n1, &copy);
			for (int i = y1; i < y2; i++) {
				for (int j = 3 * x1; j < 3 * x2; j++)
					copy[i - y1][j - 3 * x1] = (*pic)[i][j];
			}
		}
		//eliberam iarasi ce aveam inainte in matrice si o inlocuim cu matricea
		//copie de dinainte
		//de asemenea, actualizam noile dimensiuni si coordonatele de selectie
		for (int i = 0; i < *m; i++)
			free((*pic)[i]);
		free((*pic));
		*pic = copy;
		*m = m1;
		*n = n1;
		*x1_p = 0;
		*y1_p = 0;
		*x2_p = n1;
		*y2_p = m1;
		printf("Image cropped\n");
	} else {
		printf("No image loaded\n");
	}
}

int main(void)
{
	char *com, *name = NULL, *type, **param;
	int **pic, m = 0, n = 0, loaded = 0, nrp = 0, x1, x2, y1, y2;
	//alocam memorie pentru variabilele nume si tip
	type = malloc(3 * sizeof(char));
	com = malloc(101 * sizeof(char));
	if (!type) {
		printf("Nu s-a putut aloca memorie!");
		exit(-1);
	}
	while (1) {
		//deorarece dupa exit poate sa nu existe enter (e ultima comanda)
		//mai intai citim cu fgets si verificam daca este sau nu cazul sa
		//apelam functia de EXIT
		fgets(com, 5, stdin);
		if (strcmp(com, "EXIT") == 0) {
			task_exit(&pic, m, &name, &type, loaded);
			free(com);
			break;
		} else {
			//daca nu este cazul, citim si restul comenzii si apelam functia
			//de parsare a comenzi in parametri
			fgets(com + 4, 96, stdin);
			com[strlen(com) - 1] = '\0';
			parsare(com, &param, &nrp);
			//in functie de parametrul 0 vom decide ce functie apelam
			if (strcmp(param[0], "LOAD") == 0 && nrp == 2)
				task_load(&name, type, &pic, &m, &n, &loaded, &param, &x1,
						  &x2, &y1, &y2);
			else if (strcmp(param[0], "SELECT") == 0)
				task_select(&loaded, &m, &n, &param, &nrp, &x1, &x2, &y1, &y2);
			else if (strcmp(param[0], "HISTOGRAM") == 0 && nrp <= 3)
				task_histogram(&pic, type, loaded, m, n, &param, nrp);
			else if (strcmp(param[0], "EQUALIZE") == 0 && nrp == 1)
				task_equalize(&pic, type, loaded, m, n);
			else if (strcmp(param[0], "SAVE") == 0 && (nrp == 2 || nrp == 3))
				task_save(&pic, loaded, nrp, &param, m, n, type);
			else if (strcmp(param[0], "APPLY") == 0)
				task_apply(&pic, type, loaded, &param, x1,
						   x2, y1, y2, m, n, nrp);
			else if (strcmp(param[0], "CROP") == 0 && nrp == 1)
				task_crop(&pic, loaded, &x1, &x2, &y1, &y2, type, &m, &n);
			else if (strcmp(param[0], "ROTATE") == 0 && nrp == 2)
				task_rotate(&pic, type, loaded, x1, x2, y1, y2, &param, n, m);
			else
				printf("Invalid command\n");
			//de fiecare data eliberam memoria in vectorul de cuvinte
			//pentru a putea sa stocam o altaa comanda in el
			for (int i = 0; i < nrp; i++)
				free(param[i]);
			free(param);
		}
	}
	return 0;
}
