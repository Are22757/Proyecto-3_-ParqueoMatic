/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ili9341.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define CAR_MIN_Y 120  // Límite inferior para el movimiento de los carros DEJAR 120 para MoveStep de 33
#define CAR_MAX_Y 240  // Límite superior para el movimiento de los carros
#define CAR_START_Y 320 // Comenzar fuera de la pantalla (por debajo de la misma)
#define THRESHOLD_Y 100 // Coordenada de umbral para limpiar el rastro
#define I2C_ADDRESS 0x04 // Dirección del esclavo I2C (ESP32)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
extern uint8_t separacion[];
extern uint8_t rotulo[];
extern uint8_t tope[];
extern uint8_t movCARRO1[];
extern uint8_t movCARRO2[];
extern uint8_t movCARRO3[];
extern uint8_t movCARRO4[];
extern uint8_t cambioROJO[];
extern uint8_t cambioVERDE[];
extern uint8_t cero[];
extern uint8_t uno[];
extern uint8_t dos[];
extern uint8_t tres[];
extern uint8_t cuatro[];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void DisplayNumberOn7Segment(int number);
void sendSensorStatusToI2C(int availableSpaces);
void sendDebugCommand(char command);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void sendSensorStatusToI2C(int availableSpaces) {
    HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDRESS << 1, (uint8_t *)&availableSpaces, 1, HAL_MAX_DELAY);
}
void sendDebugCommand(char command) {
    HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDRESS << 1, (uint8_t *)&command, 1, HAL_MAX_DELAY);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

	LCD_Init();

	LCD_Clear(0x9CD3);

	// Posicionar el sprite en su posición inicial
	// Inicializar posiciones de los carros
	int spriteY[4] = {CAR_START_Y, CAR_START_Y, CAR_START_Y, CAR_START_Y}; // Fuera de la pantalla
	const int spriteHeight = 59; // Altura de los sprites
	const int moveStep = 33; // Cantidad a mover en cada paso


	// Muestra de los gráficos y otros elementos
	LCD_Bitmap(35, 15, 213, 34, rotulo);
	LCD_Bitmap(260, 15, 33, 33, cero);
	LCD_Bitmap(260, 15, 33, 33, uno);
	LCD_Bitmap(260, 15, 33, 33, dos);
	LCD_Bitmap(260, 15, 33, 33, tres);
	LCD_Bitmap(260, 15, 33, 33, cuatro);

	LCD_Bitmap(20, 100, 30, 8, tope);
	LCD_Bitmap(50, 100, 30, 8, tope);

	LCD_Bitmap(95, 100, 30, 8, tope);
	LCD_Bitmap(125, 100, 30, 8, tope);

	LCD_Bitmap(171, 100, 30, 8, tope);
	LCD_Bitmap(201, 100, 30, 8, tope);

	LCD_Bitmap(247, 100, 30, 8, tope);
	LCD_Bitmap(272, 100, 30, 8, tope);

	// Inicializar los sprites de indicativos de parqueo
	LCD_Bitmap(40, 70, 22, 21, cambioVERDE); // Inicialmente en verde
	LCD_Bitmap(116, 70, 22, 21, cambioVERDE); // Inicialmente en verde
	LCD_Bitmap(192, 70, 22, 21, cambioVERDE); // Inicialmente en verde
    LCD_Bitmap(266, 70, 22, 21, cambioVERDE); // Inicialmente en verde

	//FillRect(0, 0, 319, 206, 0x1911);
	FillRect(6, 100, 8, 129, 0xFFF7);
	FillRect(82, 100, 8, 129, 0xFFF7);
	FillRect(158,100, 8, 129, 0xFFF7);
	FillRect(234, 100, 8, 129, 0xFFF7);
	FillRect(305, 100, 8, 129, 0xFFF7);

	void DisplayNumberOn7Segment(int number) {
			       	    // Define los patrones para los números 0 a 4
			       	    const uint8_t segmentPatterns[5] = {
			       	    	0b11010011,  // 4
							0b10110111, // 3
							0b10111101, // 2
			       	        0b00010010, // 1
							0b01111111 // 0



			       	    };

			       	    // Asegúrate de que el número esté en el rango de 0 a 4
			       	    if (number < 0 || number > 4) {
			       	        return; // Salir si el número es inválido
			       	    }

			       	    // Obtener el patrón correspondiente
			       	    uint8_t pattern = segmentPatterns[number];

			       	    // Configurar los pines para encender los segmentos
			       	    HAL_GPIO_WritePin(DISPLAY_g_GPIO_Port, DISPLAY_g_Pin, (pattern & 0b10000000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
			       	    HAL_GPIO_WritePin(DISPLAY_f_GPIO_Port, DISPLAY_f_Pin, (pattern & 0b01000000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
			       	    HAL_GPIO_WritePin(DISPLAY_a_GPIO_Port, DISPLAY_a_Pin, (pattern & 0b00100000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
			       	    HAL_GPIO_WritePin(DISPLAY_b_GPIO_Port, DISPLAY_b_Pin, (pattern & 0b00010000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
			       	    HAL_GPIO_WritePin(DISPLAY_e_GPIO_Port, DISPLAY_e_Pin, (pattern & 0b00001000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
			       	    HAL_GPIO_WritePin(DISPLAY_d_GPIO_Port, DISPLAY_d_Pin, (pattern & 0b00000100) ? GPIO_PIN_SET : GPIO_PIN_RESET);
			       	    HAL_GPIO_WritePin(DISPLAY_c_GPIO_Port, DISPLAY_c_Pin, (pattern & 0b00000010) ? GPIO_PIN_SET : GPIO_PIN_RESET);
			       	}

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {


	// Leer las entradas de los sensores
		    uint8_t sensor1 = HAL_GPIO_ReadPin(SENS_IN1_GPIO_Port, SENS_IN1_Pin); // SENS_IN1
		    uint8_t sensor2 = HAL_GPIO_ReadPin(SENS_IN2_GPIO_Port, SENS_IN2_Pin); // SENS_IN2
		    uint8_t sensor3 = HAL_GPIO_ReadPin(SENS_IN3_GPIO_Port, SENS_IN3_Pin); // SENS_IN3
		    uint8_t sensor4 = HAL_GPIO_ReadPin(SENS_IN4_GPIO_Port, SENS_IN4_Pin); // SENS_IN4


    // Actualizar los sprites de los indicativos de parqueo según el estado de los sensores
	        if (sensor1 == 0) {
	            LCD_Bitmap(40, 70, 22, 21, cambioROJO); // Cambiar a rojo si el sensor detecta un carro
	            sendDebugCommand('S'); // Enviar 'S' si sensor1 está activo
	        } else {
	            LCD_Bitmap(40, 70, 22, 21, cambioVERDE); // Cambiar a verde si el sensor no detecta un carro
	            sendDebugCommand('T');// Enviar 'T' si sensor1 está inactivo
	        }

	        if (sensor2 == 0) {
	            LCD_Bitmap(116, 70, 22, 21, cambioROJO); // Cambiar a rojo si el sensor detecta un carro
	            sendDebugCommand('U');// Enviar 'U' si sensor2 está activo
	        } else {
	            LCD_Bitmap(116, 70, 22, 21, cambioVERDE); // Cambiar a verde si el sensor no detecta un carro
	            sendDebugCommand('V');// Enviar 'V' si sensor2 está inactivo
	        }

	        if (sensor3 == 0) {
	            LCD_Bitmap(192, 70, 22, 21, cambioROJO); // Cambiar a rojo si el sensor detecta un carro
	            sendDebugCommand('W'); // Enviar 'W' si sensor3 está activo
	        } else {
	            LCD_Bitmap(192, 70, 22, 21, cambioVERDE); // Cambiar a verde si el sensor no detecta un carro
	            sendDebugCommand('X'); // Enviar 'X' si sensor3 está inactivo
	        }

	        if (sensor4 == 0) {
	            LCD_Bitmap(266, 70, 22, 21, cambioROJO); // Cambiar a rojo si el sensor detecta un carro
	            sendDebugCommand('Y'); // Enviar 'Y' si sensor4 está activo
	        } else {
	            LCD_Bitmap(266, 70, 22, 21, cambioVERDE); // Cambiar a verde si el sensor no detecta un carro
	            sendDebugCommand('Z'); // Enviar 'Z' si sensor4 está inactivo
	        }

	    //Generar cambio de número según la disponibilidad del parqueo
	        // Variables para almacenar el contador anterior
	        int previousCounter = -1; // Inicializar en un valor que no sea válido

	        // Contador de sensores activos
	        int activeSensors = sensor1 + sensor2 + sensor3 + sensor4;

	        // Calcular el contador actual basado en los sensores activos
	        int currentCounter = 4 - activeSensors; // Restar el número de sensores activos de 4

	        // Solo actualizar si el contador ha cambiado
	        if (currentCounter != previousCounter) {
	            // Limpiar el área donde estaba el número antes de dibujarlo
	           // FillRect(260, 15, 33, 33, 0x9CD3); // Limpiar área del número

	            // Mostrar el número correspondiente
	            switch (currentCounter) {
	                case 4:
	                    LCD_Bitmap(260, 15, 33, 33, cero);
	                    break;
	                case 3:
	                    LCD_Bitmap(260, 15, 33, 33, uno);
	                    break;
	                case 2:
	                    LCD_Bitmap(260, 15, 33, 33, dos);
	                    break;
	                case 1:
	                    LCD_Bitmap(260, 15, 33, 33, tres);
	                    break;
	                case 0:
	                    LCD_Bitmap(260, 15, 33, 33, cuatro);
	                    break;
	            }
	            // Enviar el estado de los espacios disponibles al ESP32
	               sendSensorStatusToI2C(currentCounter);
	            // Mostrar el número en el display de 7 segmentos
	               DisplayNumberOn7Segment(currentCounter);
	            // Actualizar el contador anterior
	            previousCounter = currentCounter;
	        }

	        	// Variables para almacenar la posición anterior de los sprites
		       int previousY[4]; // No inicializar aquí, se llenará después

		       // Mover sprites basados en las señales de los sensores
		       for (int i = 0; i < 4; i++) {
		           previousY[i] = spriteY[i]; // Almacenar la posición anterior

		           if (i == 0 && sensor1) {
		               spriteY[i] += moveStep; // Mover carro 1 hacia abajo si hay señal
		               if (spriteY[i] > CAR_MAX_Y) spriteY[i] = CAR_MAX_Y; // Limitar el movimiento
		           } else if (i == 0) {
		               if (spriteY[i] > CAR_MIN_Y) spriteY[i] -= moveStep; // Mover carro 1 hacia arriba si no hay señal
		           }

		           if (i == 1 && sensor2) {
		               spriteY[i] += moveStep; // Mover carro 2 hacia abajo si hay señal
		               if (spriteY[i] > CAR_MAX_Y) spriteY[i] = CAR_MAX_Y; // Limitar el movimiento
		           } else if (i == 1) {
		               if (spriteY[i] > CAR_MIN_Y) spriteY[i] -= moveStep; // Mover carro 2 hacia arriba si no hay señal
		           }

		           if (i == 2 && sensor3) {
		               spriteY[i] += moveStep; // Mover carro 3 hacia abajo si hay señal
		               if (spriteY[i] > CAR_MAX_Y) spriteY[i] = CAR_MAX_Y; // Limitar el movimiento
		           } else if (i == 2) {
		               if (spriteY[i] > CAR_MIN_Y) spriteY[i] -= moveStep; // Mover carro 3 hacia arriba si no hay señal
		           }

		           if (i == 3 && sensor4) {
		               spriteY[i] += moveStep; // Mover carro 4 hacia abajo si hay señal
		               if (spriteY[i] > CAR_MAX_Y) spriteY[i] = CAR_MAX_Y; // Limitar el movimiento
		           } else if (i == 3) {
		               if (spriteY[i] > CAR_MIN_Y) spriteY[i] -= moveStep; // Mover carro 4 hacia arriba si no hay señal
		           }
		       }

		       // Limpiar el área donde estaba el sprite antes de dibujarlo en la nueva posición
		       unsigned int backgroundColor = 0x9CD3; // Color de fondo que usaste para limpiar la pantalla
		       for (int i = 0; i < 4; i++) {
		           // Solo limpiar si la posición anterior es diferente de la nueva
		           if (previousY[i] != spriteY[i]) {
		               FillRect(26 + (i * 76), previousY[i], 49, spriteHeight, backgroundColor); // Limpiar cada carro
		           }
		       }

		       // Dibuja los carros en sus nuevas posiciones
		       for (int i = 0; i < 4; i++) {
		           if (previousY[i] != spriteY[i]) { // Solo dibujar si la posición ha cambiado
		               int anima1 = (spriteY[i] / 2) % 2; // Cambiar animación basado en la posición
		               LCD_Sprite(26 + (i * 76), spriteY [i], 49, 59, (i == 0) ? movCARRO1 : (i == 1) ? movCARRO2 : (i == 2) ? movCARRO3 : movCARRO4, 2, anima1, 0, 1); // Dibujar cada carro
		           }
		       }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_ENABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LCD_RST_Pin|LCD_D1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_RD_Pin|LCD_WR_Pin|LCD_RS_Pin|LCD_D7_Pin
                          |LCD_D0_Pin|LCD_D2_Pin|DISPLAY_g_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin|DISPLAY_b_Pin|DISPLAY_a_Pin|LCD_D6_Pin
                          |DISPLAY_f_Pin|DISPLAY_c_Pin|DISPLAY_d_Pin|DISPLAY_e_Pin
                          |LCD_D3_Pin|LCD_D5_Pin|LCD_D4_Pin|SD_SS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LCD_RST_Pin LCD_D1_Pin */
  GPIO_InitStruct.Pin = LCD_RST_Pin|LCD_D1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RD_Pin LCD_WR_Pin LCD_RS_Pin LCD_D7_Pin
                           LCD_D0_Pin LCD_D2_Pin */
  GPIO_InitStruct.Pin = LCD_RD_Pin|LCD_WR_Pin|LCD_RS_Pin|LCD_D7_Pin
                          |LCD_D0_Pin|LCD_D2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SENS_IN3_Pin SENS_IN2_Pin SENS_IN1_Pin */
  GPIO_InitStruct.Pin = SENS_IN3_Pin|SENS_IN2_Pin|SENS_IN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_CS_Pin LCD_D6_Pin LCD_D3_Pin LCD_D5_Pin
                           LCD_D4_Pin SD_SS_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin|LCD_D6_Pin|LCD_D3_Pin|LCD_D5_Pin
                          |LCD_D4_Pin|SD_SS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : DISPLAY_b_Pin DISPLAY_a_Pin DISPLAY_f_Pin DISPLAY_c_Pin
                           DISPLAY_d_Pin DISPLAY_e_Pin */
  GPIO_InitStruct.Pin = DISPLAY_b_Pin|DISPLAY_a_Pin|DISPLAY_f_Pin|DISPLAY_c_Pin
                          |DISPLAY_d_Pin|DISPLAY_e_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : DISPLAY_g_Pin */
  GPIO_InitStruct.Pin = DISPLAY_g_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DISPLAY_g_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SENS_IN4_Pin */
  GPIO_InitStruct.Pin = SENS_IN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SENS_IN4_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
