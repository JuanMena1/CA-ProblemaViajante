# CA-ProblemaViajante
Este repositorio es para realizar el problema del viajante tanto de forma secuencial como de forma paralela con MPI.

## Versiones
### Versión 0.5
 - Creado esqueleto del programa

### Versión 0.8 
 - Digraph alojado en memoria y rellenado
 - Creados tour y besttour

### Versión 0.9
 - Plantilla del programa completo pasada a código
 - No funcional

### Versión 1.1
 - Cambios en el código.
 - Arregladas las violaciones de segmento.
 - No funcionan los métodos del stack.
 - Bucle infinito entre las primeras rutas.

### Versión 1.2
 - Arreglados métodos del stack.
 - Bucles infinitos por tours con 0s.

### Versión 1.3
 - Cambiados métodos de print.
 - Cambios en el código.
   - O añade 0 en los tour o se deja tour sin hacer.

### Versión 2.0
 - Versión funcional.
 - El error estaba en copiar tour->pobl completa y no solo la 1ª población (memcpy).

### Versión 2.1
 - Comentarios añadidos.