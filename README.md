<div id="header" align="center">
  <img src="https://media.giphy.com/media/M9gbBd9nbDrOTu1Mqx/giphy.gif" width="100"/>
</div>

<div align="center">
  <img src="https://media.giphy.com/media/dWesBcTLavkZuG35MI/giphy.gif" width="600" height="300"/>
</div>

---

<div align="center">
  <h1>
    <p>UNIVERSIDAD DE COSTA RICA</p>
    <p>ESCUELA DE CIENCIAS DE LA COMPUTACION E INFORMATICA</p>
    <p>CI-0117 PROGRAMACION PARALELA & CONCURRENTE — I Ciclo 2026</p>
  </h1>
</div>

<div align="center">
  <h2>
    <p>Especificación de Tarea Programada</p>
  </h2>
</div>

<div align="center">
  <h3>
    <p>Cliente de Mensajería con C y MPI</p>
  </h3>
</div>

<!-- ======================================================================= -->

<div align="center">
  <table>
    <thead>
      <tr>
        <th></th>
        <th></th>
        <th></th>
        <th></th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td><strong>Modalidad</strong></td>
        <td>Individual (Grupos de una persona)</td>
      </tr>
      <tr>
        <td><strong>Fecha y hora de entrega</strong></td>
        <td>martes 23 de junio de 2026, antes de las 06:55 a.m.</td>
      </tr>
      <tr>
        <td><strong>Presentación</strong></td>
        <td>martes 23 de junio de 2026, 07:00 a.m.</td>
      </tr>
      <tr>
        <td><strong>Valor</strong></td>
        <td>100 puntos</td>
      </tr>
    </tbody>
  </table>
</div>

---

**Mauricio Artavia Monge | C10743**

# Descripcion de la Tarea Programada

La presente tarea programada nos lleva a implementar un sistema de mensajeria distribuida similar a lo que son aplicaciones como WhatsApp o Telegram, haciendo uso de **`C`** y **`OpenMPI`**.

Mi sistema permite:

- Enviar mensajes directos entre varios procesos.
- Envar mensajes por difusion, conocido tambien como `broadcast`.
- Ejecutar multiples instancias las cuales son concurrentes.
- Visualizar mensajes mediante `CLI` o interfaz grafica `(GUI)` por medio de **GTK**.
  
El sistema lo base en una arquitectura de cliente-coordinador, donde nuestro proceso `rank 0` va a actuar como un intermediario central durante la mensajeria.

---

# Arquitectura del Sistema

## Componentes Principales

### Coordinador (`rank 0`)

- Registro de clientes al inicio.
- Mantenimiento de la tabla `rank <-> usuario`
- Enrutamiento de mensajes:
- - Directos
- - Difusion o broadcast
- Distribucion de lista de usuarios a cada cliente

### Clientes (`rank  1..N`)
- Envio de mensajes al coordinador
- Reciben mensajes del coordinador
- Ejecucion de interfaz (`CLI` o `GUI`)
- Uso de hilos para comunicacion concurrente

---

## Concurrencia

Con cada cliente tenemos una separacion de:

- Un hilo de interfaz (`CLI` o `GUI`)
- Un hilo receptor MPI (`pthread`)

Con esto se logra evitar la aparicion de bloqueos durante la interfaz grafica.

Utilizando:

- `MPI_Init_thread(MPI_THREAD_MULTIPLE)`
- `pthread_create`
- `MPI_Iprobe` para recepciones no bloqueantes

## Diseno de concurrencia

El sistema nos evita bloqueo un de la interfaz utilizando:
- Thread separado para recepción MPI
- Cola de mensajes interna

--- 

# Protocolo de mensajes

Uso de un struct tal como: 

```c
typedef struct {
  int sender_rank;
  int dest_ranks[MAX_DEST];
  int dest_count;
  size_t body_len;
  char body[MAX_BODY_LEN];
} Message;
```

## Campos a usar

- `sender_rank`: el proceso origen
- `dest_ranks[]`: la lista de destinos
- `dest_count`: la cantidad de destinos
- `body_len`: la longitud del mensaje
- `body`: el contenido del mensaje

---

## Tags MPI

| **TAG** | **SIGNIFICADO**   |
| ------- | ----------------- |
| **1**   | Mensaje Directo   |
| **2**   | Broadcast         |
| **3**   | Registro          |
| **4**   | Lista de Usuarios |

## Serializacion

El protocolo es serializado cuando se usa una copia directa de memoria:

```c
memcpy(buffer, msg, sizeof(Message));
```

---

# MPI Wrapper

## Proposito

- Encapsulamiento llamadas MPI
- Simplificacion del codigo
- Centralizado de logica de comunicacion
- Facilidad de mantenimiento.

---

# Interfaces

## `GUI` (**GTK**)

Podemos observar en la interfaz grafica lo siguiente:

- Lista de contactos
- Seleccion de modo:
- - Directo
- - Difusion
- Area de chat en tiempo real
- Hilo separado que permite la recepcion de mensajes

## `CLI`

- Permite el uso para pruebas de latencia
- Usa `MPI_Wtime`
- Permite la ejecucion sin interfaz grafica

---

# Pruebas y Calidad

## Unit tests

- Serializacion / Deserializacion de mensajes
- Integridad del protocolo
- Casos limite del cuerpo del mensaje

***Ejecucion:***

```bash
make test
```

---

## Sanitizers

Valida ausencia de:

- Memory leaks
- Undefined behavior
- Data races

***Ejecucion:***

```bash
make sanitize
```

## Valgrind

Este trabajo nos incluye un target automatizado para analisis de memoria con Valgrind.

Los logs que se generan seran almacenados en:

```bash
logs/valgrind-*.log
```

***Ejecucion:***

```bash
make valgrind
```

Esto nos ha de permitir:

- Deteccion de memory leaks
- Deteccion de accesos invalidos
- Validacion de integridad en la ejecucion concurrente de `MPI`

---

# Compilacion

```bash
make
```

---

# Limpieza

```bash
make clean
```

Elimina:

- `bin/`
- `logs/`

---

# Ejecucion

## MPI con GUI

```bash
mpirun -np N ./bin/mensajeria user1 user2 user3 ... userN --gui
```

## CLI / Pruebas

```bash
mpirun -np N ./bin/mensajeria user1 user2 user3 ... userN --test 2 10
```

---

# Estrucura del Proyecto

```
Messaging-Client/
├── assets/
│   └── diagrams/
│       ├── architecture_diagram.png
│       ├── concurrency_diagram.png
│       └── message_flow.png
│
├── bin/
│   ├── mensajeria
│   └── test_protocol
│
├── include/
│   ├── client.h
│   ├── coordinator.h
│   ├── gui.h
│   ├── mpi_wrapper.h
│   └── protocol.h
│
├── logs/
│   └── valgrind-*.log
│
├── src/
│   ├── main.c
│   ├── coordinator.c
│   ├── client.c
│   ├── gui.c
│   ├── cli.c
│   ├── mpi_wrapper.c
│   └── protocol.c
│
├── tests/
│   └── test_protocol.c
│
├── assignment.md
├── README.md
└── Makefile
```

### Descripción

* `src/`: implementación principal del sistema.
* `include/`: encabezados públicos del proyecto.
* `tests/`: pruebas unitarias.
* `bin/`: ejecutables generados por el proceso de compilación.
* `assets/diagrams/`: diagramas de arquitectura, concurrencia y flujo de mensajes.
* `README.md`: documentación técnica y guía de uso.
* `assignment.md`: especificación original de la tarea.
* `Makefile`: automatización de compilación y pruebas.


---

# Decisiones del diseno

- MPI es usado como sistema de comunicacion distribuida
- Uso del coordinador central para simplificacion del routing
- Se decidio el uso de pthreads para la concurrencia de los clientes
- GTK como principal interfaz grafica beta
- Serializacion manual para obtener control total del protocolo
- Se implemento el Wrapper MPI para poder desacoplar logica de comunicacion
---

# Referencias

- OpenMPI Documentation: [https://docs.open-mpi.org/](https://docs.open-mpi.org/)
- Message Passing Interface Forum: [https://www.mpi-forum.org/docs/](https://www.mpi-forum.org/docs/)
- GTK Project: [https://docs.gtk.org/gtk3/](https://docs.gtk.org/gtk3/)
- GNU Project: [https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html](https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html)
- Valgrind Developers: [https://valgrind.org/docs/manual/manual.html](https://valgrind.org/docs/manual/manual.html)
- Material de Clase, Prof. Sleyter Angulo.

---


## Evaluación

| Rubro | Descripción | Puntos |
| :---: | :---- | :---: |
| **Diagramas** | Arquitectura, claridad y justificación de las decisiones de diseño. | $7,5$ |
| **Código <br> (implementación)** | Cumplimiento de convenciones de nombres y estándares altos de codificación, pruebas unitarias y casos de prueba. | $20$ |
| **Interfaz gráfica de usuario** | GUI funcional construida por código (sin diseñadores de arrastrar y soltar). | $15$ |
| **Documentación** | README y documentación técnica del diseño y del protocolo. | $7,5$ |
| **Presentación** | Dominio de la arquitectura, demostración en vivo y respuestas a preguntas. | $50$ |
|  | **TOTAL** | **$100$** |  |

## Rúbrica de evaluación y retroalimentación

La siguiente tabla se utilizará durante la revisión y la presentación de la tarea. La columna de
comentarios está reservada para las observaciones del profesor.

| Rubro | Criterios de evaluación | Puntos | Obtenidos | Comentarios del profesor |
| :-- | :-- | :-- | :-- | :-- |
| **Diagramas ($7,5$ pts)** | Los diagramas representan la arquitectura completa de la aplicación; las decisiones de diseño están analizadas y justificadas; la persona estudiante comprende el diseño y puede presentarlo y demostrarlo. | $7,5$ |  |  |
| **Código ($20$ pts)** | El código sigue las convenciones de nombres y estándares altos de codificación; es modular y legible; incluye pruebas unitarias y casos de prueba; compila y se ejecuta sin errores con varias instancias simultáneas. | $20$ |  |  |
| **Interfaz gráfica ($15$ pts)** | La GUI fue construida por código con una biblioteca aprobada (GTK, Qt, FLTK u otra); permite enviar y recibir mensajes directos y de difusión; no se congela durante la comunicación. Existe además la versión CLI para pruebas y métricas. | $15$ |  |  |
| **Documentación ($7,5$ pts)** | El README explica la compilación, la ejecución y el uso; se documentan el protocolo de mensajes y el diseño; el repositorio Git muestra un historial de trabajo consistente. | $7,5$ |  |  |
| **Presentación ($50$ pts)** | Puntualidad (07:00 a. m.); dominio de la arquitectura y de las decisiones de diseño; demostración en vivo con múltiples instancias; calidad de las respuestas a las preguntas; manejo del tiempo. | $50$ |  |  |
|  | **TOTAL** | $100$ |  |  |

Estudiante: _________________________________________ Carné: ________________

Nota final: ________ / 100 Fecha: ____________________

---

<div align="center" style="font-size: 10px;">
  <i> @C10743 - Mauricio Artavia Monge </i>
</div>