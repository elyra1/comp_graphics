#include "Common.h"

const char* VertexShaderSource = R"(
#version 330 core
#define VERT_POSITION 0
#define VERT_TEXCOORD 1
#define VERT_NORMAL 2

layout(location = VERT_POSITION) in vec3 position;
layout(location = VERT_TEXCOORD) in vec2 texcoord;
layout(location = VERT_NORMAL) in vec3 normal;

out Vertex {
    vec2 texcoord;
    vec3 normal;
    vec3 worldPos;
} Vert;

struct Transform {
    mat4 model;
    mat4 viewProjection;
    mat3 normal;
    vec3 viewPosition;
};

uniform Transform transform;

void main() {
    //переведём координаты вершины в мировую систему координат
    vec4 vertex = transform.model * vec4(position, 1.0);
    //переводим координаты вершины в однородные
    gl_Position = transform.viewProjection * vertex;

    //передадим во фрагментный шейдер некоторые параметры
    //передаёмтекстурные координаты
    Vert.texcoord = texcoord;
    //передаём нормаль в мировой системе координат
    Vert.normal = transform.normal * normal;

    Vert.worldPos = vertex.xyz;
}
)";

const char* FragShaderSource = R"(
#version 330 core
layout(location = 0) out vec4 color;

in Vertex {
    vec2 texcoord;
    vec3 normal;
    vec3 worldPos;
} Vert;

uniform struct Material {
    sampler2D texture;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    float shininess;
} material;

uniform struct PointLight {
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 attenuation;
} light;

uniform struct DirLight {
    vec3 direction;
    vec4 ambient;
    vec4 diffuse;
} dirLight;

uniform struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutoff;
    float exponent;
    vec4 diffuse;
    vec3 attenuation;
    vec4 specular;
} spotLight;

uniform int u_lightModel;
uniform vec3 u_viewPosition;

float calcDiffuse(vec3 normal, vec3 lightDir) {
    return max(dot(normal, lightDir), 0.0);
}

float calcMinnaert(vec3 normal, vec3 lightDir, vec3 viewDir, float k) {
    float ndotl = max(dot(normal, lightDir), 0.0);
    float ndotv = max(dot(normal, viewDir), 0.0);
    return pow(ndotl, 1.0 + k) * pow(1.0 - ndotv, 1.0 - k);
}

// Вспомогательная функция для получения скалярной яркости из vec4 цвета
float getIntensity(vec4 color) {
    return (color.r + color.g + color.b) / 3.0;
}

void main() {
    //нормализация полученных данных для коррекции интерполяции
    vec3 normal = normalize(Vert.normal);
    vec3 viewDir = normalize(u_viewPosition - Vert.worldPos);
    vec4 texColor = texture(material.texture, Vert.texcoord);


    
    // Точечный свет 
    vec3 lightVec = light.position.xyz - Vert.worldPos;
    float distance = length(lightVec); // Расстояние
    vec3 L_point = lightVec / distance; 
    
    float attenuation = 1.0 / (light.attenuation[0] + 
                               light.attenuation[1] * distance + 
                               light.attenuation[2] * distance * distance);

    // Направленный свет
    vec3 L_dir = normalize(-dirLight.direction);

    // Прожектор/фонарик
    vec3 L_spot = normalize(spotLight.position - Vert.worldPos);
    float distSpot = length(spotLight.position - Vert.worldPos);
    float spotEffect = dot(normalize(spotLight.direction), -L_spot);
    float spotIntensity = 0.0;
    if (spotEffect > spotLight.cutoff) {
        float angularAtt = pow(spotEffect, spotLight.exponent);
        float distAtt = 1.0 / (spotLight.attenuation[0] + spotLight.attenuation[1] * distSpot + spotLight.attenuation[2] * distSpot * distSpot);
        spotIntensity = angularAtt * distAtt;
    }

    vec3 finalColor = vec3(0.0);

    if (u_lightModel == 0) { // PHONG
        vec4 ambientAcc = vec4(0.0);
        vec4 diffuseAcc = vec4(0.0);
        vec4 specularAcc = vec4(0.0);

        ambientAcc += material.ambient * light.ambient * attenuation;
        diffuseAcc += light.diffuse * calcDiffuse(normal, L_point) * attenuation;
        specularAcc += light.specular * pow(max(dot(reflect(-L_point, normal), viewDir), 0.0), material.shininess) * attenuation;

        ambientAcc += material.ambient * dirLight.ambient;
        diffuseAcc += dirLight.diffuse * calcDiffuse(normal, L_dir);
        
        diffuseAcc += spotLight.diffuse * calcDiffuse(normal, L_spot) * spotIntensity;
        specularAcc += spotLight.specular * 
                   pow(max(dot(reflect(-L_spot, normal), viewDir), 0.0), material.shininess) * 
                   spotIntensity; 

        finalColor = (material.emission + (ambientAcc + diffuseAcc) * texColor + (specularAcc * material.specular)).rgb;
    } 
    else if (u_lightModel == 1) { // TOON
        // учитываем яркость диффузного компонента каждого источника
        float intensity = 0.0;
        
        intensity += calcDiffuse(normal, L_point) * attenuation * getIntensity(light.diffuse);
        intensity += calcDiffuse(normal, L_dir) * getIntensity(dirLight.diffuse);
        intensity += calcDiffuse(normal, L_spot) * spotIntensity * getIntensity(spotLight.diffuse);

        // Квантование
        float factor = 0.0;
        if (intensity > 0.95) {
            factor = 1.0;
        }
        else if (intensity > 0.5) {
            factor = 0.7;
        }
        else if (intensity > 0.2) {   
            factor = 0.35;
        }
        else {
            factor = 0.1;
        }
        
        // Учитываем Ambient и Emission, чтобы объект не был черным при наличии фонового света
        vec3 ambientPart = (material.ambient * light.ambient * attenuation + material.ambient * dirLight.ambient).rgb;
        finalColor = material.emission.rgb + texColor.rgb * (factor + ambientPart);
    } 
    else if (u_lightModel == 2) { // MINNAERT
        const float k = 0.8;
        vec3 lightingFactor = vec3(0.0);
        
        // умножаем результат Minnaert на цвет диффузного света
        lightingFactor += light.diffuse.rgb * calcMinnaert(normal, L_point, viewDir, k) * attenuation;
        lightingFactor += dirLight.diffuse.rgb * calcMinnaert(normal, L_dir, viewDir, k);
        lightingFactor += spotLight.diffuse.rgb * calcMinnaert(normal, L_spot, viewDir, k) * spotIntensity;

        finalColor = material.emission.rgb + texColor.rgb * lightingFactor;
    }

    color = vec4(finalColor, 1.0);
}
)";


void ShaderLog(unsigned int shader) {
    int infologLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
    if (infologLen > 1) {
        std::vector<char> infoLog(infologLen);
        glGetShaderInfoLog(shader, infologLen, NULL, infoLog.data());
        std::cout << "Shader Log: " << infoLog.data() << std::endl;
    }
}

bool LoadOBJ(const char* path, std::vector<Vertex>& out_vertices) {
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "CRITICAL: Could not open " << path << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;
        if (prefix == "v") {
            glm::vec3 v; ss >> v.x >> v.y >> v.z;
            temp_vertices.push_back(v);
        }
        else if (prefix == "vt") {
            glm::vec2 uv; ss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (prefix == "vn") {
            glm::vec3 n; ss >> n.x >> n.y >> n.z;
            temp_normals.push_back(n);
        }
        else if (prefix == "f") {
            std::string vertexData;
            std::vector<Vertex> face;
            while (ss >> vertexData) {
                int vIdx = 0, uvIdx = 0, nIdx = 0;
                if (sscanf(vertexData.c_str(), "%d/%d/%d", &vIdx, &uvIdx, &nIdx) != 3) {
                    if (sscanf(vertexData.c_str(), "%d//%d", &vIdx, &nIdx) != 2) {
                        sscanf(vertexData.c_str(), "%d/%d", &vIdx, &uvIdx);
                    }
                }

                Vertex v;
                if (vIdx > 0 && vIdx <= (int)temp_vertices.size()) v.position = temp_vertices[vIdx - 1];
                else v.position = glm::vec3(0, 0, 0);

                if (uvIdx > 0 && uvIdx <= (int)temp_uvs.size()) v.texCoord = temp_uvs[uvIdx - 1];
                else v.texCoord = glm::vec2(0, 0);

                if (nIdx > 0 && nIdx <= (int)temp_normals.size()) v.normal = temp_normals[nIdx - 1];
                else v.normal = glm::vec3(0, 1, 0);

                face.push_back(v);
            }
            for (size_t i = 1; i < face.size() - 1; i++) {
                out_vertices.push_back(face[0]);
                out_vertices.push_back(face[i]);
                out_vertices.push_back(face[i + 1]);
            }
        }
    }
    return !out_vertices.empty();
}

GLuint LoadTexture(const char* path) {
    sf::Image image;
    if (!image.loadFromFile(path)) {
        std::cerr << "WARNING: Texture not found: " << path << std::endl;
        return 0;
    }
    image.flipVertically();
    GLuint tid;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
    glGenerateMipmap(GL_TEXTURE_2D);
    return tid;
}

void AddObject(std::string name, const char* modelPath, const char* texPath,
    glm::vec3 pos, glm::vec3 scale, glm::vec3 rotDegrees,
    int modelType,
    glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, glm::vec4 emission, float shininess) {
    std::vector<Vertex> vertices;
    if (LoadOBJ(modelPath, vertices)) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        SceneObject obj;
        obj.vbo = vbo;
        obj.texture = LoadTexture(texPath);
        obj.vertexCount = (int)vertices.size();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::rotate(model, glm::radians(rotDegrees.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotDegrees.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotDegrees.z), glm::vec3(0, 0, 1));
        model = glm::scale(model, scale);

        obj.modelMatrix = model;
        obj.defaultModel = modelType;
        obj.name = name;

        obj.matAmbient = ambient;
        obj.matDiffuse = diffuse;
        obj.matSpecular = specular;
        obj.matEmission = emission;
        obj.matShininess = shininess;

        sceneObjects.push_back(obj);
        std::cout << "Successfully loaded: " << name << std::endl;
    }
    else {
        std::cerr << "Failed to load model: " << modelPath << std::endl;
    }
}

GLuint CreateProgram(const char* vSrc, const char* fSrc) {
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vSrc, NULL);
    glCompileShader(vShader);
    ShaderLog(vShader);

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fSrc, NULL);
    glCompileShader(fShader);
    ShaderLog(fShader);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vShader);
    glAttachShader(prog, fShader);
    glLinkProgram(prog);
    return prog;
}