#ifndef PRODUCTO_H
#define PRODUCTO_H
#include <iostream>
#include <string>

using namespace std;

struct Producto
{
    int id;
    string nombre;
    double precio;
    double existencia;
    int idCategoria;
};
#endif