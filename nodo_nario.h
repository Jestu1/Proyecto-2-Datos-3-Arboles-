#ifndef NODO_NARIO_H
#define NODO_NARIO_H
#include <iostream>
#include <string>
#include <list>

#include "productos.h"
#include "categoria.h"

using namespace std;

class NodoNario {
public:
    Categoria categoria;                
    list<Producto*> productos;     
    list<NodoNario*> subcategorias;
    NodoNario* padre;                   

    NodoNario(Categoria cat) {
        categoria = cat;
        padre = nullptr;
    }

    void AgregarHijo(NodoNario* nuevoHijo) {
        if (nuevoHijo != nullptr) {
            subcategorias.push_back(nuevoHijo);
            nuevoHijo->padre = this;
        }
    }
};

#endif