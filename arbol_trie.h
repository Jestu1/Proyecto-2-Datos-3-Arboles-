#ifndef ARBOL_TRIE_H
#define ARBOL_TRIE_H

#include "nodo_trie.h"
#include <string>
#include <list> 

class ArbolTrie {
private:
    NodoTrie* raiz;
    int totalPalabras;

    int obtenerIndice(char c) const {
        if (c == ' ') return 26; 

        char cMin = c;
        if (c >= 'A' && c <= 'Z') {
            cMin = c + ('a' - 'A'); 
        }
        
        if (cMin >= 'a' && cMin <= 'z') {
            return cMin - 'a'; 
        }
        return -1;
    }

    void recolectarProductos(NodoTrie* nodo, std::list<Producto*>& resultados) const {
        if (nodo == nullptr) return;

        if (nodo->esFinDePalabra && nodo->producto != nullptr) {
            resultados.push_back(nodo->producto);
        }

        for (int i = 0; i < TAM_ALFABETO; i++) {
            if (nodo->hijos[i] != nullptr) {
                recolectarProductos(nodo->hijos[i], resultados);
            }
        }
    }

    int contarHijos(NodoTrie* nodo) const {
        int cuenta = 0;
        for (int i = 0; i < TAM_ALFABETO; i++) {
            if (nodo->hijos[i] != nullptr) cuenta++;
        }
        return cuenta;
    }

    bool eliminarRecursivo(NodoTrie* nodo, const std::string& palabra, int profundidad) {
        if (nodo == nullptr) return false;

        if (profundidad == palabra.length()) {
            if (!nodo->esFinDePalabra) return false;
            
            nodo->esFinDePalabra = false;
            nodo->producto = nullptr;
            return (contarHijos(nodo) == 0);
        }

        int indice = obtenerIndice(palabra[profundidad]);
        if (indice == -1) return false;

        bool debeEliminarHijo = eliminarRecursivo(nodo->hijos[indice], palabra, profundidad + 1);

        if (debeEliminarHijo) {
            delete nodo->hijos[indice];
            nodo->hijos[indice] = nullptr;
            return (!nodo->esFinDePalabra && contarHijos(nodo) == 0);
        }

        return false;
    }

public:
    ArbolTrie() {
        raiz = new NodoTrie('\0');
        totalPalabras = 0;
    }

    ~ArbolTrie() {
        delete raiz;
        raiz = nullptr;
        totalPalabras = 0;
    }

    bool insertar(Producto* prod) {
        if (prod == nullptr || prod->nombre.empty()) return false;

        NodoTrie* actual = raiz;
        std::string nombreProd = prod->nombre;

        for (size_t i = 0; i < nombreProd.length(); i++) {
            int indice = obtenerIndice(nombreProd[i]);

            if (indice == -1) continue;

            if (actual->hijos[indice] == nullptr) {
                actual->hijos[indice] = new NodoTrie(nombreProd[i]);
            }
            actual = actual->hijos[indice];
        }

        if (actual->esFinDePalabra) return false;

        actual->esFinDePalabra = true;
        actual->producto = prod;
        totalPalabras++;
        return true;
    }

    bool existe(const std::string& nombre) const {
        if (nombre.empty()) return false;

        NodoTrie* actual = raiz;
        for (size_t i = 0; i < nombre.length(); i++) {
            int indice = obtenerIndice(nombre[i]);
            if (indice == -1 || actual->hijos[indice] == nullptr) return false;
            actual = actual->hijos[indice];
        }
        return actual->esFinDePalabra;
    }

    std::list<Producto*> autocompletar(const std::string& prefijo) const {
        std::list<Producto*> resultados;
        if (prefijo.empty()) return resultados;

        NodoTrie* actual = raiz;
        
        for (size_t i = 0; i < prefijo.length(); i++) {
            int indice = obtenerIndice(prefijo[i]);
            if (indice == -1 || actual->hijos[indice] == nullptr) {
                return resultados; 
            }
            actual = actual->hijos[indice];
        }

        recolectarProductos(actual, resultados);
        return resultados;
    }

    bool eliminar(const std::string& nombre) {
        if (nombre.empty() || !existe(nombre)) return false;

        bool eliminado = eliminarRecursivo(raiz, nombre, 0);
        if (eliminado) totalPalabras--;
        return eliminado;
    }

    int contarCoincidencias(const std::string& prefijo) const {
        std::list<Producto*> coincidencias = autocompletar(prefijo);
        return coincidencias.size();
    }

    bool estaVacio() const {
        return totalPalabras == 0;
    }

    int totalDePalabras() const {
        return totalPalabras;
    }
};

#endif // ARBOL_TRIE_H