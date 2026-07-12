#ifndef NODO_TRIE_H
#define NODO_TRIE_H

#include "productos.h"

const int TAM_ALFABETO = 27;

struct NodoTrie {
    char letra;
    NodoTrie* hijos[TAM_ALFABETO];
    bool esFinDePalabra;
    Producto* producto; 

    NodoTrie(char c) {
        letra = c;
        esFinDePalabra = false;
        producto = nullptr;
        for (int i = 0; i < TAM_ALFABETO; i++) {
            hijos[i] = nullptr;
        }
    }

    ~NodoTrie() {
        for (int i = 0; i < TAM_ALFABETO; i++) {
            if (hijos[i] != nullptr) {
                delete hijos[i];
                hijos[i] = nullptr;
            }
        }

    }
};

#endif