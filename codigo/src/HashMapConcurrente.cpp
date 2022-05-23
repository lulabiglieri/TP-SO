#ifndef CHM_CPP
#define CHM_CPP
#include <algorithm>
#include <thread>
// alternativamente #include <pthread.h>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"

using namespace std;

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(string clave) {
    return (unsigned int)(clave[0] - 'a');
}

ListaAtomica<hashMapPair>::iterator find(string clave, ListaAtomica<hashMapPair>* lista) {
    auto it = lista->begin();
    while (it != lista->end() && (clave != (*it).first)) it++;
    return it;
}

void HashMapConcurrente::incrementar(string clave) {
    int index = hashIndex(clave);
    auto lista = tabla[index];
    _locks[index].lock();
    auto it = find(clave, lista);
    if (it == lista->end()) {
        lista->insertar(hashMapPair(clave, 1));
        _claves.push_back(clave);
    } else {
        (*it).second++;
    }
   _locks[index].unlock();
}

vector<string> HashMapConcurrente::claves() {
    return _claves;
}

unsigned int HashMapConcurrente::valor(string clave) {
    auto lista = tabla[hashIndex(clave)];
    auto it = find(clave, lista);
    if (it == lista->end()) return 0;
    return (*it).second;
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair *max = new hashMapPair();
    max->second = 0;
    for (int i = 0; i<cantLetras; i++) _locks[i].lock();
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (auto &p : *tabla[index]) {
            if (p.second > max->second) {
                max->first = p.first;
                max->second = p.second;
            }
        }
    }
    for (int i = 0; i<cantLetras; i++) _locks[i].unlock();
    return *max;
}

void HashMapConcurrente::maximoDeLista(atomic<int>& lastProcessedList, hashMapPair max[]) {
    int index = lastProcessedList.fetch_add(1);
    while (index < HashMapConcurrente::cantLetras) {
        _locks[index].lock();

        max[index].second = 0;
        for (auto &p : *tabla[index]) {
            if (p.second > max[index].second) {
                max[index].first = p.first;
                max[index].second = p.second;
            }
        }

        _locks[index].unlock();
        index = lastProcessedList.fetch_add(1);
    }
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cant_threads) {
    atomic<int> lastProcessedList(0);
    hashMapPair max[cantLetras];

    vector<thread> threads(HashMapConcurrente::cantLetras);
    for (int i = 0; i<cant_threads; i++) {
        threads[i] = thread(&HashMapConcurrente::maximoDeLista, this, std::ref(lastProcessedList), max);
    }

    for (int i = 0; i<cant_threads; i++) {
        threads[i].join();
    }

    int maxIndex = 0;
    for (int i = 0; i<HashMapConcurrente::cantLetras; i++) {
        if (max[maxIndex].second < max[i].second) {
            maxIndex = i;
        }
    }
    return max[maxIndex];
}

#endif