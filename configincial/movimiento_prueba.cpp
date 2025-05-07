#include <iostream> //Entrada y salida estándar
#include <cmath> //Funciones matemáticas estándar
#include <vector> //Contenedor dinámico tipo vector
#include <map> //Contenedor tipo mapa
#include <iomanip> // para std::fixed y std::setprecision; manipulacion de salida formateada


// GLEW
#include <GL/glew.h> //Extensiones OpenGL. Necesario para manejar funciones modernas de OpenGL

// GLFW
#include <GLFW/glfw3.h> //Librería para crear vebtanas y gestionar inputs del usuario

// Other Libs
#include "stb_image.h" //Cargar imágenes (usualmente texturas)

// GLM Mathematics
#include <glm/glm.hpp> //Librería matemática orientada a gráficos
#include <glm/gtc/matrix_transform.hpp> //Para generar matrices de transformacion
#include <glm/gtc/type_ptr.hpp> //Para convertir matrices a punteros

//Load Models
#include "SOIL2/SOIL2.h" //Librería para carga de texturas (similar a stb_image)


// Other includes -- Inclusion de clases propias del proyecto
#include "Shader.h" //Clase para manejar shaders
#include "Camera.h" //Clase para manejar cámara
#include "Model.h" //Clase para manejar modelos 3D
#include "Character.h" //Clase que representa un personaje
#include "Board.h" //Clase que representa el tablero (Ajedrez)


// Function prototypes -- Prototipos de funciones de callbacks y lógica del juego
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
glm::vec3 ScreenToWorld(double xpos, double ypos, float planeY); //Convierte coordenadas de pantalla a mundo
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods); 
void InitMinecraftCharacters(); //Inicializa personajes estilo MineCraft
void InitPowerRangersCharacters(); //Inicializa personajes estilo Power Rangers
void DoMovement(); //Logica para mover personajes o camara
void Animation(); //Logica para animaciones

// Window dimensions -- Dimensiones de ventana
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT; //Variables para guardar dimensiones reales

// Camera
Camera  camera(glm::vec3(0.0f, 3.0f, 5.0f)); //Inicializa camara en cierta posicion
GLfloat lastX = WIDTH / 2.0; //Ultima posicion del mouse en X
GLfloat lastY = HEIGHT / 2.0; //Ultima posicion del mouse en Y
bool keys[1024]; // Arreglo para gestionar teclas presionadas
bool firstMouse = true; //Detecta el primer movimiento del mouse
// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f); //Posicion de la luz principal
glm::vec3 spotlightPos(0.0f); //Posicion del spotlight (linterna, foco)
glm::vec3 lightDir(0.0f); //Direccion del spotlight 
bool pick_place = true; //Alterna entre modo "recoger" y "colocar"
bool active; // true->pick, false->place -- Indica si algo esta activo (seleccion)
//Posiciones para mover sportlight
double xpos, ypos;
// Coordenadas para mover personaje
float originX, originZ, destinationX, destinationZ = 999.0f; //999 puede ser un valor "no válido"
//Variables para color de spotlight
glm::vec3 ambientSL(0.5f); //Componente ambiental
glm::vec3 diffuseSL(1.0f); //Componenete difusa
glm::vec3 specularSL(1.0f); // Componenete especular

//Matrices de proyeccion y vista de la camara
glm::mat4 projection;
glm::mat4 view;
// Variable para almacenar el último punto clickeado
glm::vec3 Light1 = glm::vec3(0);
//Instancia del tablero
Board board;


// Deltatime -- Variables de tiempo para animaciones o movimientos suaves
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

//======================Constantes para definir las piezas y eqipos=======================
const unsigned int Torre = 1;
const unsigned int Caballo = 2;
const unsigned int Alfil = 3;
const unsigned int Reyna = 4;
const unsigned int Rey = 5;
const unsigned int Peon = 6;
const unsigned int minecraft = 1;
const unsigned int powerRangers = 2;
//========================================================================================


//=====================Variables que deinen la posicion de los modelos=====================
// Variables para la animación de stev

std::vector<Character> minecraftCharacters; //Vector para almacenar los personajes del equipo Minecraft
Character* selectedCharacter = nullptr; // El personaje actualmente agarrado

//================================Posiciones iniciales y variables de animacion para cada personaje de Minecraft======================================================
glm::vec3 stevPos(-0.3f, 0.0f, -2.1f); //Posicion de Steve
bool animStev = false; //Indica si Steve esta en animacion
float avanceStev = 0.0f; // Cuánto ha avanzado desde que se activó
const float limiteAvance = 0.6f; // límite de movimiento

glm::vec3 creepPos(2.1f, 0.0f, -2.1f); //Posicion de Creepper
bool animCreep = false; //Indica si Creeper esta en animacion
float avanceCreep = 0.0f; //Cuánto ha avanzado desde que se activó

glm::vec3 enderPos(0.9f, 0.0f, -2.1f); //Posicion de Enderman
bool animEnder = false; //Indica si Enderman esta en animacion
float avanceEnder = 0.0f; //Cuánto ha avanzado desde que se activó

glm::vec3 snowPos(1.5f, 0.0f, -2.1f); //Posicion de Snowman
bool animSnow = false; //Indica si Snowman esta en animacion
float avanceSnow = 0.0f; //Cuánto ha avanzado desde que se activó

glm::vec3 alexPos(0.3f, -0.4f, -2.1f); //Posicion de Alex
bool animAlex = false; //Indica si Alex esta en animacion
float avanceAlex = 0.0f; //Cuánto ha avanzado desde que se activó
//===Sigue la misma logica que Creepper===
glm::vec3 creep2Pos(-2.1f, 0.0f, -2.1f);
bool animCreep2 = false;
float avanceCreep2 = 0.0f;
//===Sigue la misma logica que Snowman===
glm::vec3 snow2Pos(-1.5f, 0.0f, -2.1f);
bool animSnow2 = false;
float avanceSnow2 = 0.0f;
//===Sigue la misma logica que Enderman===
glm::vec3 ender2Pos(-0.9f, 0.0f, -2.1f);
bool animEnder2 = false;
float avanceEnder2 = 0.0f;


//Vector que define y almacena la posicion de los peones zombie
std::vector<glm::vec3> zomPositions = {
	glm::vec3(-2.1f, 0.0f, -1.5f),
	glm::vec3(-1.5f, 0.0f, -1.5f),
	glm::vec3(-0.9f, 0.0f, -1.5f),
	glm::vec3(-0.3f, 0.0f, -1.5f),
	glm::vec3(0.3f, 0.0f, -1.5f),
	glm::vec3(0.9f, 0.0f, -1.5f),
	glm::vec3(1.5f, 0.0f, -1.5f),
	glm::vec3(2.1f, 0.0f, -1.5f)
};
//Estados de animacion y avance de cada peon zombie
std::vector<bool> animZoms(8, false);
std::vector<float> avanceZoms(8, 0.0f);

//MM Power Rangers==============Variables para personajes de Power Rangers============================================

std::vector<Character> powerRangersCharacters; // Vector para almacenar los personajes del equipo Power Rangers
Character* selectedPowerRangersCharacter = nullptr; // Personaje seleccionado actual


//Posiciones y animaciones individuales
glm::vec3 lordzPos(0.3f, -0.4f, 2.1f); //Posicion de Lord Zedd
bool animlordz = false; //Indica si Lord Zedd esta en animacion
float avancelordz = 0.0f;
const float powerRangersLimit = 0.6f; //Limite de movimiento para los Power Rangers

glm::vec3 megazordPos(-0.0f, -0.4f, 2.1f); //Posicion de MegaZord
bool animmegazord = false; //Indica si MegaZord esta en animacion
float avancemegazord = 0.0f;
// Posiciones y estados de animación para las esfinges
std::vector<glm::vec3> esfingePositions = {
	{-0.9f, -0.4f, 2.1f},
	{0.9f, -0.4f, 2.1f}
};
std::vector<bool> animesfinge(2, false);
std::vector<float> avanceesfinge(2, 0.0f);
// Posiciones y estados para los dragones
std::vector<glm::vec3> dragonPositions = {
	{-1.5f, -0.4f, 2.1f},
	{1.5f, -0.4f, 2.1f}
};
std::vector<bool> animdragon(2, false);
std::vector<float> avancedragon(2, 0.0f);
// Posiciones y animaciones para los personajes "Zack"
std::vector<glm::vec3> zackPositions = {
	{-3.7f, 0.2f, 1.5f},
	{0.4f, 0.2f, 1.5f}
};
std::vector<bool> animzack(2, false);
std::vector<float> avancezack(2, 0.0f);
// Posiciones y animaciones para los "patrulleros"
std::vector<glm::vec3> patrulleroPositions = {
	{-0.7f, -0.4f, 1.5f},
	{-0.1f, -0.4f, 1.5f},
	{0.5f,  -0.4f, 1.5f},
	{1.1f,  -0.4f, 1.5f},
	{1.7f,  -0.4f, 1.5f},
	{2.3f, -0.4f, 1.5f},
	{2.9f,  -0.4f, 1.5f},
	{3.5f,  -0.4f, 1.5f}
};
std::vector<bool> animpatrullero(8, false);
std::vector<float> avancepatrullero(8, 0.0f);

//=========================================================================================

int main()
{
	// Inicializa GLFW (librería para manejo de ventanas y entrada)
	glfwInit();

	// Configuraciones opcionales para OpenGL (comentadas aquí)
	/*
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Versión mayor de OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Versión menor de OpenGL
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Perfil core
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Compatibilidad hacia adelante (MacOS)
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // Ventana no redimensionable
	*/

	// Crea la ventana principal con el título "Fuentes de luz"
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Fuentes de luz", nullptr, nullptr);

	// Verifica si la ventana fue creada exitosamente
	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate(); // Finaliza GLFW si hay error
		return EXIT_FAILURE;
	}

	// Hace que el contexto de OpenGL use la ventana recién creada
	glfwMakeContextCurrent(window);

	// Obtiene el tamaño real del framebuffer (para soporte de pantallas HiDPI)
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Define funciones de callback para teclado y mouse
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);

	// Configuración opcional para capturar el cursor del mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Configura GLEW para usar un método moderno de carga de funciones
	glewExperimental = GL_TRUE;

	// Inicializa GLEW para manejar funciones de OpenGL
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define las dimensiones del viewport (área de dibujo)
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Carga los shaders para iluminación y lámpara
	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

	// ============================= Carga de modelos 3D =============================
	Model Dog((char*)"Models/RedDog.obj");
	Model creep((char*)"Models/creep.obj");
	Model ender((char*)"Models/ender.obj");
	Model zom((char*)"Models/zom.obj");
	Model stev((char*)"Models/stev.obj");
	Model snow((char*)"Models/snow.obj");
	Model alex((char*)"Models/alex.obj");
	Model lordz((char*)"Models/Zedd_S01_T1_lordZedd_msh.obj");
	Model megazord((char*)"Models/POWERRANGER.obj");
	Model esfinge((char*)"Models/Gldr_S01_T1_king_msh.obj");
	Model zack((char*)"Models/RngM_S01_T1_black_msh.obj");
	Model dragon((char*)"Models/MMPR - Dragonzord.obj");
	Model patrullero((char*)"Models/putty_low.obj");
	Model Piso((char*)"Models/tablero.obj");
	// ==============================================================================

	// Inicializa los personajes de Minecraft y los coloca en el tablero
	InitMinecraftCharacters();
	board.initMinecraftBoard(minecraftCharacters);

	// Inicializa los personajes de Power Rangers y los coloca en el tablero
	InitPowerRangersCharacters();
	board.initPowerRangersBoard(powerRangersCharacters);

	// Configuración de VAO y VBO para posiciones y normales
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Atributo de posición (primeros 3 floats)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Atributo de normales (siguientes 3 floats)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Configura las unidades de textura para el shader de iluminación
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

	// Configura la proyección de la cámara en perspectiva
	projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Bucle principal del juego/renderizado
	while (!glfwWindowShouldClose(window))
	{
		// Calcula el tiempo transcurrido entre frames
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Procesa eventos de entrada (teclado, mouse)
		glfwPollEvents();
		DoMovement();   // Movimiento de la cámara
		Animation();    // Actualización de animaciones

		// Limpia el framebuffer con un color gris oscuro
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Habilita el test de profundidad para evitar dibujar objetos "detrás"
		glEnable(GL_DEPTH_TEST);

		// Usa el shader de iluminación
		lightingShader.Use();

		// Configura el mapa difuso
		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);

		// Envía la posición de la cámara al shader
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		// Luz direccional (como la del sol)
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), 0.2f, 1.0f, 0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.5f, 0.5f, 0.5f); // Luz ambiental tenue
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.0f, 0.0f, 0.0f);   // Sin luz difusa
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.0f, 0.0f, 0.0f); // Sin reflejos

		// Obtiene posición del cursor en la pantalla y la convierte al mundo 3D
		glfwGetCursorPos(window, &xpos, &ypos);
		spotlightPos = ScreenToWorld(xpos, ypos, 0.0f);
		lightPos = glm::vec3(spotlightPos.x, spotlightPos.y + 3.0f, spotlightPos.z); // Elevación de la luz
		lightDir = glm::normalize(spotlightPos - lightPos); // Dirección hacia donde apunta el spotlight

		// Configura spotlight (linterna)
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), lightDir.x, lightDir.y, lightDir.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), ambientSL.x, ambientSL.y, ambientSL.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), diffuseSL.x, diffuseSL.y, diffuseSL.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), specularSL.x, specularSL.y, specularSL.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(4.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(6.0f)));

		// Configura propiedades del material (brillo especular)
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);

		// Calcula la vista de la cámara
		view = camera.GetViewMatrix();

		// Obtiene las ubicaciones de las matrices en el shader
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Envía las matrices al shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Inicializa la matriz del modelo (transformaciones)
		glm::mat4 model(1);

		// Vuelve a obtener la vista por si fue modificada
		view = camera.GetViewMatrix();
		
		//================= ===========Dibujado de personajes========================
                //Nota: .Draw dibuja o manda a llamar a los modelos de cada personaje
		//Nota2: translate para poner al personaje en su orientación
		//Nota3: rotate para ayudar a colocar al personaje de forma correcta
		//Nota4: scale ayuda a que algunos personajes mantengan un tamaño parecido entre todos
		//=============================Dibujado del tablero=================================
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));                    //  mueve hacia abajo
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));   //  rota sobre X
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));                         //  escala plano
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Piso.Draw(lightingShader);
		//=================================================================================

		//===============================Dibujado del Steve==================================
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::translate(model, stevPos); // Aplica la transformación de posición
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		stev.Draw(lightingShader);
		//=================================================================================

		//===============================Dibujado del Snowman 1==================================
		model = glm::mat4(1);
		model = glm::translate(model, snowPos);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		snow.Draw(lightingShader);
		//=======================================================================================

		//===============================Dibujado de Zombies==================================
		for (size_t i = 0; i < zomPositions.size(); i++) {
			glm::mat4 model = glm::mat4(1);
			model = glm::translate(model, zomPositions[i]);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			zom.Draw(lightingShader);
		}
		//=====================================================================================

		//===============================Dibujado del Enderman 1=================================
		model = glm::mat4(1);
		model = glm::translate(model, enderPos);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ender.Draw(lightingShader);
		//=======================================================================================

		//===============================Dibujado del Creeper 1=================================
		model = glm::mat4(1);
		model = glm::translate(model, creepPos);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		creep.Draw(lightingShader);
		//======================================================================================

		//===============================Dibujado del Alex=================================
		model = glm::mat4(1);
		model = glm::translate(model, alexPos);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		alex.Draw(lightingShader);
		//=================================================================================

		//===============================Dibujado del Creeper 2=================================
		model = glm::mat4(1);
		model = glm::translate(model, creep2Pos);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		creep.Draw(lightingShader);
		//======================================================================================

		//===============================Dibujado del Snowman 2==================================
		model = glm::mat4(1);
		model = glm::translate(model, snow2Pos);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		snow.Draw(lightingShader);
		//=======================================================================================

		//===============================Dibujado del Enderman 2=================================
		model = glm::mat4(1);
		model = glm::translate(model, ender2Pos);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ender.Draw(lightingShader);
		//=======================================================================================

		////===============================Dibujado del Lord Z=================================
		model = glm::mat4(1); //Rey
		model = glm::translate(glm::mat4(1.0f), lordzPos);
		model = glm::scale(model, glm::vec3(1.195f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.f)); //Rotacion180°
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		lordz.Draw(lightingShader);
		////===================================================================================

		////===============================Dibujado del Megazord=================================
		model = glm::translate(glm::mat4(1.0f), megazordPos); //Reyna
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Rotacion180°
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		megazord.Draw(lightingShader);
		////=====================================================================================

		////===============================Dibujado del Esfinge=================================      
		for (auto& pos : esfingePositions) { // Alfiles
			model = glm::translate(glm::mat4(1.0f), pos);
			model = glm::scale(model, glm::vec3(1.3f));
			model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f)); //Rotacion180°
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			esfinge.Draw(lightingShader);
		}
		////====================================================================================

		////===============================Dibujado del Dragon================================= 
		for (auto& pos : dragonPositions) {// Caballos
			model = glm::translate(glm::mat4(1.0f), pos);
			model = glm::scale(model, glm::vec3(1.3f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Rotacion180°
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			dragon.Draw(lightingShader);
		}
		////===================================================================================

		////===============================Dibujado de Zack=================================
		for (auto& pos : zackPositions) { // Torres
			model = glm::translate(glm::mat4(1.0f), pos);
			model = glm::scale(model, glm::vec3(0.85f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Rotacion180°
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			zack.Draw(lightingShader);
		}
		////================================================================================

		////===============================Dibujado de Patrullero=================================
		for (auto& pos : patrulleroPositions) {
			model = glm::translate(glm::mat4(1.0f), pos);
			model = glm::scale(model, glm::vec3(1.1f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.f)); //Rotacion180°
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			patrullero.Draw(lightingShader);
		}
		////======================================================================================
                // Desvincula el VAO actual para evitar modificarlo accidentalmente
		glBindVertexArray(0);

                 // Dibuja la lámpara (o fuente de luz) con su propio shader
		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
                // Obtiene las ubicaciones de las matrices 'model', 'view' y 'projection' en el shader de la lámpara
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");
		glBindVertexArray(0); //Nuevamente desvincula el VAO por seguridad



		// Swap the screen buffers -- Intercambia los buffers de pantalla (doble buffer) para mostrar la imagen renderizada
		glfwSwapBuffers(window);
	}

         //Termina GLFW y libera los recursos
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();



	return 0;
}
// Procesa el movimiento de la cámara según teclas presionadas
// Moves/alters the camera positions based on user input
void DoMovement()
{
         //Mover hacia adelante
	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}
        //Mover hacia atrás
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}
        //Rotar a la izquierda
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(CIRC_LEFT, deltaTime);


	}
        //Rotar a la derecha
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(CIRC_RIGHT, deltaTime);


	}

}
// Controla las animaciones de movimiento de personajes
void Animation() {
	float paso = 0.01f; // Paso de avance para los personajes Minecraft
	float powerRangersPaso = 0.01f; // Paso para personajes Power Rangers

	// Animaciones individuales para cada personaje
	// Se incrementa la posición en Z mientras no se alcance el límite
	// Una vez alcanzado, se detiene la animación
	
        // Steve avanza hacia adelante
	if (animStev && avanceStev < limiteAvance) {
		stevPos.z += paso;
		avanceStev += paso;
		if (avanceStev >= limiteAvance) animStev = false;
		
	}// Creeper retrocede
	if (animCreep && avanceCreep < limiteAvance) {
		creepPos.z += paso;  // Retrocede
		avanceCreep += paso;
		if (avanceCreep >= limiteAvance) animCreep = false;
		
	}// Enderman avanza
	if (animEnder && avanceEnder < limiteAvance) {
		enderPos.z += paso;
		avanceEnder += paso;
		if (avanceEnder >= limiteAvance) animEnder = false;
		
	}// Itera sobre todos los zombies para animarlos
	for (size_t i = 0; i < animZoms.size(); i++) {
		if (animZoms[i] && avanceZoms[i] < limiteAvance) {
			zomPositions[i].z += paso;
			avanceZoms[i] += paso;
			if (avanceZoms[i] >= limiteAvance) animZoms[i] = false;
		}
	}// Misma lógica para Snowman, Alex, Creeper2, Snow2, Ender2
	if (animSnow && avanceSnow < limiteAvance) {
		snowPos.z += paso;
		avanceSnow += paso;
		if (avanceSnow >= limiteAvance) animSnow = false;
	}
	if (animAlex && avanceAlex < limiteAvance) {
		alexPos.z += paso;
		avanceAlex += paso;
		if (avanceAlex >= limiteAvance) animAlex = false;
	}
	if (animCreep2 && avanceCreep2 < limiteAvance) {
		creep2Pos.z += paso;
		avanceCreep2 += paso;
		if (avanceCreep2 >= limiteAvance) animCreep2 = false;
	}
	if (animSnow2 && avanceSnow2 < limiteAvance) {
		snow2Pos.z += paso;
		avanceSnow2 += paso;
		if (avanceSnow2 >= limiteAvance) animSnow2 = false;
	}
	if (animEnder2 && avanceEnder2 < limiteAvance) {
		ender2Pos.z += paso;
		avanceEnder2 += paso;
		if (avanceEnder2 >= limiteAvance) animEnder2 = false;
	} //Lord Zedd (Rey) retrocede
	if (animlordz && avancelordz < powerRangersLimit) { //Rey
		lordzPos.z -= powerRangersPaso;
		avancelordz + -powerRangersPaso;
		if (avancelordz >= powerRangersLimit) animlordz = false;
	}// Megazord (Reina) retrocede
	if (animmegazord && avancemegazord < powerRangersLimit) { //Reina
		megazordPos.z -= powerRangersPaso;
		avancemegazord += powerRangersPaso;
		if (avancemegazord >= powerRangersLimit) animmegazord = false;
	} // Misma lógica para Esfinges, Dragones, Zack y Patrulleros
	for (size_t i = 0; i < animesfinge.size(); i++) { //alfiles
		if (animesfinge[i] && avanceesfinge[i] < powerRangersLimit) {
			esfingePositions[i].z -= powerRangersPaso;

			if (avanceesfinge[i] >= powerRangersLimit) animesfinge[i] = false;
		}
	}
	for (size_t i = 0; i < animdragon.size(); i++) { //Caballos
		if (animdragon[i] && avancedragon[i] < powerRangersLimit) {
			dragonPositions[i].z -= powerRangersPaso;
			avancedragon[i] += powerRangersPaso;
			if (avancedragon[i] >= powerRangersLimit) animdragon[i] = false;
		}
	}
	for (size_t i = 0; i < animzack.size(); i++) { //Torres
		if (animzack[i] && avancezack[i] < powerRangersLimit) {
			zackPositions[i].z -= powerRangersPaso;
			avancezack[i] += powerRangersPaso;
			if (avancezack[i] > powerRangersLimit) animzack[i] = false;
		}
	}
	for (size_t i = 0; i < animpatrullero.size(); i++) {
		if (animpatrullero[i] && avancepatrullero[i] < powerRangersLimit) {
			patrulleroPositions[i].z -= powerRangersPaso;
			avancepatrullero[i] += powerRangersPaso;
			if (avancepatrullero[i] >= powerRangersLimit) animpatrullero[i] = false;
		}
	}
}



// Is called whenever a key is pressed/released via GLFW -- Funcion que se llama cuando una tecla se presiona o se suelta
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//Cierra la ventana si se presiona ESC
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
        // Guarda el estado de la tecla (presionada o soltada)
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
        // Alterna la luz con la tecla espacio
	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
		{
			Light1 = glm::vec3(1.0f, 1.0f, 0.0f);
		}
		else
		{
			Light1 = glm::vec3(0);//Cuado es solo un valor en los 3 vectores pueden dejar solo una componente
		}
	}
	//Maneja activación de animaciones con teclas específicas
	// Por ejemplo: V activa animación de Steve
	if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		if (!animStev) {
			animStev = true;
			avanceStev = 0.0f; // Reinicia avance para que se mueva 2.0 cada vez
		}
	}// Otras teclas para animar personajes específicos (Z, X, C, 9, N, M)
	if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		animCreep2 = true;
		avanceCreep2 = 0.0f;
	}
	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		animSnow2 = true;
		avanceSnow2 = 0.0f;
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		animEnder2 = true;
		avanceEnder2 = 0.0f;
	}
	if (key == GLFW_KEY_9 && action == GLFW_PRESS) { // creep
		animCreep = true;
		avanceCreep = 0.0f;
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS) { // ender
		animEnder = true;
		avanceEnder = 0.0f;
	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS) { // snow
		animSnow = true;
		avanceSnow = 0.0f;
	}
}

//Control de rotacion de camara con el boton central del mouse
void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	static bool middleButtonPressed = false;

	// Verifica si el botón derecho está presionado
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		if (firstMouse)
		{
			lastX = xPos;
			lastY = yPos;
			firstMouse = false;
			middleButtonPressed = true;
		}

		if (middleButtonPressed)
		{
			float xOffset = xPos - lastX;
			float yOffset = lastY - yPos;  // Invertido porque el eje Y va de abajo hacia arriba

			lastX = xPos;
			lastY = yPos;

			camera.ProcessMouseMovement(xOffset, yOffset);
		}
	}
	else
	{
		// Si el botón derecho no está presionado, reinicia el estado para evitar movimientos no deseados
		firstMouse = true;
		middleButtonPressed = false;
	}
}



// Función para convertir coordenadas del mouse a coordenadas del mundo relacionadas a las casillas del tablero (plano Y=0)
glm::vec3 ScreenToWorld(double xpos, double ypos, float planeY = 0.0f) {
	// Convertir coordenadas del mouse a NDC
	float x = (2.0f * xpos) / WIDTH - 1.0f;
	float y = 1.0f - (2.0f * ypos) / HEIGHT;

	// Crear vector en espacio de clip
	glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

	// Convertir a espacio de ojos (eye space)
	glm::mat4 invProjection = glm::inverse(projection);
	glm::vec4 rayEye = invProjection * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

	// Convertir a espacio del mundo
	glm::mat4 invView = glm::inverse(view);
	glm::vec4 rayWorld = invView * rayEye;
	glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorld));

	// Calcular intersección con el plano Y = planeY
	float t = (planeY - camera.GetPosition()[1]) / rayDir.y;
	glm::vec3 worldPos = camera.GetPosition() + t * rayDir;

	float cellSize = 0.6f; // Distancia entre casillas
	float boardOriginX = -2.1f; // Origen del tablero en X
	float boardOriginZ = -2.1f; // Origen del tablero en Z
	int boardSize = 8;          // Tamaño 8x8 casillas

	// Calcular casilla
	float relativeX = (worldPos.x - boardOriginX) / cellSize;
	float relativeZ = (worldPos.z - boardOriginZ) / cellSize;

	int cellX = static_cast<int>(round(relativeX));
	int cellZ = static_cast<int>(round(relativeZ));

	// Verificar si está dentro del tablero
	if (cellX < 0 || cellX >= boardSize || cellZ < 0 || cellZ >= boardSize) {
		// Fuera del tablero
		return glm::vec3(9999.0f);
	}

	// Calcular posición centrada
	float snappedX = boardOriginX + cellX * cellSize;
	float snappedZ = boardOriginZ + cellZ * cellSize;

	return glm::vec3(snappedX, planeY, snappedZ);
}

static bool IsClose(float a, float b, float epsilon = 0.05f) {
	return fabs(a - b) < epsilon;
}

// Maneja clics del mouse en el tablero
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
	        // Detecta posición del clic y la transforma al tablero
		// Si estamos en modo "seleccionar personaje", lo agarra
		// Si ya se tiene uno seleccionado, intenta moverlo a otra celda
		glfwGetCursorPos(window, &xpos, &ypos);
		glm::vec3 posClic = ScreenToWorld(xpos, ypos, 0.0f); // Se obtiene la posicion en el tablero

		std::cout << "(" << posClic.x << "," << posClic.y << "," << posClic.z << ")" << std::endl;
		if (posClic.x == 0.999f) return;	// Posicion fuera del tablero, no se hace nada
		auto cell = board.getCellFromPosition(posClic.x, posClic.z);
		// Si puedes agarrar un personaje
		if (pick_place) {
			// Si hay un personaje en la casilla, lo agarras
			if (board.haveCharacter(cell.first, cell.second)) {
				diffuseSL.y = diffuseSL.z = 0.0f;			//Cambia la luz de color
				pick_place = false;						    //Cambiar de modo
				originX = posClic.x;
				originZ = posClic.z;						// Guarda las coordenadas de inicio
			}
		}
		else {
			// Si tienes un personaje seleccionado, intenta soltarlo
			destinationX = posClic.x;
			destinationZ = posClic.z;			// Coordenadas de destino
			if (!board.move(originX, originZ, destinationX, destinationZ)) return;	//Si no se pudo mover, termina
			diffuseSL.y = diffuseSL.z = 1.0f;		// Se resetea la luz
			pick_place = true;						// Se cambia de modo
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) { //Cancela la seleccion actual si se presiona clic derecho
		// Cancelar la selección
		if (!pick_place) {		// Si se tiene un personaje agarrado
			diffuseSL.y = diffuseSL.z = 1.0f;
			pick_place = true;
		}
	}
}

//Inicializa personajes del bando Minecraft con su tipo de pieza
void InitMinecraftCharacters() {//Crea personajes iniciales
                                //Luego agrega zombies como peones
	minecraftCharacters.push_back({ "Creeper 2", &creep2Pos, minecraft, Torre });   // cells[0][0]
	minecraftCharacters.push_back({ "Snowman 2", &snow2Pos, minecraft, Caballo });    // cells[0][1]
	minecraftCharacters.push_back({ "Enderman 2", &ender2Pos, minecraft, Alfil });   // cells[0][2]
	minecraftCharacters.push_back({ "Steve", &stevPos, minecraft, Rey });     // cells[0][3]
	minecraftCharacters.push_back({ "Alex", &alexPos, minecraft, Reyna });     // cells[0][4]
	minecraftCharacters.push_back({ "Enderman", &enderPos, minecraft, Alfil }); // cells[0][5]
	minecraftCharacters.push_back({ "Snowman", &snowPos, minecraft, Caballo });  // cells[0][6]
	minecraftCharacters.push_back({ "Creeper", &creepPos, minecraft, Torre }); // cells[0][7]
	// Zombies (se asignarán a otras celdas después)
	for (int i = 0; i < zomPositions.size(); ++i) {
		minecraftCharacters.push_back({ "Zombie " + std::to_string(i), &zomPositions[i], minecraft, Peon });
	}
}
//Inicializa personajes del bando de Power Rangers
void InitPowerRangersCharacters() {//Rey y Reina
	                          //alfiles (Esfinges), Caballos (Dragonzords), Torres (Zack), Peones (Patrulleros) 
	powerRangersCharacters.push_back({ "Lord Zedd", &lordzPos, powerRangers, Rey });
	powerRangersCharacters.push_back({ "Megazord", &megazordPos, powerRangers, Reyna });

	for (size_t i = 0; i < esfingePositions.size(); ++i) {
		powerRangersCharacters.push_back({ "Esfinge" + std::to_string(i + 1), &esfingePositions[i], powerRangers, Alfil });
	}
	for (size_t i = 0; i < dragonPositions.size(); ++i) {
		powerRangersCharacters.push_back({ "Dragozord" + std::to_string(i + 1), &dragonPositions[i], powerRangers, Caballo });
	}
	for (size_t i = 0; i < zackPositions.size(); ++i) {
		powerRangersCharacters.push_back({ "Zack" + std::to_string(i + 1), &zackPositions[i], powerRangers, Torre });
	}
	for (size_t i = 0; i < patrulleroPositions.size(); ++i) {
		powerRangersCharacters.push_back({ "Patrullero" + std::to_string(i + 1), &patrulleroPositions[i], powerRangers, Peon });
	}
}

