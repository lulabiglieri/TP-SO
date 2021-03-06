#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
//#include <pthread.h>

#include "CargarArchivos.hpp"

using namespace std;

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath)
{
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open())
    {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual)
    {
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof())
    {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}

void cargarArchivoParalelo(HashMapConcurrente &hashMap, vector<string> &filePaths, atomic<int> &lastProcessedFile) {
    long unsigned int index = lastProcessedFile.fetch_add(1);
    while (index < filePaths.size()) {
        cargarArchivo(hashMap, filePaths[index]);
        index = lastProcessedFile.fetch_add(1);
    }
}

void cargarMultiplesArchivos(HashMapConcurrente &hashMap, unsigned int cantThreads, vector<string> filePaths) {
    atomic<int> lastProcessedFile(0);
    vector<thread> threads(cantThreads);

    //clock
    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);

    for (auto &t : threads) {
        t = thread(&cargarArchivoParalelo, ref(hashMap), ref(filePaths), ref(lastProcessedFile));
    }

    for (auto &t : threads) {
        t.join();
    }

    //end clock
    clock_gettime(CLOCK_REALTIME, &stop);
    printf("%i,%li,%li,cargarMultiplesArchivos\n", cantThreads, stop.tv_sec - start.tv_sec, stop.tv_nsec-start.tv_nsec);
}

#endif
