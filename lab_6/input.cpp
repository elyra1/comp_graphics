#include "input.h"
#include "renderer.h"
#include <iostream>
#include <variant>

void applyTransformation(const Matrix4x4 &transformation)
{
    currentTransformation = transformation * currentTransformation;
}

void applyTransformationAroundPolyhedronCenter(const Matrix4x4 &transformation)
{
    Point3D center = currentTransformation.transform(currentPolyhedron.getCenter());
    Matrix4x4 T = createTranslationMatrix(-center.x, -center.y, -center.z);
    Matrix4x4 T_inv = createTranslationMatrix(center.x, center.y, center.z);
    applyTransformation(T_inv * transformation * T);
}

void handleEvent(const sf::Event &event)
{
    if (const auto* eventDetails = event.getIf<sf::Event::KeyPressed>())
    {
        bool isShiftPressed = eventDetails->shift;

        switch (eventDetails->code)
        {
        // Выбор фигур
        case sf::Keyboard::Key::Num1:
            currentPolyhedron = createTetrahedron();
            break;
        case sf::Keyboard::Key::Num2:
            currentPolyhedron = createHexahedron();
            break;
        case sf::Keyboard::Key::Num3:
            currentPolyhedron = createOctahedron();
            break;
        case sf::Keyboard::Key::Num4:
            currentPolyhedron = createIcosahedron();
            break;
        case sf::Keyboard::Key::Num5:
            currentPolyhedron = createDodecahedron();
            break;

        // Аффинные преобразования
        case sf::Keyboard::Key::T:
            applyTransformation(isShiftPressed ? createTranslationMatrix(-0.5, 0, 0) : createTranslationMatrix(0.5, 0, 0));
            break;
        case sf::Keyboard::Key::G:
            applyTransformation(isShiftPressed ? createTranslationMatrix(0, -0.5, 0) : createTranslationMatrix(0, 0.5, 0));
            break;
        case sf::Keyboard::Key::H:
            applyTransformation(isShiftPressed ? createTranslationMatrix(0, 0, -0.5) : createTranslationMatrix(0, 0, 0.5));
            break;
        case sf::Keyboard::Key::S:
            applyTransformation(isShiftPressed ? createScaleMatrix(0.8, 0.8, 0.8) : createScaleMatrix(1.2, 1.2, 1.2));
            break;
        case sf::Keyboard::Key::X:
            applyTransformation(isShiftPressed ? createRotationXMatrix(-15) : createRotationXMatrix(15));
            break;
        case sf::Keyboard::Key::Y:
            applyTransformation(isShiftPressed ? createRotationYMatrix(-15) : createRotationYMatrix(15));
            break;
        case sf::Keyboard::Key::Z:
            applyTransformation(isShiftPressed ? createRotationZMatrix(-15) : createRotationZMatrix(15));
            break;
        case sf::Keyboard::Key::A:
        {
            Point3D p1(1, 1, 1);
            Point3D p2(-1, -1, -1);
            applyTransformation(createArbitraryRotationMatrix(p1, p2, isShiftPressed ? -15.0 : 15.0));
            break;
        }

        // Отражение
        case sf::Keyboard::Key::M:
            applyTransformation(createReflectionMatrix(ReflectionPlane::X));
            break;
        case sf::Keyboard::Key::N:
            applyTransformation(createReflectionMatrix(ReflectionPlane::Y));
            break;
        case sf::Keyboard::Key::B:
            applyTransformation(createReflectionMatrix(ReflectionPlane::Z));
            break;

        // Масштабирование относительно центра
        case sf::Keyboard::Key::C:
        {
            applyTransformationAroundPolyhedronCenter(createScaleMatrix(isShiftPressed ? 0.7 : 1.5, isShiftPressed ? 0.7 : 1.5, isShiftPressed ? 0.7 : 1.5));
            break;
        }

        // Вращение относительно центра
        case sf::Keyboard::Key::J:
        {
            applyTransformationAroundPolyhedronCenter(createRotationXMatrix(isShiftPressed ? -15 : 15));
            break;
        }
        case sf::Keyboard::Key::K:
        {
            applyTransformationAroundPolyhedronCenter(createRotationYMatrix(isShiftPressed ? -15 : 15));
            break;
        }
        case sf::Keyboard::Key::L:
        {
            applyTransformationAroundPolyhedronCenter(createRotationZMatrix(isShiftPressed ? -15 : 15));
            break;
        }

        // Проекции
        case sf::Keyboard::Key::P:
            perspectiveProjection = !perspectiveProjection;
            break;

        // Сброс
        case sf::Keyboard::Key::R:
            rotationX = rotationY = 0.0f;
            currentTransformation.identity();
            break;

        // Вращение камеры
        case sf::Keyboard::Key::Left:
            rotationY -= 5.0f;
            break;
        case sf::Keyboard::Key::Right:
            rotationY += 5.0f;
            break;
        case sf::Keyboard::Key::Up:
            rotationX -= 5.0f;
            break;
        case sf::Keyboard::Key::Down:
            rotationX += 5.0f;
            break;

        default:
            break;
        }
    }
}