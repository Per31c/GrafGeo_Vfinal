# Proyecto de Dibujo de Figuras Geométricas en Consola

## Descripción Técnica

Este proyecto permite dibujar figuras geométricas en una consola de Windows. Utiliza la biblioteca `windows.h` para manejar colores y posiciones del cursor en la consola. Las figuras pueden ser personalizadas con diferentes colores, orientaciones y tamaños.

## Estructura del Código

### Archivos Incluidos

- `<iostream>`
- `<cmath>`
- `<vector>`
- `<fstream>`
- `<windows.h>`
- `<sstream>`

### Constantes

- `MAX_ALTO_PANTALLA`: Altura máxima de la pantalla.
- `MAX_ANCHO_PANTALLA`: Ancho máximo de la pantalla.
- `COLOR_ROJO`, `COLOR_VERDE`, `COLOR_AZUL`: Colores predefinidos para el texto en la consola.

### Variables Globales

- `xPantalla`, `yPantalla`: Dimensiones actuales de la pantalla.
- `contPantalla`: Matriz que almacena los caracteres en pantalla.
- `colorPantalla`: Matriz que almacena los colores en pantalla.
- `caracDibujo`: Carácter por defecto para dibujar figuras.
- `cursorX`, `cursorY`: Coordenadas actuales del cursor.
- `menuY`: Línea inicial del menú lateral.

### Enumeraciones

- `Figura`: Tipos de figuras geométricas (TRIANGULO, CUADRADO, RECTANGULO, CIRCULO, LINEA, ROMBO, HEXAGONO).
- `Direccion`: Direcciones de movimiento (ARRIBA, ABAJO, IZQUIERDA, DERECHA, DIAG_ARR_IZQ, DIAG_ARR_DER, DIAG_ABJ_IZQ, DIAG_ABJ_DER).

### Funciones Principales

#### Funciones de Manejo de la Consola

- `cambiarColor(WORD color)`: Cambia el color del texto en la consola.
- `obtenerColorActual()`: Obtiene el color actual del texto en la consola.
- `moverCursorConsola(int x, int y)`: Mueve el cursor a una posición específica en la consola.

#### Funciones de Dibujo

- `dibujarFigura(int x, int y, int orientacion, int size1, int size2, WORD color, Figura figura, char carac)`: Dibuja una figura en la posición especificada con las características dadas.
- `dibujarLinea(int x, int y, Direccion direccion, int longitud, WORD color, char carac)`: Dibuja una línea desde la posición actual en la dirección y longitud especificada.

#### Funciones de Manejo de la Pantalla

- `limpiarPantalla()`: Limpia la pantalla y reinicia los valores.
- `actualizarPantalla()`: Actualiza el contenido de la pantalla según los buffers de contenido y color.
- `grabarPantalla()`: Guarda el estado actual de la pantalla en un archivo.
- `abrirArchivo(const std::string &nombreArchivo)`: Abre un archivo y carga su contenido en la pantalla.

#### Funciones de Interacción

- `procesarTeclaFuncion(int tecla)`: Procesa las teclas de función y realiza la acción correspondiente.
- `seleccionarFigura(Figura figuraSeleccionada)`: Permite seleccionar una figura y sus características para dibujar.
- `modificarOpciones()`: Permite modificar las opciones de dibujo (carácter y color).
- `seleccionarColor()`: Permite seleccionar un color de entre las opciones disponibles.
- `seleccionarOrientacion()`: Permite seleccionar la orientación del dibujo.
- `seleccionarCaracter()`: Permite seleccionar el carácter de dibujo.
- `mostrarMenuPrincipal()`: Muestra el menú principal.
- `mostrarSubMenuColorCaracter()`: Muestra el submenú para seleccionar color.
- `mostrarSubMenuDireccion()`: Muestra el submenú para seleccionar dirección.
- `reiniciarCursorMenu()`: Reinicia el cursor del menú a la posición inicial.

### Main Loop

El bucle principal del programa captura las entradas del usuario y llama a las funciones correspondientes para actualizar la pantalla y procesar los comandos.

```cpp
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
## Contacto

Ing. Perez Marvin  
mperezc31@miumg.edu.gt
