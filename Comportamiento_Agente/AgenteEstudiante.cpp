#include "AgenteEstudiante.hpp"
#include <iostream>
#include <limits>
#include <vector>
#include <algorithm>
#include <cmath>
#include <functional>

AgenteEstudiante::AgenteEstudiante(int id, int profundidadMax, double tiempoMax, int numHeuristica, ModoJuego modo) 
    : id(id), profundidadMax(profundidadMax), tiempoMaxSegundos(tiempoMax), numHeuristica(numHeuristica), modo(modo), abortarBanda(false) {
    nodosVisitados = 0;
}

bool AgenteEstudiante::tieneLimiteDeTiempo() const {
    return modo != ModoJuego::STATUS;
}

std::pair<int, int> AgenteEstudiante::think(const Tablero& tablero) {
    std::pair<int, int> mejor;
    nodosVisitados = 0;
    abortarBanda = false;
    inicioBusqueda = std::chrono::steady_clock::now();

    switch (modo)
    {
    case ModoJuego::ALEATORIO:
        return JuegaAleatorio(tablero);
        break;
    
    case ModoJuego::STATUS:
        Status(tablero, mejor);
        return mejor;
        break;    

    case ModoJuego::MINIMAX:
        minimax(tablero, 0, profundidadMax, mejor);
        return mejor;
        break; 

    case ModoJuego::INTELIGENTE:
        return JuegaInteligente(tablero);   
        break;
    }
        
    return {-1, -1};
}


/**
 * @brief Compara dos tableros para identificar cuál ha sido el movimiento realizado.
 * @param padre Estado inicial del tablero.
 * @param hijo Estado resultante tras un movimiento.
 * @return Un par (fila, columna) con la posición de la nueva pieza.
 */
std::pair<int, int> SacarMovimiento(const Tablero& padre, const Tablero &hijo){
    for(int f=0; f<padre.getFilas(); ++f)
        for(int c=0; c<padre.getColumnas(); ++c)
            if (padre.getCelda(f,c) == 0 && hijo.getCelda(f,c) != 0) 
                return {f, c};
    return {-1, -1};
}

/**
 * @brief Implementa un agente que juega de forma totalmente aleatoria.
 * @param tablero Estado actual del juego.
 * @return La jugada elegida al azar.
 */
std::pair<int, int> AgenteEstudiante::JuegaAleatorio(const Tablero& tablero) {

    // Calculo los tableros descendientes de tablero
    auto sucesores = tablero.getSucesores();

    // Si no tiene descendientes, paso el turno
    if (sucesores.empty()) return {-1, -1};

    // Elijo aleatoriamente uno de los descendientes
    int elegido = rand() % sucesores.size();

    // Saco el movimiento realizado comparando el tablero original con el elegido.
    std::pair<int,int> Mov = SacarMovimiento(tablero, sucesores[elegido]);

    return Mov;
}


/**
 * @brief Algoritmo de resolución completa para estados de final de juego.
 * Determina si una posición está matemáticamente ganada, perdida o empatada.
 * @param tablero Estado a evaluar.
 * @param Mov [Salida] La jugada óptima encontrada.
 * @return Resultado del análisis (VICTORIA, DERROTA o EMPATE).
 */
AgenteEstudiante::Resultado AgenteEstudiante::Status(const Tablero &tablero, std::pair<int,int> &Mov) {
    /* ============== Este trozo de código se tiene que quedar aquí  =============== */
    nodosVisitados++;
    /* ============== Empieza a partir de aquí tu implementación  =============== */

    int ganador = tablero.comprobarGanador();

    if (ganador == id) return Resultado::VICTORIA;
    else if (ganador != 0 && ganador != -1) return Resultado::DERROTA;
    else if (ganador == -1) return Resultado::EMPATE;

    auto sucesores = tablero.getSucesoresConMovimientos();
    bool esMiTurno = (tablero.getJugadorTurno() == id);

    if (!sucesores.empty()) Mov = sucesores[0].second;

    Resultado mejorResultado = esMiTurno ? Resultado::DERROTA : Resultado::VICTORIA;

    for (const auto& hijo : sucesores) {
        Tablero tableroHijo = hijo.first;
        pair<int, int> movimientoHijo = hijo.second;
        
        pair<int, int> movAUX; 
        Resultado resHijo = Status(tableroHijo, movAUX);

        if (esMiTurno) {
            if (resHijo == Resultado::VICTORIA) {
                mejorResultado = Resultado::VICTORIA;
                Mov = movimientoHijo;
                break;
            } 
            else if (resHijo == Resultado::EMPATE && mejorResultado == Resultado::DERROTA) {
                mejorResultado = Resultado::EMPATE;
                Mov = movimientoHijo; 
            }
        } else {
            if (resHijo == Resultado::DERROTA) {
                mejorResultado = Resultado::DERROTA;
                Mov = movimientoHijo;
                break;
            }
            else if (resHijo == Resultado::EMPATE && mejorResultado == Resultado::VICTORIA) {
                mejorResultado = Resultado::EMPATE;
                Mov = movimientoHijo;
            }
        }
    }
    return mejorResultado;
}



/**
 * @brief Implementación del algoritmo Minimax clásico.
 * @param tablero Estado actual.
 * @param profundidad Nivel actual en el árbol de búsqueda.
 * @param prof_Max Límite de profundidad de la búsqueda.
 * @param Mov [Salida] La mejor jugada encontrada en la raíz.
 * @return Valor heurístico del estado.
 */
double AgenteEstudiante::minimax(const Tablero &tablero, int profundidad, int prof_Max, std::pair<int,int> &Mov) {
    /* ============== Este trozo de código se tiene que quedar aquí  =============== */
    nodosVisitados++;
    if (abortarBanda) return 0;
    
    if (std::chrono::duration<double>(std::chrono::steady_clock::now() - inicioBusqueda).count() > tiempoMaxSegundos) {
        abortarBanda = true;
        return 0;
    
    }
   /* ============== Empieza a partir de aquí tu implementación  =============== */

    int ganador = tablero.comprobarGanador();

    if (ganador == id) return GANAR;
    else if (ganador != 0 && ganador != -1) return PERDER;
    else if (ganador == -1) return 0;
    if (profundidad >= prof_Max) return heuristica(tablero);

    auto sucesores = tablero.getSucesoresConMovimientos();
    bool esMiTurno = (tablero.getJugadorTurno() == id);

    if (!sucesores.empty()) Mov = sucesores[0].second;

    double mejorPuntuacion = esMiTurno ? MenosInfinito : MasInfinito;

    for (const auto& hijo : sucesores) {
        Tablero tableroHijo = hijo.first;
        pair<int, int> movimientoHijo = hijo.second; 
        pair<int, int> movAUX; 
        double puntuacionHijo = minimax(tableroHijo, profundidad + 1, prof_Max, movAUX);

        if (esMiTurno) {
            if (puntuacionHijo > mejorPuntuacion) {
                mejorPuntuacion = puntuacionHijo;
                Mov = movimientoHijo;
            }
        } else {
            if (puntuacionHijo < mejorPuntuacion) {
                mejorPuntuacion = puntuacionHijo;
                Mov = movimientoHijo;
            }
        }
    }

    if (profundidad == 0) cout << "Valor Minimax: " << mejorPuntuacion << " Jugada: (" << Mov.first << ", " << Mov.second << ")\n";

    return mejorPuntuacion;
}


/**
 * @brief Punto de entrada para el juego inteligente.
 * @param tablero Estado actual del juego.
 * @return La jugada elegida por el algoritmo de búsqueda.
 */
std::pair<int, int> AgenteEstudiante::JuegaInteligente(const Tablero& tablero) {
    std::pair<int,int> Mov;

    double valor = alfaBeta(tablero, 0, profundidadMax, MenosInfinito, MasInfinito, Mov);
    std::cout << "Valor Minimax: " << valor << "\tJugada: (" << Mov.first << ", " << Mov.second << ")\n";
    return Mov;
}


/**
 * @brief Implementación del algoritmo Minimax con Poda Alfa-Beta.
 * @param tablero Estado actual.
 * @param profundidad Nivel actual en el árbol de búsqueda.
 * @param prof_Max Límite de profundidad de la búsqueda.
 * @param alfa Valor mínimo garantizado para el jugador MAX.
 * @param beta Valor máximo garantizado para el jugador MIN.
 * @param Mov [Salida] La mejor jugada encontrada en la raíz.
 * @return Valor heurístico del estado tras la poda.
 */
double AgenteEstudiante::alfaBeta(const Tablero &tablero, int profundidad, int prof_Max, double alfa, double beta, std::pair<int,int> &Mov) {
    /* ============== Este trozo de código se tiene que quedar aquí  =============== */
    nodosVisitados++;
    if (abortarBanda) return 0;
    
    if (std::chrono::duration<double>(std::chrono::steady_clock::now() - inicioBusqueda).count() > tiempoMaxSegundos) {
        abortarBanda = true;
        return 0;
    }
    /* ============== Empieza a partir de aquí tu implementación  =============== */


    int ganador = tablero.comprobarGanador();
    if (ganador == id) return GANAR - profundidad;
    else if (ganador != 0 && ganador != -1) return PERDER + profundidad;
    else if (ganador == -1) return 0;

    if (profundidad >= prof_Max) return heuristica(tablero);

    auto sucesores = tablero.getSucesoresConMovimientos();
    bool esMiTurno = (tablero.getJugadorTurno() == id);

    if (!sucesores.empty()) Mov = sucesores[0].second;

    double mejorPuntuacion = esMiTurno ? MenosInfinito : MasInfinito;

    for (const auto& hijo : sucesores) {
        Tablero tableroHijo = hijo.first;
        pair<int, int> movimientoHijo = hijo.second; 
        pair<int, int> movAUX; 
        double puntuacionHijo = alfaBeta(tableroHijo, profundidad + 1, prof_Max, alfa, beta, movAUX);

        if (esMiTurno) {
            if (puntuacionHijo > mejorPuntuacion) {
                mejorPuntuacion = puntuacionHijo;
                Mov = movimientoHijo;
            }

            alfa = max(alfa, mejorPuntuacion);
            if (beta <= alfa)  break;

        } else {
            if (puntuacionHijo < mejorPuntuacion) {
                mejorPuntuacion = puntuacionHijo;
                Mov = movimientoHijo;
            }

            beta = min(beta, mejorPuntuacion);
            if (beta <= alfa) break;

        }
    }

    return mejorPuntuacion;
}

/**
 * @brief Función heurística para evaluar la calidad de un tablero.
 * @param tablero Estado a evaluar.
 * @return Puntuación numérica (positiva para ventaja de J1, negativa para J2).
 */
double AgenteEstudiante::heuristica(const Tablero& tablero) {
    switch(numHeuristica) {
        case 0: return heuristicaPrueba(tablero);
                break;
        case 1: return heuristica1(tablero);
                break;
        case 2: return heuristica2(tablero);
                break;
        default: return heuristica1(tablero);
    }
}

double AgenteEstudiante::heuristicaPrueba(const Tablero& tablero) {
    // n es el número de fichas en línea para ganar.
    int n = tablero.getNParaGanar();
    int oponente = (id == 1) ? 2 : 1;
    double score_positivo = 0;

    double score_negativo = 0;

    for (int f=0; f< tablero.getFilas(); f++ ){
        for (int c = 0; c< tablero.getColumnas(); c++){
            if (tablero.getCelda(f,c) != 0 ){
                int valor = tablero.getFilas()-abs(f-(tablero.getFilas()/2)) + tablero.getColumnas()-abs(c-(tablero.getColumnas()/2)); 
                if (tablero.getCelda(f,c) == id){
                  score_positivo += valor;
                 }
                else {
                  score_negativo += valor;
                }
            }
        }
    }

   
    return score_positivo - score_negativo;
}


double AgenteEstudiante::heuristica1(const Tablero& tablero) {
    //A implementar por el estudiante
    double puntuacion = 0.0;
    int rival = (id == 1) ? 2 : 1;

    //ATAQUE
    int mis_cuatros = tablero.contarCombinaciones(4, id);
    puntuacion += (mis_cuatros * 1000.0);

    int mis_tres = tablero.contarCombinaciones(3, id);
    puntuacion += (mis_tres * 100.0);
    
    int mis_doses = tablero.contarCombinaciones(2, id);
    puntuacion += (mis_doses * 10.0);


    //DEFENSA
    int rival_cuatros = tablero.contarCombinaciones(4, rival);
    puntuacion -= (rival_cuatros * 5000.0);

    int rival_tres = tablero.contarCombinaciones(3, rival);
    puntuacion -= (rival_tres * 200.0);

    int rival_doses = tablero.contarCombinaciones(2, rival);
    puntuacion -= (rival_doses * 15.0);


    return puntuacion;
}

double AgenteEstudiante::heuristica2(const Tablero& tablero) {
    //A implementar por el estudiante
    double puntuacion = 0.0;
    int rival = (id == 1) ? 2 : 1; 

    //ATAQUE
    int mis_cuatros = tablero.contarCombinaciones(4, id);
    puntuacion += (mis_cuatros * 1000000.0);

    int mis_tres = tablero.contarCombinaciones(3, id);
    puntuacion += (mis_tres * 5000.0);
    
    int mis_doses = tablero.contarCombinaciones(2, id);
    puntuacion += (mis_doses * 100.0);


    //DEFENSA
    int rival_cuatros = tablero.contarCombinaciones(4, rival);
    puntuacion -= (rival_cuatros * 10.0);

    int rival_tres = tablero.contarCombinaciones(3, rival);
    puntuacion -= (rival_tres * 5.0);

    int rival_doses = tablero.contarCombinaciones(2, rival);
    puntuacion -= (rival_doses * 1.0);


    return puntuacion;
}

