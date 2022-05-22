#ifndef CHM_CPP
#define CHM_CPP
#include <algorithm>
#include <thread>
// alternativamente #include <pthread.h>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    auto lista = tabla[hashIndex(clave)];
    auto it = lista->begin();
    while (it != lista->end() && (clave != (*it).first)) it++;
    if (it == lista->end()) {
        lista->insertar(hashMapPair(clave, 1));
        _claves.push_back(clave);
    } else (*it).second++;
}

std::vector<std::string> HashMapConcurrente::claves() {
    return _claves;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    auto lista = tabla[hashIndex(clave)];
    auto it = lista->begin();
    while (it != lista->end() && (clave != (*it).first)) it++;
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
