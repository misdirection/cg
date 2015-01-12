//////////////////////////////////////////////////////////////////////////
//Sources: http://openglbook.com/
////////// http://www.dhpoware.com/demos/glObjViewer.html
////////// http://www.arcsynthesis.org/gltut/
/////////////////////////////////////////////////////////////////////////
#define GLEW_STATIC

#include <iostream>
#include <climits>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <queue>

#include <TransformationStack.h>
#include <Shader.h>
//include gloost::Mesh wich is a geometry container
#include <Mesh.h>
//mesh object for loading planet geometry
gloost::Mesh* mesh = 0;

// loader for the wavefront *.obj file format
#include <ObjLoader.h>

#include <ctime> 
#include <cstdlib> //rand and srand

#include "TextureLoader.h"

#define PI 3.1415926535897932384626433832795028841971

//
int windowWidth = 800;
int windowHeight = 600;
int windowHandle = 0;

//initial camera position
float camera_position[] = { 0.0f, 2.0, 8.0f };

bool middleMouseButtonDown = false;
bool leftMouseButtonDown = false;
bool rightMouseButtonDown = false;


unsigned frameCount = 0;

//handles for shader variables
unsigned projectionMatrixUniformLocation = 0;
unsigned modelMatrixUniformLocation = 0;
unsigned viewMatrixUniformLocation = 0;
unsigned normalMatrixUniformLocation = 0;

unsigned planetColorUniformLocation = 0;
unsigned useToonShaderUniformLocation = 0;
unsigned colorTextureUniformLocation = 0;
unsigned normalTextureUniformLocation = 0;
unsigned useGreyScaleUniformLocation = 0;
unsigned useMirrorUniformLocation = 0;

//handles for all sort of geometry objects
unsigned vertexArrayObject = 0;
unsigned vertexBufferObject = 0;
unsigned elementArrayBuffer = 0;

//handles for shader program and shaders
unsigned shaderProgram = 0;
unsigned vertexShader = 0;
unsigned fragmentShader = 0;


//texture handle
unsigned sunColorTexture = 0;
unsigned mercuryColorTexture = 0;
unsigned venusColorTexture = 0;
unsigned earthColorTexture = 0;
unsigned marsColorTexture = 0;
unsigned jupiterColorTexture = 0;
unsigned saturnColorTexture = 0;
unsigned uranusColorTexture = 0;
unsigned neptuneColorTexture = 0;
unsigned moonColorTexture = 0;

unsigned sunNormalTexture = 0;
unsigned mercuryNormalTexture = 0;
unsigned venusNormalTexture = 0;
unsigned earthNormalTexture = 0;
unsigned marsNormalTexture = 0;
unsigned jupiterNormalTexture = 0;
unsigned saturnNormalTexture = 0;
unsigned uranusNormalTexture = 0;
unsigned neptuneNormalTexture = 0;
unsigned moonNormalTexture = 0;

unsigned skySphereColorTexture = 0;
unsigned skySphereNormalTexture = 0;

bool useToonShader = 0;
int useGreyScale = 0;
int useMirror = 0;
//the three different matrices for projection, viewing and model transforming

TransformationStack modelTransformationStack;
TransformationStack cameraTransformationStack;

//Function callbacks
void initialize(int, char*[]);
void initWindow(int, char*[]);
void resizeFunction(int, int);
void idleFunction(void);

//forward declaration of functions
void mouseInput(int button, int state, int x, int y);
void specialKeyRelease(int keyEvent, int x, int y);
void specialKeyPress(int keyEvent, int x, int y);
void keyRelease(unsigned char keyEvent, int x, int y);
void keyPress(unsigned char keyEvent, int x, int y);

void timerFunction(int);
void cleanup(void);
void loadModel(void);
void loadTextures(void);
void setupShader();
void draw(void);
void renderFunction(void);

void drawSun(glm::mat4 const& model_matrix, glm::mat4 const& view_matrix, float* color);
void drawPlanet(glm::mat4 const& model_matrix, glm::mat4 const& view_matrix, float* color);
void drawStars(glm::mat4 const& model_matrix, glm::mat4 const& view_matrix);
void drawOrbit(glm::mat4 const& model_matrix, glm::mat4 const& view_matrix);

std::vector<float> starCPUGeometry;

unsigned starVertexArrayObject = 0;
unsigned starVertexBufferObject = 0;

unsigned starOrbitShaderProgram = 0;
unsigned starOrbitVertexShader = 0;
unsigned starOrbitFragmentShader = 0;

unsigned starOrbitShaderProjectionMatrixUniformLocation = 0;
unsigned starOrbitShaderModelMatrixUniformLocation = 0;
unsigned starOrbitShaderViewMatrixUniformLocation = 0;

unsigned numStars = 10000;

//orbit variables and handles
std::vector<float> orbitCPUGeometry;
unsigned orbitVertexArrayObject = 0;
unsigned orbitVertexBufferObject = 0;

unsigned numVerticesInOrbit = 360;

/////////////////////////////

unsigned renderFBO = 0; //frame buffer object handle
unsigned renderColorBuffer = 0; //texture handle for color attachment
unsigned renderDepthBuffer = 0; //texture handle for depth attachment
unsigned screenQuadVertexArrayObject = 0; //VAO for screenquad
unsigned screenQuadVertexBufferObject = 0; //VBO for screenquad
unsigned screenQuadShaderProgram = 0; //textured fs quad shader
unsigned screenQuadVertexShader = 0; //corresponding vertex shader
unsigned screenQuadFragmentShader = 0; //corresponding fragment shad.
//uniform location for the texture of the first rendering pass
unsigned screenQuadShaderColorTextureUniformLocation = 0;
//uniform location for screen dimensions (windowWidth & windowHeight)
unsigned screenQuadShaderScreenDimensionsUniformLocation = 0;


float screenQuadGeometry[] =
{
	//first triangle
	-1.0f, 1.0f, 0.0f, //#######
	1.0f, 1.0f, 0.0f, //####
	-1.0f, -1.0f, 0.0f, //##
	//second triangle
	-1.0f, -1.0f, 0.0f, // ##
	1.0f, 1.0f, 0.0f, // ####
	1.0f, -1.0f, 0.0f // ######
};


int main(int argc, char* argv[])
{
	initialize(argc, argv);

	//start the glut event system
	glutMainLoop();

	exit(EXIT_SUCCESS);
}


/////////////////////////////////////////////////////////////////////////////////////////


//called every frame this functions draw
void draw(void)
{



	int now = glutGet(GLUT_ELAPSED_TIME);

	//planetOwnRotation
	float planetOwnRotation = now * 0.01 * 365 / 30.0;

	// Rotation
	float sunRotation = now*0.005;
	float mercuryRotation = sunRotation * 365 / 88.0;
	float venusRotation = sunRotation * 365 / 225.0;
	float earthRotation = sunRotation * 365 / 365.0;
	float marsRotation = sunRotation * 365 / 544.0;
	float jupiterRotation = sunRotation * 365 / 702.0;
	float saturnRotation = sunRotation * 365 / 830.0;
	float uranusRotation = sunRotation * 365 / 935.0;
	float neptuneRotation = sunRotation * 365 / 1022.0;

	//translation
	float distanceConstant = 10.0f;

	float sunTranslation = 0.0 * distanceConstant;
	float mercuryTranslation = 0.4 * distanceConstant;
	float venusTranslation = 0.7 * distanceConstant;
	float earthTranslation = 1 * distanceConstant;
	float marsTranslation = 1.5 * distanceConstant;
	float jupiterTranslation = 2 * distanceConstant;
	float saturnTranslation = 2.5 * distanceConstant;
	float uranusTranslation = 2.9 * distanceConstant;
	float neptuneTranslation = 3.3 * distanceConstant;

	//scaling
	float sunScaling = 4.0f;
	float mercuryScaling = 0.383;
	float venusScaling = 0.949;
	float earthScaling = 1.0;
	float marsScaling = 0.533;
	float jupiterScaling = 1.35;
	float saturnScaling = 1.49;
	float uranusScaling = 1.1;
	float neptuneScaling = 1.1;

	float planetColor[] = { 0.0, 0.0, 0.0 };

	//////////////////////////////////////////////////////////////////////////
	//Preparations for PASS 1
	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
	glClearColor(0.0, 0.0, 0.0, 1.0); //specify clear color for color attachments
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//PUT YOUR SOLAR SYSTEM RENDER CODE HERE


	glUseProgram(shaderProgram);

	cameraTransformationStack.clear();

	//translate the camera in positive z direction to actually see the geometry residing in the coordinate origin 
	cameraTransformationStack.pushMatrix(glm::translate(glm::mat4(1.0), glm::vec3(camera_position[0], camera_position[1], camera_position[2])));

	//invert the camera transformation to move the vertices
	glm::mat4 viewMatrix = glm::inverse(cameraTransformationStack.topMatrix());

	//upload the view matrix to the currently bound shader
	glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	glUniform1i(useToonShaderUniformLocation, useToonShader);
	glUniform1i(colorTextureUniformLocation, 0);
	glUniform1i(normalTextureUniformLocation, 1);

	//create the planet transformation matrices
	//drawSkySphere
	//rotate the sun around its own axis
	//modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(planetOwnRotation), glm::vec3(0.0f, 1.0f, 0.0f)));
	//scale the skySphere
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(1500.0f)));
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), 180.0f, glm::vec3(0.0, 1.0, 0.0f)));
	modelTransformationStack.pushMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(camera_position[0], camera_position[1], camera_position[2])));
	//rotate it slowly around the y-axis
	//modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(sunRotation), glm::vec3(0.0f, 1.0f, 0.0f)));

	//yellow
	planetColor[0] = 3.0;
	planetColor[1] = 3.0;
	planetColor[2] = 0.0;

	//skyspherecolor texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skySphereColorTexture);
	//skyspherenormal texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, skySphereNormalTexture);
	//draw the geometry
	drawSun(modelTransformationStack.topMatrix(), viewMatrix, planetColor);
	//clear the transformation stack
	modelTransformationStack.clear();


	//create the planet transformation matrices
	//sun 
	//rotate the sun around its own axis
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(planetOwnRotation), glm::vec3(0.0f, 1.0f, 0.0f)));
	//scale the sun
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(sunScaling)));
	//rotate it slowly around the y-axis
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(sunRotation), glm::vec3(0.0f, 1.0f, 0.0f)));

	//yellow
	planetColor[0] = 3.0;
	planetColor[1] = 3.0;
	planetColor[2] = 0.0;

	//suncolor texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunColorTexture);
	//sunnormal texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sunNormalTexture);

	//draw the geometry
	drawSun(modelTransformationStack.topMatrix(), viewMatrix, planetColor);
	//clear the transformation stack
	modelTransformationStack.clear();

	//mercury
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(planetOwnRotation), glm::vec3(0.0f, 1.0f, 0.0f)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(mercuryScaling)));
	modelTransformationStack.pushMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(mercuryTranslation,0.0, 0.0) ) );
	//rotate it slowly around the y-axis
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(mercuryRotation), glm::vec3(0.0f, 1.0f, 0.0f)));

	//grey
	planetColor[0] = 0.5;
	planetColor[1] = 0.5;
	planetColor[2] = 0.5;

	//mercurycolor texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mercuryColorTexture);
	//mercurynormal texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mercuryNormalTexture);
	//draw the geometry
	drawPlanet(modelTransformationStack.topMatrix(), viewMatrix, planetColor);
	//clear the transformation stack
	modelTransformationStack.clear();

	//venus
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(planetOwnRotation), glm::vec3(0.0f, 1.0f, 0.0f)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(venusScaling)));
	modelTransformationStack.pushMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(venusTranslation, 0.0, 0.0)));
	//rotate it slowly around the y-axis
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(venusRotation), glm::vec3(0.0f, 1.0f, 0.0f)));

	//orange-ish
	planetColor[0] = 0.8;
	planetColor[1] = 0.6;
	planetColor[2] = 0.4;

	//venuscolor texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, venusColorTexture);
	//venusnormal texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, venusNormalTexture);

	//draw the geometry
	drawPlanet(modelTransformationStack.topMatrix(), viewMatrix, planetColor);
	//clear the transformation stack
	modelTransformationStack.clear();

	//earth
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(planetOwnRotation), glm::vec3(0.0f, 1.0f, 0.0f)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(earthScaling)));
	modelTransformationStack.pushMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(earthTranslation, 0.0, 0.0)));
	//rotate it slowly around the y-axis
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(earthRotation), glm::vec3(0.0f, 1.0f, 0.0f)));

	//blue
	planetColor[0] = 0.3;
	planetColor[1] = 0.3;
	planetColor[2] = 0.7;

	//earth color texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthColorTexture);
	//earthnormal texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthNormalTexture);

	//draw the geometry
	drawPlanet(modelTransformationStack.topMatrix(), viewMatrix, planetColor);
	//clear the transformation stack
	modelTransformationStack.clear();

	//earth's moon
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(planetOwnRotation), glm::vec3(0.0f, 1.0f, 0.0f)));
	modelTransformationStack.pushMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.0f, 0.0f ) ) );
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(earthRotation * 30), glm::vec3(0.0f, 1.0f, 0.0f)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(0.374)));
	modelTransformationStack.pushMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(earthTranslation, 0.0, 0.0)));
	//rotate it slowly around the y-axis
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(earthRotation), glm::vec3(0.0f, 1.0f, 0.0f)));

	//grey
	planetColor[0] = 0.3;
	planetColor[1] = 0.3;
	planetColor[2] = 0.3;

	//mooncolor texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, moonColorTexture);
	//moonnormal texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, moonNormalTexture);

	//draw the geometry
	drawPlanet(modelTransformationStack.topMatrix(), viewMatrix, planetColor);
	//clear the transformation stack
	modelTransformationStack.clear();

	//mars
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(planetOwnRotation), glm::vec3(0.0f, 1.0f, 0.0f)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(marsScaling)));
	modelTransformationStack.pushMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(marsTranslation, 0.0, 0.0)));
	//rotate it slowly around the y-axis
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(marsRotation), glm::vec3(0.0f, 1.0f, 0.0f)));

	//red-ish
	planetColor[0] = 0.6;
	planetColor[1] = 0.4;
	planetColor[2] = 0.2;

	//marscolor texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, marsColorTexture);
	//marsnormal texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, marsNormalTexture);

	//draw the geometry
	drawPlanet(modelTransformationStack.topMatrix(), viewMatrix, planetColor);
	//clear the transformation stack
	modelTransformationStack.clear();

	//jupiter
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(planetOwnRotation), glm::vec3(0.0f, 1.0f, 0.0f)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(jupiterScaling)));
	modelTransformationStack.pushMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(jupiterTranslation, 0.0, 0.0)));
	//rotate it slowly around the y-axis
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(jupiterRotation), glm::vec3(0.0f, 1.0f, 0.0f)));

	//grey
	planetColor[0] = 0.75;
	planetColor[1] = 0.75;
	planetColor[2] = 0.75;

	//jupitercolor texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, jupiterColorTexture);
	//jupiternormal texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, jupiterNormalTexture);

	//draw the geometry
	drawPlanet(modelTransformationStack.topMatrix(), viewMatrix, planetColor);
	//clear the transformation stack
	modelTransformationStack.clear();


	//saturn
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(planetOwnRotation), glm::vec3(0.0f, 1.0f, 0.0f)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(saturnScaling)));
	modelTransformationStack.pushMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(saturnTranslation, 0.0, 0.0)));
	//rotate it slowly around the y-axis
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(saturnRotation), glm::vec3(0.0f, 1.0f, 0.0f)));

	//yellow
	planetColor[0] = 0.9;
	planetColor[1] = 0.9;
	planetColor[2] = 0.7;

	//saturncolor texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, saturnColorTexture);
	//saturnnormal texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, saturnNormalTexture);

	//draw the geometry
	drawPlanet(modelTransformationStack.topMatrix(), viewMatrix, planetColor);
	//clear the transformation stack
	modelTransformationStack.clear();

	//uranus
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(planetOwnRotation), glm::vec3(0.0f, 1.0f, 0.0f)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(uranusScaling)));
	modelTransformationStack.pushMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(uranusTranslation, 0.0, 0.0)));
	//rotate it slowly around the y-axis
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(uranusRotation), glm::vec3(0.0f, 1.0f, 0.0f)));

	//light blue
	planetColor[0] = 0.7;
	planetColor[1] = 0.98;
	planetColor[2] = 0.98;

	//uranuscolor texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, uranusColorTexture);
	//uranusnormal texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, uranusNormalTexture);

	//draw the geometry
	drawPlanet(modelTransformationStack.topMatrix(), viewMatrix, planetColor);
	//clear the transformation stack
	modelTransformationStack.clear();

	//neptune
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(planetOwnRotation), glm::vec3(0.0f, 1.0f, 0.0f)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(neptuneScaling)));
	modelTransformationStack.pushMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(neptuneTranslation, 0.0, 0.0)));
	//rotate it slowly around the y-axis
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), (float)(neptuneRotation), glm::vec3(0.0f, 1.0f, 0.0f)));

	//darker blue
	planetColor[0] = 0.4;
	planetColor[1] = 0.5;
	planetColor[2] = 0.8;

	//neptunecolor texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, neptuneColorTexture);
	//neptunenormal texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, neptuneNormalTexture);

	//draw the geometry
	drawPlanet(modelTransformationStack.topMatrix(), viewMatrix, planetColor);
	//clear the transformation stack
	modelTransformationStack.clear();

	glBindVertexArray(0);
	glUseProgram(0);

	//##
	//some stars
	//now use the star shader program
	glUseProgram(starOrbitShaderProgram);
	//upload the view matrix also for the star shader program
	glUniformMatrix4fv(starOrbitShaderViewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	//scale the unit star cube up to the range of -10^3 to 10^3
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(100.0)));
	//have a look inside of the function to see the binding and draw call for the star geometry
	drawStars(modelTransformationStack.topMatrix(), viewMatrix);
	//reset our transformations
	modelTransformationStack.clear();




	//scale and rotate the orbit for the mercury
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), float(90.0), glm::vec3(1.0, 0.0, 0.0)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(mercuryTranslation)));

	//have a look inside of the function to see the binding and draw call for the star geometry
	drawOrbit(modelTransformationStack.topMatrix(), viewMatrix);
	//reset our transformations
	modelTransformationStack.clear();


	//scale and rotate the orbit for the venus
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), float(90.0), glm::vec3(1.0, 0.0, 0.0)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(venusTranslation)));

	//have a look inside of the function to see the binding and draw call for the star geometry
	drawOrbit(modelTransformationStack.topMatrix(), viewMatrix);
	//reset our transformations
	modelTransformationStack.clear();

	//scale and rotate the orbit for the earth
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), float(90.0), glm::vec3(1.0, 0.0, 0.0)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(earthTranslation)));

	//have a look inside of the function to see the binding and draw call for the star geometry
	drawOrbit(modelTransformationStack.topMatrix(), viewMatrix);
	//reset our transformations
	modelTransformationStack.clear();

	//scale and rotate the orbit for the mars
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), float(90.0), glm::vec3(1.0, 0.0, 0.0)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(marsTranslation)));

	//have a look inside of the function to see the binding and draw call for the star geometry
	drawOrbit(modelTransformationStack.topMatrix(), viewMatrix);
	//reset our transformations
	modelTransformationStack.clear();

	//scale and rotate the orbit for the jupiter
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), float(90.0), glm::vec3(1.0, 0.0, 0.0)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(jupiterTranslation)));

	//have a look inside of the function to see the binding and draw call for the star geometry
	drawOrbit(modelTransformationStack.topMatrix(), viewMatrix);
	//reset our transformations
	modelTransformationStack.clear();

	//scale and rotate the orbit for the mars
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), float(90.0), glm::vec3(1.0, 0.0, 0.0)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(saturnTranslation)));

	//have a look inside of the function to see the binding and draw call for the star geometry
	drawOrbit(modelTransformationStack.topMatrix(), viewMatrix);
	//reset our transformations
	modelTransformationStack.clear();


	//scale and rotate the orbit for the mars
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), float(90.0), glm::vec3(1.0, 0.0, 0.0)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(uranusTranslation)));

	//have a look inside of the function to see the binding and draw call for the star geometry
	drawOrbit(modelTransformationStack.topMatrix(), viewMatrix);
	//reset our transformations
	modelTransformationStack.clear();

	//scale and rotate the orbit for the mars
	modelTransformationStack.pushMatrix(glm::rotate(glm::mat4(1.0f), float(90.0), glm::vec3(1.0, 0.0, 0.0)));
	modelTransformationStack.pushMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(neptuneTranslation)));

	//have a look inside of the function to see the binding and draw call for the star geometry
	drawOrbit(modelTransformationStack.topMatrix(), viewMatrix);
	//reset our transformations
	modelTransformationStack.clear();

	//after drawing the solar system, bind the default framebuffer and
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(screenQuadShaderProgram); //use the fullscreen quad shader
	//upload all your uniform
	glUniform2fv(screenQuadShaderScreenDimensionsUniformLocation,1,glm::value_ptr(glm::vec2(windowWidth,windowHeight)));
	glUniform1i(useGreyScaleUniformLocation, useGreyScale);
	glUniform1i(useMirrorUniformLocation, useMirror);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderColorBuffer);
	glBindVertexArray(screenQuadVertexArrayObject);
	glDrawArrays(GL_TRIANGLES, 0, 6);

}

/////////////////////////////////////////////////////////////////////////////////////////
void mouseInput(int button, int state, int x, int y)
{
	//handle mouse input here
}

/////////////////////////////////////////////////////////////////////////////////////////
void specialKeyRelease(int keyEvent, int x, int y)
{
	//handle key release events of special keys here (like arrow keys)
}

/////////////////////////////////////////////////////////////////////////////////////////
void specialKeyPress(int keyEvent, int x, int y)
{
	//handle key press events of special keys here (like arrow keys)
}

/////////////////////////////////////////////////////////////////////////////////////////
void keyRelease(unsigned char keyEvent, int x, int y)
{
	if (keyEvent == ' ')
	{
		setupShader();
		resizeFunction(windowWidth, windowHeight);
	}
	if (keyEvent == 'f' || keyEvent == 'F')
	{
		glutFullScreenToggle();
	}
	if (keyEvent == 27) //escape key
	{
		cleanup();
		glutExit();
	}
	if (keyEvent == 'w' || keyEvent == 'W')
	{
		camera_position[2] -= 1.0;
	}
	if (keyEvent == 's' || keyEvent == 'S')
	{
		camera_position[2] += 1.0;
	}
	if (keyEvent == '1')
	{
		useToonShader = 0;
	}
	if (keyEvent == '2')
	{
		useToonShader = 1;
	}
	if (keyEvent == '7')
	{
		if(useGreyScale == 1){
			useGreyScale = 0;}
		else{useGreyScale = 1;}
	}

	if (keyEvent == '8')
	{
		if(useMirror == 1){
			useMirror = 0;}
		else{useMirror = 1;}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
void keyPress(unsigned char keyEvent, int x, int y)
{}




/////////////////////////////////////////////////////////////////////////////////////////
void timerFunction(int value){
	if (0 != value)
	{
		int fps = frameCount * 4;
		glutSetWindowTitle((gloost::toString(fps) + " fps").c_str());

	}
	frameCount = 0;
	glutTimerFunc(250, timerFunction, 1);
}


void renderFunction(void)
{
	++frameCount;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw();

	glutSwapBuffers();
	glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////////////////////////
void setupFrameBufferObjects()
{
	glGenFramebuffers(1, &renderFBO); //create FBO resource
	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO); //and set it as curr.
	glGenTextures(1, &renderColorBuffer); //create a texture resource

	glBindTexture(GL_TEXTURE_2D, renderColorBuffer); //set it as curr.
	//and set the texture parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//allocate texture memory for width*height RGBA8 pixel on the GPU
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth, windowHeight,
		0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	//bind color attachment to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, renderColorBuffer, 0);
	//generarete the depth buffer out of your handle
	glGenRenderbuffers(1, &renderDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderDepthBuffer); //bind it
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
		windowWidth, windowHeight);//and allocate the memory
	//bind depthbuffer as so called renderbuffer to framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, renderDepthBuffer);
	//unbind FBO = set default FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void resizeFBOTextures() //use this code to resize your attachments
{

	glBindTexture(GL_TEXTURE_2D, renderColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth, windowHeight,
		0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, renderDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
		windowWidth, windowHeight);
}

void setupScreenQuad()
{
	//create V(ertex)A(rray)O(object) which stores the specified attributes of our V(ertex)B(uffer)O(bject)
	glGenVertexArrays(1, &screenQuadVertexArrayObject);
	//bind VAO - scope begins
	glBindVertexArray(screenQuadVertexArrayObject);
	//create a vertex buffer object
	glGenBuffers(1, &screenQuadVertexBufferObject);
	//and bind it as a array buffer target
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVertexBufferObject);

	//load data that resides in CPU RAM into video RAM.
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(screenQuadGeometry),
		&(screenQuadGeometry[0]),
		GL_STATIC_DRAW);
	//enable vertex attribute at location 0 (compare with vertex shader input)
	glEnableVertexAttribArray(0);
	//specify where to read the data for attribute at location 0
	glVertexAttribPointer(0,
		3, //3 floats per triangle vertex
		GL_FLOAT, //datatype
		GL_FALSE, //should the data be normalized?
		sizeof(float) * 3, //size of attribute stride for one primitive
		(GLvoid*)0); //offset in stride
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
//////////////////////////////////////////

void setupShader()
{

	// LOAD AND LINK SHADER
	shaderProgram = glCreateProgram();
	{
		//load a shader (of the given type) and compile it in the convenience class 'Shader'
		vertexShader = Shader::loadShader("../../../data/shaders/simpleVertexShader.vs", GL_VERTEX_SHADER);
		fragmentShader = Shader::loadShader("../../../data/shaders/simpleFragmentShader.fs", GL_FRAGMENT_SHADER);

		//attach the different shader components to the shader program ...
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
	}
	//... and compile it
	glLinkProgram(shaderProgram);

	//program is linked, so we can detach compiled shaders again
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);

	//obtain shader variable locations
	modelMatrixUniformLocation = glGetUniformLocation(shaderProgram, "ModelMatrix");
	viewMatrixUniformLocation = glGetUniformLocation(shaderProgram, "ViewMatrix");
	projectionMatrixUniformLocation = glGetUniformLocation(shaderProgram, "ProjectionMatrix");
	normalMatrixUniformLocation = glGetUniformLocation(shaderProgram, "NormalMatrix");

	planetColorUniformLocation = glGetUniformLocation(shaderProgram, "PlanetColor");
	useToonShaderUniformLocation = glGetUniformLocation(shaderProgram, "useToonShader");

	colorTextureUniformLocation = glGetUniformLocation(shaderProgram, "colorTexture");
	normalTextureUniformLocation = glGetUniformLocation(shaderProgram, "normalTexture");
	//##
	//////////////////////////////
	//create star shader program out of star vertex and star fragment shader

	starOrbitShaderProgram = glCreateProgram();
	{
		//load a shader (of the given type) and compile it in the convenience class 'Shader'
		starOrbitVertexShader = Shader::loadShader("../../../data/shaders/starOrbitVertexShader.vs", GL_VERTEX_SHADER);
		starOrbitFragmentShader = Shader::loadShader("../../../data/shaders/starOrbitFragmentShader.fs", GL_FRAGMENT_SHADER);

		//attach the different shader components to the shader program ...
		glAttachShader(starOrbitShaderProgram, starOrbitVertexShader);
		glAttachShader(starOrbitShaderProgram, starOrbitFragmentShader);
	}
	//... and compile it
	glLinkProgram(starOrbitShaderProgram);

	//program is linked, so we can detach compiled shaders again
	glDetachShader(starOrbitShaderProgram, starOrbitVertexShader);
	glDetachShader(starOrbitShaderProgram, starOrbitFragmentShader);

	//obtain shader variable locations
	starOrbitShaderModelMatrixUniformLocation = glGetUniformLocation(starOrbitShaderProgram, "ModelMatrix");
	starOrbitShaderViewMatrixUniformLocation = glGetUniformLocation(starOrbitShaderProgram, "ViewMatrix");
	starOrbitShaderProjectionMatrixUniformLocation = glGetUniformLocation(starOrbitShaderProgram, "ProjectionMatrix");

	// LOAD AND LINK SHADER
	screenQuadShaderProgram = glCreateProgram();
	{
		//load a shader (of the given type) and compile it in the convenience class 'Shader'
		screenQuadVertexShader = Shader::loadShader("../../../data/shaders/screenQuadVertexShader.vs", GL_VERTEX_SHADER);
		screenQuadFragmentShader = Shader::loadShader("../../../data/shaders/screenQuadFragmentShader.fs", GL_FRAGMENT_SHADER);
		//attach the different shader components to the shader program ...
		glAttachShader(screenQuadShaderProgram, screenQuadVertexShader);
		glAttachShader(screenQuadShaderProgram, screenQuadFragmentShader);
	}
	//... and compile it
	glLinkProgram(screenQuadShaderProgram);
	//program is linked, so we can detach compiled shaders again
	glDetachShader(screenQuadShaderProgram, screenQuadVertexShader);
	glDetachShader(screenQuadShaderProgram, screenQuadFragmentShader);
	screenQuadShaderColorTextureUniformLocation = glGetUniformLocation(screenQuadShaderProgram, "screenQuadColorTexture");

	screenQuadShaderScreenDimensionsUniformLocation = glGetUniformLocation(screenQuadShaderProgram, "screenDimensions");
	useGreyScaleUniformLocation = glGetUniformLocation(screenQuadShaderProgram, "useGreyScale");
	useMirrorUniformLocation = glGetUniformLocation(screenQuadShaderProgram, "useMirror");
}


/////////////////////////////////////////////////////////////////////////////////////////


void loadModel()
{

	//load a wavefront *.obj file
	gloost::ObjLoader loader("../../../data/objects/sphere.obj");
	mesh = loader.getMesh();

	//IMPORTANT: use this to increase the reference counter
	//gloost::meshes have a garbage collector which throws
	//the mesh away otherwise
	mesh->takeReference();

	mesh->generateNormals();

	//normalizes the mesh
	mesh->scaleToSize(1.0);
	//puts the meshdata in one array
	mesh->interleave();

	//mesh->printMeshInfo();

	//create V(ertex)A(rray)O(object) which stores the specified attributes of our V(ertex)B(uffer)O(bject)
	glGenVertexArrays(1, &vertexArrayObject);
	//bind VAO - scope begins
	glBindVertexArray(vertexArrayObject);


	//create a vertex buffer object
	glGenBuffers(1, &vertexBufferObject);
	//and bind it as a array buffer target
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	//load data that resides in CPU RAM into video RAM.
	glBufferData(GL_ARRAY_BUFFER, //??
		sizeof(float)* mesh->getInterleavedAttributes().size(), //??
		&mesh->getInterleavedAttributes().front(), //??
		GL_STATIC_DRAW); //??

	//enable vertex attribute at location 0 (compare with vertex shader input)
	glEnableVertexAttribArray(0);

	//specify where to read the data for attribute at location 0 
	glVertexAttribPointer(0,
		GLOOST_MESH_NUM_COMPONENTS_VERTEX,
		GL_FLOAT, //datatype
		GL_FALSE, //should the data be normalized?
		mesh->getInterleavedInfo().interleavedPackageStride, //size of attribute stride for one primitive
		(GLvoid*)(mesh->getInterleavedInfo().interleavedVertexStride)); //offset in stride


	//enable vertex attribute at location 1 (compare with vertex shader input)
	glEnableVertexAttribArray(1);

	//specifies where in the GL_ARRAY_BUFFER our data(the vertex normal) is exactly (compare with vertex shader input)
	glVertexAttribPointer(1,
		GLOOST_MESH_NUM_COMPONENTS_NORMAL,
		GL_FLOAT, GL_FALSE,
		mesh->getInterleavedInfo().interleavedPackageStride,
		(GLvoid*)(mesh->getInterleavedInfo().interleavedNormalStride));

	glEnableVertexAttribArray(2);
	//specifies where in the GL_ARRAY_BUFFER our data(the per-vertex texture coordinates) is exactly
	glVertexAttribPointer(2,
		GLOOST_MESH_NUM_COMPONENTS_TEXCOORD,
		GL_FLOAT, GL_FALSE,
		mesh->getInterleavedInfo().interleavedPackageStride,
		(GLvoid*)(mesh->getInterleavedInfo().interleavedTexcoordStride));

	//the buffer that becomes the element array object is created
	glGenBuffers(1, &elementArrayBuffer);
	//and then bound to the element array buffer target
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBuffer);
	//the triangle indices are loaded and associated with the bound array element buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(gloost::TriangleFace) * mesh->getTriangles().size(),
		&mesh->getTriangles().front(),
		GL_STATIC_DRAW);

	// unbind the VAO - scope ends
	glBindVertexArray(0);


	//##start of star geometry generation
	//NOTE: rand() creates an integer value between 0 and RAND_MAX.
	//      Therefore, float(rand)/RAND_MAX gets us a normalized float value between 0.0 and 1.0.
	//create geometry for numStars stars in our CPU Buffer
	for (int starNum = 0; starNum < numStars; ++starNum)
	{
		//3D position
		starCPUGeometry.push_back((float(rand()) / RAND_MAX) * 2.0 - 1.0); //x value between -1.0 and 1.0
		starCPUGeometry.push_back((float(rand()) / RAND_MAX) * 2.0 - 1.0); //y value between -1.0 and 1.0
		starCPUGeometry.push_back((float(rand()) / RAND_MAX) * 2.0 - 1.0); //z value between -1.0 and 1.0

		//RGB color value between 0.0 and 1.0
		starCPUGeometry.push_back(1.0); //full red component 
		starCPUGeometry.push_back((float(rand())/RAND_MAX) * 0.1 + 0.9); //green component between 0.9 and 1.0
		starCPUGeometry.push_back((float(rand()) / RAND_MAX)); //blue component between 0.0 and 1.0
	}

	//generate a new VertexArrayObject. This describes our BufferLayout
	glGenVertexArrays(1, &starVertexArrayObject);
	//bind it to be our active VAO
	glBindVertexArray(starVertexArrayObject);

	//generate a new VertexBufferObject. This will become the GPU representation of our CPU data
	glGenBuffers(1, &starVertexBufferObject);
	//bind it to be our active VBO
	glBindBuffer(GL_ARRAY_BUFFER, starVertexBufferObject);
	//---------------

	//load the data of our CPU buffer via the handle we just created into our GPU Buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*starCPUGeometry.size(), &(starCPUGeometry[0]), GL_STATIC_DRAW);


	//DEFINE POSITION IN LAYOUT
	//enable vertex attribute at location 0 (compare with vertex shader input)
	glEnableVertexAttribArray(0); //this will describe our position

	//specify where to read the data for attribute at location 0 
	glVertexAttribPointer(0, //first attribute
		3, //how many components for the position?
		GL_FLOAT, //datatype 
		GL_FALSE, //should the data be normalized?
		sizeof(float)* 6, //size of whole vertex attribute stride for one primitive (3 * float for position + 3 * float for color)
		(GLvoid*)(0)); //offset in stride: position is begins at byte 0 in the stride. expected to be cast to GLvoid*

	//DEFINE COLOR IN LAYOUT
	//enable vertex attribute at location 1 (compare with vertex shader input)
	glEnableVertexAttribArray(1); //this will describe our position

	//specify where to read the data for attribute at location 0 
	glVertexAttribPointer(1, //second attribute
		3, //how many components for the color?
		GL_FLOAT, //datatype 
		GL_FALSE, //should the data be normalized?
		sizeof(float) * 6, //size of whole vertex attribute stride for one primitive (3 * float for position + 3 * float for color)
		(GLvoid*)(sizeof(float) * 3)); //offset in stride: color starts after 3 * sizeof(float) bytes 



	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//create 360 vertices on a unit circle on the XY-Plane
	for (float currAngleInRad = 0.0; currAngleInRad < 2*PI; currAngleInRad += (2*PI)/numVerticesInOrbit )
	{
		//push position on XY Plane
		orbitCPUGeometry.push_back(std::cos(currAngleInRad)); //x component
		orbitCPUGeometry.push_back(std::sin(currAngleInRad)); //y component
		orbitCPUGeometry.push_back(0); //z component

		//push RGB = (1,1,1)
		orbitCPUGeometry.push_back(1.0);
		orbitCPUGeometry.push_back(1.0);
		orbitCPUGeometry.push_back(1.0);
	}

	//generate a new VertexArrayObject. This describes our BufferLayout
	glGenVertexArrays(1, &orbitVertexArrayObject);
	//bind it to be our active VAO
	glBindVertexArray(orbitVertexArrayObject);

	//generate a new VertexBufferObject. This will become the GPU representation of our CPU data
	glGenBuffers(1, &orbitVertexBufferObject);
	//bind it to be our active VBO
	glBindBuffer(GL_ARRAY_BUFFER, orbitVertexBufferObject);
	//---------------

	//load the data of our CPU buffer via the handle we just created into our GPU Buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*orbitCPUGeometry.size(), &(orbitCPUGeometry[0]), GL_STATIC_DRAW);


	//DEFINE POSITION IN LAYOUT
	//enable vertex attribute at location 0 (compare with vertex shader input)
	glEnableVertexAttribArray(0); //this will describe our position

	//specify where to read the data for attribute at location 0 
	glVertexAttribPointer(0, //first attribute
		3, //how many components for the position?
		GL_FLOAT, //datatype 
		GL_FALSE, //should the data be normalized?
		sizeof(float)* 6, //size of whole vertex attribute stride for one primitive (3 * float for position + 3 * float for color)
		(GLvoid*)(0)); //offset in stride: position is begins at byte 0 in the stride. expected to be cast to GLvoid*

	//DEFINE COLOR IN LAYOUT
	//enable vertex attribute at location 1 (compare with vertex shader input)
	glEnableVertexAttribArray(1); //this will describe our position

	//specify where to read the data for attribute at location 0 
	glVertexAttribPointer(1, //second attribute
		3, //how many components for the color?
		GL_FLOAT, //datatype 
		GL_FALSE, //should the data be normalized?
		sizeof(float)* 6, //size of whole vertex attribute stride for one primitive (3 * float for position + 3 * float for color)
		(GLvoid*)(sizeof(float)* 3)); //offset in stride: color starts after 3 * sizeof(float) bytes 



	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


/////////////////////////////////////////////////////////////////////////////////////////

void loadTextures()
{
	TextureLoader::loadImageToGLTexture(sunColorTexture, "../../../data/textures/sunmapSaturated2.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(earthColorTexture, "../../../data/textures/earthmap1k.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(marsColorTexture, "../../../data/textures/mars_1k_color.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(jupiterColorTexture , "../../../data/textures/jupitermap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(uranusColorTexture, "../../../data/textures/uranusmap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(saturnColorTexture, "../../../data/textures/saturnmap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(venusColorTexture, "../../../data/textures/venusmap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(mercuryColorTexture, "../../../data/textures/mercurymap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(neptuneColorTexture, "../../../data/textures/neptunemap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(moonColorTexture, "../../../data/textures/moonmap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(skySphereColorTexture, "../../../data/textures/universe.jpg", GL_RGB8, GL_TEXTURE0);


	TextureLoader::loadImageToGLTexture(sunNormalTexture, "../../../data/textures/sunmapSaturated2_bumpmap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(earthNormalTexture, "../../../data/textures/earthmap1k_bumpmap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(marsNormalTexture, "../../../data/textures/mars_1k_color_bumpmap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(jupiterNormalTexture , "../../../data/textures/jupitermap_bumpmap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(uranusNormalTexture, "../../../data/textures/uranusmap_bumpmap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(saturnNormalTexture, "../../../data/textures/saturnmap_bumpmap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(venusNormalTexture, "../../../data/textures/venusmap_bumpmap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(mercuryNormalTexture, "../../../data/textures/mercurymap_bumpmap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(neptuneNormalTexture, "../../../data/textures/neptunemap_bumpmap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(moonNormalTexture, "../../../data/textures/moonmap_bumpmap.jpg", GL_RGB8, GL_TEXTURE0);
	TextureLoader::loadImageToGLTexture(skySphereNormalTexture, "../../../data/textures/universe_bumpmap.jpg", GL_RGB8, GL_TEXTURE0);

}

/////////////////////////////////////////////////////////////////////////////////////////

void cleanup()
{
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);

	glDeleteBuffers(1, &vertexBufferObject);
	glDeleteBuffers(1, &elementArrayBuffer);

	glDeleteVertexArrays(1, &vertexArrayObject);
}


/////////////////////////////////////////////////////////////////////////////////////////


void idleFunction(void)
{
	glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////////////////////////


void resizeFunction(int Width, int Height)
{
	windowWidth = Width;
	windowHeight = Height;
	glViewport(0, 0, windowWidth, windowHeight);
	//create a projection matrix 
	glm::mat4 projectionMatrix = glm::perspective(90.0f, //FOV 60.0°
		(float)windowWidth / windowHeight, //aspect ratio of the projection
		0.10f, //near clipping plane
		1600.0f); //far clipping plane

	glUseProgram(shaderProgram); //bind shader program
	//upload projection matrix to the shader.
	glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix)); //upload projection matrix to bound shader
	glUseProgram(0); //unbind shader program

	//##also upload projection matrix for our newly created star shader
	glUseProgram(starOrbitShaderProgram);
	glUniformMatrix4fv(starOrbitShaderProjectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUseProgram(0);
	resizeFBOTextures();

}


/////////////////////////////////////////////////////////////////////////////////////////


void initWindow(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
		);

	glutInitWindowSize(windowWidth, windowHeight);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	windowHandle = glutCreateWindow("");

	if (windowHandle < 1)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a new rendering window.\n"
			);
		glutExit();
	}

	//GLUT function callbacks. This means: register functions which are called for the specific glut events. Mind the function signature!
	//??
	glutMouseFunc(mouseInput);
	//??
	glutSpecialFunc(specialKeyPress);
	//??
	glutSpecialUpFunc(specialKeyRelease);
	//??
	glutKeyboardFunc(keyPress);
	//??
	glutKeyboardUpFunc(keyRelease);

	//??
	glutTimerFunc(0, timerFunction, 0);
	//??
	glutReshapeFunc(resizeFunction);
	//??
	glutDisplayFunc(renderFunction);
	//??
	glutIdleFunc(idleFunction);
}

/////////////////////////////////////////////////////////////////////////////////////////

void initialize(int argc, char* argv[])
{

	srand(time(0));

	GLenum GlewInitResult;

	initWindow(argc, argv);

	glewExperimental = GL_TRUE;
	//initialize glew for extensions
	GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		fprintf(
			stderr,
			"ERROR: %s\n",
			glewGetErrorString(GlewInitResult)
			);
		glutExit();
	}

	fprintf(
		stdout,
		"INFO: OpenGL Version: %s\n",
		glGetString(GL_VERSION)
		);

	glGetError();
	//set color to clear the frame buffer with
	glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//create shaders
	setupShader();

	//load model and fill buffer objects
	loadModel();
	//load all of our textures
	loadTextures();

	//create FBO 
	setupFrameBufferObjects();
	setupScreenQuad();
}


void drawPlanet(glm::mat4 const& model_matrix, glm::mat4 const& view_matrix, float* color)
{
	// transfer model matrix to shader by the associated uniform location
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));

	//calculate the normal transfomrations from modelview matrix
	glm::mat4 normalMatrix = glm::mat4(1.0f);
	normalMatrix = view_matrix*model_matrix;
	normalMatrix = glm::inverse(normalMatrix);
	normalMatrix = glm::transpose(normalMatrix);

	// transfer NormalMatrix for Geometry to Shaders
	glUniformMatrix4fv(normalMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));



	glUniform3fv(planetColorUniformLocation, 1, color);

	//bind the planet geometry (the VAO!)
	glBindVertexArray(vertexArrayObject);

	// draw object according to (the EAO!). Note, that the geometry type is triangles.
	glDrawElements(GL_TRIANGLES, mesh->getTriangles().size() * 3, GL_UNSIGNED_INT, 0);

}

void drawSun(glm::mat4 const& model_matrix, glm::mat4 const& view_matrix, float* color)
{
	// transfer model matrix to shader by the associated uniform location
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));

	//calculate the normal transfomrations from modelview matrix
	glm::mat4 normalMatrix = glm::mat4(1.0f);
	normalMatrix = glm::mat4(-1.0f) * view_matrix *  model_matrix;
	normalMatrix = glm::inverse(normalMatrix);
	normalMatrix = glm::transpose(normalMatrix);

	// transfer NormalMatrix for Geometry to Shaders
	glUniformMatrix4fv(normalMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));



	glUniform3fv(planetColorUniformLocation, 1, color);

	//bind the planet geometry (the VAO!)
	glBindVertexArray(vertexArrayObject);

	// draw object according to (the EAO!). Note, that the geometry type is triangles.
	glDrawElements(GL_TRIANGLES, mesh->getTriangles().size() * 3, GL_UNSIGNED_INT, 0);

}

void drawStars(glm::mat4 const& model_matrix, glm::mat4 const& view_matrix)
{
	//##
	glUniformMatrix4fv(starOrbitShaderModelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));

	//##bind star VertexArrayObject
	glBindVertexArray(starVertexArrayObject);

	//##start at Vertex 0 for the bound Geometry, render "numStars" vertices as GL_POINTS.
	glDrawArrays(GL_POINTS, 0, numStars);

}

void drawOrbit(glm::mat4 const& model_matrix, glm::mat4 const& view_matrix)
{
	//
	glUniformMatrix4fv(starOrbitShaderModelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));

	//bind orbit VertexArrayObject
	glBindVertexArray(orbitVertexArrayObject);

	//start at Vertex 0 for the bound Geometry, render "numVerticesInOrbit" vertices as GL_LINE_LOOP.
	glDrawArrays(GL_LINE_LOOP, 0, numVerticesInOrbit);

}

