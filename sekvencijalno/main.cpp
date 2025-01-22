#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <climits>
#include <filesystem>
#include <chrono>  // For measuring execution time

namespace fs = std::filesystem;

using namespace std;

// Function to load a matrix from a file
int* loadMatrixFromFile(const std::string& filename, int& n) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return nullptr;
    }

    file >> n;  // Read matrix order from the first line

    // Dynamically allocate memory for the matrix
    int* matrix = new int[n*n];

    // Fill the matrix with data from the file
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            file >> matrix[i*n + j];
        }
    }

    file.close();
    return matrix;
}

// Function to free dynamically allocated matrix memory
void deleteMatrix(int* matrix, int n) {
    delete[] matrix;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Cvor
{
    int vrijednost;
    vector<int> dosadasnja_putanja;
    vector<bool> dosad_posjeceni_cvorovi;
    int dosadasnji_trosak;
};

void popuni_sa_min_ivicama(int* matrica, int N, int* min_ivice){
    for(int i=0; i<N; i++){
        int min_ivica = INT_MAX;
        for(int j=0; j<N; j++){
            if(matrica[i*N + j]<min_ivica && i!=j){
                min_ivica = matrica[i*N + j];
            }
        }min_ivice[i] = min_ivica;
    }
}

int izracunavanje_granice(Cvor& cvor, int N, int* min_ivice) {
    int granica = cvor.dosadasnji_trosak;

    for (int i = 0; i < N; ++i) {
        if (!cvor.dosad_posjeceni_cvorovi[i]) {
                granica+=min_ivice[i];
        }
    }

    return granica;
}

void branch_and_bound(int vrijednost, vector<int>& dosadasnja_putanja, vector<bool>& posjeceni_cvorovi, int trenutni_trosak, int& najmanji_trosak, vector<int>& najbolja_putanja,
                     int* matrica, int N, int* min_ivice){
    if (dosadasnja_putanja.size() == N) {
        trenutni_trosak += matrica[vrijednost * N + dosadasnja_putanja[0]];
        if (trenutni_trosak < najmanji_trosak) {
            najmanji_trosak = trenutni_trosak;
            najbolja_putanja = dosadasnja_putanja;
            najbolja_putanja.push_back(0);
        }
        return;
    }

    posjeceni_cvorovi[vrijednost] = true;

    for (int iduci_cvor = 0; iduci_cvor < N; ++iduci_cvor) {
        if (!posjeceni_cvorovi[iduci_cvor]) {
            dosadasnja_putanja.push_back(iduci_cvor);

            int novi_trosak = trenutni_trosak + matrica[vrijednost * N + iduci_cvor];

            posjeceni_cvorovi[iduci_cvor] = true;

            Cvor cvor = {iduci_cvor, dosadasnja_putanja, posjeceni_cvorovi, novi_trosak};
            int granica = izracunavanje_granice(cvor, N, min_ivice);

            posjeceni_cvorovi[iduci_cvor] = false;

            if(granica < najmanji_trosak){
                branch_and_bound(iduci_cvor, dosadasnja_putanja, posjeceni_cvorovi, novi_trosak, najmanji_trosak, najbolja_putanja, matrica, N, min_ivice);
            }
            dosadasnja_putanja.pop_back();
        }
    }
    posjeceni_cvorovi[vrijednost] = false;
}

int racunanje_pocetne_granice(int* matrica, int N){  //pocetna granica je trosak putanje 0, 1, 2, 3, ... N-1, 0
    int res = 0;
    vector<int> putanja;
    for(int i=0; i<N; i++){
        putanja.push_back(i);
    }
    putanja.push_back(0);
    for(int i=0; i<N; i++){
        res+=matrica[putanja[i]*N + putanja[i+1]];
    }
    return res;
}

// Function to print a matrix
void TSP(int* matrica, int N, int &optimal_path_value, int *optimal_path) {

    int najmanji_trosak = racunanje_pocetne_granice(matrica, N);

    vector<int> najbolja_putanja;

    vector<int> dosadasnja_putanja = {0};
    vector<bool> posjeceni_cvorovi(N, false);

    int min_ivice[N];

    popuni_sa_min_ivicama(matrica, N, min_ivice);

    branch_and_bound(0, dosadasnja_putanja, posjeceni_cvorovi, 0, najmanji_trosak, najbolja_putanja, matrica, N, min_ivice);

	optimal_path_value = najmanji_trosak;

    for(int i=0; i<N+1; i++) {
        optimal_path[i] = najbolja_putanja[i];
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Function to log execution time to a CSV file
void logStatistics(const std::string& filename, double execution_time, int optimal_path_value, int* optimal_path, int n) {
    std::ofstream csv_file("statistics.csv", std::ios_base::app);  // Open file in append mode
    if (!csv_file.is_open()) {
        std::cerr << "Error opening statistics.csv for logging." << std::endl;
        return;
    }

    csv_file << filename << "," << execution_time <<","<<optimal_path_value;
    for(int i=0; i<n+1; ++i)
       csv_file <<","<<optimal_path[i];

    csv_file<<"\n";  // Write the filename and execution time in milliseconds
    csv_file.close();
}

int main() {
    std::string folderPath = "test_examples";  // Folder containing the matrix files

    // Check if folder exists
    if (!fs::exists(folderPath)) {
        std::cerr << "Folder does not exist: " << folderPath << std::endl;
        return 1;
    }

    // Open or create the CSV file and write the header
    std::ofstream csv_file("statistics.csv");
    if (csv_file.is_open()) {
        csv_file << "Filename,Execution Time (s), Optimal Path Value, Optimal Path\n";  // CSV header
        csv_file.close();
    } else {
        std::cerr << "Error opening statistics.csv for writing." << std::endl;
        return 1;
    }

    // Iterate over all files in the folder
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        std::string filename = entry.path().string();
        int matrix_size = 0;

        int* matrix = loadMatrixFromFile(filename, matrix_size);

        int optimal_path_value = 0;    //Tezina/cijena optimalne rute.
        int* optimal_path = new int[matrix_size + 1](); //Optimalna ruta.

        if (matrix) {
            std::cout << "Matrix from file: " << filename << " (Order: " << matrix_size << "x" << matrix_size << "):" << std::endl;

            // Measure the time it takes to print the matrix
            auto start = std::chrono::high_resolution_clock::now();

            TSP(matrix, matrix_size, optimal_path_value, optimal_path);

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> exec_time = end - start;

            // Rezultate zapisujemo u jedan fajl: vrijeme izvrsavanja, optimalnu cijenu putanje, optimalna putanja.
            logStatistics(filename, exec_time.count(), optimal_path_value, optimal_path, matrix_size);

            // Free dynamically allocated memory
            deleteMatrix(matrix, matrix_size);
            std::cout << std::endl;
        }
    }

    std::cout << "Execution times saved to statistics.csv." << std::endl;
    return 0;
}
