/*
 * ============================================================
 *  C++ 课程项目 · 《明日方舟》风格 2D 塔防   【SFML 版 · SFML 3.x】
 *  分支 M1：敌人的移动
 * ============================================================
 *
 *  这个文件怎么读？——它只有【两大部分】：
 *
 *    ┌── 上半部分 · main()：游戏数据 + 你要写的代码 ──────────────┐
 *    │  地图、敌人坐标、生命等【数据】就摆在 main() 开头，紧挨着     │
 *    │  你要写的移动代码——参考近在眼前。你的任务全部在 main() 里。 │
 *    └──────────────────────────────────────────────────────────┘
 *    ┌── 下半部分 · 渲染引擎（把游戏画到窗口上）────────────────────┐
 *    │  常量、图片/字体的加载、各种 drawXxx() 画图函数。            │
 *    │  【M1 你可以完全跳过、不用看懂，也不要改】。你只要知道：      │
 *    │  这些函数负责"照着数据把画面画出来"——你改了数据，画面就变。 │
 *    └──────────────────────────────────────────────────────────┘
 *
 * ============================================================
 */

#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <random>
#include <queue>



// ---------- 常量（棋盘尺寸等固定配置）----------
constexpr int GRID_W = 9;    // 地图列数（对应关卡图）
constexpr int GRID_H = 4;    // 地图行数

// 棋盘显示尺寸：保持关卡图 16:9 的比例，避免背景图被拉伸变形。
constexpr int BOARD_W = 800;
constexpr int BOARD_H = 450;

constexpr int HUD_W = 200;   // 右侧信息栏宽度


void loadAssets();
void drawMap(sf::RenderWindow& win, const std::vector<std::string>& map);
void drawEnemy(sf::RenderWindow& win, bool enemyAlive,
               int enemyRow, int enemyCol, float stepTimer, float STEP_TIME);
void drawHud(sf::RenderWindow& win);
void drawTopStatus(sf::RenderWindow& win, int enemiesTotal, int remainingEnemies, int life);
void drawGameOver(sf::RenderWindow& win, bool victory);


extern std::vector<std::vector<std::pair<int,int>>> paths;
extern std::pair<int,int> zuobiao;
extern std::pair<int,int> destiny;

using Cell = std::pair<int, int>;

void deep_find(Cell now,
               Cell destiny,
               std::vector<std::string>& board,
               std::vector<Cell>& currentPath,
               std::vector<std::vector<Cell>>& allPaths);
void alldeep_find(Cell now,
               Cell destination,
               std::vector<std::string>& board,
               std::vector<Cell>& currentPath,
               std::vector<Cell>& all);
bool canWalk(Cell cell, const std::vector<std::string>& board);
bool allcanWalk(Cell cell, const std::vector<std::string>& board);
std::vector<std::pair<int,int>> neighbor(std::pair<int,int> zuobiao);

void circle(std::vector<Cell>& path,
            Cell p1,
            Cell p2,
            int num = 1);

// ============================================================
//  上半部分 · main()：游戏数据 + 你要写的代码
// ============================================================

int main()
{
    // 开一个窗口，并把刷新上限设成每秒 60 帧（下面主循环会解释"帧"是什么）。
    sf::RenderWindow window(sf::VideoMode({unsigned(BOARD_W + HUD_W), unsigned(BOARD_H)}),
                            "Arknights");
    window.setFramerateLimit(60);
    loadAssets();                 // 载入字体/贴图（只需一次）

    // ========================================================
    //  游戏数据与状态  ——  你写代码时会用到的，就是下面这些普通变量
    // ========================================================

    //  地图：一个字符网格。map[行][列] 就是那一格的地形字符：
    //     'S' 出怪口     'E' 终点（蓝门，敌人想去的地方）
    //     '#' 道路（敌人能走的地面）
    //     'P' 高台 / '.' 障碍  ——  敌人都【不能】走（'P' 将来用来放远程干员，是后面任务的事）
    //     'X'敌人已经走过一次
    std::vector<std::string> map = {
        ".PPPPPPP.",
        "E#P#####.",
        ".###P###S",
        ".PPPPPPP.",
    };

    // 关卡进度 / 结束状态
    int  life = 3;                // 剩余生命：敌人溜进蓝门就降低，归零判负
    int  remainingEnemies = 1;    // 场上还剩几个敌人：死一个 -1，归零判胜
    int  enemiesTotal     = 1;    // 本关敌人总数；
    bool gameOver = false;        // 游戏是否已结束（结束后就不再更新、画面定格）
    bool victory  = false;        // 结束时的结果：true=胜利, false=失败

    // 敌人（源石虫）：M1 全场只有这一个敌人
    int   enemyRow = 2, enemyCol = 8;   // 敌人现在站在第几行、第几列（出生在出怪口 S）
    bool  enemyAlive = true;            // 是否还在场上（到终点或被消灭后变 false）
    float stepTimer  = 0.f;             // "走格计时器"：攒时间用，见下面主循环的说明
    const float STEP_TIME = 1.0f;       // 每走一格要花几秒（调大走得慢、调小走得快）
    int direction = 0; // 0,1,2,3,4不动，上,右,下,左
    std::vector<std::vector<int>> plans;

    // 用局部副本搜索：递归会临时标记 board，但回溯后恢复，不污染原地图。
    std::vector<std::string> searchBoard = map;
    std::vector<Cell> currentPath;
    std::vector<std::vector<Cell>> allPaths;
    deep_find({2, 8}, {1, 0}, searchBoard, currentPath, allPaths);

    std::vector<Cell> bestPath;
    // if (!allPaths.empty())
    // {
    //     bestPath = allPaths.front();
    //     for (const auto& candidate : allPaths)
    //         if (candidate.size() < bestPath.size())
    //             bestPath = candidate;
    // } //不重复寻路取最短的一条
    // circle(bestPath,Cell{1,5},Cell{2,7},2); //添加绕圈逻辑
    alldeep_find({2, 8}, {1, 0}, searchBoard, currentPath, bestPath)
    std::vector<int> plan;
    for (std::size_t i = 1; i < bestPath.size(); ++i)
    {
        int dr = bestPath[i].first - bestPath[i - 1].first;
        int dc = bestPath[i].second - bestPath[i - 1].second;
        plan.push_back(dr == -1 ? 1 : dr == 1 ? 3 : dc == 1 ? 2 : 4);
    }
    std::size_t planIndex = 0;

    // ╔════════════════ 待完成：定义变量 ════════════════╗
    // 如果你需要定义额外变量来实现移动逻辑，就定义在这里
    // ╚════════════════ 定义变量到此为止 ════════════════╝

    sf::Clock clock;              // 计时器，用来量"每一帧过了多少秒"

    // ========================================================
    //  游戏主循环：游戏里的"时间"，就是靠这个 while 循环流动起来的
    //  ------------------------------------------------------
    //  程序不是"从上到下执行一遍就结束"，而是靠下面这个 while 循环【一圈又一圈】
    //  地重复运行，直到你关掉窗口。每重复一圈叫做一"帧"(frame)。
    //  我们把刷新上限设成 60（setFramerateLimit(60)），所以这个循环大约【每秒转 60 圈】，
    //  每圈约 1/60 ≈ 0.016 秒。每一圈都做三件事：
    //     ① 处理事件（比如你点了窗口的关闭按钮）
    //     ② 更新游戏世界一点点（按"过了多久"让敌人往前挪）——这就是"时间在流逝"
    //     ③ 把整个画面重画一遍
    //  每圈只动一点点、又画得飞快，一圈圈连起来看，就成了流畅的动画。
    // ========================================================
    while (window.isOpen())
    {
        // ① 处理事件：SFML 3 里 pollEvent() 返回 std::optional<sf::Event>
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // dt = 距离上一圈过了多少"秒"（大约 0.016）。这是"时间"的最小单位。
        float dt = clock.restart().asSeconds();

        // ② 更新游戏世界（只在游戏进行中；胜/负后画面定格）
        //  目标：让敌人（源石虫）从出生点 S 一步步沿着道路('#')走到终点 E（蓝门）。
        //
        //  你手上的"工具"（就是上面刚声明的那些变量）：
        //    • enemyRow, enemyCol  —— 敌人现在的行、列。【改这两个数，敌人就移动】
        //    • map[行][列]         —— 查某格地形：'#''S''E' 能走，'P''.' 不能走
        //    • stepTimer, STEP_TIME —— 控制节奏；enemyAlive/life/remainingEnemies —— 到终点后要更新
        //
        //  为什么要用 stepTimer？——如果每一帧都走一格，一秒就走 60 格，敌人会"biu"一下
        //  瞬间冲到终点。所以我们【把 dt 一点点攒进 stepTimer，攒够 STEP_TIME 秒(=1秒)才走一格】，
        //  这样不管电脑快慢，敌人都稳定地"每秒走一格"。
        //
        bool shouldMove = false;                     // 本帧是否需要移动一步
        if (!gameOver && enemyAlive)
        {
            stepTimer += dt;                     // 把这一帧的时间攒进计时器
            shouldMove = (stepTimer >= STEP_TIME); // 攒够 1 秒 → 走一格
        }

        if (shouldMove)
        {
            stepTimer -= STEP_TIME;
            if (enemyRow == 1 && enemyCol == 0)          // 已在终点 E（本图 E 在 (1,0)）→ 溜进蓝门
            {
                enemyAlive = false;
                --life;
                --remainingEnemies;
                shouldMove = false;
            }
            
        }

        if (shouldMove) // 进行移动
        {
            // ╔════════════════ 待完成：移动代码 ════════════════╗
            if (planIndex < plan.size())
                direction = plan[planIndex++];
            else
                direction = 0;

            switch (direction)
            {
            case 1: --enemyRow; break;
            case 2: ++enemyCol; break;
            case 3: ++enemyRow; break;
            case 4: --enemyCol; break;
            default: break;
            }
            // ╚════════════════ 移动代码到此为止 ════════════════╝
        }

        if (!gameOver)
        {
            // 引擎：胜负判定（每帧检查；这两行不用改）
            if (life <= 0)                  { gameOver = true; victory = false; }   // 生命归零 → 失败
            else if (remainingEnemies <= 0) { gameOver = true; victory = true;  }   // 敌人清空 → 胜利
        }

        // ③ 重画整个画面（引擎）：清屏 → 按当前数据画地图/敌人/文字 → 显示
        window.clear(sf::Color(0, 0, 40));
        drawMap(window, map);
        drawEnemy(window, enemyAlive, enemyRow, enemyCol, stepTimer, STEP_TIME);
        drawTopStatus(window, enemiesTotal, remainingEnemies, life);
        drawHud(window);
        if (gameOver) drawGameOver(window, victory);
        window.display();
    }
    return 0;
}

std::pair<int,int> zuobiao = {2,8};   // S：出入口
std::pair<int,int> destiny = {1,0};  // E：基地
std::vector<std::string> change_map = {
        ".PPPPPPP.",
        "E#P#####.",
        ".###P###S",
        ".PPPPPPP.",
    };

// 范围：first 是行，second 是列；上/左边界为 0，右/下边界取地图尺寸。
int right_range = static_cast<int>(change_map[0].size());
int left_range = 0;
int up_range = 0;
int down_range = static_cast<int>(change_map.size());

int fastRand(int lo, int hi)
{
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(gen);
}
bool canWalk(Cell cell, const std::vector<std::string>& board)
{
    if (cell.first < 0 || cell.first >= static_cast<int>(board.size()) ||
        cell.second < 0 || cell.second >= static_cast<int>(board[cell.first].size()))
        return false;

    const char tile = board[cell.first][cell.second];
    return tile == '#' || tile == 'S' || tile == 'E';
}
bool allcanWalk(Cell cell, const std::vector<std::string>& board)
{
    if (cell.first < 0 || cell.first >= static_cast<int>(board.size()) ||
        cell.second < 0 || cell.second >= static_cast<int>(board[cell.first].size()))
        return false;

    const char tile = board[cell.first][cell.second];
    return tile == '#' || tile == 'S' || tile == 'E'||tile == '1'||tile == '2';
}

void deep_find(Cell now,
               Cell destination,
               std::vector<std::string>& board,
               std::vector<Cell>& currentPath,
               std::vector<std::vector<Cell>>& allPaths)
{
    currentPath.push_back(now);
    const char oldTile = board[now.first][now.second];
    board[now.first][now.second] = 'X';

    if (now == destination)
    {
        allPaths.push_back(currentPath);
    }
    else
    {
        for (const Cell next : neighbor(now))
        {
            if (canWalk(next, board))
                deep_find(next, destination, board, currentPath, allPaths);
        }
    }

    // 回溯：只恢复这一次递归修改的格子。
    board[now.first][now.second] = oldTile;
    currentPath.pop_back();
}

void alldeep_find(Cell now,
               Cell destination,
               std::vector<std::string>& board,
               std::vector<Cell>& currentPath,
               std::vector<Cell>& all)
{
    currentPath.push_back(now);
    const char oldTile = board[now.first][now.second];
    if(oldTile == '#')
    board[now.first][now.second] = '1';
    else if(oldTile == '1')
    board[now.first][now.second] = '2';
    else if(oldTile == '2')
    board[now.first][now.second] = '3';
    


    if (now == destination )
    {
        bool hasUnvisitedRoad = false;
    for (const auto& row : board)
    {
        if (row.find('#') != std::string::npos)
        {
            hasUnvisitedRoad = true;
            break;
        }
    }
    // 没有未访问道路时，保存当前遍历路线并结束本次递归。
    if (!hasUnvisitedRoad)
    {
        all = currentPath;
        board[now.first][now.second] = oldTile;
        currentPath.pop_back();
        return;
    }
    }
    else
    {
        for (const Cell next : neighbor(now))
        {
            if (allcanWalk(next, board))
                alldeep_find(next, destination, board, currentPath, all);
        }
    }

    // 回溯：只恢复这一次递归修改的格子。
    board[now.first][now.second] = oldTile;
    currentPath.pop_back();
}


std::vector<std::pair<int,int>> neighbor(std::pair<int,int> position)
{
    return {
        {position.first - 1, position.second},
        {position.first, position.second + 1},
        {position.first + 1, position.second},
        {position.first, position.second - 1}
    };
}

void circle(std::vector<Cell>& path, Cell p1, Cell p2, int num)
{
    if (num <= 0)
        return;

    auto firstIt = std::find(path.begin(), path.end(), p1);
    auto secondIt = std::find(path.begin(), path.end(), p2);
    if (firstIt == path.end() || secondIt == path.end() || firstIt == secondIt)
        return;

    if (firstIt > secondIt)
    {
        std::swap(firstIt, secondIt);
        std::swap(p1, p2);
    }
    //上面在调整p1,p2顺序
    // 直接复用通用 deep_find，寻找 p1 到 p2 的所有合法路径。
    std::vector<std::string> board = change_map;
    std::vector<Cell> currentPath;
    std::vector<std::vector<Cell>> allPaths;
    deep_find(p1, p2, board, currentPath, allPaths);
    if (allPaths.size()<2)
        return;

    std::vector<Cell> shortest = allPaths.front();
    std::vector<Cell> secondSHORT;

    for (const auto& candidate : allPaths)
    {
        if (candidate.size() < shortest.size())
        {
        shortest = candidate;
        }
    }

    for (const auto& candidate : allPaths)
    {
        if (candidate != shortest &&
             (secondSHORT.empty() ||
              candidate.size() < secondSHORT.size()))
         {
             secondSHORT = candidate;
          }
}

    if (secondSHORT.empty())
        secondSHORT = shortest;
    std::reverse(secondSHORT.begin(),secondSHORT.end());
    // 构造 p1 -> ... -> p2 -> ... -> p1 的回路。
    std::vector<Cell> loop;
    for (int round = 0; round < num; ++round)
    {
        loop.insert(loop.end(), shortest.begin() + 1, shortest.end());
        loop.insert(loop.end(),secondSHORT.begin()+1,secondSHORT.end());
    }

    // firstIt 仍指向原 path 的位置，插入点之前的迭代器不会失效。
    path.insert(firstIt + 1, loop.begin(), loop.end());
}
// ============================================================
//  下半部分 · 渲染引擎  【M1 学生完全跳过，别改】
//  ----------------------------------------------------------
//  这一整块都是"把游戏画到窗口上"的引擎代码（SFML 贴图 / 文字 / 形状）。
//  每个 drawXxx() 都是把当前的数据（地图、敌人坐标、生命…）当参数传进来，
//  照着画一遍而已。你不需要读懂，只要在 main() 里改数据，画面就会跟着变。
// ============================================================

// 每格的像素尺寸（非正方形没关系：关卡图是透视图，逻辑格对上大致位置即可）。
constexpr float CELL_W = float(BOARD_W) / GRID_W;   // ≈ 88.9
constexpr float CELL_H = float(BOARD_H) / GRID_H;   // = 112.5

// 背景图裁剪：关卡图是透视渲染，真正能走的地板只占画面中间那块（四周是管道/箱子等装饰）。
// drawMap 先按下面这个框只取「地板那块」，再拉伸铺满棋盘，让 9×4 网格落在地板上。
// 取值＝真实地板透视梯形四角的平均矩形（让中间两行对得最准）；想微调就改这 4 个数。
constexpr int MAP_CROP_X = 137, MAP_CROP_Y = 135;   // 裁剪框左上角（像素，基于 1280×720 原图）
constexpr int MAP_CROP_W = 955, MAP_CROP_H = 393;   // 裁剪框宽、高

// 小工具：格子(行,列) → 该格中心的像素坐标（画图时用来把东西摆到格子正中）。
sf::Vector2f cellCenter(int row, int col)
{
    return sf::Vector2f((col + 0.5f) * CELL_W, (row + 0.5f) * CELL_H);
}

// ---------- 资源（图片/字体，加载一次，画图函数内部用）----------
sf::Font    font;
bool        hasFont = false;
sf::Texture levelTex;     // 关卡背景图
sf::Texture slugTex;      // 敌人（源石虫）
bool        hasLevelTex = false;
bool        hasSlugTex  = false;

// 根据地形字符返回"叠加色"——半透明，盖在背景图上，让背景氛围透出来。
static sf::Color tileOverlayColor(char t)
{
    switch (t)
    {
        case '#': return sf::Color(235, 215, 150,  55);  // 道路(可走)：暖色薄雾，背景基本透出
        case 'S': return sf::Color(220,  60,  60, 120);  // 出怪口（红门）
        case 'E': return sf::Color( 60, 140, 230, 130);  // 终点（蓝门）
        case 'P': return sf::Color( 90, 150, 230,  70);  // 高台（怪不能走；将来可放远程干员）
        default:  return sf::Color( 10,  12,  20, 120);  // 障碍 '.'：压暗，表示不可走
    }
}

// 依次尝试几个常见字体路径；找到就能画文字。
bool loadAnyFont()
{
    const char* candidates[] = {
        "assets/font.otf",                                  // 自备（推荐，含中文）
        "C:/Windows/Fonts/font.ttf",                        // Windows 微软雅黑（含中文）
        "C:/Windows/Fonts/arial.ttf",                       // Windows 英文
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Linux
        "/System/Library/Fonts/Supplemental/Arial.ttf",     // macOS
    };
    for (const char* p : candidates)
        if (font.openFromFile(p)) return true;
    std::cerr << "[提示] 未找到字体，文字暂不显示。把任意字体放到 assets/font.otf 即可。\n";
    return false;
}

// 载入字体 + 美术素材（在 main 里调用一次）。失败自动兜底成纯色。
void loadAssets()
{
    hasFont = loadAnyFont();
    // 相对路径，相对于程序的"工作目录"——要在项目根目录下运行（见 README）。
    hasLevelTex = levelTex.loadFromFile("graphics/map/main_00-01.png");
    hasSlugTex  = slugTex.loadFromFile("graphics/enemy/Originium_Slug.png");
    if (!hasLevelTex)
        std::cerr << "[提示] 未找到关卡背景图 graphics/map/main_00-01.png，改用纯色格子。\n";
    if (!hasSlugTex)
        std::cerr << "[提示] 未找到敌人贴图 graphics/enemy/Originium_Slug.png，改用红圆点。\n";
}

// 画地图：背景图 + 逻辑网格（每格按地形上色 + 白色细边框）。
void drawMap(sf::RenderWindow& win, const std::vector<std::string>& map)
{
    // 第 1 层：有背景图就铺背景图（仅做氛围）；没有就填深色。
    if (hasLevelTex)
    {
        sf::Sprite bg(levelTex);
        bg.setTextureRect(sf::IntRect({MAP_CROP_X, MAP_CROP_Y}, {MAP_CROP_W, MAP_CROP_H}));    // 只取地板那块
        bg.setScale(sf::Vector2f(float(BOARD_W) / MAP_CROP_W, float(BOARD_H) / MAP_CROP_H));   // 裁剪块铺满棋盘
        bg.setPosition(sf::Vector2f(0.f, 0.f));
        win.draw(bg);

        sf::RectangleShape dim(sf::Vector2f{float(BOARD_W), float(BOARD_H)});
        dim.setFillColor(sf::Color(0, 0, 0, 60));   // 轻微压暗，让网格线更清楚
        win.draw(dim);
    }
    else
    {
        sf::RectangleShape base(sf::Vector2f{float(BOARD_W), float(BOARD_H)});
        base.setFillColor(sf::Color(30, 34, 44));
        win.draw(base);
    }

    // 第 2 层：真实逻辑网格——这才是游戏坐标的准绳，敌人就落在这套格子上。
    for (int r = 0; r < GRID_H; ++r)
        for (int c = 0; c < GRID_W; ++c)
        {
            sf::RectangleShape cell(sf::Vector2f(CELL_W, CELL_H));
            cell.setPosition(sf::Vector2f(c * CELL_W, r * CELL_H));
            cell.setFillColor(tileOverlayColor(map[r][c]));
            cell.setOutlineThickness(-1.f);                    // 向内 1px 边框
            cell.setOutlineColor(sf::Color(255, 255, 255, 50));
            win.draw(cell);
        }
}

// 画敌人：位置完全由传进来的 enemyRow/enemyCol 决定——你在 main 里改这俩，敌人就动。
void drawEnemy(sf::RenderWindow& win, bool enemyAlive,
               int enemyRow, int enemyCol, float stepTimer, float STEP_TIME)
{
    if (!enemyAlive) return;

    // —— 平滑移动（引擎侧小把戏，学生不用管）——
    // 学生的移动逻辑是"整数格瞬移"：enemyRow/enemyCol 一变就跳一格。这里让精灵不瞬移，
    // 而是在本格用时(STEP_TIME)内，从"上一格中心"平滑滑到"当前格中心"。
    // 只读传进来的坐标 + stepTimer，学生代码一字不用改。
    static int fromR = enemyRow, fromC = enemyCol;   // 正从哪一格滑出
    static int toR   = enemyRow, toC   = enemyCol;   // 正滑向哪一格（= 当前格）
    if (enemyRow != toR || enemyCol != toC)          // 学生把敌人挪了一格 → 记下新/旧格
    {
        fromR = toR;      fromC = toC;
        toR   = enemyRow; toC   = enemyCol;
    }
    float t = stepTimer / STEP_TIME;                 // 本格已走过的比例 0→1
    if (t > 1.f) t = 1.f;                            // 防御性夹取
    sf::Vector2f a = cellCenter(fromR, fromC);
    sf::Vector2f b = cellCenter(toR,   toC);
    sf::Vector2f center = a + (b - a) * t;           // 插值出的像素位置

    if (hasSlugTex)
    {
        sf::Sprite slug(slugTex);
        sf::Vector2u ts = slugTex.getSize();
        float scale = (CELL_W * 0.95f) / float(ts.x);
        slug.setScale(sf::Vector2f(scale, scale));
        slug.setOrigin(slug.getLocalBounds().size / 2.f);
        slug.setPosition(center);
        win.draw(slug);
    }
    else
    {
        float radius = CELL_H * 0.3f;                // 兜底：没贴图就画红圆点
        sf::CircleShape body(radius);
        body.setOrigin(sf::Vector2f(radius, radius));
        body.setFillColor(sf::Color(220, 60, 60));
        body.setPosition(center);
        win.draw(body);
    }
}

// 右侧信息栏：只放标题（M1 单敌人、不部署，进度/生命放在顶部条里）。
void drawHud(sf::RenderWindow& win)
{
    if (!hasFont) return;
    float hx = float(BOARD_W) + 12.f;

    auto line = [&](float y, const std::string& s, unsigned size) {
        // 用 fromUtf8 按 UTF-8 解析，中文才不乱码
        sf::Text t(font, sf::String::fromUtf8(s.begin(), s.end()), size);
        t.setFillColor(sf::Color::White);
        t.setPosition(sf::Vector2f(hx, y));
        win.draw(t);
    };
    line(10, "明日方舟",  30);
    line(40, "Arknights", 30);
}

// 顶部居中的状态条：进度（已清/总数）+ 剩余生命，垫一条半透明底便于看清。
void drawTopStatus(sf::RenderWindow& win, int enemiesTotal, int remainingEnemies, int life)
{
    if (!hasFont) return;
    int killed = enemiesTotal - remainingEnemies;
    std::string s = "进度 " + std::to_string(killed) + "/" + std::to_string(enemiesTotal)
                  + "    生命 " + std::to_string(life);
    sf::Text t(font, sf::String::fromUtf8(s.begin(), s.end()), 20);
    t.setStyle(sf::Text::Bold);
    t.setFillColor(life > 1 ? sf::Color(235, 235, 240) : sf::Color(245, 90, 90));

    sf::FloatRect b = t.getLocalBounds();
    float cx = float(BOARD_W + HUD_W) / 2.f, cy = 18.f;
    t.setOrigin(sf::Vector2f{b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f});
    t.setPosition(sf::Vector2f{cx, cy});

    sf::RectangleShape bg(sf::Vector2f{b.size.x + 28.f, b.size.y + 12.f});
    bg.setOrigin(bg.getSize() / 2.f);
    bg.setPosition(sf::Vector2f{cx, cy});
    bg.setFillColor(sf::Color(0, 0, 0, 130));
    win.draw(bg);
    win.draw(t);
}

// 游戏结束：盖一层半透明幕布 + 居中的"胜利/失败"提示。
void drawGameOver(sf::RenderWindow& win, bool victory)
{
    sf::Vector2u ws = win.getSize();
    sf::RectangleShape veil(sf::Vector2f{float(ws.x), float(ws.y)});
    veil.setFillColor(sf::Color(0, 0, 0, 160));
    win.draw(veil);

    if (!hasFont) return;

    auto centered = [&](const std::string& s, unsigned size, sf::Color col, float cy) {
        sf::Text t(font, sf::String::fromUtf8(s.begin(), s.end()), size);
        t.setFillColor(col);
        t.setStyle(sf::Text::Bold);
        sf::FloatRect b = t.getLocalBounds();
        t.setOrigin(b.position + b.size / 2.f);
        t.setPosition(sf::Vector2f(float(ws.x) / 2.f, cy));
        win.draw(t);
    };

    float midY = float(ws.y) / 2.f;
    if (victory)
    {
        centered("游戏胜利！", 52, sf::Color(90, 220, 130), midY - 20.f);
        centered("敌人已清空", 22, sf::Color(210, 210, 210), midY + 30.f);
    }
    else
    {
        centered("游戏失败！", 52, sf::Color(235, 80, 80), midY - 20.f);
        centered("生命值归零", 22, sf::Color(210, 210, 210), midY + 30.f);
    }
}
