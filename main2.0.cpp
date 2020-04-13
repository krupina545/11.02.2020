#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>

using namespace std;
using namespace sf;

constexpr int windowWidth{800}, windowHeight{600};
constexpr float ballRadius{10}, ballVelocity{5};
constexpr float paddleWidth{20}, paddleHeight{80}, paddleVelocity{6};


class cBall
{

    public:
        CircleShape shape;
        Vector2f velocity{-ballVelocity, -ballVelocity};

        cBall(int posX, int posY)
        {
            shape.setPosition(posX, posY);
            shape.setRadius(ballRadius);
            shape.setFillColor(Color::Red);
            shape.setOrigin(ballRadius, ballRadius);
        }

    void update() {
        shape.move(velocity);
        if(left() < 0) {velocity.x = ballVelocity;}
        else if (right() > windowWidth) {velocity.x = -ballVelocity;}
        if(top() < 0) {velocity.y = ballVelocity;}
        else if(bottom() > windowHeight) {velocity.y = -ballVelocity;}
    }

    void Reset()
    {
        shape.setPosition(windowWidth/2, windowHeight/2);
    }
    float x() {return shape.getPosition().x;}
    float y() {return shape.getPosition().y;}
    float left() {return x() - shape.getRadius();}
    float right() {return x() + shape.getRadius();}
    float top() {return y() - shape.getRadius();}
    float bottom() {return y() + shape.getRadius();}
};

class Paddle
{
public:
    RectangleShape shape;
    Vector2f velocity;
    Keyboard::Key upKey;
    Keyboard::Key downKey;

    Paddle(float mX, float mY, Keyboard::Key up, Keyboard::Key down) : upKey(up), downKey(down)
    {
        shape.setPosition(mX, mY);
        shape.setSize({paddleWidth, paddleHeight});
        shape.setFillColor(Color::Green);
        shape.setOrigin(paddleWidth/2, paddleHeight/2);
    }
    void update() {
        shape.move(velocity);
        if(Keyboard::isKeyPressed(upKey) && top()>0) {velocity.y = -paddleVelocity;}
        else if(Keyboard::isKeyPressed(downKey) && bottom()<windowHeight) {velocity.y = paddleVelocity;}
        else {velocity.y = 0;}
    }

    float x() {return shape.getPosition().x;}
    float y() {return shape.getPosition().y;}
    float left() {return x() - shape.getSize().x/2;}
    float right() {return x() + shape.getSize().x/2;}
    float top() {return y() - shape.getSize().y/2;}
    float bottom() {return y() + shape.getSize().y/2;}
};

template<class T> bool isIntersecting(T& mA, cBall& mB)
{
    return (mA.right() >= mB.left() && mA.left() <= mB.right()
        && mA.bottom() >= mB.top() && mA.top() <= mB.bottom());
}

void testCollision(Paddle& mPaddle, cBall& mBall, int speedSign)
{
    if(!isIntersecting(mPaddle, mBall)) return;
    mBall.velocity.x = speedSign*ballVelocity;
    if(mBall.y() >= mPaddle.y()){mBall.velocity.y = -ballVelocity;}
    else {mBall.velocity.y = ballVelocity;}
}

int main()
{
    cBall ball{windowWidth/2, windowHeight/2};
    Paddle paddle1(windowWidth/10, windowHeight/2, Keyboard::Key::Q, Keyboard::Key::A);
    Paddle paddle2(windowWidth*9/10, windowHeight/2, Keyboard::Key::O, Keyboard::Key::L);

    static int score1 = 0;
    static int score2 = 0;
    Text score;
    Font font;
    font.loadFromFile("arial.ttf");
    score.setFont(font);
    score.setCharacterSize(30);
    score.setPosition({windowWidth/3, windowHeight/10 });
    score.setFillColor(Color::White);

    SoundBuffer buffer1;
    buffer1.loadFromFile("coindrop.wav");
    Sound sound1;
    sound1.setBuffer(buffer1);

    SoundBuffer buffer2;
    buffer2.loadFromFile("bounce.wav");
    Sound sound2;
    sound2.setBuffer(buffer2);
    sound2.play();

    RenderWindow window{{windowWidth, windowHeight}, "Ping Pong 1.0"};
    window.setFramerateLimit(60);
    while (true)
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // clear the window with black color
        window.clear(Color::Black);
        if(Keyboard::isKeyPressed(Keyboard::Key::Escape)) break;
        ball.update();
        if(ball.left() < 0) {
                score2++;
                sound1.play();
                ball.Reset();
        }
        if(ball.right() > windowWidth) {
                score1++;
                sound1.play();
                ball.Reset();
        }
        if(isIntersecting(paddle1, ball) || isIntersecting(paddle2, ball)) {sound2.play();}
        ostringstream sscore;
        sscore << "Player 1: " << score1 << ' ' << "Player 2: " << score2;
        score.setString(sscore.str());
        paddle1.update();
        paddle2.update();
        testCollision(paddle1, ball, 1);
        testCollision(paddle2, ball, -1);
        window.draw(score);
        window.draw(paddle1.shape);
        window.draw(paddle2.shape);
        window.draw(ball.shape);


        window.display();
    }

    return 0;
}
