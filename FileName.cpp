#include <easyx.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <tchar.h>

using namespace std; 

// 基类：游戏对象
class GameObject {
protected:
    int x, y;
    int width, height;
    COLORREF color;

public:
    GameObject(int x, int y, int w, int h, COLORREF c)
        : x(x), y(y), width(w), height(h), color(c) {
    }

    virtual void draw() const = 0;  // 纯虚函数

    // 获取碰撞检测用的矩形
    RECT getRect() const {
        RECT rect = { x, y, x + width, y + height };
        return rect;
    }

    // 坐标访问接口
    int getX() const { return x; }
    int getY() const { return y; }
    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
};

// 蛇头类
class SnakeHead : public GameObject {
private:
    int dir;  // 1-上，2-下，3-左，4-右

public:
    SnakeHead(int x, int y) : GameObject(x, y, 20, 20, RGB(0, 100, 0)), dir(4) {}

    // 设置方向（禁止反向）
    void setDir(int newDir) {
        if ((dir == 1 && newDir != 2) || (dir == 2 && newDir != 1) ||
            (dir == 3 && newDir != 4) || (dir == 4 && newDir != 3)) {
            dir = newDir;
        }
    }

    // 移动蛇头
    void move() {
        switch (dir) {
        case 1: setY(getY() - 20); break;  // 上移
        case 2: setY(getY() + 20); break;  // 下移
        case 3: setX(getX() - 20); break;  // 左移
        case 4: setX(getX() + 20); break;  // 右移
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

// 蛇身类
class SnakeBody : public GameObject {
public:
    SnakeBody(int x, int y) : GameObject(x, y, 20, 20, RGB(0, 200, 0)) {}  // 浅绿色

    // 跟随前一节位置
    void follow(int prevX, int prevY) {
        setX(prevX);
        setY(prevY);
    }

    void draw() const override {
        setfillcolor(color);
        solidrectangle(x, y, x + width, y + height);
    }
};

// 食物类
class Food : public GameObject {
public:
    Food() : GameObject(0, 0, 20, 20, RED) {}

    // 随机生成位置（避开蛇身）
    void randomPos(const SnakeHead& head, const vector<SnakeBody>& body) {
        vector<pair<int, int>> snakePos;
        snakePos.emplace_back(head.getX(), head.getY());
        for (const auto& part : body) {
            snakePos.emplace_back(part.getX(), part.getY());
        }

        while (true) {
            // 生成20的倍数坐标（与蛇对齐）
            int newX = (rand() % 32) * 20;  // 0-620
            int newY = (rand() % 24) * 20;  // 0-460

            // 检查是否在蛇身上
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
        solidcircle(x + 10, y + 10, 10);  // 圆形食物
    }
};

// 游戏控制类
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
        // 初始化蛇身（2节）
        snakeBody.emplace_back(280, 200);
        snakeBody.emplace_back(260, 200);
        // 初始化食物位置
        food.randomPos(snakeHead, snakeBody);
    }

    // 处理输入
    void handleInput() {
        if (_kbhit()) {
            int key = _getch();
            // 处理方向键（方向键会先返回0xE0或224）
            if (key == 0xE0 || key == 224) {
                key = _getch(); // 获取方向键的第二个字符
                switch (key) {
                case 72: snakeHead.setDir(1); break; // 上箭头
                case 80: snakeHead.setDir(2); break; // 下箭头
                case 75: snakeHead.setDir(3); break; // 左箭头
                case 77: snakeHead.setDir(4); break; // 右箭头
                }
            }
            // 处理字母键和ESC
            else {
                switch (key) {
                case 'w': case 'W': snakeHead.setDir(1); break; // 上
                case 's': case 'S': snakeHead.setDir(2); break; // 下
                case 'a': case 'A': snakeHead.setDir(3); break; // 左
                case 'd': case 'D': snakeHead.setDir(4); break; // 右
                case 27: isOver = true; break;       // ESC退出
                }
            }
        }
    }

    // 检测碰撞
    void checkCollision() {
        // 边界碰撞
        if (snakeHead.getX() < 0 || snakeHead.getX() >= WIDTH ||
            snakeHead.getY() < 0 || snakeHead.getY() >= HEIGHT) {
            isOver = true;
        }

        // 自身碰撞
        for (const auto& part : snakeBody) {
            if (snakeHead.getX() == part.getX() && snakeHead.getY() == part.getY()) {
                isOver = true;
                break;
            }
        }

        // 食物碰撞
        if (!isOver && checkRectCollision(snakeHead.getRect(), food.getRect())) {
            score += 10;
            // 蛇身增长（添加一节到尾部）
            const auto& lastPart = snakeBody.back();
            snakeBody.emplace_back(lastPart.getX() - 20, lastPart.getY());
            // 生成新食物
            food.randomPos(snakeHead, snakeBody);
        }
    }

    // 矩形碰撞检测
    bool checkRectCollision(RECT a, RECT b) {
        return a.left < b.right && a.right > b.left &&
            a.top < b.bottom && a.bottom > b.top;
    }

    // 更新蛇身位置
    void updateSnake() {
        if (isOver) return;

        // 记录各部分旧位置
        int prevX = snakeHead.getX();
        int prevY = snakeHead.getY();
        snakeHead.move();  // 移动头部

        // 边界穿越处理
        if (snakeHead.getX() < 0)
            snakeHead.setX(WIDTH - 20);
        else if (snakeHead.getX() >= WIDTH)
            snakeHead.setX(0);
        if (snakeHead.getY() < 0)
            snakeHead.setY(HEIGHT - 20);
        else if (snakeHead.getY() >= HEIGHT)
            snakeHead.setY(0);

        // 更新身体
        for (auto& part : snakeBody) {
            int currX = part.getX();
            int currY = part.getY();
            part.follow(prevX, prevY);
            prevX = currX;
            prevY = currY;
        }
    }

    // 绘制画面
    void draw() {
        BeginBatchDraw();
        cleardevice();  // 清屏

        // 绘制蛇头和身体
        snakeHead.draw();
        for (const auto& part : snakeBody) {
            part.draw();
        }

        // 绘制食物
        food.draw();

        // 绘制分数
        TCHAR scoreText[20];
        _stprintf_s(scoreText, _T("分数：%d"), score);
        settextcolor(BLACK);
        outtextxy(10, 10, scoreText);

        // 绘制游戏结束提示
        if (isOver) {
            settextcolor(RED);
            TCHAR overText[50];
            _stprintf_s(overText, _T("游戏结束！最终得分：%d"), score);
            RECT rect = { 100, 200, 540, 250 };
            drawtext(overText, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        FlushBatchDraw();
    }

    // 游戏主循环
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