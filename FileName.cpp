#include <easyx.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <tchar.h>

using namespace std; 

// ���ࣺ��Ϸ����
class GameObject {
protected:
    int x, y;
    int width, height;
    COLORREF color;

public:
    GameObject(int x, int y, int w, int h, COLORREF c)
        : x(x), y(y), width(w), height(h), color(c) {
    }

    virtual void draw() const = 0;  // ���麯��

    // ��ȡ��ײ����õľ���
    RECT getRect() const {
        RECT rect = { x, y, x + width, y + height };
        return rect;
    }

    // ������ʽӿ�
    int getX() const { return x; }
    int getY() const { return y; }
    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
};

// ��ͷ��
class SnakeHead : public GameObject {
private:
    int dir;  // 1-�ϣ�2-�£�3-��4-��

public:
    SnakeHead(int x, int y) : GameObject(x, y, 20, 20, RGB(0, 100, 0)), dir(4) {}

    // ���÷��򣨽�ֹ����
    void setDir(int newDir) {
        if ((dir == 1 && newDir != 2) || (dir == 2 && newDir != 1) ||
            (dir == 3 && newDir != 4) || (dir == 4 && newDir != 3)) {
            dir = newDir;
        }
    }

    // �ƶ���ͷ
    void move() {
        switch (dir) {
        case 1: setY(getY() - 20); break;  // ����
        case 2: setY(getY() + 20); break;  // ����
        case 3: setX(getX() - 20); break;  // ����
        case 4: setX(getX() + 20); break;  // ����
        }
    }

    void draw() const override {
        setfillcolor(color);
        solidrectangle(x, y, x + width, y + height);
        setfillcolor(WHITE);
        POINT pts[3];
        if (dir == 1) { pts[0] = { x + 10, y }; pts[1] = { x, y + 20 }; pts[2] = { x + 20, y + 20 }; }
        else if (dir == 2) { pts[0] = { x + 10, y + 20 }; pts[1] = { x, y }; pts[2] = { x + 20, y }; }
        else if (dir == 3) { pts[0] = { x, y + 10 }; pts[1] = { x + 20, y }; pts[2] = { x + 20, y + 20 }; }
        else if (dir == 4) { pts[0] = { x + 20, y + 10 }; pts[1] = { x, y }; pts[2] = { x, y + 20 }; }
        fillpolygon(pts, 3);
    }
};

// ������
class SnakeBody : public GameObject {
public:
    SnakeBody(int x, int y) : GameObject(x, y, 20, 20, RGB(0, 200, 0)) {}  // ǳ��ɫ

    // ����ǰһ��λ��
    void follow(int prevX, int prevY) {
        setX(prevX);
        setY(prevY);
    }

    void draw() const override {
        setfillcolor(color);
        solidrectangle(x, y, x + width, y + height);
    }
};

// ʳ����
class Food : public GameObject {
public:
    Food() : GameObject(0, 0, 20, 20, RED) {}

    // �������λ�ã��ܿ�����
    void randomPos(const SnakeHead& head, const vector<SnakeBody>& body) {
        vector<pair<int, int>> snakePos;
        snakePos.emplace_back(head.getX(), head.getY());
        for (const auto& part : body) {
            snakePos.emplace_back(part.getX(), part.getY());
        }

        while (true) {
            // ����20�ı������꣨���߶��룩
            int newX = (rand() % 32) * 20;  // 0-620
            int newY = (rand() % 24) * 20;  // 0-460

            // ����Ƿ���������
            bool collision = false;
            for (const auto& pos : snakePos) {
                if (newX == pos.first && newY == pos.second) {
                    collision = true;
                    break;
                }
            }
            if (!collision) {
                setX(newX);
                setY(newY);
                break;
            }
        }
    }

    void draw() const override {
        setfillcolor(color);
        solidcircle(x + 10, y + 10, 10);  // Բ��ʳ��
    }
};

// ��Ϸ������
class Game {
private:
    const int WIDTH = 640;
    const int HEIGHT = 480;
    SnakeHead snakeHead;
    vector<SnakeBody> snakeBody;
    Food food;
    int score;
    bool isOver;

public:
    Game() : snakeHead(300, 200), score(0), isOver(false) {
        // ��ʼ������2�ڣ�
        snakeBody.emplace_back(280, 200);
        snakeBody.emplace_back(260, 200);
        // ��ʼ��ʳ��λ��
        food.randomPos(snakeHead, snakeBody);
    }

    // ��������
    void handleInput() {
        if (_kbhit()) {
            int key = _getch();
            // �����������������ȷ���0xE0��224��
            if (key == 0xE0 || key == 224) {
                key = _getch(); // ��ȡ������ĵڶ����ַ�
                switch (key) {
                case 72: snakeHead.setDir(1); break; // �ϼ�ͷ
                case 80: snakeHead.setDir(2); break; // �¼�ͷ
                case 75: snakeHead.setDir(3); break; // ���ͷ
                case 77: snakeHead.setDir(4); break; // �Ҽ�ͷ
                }
            }
            // ������ĸ����ESC
            else {
                switch (key) {
                case 'w': case 'W': snakeHead.setDir(1); break; // ��
                case 's': case 'S': snakeHead.setDir(2); break; // ��
                case 'a': case 'A': snakeHead.setDir(3); break; // ��
                case 'd': case 'D': snakeHead.setDir(4); break; // ��
                case 27: isOver = true; break;       // ESC�˳�
                }
            }
        }
    }

    // �����ײ
    void checkCollision() {
        // �߽���ײ
        if (snakeHead.getX() < 0 || snakeHead.getX() >= WIDTH ||
            snakeHead.getY() < 0 || snakeHead.getY() >= HEIGHT) {
            isOver = true;
        }

        // ������ײ
        for (const auto& part : snakeBody) {
            if (snakeHead.getX() == part.getX() && snakeHead.getY() == part.getY()) {
                isOver = true;
                break;
            }
        }

        // ʳ����ײ
        if (!isOver && checkRectCollision(snakeHead.getRect(), food.getRect())) {
            score += 10;
            // �������������һ�ڵ�β����
            const auto& lastPart = snakeBody.back();
            snakeBody.emplace_back(lastPart.getX() - 20, lastPart.getY());
            // ������ʳ��
            food.randomPos(snakeHead, snakeBody);
        }
    }

    // ������ײ���
    bool checkRectCollision(RECT a, RECT b) {
        return a.left < b.right && a.right > b.left &&
            a.top < b.bottom && a.bottom > b.top;
    }

    // ��������λ��
    void updateSnake() {
        if (isOver) return;

        // ��¼�����־�λ��
        int prevX = snakeHead.getX();
        int prevY = snakeHead.getY();
        snakeHead.move();  // �ƶ�ͷ��

        // �߽紩Խ����
        if (snakeHead.getX() < 0)
            snakeHead.setX(WIDTH - 20);
        else if (snakeHead.getX() >= WIDTH)
            snakeHead.setX(0);
        if (snakeHead.getY() < 0)
            snakeHead.setY(HEIGHT - 20);
        else if (snakeHead.getY() >= HEIGHT)
            snakeHead.setY(0);

        // ��������
        for (auto& part : snakeBody) {
            int currX = part.getX();
            int currY = part.getY();
            part.follow(prevX, prevY);
            prevX = currX;
            prevY = currY;
        }
    }

    // ���ƻ���
    void draw() {
        BeginBatchDraw();
        cleardevice();  // ����

        // ������ͷ������
        snakeHead.draw();
        for (const auto& part : snakeBody) {
            part.draw();
        }

        // ����ʳ��
        food.draw();

        // ���Ʒ���
        TCHAR scoreText[20];
        _stprintf_s(scoreText, _T("������%d"), score);
        settextcolor(BLACK);
        outtextxy(10, 10, scoreText);

        // ������Ϸ������ʾ
        if (isOver) {
            settextcolor(RED);
            TCHAR overText[50];
            _stprintf_s(overText, _T("��Ϸ���������յ÷֣�%d"), score);
            RECT rect = { 100, 200, 540, 250 };
            drawtext(overText, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        FlushBatchDraw();
    }

    // ��Ϸ��ѭ��
    void run() {
        initgraph(WIDTH, HEIGHT);
        setbkcolor(WHITE);

        while (!isOver) {
            handleInput();
            updateSnake();
            checkCollision();
            draw();
            Sleep(150);
        }

        _getch();
        closegraph();
    }
};

int main() {
    srand((unsigned int)time(NULL));
    Game game;
    game.run();
    return 0;
}