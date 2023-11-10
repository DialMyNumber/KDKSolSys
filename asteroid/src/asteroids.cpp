#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
//#include <learnopengl/filesystem.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
//unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

bool stopOrbit = false;
int setOrbit;

int lotatespeed = 100;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(300.0f, 25.0f, 25.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);    // MSAA

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader asteroidShader("shader/asteroids.vs", "shader/asteroids.fs");
    Shader skyboxShader("shader/skybox.vs", "shader/skybox.fs");
    Shader lightingShader("shader/light.vs", "shader/light.fs");
    /* earth's cloud 
    Shader cloudShader("shader/cloud.vs", "shader/cloud.fs");
    */
    /* geometry orbit
    Shader geoShader("geometry_shader.vs", "geometry_shader.fs", "geometry_shader.gs");
    */
    // load models
    // -----------
    Model rock("resources/objects/rock/rock.obj");
    Model sunplanet("resources/objects/planet/sun/planet.obj");
    Model mercuryplanet("resources/objects/planet/mercury/planet.obj");
    Model venusplanet("resources/objects/planet/venus/planet.obj");
    Model earthplanet("resources/objects/planet/earth_photorealistic_2k/Earth 2K.obj");
    Model nightearthplanet("resources/objects/planet/nightearth_photorealistic_2k/Earth 2K.obj");
    //Model cloudearthplanet("resources/objects/planet/cloudearth_photorealistic_2k/Earth 2K.obj");
    //unsigned int cloudTexture = loadTexture(FileSystem::getPath("resources/objects/planet/cloudearth_photorealistic_2k/Clouds_2K.jpg").c_str());
    Model moonplanet("resources/objects/planet/moonE/PlanetsOBJ.obj");
    Model marsplanet("resources/objects/planet/mars/planet.obj");
    Model jupiterplanet("resources/objects/planet/jupiterE/13905_Jupiter_V1_l3.obj");
    Model saturnplanet("resources/objects/planet/saturnEE/3136216_Saturn.obj");
    Model uranusplanet("resources/objects/planet/uranusE/13907_Uranus_v2_l3.obj");
    Model neptuneplanet("resources/objects/planet/neptuneE/13908_Neptune_V2_l3.obj");




    /* geometry orbit
    float points[] = {
        // coordiantes   //     color
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
    };
    unsigned int orbitVBO, orbitVAO;
    glGenBuffers(1, &orbitVBO);
    glGenVertexArrays(1, &orbitVAO);
    glBindVertexArray(orbitVAO);
    glBindBuffer(GL_ARRAY_BUFFER, orbitVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
    */




    // generate a large list of semi-random model transformation matrices
    // ------------------------------------------------------------------
    unsigned int amount = 1500;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    float radius = 150;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = static_cast<float>((rand() % 360));
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }
    
    // configure here
    // configure instanced array
    // -------------------------
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    // set transformation matrices as an instance vertex attribute (with divisor 1)
    // note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
    // normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
    // -----------------------------------------------------------------------------------------------------------------------------------
    for (unsigned int i = 0; i < rock.meshes.size(); i++)
    {
        unsigned int VAO = rock.meshes[i].VAO;
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // load skybox's textures
    // -------------
    vector<std::string> faces
    {
        "resources/skybox/px.jpg",  // right = px
        "resources/skybox/nx.jpg",  // left = nx
        "resources/skybox/py.jpg",  // top = py
        "resources/skybox/ny.jpg",  // bottom = ny
        "resources/skybox/pz.jpg",  // front = pz
        "resources/skybox/nz.jpg"   // back = nz
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    // shader configuration
    // --------------------
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    glEnable(GL_MULTISAMPLE);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
        glm::mat4 view = camera.GetViewMatrix();;
        asteroidShader.use();
        asteroidShader.setMat4("projection", projection);
        asteroidShader.setMat4("view", view);

        // check Orbit
        if (stopOrbit) {
            setOrbit = 10000;
        }
        else {
            setOrbit = 1;
        }

        if (lotatespeed < 10)
            lotatespeed = 10;
        if (lotatespeed > 2000)
            lotatespeed = 2000;

        /*in Linux, use fork() to get current count
        #include <unistd.h>
        pid = fork();
        count = 0;
        if(pid == 0) {
            count++;
            sleep(1);   // because, sleep() stops all process
        }
        use "glfwGetTime()-count" to present correct location  
        */


        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("light.position", lightPos);
        lightingShader.setVec3("light.direction", camera.Front);
        lightingShader.setVec3("viewPos", camera.Position);
        // light properties
        lightingShader.setVec3("light.diffuse", 0.9f, 0.9f, 0.9f);
        lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("shininess", 2.0f);
        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
        view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // draw planet

        lightingShader.setVec3("light.ambient", 1.6f, 1.6f, 1.6f); // lightPos is inside of sun
        glm::mat4 sunmodel = glm::mat4(1.0f);
        sunmodel = glm::translate(sunmodel, glm::vec3(0.0f, 0.0f, 0.0f));
        sunmodel = glm::rotate(sunmodel, (float)glm::radians(glfwGetTime() * 10), glm::vec3(0.0f, 1.0f, 0.0f));
        sunmodel = glm::scale(sunmodel, glm::vec3(4.0f, 4.0f, 4.0f));
        lightingShader.setMat4("model", sunmodel);
        sunplanet.Draw(lightingShader);

        lightingShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
        lightingShader.setFloat("shininess", 32.0f);

        glm::mat4 mercurymodel = glm::mat4(1.0f);
        mercurymodel = glm::translate(mercurymodel, glm::vec3(25.0f * glm::cos(glfwGetTime() / setOrbit), 0.0f, -25.0f * glm::sin(glfwGetTime() / setOrbit)));
        mercurymodel = glm::rotate(mercurymodel, (float)glm::radians(glfwGetTime() * 20 + lotatespeed), glm::vec3(0.0f, 1.0f, 0.0f));
        mercurymodel = glm::scale(mercurymodel, glm::vec3(0.25f, 0.25f, 0.25f));
        lightingShader.setMat4("model", mercurymodel);
        mercuryplanet.Draw(lightingShader);

        glm::mat4 venusmodel = glm::mat4(1.0f);
        venusmodel = glm::translate(venusmodel, glm::vec3(40.0f * glm::cos(glfwGetTime() / (2 * setOrbit)), 0.0f, -40.0f * glm::sin(glfwGetTime() / (2 * setOrbit))));
        venusmodel = glm::rotate(venusmodel, (float)glm::radians(glfwGetTime() * 10 + lotatespeed), glm::vec3(0.0f, 1.0f, 0.0f));
        venusmodel = glm::scale(venusmodel, glm::vec3(0.6f, 0.6f, 0.6f));
        lightingShader.setMat4("model", venusmodel);
        venusplanet.Draw(lightingShader);

        glm::mat4 earthmodel = glm::mat4(1.0f);
        earthmodel = glm::translate(earthmodel, glm::vec3(75.0f * glm::cos(glfwGetTime() / (3 * setOrbit)), 0.0f, -75.0f * glm::sin(glfwGetTime() / (3 * setOrbit))));
        earthmodel = glm::rotate(earthmodel, (float)glm::radians(glfwGetTime() * 20 + lotatespeed), glm::vec3(-0.2f, 1.0f, -0.2f));
        earthmodel = glm::scale(earthmodel, glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setMat4("model", earthmodel);
        earthplanet.Draw(lightingShader);

        int cosnightshift;
        int sinnightshift;
        if (stopOrbit) {
            cosnightshift = 1;
            sinnightshift = -1;
        }
        else {
            if (glm::sin(glfwGetTime()) >= 0) {
                if (glm::cos(glfwGetTime()) >= 0) {
                    cosnightshift = 1;
                    sinnightshift = 1;
                }   // 1st quad
                else {
                    cosnightshift = -1;
                    sinnightshift = 1;
                }   // 2nd quad
            }
            else {
                if (glm::cos(glfwGetTime()) < 0) {
                    cosnightshift = -1;
                    sinnightshift = -1;
                }   // 3rd quad
                else {
                    cosnightshift = 1;
                    sinnightshift = -1;
                }   // 4th quad
            }
        }
        lightingShader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
        glm::mat4 nightearthmodel = glm::mat4(1.0f);
        nightearthmodel = glm::translate(nightearthmodel, glm::vec3(75.0f * glm::cos(glfwGetTime() / (3 * setOrbit)) + 0.1f * cosnightshift, 0.0f, -75.0f * glm::sin(glfwGetTime() / (3 * setOrbit)) + 0.05f * sinnightshift));
        nightearthmodel = glm::rotate(nightearthmodel, (float)glm::radians(glfwGetTime()*20 + lotatespeed), glm::vec3(-0.2f, 1.0f, -0.2f));
        nightearthmodel = glm::scale(nightearthmodel, glm::vec3(0.99f, 0.99f, 0.99f));
        lightingShader.setMat4("model", nightearthmodel);
        nightearthplanet.Draw(lightingShader);

        /* cloud shader
        glm::mat4 cloudearthmodel = glm::mat4(1.0f);
        cloudearthmodel = glm::translate(cloudearthmodel, glm::vec3(75.0f * glm::cos(glfwGetTime() / (3 * setOrbit)), 0.0f, -75.0f * glm::sin(glfwGetTime() / (3 * setOrbit))));
        cloudearthmodel = glm::rotate(cloudearthmodel, (float)glm::radians(glfwGetTime() * 20 + lotatespeed), glm::vec3(-0.2f, 1.0f, -0.2f));
        cloudearthmodel = glm::scale(cloudearthmodel, glm::vec3(1.05f, 1.05f, 1.05f));
        cloudShader.setMat4("model", cloudearthmodel);
        cloudearthplanet.Draw(cloudShader);
        */

        lightingShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);

        glm::mat4 moonmodel = glm::mat4(1.0f);
        moonmodel = glm::translate(moonmodel, glm::vec3(75.0f * glm::cos(glfwGetTime() / (3 * setOrbit)), 0.0f, -75.0f * glm::sin(glfwGetTime() / (3 * setOrbit))));
        moonmodel = glm::rotate(moonmodel, (float)glm::radians(glfwGetTime() * 50 + lotatespeed), glm::vec3(0.0f, 1.0f, 0.0f));
        moonmodel = glm::scale(moonmodel, glm::vec3(2.0f, 2.0f, 2.0f));
        lightingShader.setMat4("model", moonmodel);
        moonplanet.Draw(lightingShader);

        glm::mat4 marsmodel = glm::mat4(1.0f);
        marsmodel = glm::translate(marsmodel, glm::vec3(110.0f * glm::cos(glfwGetTime() / (3 * setOrbit)), 0.0f, -110.0f * glm::sin(glfwGetTime() / (3 * setOrbit))));
        marsmodel = glm::rotate(marsmodel, (float)30, glm::vec3(-0.25f, 0.0f, 0.0f));
        marsmodel = glm::rotate(marsmodel, (float)glm::radians(glfwGetTime()*-20 - lotatespeed), glm::vec3(0.0f, 0.0f, 0.05f));
        marsmodel = glm::scale(marsmodel, glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setMat4("model", marsmodel);
        marsplanet.Draw(lightingShader);

        glm::mat4 jupitermodel = glm::mat4(1.0f);
        jupitermodel = glm::translate(jupitermodel, glm::vec3(230.0f * glm::cos(glfwGetTime() / (6 * setOrbit)), 0.0f, -230.0f * glm::sin(glfwGetTime() / (6 * setOrbit))));
        jupitermodel = glm::rotate(jupitermodel, (float)glm::radians(90.0), glm::vec3(1.0f, 0.0f, 0.0f));
        jupitermodel = glm::rotate(jupitermodel, -(float)glm::radians(glfwGetTime()*100 + lotatespeed), glm::vec3(0.0f, 0.0f, 1.0f));
        jupitermodel = glm::scale(jupitermodel, glm::vec3(0.025f, 0.025f, 0.025f));
        lightingShader.setMat4("model", jupitermodel);
        jupiterplanet.Draw(lightingShader);

        glm::mat4 saturnmodel = glm::mat4(1.0f);
        saturnmodel = glm::translate(saturnmodel, glm::vec3(300.0f * glm::cos(glfwGetTime() / (8 * setOrbit)), 0.0f, -270.0f * glm::sin(glfwGetTime() / (8 * setOrbit))));
        saturnmodel = glm::rotate(saturnmodel, (float)glm::radians(-15.0), glm::vec3(-0.1f, 0.9f, 0.0f));
        saturnmodel = glm::rotate(saturnmodel, (float)glm::radians(glfwGetTime() + lotatespeed), glm::vec3(0.0f, 1.0f, 0.0f));
        saturnmodel = glm::scale(saturnmodel, glm::vec3(10.0f, 10.0f, 10.0f));
        lightingShader.setMat4("model", saturnmodel);
        saturnplanet.Draw(lightingShader);

        glm::mat4 uranusmodel = glm::mat4(1.0f);
        uranusmodel = glm::translate(uranusmodel, glm::vec3(350.0f * glm::cos(glfwGetTime() / (9 * setOrbit)), 0.0f, -370.0f * glm::sin(glfwGetTime() / (9 * setOrbit))));
        uranusmodel = glm::rotate(uranusmodel, (float)glm::radians(glfwGetTime()*5 + lotatespeed), glm::vec3(1.0f, 0.0f, 0.0f));
        uranusmodel = glm::scale(uranusmodel, glm::vec3(0.05f, 0.05f, 0.05f));
        lightingShader.setMat4("model", uranusmodel);
        uranusplanet.Draw(lightingShader);

        glm::mat4 neptunemodel = glm::mat4(1.0f);
        neptunemodel = glm::translate(neptunemodel, glm::vec3(410.0f * glm::cos(glfwGetTime() / (12 * setOrbit)), 0.0f, -410.0f * glm::sin(glfwGetTime() / (12 * setOrbit))));
        neptunemodel = glm::rotate(neptunemodel, (float)glm::radians(90.0), glm::vec3(1.0f, 0.0f, 0.0f));
        neptunemodel = glm::rotate(neptunemodel, (float)glm::radians(glfwGetTime()*-5 - lotatespeed), glm::vec3(0.0f, 0.0f, 1.0f));
        neptunemodel = glm::scale(neptunemodel, glm::vec3(0.05f, 0.05f, 0.05f));
        lightingShader.setMat4("model", neptunemodel);
        neptuneplanet.Draw(lightingShader);

        asteroidShader.use();
        asteroidShader.setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
        for (unsigned int i = 0; i < rock.meshes.size(); i++)
        {
            glBindVertexArray(rock.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rock.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);
            glBindVertexArray(0);
        }

        /* geometry orbit
        geoShader.use();
        glBindVertexArray(orbitVAO);
        glDrawArrays(GL_POINTS, 0, 1);
        */

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    /* geometry orbit
    glDeleteVertexArrays(1, &orbitVAO);
    glDeleteBuffers(1, &orbitVBO);
    */
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        stopOrbit = true;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        stopOrbit = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        lotatespeed -= 1;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        lotatespeed += 1;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

/*
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
*/