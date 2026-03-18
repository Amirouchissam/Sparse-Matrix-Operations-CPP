#include <fstream>
#include <iostream>

#define MAX 100

using namespace std;

struct cellule
{
    int info;
    int ligne, col;
    cellule* SuivL;
    cellule* SuivC;
};

struct tmat
{
    cellule* TV[MAX];
    cellule* TVQ[MAX];
    cellule* TH[MAX];
    cellule* THQ[MAX];
};

typedef tmat* Pmat;
typedef cellule* Pcellule;

// 2 --------------------------------------------

void InitM(Pmat M)
{
    for (int i = 0; i < MAX; i++) {
        M->TV[i]  = 0;
        M->TVQ[i] = 0;
        M->TH[i]  = 0;
        M->THQ[i] = 0;
    }
}

// Wrapper ??? Init_Mat ???? ???????? ?? Somme ????
void Init_Mat(Pmat &M)
{
    M = new tmat;
    InitM(M);
}

// 1 --------------------------------------------

void CreerMat(Pmat &M, int &nbl, int &nbc, const char* name) {
    ifstream F(name);

    F >> nbl;
    F >> nbc;

    M = new tmat;
    InitM(M);

    for (int i = 0; i < nbl; i++) {
        for (int j = 0; j < nbc; j++) {
            int valeur;
            F >> valeur;

            if (valeur != 0) {
                Pcellule P = new cellule;

                P->info  = valeur;
                P->ligne = i;
                P->col   = j;
                P->SuivL = 0;
                P->SuivC = 0;

                // insertion ligne i
                if (M->TV[i] == 0) {
                    M->TV[i]  = P;
                    M->TVQ[i] = P;
                } else {
                    M->TVQ[i]->SuivL = P;
                    M->TVQ[i] = P;
                }

                // insertion colonne j
                if (M->TH[j] == 0) {
                    M->TH[j]  = P;
                    M->THQ[j] = P;
                } else {
                    M->THQ[j]->SuivC = P;
                    M->THQ[j] = P;
                }
            }
        }
    }
    F.close();
}

// 3---------------------

void Liberer(Pmat &M, int nbl, int nbc)
{
    if (M == 0) return;

    int i;
    Pcellule P, Q;

    for (i = 0; i < nbl; i++) {
        P = M->TV[i];
        while (P != 0) {
            Q = P;
            P = P->SuivL;
            delete Q;
        }
        M->TV[i]  = 0;
        M->TVQ[i] = 0;
    }

    for (i = 0; i < nbc; i++) {
        M->TH[i]  = 0;
        M->THQ[i] = 0;
    }

    delete M;
    M = 0;
}

// 4----------------------------------------

void Afficher_Mat(Pmat M, int nbl, int nbc) {
    int i, j, Deb;
    Pcellule P;

    for (i = 0; i < nbl; i++) {

        if (M->TV[i] == 0) {
            for (j = 0; j < nbc; j++) {
                cout << "0 ";
            }
            cout << endl;
        }
        else {
            P = M->TV[i];
            Deb = 0;

            while (P != 0) {
                for (j = Deb; j < P->col; j++) {
                    cout << "0 ";
                }

                cout << P->info << " ";

                Deb = P->col + 1;
                P = P->SuivL;
            }

            for (j = Deb; j < nbc; j++) {
                cout << "0 ";
            }
            cout << endl;
        }
    }
}

// 5- ajouter et la somme

void AjouterFin(Pmat M3, int elem, int i, int j) {
    Pcellule P = new cellule;
    P->info = elem;
    P->ligne = i;
    P->col = j;
    P->SuivL = 0;
    P->SuivC = 0;

    if (M3->TV[i] == 0) {
        M3->TV[i] = M3->TVQ[i] = P;
    } else {
        M3->TVQ[i]->SuivL = P;
        M3->TVQ[i] = P;
    }

    if (M3->TH[j] == 0) {
        M3->TH[j] = M3->THQ[j] = P;
    } else {
        M3->THQ[j]->SuivC = P;
        M3->THQ[j] = P;
    }
}

// ------------------ Somme ------------------
void Somme(Pmat M1, Pmat M2, Pmat &M3, int nbl, int nbc) {
    if (M1 == 0 && M2 == 0) {
        M3 = 0;
        return;
    }

    Init_Mat(M3);

    for (int i = 0; i < nbl; i++) {

        Pcellule P;
        Pcellule Q;

        if (M1 != 0) P = M1->TV[i];
        else         P = 0;

        if (M2 != 0) Q = M2->TV[i];
        else         Q = 0;

        while (P != 0 || Q != 0) {
            if (Q == 0 || (P != 0 && P->col < Q->col)) {
                AjouterFin(M3, P->info, i, P->col);
                P = P->SuivL;
            }
            else if (P == 0 || (Q != 0 && Q->col < P->col)) {
                AjouterFin(M3, Q->info, i, Q->col);
                Q = Q->SuivL;
            }
            else {
                int s = P->info + Q->info;
                if (s != 0) {
                    AjouterFin(M3, s, i, P->col);
                }
                P = P->SuivL;
                Q = Q->SuivL;
            }
        }
    }
}

//6----------------------------------

void Produit(Pmat M1, Pmat M2, Pmat &M4, int nbl, int nbc) {
    if (M1 == 0 || M2 == 0) {
        M4 = 0;
        return;
    }

    Init_Mat(M4);

    for (int i = 0; i < nbl; i++) {
        for (int j = 0; j < nbc; j++) {

            int somme = 0;

            // ligne i de M1
            Pcellule Q = M1->TV[i];

            // colonne j de M2
            Pcellule P = M2->TH[j];

            while (Q != 0 && P != 0) {

                if (Q->col == P->ligne) {
                    somme += Q->info * P->info;
                    Q = Q->SuivL;
                    P = P->SuivC;
                }
                else if (Q->col < P->ligne) {
                    Q = Q->SuivL;
                }
                else {
                    P = P->SuivC;
                }
            }

            if (somme != 0) {
                AjouterFin(M4, somme, i, j);
            }
        }
    }
}

void Sigma_Puissance(Pmat M, int n, Pmat &S, int nbl, int nbc) {

    if (M == 0 || n <= 0) {
        S = 0;
        return;
    }

    Pmat P;
    Pmat T; // (???? ?????? ???? ????? ????? ????)

    // P = M^1
    Init_Mat(P);
    Somme(M, 0, P, nbl, nbc);

    // S = M
    Init_Mat(S);
    Somme(M, 0, S, nbl, nbc);

    for (int k = 2; k <= n; k++) {
        // P = P * M
        Pmat NewP;
        Produit(P, M, NewP, nbl, nbc);
        Liberer(P, nbl, nbc);
        P = NewP;

        // S = S + P
        Pmat NewS;
        Somme(S, P, NewS, nbl, nbc);
        Liberer(S, nbl, nbc);
        S = NewS;
    }
}

// ------------------ Programme Principal ------------------
int main() {

    Pmat M1 = 0, M2 = 0, M3 = 0, M4 = 0, S = 0;
    int nbl1, nbc1, nbl2, nbc2;
    int choix;
    int n;

    do {
        cout << "\n========= MENU =========\n";
        cout << "1. Charger M1 depuis fichier\n";
        cout << "2. Charger M2 depuis fichier\n";
        cout << "3. Afficher M1\n";
        cout << "4. Afficher M2\n";
        cout << "5. Somme M1 + M2\n";
        cout << "6. Produit M1 * M2\n";
        cout << "7. Sigma Puissance de M1\n";
        cout << "0. Quitter\n";
        cout << "Votre choix : ";
        cin >> choix;

        switch (choix) {

        case 1:
        	int i,j,elem;
            CreerMat(M1, nbl1, nbc1, "mat1.txt");
            AjouterFin(M3, elem,i,j);
            int a;
			cout<<"entrer une valeur: "<<endl;
			cin>>a;
		    cout << "M1 chargee.\n";
            break;

        case 2:
            CreerMat(M2, nbl2, nbc2, "mat2.txt");
            cout << "M2 chargee.\n";
            break;

        case 3:
            if (M1 != 0)
                Afficher_Mat(M1, nbl1, nbc1);
            else
                cout << "M1 non chargee.\n";
            break;

        case 4:
            if (M2 != 0)
                Afficher_Mat(M2, nbl2, nbc2);
            else
                cout << "M2 non chargee.\n";
            break;

        case 5:
            if (M1 != 0 && M2 != 0 && nbl1 == nbl2 && nbc1 == nbc2) {
                Somme(M1, M2, M3, nbl1, nbc1);
                cout << "Resultat M3 = M1 + M2:\n";
                Afficher_Mat(M3, nbl1, nbc1);
            }
            else {
                cout << "Erreur: matrices non chargees ou dimensions incompatibles.\n";
            }
            break;

        case 6:
            if (M1 != 0 && M2 != 0) {
                Produit(M1, M2, M4, nbl1, nbc1);
                cout << "Resultat M4 = M1 * M2:\n";
                Afficher_Mat(M4, nbl1, nbc1);
            }
            else {
                cout << "Erreur: matrices non chargees.\n";
            }
            break;

        case 7:
            if (M1 != 0) {
                cout << "Donner n : ";
                cin >> n;
                Sigma_Puissance(M1, n, S, nbl1, nbc1);
                cout << "Resultat S = M1 + ... + M1^" << n << ":\n";
                Afficher_Mat(S, nbl1, nbc1);
            }
            else {
                cout << "Erreur: M1 non chargee.\n";
            }
            break;

        case 0:
            cout << "Fin du programme.\n";
            break;

        default:
            cout << "Choix invalide.\n";
        }

    } while (choix != 0);

    // Libération mémoire finale
    Liberer(M1, nbl1, nbc1);
    Liberer(M2, nbl2, nbc2);
    Liberer(M3, nbl1, nbc1);
    Liberer(M4, nbl1, nbc1);
    Liberer(S,  nbl1, nbc1);

    return 0;
}