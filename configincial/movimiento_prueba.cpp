#include <iostream>
#include <cmath>
#include <vector>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
glm::vec3 ScreenToWorld(double xpos, double ypos, float planeY);
void DoMovement();
void Animation();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 3.0f, 5.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 spotlightPos(0.0f);
glm::vec3 lightDir(0.0f);
bool active;

glm::mat4 projection;
glm::mat4 view;
glm::vec3 PosMouseW;
// Variable para almacenar el último punto clickeado
glm::vec3 lastClickPos(0.0f);
glm::vec3 Light1 = glm::vec3(0);


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame


//=====================Variables que deinen la posicion de los modelos=====================
// Variables para la animación de stev
glm::vec3 stevPos(-0.3f, 0.0f, -2.1f);
bool animStev = false;
float avanceStev = 0.0f; // cuánto ha avanzado desde que se activó
const float limiteAvance = 0.6f; // límite de movimiento

glm::vec3 creepPos(2.1f, 0.0f, -2.1f);
bool animCreep = false;
float avanceCreep = 0.0f;

glm::vec3 enderPos(0.9f, 0.0f, -2.1f);
bool animEnder = false;
float avanceEnder = 0.0f;

glm::vec3 snowPos(1.5f, 0.0f, -2.1f);
bool animSnow = false;
float avanceSnow = 0.0f;

glm::vec3 alexPos(0.3f, -0.4f, -2.1f);
bool animAlex = false;
float avanceAlex = 0.0f;

glm::vec3 creep2Pos(-2.1f, 0.0f, -2.1f);
bool animCreep2 = false;
float avanceCreep2 = 0.0f;

glm::vec3 snow2Pos(-1.5f, 0.0f, -2.1f);
bool animSnow2 = false;
float avanceSnow2 = 0.0f;

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

std::vector<bool> animZoms(8, false);
std::vector<float> avanceZoms(8, 0.0f);

glm::vec3 lordzPos(4.0f, 0.0f, 0.0f);
glm::vec3 megazordPos(3.0f, 0.0f, 0.0f);

std::vector<glm::vec3> esfingePositions = {
    {2.0f, 0.0f, 0.0f},
    {5.0f, 0.0f, 0.0f}
};

std::vector<glm::vec3> dragonPositions = {
    {1.0f, 0.0f, 0.0f},
    {6.0f, 0.0f, 0.0f}
};

std::vector<glm::vec3> zackPositions = {
    {0.0f, 0.0f, 0.0f},
    {7.0f, 0.0f, 0.0f}
};

std::vector<glm::vec3> patrulleroPositions = {
    {0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 1.0f},
    {2.0f, 0.0f, 1.0f},
    {3.0f, 0.0f, 1.0f},
    {4.0f, 0.0f, 1.0f},
    {5.0f, 0.0f, 1.0f},
    {6.0f, 0.0f, 1.0f},
    {7.0f, 0.0f, 1.0f}
};
//=========================================================================================

int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Fuentes de luz", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);



	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");
	
	//=============================Carga de modelos=============================
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
	//==========================================================================


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

	projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		Animation();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	   
		// OpenGL options
		glEnable(GL_DEPTH_TEST);

		
		
		//Load Model
	

		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();

                  glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "specular"),1);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);


		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), 0.2f, 1.0f, 0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.5f, 0.5f, 0.5f); // Ambiente tenue
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"),0.0f, 0.0f, 0.0f);

		if (active) {
			lightPos = glm::vec3(spotlightPos.x, spotlightPos.y + 3.0f, spotlightPos.z);
			lightDir = glm::normalize(spotlightPos - lightPos);
			// Configuración del spotlight
			glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), lightPos.x, lightPos.y, lightPos.z);
		}
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), lightDir.x, lightDir.y, lightDir.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.1f, 0.1f, 0.1f);  // Luz ambiental baja
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);  // Luz difusa blanca brillante
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);  // Reflejos blancos
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);  // Atenuación moderada
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f); // Atenuación cuadrática
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(5.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(7.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);

		// Create camera transformations
		view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glm::mat4 model(1);

	
		view = camera.GetViewMatrix();

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
		//model = glm::mat4(1); //Rey
  //      model = glm::translate(glm::mat4(1.0f), lordzPos);
		//model = glm::scale(model, glm::vec3(0.5f));
  //      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  //      lordz.Draw(lightingShader);
		////===================================================================================
  //              
		////===============================Dibujado del Megazord=================================
  //      model = glm::translate(glm::mat4(1.0f), megazordPos); //Reyna
		//model = glm::scale(model, glm::vec3(80.0f));
  //      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  //      megazord.Draw(lightingShader);
		////=====================================================================================

		////===============================Dibujado del Esfinge=================================      
  //      for (auto& pos : esfingePositions) { // Alfiles
		//	model = glm::translate(glm::mat4(1.0f), pos);
		//	model = glm::scale(model, glm::vec3(1.0f));	       
  //          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  //          esfinge.Draw(lightingShader);
  //      }
		////====================================================================================
		//
		////===============================Dibujado del Dragon================================= 
  //      for (auto& pos : dragonPositions) {// Caballos
		//	model = glm::translate(glm::mat4(1.0f), pos);
		//	model = glm::scale(model, glm::vec3(0.2f));
  //          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  //          dragon.Draw(lightingShader);
  //      }
		////===================================================================================

		////===============================Dibujado de Zack=================================
  //      for (auto& pos : zackPositions) { // Torres
		//	model = glm::translate(glm::mat4(1.0f), pos);
		//	model = glm::scale(model, glm::vec3(1.0f));
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  //          zack.Draw(lightingShader);
  //      }
		////================================================================================

		////===============================Dibujado de Patrullero=================================
  //      for (auto& pos : patrulleroPositions) {
		//	model = glm::translate(glm::mat4(1.0f), pos);
		//	model = glm::scale(model, glm::vec3(0.015f));
  //          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  //          patrullero.Draw(lightingShader);
  //      }
		////======================================================================================

		glBindVertexArray(0);
	

		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");
		glBindVertexArray(0);



		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();



	return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(CIRC_LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(CIRC_RIGHT, deltaTime);


	}
	
}
void Animation() {
	float paso = 0.01f;

	if (animStev && avanceStev < limiteAvance) {
		stevPos.z += paso;
		avanceStev += paso;
		if (avanceStev >= limiteAvance) animStev = false;
	}
	if (animCreep && avanceCreep < limiteAvance) {
		creepPos.z += paso;  // Retrocede
		avanceCreep += paso;
		if (avanceCreep >= limiteAvance) animCreep = false;
	}
	if (animEnder && avanceEnder < limiteAvance) {
		enderPos.z += paso;
		avanceEnder += paso;
		if (avanceEnder >= limiteAvance) animEnder = false;
	}
	for (size_t i = 0; i < animZoms.size(); i++) {
		if (animZoms[i] && avanceZoms[i] < limiteAvance) {
			zomPositions[i].z += paso;
			avanceZoms[i] += paso;
			if (avanceZoms[i] >= limiteAvance) animZoms[i] = false;
		}
	}
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
	}
}



// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

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
	if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		if (!animStev) {
			animStev = true;
			avanceStev = 0.0f; // Reinicia avance para que se mueva 2.0 cada vez
		}
	}
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
	if (key == GLFW_KEY_B && action == GLFW_PRESS) { // alex
		animAlex = true;
		avanceAlex = 0.0f;
	}
	if (key >= GLFW_KEY_1 && key <= GLFW_KEY_8 && action == GLFW_PRESS) {
		int index = key - GLFW_KEY_1;
		if (index < animZoms.size()) {
			animZoms[index] = true;
			avanceZoms[index] = 0.0f;
		}
	}

}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	static bool rightButtonPressed = false;

	// Verifica si el botón derecho está presionado
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if (firstMouse)
		{
			lastX = xPos;
			lastY = yPos;
			firstMouse = false;
			rightButtonPressed = true;
		}

		if (rightButtonPressed)
		{
			float xOffset = xPos - lastX;
			float yOffset = lastY - yPos;  // Invertido porque el eje Y va de abajo hacia arriba

			lastX = xPos;
			lastY = yPos;

			camera.ProcessMouseMovement(xOffset, yOffset);
		}
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		spotlightPos = ScreenToWorld(xpos, ypos, 0.0f);
		active  = true;
		PosMouseW = ScreenToWorld(xpos, ypos, 0.0f);
		std::cout << "[CLICK] Posicion en el plano: ("
			<< PosMouseW.x << ", " << PosMouseW.y << ", " << PosMouseW.z << ")" << std::endl;
	}
	else
	{
		// Si el botón derecho no está presionado, reinicia el estado para evitar movimientos no deseados
		firstMouse = true;
		rightButtonPressed = false;
	}
}



// Función para convertir coordenadas del mouse a coordenadas del mundo (plano Y=0)
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

	return worldPos;
}
