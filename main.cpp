#ifndef MAIN_CPP
#define MAIN_CPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <stdlib.h>

#include "categoria.h"
#include "productos.h"
#include "arbol_nario.h"
#include "arbol_trie.h"

using namespace std;

bool cargarCategorias(ArbolNario &arbolCat, const string &rutaArchivo)
{
    list<Categoria> listaAux;
    ifstream archivo(rutaArchivo);
    if (!archivo.is_open())
        return false;

    string linea;
    while (getline(archivo, linea))
    {
        if (linea.empty())
            continue;

        stringstream ss(linea);
        string idStr, nombre, padreStr;

        if (getline(ss, idStr, '|') && getline(ss, nombre, '|') && getline(ss, padreStr))
        {
            int id = stoi(idStr);
            int idPadre = (padreStr == "nulo" || padreStr == "0") ? 0 : stoi(padreStr);

            Categoria cat = {id, nombre, idPadre};
            listaAux.push_back(cat);
            cout << "Categoria leida: " << id << " | " << nombre << " | Padre: " << padreStr << endl;
        }
    }
    archivo.close();
    arbolCat.cargarDesdeLista(listaAux);
    return true;
}

bool cargarProductos(ArbolTrie &trieProd,
                     ArbolNario &arbolCat,
                     list<Producto *> &listaMaestra,
                     const string &rutaArchivo)
{
    ifstream archivo(rutaArchivo);
    if (!archivo.is_open())
        return false;

    string linea;
    while (getline(archivo, linea))
    {
        if (linea.empty())
            continue;

        stringstream ss(linea);
        string idStr, nombre, precioStr, stockStr, idCatStr;
        int campos = 0;

        string temp;
        stringstream ssCount(linea);
        while (getline(ssCount, temp, '|'))
            campos++;

        if (campos != 5)
        {
            cout << "[ADVERTENCIA] Registro ignorado por formato invalido: " << linea << endl;
            continue;
        }

        getline(ss, idStr, '|');
        getline(ss, nombre, '|');
        getline(ss, precioStr, '|');
        getline(ss, stockStr, '|');
        getline(ss, idCatStr, '|');

        int idCat = (idCatStr == "nulo" || idCatStr == "0") ? 0 : stoi(idCatStr);

        if (idCat != 0 && arbolCat.buscarNodo(idCat) == nullptr)
        {
            cout << "[ADVERTENCIA] Producto ignorado: Categoria ID " << idCat << " no existe." << endl;
            continue;
        }

        Producto *nuevoProd = new Producto();
        nuevoProd->id = stoi(idStr);
        nuevoProd->nombre = nombre;
        nuevoProd->precio = stod(precioStr);
        nuevoProd->existencia = stod(stockStr);
        nuevoProd->idCategoria = idCat;

        listaMaestra.push_back(nuevoProd);
        trieProd.insertar(nuevoProd);

        arbolCat.agregarProducto(idCat, nuevoProd);

        cout << "[CARGA EXITOSA] " << nuevoProd->nombre << " | " << nuevoProd->idCategoria << " | agregado." << endl;
    }
    archivo.close();
    return true;
}

void mostrarMenuDinamico(const ArbolNario &arbolCat)
{
    NodoNario *actual = arbolCat.ObtenerNodoActual();
    int idActual = actual->categoria.id;
    cout << "\n=============================================" << endl;
    cout << " CATEGORiA ACTUAL: " << arbolCat.obtenerRutaDeNodo(idActual) << endl;
    cout << "=============================================" << endl;
}

void mostrarCategoria(const ArbolNario &arbolCat)
{
    NodoNario *actual = arbolCat.ObtenerNodoActual();
    if (actual->subcategorias.empty())
    {
        cout << " [No hay subcategorias en este nivel]" << endl;
    }
    else
    {
        cout << " INGRESAR A CATEGORIA:" << endl;
        int indice = 1;
        for (NodoNario *sub : actual->subcategorias)
        {
            cout << "  c" << indice++ << ". " << sub->categoria.nombre << endl;
        }
    }
}
bool esCategoriaDescendiente(const ArbolNario &arbolCat, int idCatProducto, int idCatActual)
{

    if (idCatActual == 0)
        return true;

    if (idCatProducto == idCatActual)
        return true;

    list<NodoNario *> descendientes = arbolCat.obtenerDescendientes(idCatActual);
    for (NodoNario *nodo : descendientes)
    {
        if (nodo->categoria.id == idCatProducto)
        {
            return true;
        }
    }
    return false;
}

bool esNumeroValido(const string &str)
{
    if (str.empty())
        return false;

    size_t inicio = 0;
    if (str[0] == '-')
    {
        if (str.length() == 1)
            return false;
        inicio = 1;
    }

    for (size_t i = inicio; i < str.length(); i++)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            return false;
        }
    }
    return true;
}

bool esFormatoCategoriaValido(const string &str)
{
    if (str.length() < 2 || str[0] != 'c')
        return false;

    string parteNumerica = str.substr(1);

    return esNumeroValido(parteNumerica);
}

string obtenerNombreCategoria(const ArbolNario &arbolCat, int idCategoria)
{
    if (idCategoria == 0)
    {
        return "PRINCIPAL";
    }

    NodoNario *nodoCat = arbolCat.buscarNodo(idCategoria);

    if (nodoCat != nullptr)
    {
        return nodoCat->categoria.nombre;
    }

    return "Desconocida";
}

int main()
{
    ArbolNario arbolCategorias;
    ArbolTrie trieProductos;
    list<Producto *> listaMaestraProductos;

    // Cargas iniciales de datos
    if (!cargarCategorias(arbolCategorias, "categorias.txt"))
    {
        cout << "Error: No se pudo cargar categorias.txt" << endl;
        return 1;
    }

    if (!cargarProductos(trieProductos, arbolCategorias, listaMaestraProductos, "productos.txt"))
    {
        cout << "Error: No se pudo cargar productos.txt" << endl;
        return 1;
    }

    cout << "\nPresione Enter para continuar...";
    cin.ignore();
    cin.get();

    system("cls");

    string entradaUsuario = "";
    int opcion = -3;

    while (opcion != -2)
    {

        mostrarMenuDinamico(arbolCategorias);
        cout << " OPCIONES:" << endl;
        cout << "   0. Buscar Producto por Prefijo (Autocompletar)" << endl;
        cout << "   1. Listar Productos de este nivel y subcategorias" << endl;
        cout << "   2. Regresar a Categoria Padre" << endl;
        cout << "   3. Salir del Programa" << endl;
        cout << "---------------------------------------------" << endl;
        mostrarCategoria(arbolCategorias);
        cout << "Seleccione una opcion o ID: ";
        cin >> entradaUsuario;

        if (esFormatoCategoriaValido(entradaUsuario))
        {
            int indice = stoi(entradaUsuario.substr(1)) - 1; // Ajuste a base 0
            NodoNario *actual = arbolCategorias.ObtenerNodoActual();

            if (indice >= 0 && indice < (int)actual->subcategorias.size())
            {
                auto it = actual->subcategorias.begin();
                advance(it, indice);
                arbolCategorias.navegarHacia((*it)->categoria.id);
            }
            else
            {
                cout << "\n[!] Error: Indice de categoria invalido." << endl;
                cin.ignore();
                cin.get();
            }
        }

        else if (esNumeroValido(entradaUsuario))
        {
            opcion = stoi(entradaUsuario);

            if (opcion == 3)
                break;

            if (opcion == 2)
            {
                if (!arbolCategorias.regresarNivel())
                {
                    cout << "\nNO ES POSIBLE REALIZAR ESA OPERACION EN ESTE NIVEL" << endl;
                }
            }
            else if (opcion == 1)
            {
                list<Producto *> productosNivel = arbolCategorias.obtenerProductosNivelActual();

                cout << "\n--- PRODUCTOS DISPONIBLES EN ESTE NIVEL ---" << endl;
                if (productosNivel.empty())
                {
                    cout << " No hay productos registrados en esta rama del catalogo." << endl;
                }
                else
                {
                    for (Producto *prod : productosNivel)
                    {
                        cout << " -> ID: " << prod->id
                             << " | Nombre: " << prod->nombre
                             << " | Precio: $" << prod->precio
                             << " | Existencia: " << prod->existencia
                             << " | Categoria: [" << prod->idCategoria << "] "
                             << obtenerNombreCategoria(arbolCategorias, prod->idCategoria) << endl;
                    }
                }
                cout << "---------------------------------------------" << endl;
            }
            else if (opcion == 0)
            {
                string prefijo;
                cout << "Ingrese el prefijo a buscar: ";
                cin.ignore();
                getline(cin, prefijo);

                list<Producto *> coincidenciasGlobales = trieProductos.autocompletar(prefijo);

                if (coincidenciasGlobales.empty())
                {
                    cout << "\n No se encontraron productos con el prefijo \"" << prefijo << "\"." << endl;
                }
                else
                {
                    int idActual = arbolCategorias.ObtenerNodoActual()->categoria.id;

                    cout << "\n--- RESULTADOS DE BUSQUEDA CONTEXTUAL ---" << endl;
                    bool coincidenciaLocal = false;

                    for (Producto *prod : coincidenciasGlobales)
                    {
                        if (esCategoriaDescendiente(arbolCategorias, prod->idCategoria, idActual))
                        {
                            cout << " -> ID: " << prod->id
                                 << " | Nombre: " << prod->nombre
                                 << " | Precio: $" << prod->precio
                                 << " | Existencia: " << prod->existencia
                                 << " | Categoria: [" << prod->idCategoria << "] "
                                 << obtenerNombreCategoria(arbolCategorias, prod->idCategoria) << endl;
                            coincidenciaLocal = true;
                        }
                    }

                    if (!coincidenciaLocal)
                    {
                        cout << " Existen coincidencias globales, pero ninguna está dentro de esta rama." << endl;
                    }
                }
            }
            else
            {
                cout << "\n[!] Opcion no valida." << endl;
            }
        }
        else
        {
            cout << "\n[!] Entrada invalida." << endl;
            cin.ignore();
            cin.get();
        }
        cout << "\nPresione Enter para continuar...";
        cin.ignore();
        cin.get();

        system("cls");
    }

    for (Producto *prod : listaMaestraProductos)
    {
        delete prod;
    }
    listaMaestraProductos.clear();

    cout << "\nMemoria liberada con exito. ¡Catálogo cerrado!" << endl;
    return 0;
}

#endif