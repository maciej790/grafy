#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <chrono>
#include <iomanip>

using namespace std;

// Klasa reprezentująca graf
class Graph {
public:
    int V; // Liczba wierzchołków
    vector<vector<int>> macierzSasiedztwa;
    vector<vector<pair<int, int>>> listaSasiedztwa;

    Graph(int V) : V(V) {
        macierzSasiedztwa.resize(V, vector<int>(V, numeric_limits<int>::max()));
        listaSasiedztwa.resize(V);
    }

    void dodajKrawedz(int u, int v, int waga) {
        macierzSasiedztwa[u][v] = waga;
        listaSasiedztwa[u].push_back({v, waga});
    }

    void drukujMacierzSasiedztwa() {
        cout << "Macierz sasiedztwa:" << endl;
        for (int i = 0; i < V; ++i) {
            for (int j = 0; j < V; ++j) {
                if (macierzSasiedztwa[i][j] == numeric_limits<int>::max())
                    cout << "INF ";
                else
                    cout << macierzSasiedztwa[i][j] << " ";
            }
            cout << endl;
        }
    }

    void drukujListeSasiedztwa() {
        cout << "Lista sasiedztwa:" << endl;
        for (int i = 0; i < V; ++i) {
            cout << i << ": ";
            for (auto& krawedz : listaSasiedztwa[i]) {
                cout << "(" << krawedz.first << ", " << krawedz.second << ") ";
            }
            cout << endl;
        }
    }
};

// Klasa reprezentująca kolejkę priorytetową
class KolejkaPriorytetowa {
private:
    vector<pair<int, int>> kopiec; // Para: (waga, wierzchołek)

    void naprawKopiecWgGory(int idx) {
        while (idx > 0 && kopiec[idx].first < kopiec[(idx - 1) / 2].first) {
            swap(kopiec[idx], kopiec[(idx - 1) / 2]);
            idx = (idx - 1) / 2;
        }
    }

    void naprawKopiecWgDolu(int idx) {
        int najmniejszy = idx;
        int lewy = 2 * idx + 1;
        int prawy = 2 * idx + 2;
        if (lewy < kopiec.size() && kopiec[lewy].first < kopiec[najmniejszy].first)
            najmniejszy = lewy;
        if (prawy < kopiec.size() && kopiec[prawy].first < kopiec[najmniejszy].first)
            najmniejszy = prawy;
        if (najmniejszy != idx) {
            swap(kopiec[idx], kopiec[najmniejszy]);
            naprawKopiecWgDolu(najmniejszy);
        }
    }

public:
    bool pusty() {
        return kopiec.empty();
    }

    void dodaj(int waga, int wierzcholek) {
        kopiec.push_back({waga, wierzcholek});
        naprawKopiecWgGory(kopiec.size() - 1);
    }

    pair<int, int> usun() {
        auto min = kopiec.front();
        swap(kopiec.front(), kopiec.back());
        kopiec.pop_back();
        naprawKopiecWgDolu(0);
        return min;
    }
};

// Funkcja realizująca algorytm Dijkstry dla macierzy sąsiedztwa
void dijkstraMacierz(Graph& graf, int src, int dest, double& czasWykonania) {
    vector<int> odleglosci(graf.V, numeric_limits<int>::max());
    vector<int> rodzic(graf.V, -1);
    KolejkaPriorytetowa kolejka;
    odleglosci[src] = 0;
    kolejka.dodaj(0, src);

    auto startMacierz = chrono::high_resolution_clock::now();

    while (!kolejka.pusty()) {
        pair<int, int> obecny = kolejka.usun();
        int obecnaOdleglosc = obecny.first;
        int u = obecny.second;
        if (obecnaOdleglosc > odleglosci[u])
            continue;

        for (int v = 0; v < graf.V; ++v) {
            if (graf.macierzSasiedztwa[u][v] != numeric_limits<int>::max()) {
                int waga = graf.macierzSasiedztwa[u][v];
                if (odleglosci[u] + waga < odleglosci[v]) {
                    odleglosci[v] = odleglosci[u] + waga;
                    rodzic[v] = u;
                    kolejka.dodaj(odleglosci[v], v);
                }
            }
        }
    }

    auto koniecMacierz = chrono::high_resolution_clock::now();
    chrono::duration<double> czasZnajdowaniaMacierz = koniecMacierz - startMacierz;
    czasWykonania = czasZnajdowaniaMacierz.count();

    cout << "Najkrotsza sciezka z " << src << " do " << dest << " wynosi " << odleglosci[dest] << endl;
    cout << "Czas wykonania (macierzowa): " << czasWykonania << " sekund\n";
}

// Funkcja realizująca algorytm Dijkstry dla listy sąsiedztwa
void dijkstraLista(Graph& graf, int src, int dest, double& czasWykonania) {
    vector<int> odleglosci(graf.V, numeric_limits<int>::max());
    vector<int> rodzic(graf.V, -1);
    KolejkaPriorytetowa kolejka;
    odleglosci[src] = 0;
    kolejka.dodaj(0, src);

    auto startLista = chrono::high_resolution_clock::now();

    while (!kolejka.pusty()) {
        pair<int, int> obecny = kolejka.usun();
        int obecnaOdleglosc = obecny.first;
        int u = obecny.second;
        if (obecnaOdleglosc > odleglosci[u])
            continue;

        for (auto& krawedz : graf.listaSasiedztwa[u]) {
            int v = krawedz.first;
            int waga = krawedz.second;
            if (odleglosci[u] + waga < odleglosci[v]) {
                odleglosci[v] = odleglosci[u] + waga;
                rodzic[v] = u;
                kolejka.dodaj(odleglosci[v], v);
            }
        }
    }

    auto koniecLista = chrono::high_resolution_clock::now();
    chrono::duration<double> czasZnajdowaniaLista = koniecLista - startLista;
    czasWykonania = czasZnajdowaniaLista.count();

    cout << "Najkrotsza sciezka z " << src << " do " << dest << " wynosi " << odleglosci[dest] << endl;
    cout << "Czas wykonania (listowa): " << czasWykonania << " sekund\n";
}

// Funkcja generująca losowy graf
Graph* generujLosowyGraf(int V, int gestosc) {
    Graph* graf = new Graph(V);
    int maksymalneKrawedzie = V * (V - 1);
    int liczbaKrawedzi = maksymalneKrawedzie * gestosc / 100;

    for (int i = 0; i < liczbaKrawedzi; ++i) {
        int u = rand() % V;
        int v = rand() % V;
        int waga = rand() % 100;
        if (u != v) {
            graf->dodajKrawedz(u, v, waga);
        }
    }

    return graf;
}

// Funkcja wczytująca graf z pliku
void wczytajGrafZPliku(Graph*& graf, const string& nazwaPliku) {
    ifstream plik(nazwaPliku);
    if (!plik.is_open()) {
        cerr << "Nie mozna otworzyc pliku: " << nazwaPliku << endl;
        return;
    }

    int V, E;
    plik >> V >> E;
    graf = new Graph(V);

    int u, v, waga;
    for (int i = 0; i < E; ++i) {
            plik >> u >> v >> waga;
        graf->dodajKrawedz(u, v, waga);
    }

    plik.close();
}

int main() {
    srand(time(nullptr));
    Graph* graf = nullptr;
    int wybor;
    string nazwaPliku;
    int V, gestosc, src, dest;

    while (true) {
        cout << "Menu:\n";
        cout << "1. Wczytaj z pliku\n";
        cout << "2. Wygeneruj losowy graf\n";
        cout << "3. Wyswietl graf\n";
        cout << "4. Dijkstra (macierzowa)\n";
        cout << "5. Dijkstra (listowa)\n";
        cout << "6. Testowanie\n";
        cout << "7. Wyjscie\n";
        cout << "Wybor: ";
        cin >> wybor;

        switch (wybor) {
            case 1:
                cout << "Podaj nazwe pliku: ";
                cin >> nazwaPliku;
                wczytajGrafZPliku(graf, nazwaPliku);
                break;
            case 2:
                cout << "Podaj liczbe wierzcholkow: ";
                cin >> V;
                cout << "Podaj gestosc w %: ";
                cin >> gestosc;
                graf = generujLosowyGraf(V, gestosc);
                break;
            case 3:
                if (graf) {
                    graf->drukujMacierzSasiedztwa();
                    cout << endl;
                    graf->drukujListeSasiedztwa();
                } else {
                    cout << "Graf nie zostal jeszcze wczytany ani wygenerowany.\n";
                }
                break;
            case 4:
                if (graf) {
                    cout << "Podaj wierzcholek zrodlowy: ";
                    cin >> src;
                    cout << "Podaj wierzcholek docelowy: ";
                    cin >> dest;
                    if (src >= 0 && src < graf->V && dest >= 0 && dest < graf->V) {
                        double czasMacierz = 0.0;
                        dijkstraMacierz(*graf, src, dest, czasMacierz);
                    } else {
                        cout << "Podane wierzcholki sa spoza zakresu grafu.\n";
                    }
                } else {
                    cout << "Najpierw wczytaj graf lub wygeneruj nowy.\n";
                }
                break;
            case 5:
                if (graf) {
                    cout << "Podaj wierzcholek zrodlowy: ";
                    cin >> src;
                    cout << "Podaj wierzcholek docelowy: ";
                    cin >> dest;
                    if (src >= 0 && src < graf->V && dest >= 0 && dest < graf->V) {
                        double czasLista = 0.0;
                        dijkstraLista(*graf, src, dest, czasLista);
                    } else {
                        cout << "Podane wierzcholki sa spoza zakresu grafu.\n";
                    }
                } else {
                    cout << "Najpierw wczytaj graf lub wygeneruj nowy.\n";
                }
                break;
            case 6:
                if (graf) {
                    int liczbaInstancji;
                    cout << "Podaj liczbe instancji do wygenerowania i przetestowania: ";
                    cin >> liczbaInstancji;
                    vector<double> czasyMacierz(liczbaInstancji, 0.0);
                    vector<double> czasyLista(liczbaInstancji, 0.0);

                    for (int instancja = 0; instancja < liczbaInstancji; ++instancja) {
                        int src = rand() % graf->V;
                        int dest = rand() % graf->V;

                        double czasMacierz = 0.0;
                        dijkstraMacierz(*graf, src, dest, czasMacierz);
                        czasyMacierz[instancja] = czasMacierz;

                        double czasLista = 0.0;
                        dijkstraLista(*graf, src, dest, czasLista);
                        czasyLista[instancja] = czasLista;
                    }

                    double sredniCzasMacierz = 0.0;
                    double sredniCzasLista = 0.0;
                    for (int i = 0; i < liczbaInstancji; ++i) {
                        sredniCzasMacierz += czasyMacierz[i];
                        sredniCzasLista += czasyLista[i];
                    }
                    sredniCzasMacierz /= liczbaInstancji;
                    sredniCzasLista /= liczbaInstancji;

                    cout << "Sredni czas wykonania (macierzowa) dla " << liczbaInstancji << " instancji: " << fixed << setprecision(6) << sredniCzasMacierz << " sekund\n";
                    cout << "Sredni czas wykonania (listowa) dla " << liczbaInstancji << " instancji: " << fixed << setprecision(6) << sredniCzasLista << " sekund\n";
                } else {
                    cout << "Najpierw wczytaj graf lub wygeneruj nowy.\n";
                }
                break;
            case 7:
                cout << "Wyjscie z programu.\n";
                if (graf) {
                    delete graf;
                }
                return 0;
            default:
                cout << "Niepoprawny wybor. Sprobuj ponownie.\n";
                break;
        }
    }

    return 0;
}

