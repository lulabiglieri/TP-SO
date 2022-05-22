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

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (auto &p : *tabla[index]) {
            if (p.second > max->second) {
                max->first = p.first;
                max->second = p.second;
            }
        }
    }

    return *max;
}



hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cant_threads) {
    // Completar (Ejercicio 3)
}

#endif
