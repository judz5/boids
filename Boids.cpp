#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <vector>

const int PROTECTED_RANGE = 15;
const int VISUAL_RANGE = 50;
const double CENTERING_FACTOR = .0005;
const double AVOID_FACTOR = .1;
const double MATCHING_FACTOR = .1;
const double TURN_FACTOR = 0.5;
const int MAXSPEED = 5;
const int MINSPEED = 1;

class Boid {

public:
    double x, y, vx, vy;
    int type;
    sf::CircleShape shape;

    Boid(double xPos, double yPos) {
        x = xPos;
        y = yPos;
        vx = rand() % (MAXSPEED - MINSPEED + 1) + MINSPEED;
        vy = rand() % (MAXSPEED - MINSPEED + 1) + MINSPEED;

        shape.setRadius(2);
        shape.setFillColor(sf::Color::White);
    }

    void draw(sf::RenderWindow &window) {
        shape.setPosition(x, y);
        window.draw(shape);

        // sf::Vertex velocityLine[] = {
        //     sf::Vertex(sf::Vector2f(x, y), sf::Color::Blue),                      // Starting point at the center of the boid
        //     sf::Vertex(sf::Vector2f(x + (2 * vx), y + (2 * vy)), sf::Color::Blue) // Ending point based on velocity
        // };
        // window.draw(velocityLine, 2, sf::Lines);

        // // Draw the protected range circle
        // sf::CircleShape protectedCircle(PROTECTED_RANGE);
        // protectedCircle.setFillColor(sf::Color(255, 0, 0, 50)); // Semi-transparent red
        // protectedCircle.setOrigin(PROTECTED_RANGE, PROTECTED_RANGE);
        // protectedCircle.setPosition(x, y);
        // window.draw(protectedCircle);

        // // Draw the view range circle
        // sf::CircleShape viewCircle(VISUAL_RANGE);
        // viewCircle.setFillColor(sf::Color(0, 255, 0, 50)); // Semi-transparent green
        // viewCircle.setOrigin(VISUAL_RANGE, VISUAL_RANGE);
        // viewCircle.setPosition(x, y);
        // window.draw(viewCircle);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(500, 500), "Boids Simulation");

    window.setFramerateLimit(60);

    std::vector<Boid> flock;
    double xpos_avg, ypos_avg, xvel_avg, yvel_avg, close_dx, close_dy, dx, dy;
    int neighboring_boids;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                Boid bird(event.mouseButton.x, event.mouseButton.y);
                flock.push_back(bird);
                std::cout << "mouse click, current flock size = " << flock.size() << std::endl;
            }
        }

        window.clear();

        for (Boid &b : flock) {
            xpos_avg = ypos_avg = xvel_avg = yvel_avg = close_dx = close_dy = dx = dy = 0.0;
            neighboring_boids = 0;

            for (Boid &bo : flock) {
                if (&bo != &b) {
                    dx = b.x - bo.x;
                    dy = b.y - bo.y;

                    if (abs(dx) < VISUAL_RANGE && abs(dy) < VISUAL_RANGE) {

                        double squared_distance = (dx * dx) + (dy * dy);

                        if (squared_distance < (PROTECTED_RANGE * PROTECTED_RANGE)) {
                            close_dx += b.x - bo.x;
                            close_dy += b.y - bo.y;
                        } else if (squared_distance < (VISUAL_RANGE * VISUAL_RANGE)) {
                            xpos_avg += bo.x;
                            ypos_avg += bo.y;
                            xvel_avg += bo.vx;
                            yvel_avg += bo.vy;

                            neighboring_boids++;
                        }
                    }
                }
            }
            if (neighboring_boids > 0) {
                xpos_avg = xpos_avg / neighboring_boids;
                ypos_avg = ypos_avg / neighboring_boids;
                xvel_avg = xvel_avg / neighboring_boids;
                yvel_avg = yvel_avg / neighboring_boids;

                b.vx = (b.vx + (xpos_avg - b.x) * CENTERING_FACTOR + (xvel_avg - b.vx) * MATCHING_FACTOR);
                b.vy = (b.vy + (ypos_avg - b.y) * CENTERING_FACTOR + (yvel_avg - b.vy) * MATCHING_FACTOR);
            }

            b.vx = b.vx + (close_dx * AVOID_FACTOR);
            b.vy = b.vy + (close_dy * AVOID_FACTOR);

            if (b.y < 50) {
                b.vy = b.vy + TURN_FACTOR;
            }
            if (b.y > 450) {
                b.vy = b.vy - TURN_FACTOR;
            }
            if (b.x < 50) {
                b.vx = b.vx + TURN_FACTOR;
            }
            if (b.x > 450) {
                b.vx = b.vx - TURN_FACTOR;
            }

            double speed = sqrt((b.vx * b.vx) + (b.vy * b.vy));

            if (speed < MINSPEED) {
                b.vx = (b.vx / speed) * MINSPEED;
                b.vy = (b.vy / speed) * MINSPEED;
            }
            if (speed > MAXSPEED) {
                b.vx = (b.vx / speed) * MAXSPEED;
                b.vy = (b.vy / speed) * MAXSPEED;
            }

            b.x = b.x + b.vx;
            b.y = b.y + b.vy;

            // std::cout << "SHOULD DRAW NOW" << std::endl;
            b.draw(window);
        }

        window.display();
    }

    return 0;
}