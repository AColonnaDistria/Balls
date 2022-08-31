#include <iostream>
#include <cmath>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>

int myrand(int min, int max)
{
    int value = rand() % (max - min) + min;

    if (rand() % 2)
    {
        value = -value;
    }
    return value;
}

int main()
{
    //std::ofstream foutput("output.csv");
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(), "Balls", sf::Style::Fullscreen);
    sf::Mouse mouse;

    sf::Text labelSpeed;

    sf::Font font;
    font.loadFromFile("arial.ttf");

    labelSpeed.setFont(font);
    labelSpeed.setCharacterSize(30);
    labelSpeed.setPosition(25.f, 25.f);
    labelSpeed.setFillColor(sf::Color::White);

    std::vector<sf::CircleShape> shapes;
    std::vector<sf::Vector2f> speeds;

    sf::CircleShape mouseShape(5.f);
    mouseShape.setPosition(mouse.getPosition().x, mouse.getPosition().y);
    mouseShape.setFillColor(sf::Color::Red);

    int number = 50;
    float gravity = 0.0001f;
    float friction = 0.00001f;
    float resistance = 0.000001f;

    float averageSpeedInitial = 0.0f;
    for (int i = 0; i < number; ++i)
    {
        shapes.push_back(sf::CircleShape((rand() % 20 + 20) * 0.2f));

        shapes[i].setPosition(rand() % window.getSize().x, rand() % window.getSize().y);
        shapes[i].setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));

        speeds.push_back(sf::Vector2f(myrand(5, 10) * 0.01f, (myrand(5, 10) % 20 - 10) * 0.01f));
        averageSpeedInitial += sqrt(speeds[i].x * speeds[i].x + speeds[i].y * speeds[i].y);
    }
    averageSpeedInitial /= number;

    while (window.isOpen())
    {
        sf::Event my_event;
        while (window.pollEvent(my_event))
        {
            if (my_event.type == sf::Event::Closed)
            {
                //foutput.close();
                window.close();
            }
        }

        int collisions = 0;
        for (int i = 0; i < shapes.size(); ++i)
        {
            auto& shape = shapes[i];
            auto& speed = speeds[i];

            shape.setPosition(shape.getPosition() + speed);
            // gravity
            speed.y += gravity;

            speed.x *= (1.f - resistance);
            speed.y *= (1.f - resistance);

            if (shape.getPosition().x <= 0)
            {
                shape.setPosition(0.f, shape.getPosition().y);
                speed.x = -speed.x;
                speed.x *= (1.0f - friction);
                ++collisions;
            }
            if (shape.getPosition().x >= window.getDefaultView().getSize().x - shape.getRadius() * 2.f)
            {
                shape.setPosition(window.getDefaultView().getSize().x - shape.getRadius() * 2.f, shape.getPosition().y);
                speed.x = -speed.x;
                speed.x *= (1.0f - friction);
                ++collisions;
            }

            if (shape.getPosition().y <= 0)
            {
                shape.setPosition(shape.getPosition().x, 0.f);
                speed.y = -speed.y;
                speed.y *= (1.0f - friction);
                ++collisions;
            }
            if (shape.getPosition().y >= window.getDefaultView().getSize().y - shape.getRadius() * 2.f)
            {
                shape.setPosition(shape.getPosition().x, window.getDefaultView().getSize().y - shape.getRadius() * 2.f);
                speed.y = -speed.y;
                speed.y *= (1.0f - friction);
                ++collisions;
            }
        }

        // check collision
        for (int i = 0; i < shapes.size(); ++i)
        {
            auto& shape1 = shapes[i];

            for (int j = i + 1; j < shapes.size(); ++j)
            {
                auto& shape2 = shapes[j];

                auto midpoint1 = shape1.getPosition() + sf::Vector2f(shape1.getRadius(), shape1.getRadius());
                auto midpoint2 = shape2.getPosition() + sf::Vector2f(shape2.getRadius(), shape2.getRadius());

                auto distanceSq = (midpoint1.x - midpoint2.x) * (midpoint1.x - midpoint2.x) + (midpoint1.y - midpoint2.y) * (midpoint1.y - midpoint2.y);

                if (distanceSq <= (shape1.getRadius() + shape2.getRadius()) * (shape1.getRadius() + shape2.getRadius()))
                {
                    // collision

                    /*
                    auto collisionVector = (midpoint1 - midpoint2);
                    auto norm = sqrt((collisionVector.x * collisionVector.x) + (collisionVector.y * collisionVector.y));
                    collisionVector.x /= norm;
                    collisionVector.y /= norm;

                    speeds[i] = collisionVector * sqrt((speeds[i].x * speeds[i].x) + (speeds[i].y * speeds[i].y)) * (1.f - friction);
                    speeds[j] = -collisionVector * sqrt((speeds[j].x * speeds[j].x) + (speeds[j].y * speeds[j].y)) * (1.f - friction);
                    */

                    float m1 = shape1.getRadius() / 2.f;
                    float m2 = shape2.getRadius() / 2.f;

                    float tempSpeed1_x = speeds[i].x;
                    float tempSpeed1_y = speeds[i].y;

                    speeds[i].x = ((m1 - m2) / (m1 + m2) * speeds[i].x + (2 * m2) / (m1 + m2) * speeds[j].x) * (1.f - friction);
                    speeds[i].y = ((m1 - m2) / (m1 + m2) * speeds[i].y + (2 * m2) / (m1 + m2) * speeds[j].y) * (1.f - friction);

                    speeds[j].x = ((m2 - m1) / (m1 + m2) * speeds[j].x + (2 * m1) / (m1 + m2) * tempSpeed1_x) * (1.f - friction);
                    speeds[j].y = ((m2 - m1) / (m1 + m2) * speeds[j].y + (2 * m1) / (m1 + m2) * tempSpeed1_y) * (1.f - friction);

                    ++collisions;
                }
            }
        }

        float averageSpeed = 0.0f;
        for (auto speed : speeds)
        {
            averageSpeed += sqrt(speed.x * speed.x + speed.y * speed.y) / number;
        }
        std::cout << "average speed: " << averageSpeed << " temperature: " << averageSpeed * 75.f << "�C \n";

        labelSpeed.setString("Average speed: " + std::to_string(averageSpeed) + " Temperature: " + std::to_string(averageSpeed * 75.f) + "�C " + "Gravity: " + std::to_string(gravity) + " Friction: " + std::to_string(friction) + " Collisions: " + std::to_string(collisions) + " Number: " + std::to_string(number));

        mouseShape.setPosition(mouse.getPosition().x - window.getPosition().x, mouse.getPosition().y - window.getPosition().y);
        
        window.clear();
        for (auto shape : shapes)
        {
            window.draw(shape);
        }
        window.draw(mouseShape);

        window.draw(labelSpeed);
        window.display();
    }

    return 0;
}
