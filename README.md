# Urbanite Project

## Autores

* **Mateo Pansard** - email: [mateo.pansard@alumnos.upm.es](mailto:mateo.pansard@alumnos.upm.es)
* **Lucia Petit** - email: [lucia.petit@alumnos.upm.es](mailto:lucia.petit@alumnos.upm.es)

Nuestro sistema base simula el sensor de aparcamiento trasero de un coche. Al presionar el **botón azul**, se activa el sistema como si el coche pusiera la **marcha atrás**. En ese momento, el sistema central envía un pulso al **sensor ultrasónico HC-SR04** para medir la distancia hasta un obstáculo.

El resultado se muestra en el **LED RGB de la placa Nucleo-STM32**, que cambia de color según la proximidad. Para evitar distracciones, el usuario puede **pausar/reanudar la iluminación** con una pulsación corta del botón, excepto en caso de colisión inminente.

El sistema tiene cuatro módulos principales:

    1. Sistema central: Placa Nucleo-STM32 con el microcontrolador STM32F446RE, que gestiona el encendido y la medición.
    2. Control básico: Incluye el botón de usuario y un LED para indicar acciones.
    3. Medición de distancia: Sensor HC-SR04 que emite y recibe ondas ultrasónicas.
    4. Módulo de actuación: LED RGB que indica la distancia al obstáculo.

Para apagar el sistema, se mantiene pulsado el botón durante un tiempo largo. Se muestra un **log en modo depuración**, y hay un video demostrativo disponible.

Our base system simulates a **rear parking sensor** in a car. When the blue button is pressed, the system activates as if the car were shifting into **reverse gear**. At that moment, the central system sends a pulse to the **HC-SR04 ultrasonic sensor** to measure the distance to an obstacle.

The result is displayed on the **RGB LED of the Nucleo-STM32 board**, which changes color based on proximity. To prevent distractions, the user can **pause/resume the LED display** with a short button press, except when a collision is imminent.

The system consists of four main modules:

    1. Central system: Nucleo-STM32 board with the STM32F446RE microcontroller, managing power and measurement.
    2. Basic control: Includes the user button and an LED for feedback.
    3. Distance measurement: HC-SR04 sensor that emits and detects ultrasonic waves.
    4. Actuation module: RGB LED that indicates the obstacle’s distance.

To turn off the system, the button must be held down for a long press. A **log message appears in debug mode**, and a demonstration video is available.

Puede añadir una imagen de portada **de su propiedad** aquí. Por ejemplo, del montaje final, o una captura de osciloscopio, etc.

**Las imágenes se deben guardar en la carpeta `docs/assets/imgs/` y se pueden incluir en el documento de la siguiente manera:**

```markdown
![Texto alternativo](docs/assets/imgs/imagen.png)
```

NOTA: **NO** añada el código ```markdown``` en el fichero `README.md` de su proyecto, sino lo de dentro. Este código es un para mostrar de forma literal cómo se puede añadir una imagen al fichero `README.md`.

**Añada un enlace a un vídeo público de su propiedad aquí con la demostración del proyecto explicando lo que haya hecho en la versión V5.**

Para añadir un enlace a un vídeo de Youtube, puede usar el siguiente código:

```markdown
[![Texto alternativo](docs/assets/imgs/imagen.png)](https://youtu.be/ID_DEL_VIDEO "Texto al pasar el ratón por encima de la imagen.")
```

NOTA: **NO** añada el código ```markdown``` sino lo de dentro. Este código es un para mostrar de forma literal cómo se puede añadir un enlace a un vídeo de Youtube al fichero `README.md`.

## Version 1

In Version 1, the system works with the user button only. The user button is connected to the pin PC13. The code uses the EXTI13 interrupt to detect the button press.

| Parameter | Value | 
| --------- | --------- | 
| Pin  | PC13  | 
| Mode  | Input   |
| Pull up/ down	  | No push no pull  |
| EXTI | EXTI13  |
| ISR | EXTI15_10_IRQHandler  |
| Priority | 1  |
| Subpriority | 0  |
| Debounce time | 100-200 ms  |

This is the FSM of the button: ![FSM del botón](docs/assets/imgs/fsm_button.png)

## Version 2

In Version 2, the system adds the ultrasonic transceiver to measure the distance to an object. The trigger pin is connected to the pin PB0, and the echo pin is connected to the pin PA1. The code uses the TIM2, TIM3 and TIM5 timers to control the ultrasonic transceiver.

To measure the distance in centimeters with a timer resolution of 1 microseconds, we can say that 1 cm is equivalent to 58.3 microseconds. The speed of sound is 343 m/s at 20ºC. The ultrasonic transceiver is the HC-SR04.

The characteristics and connections of the ultrasonic transceiver HC-SR04 are shown in the table below:

| Parameter | Value | 
| --------- | --------- | 
| Power supply| 5 V | 
| Current | 15 mA  |
| Pull up/ down	  | No pull   |
| Angle of aperture | 15º |
| Frequency | 40 kHz |
| Measurement range | 2 cm to 400 cm |
| Pins | PB0 (Trigger) and PA1 (Echo)  |
| Mode | Output (Trigger) and alternative (Echo)  |
Timer	|TIM3 (Trigger) and TIM2 (Echo) |

The system uses 3 timers: (i) one to control the duration of the trigger signal, (ii) another to measure the echo time, and (iii) another to measure the timeout between consecutive measurements that we want to make. The timing diagram of the measurements is shown in the figure below.

To generate the **trigger signal**, we will activate (1 logical) the trigger pin for at least 10 microseconds. To control the duration of this signal, we will configure the timer TIM3 as shown in the table below.


|Parameter	|Value|
| --------- | --------- | 
|Timer	|TIM3|
|ISR|	TIM3_IRQHandler()|
|Priority	|4|
|Subpriority|	0|

To **measure the echo time**, we will configure the timer TIM2 in **input capture mode**, which will capture the value of the counter at the moment the echo signal is activated and deactivated.

|Parameter|	Value|
| --------- | --------- | 
|Timer|	TIM2|
|ISR	|TIM2_IRQHandler()|
|Priority	|3|
|Subpriority	|0|

The timer that controls the **timeout between consecutive measurements** is TIM5. The characteristics of this timer are shown in the table below. The FSM will give a value every PORT_PARKING_SENSOR_TIMEOUT_MS milliseconds.

| Parameter| 	Value| 
| --------- | --------- | 
| Timer	| TIM5| 
| ISR| 	TIM5_IRQHandler()| 
| Priority| 	5| 
| Subpriority| 	0| 

This is the FSM of the ultrasonic transceiver:
This is the FSM of the button: ![FSM del ultrasonic](docs/assets/imgs/fsm_ultrasound.png)


* Para poner un texto en negrita se usa el símbolo `**` de manera consecutiva. Por ejemplo: **Texto en negrita**
* Para poner un texto en cursiva se usa el símbolo `*` de manera consecutiva. Por ejemplo: *Texto en cursiva*
* Para poner un texto en cursiva y negrita se usa el símbolo `***` de manera consecutiva. Por ejemplo: ***Texto en cursiva y negrita***

Para añadir subsecciones se usa el símbolo `#` de manera consecutiva. Por ejemplo:

### Subsección 1

Breve descripción de la subsección 1.

Para añadir una lista de elementos se usa el símbolo `-` de manera consecutiva. Por ejemplo:

* Elemento 1
* Elemento 2
* Elemento 3

Para añadir una lista de elementos numerados se usa el símbolo `1.` de manera consecutiva. Por ejemplo:

1. Elemento 1
2. Elemento 2
3. Elemento 3

Para añadir un enlace a una página web se usa el siguiente código:

```markdown
Enlace a [Google](https://www.google.com).
```

NOTA: **NO** añada el código ```markdown``` sino lo de dentro. Este código es un para mostrar de forma literal cómo se puede añadir un enlace a una página web al fichero `README.md`.

Puede añadir tablas de la siguiente manera:

| Columna 1 | Columna 2 | Columna 3 |
| --------- | --------- | --------- |
| Valor 1   | Valor 2   | Valor 3   |
| Valor 4   | Valor 5   | Valor 6   |

Para añadir un enlace a un fichero `.c` o `.h` puede usar el siguiente código. Se trata de enlaces a ficheros `.html` que se generan automáticamente con la documentación del código al ejecutar Doxygen y que se encuentran en la carpeta `docs/html/`.

```markdown
Enlace a la [FSM de Version 1](fsm__button_8c.html).
```

NOTA: **NO** añada el código ```markdown``` sino lo de dentro. Este código es un para mostrar de forma literal cómo se puede añadir un enlace a un fichero `.c` o `.h` al fichero `README.md`.


## Version 3

Breve descripción de la versión 3.

## Version 4

Breve descripción de la versión 4.

## Version 5

Breve descripción de la versión 5.
