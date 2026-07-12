#ifndef ARBOL_NARIO_H
#define ARBOL_NARIO_H

#include "nodo_nario.h"
#include <list>

using namespace std;

class ArbolNario
{
protected:
    NodoNario *raiz;
    NodoNario *nodoActual;
    int totalNodos;

    void _Destructor(NodoNario *nodo)
    {
        if (nodo != nullptr)
        {
            for (NodoNario *hijo : nodo->subcategorias)
            {
                _Destructor(hijo);
            }
            for (Producto *prod : nodo->productos)
            {
                delete prod;
            }
            delete nodo;
        }
    }

    NodoNario *_buscarNodo(NodoNario *nodo, int idCategoria) const
    {
        if (nodo == nullptr)
            return nullptr;
        if (nodo != raiz && nodo->categoria.id == idCategoria)
        {
            return nodo;
        }
        for (NodoNario *hijo : nodo->subcategorias)
        {
            NodoNario *encontrado = _buscarNodo(hijo, idCategoria);
            if (encontrado != nullptr)
                return encontrado;
        }
        return nullptr;
    }

    void _acumularProductosRecursivo(NodoNario *nodo, std::list<Producto *> &listaAcumulada) const
    {
        if (nodo == nullptr)
            return;
        for (Producto *prod : nodo->productos)
        {
            listaAcumulada.push_back(prod);
        }
        for (NodoNario *hijo : nodo->subcategorias)
        {
            _acumularProductosRecursivo(hijo, listaAcumulada);
        }
    }

    void _obtenerDescendientesRecursivo(NodoNario *nodo, std::list<NodoNario *> &lista) const
    {
        if (nodo == nullptr)
            return;
        for (NodoNario *hijo : nodo->subcategorias)
        {
            lista.push_back(hijo);
            _obtenerDescendientesRecursivo(hijo, lista);
        }
    }
    std::string _construirRutaRecursiva(NodoNario *nodo) const
    {

        if (nodo == nullptr || nodo == raiz)
        {
            return raiz->categoria.nombre;
        }

        if (nodo->padre == raiz)
        {
            return nodo->categoria.nombre;
        }

        return _construirRutaRecursiva(nodo->padre) + " / " + nodo->categoria.nombre;
    }

public:
    ArbolNario()
    {
        Categoria raizDummy = {0, "PRINCIPAL", -1}; // Nivel raíz contenedor vacío[cite: 2]
        raiz = new NodoNario(raizDummy);
        nodoActual = raiz;
        totalNodos = 1;
    }

    ~ArbolNario()
    {
        _Destructor(raiz);
        raiz = nullptr;
        nodoActual = nullptr;
        totalNodos = 0;
    }

    bool Vacio() const { return (raiz == nullptr || raiz->subcategorias.empty()); }
    int Tamanio() const { return totalNodos; }
    NodoNario *ObtenerRaiz() const { return raiz; }
    NodoNario *ObtenerNodoActual() const { return nodoActual; }
    bool EsHoja(NodoNario *nodo) const { return (nodo == nullptr) ? false : nodo->subcategorias.empty(); }
    bool estaEnRaiz() const { return (nodoActual == raiz); }

    void insertar(Categoria cat)
    {
        NodoNario *nuevo = new NodoNario(cat);
        if (cat.idPadre <= 0)
        {
            raiz->AgregarHijo(nuevo);
        }
        else
        {
            NodoNario *padreNodo = buscarNodo(cat.idPadre);
            if (padreNodo != nullptr)
            {
                padreNodo->AgregarHijo(nuevo);
            }
            else
            {
                raiz->AgregarHijo(nuevo);
            }
        }
        totalNodos++;
    }

    void cargarDesdeLista(const list<Categoria> &listaDatos)
    {
        std::list<NodoNario *> listaAux;

        for (const Categoria &cat : listaDatos)
        {
            listaAux.push_back(new NodoNario(cat));
        }

        for (NodoNario *hijo : listaAux)
        {
            if (hijo->categoria.idPadre <= 0)
            {
                raiz->AgregarHijo(hijo);
            }
            else
            {
                for (NodoNario *posiblePadre : listaAux)
                {
                    if (posiblePadre->categoria.id == hijo->categoria.idPadre)
                    {
                        posiblePadre->AgregarHijo(hijo);
                        break;
                    }
                }
            }
        }
    }

    std::string obtenerRutaDeNodo(int idCategoria) const
    {
        NodoNario *nodo = buscarNodo(idCategoria);
        return _construirRutaRecursiva(nodo);
    }

    NodoNario *buscarNodo(int idCategoria) const
    {
        return _buscarNodo(raiz, idCategoria);
    }

    list<NodoNario *> obtenerSubcategorias(int idCategoria) const
    {
        NodoNario *nodo = buscarNodo(idCategoria);
        return (nodo != nullptr) ? nodo->subcategorias : std::list<NodoNario *>();
    }

    std::list<NodoNario *> obtenerDescendientes(int idCategoria) const
    {
        std::list<NodoNario *> descendientes;
        NodoNario *nodo = buscarNodo(idCategoria);
        if (nodo != nullptr)
        {
            _obtenerDescendientesRecursivo(nodo, descendientes);
        }
        return descendientes;
    }

    // Fase 2 y 3: Asigna un producto a su categoría correspondiente[cite: 2]
    bool agregarProducto(int idCategoria, Producto *producto)
    {
        if (idCategoria <= 0)
        {
            raiz->productos.push_back(producto);
            return true;
        }
        NodoNario *nodo = buscarNodo(idCategoria);
        if (nodo != nullptr)
        {
            nodo->productos.push_back(producto);
            return true;
        }
        return false;
    }

    // Mueve el estado de navegación[cite: 2]
    bool navegarHacia(int idCategoria)
    {
        NodoNario *nodo = buscarNodo(idCategoria);
        if (nodo != nullptr)
        {
            nodoActual = nodo;
            return true;
        }
        return false;
    }

    bool regresarNivel()
    {
        if (!estaEnRaiz() && nodoActual->padre != nullptr)
        {
            nodoActual = nodoActual->padre;
            return true;
        }
        return false;
    }

    std::list<Producto *> obtenerProductosNivelActual() const
    {
        std::list<Producto *> listaProductos;
        _acumularProductosRecursivo(nodoActual, listaProductos);
        return listaProductos;
    }
};

#endif