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

## 1. Descripción general

El propósito de esta tarea programada es que la persona estudiante aprenda a diseñar la arquitectura y el desarrollo de una aplicación de mensajería — similar a WhatsApp o Telegram — capaz de enviar mensajes a un destinatario específico o a una lista de difusión de destinatarios.

La aplicación se implementará en lenguaje `C` utilizando la biblioteca de paso de mensajes `OpenMPI`.

Para mantener la tarea en un nivel introductorio, la aplicación solo debe admitir mensajes en formato de texto plano. No debe enviar imágenes, audio ni stickers.

## 2. Especificación de la tarea

### 2.1 Requisitos funcionales

- Enviar mensajes de texto a un destinatario específico.

- Enviar mensajes de texto a una lista de difusión (broadcast) de destinatarios.

- Admitir únicamente texto plano: la aplicación no debe enviar imágenes, audio ni stickers.

- Permitir varias instancias de la aplicación en ejecución simultánea, cada una enviando y recibiendo mensajes.

- Incluir un proceso coordinador, correspondiente al `rank 0`, encargado de administrar y enrutar la mensajería.

- Ofrecer una interfaz gráfica de usuario (**GUI**) y una versión de linea de comandos (**CLI**).


### 2.2 Arquitectura sugerida del cliente

Se sugiere que cada instancia del cliente sea un proceso **MPI**, lanzado con `mpirun -np N ./cliente`, donde $N$ es el número de clientes simultáneos. La arquitectura recomendada es la siguiente:

- **Coordinador (rank 0):** registra a los clientes conectados, mantiene la tabla que asocia cada rank con un nombre de usuario, administra las listas de difusión y enruta cada mensaje hacia su destino. También puede registrar bitácoras y métricas globales.

- **Clientes (ranks $1$ a $N−1$):** envían sus mensajes al coordinador indicando el tipo (directo o difusión) mediante etiquetas (`tags`) de `MPI`, y reciben los mensajes dirigidos a ellos.

- **Concurrencia interna:** cada cliente debe separar el hilo de la interfaz del hilo de comunicación (por ejemplo, con `pthreads` y `MPI_Init_thread`), de manera que la **GUI** no se congele mientras se espera un mensaje.

- **Protocolo de mensajes:** defina una estructura con remitente, destinatario o lista de destino, longitud y cuerpo del mensaje, serializada como un búfer de caracteres. Documente este protocolo en el `README` y en los diagramas.

### 2.3 Interfaces de usuario

La tarea debe implementar una interfaz gráfica de usuario construida con alguna biblioteca de `C` para **GUI**, tal como `GTK`, `Qt` o `FLTK`. *Cualquier otra biblioteca debe ser aprobada previamente por el profesor*. La interfaz debe desarrollarse completamente por código: no se permite el uso de diseñadores de tipo arrastrar y soltar (*drag and drop*).

Además, debe implementarse una versión de línea de comandos (**CLI**) con el mismo núcleo de comunicación. Esta versión facilita la generación de casos de prueba y la obtención de métricas, por ejemplo, la latencia y el rendimiento medidos con `MPI_Wtime`.

## 3. Entregables

- Todos los archivos fuente de la aplicación.

- README con instrucciones de compilación, ejecución y uso, y con la documentación del protocolo de mensajes.

- Repositorio Git con un historial de commits consistente con el desarrollo del proyecto.

- Diagramas con el diseño de la arquitectura y el análisis que justifica las decisiones tomadas. La persona estudiante debe comprender este diseño en su totalidad y estar en capacidad de presentarlo y demostrarlo.

- Presentación para la defensa de la tarea.


## 4. Sugerencia de tecnología

- **Lenguaje:** `C` (estándar C11 o superior).

- **Paso de mensajes:** `OpenMPI`.

- **Interfaz gráfica:** `GTK`, `Qt` (por código, sin diseñadores de arrastrar y soltar) o `FLTK`; *otras bibliotecas requieren aprobación previa del profesor*.

- **Hilos:** pthreads para separar la interfaz de la comunicación.

- **Construcción:** `Make`.

- **Control de versiones:** `Git`.

- **Calidad:** `Valgrind` y los sanitizers de `GCC`/`Clang` para detectar errores de memoria y condiciones de carrera; `Check`, `CUnit` o `Unity` para las pruebas unitarias.

## 5. Funciones MPI de la biblioteca OpenMPI

La siguiente tabla resume las funciones de MPI recomendadas para esta tarea:

| Función                            | Propósito en esta tarea |
| :--------------------------------- | :---- |
| **`MPI_Init` / `MPI_Init_thread`** | Inicializan el entorno **MPI**. La variante `MPI_Init_thread` permite solicitar soporte de hilos (por ejemplo, `MPI_THREAD_FUNNELED`), necesario cuando la interfaz gráfica y la comunicación se ejecutan en hilos distintos. |
| **`MPI_Comm_rank`**                | Obtiene el identificador (`rank`) del proceso dentro del comunicador. El proceso con `rank 0` actúa como coordinador. |
| **`MPI_Comm_size`**                | Obtiene el número total de procesos (**clientes**) en ejecución. |
| **`MPI_Send` / `MPI_Recv`**        | Envío y recepción bloqueantes punto a punto. Base del intercambio de mensajes entre clientes y coordinador. |
| **`MPI_Isend` / `MPI_Irecv`**      | Versiones no bloqueantes del envío y la recepción. Útiles para que la interfaz no se congele mientras se espera un mensaje. |
| **`MPI_Probe` / `MPI_Iprobe`**     | Consultan si existe un mensaje pendiente (y sus metadatos) sin recibirlo. `MPI_Iprobe` permite sondear sin bloquear el hilo. |
| **`MPI_Get_count`**                | Determina la cantidad de elementos del mensaje recibido; permite reservar el búfer del tamaño correcto. |
| **`MPI_Bcast`**                    | Operación colectiva de difusión desde un proceso raíz hacia todos los demás; útil para anuncios globales del coordinador. |
| **`MPI_Barrier`**                  | Sincroniza todos los procesos; útil en el arranque ordenado de la aplicación y en las pruebas. |
| **`MPI_Wtime`**                    | Devuelve el tiempo transcurrido en segundos; sirve para medir métricas de latencia y rendimiento desde la versión **CLI**. |
| **`MPI_Finalize`**                 | Cierra ordenadamente el entorno **MPI** al terminar la aplicación. |
| **`MPI_Abort`**                    | Termina todos los procesos ante un error irrecuperable. |

## 6. Consideraciones

- La tarea debe desarrollarse en grupos de una persona (de manera individual).

- No se aceptarán entregas tardías. Si la plataforma de entrega está fuera de servicio (Mediación Virtual), la persona estudiante debe enviar su trabajo desde el correo institucional al profesor, con copia obligatoria a la persona asistente, antes de la hora límite.

- La presentación inicia a las 07:00 a. m. La persona estudiante que llegue más de 15 minutos tarde **NO PODRA PRESENTAR** y se registrará como ausente, conforme a lo dispuesto en el programa del curso y en los artículos 14 bis y 24 del Reglamento de Régimen Académico Estudiantil de la Universidad de Costa Rica. Para optar por la reposición, deberá presentar la solicitud con toda la documentación probatoria requerida, a más tardar cinco días hábiles después de reintegrarse a sus estudios (artículo 24).

- Queda prohibido el uso de inteligencia artificial de cualquier tipo. En caso de utilizarla, la calificación final de esta tarea será de **cero** y se iniciará el proceso administrativo correspondiente.

- El plagio también está prohibido y se sancionará conforme a la normativa de la Universidad de Costa Rica.


## 7. Evaluación

| Rubro | Descripción | Puntos |
| :---: | :---- | :---: |
| **Diagramas** | Arquitectura, claridad y justificación de las decisiones de diseño. | $7,5$ |
| **Código <br> (implementación)** | Cumplimiento de convenciones de nombres y estándares altos de codificación, pruebas unitarias y casos de prueba. | $20$ |
| **Interfaz gráfica de usuario** | GUI funcional construida por código (sin diseñadores de arrastrar y soltar). | $15$ |
| **Documentación** | README y documentación técnica del diseño y del protocolo. | $7,5$ |
| **Presentación** | Dominio de la arquitectura, demostración en vivo y respuestas a preguntas. | $50$ |
|  | **TOTAL** | **$100$** |  |

## 8. Referencias

Consejo Universitario, Universidad de Costa Rica. Reglamento de Régimen Académico
Estudiantil (aprobado en sesión 4632-03 del 9 de mayo de 2001; artículo 14 bis reformado, más
recientemente, en sesión 6968-15 del 12 de febrero de 2026). Disponible en:
https://www.cu.ucr.ac.cr/normativa/regimen_academico_estudiantil.pdf

The Open MPI Project. Open MPI Documentation. Disponible en: https://docs.open-mpi.org/

## 9. Rúbrica de evaluación y retroalimentación

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