Example: tdse_project_menu

 Description:
 Bare Metal - Event-Triggered Systems (ETS)
 App - Menu Application
 Project for STM32 Project (STM32CubeIDE Version: 1.7.0)

  SystemCoreClock     => 64MHz (15.625nS)
  SysTick Rate Hertz  => 1000 ticks per second (1mS)

  app.c (app.h)
   Endless loops, which execute tasks with fixed computing time. This 
   sequential execution is only deviated from when an interrupt event occurs.

  task_system.c (task_system.h, task_system_attribute.h) 
   Non-Blocking Code -> System Modeling
  
  task_system_interface.c (task_system_interface.h)
   Non-Blocking Code
   
  task_screen.c (task_screen.h, task_screen_attribute.h) 
   Non-Blocking & Update By Time Code -> Screen Modeling

  task_screen_interface.c (task_screen_interface.h)
   Non-Blocking Code

  task_button.c (task_button.h, task_button_attribute.h) 
   Non-Blocking & Update By Time Code -> Button Modeling

  menu.h (menu.c)
   ADT -> Menu Modeling

  display.h (display.c)
   Utilities for Display strings to LCD Display

  logger.h (logger.c)
   Utilities for Retarget "printf" to Console

  dwt.h
   Utilities for Mesure "clock cycle" and "execution time" of code
   
  Connection requirements:
  	* LCD Display I2C Adapter
	   				   SDA --> Board SDA/D14
	   				   SCL --> Board SCL/D15
	   				   VCC --> Board 5V
	   				   GND --> Board GND
	* Board GND <-- S1 BTN --> Board D2
	* Board GND <-- S2 BTN --> Board D4
	* Board GND <-- S3 BTN --> Board D5
	* Board GND <-- S4 BTN --> Board D6

  Special connection requirements:
   There are no special connection requirements for this example.

Build procedures:
Visit the Getting started with STM32: STM32 step-by-step at 
"https://wiki.st.com/stm32mcu/wiki/STM32StepByStep:Getting_started_with_STM32_:_STM32_step_by_step"
to get started building STM32 Projects.