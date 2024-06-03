#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <windows.h>
#include <sstream>

// constantes predefinidas en Windows para manejar atributos de color y tamano de texto en la consola
const int MAX_ALTO_PANTALLA = 25;
const int MAX_ANCHO_PANTALLA = 80;
const int COLOR_ROJO = FOREGROUND_RED | FOREGROUND_INTENSITY;
const int COLOR_VERDE = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const int COLOR_AZUL = FOREGROUND_BLUE | FOREGROUND_INTENSITY;

// Variables Globales para el manejo del estado de la pantalla y el cursor
int xPantalla = MAX_ANCHO_PANTALLA, yPantalla = MAX_ALTO_PANTALLA;
char contPantalla[MAX_ALTO_PANTALLA][MAX_ANCHO_PANTALLA];
WORD colorPantalla[MAX_ALTO_PANTALLA][MAX_ANCHO_PANTALLA];
char caracDibujo = '*'; // Caracter por defecto para dibujar figuras
int cursorX = xPantalla / 2, cursorY = yPantalla / 2; // Inicializacion del cursor al centro de la pantalla
int menuY = 0; // Linea inicial para el menu lateral

// Enumeraciones para representar tipos de figuras y direcciones de movimiento
enum Figura { TRIANGULO, CUADRADO, RECTANGULO, CIRCULO, LINEA, ROMBO, HEXAGONO };
enum Direccion { ARRIBA = 1, ABAJO, IZQUIERDA, DERECHA, DIAG_ARR_IZQ, DIAG_ARR_DER, DIAG_ABJ_IZQ, DIAG_ABJ_DER };

// Declaraciones de funciones
void procesarTeclaFuncion(int tecla);
void moverCursor(int dx, int dy);
void calcularPosicionCircular(int &x, int &y);
void limpiarPantalla();
void grabarPantalla();
void abrirArchivo(const std::string &nombreArchivo);
void dibujarFigura(int x, int y, int orientacion, int size1, int size2, WORD color, Figura figura, char carac);
void dibujarLinea(int x, int y, Direccion direccion, int longitud, WORD color, char carac);
void modificarOpciones();
WORD seleccionarColor();// WORD es un tipo de dato definido en el encabezado de Windows (Windows.h) Representa un entero sin signo de 16 bits (2 bytes)
//Uso: Se utiliza en este contexto para manejar atributos de color en la consola.
int seleccionarOrientacion();
char seleccionarCaracter();
void mostrarMenuSeleccion();
void mostrarMenuPrincipal();
void mostrarSubMenuColorCaracter();
void mostrarSubMenuDireccion();
void reiniciarCursorMenu();
void moverCursorConsola(int x, int y);
void cambiarColor(WORD color);
WORD obtenerColorActual();
void restaurarColor(WORD color);
void actualizarPantalla();
void seleccionarFigura(Figura figuraSeleccionada);

// Implementacion de funciones

// Cambia el color del texto en la consola
// Uso: Para establecer el color del texto de salida en la consola
// Mantenimiento: Se podria expandir para cambiar tambien el fondo
void cambiarColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

/*
La estructura CONSOLE_SCREEN_BUFFER_INFO contiene informacion sobre el bufer de pantalla de la consola, incluyendo el tamno del bufer, la posicion del cursor y los atributos de texto actuales.

Campos principales:
- dwSize: Tamano del bufer de pantalla en caracteres (ancho y alto).
- dwCursorPosition: Posicion actual del cursor en la consola.
- wAttributes: Atributos actuales del texto (colores de texto y fondo).
- srWindow: Coordenadas de la ventana de consola (esquina superior izquierda y esquina inferior derecha).
- dwMaximumWindowSize: Tamano maximo de la ventana de consola.

Esta estructura se usa principalmente con la funcion GetConsoleScreenBufferInfo para recuperar el estado actual de la consola, lo que permite realizar operaciones como obtener y restaurar atributos de color y posicion del cursor.
*/
// Obtiene el color actual del texto en la consola
// Uso: Para recuperar el color actual antes de cambiarlo y restaurarlo despues
// Mantenimiento: No se esperan cambios a menos que se agreguen mas atributos de consola
WORD obtenerColorActual() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.wAttributes;
}
/*
STD_OUTPUT_HANDLE es una constante predefinida en Windows que se utiliza con la funcion GetStdHandle para obtener el manejador del bufer de salida estandar de la consola. Este manejador es esencial para realizar diversas operaciones en la consola, como cambiar el color del texto, mover el cursor y escribir texto en posiciones especificas.
*/
// Mueve el cursor a una posicion especifica en la consola
// Uso: Para posicionar el cursor de texto en coordenadas especificas
// Mantenimiento: Podria mejorarse para validar los limites de la consola
void moverCursorConsola(int x, int y) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (SHORT)x, (SHORT)y });
}

// Mueve el cursor por un desplazamiento dado y actualiza su posicion circularmente
// Uso: Para mover el cursor y envolver su posicion dentro de los limites de la pantalla
// Mantenimiento: Considerar el manejo de casos en los que el desplazamiento lo saque de la pantalla
void moverCursor(int dx, int dy) {
    cursorX += dx;
    cursorY += dy;
    calcularPosicionCircular(cursorX, cursorY);
    moverCursorConsola(cursorX, cursorY);
    std::cout << ".";
}

// Calcula la posicion circular del cursor para que se mantenga dentro de los limites de la pantalla
// Uso: Para asegurar que el cursor no salga de los limites de la pantalla
// Mantenimiento: Revisar si se necesita para pantallas con diferentes dimensiones
void calcularPosicionCircular(int &x, int &y) {
    x = (x + xPantalla) % xPantalla; // Uso del operador modulo para envolver la posicion en el eje X
    y = (y + yPantalla) % yPantalla; // Uso del operador modulo para envolver la posicion en el eje Y
}

// Limpia la pantalla y reinicia los valores
// Uso: Para restablecer la pantalla a su estado inicial
// Mantenimiento: Podria optimizarse limpiando solo areas especificas en lugar de toda la pantalla
void limpiarPantalla() {
    for (int y = 0; y < yPantalla; ++y)
        for (int x = 0; x < xPantalla; ++x) {
            contPantalla[y][x] = ' ';
            colorPantalla[y][x] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        }
    actualizarPantalla();
}

// Actualiza el contenido de la pantalla segun los buffers de contenido y color
// Uso: Para refrescar la pantalla despues de cambios
// Mantenimiento: Se puede mejorar anadiendo actualizaciones parciales en lugar de toda la pantalla
void actualizarPantalla() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    for (int y = 0; y < yPantalla; ++y) {
        moverCursorConsola(0, y);
        for (int x = 0; x < xPantalla; ++x) {
            SetConsoleTextAttribute(hConsole, colorPantalla[y][x]);
            std::cout << contPantalla[y][x];
        }
    }
    moverCursorConsola(cursorX, cursorY);
    std::cout << ".";
    mostrarMenuPrincipal();
}

// Procesa las teclas de funcion y realiza la accion correspondiente
// Uso: Para manejar la logica de entrada de teclas de funcion
// Mantenimiento: Se puede expandir para soportar mas teclas de funcion o comandos personalizados
void procesarTeclaFuncion(int tecla) {
    switch (tecla) {
        case VK_F1: case VK_F2: case VK_F3: case VK_F4: case VK_F5: case VK_F6: case VK_F7:
            seleccionarFigura(static_cast<Figura>(tecla - VK_F1));
            break;
        case VK_F9:
            limpiarPantalla();
            break;
        case VK_F10:
            modificarOpciones();
            break;
        case VK_F12:
            grabarPantalla();
            break;
    }
}

// Permite seleccionar una figura y sus caracteristicas para dibujar
// Uso: Para seleccionar y configurar las caracteristicas de una figura antes de dibujarla
// Mantenimiento: Se puede agregar mas figuras o configuraciones adicionales
void seleccionarFigura(Figura figuraSeleccionada) {
    WORD colorActual = obtenerColorActual();
    WORD colorSeleccionado = seleccionarColor();
    int orientacion = seleccionarOrientacion();
    char carac = seleccionarCaracter();
    int size1, size2;

    switch (figuraSeleccionada) {
        case TRIANGULO:
            std::cout << "Ingrese base del triangulo: ";
            std::cin >> size1;
            dibujarFigura(cursorX, cursorY, orientacion, size1, 0, colorSeleccionado, TRIANGULO, carac);
            break;
        case CUADRADO:
            std::cout << "Ingrese lado del cuadrado: ";
            std::cin >> size1;
            dibujarFigura(cursorX, cursorY, orientacion, size1, size1, colorSeleccionado, CUADRADO, carac);
            break;
        case RECTANGULO:
            std::cout << "Ingrese base del rectangulo: ";
            std::cin >> size1;
            std::cout << "Ingrese la altura del rectangulo: ";
            std::cin >> size2;
            dibujarFigura(cursorX, cursorY, orientacion, size1, size2, colorSeleccionado, RECTANGULO, carac);
            break;
        case CIRCULO:
            std::cout << "Ingrese el radio del circulo: ";
            std::cin >> size1;
            dibujarFigura(cursorX, cursorY, orientacion, size1, 0, colorSeleccionado, CIRCULO, carac);
            break;
        case LINEA:
            std::cout << "Ingrese la longitud o largo de la linea: ";
            std::cin >> size1;
            dibujarLinea(cursorX, cursorY, static_cast<Direccion>(orientacion), size1, colorSeleccionado, carac);
            break;
        case ROMBO:
            std::cout << "Ingrese el tamano de un lado del rombo: ";
            std::cin >> size1;
            dibujarFigura(cursorX, cursorY, orientacion, size1, size1, colorSeleccionado, ROMBO, carac);
            break;
        case HEXAGONO:
            std::cout << "Ingrese el tamano de un lado del hexagono (ME DISCULPO POR EL EXAGONO TAN FEO :D): ";
            std::cin >> size1;
            dibujarFigura(cursorX, cursorY, orientacion, size1, size1, colorSeleccionado, HEXAGONO, carac);
            break;
        default:
            std::cout << "Opcion no valida.\n";
    }
    cambiarColor(colorActual);
}

// Guarda el estado actual de la pantalla en un archivo
// Uso: Para persistir el estado actual de la pantalla en un archivo de texto
// Mantenimiento: Se puede mejorar anadiendo formatos adicionales de archivo
void grabarPantalla() {
    std::string nombreArchivo;
    std::cout << "Ingrese nombre del archivo para guardar (con extension .TXT): ";
    std::cin >> nombreArchivo;

    std::ofstream archivo(nombreArchivo);
    if (archivo.is_open()) {
        for (int y = 0; y < yPantalla; ++y) {
            for (int x = 0; x < xPantalla; ++x) {
                archivo << contPantalla[y][x] << " " << colorPantalla[y][x] << " ";
            }
            archivo << std::endl;
        }
        std::cout << "La pantalla se ha guardado correctamente en el archivo: " << nombreArchivo << std::endl;
    } else {
        std::cerr << "Error: No se pudo abrir el archivo." << std::endl;
    }
}

// Abre un archivo y carga su contenido en la pantalla
// Uso: Para restaurar el estado de la pantalla desde un archivo de texto
// Mantenimiento: Se puede mejorar anadiendo soporte para diferentes formatos de archivo
void abrirArchivo(const std::string &nombreArchivo) {
    std::ifstream archivo(nombreArchivo);
    if (archivo.is_open()) {
        limpiarPantalla();
        std::string linea;
        int y = 0;
        while (getline(archivo, linea) && y < yPantalla) {
            std::istringstream iss(linea);
            for (int x = 0; x < xPantalla; ++x) {
                char carac;
                int color;
                if (iss >> carac >> color) {
                    contPantalla[y][x] = carac;
                    colorPantalla[y][x] = color;
                }
            }
            ++y;
        }
        std::cout << "El archivo se ha abierto correctamente: " << nombreArchivo << std::endl;
        actualizarPantalla();
    } else {
        std::cerr << "Error: No se pudo abrir el archivo." << std::endl;
    }
}

// Permite modificar las opciones de dibujo (caracter y color)
// Uso: Para personalizar las opciones de dibujo
// Mantenimiento: Se puede mejorar anadiendo mas opciones de personalizacion
void modificarOpciones() {
    std::cout << "Ingrese el nuevo caracter, letra, numero o lo que se te ocurra para dibujar: ";
    std::cin >> caracDibujo;
    seleccionarColor();
}

// Permite seleccionar un color de entre las opciones disponibles
// Uso: Para elegir el color del dibujo
// Mantenimiento: Se puede mejorar anadiendo mas colores o una seleccion mas visual
WORD seleccionarColor() {
    mostrarSubMenuColorCaracter();
    int opcionColor;
    std::cout << "\nSeleccione una opcion de color: ";
    std::cin >> opcionColor;
    switch(opcionColor) {
        case 1: return COLOR_ROJO;
        case 2: return COLOR_VERDE;
        case 3: return COLOR_AZUL;
        default: std::cout << "Opcion de color no valida. Manteniendo el color actual.\n"; return obtenerColorActual();
    }
}

// Permite seleccionar la orientacion del dibujo
// Uso: Para definir la direccion en la que se dibujara
// Mantenimiento: Se puede mejorar anadiendo mas opciones de orientacion
int seleccionarOrientacion() {
    mostrarSubMenuDireccion();
    int opcion;
    std::cout << "\nSeleccione una opcion de orientacion: ";
    std::cin >> opcion;

    if (opcion >= 1 && opcion <= 8) return opcion;
    else {
        std::cout << "Opcion no valida. Dibujando abajo por defecto.\n";
        return 2; // Por defecto dibuja hacia abajo si la opcion es invalida
    }
}

// Permite seleccionar el caracter de dibujo
// Uso: Para elegir el caracter que se utilizara en el dibujo
// Mantenimiento: No se esperan cambios a menos que se agreguen mas tipos de caracteres
char seleccionarCaracter() {
    std::cout << "Seleccione el caracter, letra o lo que se te ocurra de dibujo:\n";
    char carac;
    std::cin >> carac;
    return carac;
}

// Muestra el menu principal
// Uso: Para desplegar el menu de opciones principales al usuario
// Mantenimiento: Se puede mejorar anadiendo mas opciones o submenus
void mostrarMenuPrincipal() {
    reiniciarCursorMenu();
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    const int menuX = MAX_ANCHO_PANTALLA + 2;
    moverCursorConsola(menuX, 0);
    std::cout << "-------------------";
    moverCursorConsola(menuX, 2);
    std::cout << "MENU DE OPCIONES PRINCIPALES:";
    moverCursorConsola(menuX, 5);
    std::cout << "F1: Triangulo";
    moverCursorConsola(menuX, 6);
    std::cout << "F2: Cuadrado";
    moverCursorConsola(menuX, 7);
    std::cout << "F3: Rectangulo";
    moverCursorConsola(menuX, 8);
    std::cout << "F4: Circulo";
    moverCursorConsola(menuX, 9);
    std::cout << "F5: Linea";
    moverCursorConsola(menuX, 10);
    std::cout << "F6: Rombo";
    moverCursorConsola(menuX, 11);
    std::cout << "F7: Hexagono";
    moverCursorConsola(menuX, 13);
    std::cout << "F9: Limpiar Pantalla";
    moverCursorConsola(menuX, 14);
    std::cout << "F12: Grabar Pantalla";
    moverCursorConsola(menuX, 15);
    std::cout << "Ctrl + O: Abrir archivo y mostrar en pantalla";
    moverCursorConsola(menuX, 16);
    std::cout << "Presiona 'Q' para salir";
}

// Muestra el submenu para seleccionar color
// Uso: Para desplegar las opciones de color al usuario
// Mantenimiento: Se puede mejorar anadiendo una interfaz grafica para la seleccion de color
void mostrarSubMenuColorCaracter() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    const int menuX = MAX_ANCHO_PANTALLA + 2;
    moverCursorConsola(menuX, 18);
    std::cout << "-------------------";
    moverCursorConsola(menuX, 20);
    std::cout << "ELIGE EL COLOR DEL CARACTER:";
    moverCursorConsola(menuX, 22);
    std::cout << "1. Rojo";
    moverCursorConsola(menuX, 23);
    std::cout << "2. Verde";
    moverCursorConsola(menuX, 24);
    std::cout << "3. Azul";
}

// Muestra el submenu para seleccionar direccion
// Uso: Para desplegar las opciones de direccion al usuario
// Mantenimiento: Se puede mejorar anadiendo mas direcciones o una representacion visual de la direccion
void mostrarSubMenuDireccion() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    const int menuX = MAX_ANCHO_PANTALLA + 2;
    moverCursorConsola(menuX, 26);
    std::cout << "-------------------";
    moverCursorConsola(menuX, 28);
    std::cout << "SELECCIONE LA ORIENTACION DEL DIBUJO:";
    moverCursorConsola(menuX, 29);
    std::cout << "DE LA OPCION 5 EN ADELANTE APLICA SOLO PARA LA LINEA:";
    moverCursorConsola(menuX, 31);
    std::cout << "1: Arriba";
    moverCursorConsola(menuX, 32);
    std::cout << "2: Abajo";
    moverCursorConsola(menuX, 33);
    std::cout << "3: Izquierda";
    moverCursorConsola(menuX, 34);
    std::cout << "4: Derecha";
    moverCursorConsola(menuX, 35);
    std::cout << "5: Diagonal arriba a la izquierda '\'";
    moverCursorConsola(menuX, 36);
    std::cout << "6: Diagonal arriba a la derecha '/'";
    moverCursorConsola(menuX, 37);
    std::cout << "7: Diagonal abajo a la izquierda";
    moverCursorConsola(menuX, 38);
    std::cout << "8: Diagonal abajo a la derecha";
}

// Reinicia el cursor del menu a la posicion inicial
// Uso: Para restablecer la posicion inicial del menu
// Mantenimiento: No se esperan cambios
void reiniciarCursorMenu() {
    menuY = 12;
}

// Dibuja una figura en la posicion especificada con las caracteristicas dadas
// Uso: Para dibujar diferentes figuras geometricas en la pantalla
// Mantenimiento: Se puede mejorar anadiendo mas tipos de figuras o propiedades de dibujo
void dibujarFigura(int x, int y, int orientacion, int size1, int size2, WORD color, Figura figura, char carac) {
    // Reubicacion del cursor segun la orientacion
    switch (orientacion) {
        case ARRIBA: y -= size2; break;
        case IZQUIERDA: x -= size1; break;
        default: break;
    }

    auto dibujarPunto = [&](int nx, int ny) {
        calcularPosicionCircular(nx, ny);
        if (nx >= 0 && nx < xPantalla && ny >= 0 && ny < yPantalla) {
            contPantalla[ny][nx] = carac;
            colorPantalla[ny][nx] = color;
        }
    };

    switch (figura) {
        case TRIANGULO: {
            int altura = size1 / 2;
            for (int i = 0; i <= altura; ++i) {
                for (int j = -i; j <= i; ++j) {
                    if (i == altura || j == -i || j == i)
                        dibujarPunto(x + j, y + i);
                }
            }
            break;
        }
        case CIRCULO: {
            for (int ny = -size1; ny <= size1; ++ny) {
                for (int nx = -size1; nx <= size1; ++nx) {
                    if (std::abs((nx * nx + ny * ny) - (size1 * size1)) < size1)
                        dibujarPunto(x + nx, y + ny);
                }
            }
            break;
        }
        case CUADRADO: {
            for (int i = 0; i < size1; ++i) {
                for (int j = 0; j < size1; ++j) {
                    if (i == 0 || i == size1 - 1 || j == 0 || j == size1 - 1)
                        dibujarPunto(x + j, y + i);
                }
            }
            break;
        }
        case RECTANGULO: {
            for (int i = 0; i < size2; ++i) {
                for (int j = 0; j < size1; ++j) {
                    if (i == 0 || i == size2 - 1 || j == 0 || j == size1 - 1)
                        dibujarPunto(x + j, y + i);
                }
            }
            break;
        }
        case ROMBO: {
            for (int i = -size1; i <= size1; ++i) {
                for (int j = -size1; j <= size1; ++j) {
                    if (abs(i) + abs(j) == size1)
                        dibujarPunto(x + j, y + i);
                }
            }
            break;
        }
        case HEXAGONO: {
            const int dx[] = { 1, 1, 0, -1, -1, 0 };
            const int dy[] = { 0, 1, 1, 0, -1, -1 };

            int nx = x, ny = y;
            for (int i = 0; i < 6; ++i) {
                for (int j = 0; j < size1; ++j) {
                    dibujarPunto(nx, ny);
                    nx += dx[i];
                    ny += dy[i];
                }
            }
            break;
        }
        default:
            std::cout << "Figura no reconocida.\n";
    }
    actualizarPantalla();
}

// Dibuja una linea desde la posicion actual en la direccion y longitud especificada
// Uso: Para dibujar lineas en diferentes direcciones desde el cursor
// Mantenimiento: Se puede mejorar anadiendo mas tipos de lineas (puneteada, continua, etc.)
void dibujarLinea(int x, int y, Direccion direccion, int longitud, WORD color, char carac) {
    int dx = 0, dy = 0;
    switch (direccion) {
        case ARRIBA: dy = -1; break;
        case ABAJO: dy = 1; break;
        case IZQUIERDA: dx = -1; break;
        case DERECHA: dx = 1; break;
        case DIAG_ARR_IZQ: dx = -1; dy = -1; break;
        case DIAG_ARR_DER: dx = 1; dy = -1; break;
        case DIAG_ABJ_IZQ: dx = -1; dy = 1; break;
        case DIAG_ABJ_DER: dx = 1; dy = 1; break;
        default: std::cout << "Direccion no valida.\n"; return;
    }
    for (int i = 0; i < longitud; ++i) {
        int nx = x + i * dx, ny = y + i * dy;
        calcularPosicionCircular(nx, ny);
        if (nx >= 0 && nx < xPantalla && ny >= 0 && ny < yPantalla) {
            contPantalla[ny][nx] = carac;
            colorPantalla[ny][nx] = color;
        }
    }
    actualizarPantalla();
}

int main() {
    std::cout << "Bienvenido a mi primer proyecto de ingenieria PROGRA1 UMG para dibujar figuras geometricas.\n\nPresione 'M' para mostrar menu de opciones\n\nPresione las teclas de funcion (F1-F12) para seleccionar una accion, o presione 'Q' para salir.\n\nPresione Ctrl + O para abrir un archivo y mostrar en pantalla.\n";
    bool ejecucion = true;
    while (ejecucion) {
        for (int identificador = VK_F1; identificador <= VK_F12; identificador++)
            if (GetAsyncKeyState(identificador) & 0x0001) procesarTeclaFuncion(identificador);
        if (GetAsyncKeyState('Q') & 0x8000) ejecucion = false;
        if (GetAsyncKeyState('M') & 0x8000) mostrarMenuPrincipal();
        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && ((GetAsyncKeyState('O') & 0x8000) || (GetAsyncKeyState('o') & 0x8000))) {
            std::string nombreArchivo;
            std::cout << "Ingrese el nombre del archivo a abrir con la extension: ";
            std::cin >> nombreArchivo;
            abrirArchivo(nombreArchivo);
        }
        Sleep(100);
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) moverCursor(-1, 0);
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) moverCursor(1, 0);
        if (GetAsyncKeyState(VK_UP) & 0x8000) moverCursor(0, -1);
        if (GetAsyncKeyState(VK_DOWN) & 0x8000) moverCursor(0, 1);
    }
    cambiarColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << "Saliendo del programa...\n";
    return 0;
}
