import subprocess

def jugar_partida(comando, soy_jugador_1):
    try:
        # Lanzamos el simulador y capturamos lo que escupe por pantalla
        proceso = subprocess.run(comando, shell=True, capture_output=True, text=True)
        salida = proceso.stdout
        
        # Analizamos el texto final para ver quién gana
        if "Gana el Jugador 1" in salida:
            return "Victoria" if soy_jugador_1 else "Derrota"
        elif "Gana el Jugador 2" in salida:
            return "Derrota" if soy_jugador_1 else "Victoria"
        elif "Empate" in salida:
            return "Empate"
        else:
            return "Error (Revisa la consola)"
            
    except Exception as e:
        return f"Error de ejecución: {e}"

# Configuramos lo que queremos probar
heuristicas = [0, 1, 2]
ninjas = [1, 2, 3, 4]

print("⚔️ INICIANDO EL TORNEO AUTOMATIZADO ⚔️")
print("Ten paciencia, la profundidad 7 tarda un rato...\n")

for h in heuristicas:
    print(f"=====================================")
    print(f"       TESTEANDO HEURÍSTICA {h}      ")
    print(f"=====================================")
    puntos = 0.0
    
    for n in ninjas:
        # Partida como Jugador 1 (Atacando)
        cmd_p1 = f"./n_en_rayaSH -p1 inteligente -id1 {h} -p2 ninja{n} -d 7 -nogui"
        print(f"H{h} vs Ninja {n} (Tú eres P1)... ", end="", flush=True)
        resultado_p1 = jugar_partida(cmd_p1, soy_jugador_1=True)
        
        # Puntuamos
        if resultado_p1 == "Victoria": puntos += 1
        elif resultado_p1 == "Empate": puntos += 0.5
        
        # Mostramos el resultado con un poco de color
        if resultado_p1 == "Victoria": print("✅ VICTORIA")
        elif resultado_p1 == "Derrota": print("❌ DERROTA")
        else: print("🟰 EMPATE")

        # Partida como Jugador 2 (Defendiendo)
        cmd_p2 = f"./n_en_rayaSH -p1 ninja{n} -p2 inteligente -id2 {h} -d 7 -nogui"
        print(f"H{h} vs Ninja {n} (Tú eres P2)... ", end="", flush=True)
        resultado_p2 = jugar_partida(cmd_p2, soy_jugador_1=False)
        
        # Puntuamos
        if resultado_p2 == "Victoria": puntos += 1
        elif resultado_p2 == "Empate": puntos += 0.5
        
        # Mostramos el resultado
        if resultado_p2 == "Victoria": print("✅ VICTORIA")
        elif resultado_p2 == "Derrota": print("❌ DERROTA")
        else: print("🟰 EMPATE")
        
    print(f"\n🏆 PUNTUACIÓN FINAL HEURÍSTICA {h}: {puntos}/8.0 puntos\n")
