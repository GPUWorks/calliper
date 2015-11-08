#include "cviewport.h"
#include <QtMath>
#include <QKeyEvent>
#include <QImage>
#include <QApplication>
#include "ccameralens.h"
#include "cvertexbundle.h"
#include "cdebugcube.h"
#include "mainwindow.h"
#include "cscene.h"

GLuint gTextureBuffer = 0;

const static GLfloat cube_vertices[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

const static GLfloat cube_vertices_interleaved[] = {
    -1.0f,-1.0f,-1.0f, 0,0,
    -1.0f,-1.0f, 1.0f, 1,0,
    -1.0f, 1.0f, 1.0f, 1,1,

    1.0f, 1.0f,-1.0f,  0,0,
    -1.0f,-1.0f,-1.0f, 1,0,
    -1.0f, 1.0f,-1.0f, 1,1,

    1.0f,-1.0f, 1.0f,  0,0,
    -1.0f,-1.0f,-1.0f, 1,0,
    1.0f,-1.0f,-1.0f,  1,1,

    1.0f, 1.0f,-1.0f,  0,0,
    1.0f,-1.0f,-1.0f,  1,1,
    -1.0f,-1.0f,-1.0f, 0,1,

    -1.0f,-1.0f,-1.0f, 0,0,
    -1.0f, 1.0f, 1.0f, 1,1,
    -1.0f, 1.0f,-1.0f, 0,1,

    1.0f,-1.0f, 1.0f,  0,0,
    -1.0f,-1.0f, 1.0f, 1,1,
    -1.0f,-1.0f,-1.0f, 0,1,

    -1.0f, 1.0f, 1.0f, 0,0,
    -1.0f,-1.0f, 1.0f, 1,0,
    1.0f,-1.0f, 1.0f,  1,1,

    1.0f, 1.0f, 1.0f, 0,0,
    1.0f,-1.0f,-1.0f, 1,0,
    1.0f, 1.0f,-1.0f, 1,1,

    1.0f,-1.0f,-1.0f, 0,0,
    1.0f, 1.0f, 1.0f, 1,1,
    1.0f,-1.0f, 1.0f, 0,1,

    1.0f, 1.0f, 1.0f,  0,0,
    1.0f, 1.0f,-1.0f,  1,0,
    -1.0f, 1.0f,-1.0f, 1,1,

    1.0f, 1.0f, 1.0f,  0,0,
    -1.0f, 1.0f,-1.0f, 1,1,
    -1.0f, 1.0f, 1.0f, 0,1,

    1.0f, 1.0f, 1.0f,  0,0,
    -1.0f, 1.0f, 1.0f, 1,1,
    1.0f,-1.0f, 1.0f,  0,1,
};

const static GLfloat g_color_buffer_data[] = {
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
};

const static GLfloat g_uv_buffer_data[] = {
    0,0,
    1,0,
    1,1, //
    
    0,0,
    1,0,
    1,1, //
    
    0,0,
    1,0,
    1,1, //
    
    0,0,
    1,1,
    0,1, //
    
    0,0,
    1,1,
    0,1, //
    
    0,0,
    1,1,
    0,1, //
    
    0,0,
    1,0,
    1,1, //
    
    0,0,
    1,0,
    1,1, //
    
    0,0,
    1,1,
    0,1, //
    
    0,0,
    1,0,
    1,1, //
    
    0,0,
    1,1,
    0,1, //
    
    0,0,
    1,1,
    0,1, //
};

#define CUBE_VERTICES_SIZE (sizeof(GLfloat) * 3 * 3 * 2 * 6)
#define CUBE_UV_SIZE (sizeof(GLfloat) * 2 * 3 * 2 * 6)

const static GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};

#define VERTEX_DATA_SIZE (9 * sizeof(GLfloat))

const char* vertexShader =
        "#version 330 core\n"
        "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
        "layout(location = 1) in vec2 vertexUV;\n"
        "out vec2 UV;\n"
        "uniform mat4 MVP;\n"
        "void main(){\n"
        "vec4 v = vec4(vertexPosition_modelspace,1);\n"
        "gl_Position = MVP * v;\n"
        "UV = vertexUV; }\n";

const char* fragmentShader =
        "#version 330 core\n"
        "in vec2 UV;\n"
        "out vec3 color;\n"
        "uniform sampler2D myTextureSampler;\n"
        "void main(){\n"
        "color = texture(myTextureSampler, UV).rgb; }\n";

QMatrix4x4 perspectiveMatrix(float fov, float aspectRatio, float nearPlane, float farPlane)
{
    float top = nearPlane * qTan((M_PI/180.0f) * (fov/2.0f));
    float bottom = -top;
    float right = top * aspectRatio;
    float left = -right;

    return QMatrix4x4((2.0f*nearPlane)/(right-left), 0, (right+left)/(right-left), 0,
                      0, (2.0f*nearPlane)/(top-bottom), (top+bottom)/(top-bottom), 0,
                      0, 0, -(farPlane+nearPlane)/(farPlane-nearPlane), -(2.0f*farPlane*nearPlane)/(farPlane-nearPlane),
                      0, 0, -1, 0);
}

QMatrix4x4 orthographicMatrix(float top, float bottom, float left, float right, float nearPlane, float farPlane)
{
    return QMatrix4x4((2.0f)/(right-left), 0, 0, -(right+left)/(right-left),
                      0, (2.0f)/(top-bottom), 0, -(top+bottom)/(top-bottom),
                      0, 0, -(2.0f)/(farPlane-nearPlane), -(farPlane+nearPlane)/(farPlane-nearPlane),
                      0, 0, 0, 1);
}

CViewport::CViewport(QWidget * parent, Qt::WindowFlags f) : QOpenGLWidget(parent, f), QOpenGLFunctions_3_2_Core()
{
    Top = 2;
    Bottom = -2;
    Left = -2;
    Right = 2;
    Near = 0.01f;
    Far = 10;

    setFocusPolicy(Qt::StrongFocus);
    useGlobalTexture = false;
    ctrlPressed = false;
    useCoordTransform = false;

    CCameraLens* l = camera.lens();
    l->setPlanes(Left, Right, Top, Bottom, Near, Far);
    l->setAspectRatio((float)width()/(float)height());
    l->setFieldOfView(45);
    debugCube = NULL;
    shaderProgram = NULL;
}

CViewport::~CViewport()
{
    makeCurrent();
    
    glDeleteTextures(1, &texturebuffer);
    //glDeleteProgram(ProgramID);
    delete shaderProgram;
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    delete debugCube;
    debugCube = NULL;
}

CVertexBundle* createTestCube()
{
    CVertexBundle* vd = new CVertexBundle();
    vd->setInterleavingFormat(COpenGLRenderer::FormatPositionUV);

    for (int i = 0; i < 3*2*6; i++)
    {
        vd->appendVertex(
                    QVector3D(cube_vertices[3*i], cube_vertices[(3*i)+1], cube_vertices[(3*i)+2]),
                    QVector2D(g_uv_buffer_data[2*i], g_uv_buffer_data[(2*i)+1]));
    }

    return vd;
}

void CViewport::initializeGL()
{
    initializeOpenGLFunctions();
    context()->setShareContext(QOpenGLContext::globalShareContext());   // Is this needed or is it automatic?

    glClearColor(0.0f, 0.58f, 1.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

#if 0
    debugCube = new CDebugCube();
    qDebug() << "Number of vertices:" << debugCube->vertexData()->vertexCount();

    // Generate a Vertex Array Object (VAO).
    // This stores the "state" of the attributes for vertices:
    // when you set up how position/colour/normal/etc. are specified
    // in the buffers, the VAO remembers this format in order to
    // easily re-apply it later. In this sense it's sort of like
    // a macro.
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Generate and populate a buffer of vertex position data.
    glGenBuffers(1, &vertexbuffer);                 // Generates a buffer and records its handle in vertexbuffer.
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);    // Sets the array buffer target to use this buffer.

    // Sets up the buffer currently bound to GL_ARRAY_BUFFER, which as above is the vertex buffer we generated.
    // The size is 9 * sizeof(float), to cater for all 9 vertices, and the data is initialised from
    // the static vertex data for the triangle. The drawing mode hint is set to static because the triangle
    // geometry will not change.
    //glBufferData(GL_ARRAY_BUFFER, VERTEX_DATA_SIZE, g_vertex_buffer_data, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, CUBE_VERTICES_SIZE, cube_vertices, GL_STATIC_DRAW);
    const CVertexBundle* vertexData = debugCube->vertexData();
    glBufferData(GL_ARRAY_BUFFER, vertexData->vertexDataSize(), vertexData->vertexConstData(), GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices_interleaved), cube_vertices_interleaved, GL_STATIC_DRAW);

    glGenBuffers(1, &IndexArrayID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexArrayID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexData->indexDataSize(), vertexData->indexConstData(), GL_STATIC_DRAW);

    // Colours too.
//    glGenBuffers(1, &colorbuffer);
//    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
//    glBufferData(GL_ARRAY_BUFFER, CUBE_VERTICES_SIZE, g_color_buffer_data, GL_STATIC_DRAW);

    QImage image(":/uvsample.png");
    const uchar* imgData = image.constBits();
    int numImagePixels = image.width() * image.height();

    
    // Load texture.
    // The format needs to be RGBA instead of ARGB.
    // Qt doesn't have this format, so we have to do it ourselves.
    qDebug() << "Texture image format:" << image.format() << "Number of pixels:" << numImagePixels;
    uint* rawImage = new uint[numImagePixels];
    for ( int i = 0; i < numImagePixels; i++ )
    {
        // Get a integer representing 4 bytes of image data.
        uint data = ((const uint*)imgData)[i];

        // Swap the bytes round.
        uchar* d = (uchar*)&data;
        uchar alpha = d[3];

        d[3] = d[2];
        d[2] = d[1];
        d[1] = d[0];
        d[0] = alpha;

        // Write the integer to our raw image buffer.
        //qDebug("Original color: 0x%08x Colour written: 0x%08x", ((const uint*)imgData)[i], data);
        rawImage[i] = data;
    }

    glGenTextures(1, &texturebuffer);
    glBindTexture(GL_TEXTURE_2D, texturebuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, rawImage);
    delete rawImage;
    
/*
    uchar* img = new uchar[4*3];
    img[0] = 0x00; img[1] = 0xff; img[2] = 0x00;
    img[3] = 0xff; img[4] = 0xff; img[5] = 0x00;
    img[6] = 0x00; img[7] = 0x00; img[8] = 0x00;
    img[9] = 0xff; img[10] = 0x00; img[11] = 0x00;
    glGenTextures(1, &textureBuffer);
    glBindTexture(GL_TEXTURE_2D, textureBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
    delete img;
    */

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // No longer needed!
    /*
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, CUBE_UV_SIZE, g_uv_buffer_data, GL_STATIC_DRAW);
    */

    /*
    // Record handles to our vertex and fragment shaders.
    VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    char const * VertexSourcePointer = vertexShader;

    // This function is a bit weird because it caters to multiple strings.
    // The third and fourth parameters are arrays of char pointers and ints,
    // and they both have the same number of elements (specified by parameter 2).
    // All of these strings are concatenated and become the shader source code.
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);

    // Compile!
    glCompileShader(VertexShaderID);

    // Get the results of the compilation in case of errors.
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        qDebug() << "Vertex shader compilation messages:" << &VertexShaderErrorMessage[0];
    }

    // Same game for fragment shader.
    char const * FragmentSourcePointer = fragmentShader;
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Check results.
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        qDebug() << "Fragment shader compilation messages:" << &FragmentShaderErrorMessage[0];
    }

    // Link the program
    ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        std::vector<char> ProgramErrorMessage(InfoLogLength);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        qDebug() << "Shader program linking messages:" <<  &ProgramErrorMessage[0];
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    glUseProgram(ProgramID);
    */

    shaderProgram = new QOpenGLShaderProgram();
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader);
    shaderProgram->link();
    shaderProgram->bind();
    locvVertex = shaderProgram->attributeLocation("vertexPosition_modelspace");
    locvUV = shaderProgram->attributeLocation("vertexUV");
    locvMatrix = shaderProgram->attributeLocation("MVP");
    locfUV = shaderProgram->attributeLocation("UV");
    locfTexture = shaderProgram->attributeLocation("myTextureSampler");
#endif
}

void CViewport::resizeGL(int w, int h)
{
    CCameraLens* l = camera.lens();
    l->setAspectRatio((float)width()/(float)height());
    update();
}


void CViewport::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 0
    /*QMatrix4x4 Projection = usePerspective ? perspectiveMatrix(45.0f, (float)width()/(float)height(), Near, Far)
                                           : orthographicMatrix(Top, Bottom, Left, Right, Near, Far);*/
    QMatrix4x4 Projection = camera.lens()->projectionMatrix();

    /*QMatrix4x4 View(1,0,0,camPos.x(),
                    0,1,0,camPos.y(),
                    0,0,1,camPos.z(),
                    0,0,0,1);*/

    QMatrix4x4 View = camera.worldToCamera();

    float deg = 30;
    float s45 = qSin(qRadiansToDegrees(deg));
    float c45 = qCos(qRadiansToDegrees(deg));
    QMatrix4x4 scaleDown(0.5f,0,0,0, 0,0.5f,0,0, 0,0,0.5f,0, 0,0,0,1);
    QMatrix4x4 rotateY(c45,0,s45,0, 0,1,0,0, -s45,0,c45,0, 0,0,0,1);
    QMatrix4x4 rotateX(1,0,0,0, 0,c45,-s45,0, 0,s45,c45,0, 0,0,0,1);
    QMatrix4x4 model = rotateX * rotateY * scaleDown;

    if ( useCoordTransform )
        MVP = Projection * CBasicCamera::coordsHammerToOpenGL() * View * model;
    else
        MVP = Projection * View * model;

    //MatrixID = glGetUniformLocation(ProgramID, "MVP");
    //textureID = glGetUniformLocation(ProgramID, "myTextureSampler");
    //glUniform1i(textureID, 0);
    glUniform1i(locfTexture, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturebuffer);

    // TODO: How do we actually set up the attribute info for QOpenGLShaderProgram?
    // It looks like it needs a pointer to the actual data, but that's already been uploaded.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
       COpenGLRenderer::Position, // attribute 0. No particular reason for 0, but must match the layout in the shader.
       COpenGLRenderer::attributeSize(COpenGLRenderer::Position)/sizeof(float),   // size
       GL_FLOAT,           // type
       GL_FALSE,           // normalized?
       COpenGLRenderer::interleavingFormatSize(COpenGLRenderer::FormatPositionUV),        // stride
       (void*)COpenGLRenderer::attributeOffset(COpenGLRenderer::FormatPositionUV, COpenGLRenderer::Position)  // array buffer offset
    );

    /*
    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    */

    glEnableVertexAttribArray(1);
    //glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        COpenGLRenderer::UV,  // attribute. No particular reason for 1, but must match the layout in the shader.
        COpenGLRenderer::attributeSize(COpenGLRenderer::UV)/sizeof(float),    // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        COpenGLRenderer::interleavingFormatSize(COpenGLRenderer::FormatPositionUV),         // stride
        (void*)COpenGLRenderer::attributeOffset(COpenGLRenderer::FormatPositionUV, COpenGLRenderer::UV)   // array buffer offset
    );

    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, MVP.constData());

    // Draw the triangle !
    //glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
    //glDrawArrays(GL_TRIANGLES, 0, debugCube->vertexData()->vertexCount()/*3 * 2 * 6*/);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexArrayID);
    glDrawElements(
         GL_TRIANGLES,      // mode
         debugCube->vertexData()->indexCount(),    // count
         GL_UNSIGNED_INT,   // type
         (void*)0           // element array buffer offset
     );

    glDisableVertexAttribArray(0);
#endif

    MainWindow* w = appMainWindow();
    CSceneObject* r = w->scene()->root();
    CSceneObject* c = r->findChild<CSceneObject*>("Debug Cube", Qt::FindDirectChildrenOnly);
    Q_ASSERT(c);
    appMainWindow()->renderer()->render(this, c, &camera);
}

void CViewport::keyReleaseEvent(QKeyEvent *e)
{
    switch ( e->key() )
    {
        case Qt::Key_Control:
        {
            ctrlPressed = false;
            qDebug() << "Control released.";
            return;
        }

        default:
        QOpenGLWidget::keyReleaseEvent(e);
        return;
    }
}

void CViewport::keyPressEvent(QKeyEvent *e)
{
    switch ( e->key() )
    {

    case Qt::Key_Control:
    {
        ctrlPressed = true;
        qDebug() << "Control pressed";
        return;
    }

    case Qt::Key_W:
    {
        if ( ctrlPressed )
        {
            camera.setPosition(camera.position() + QVector3D(0,0,-0.1f));
            qDebug() << "POSITION ONLY:" << camera.position();
        }
        else
        {
            camera.translateLocal(QVector3D(0,0,-0.1f));
            qDebug() << camera.position();
        }

        update();
        return;
    }

    case Qt::Key_S:
    {
        if ( ctrlPressed )
        {
            camera.setPosition(camera.position() + QVector3D(0,0,0.1f));
            qDebug() << "POSITION ONLY:" << camera.position();
        }
        else
        {
            camera.translateLocal(QVector3D(0,0,0.1f));
            qDebug() << camera.position();
        }

        update();
        return;
    }

    case Qt::Key_A:
    {
        if ( ctrlPressed )
        {
            camera.setPosition(camera.position() + QVector3D(-0.1f,0,0));
            qDebug() << "POSITION ONLY:" << camera.position();
        }
        else
        {
            camera.translateLocal(QVector3D(-0.1f,0,0));
            qDebug() << camera.position();
        }

        update();
        return;
    }

    case Qt::Key_D:
    {
        if ( ctrlPressed )
        {
            camera.setPosition(camera.position() + QVector3D(0.1f,0,0));
            qDebug() << "POSITION ONLY:" << camera.position();
        }
        else
        {
            camera.translateLocal(QVector3D(0.1f,0,0));
            qDebug() << camera.position();
        }

        update();
        return;
    }

    case Qt::Key_Q:
    {
        if ( ctrlPressed )
        {
            camera.setPosition(camera.position() + QVector3D(0,-0.1f,0));
            qDebug() << "POSITION ONLY:" << camera.position();
        }
        else
        {
            camera.translateLocal(QVector3D(0,-0.1f,0));
            qDebug() << camera.position();
        }

        update();
        return;
    }

    case Qt::Key_Z:
    {
        if ( ctrlPressed )
        {
            camera.setPosition(camera.position() + QVector3D(0,0.1f,0));
            qDebug() << "POSITION ONLY:" << camera.position();
        }
        else
        {
            camera.translateLocal(QVector3D(0,0.1f,0));
            qDebug() << camera.position();
        }

        update();
        return;
    }

    case Qt::Key_Space:
    {
        //usePerspective = !usePerspective;
        //qDebug() << "Use perspective:" << usePerspective;
        CCameraLens* l = camera.lens();
        if (l->type() == CCameraLens::Perspective)
        {
            l->setType(CCameraLens::Orthographic);
        }
        else
        {
            l->setType(CCameraLens::Perspective);
        }
        qDebug() << "Perspective:" << (l->type() == CCameraLens::Perspective);
        update();
        return;
    }

    case Qt::Key_P:
        useCoordTransform = !useCoordTransform;
        qDebug() << "Using co-ord trandform:" << useCoordTransform;
        update();
        return;

    default:
        QOpenGLWidget::keyPressEvent(e);
        return;
    }
}
