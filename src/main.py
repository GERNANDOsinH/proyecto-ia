import ae_modulo
import matplotlib.pyplot as plt

TAMANO_POBLACION = 500
BETA = 0.1
ITERACIONES_TOTALES = 100000
PASO_GRAFICO = 5000
RUTA = "instancias/instancia_1.txt"

historial_normal_mejor = []
historial_uniforme_mejor = []
eje_x = []

try:
    ae_uniforme = ae_modulo.Uniform_AE(TAMANO_POBLACION, BETA, RUTA)
    ae_punto = ae_modulo.AE(TAMANO_POBLACION, BETA, RUTA)

except RuntimeError as e:
    print(f"\nError: {e}")

for iteracion_actual in range(0, ITERACIONES_TOTALES, PASO_GRAFICO):
    # Avanzar 10 iteraciones en el algoritmo normal
    ae_punto.solve(PASO_GRAFICO)
    res_normal = ae_punto.print()
    
    # Avanzar 10 iteraciones en el algoritmo uniforme
    ae_uniforme.solve(PASO_GRAFICO)
    res_uniforme = ae_uniforme.print()
    
    # Guardar los mejores resultados de la FE actual
    historial_normal_mejor.append(res_normal.better_FE)
    historial_uniforme_mejor.append(res_uniforme.better_FE)
    
    # Guardar el número de iteración para el eje X
    eje_x.append(iteracion_actual + PASO_GRAFICO)
    
    # Progreso en consola
    if (iteracion_actual + PASO_GRAFICO) % 100 == 0:
        print(f" -> Iteración {iteracion_actual + PASO_GRAFICO}/{ITERACIONES_TOTALES} procesada.")

print("\nGenerando gráfica de convergencia...")

plt.figure(figsize=(10, 6))

plt.plot(eje_x, historial_normal_mejor, label='AE Normal (Cruce Estándar)', color='blue', linewidth=2)
plt.plot(eje_x, historial_uniforme_mejor, label='AE Uniforme (Cruce Uniforme)', color='orange', linewidth=2)

plt.title('Curva de Convergencia: AE Normal vs AE Uniforme', fontsize=14, fontweight='bold')
plt.xlabel('Iteraciones', fontsize=12)
plt.ylabel('Mejor Función de Evaluación (Fitness)', fontsize=12)
plt.grid(True, linestyle='--', alpha=0.6)
plt.legend(fontsize=11)

mejor_normal_final = historial_normal_mejor[-1]
mejor_uniforme_final = historial_uniforme_mejor[-1]
print(f"\nResultado Final:")
print(f" - AE Normal mejor FE: {mejor_normal_final}")
print(f" - AE Uniforme mejor FE: {mejor_uniforme_final}")

# Desplegar la ventana con el gráfico
plt.show()