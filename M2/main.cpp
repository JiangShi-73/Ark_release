/*
 * ============================================================
 *  塔防课程项目 
 *  主题: 《明日方舟》风格 2D 塔防
 * ============================================================
 *
 *  本文件分三部分，按下面的顺序阅读：
 *    第一部分 · 游戏数据 + M2 任务——先看数据和工具，只补全标有「★任务」的 5 个函数
 *    第二部分 · 运行框架——【本阶段跳过、不用修改】main() 会调用你写的函数
 *    第三部分 · 渲染引擎——【学生完全跳过、不用看懂，也别改】把游戏画到窗口上
 *
 * ============================================================
 */

#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

// ============================================================
//  第一部分 · 游戏数据 + M2 任务
//  常量和数据供下面的函数使用；只需补全标有「★任务」的 5 个函数。
// ============================================================

// ---------- 常量 ----------
constexpr int GRID_W = 9;    // 地图列数（对应关卡图）
constexpr int GRID_H = 4;    // 地图行数

// ---------- 游戏状态：下面这些普通变量，游戏逻辑（含你 M2 要写的函数）都能直接读写 ----------

// 地图布局（对应关卡图 main_00-01.png）：
//   'S' 出怪口   'E' 终点（蓝门）   '#' 怪物路径（也是「地面」：近战/重装可部署）
//   'P' 高台（怪不能走，可放远程：狙击/术师）   '.' 不可走、不可部署
// row行col列的地块可通过map[row][col]访问
std::vector<std::string> map = {
    ".PPPPPPP.",
    "E#P#####.",
    ".###P###S",
    ".PPPPPPP.",
};

int  dp   = 20; // 部署费用（Deployment Points，DP），初始 20，随时间自动增加

// ---------- 干员信息：干员编号 + 数据表 + 存储 + 引擎工具（请勿修改）----------
const int OP_COUNTS = 3;

// —— 干员数据表：每个干员一列，下标 = 干员编号（下面所有表【同一下标】对应【同一个干员】）——
// 想加干员 / 调数值，就在这几张表的对应位置一起改。
const int OP_SNIPER   = 0;   // 狙击
const int OP_DEFENDER = 1;   // 重装
const int OP_CASTER   = 2;   // 术师
const char*     OP_NAME[OP_COUNTS]   = { "巡林者", "黑角", "杜林" };          // 干员名
const int     OP_ROLE[OP_COUNTS]   = { OP_SNIPER, OP_DEFENDER, OP_CASTER };    // 职业（
const char*     OP_ASSET[OP_COUNTS]  = { "rangers","noircorne","durin" };    // 美术前缀：graphics/operator/<前缀>_avatar.png（按钮头像）/ _model.png（地图）
const int       OP_COST[OP_COUNTS]   = { 15, 10, 20 };                       // 部署费用（DP）
// 注：主题色由职业取（见渲染区 ROLE_COLOR）——数据表里不再逐个存颜色。

// 已部署干员：因为【同一干员最多同时一个在场】，就直接【按干员编号 op】记它在不在场、在哪一格。
// 下标 = 干员编号（和 OP_NAME/OP_COST 同一套编号）；不用「槽位列表 + opCount」，撤退也不用挪数组。
bool onField[OP_COUNTS] = { false, false, false };   // 干员 op 是否已部署在场
int  opRow[OP_COUNTS];                                // 在场时所在行（onField[op]=true 时才有意义；全局数组默认全 0）
int  opCol[OP_COUNTS];                                // 在场时所在列

// 撤退相关（引擎管理）：
const float DEPLOY_COOLDOWN = 30.f;              // 干员撤退/倒下后，多少秒才能再部署
float opCooldown[OP_COUNTS] = { 0.f, 0.f, 0.f };  // 每个干员的再部署冷却（秒），>0=冷却中

// ---------- 引擎工具（可直接调用）----------

// 编号 op 的干员是不是已经在场上了（同一干员同时只能有一个）。
bool isOpDeployed(int op)
{
    return onField[op];
}

// 查表返回编号 op 干员的部署费用（DP）。
int operatorCost(int op)
{
    return OP_COST[op];
}

// ---------- ★任务：下面这 5 个函数就是 M2 要你写的 ----------
//  能用的材料：OP_COST / OP_NAME / OP_ROLE 等顶部干员数据表（按干员编号 op 查）、
//  地图 map、格子数 GRID_W / GRID_H、当前部署费用 dp等变量，以及上面的引擎工具。


// ★任务(1) 格子可用性判断：(row,col) 这个格子上是不是已经站了干员。
// 如果格子上有干员，返回 true；否则返回 false。
bool isCellOccupied(int row, int col)
{
    //请补充代码，必要时可删掉下一行
    return false;
}

// ★任务(2) 放置规则：编号 op 的干员能不能放在 (row,col)？
//  看它的职业：重装 → 只能放地面 '#'；狙击/术师 → 只能放高台 'P'（。
// 如果能放，返回 true；否则返回 false（地形不对、越界都算不对）。
bool canPlaceHere(int row, int col, int op)
{
    //请补充代码，必要时可删掉下一行
    return false;
}

// ★任务(3) 上场：让编号 op 的干员上场，落在 (row,col)。
// 更新 onField / opRow / opCol 三个数组，表示它已经在场上了，并记录位置。
void placeOperator(int row, int col, int op)
{
    //请补充代码
}

// ★任务(4) 部署主逻辑：尝试在 (row,col) 放下编号 op 的干员。
//  该干员尚未被部署 + 地形合法 + 该格没人 + 部署费用够 → 放下并扣费、返回 true；否则返回 false（不扣费）。
bool tryPlaceOperator(int row, int col, int op)
{
    //请补充代码，必要时可删掉下一行
    return false;
}


// ★任务(5) 撤退：找到 (row,col) 上的干员，返还一半部署费用、进入再部署冷却、并让它下场。成功撤退则返回 true；没找到干员就返回 false。
// 需要修改的信息数组包括opCooldown / onField等
bool retreatOperatorAt(int row, int col)
{
    //请补充代码，必要时可删掉下一行
    return false;
}



// ============================================================
//  第二部分 · 运行框架（本阶段跳过，不用修改）
//  下面负责窗口、计时和鼠标操作；完成 M2 不需要读懂这些代码。
// ============================================================

// 棋盘显示尺寸：保持关卡图 1280x720 的 16:9 比例，避免背景图被拉伸变形。
constexpr int BOARD_W = 800;
constexpr int BOARD_H = 450;
// 每格的像素尺寸（非正方形没关系：关卡图是透视图，逻辑格对上大致位置即可）。
constexpr float CELL_W = float(BOARD_W) / GRID_W;   // ≈ 88.9
constexpr float CELL_H = float(BOARD_H) / GRID_H;   // = 112.5

constexpr int HUD_W    = 200;   // 右侧信息栏宽度
constexpr int DEPLOY_H = 96;    // 底部「部署栏」高度（放干员按钮）

// 背景图裁剪：关卡图是透视渲染，真正能走的地板只占画面中间那块（四周是管道/箱子等装饰）。
// drawMap 先按下面这个框只取「地板那块」，再拉伸铺满棋盘，让 9×4 网格落在地板上。
// 取值＝真实地板透视梯形四角的平均矩形（让中间两行对得最准）；想微调就改这 4 个数。
constexpr int MAP_CROP_X = 137, MAP_CROP_Y = 135;   // 裁剪框左上角（像素，基于 1280×720 原图）
constexpr int MAP_CROP_W = 955, MAP_CROP_H = 393;   // 裁剪框宽、高

int  life = 3;  // 生命值（敌人到终点就 -1，归零=失败）
int  remainingEnemies = 1;    // 剩余敌人数：初始 1，死一个 -1；归零=本关清完
int  enemiesTotal     = 1;    // 本关敌人总数（单敌人）；顶部「进度 已清/总数」用
bool gameOver = false;        // 游戏是否已结束（结束后 updateGame 直接返回、不再推进）
bool victory  = false;        // 结束时的结果：true=胜利, false=失败

// 敌人的基本信息
int   enemyRow = 2, enemyCol = 8;   // 敌人当前所在格：出生在出怪口 S =(行2, 列8)
bool  enemyAlive = true;            // 是否还在场上（到终点或被消灭后置 false）
float stepTimer  = 0.f;             // 计时器：攒够 STEP_TIME 秒就走一格（防止一帧冲到底）
const float STEP_TIME = 2.0f;       // 每格用时（调大走得慢、调小走得快）
float dpTimer  = 0.f;             //  计时器：攒够 DP_TIME 秒就 +1 部署费用
const float DP_TIME = 1.0f;       // 每秒增加多少部署费用（调大加得慢、调小加得快）


int selectedOp = -1;      // 当前选中的干员卡（-1 = 未选）

int   retreatRow = -1, retreatCol = -1;          // 正弹出撤退按钮的干员所在格（-1 = 没有）

// ---------- 资源（引擎用，请勿修改）----------
sf::Font    font;
bool        hasFont = false;
sf::Texture levelTex;     // 关卡背景图
sf::Texture slugTex;      // 敌人（源石虫）
bool        hasLevelTex = false;
bool        hasSlugTex  = false;
sf::Texture escapeTex;    // 撤退按钮图标（graphics/ui/escape.png）
bool        hasEscapeTex = false;
sf::Texture costTex;      // 部署费用图标（graphics/ui/img_cost.png）
bool        hasCostTex = false;

// 干员贴图（运行时加载，按干员编号存；不放进上面的 const 表里）
sf::Texture opAvatarTex[OP_COUNTS];     // 部署卡头像（_avatar 256² 方图，填满卡片）
sf::Texture opModelTex[OP_COUNTS];   // 地图上用（_model Q 版）
bool hasOpAvatar[OP_COUNTS]   = { false, false, false };
bool hasOpModel[OP_COUNTS] = { false, false, false };

// ---------- 渲染函数声明（实现在文件末尾，本阶段跳过）----------
void loadAssets();
sf::FloatRect retreatButtonRect(int row, int col);
sf::FloatRect deployCardRect(int op);
void drawMap(sf::RenderWindow& win);
void drawOperators(sf::RenderWindow& win);
void drawEnemy(sf::RenderWindow& win);
void drawRetreatButton(sf::RenderWindow& win);
void drawTopStatus(sf::RenderWindow& win);
void drawHud(sf::RenderWindow& win);
void drawDeployPanel(sf::RenderWindow& win);
void drawGameOver(sf::RenderWindow& win);

// ---------- 每帧更新（引擎调用）：DP 回升 + 敌人移动 + 判胜负 ----------
//  dt = 距上一帧过了多少秒。敌人的移动是 M1 的任务、已经写好，M2 不用改这里。
void updateGame(float dt)
{
    if (gameOver) return;   // 已分胜负：不再推进，画面定格

    // DP 每 DP_TIME 秒自动 +1（部署要花 DP）
    dpTimer += dt;
    if (dpTimer >= DP_TIME) { dpTimer -= DP_TIME; ++dp; }

    // 敌人移动（M1 已完成）：攒够 STEP_TIME 走一格；走到终点 E 就离场并扣 1 点生命。
    if (enemyAlive)
    {
        stepTimer += dt;
        if (stepTimer >= STEP_TIME)
        {
            stepTimer -= STEP_TIME;
            if (enemyRow == 1 && enemyCol == 0)   // 到终点 E（本图 E 在 (1,0)）→ 漏进蓝门
            {
                enemyAlive = false;
                --life;
                --remainingEnemies;
            }
            else if (enemyCol > 0        && (map[enemyRow][enemyCol-1] == '#' || map[enemyRow][enemyCol-1] == 'E')) --enemyCol;
            else if (enemyRow > 0        && (map[enemyRow-1][enemyCol] == '#' || map[enemyRow-1][enemyCol] == 'E')) --enemyRow;
            else if (enemyRow < GRID_H-1 && (map[enemyRow+1][enemyCol] == '#' || map[enemyRow+1][enemyCol] == 'E')) ++enemyRow;
            else if (enemyCol < GRID_W-1 && (map[enemyRow][enemyCol+1] == '#' || map[enemyRow][enemyCol+1] == 'E')) ++enemyCol;
        }
    }

    // 胜负判定（每帧）：生命归零 → 失败；敌人清空 → 胜利
    if (life <= 0)                  { gameOver = true; victory = false; }
    else if (remainingEnemies <= 0) { gameOver = true; victory = true;  }
}
// ---------- 程序入口 ----------
int main()
{
    sf::RenderWindow window(sf::VideoMode({unsigned(BOARD_W + HUD_W), unsigned(BOARD_H + DEPLOY_H)}),
                            "Arknights");
    window.setFramerateLimit(60);

    loadAssets();          // 载入字体/贴图（一次）
    sf::Clock clock;

    while (window.isOpen())
    {
        // SFML 3: pollEvent() 返回 std::optional<sf::Event>
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            // 鼠标左键：撤退按钮 / 干员卡 / 棋盘，分情况处理。
            if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mb->button == sf::Mouse::Button::Left && !gameOver)
                {
                    float mx = float(mb->position.x);
                    float my = float(mb->position.y);

                    // 1) 撤退按钮正弹着且点在按钮上 → 确认撤退（移走干员 + 进入再部署冷却）
                    if (retreatRow >= 0 &&
                        retreatButtonRect(retreatRow, retreatCol).contains(sf::Vector2f{mx, my}))
                    {
                        retreatOperatorAt(retreatRow, retreatCol);
                        retreatRow = retreatCol = -1;
                    }
                    else
                    {
                        // 2) 点干员卡 → 选中（顺便收起撤退按钮）
                        bool hitCard = false;
                        for (int op = 0; op < OP_COUNTS; op++)
                            if (deployCardRect(op).contains(sf::Vector2f{mx, my}))
                            {
                                selectedOp = op;
                                retreatRow = retreatCol = -1;
                                hitCard = true;
                                break;
                            }

                        // 3) 点棋盘
                        if (!hitCard && mx < BOARD_W && my < BOARD_H)
                        {
                            int col = int(mx / CELL_W);
                            int row = int(my / CELL_H);

                            // 3a) 点到已部署的干员 → 切换它头顶的撤退按钮（再点同一个 = 收起）
                            if (isCellOccupied(row, col))
                            {
                                if (retreatRow == row && retreatCol == col)
                                    retreatRow = retreatCol = -1;
                                else { retreatRow = row; retreatCol = col; }
                                selectedOp = -1;                 // 撤退与部署互斥
                            }
                            // 3b) 点空格子：收起撤退按钮；选了卡且过了「同一干员唯一 + 冷却」门槛就部署
                            else
                            {
                                retreatRow = retreatCol = -1;
                                if (selectedOp >= 0 &&
                                    !isOpDeployed(selectedOp) &&
                                    opCooldown[selectedOp] <= 0.f &&
                                    tryPlaceOperator(row, col, selectedOp))
                                    selectedOp = -1;             // 部署成功 → 取消选中
                            }
                        }
                    }
                }
            }
        }

        float dt = clock.restart().asSeconds();
        updateGame(dt);                    // 每帧推进游戏（DP/移动/胜负；M2 不用改它）

        // 干员再部署冷却倒计时（引擎维护，学生不用管）
        for (int op = 0; op < OP_COUNTS; op++)
            if (opCooldown[op] > 0.f) { opCooldown[op] -= dt; if (opCooldown[op] < 0.f) opCooldown[op] = 0.f; }

        window.clear(sf::Color(0, 0, 40));
        drawMap(window);
        drawOperators(window);
        drawEnemy(window);
        drawRetreatButton(window);         // 撤退按钮浮在干员头顶
        drawTopStatus(window);             // 顶部居中：进度 + 生命
        drawHud(window);
        drawDeployPanel(window);
        if (gameOver) drawGameOver(window);
        window.display();
    }
    return 0;
}

// ============================================================
//  第三部分 · 渲染引擎（无需阅读，请勿修改）
//  —— 全是「把游戏画到窗口上」的代码；直接读上面的全局数据，你改数据、画面就跟着变。 ——
// ============================================================

// 一个「函数」的范例：格子(行,列) → 该格中心的像素坐标。
// （M2 你会照着这个样子，自己写更多这样的函数。）
sf::Vector2f cellCenter(int row, int col)
{
    return sf::Vector2f((col + 0.5f) * CELL_W, (row + 0.5f) * CELL_H);
}

// 干员名字（供 UI 卡片用）——直接查上面的 OP_NAME 表。
const char* operatorName(int op)
{
    return OP_NAME[op];
}

// 职业主题色（渲染用）：每个职业一个色，下标 = 职业常量 OP_SNIPER/OP_DEFENDER/OP_CASTER。
// 干员颜色不再逐个存，而是按它的职业(OP_ROLE)取——加干员不用管颜色。
const sf::Color ROLE_COLOR[] = {
    sf::Color( 80, 170, 255),   // OP_SNIPER   狙击：蓝
    sf::Color(240, 200,  60),   // OP_DEFENDER 重装：黄
    sf::Color(200,  90, 230),   // OP_CASTER   术师：紫
};
// 查表返回编号 op 干员的颜色（= 它职业的主题色；画干员 / 卡片时用）。
sf::Color operatorColor(int op)
{
    return ROLE_COLOR[OP_ROLE[op]];
}


// 根据地形字符返回「叠加色」——半透明，盖在背景图上，让背景氛围透出来。
// 第四个参数是 alpha（0=全透明, 255=不透明）。
static sf::Color tileOverlayColor(char t)
{
    switch (t)
    {
        case '#': return sf::Color(235, 215, 150,  55);  // 道路(可走)：暖色薄雾，背景基本透出
        case 'S': return sf::Color(220,  60,  60, 120);  // 出怪口（红门）
        case 'E': return sf::Color( 60, 140, 230, 130);  // 终点（蓝门）
        case 'P': return sf::Color( 90, 150, 230,  70);  // 高台：可放「高台干员」，怪不能走
        default:  return sf::Color( 10,  12,  20, 120);  // 地面 '.'：压暗，表示不可走/不可部署
    }
}

// 依次尝试几个常见字体路径；找到就能画 HUD 文字。
bool loadAnyFont()
{
    const char* candidates[] = {
        "assets/font.otf",                                  // 自备（推荐，放一个含中文的字体）
        "C:/Windows/Fonts/msyh.ttc",                        // Windows 微软雅黑（含中文）
        "C:/Windows/Fonts/arial.ttf",                       // Windows 英文
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Linux
        "/System/Library/Fonts/Supplemental/Arial.ttf",     // macOS
    };
    for (const char* p : candidates)
        if (font.openFromFile(p)) return true;
    std::cerr << "[提示] 未找到字体，HUD 文字暂不显示。把任意字体放到 assets/font.otf 即可。\n";
    return false;
}

// 载入字体 + 美术素材到全局（在 main 里调用一次）。失败自动兜底。
void loadAssets()
{
    hasFont = loadAnyFont();
    // 相对路径，相对于程序的「工作目录」——见 README 里关于工作目录的说明。
    hasLevelTex = levelTex.loadFromFile("graphics/map/main_00-01.png");
    hasSlugTex  = slugTex.loadFromFile("graphics/enemy/Originium_Slug.png");
    if (!hasLevelTex)
        std::cerr << "[提示] 未找到关卡背景图 graphics/map/main_00-01.png，改用纯色格子。\n";
    if (!hasSlugTex)
        std::cerr << "[提示] 未找到敌人贴图 graphics/enemy/Originium_Slug.png，改用红圆点。\n";
    hasEscapeTex = escapeTex.loadFromFile("graphics/ui/escape.png");
    if (!hasEscapeTex)
        std::cerr << "[提示] 未找到撤退按钮图标 graphics/ui/escape.png，改用文字「撤」。\n";
    hasCostTex = costTex.loadFromFile("graphics/ui/img_cost.png");   // 部署费用图标（缺了就只显示数字）

    // 干员素材：每个干员各加载 _avatar（部署卡头像）和 _model（地图）。缺图自动兜底（色块）。
    for (int op = 0; op < OP_COUNTS; op++)
    {
        std::string base = std::string("graphics/operator/") + OP_ASSET[op];
        hasOpAvatar[op]   = opAvatarTex[op].loadFromFile(base + "_avatar.png");
        hasOpModel[op] = opModelTex[op].loadFromFile(base + "_model.png");
        if (!hasOpAvatar[op] || !hasOpModel[op])
            std::cerr << "[提示] 干员素材缺失：" << base << "_avatar/_model.png\n";
    }
}

void drawMap(sf::RenderWindow& win)
{
    // —— 第 1 层：底。有背景图就铺背景图（仅做氛围）；没有就填深色。——
    if (hasLevelTex)
    {
        sf::Sprite bg(levelTex);
        bg.setTextureRect(sf::IntRect({MAP_CROP_X, MAP_CROP_Y}, {MAP_CROP_W, MAP_CROP_H}));    // 只取地板那块
        bg.setScale(sf::Vector2f(float(BOARD_W) / MAP_CROP_W, float(BOARD_H) / MAP_CROP_H));   // 裁剪块铺满棋盘
        bg.setPosition(sf::Vector2f(0.f, 0.f));
        win.draw(bg);

        // 第 2 层：轻微压暗，让上面的网格线更清楚。
        sf::RectangleShape dim(sf::Vector2f{float(BOARD_W), float(BOARD_H)});
        dim.setFillColor(sf::Color(0, 0, 0, 60));
        win.draw(dim);
    }
    else
    {
        sf::RectangleShape base(sf::Vector2f{float(BOARD_W), float(BOARD_H)});
        base.setFillColor(sf::Color(30, 34, 44));
        win.draw(base);
    }

    // —— 第 3 层：真实逻辑网格。这才是游戏坐标的准绳，实体都落在这套格子上。——
    // 每格按地形上一层半透明色 + 一圈白色细边框，让 9×4 网格清清楚楚。
    for (int r = 0; r < GRID_H; ++r)
        for (int c = 0; c < GRID_W; ++c)
        {
            sf::RectangleShape cell(sf::Vector2f(CELL_W, CELL_H));
            cell.setPosition(sf::Vector2f(c * CELL_W, r * CELL_H));
            cell.setFillColor(tileOverlayColor(map[r][c]));
            cell.setOutlineThickness(-1.f);                    // 向内 1px 边框，不越界
            cell.setOutlineColor(sf::Color(255, 255, 255, 50));
            win.draw(cell);
        }
}

// 把敌人画在它当前所在的格子中心，位置完全由 enemyRow / enemyCol 决定。
// updateGame() 里改这两个变量，敌人就会跟着动——绘制这块不用学生操心。
void drawEnemy(sf::RenderWindow& win)
{
    if (!enemyAlive) return;

    // —— 平滑移动（引擎侧小把戏，学生不用管）——
    // updateGame() 里让 enemyRow/enemyCol「整数格瞬移」；这里让精灵不瞬移，
    // 而是在本格用时(STEP_TIME)内，从「上一格中心」平滑滑到「当前格中心」。
    static int fromR = enemyRow, fromC = enemyCol;   // 正从哪一格滑出
    static int toR   = enemyRow, toC   = enemyCol;   // 正滑向哪一格（= 当前格）
    if (enemyRow != toR || enemyCol != toC)          // 敌人挪了一格 → 记下新/旧格
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
        float scale = (CELL_W * 0.95f) / float(ts.x);   // 缩到约一格宽，保持原比例
        slug.setScale(sf::Vector2f(scale, scale));
        slug.setOrigin(slug.getLocalBounds().size / 2.f); // 原点移到图片中心，方便按格居中
        slug.setPosition(center);
        win.draw(slug);
    }
    else
    {
        // 兜底：没有敌人贴图就画红圆点。
        float radius = CELL_H * 0.3f;
        sf::CircleShape body(radius);
        body.setOrigin(sf::Vector2f(radius, radius));
        body.setFillColor(sf::Color(220, 60, 60));
        body.setPosition(center);
        win.draw(body);
    }
}

void drawHud(sf::RenderWindow& win)
{
    if (!hasFont) return;   // 没字体就跳过文字（地图仍会显示）
    float hx = float(BOARD_W) + 12.f;

    auto line = [&](float y, const std::string& s, sf::Color col, unsigned size = 16) {
        // 必须用 fromUtf8 按 UTF-8 解析，中文才不乱码（英文也照常工作）
        sf::Text t(font, sf::String::fromUtf8(s.begin(), s.end()), size);
        t.setFillColor(col);
        t.setPosition(sf::Vector2f(hx, y));
        win.draw(t);
    };

    line(8,  "明日方舟",  sf::Color::White, 22);
    line(32, "Arknights", sf::Color(200, 205, 215), 15);

    line(52,  "· 部署费用（右下角）",       sf::Color(185, 190, 200), 15);
    line(73,  "   随时间自动增加",          sf::Color(140, 145, 155), 13);
    line(92,  "   部署干员会消耗费用",       sf::Color(140, 145, 155), 13);

    line(124, "· 点干员卡 → 选中",         sf::Color(185, 190, 200), 15);
    line(146, "· 点格子 → 部署",           sf::Color(185, 190, 200), 15);
    line(167, "   重装→地面 / 术师狙击→高台",   sf::Color(140, 145, 155), 13);

    line(199, "· 点场上干员 → 撤退按钮",    sf::Color(185, 190, 200), 15);
    line(221, "   再点按钮确认撤退",         sf::Color(140, 145, 155), 13);
    line(240, "   撤退返还一半部署费用",      sf::Color(140, 145, 155), 13);
    line(259, "   干员死亡或撤退时",      sf::Color(140, 145, 155), 13);
    line(278, "   会进入30秒部署冷却",      sf::Color(140, 145, 155), 13);
}

// 正上方中间：进度（已清/总数）+ 剩余生命，叠在棋盘顶部、垫一条半透明底便于看清。
void drawTopStatus(sf::RenderWindow& win)
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

// 游戏结束时：盖一层半透明幕布 + 居中的「胜利/失败」提示。
void drawGameOver(sf::RenderWindow& win)
{
    sf::Vector2u ws = win.getSize();

    // 半透明黑幕，压住整个窗口。
    sf::RectangleShape veil(sf::Vector2f{float(ws.x), float(ws.y)});
    veil.setFillColor(sf::Color(0, 0, 0, 160));
    win.draw(veil);

    if (!hasFont) return;   // 没字体就只显示幕布，避免崩

    // 画一行「水平居中」的文字：原点移到文字包围盒中心，再放到窗口中线。
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

// 把已部署的干员画在各自格子中心（有 _model 立绘就用它，否则兜底色块）。
void drawOperators(sf::RenderWindow& win)
{
    for (int op = 0; op < OP_COUNTS; op++)
    {
        if (!onField[op]) continue;
        sf::Vector2f center = cellCenter(opRow[op], opCol[op]);

        if (hasOpModel[op])
        {
            sf::Sprite m(opModelTex[op]);
            sf::Vector2u ts = opModelTex[op].getSize();
            float scale = (CELL_H * 1.7f) / float(ts.y);   // Q 版画布带透明留白，缩到约 1.7 格高
            m.setScale(sf::Vector2f{scale, scale});
            m.setOrigin(m.getLocalBounds().size / 2.f);
            m.setPosition(center);
            win.draw(m);
        }
        else
        {
            float s = CELL_H * 0.42f;
            sf::RectangleShape body(sf::Vector2f{s, s});
            body.setOrigin(sf::Vector2f{s / 2.f, s / 2.f});
            body.setFillColor(operatorColor(op));
            body.setOutlineThickness(-2.f);
            body.setOutlineColor(sf::Color(20, 22, 30, 220));
            body.setPosition(center);
            win.draw(body);
        }
    }
}

// 撤退按钮的矩形：浮在 (row,col) 干员头顶（画按钮和判断点击共用）。
sf::FloatRect retreatButtonRect(int row, int col)
{
    const float bs = 46.f;                         // 按钮边长
    sf::Vector2f c = cellCenter(row, col);
    float top = c.y - CELL_H * 0.45f - bs / 2.f;   // 抬到干员头顶附近
    if (top < 2.f) top = 2.f;                      // 别顶出画面
    return sf::FloatRect({c.x - bs / 2.f, top}, {bs, bs});
}

// 点了场上干员后，在它头顶画一个「撤退」按钮（escape 图标 + 白框，表示可点）。
void drawRetreatButton(sf::RenderWindow& win)
{
    if (retreatRow < 0) return;
    sf::FloatRect r = retreatButtonRect(retreatRow, retreatCol);
    sf::Vector2f  c{ r.position.x + r.size.x / 2.f, r.position.y + r.size.y / 2.f };

    sf::RectangleShape frame(r.size);              // 白框底，突出「可点击」
    frame.setPosition(r.position);
    frame.setFillColor(sf::Color(20, 22, 28, 235));
    frame.setOutlineThickness(-2.f);
    frame.setOutlineColor(sf::Color(255, 255, 255, 220));
    win.draw(frame);

    if (hasEscapeTex)
    {
        sf::Sprite ic(escapeTex);
        sf::Vector2u ts = escapeTex.getSize();
        float scale = (r.size.x * 0.9f) / float(ts.x);
        ic.setScale(sf::Vector2f{scale, scale});
        ic.setOrigin(ic.getLocalBounds().size / 2.f);
        ic.setPosition(c);
        win.draw(ic);
    }
    else if (hasFont)
    {
        std::string s = "撤";
        sf::Text t(font, sf::String::fromUtf8(s.begin(), s.end()), 24);
        t.setStyle(sf::Text::Bold);
        t.setFillColor(sf::Color(235, 90, 90));
        sf::FloatRect b = t.getLocalBounds();
        t.setOrigin(b.position + b.size / 2.f);
        t.setPosition(c);
        win.draw(t);
    }
}

// 编号 op 的干员卡在右侧 HUD 里的矩形（画卡片和判断点击都用它，坐标只写一处）。
sf::FloatRect deployCardRect(int op)
{
    // 底部部署栏里，从左往右一排方形头像卡。
    float cardW = float(DEPLOY_H) - 12.f;   // 正方形卡（≈84）
    float gap   = 10.f;
    float x = 12.f + op * (cardW + gap);
    float y = float(BOARD_H) + 6.f;
    return sf::FloatRect({x, y}, {cardW, cardW});
}

// 明日方舟式「底部部署栏」：一排干员头像卡（用 _avatar 头像），高亮选中的那张。
void drawDeployPanel(sf::RenderWindow& win)
{
    // 部署栏背景条（横跨整个窗口底部）
    sf::RectangleShape bar(sf::Vector2f{float(BOARD_W + HUD_W), float(DEPLOY_H)});
    bar.setPosition(sf::Vector2f{0.f, float(BOARD_H)});
    bar.setFillColor(sf::Color(22, 26, 34));
    win.draw(bar);

    for (int op = 0; op < OP_COUNTS; op++)
    {
        sf::FloatRect r = deployCardRect(op);
        bool  sel     = (selectedOp == op);
        bool  afford  = (dp >= operatorCost(op));
        bool  onField = isOpDeployed(op);        // 已经有一个在场
        float cd      = opCooldown[op];            // 再部署冷却剩余秒数
        bool  enabled = !onField && cd <= 0.f && afford;

        // 卡片底
        sf::RectangleShape card(r.size);
        card.setPosition(r.position);
        card.setFillColor(sf::Color(45, 52, 66));
        win.draw(card);

        // 头像（_avatar 256² 方图，直接等比填满方形卡片，各卡天然对齐）
        if (hasOpAvatar[op])
        {
            sf::Sprite av(opAvatarTex[op]);
            sf::Vector2u ts = opAvatarTex[op].getSize();
            av.setScale(sf::Vector2f{ r.size.x / float(ts.x), r.size.y / float(ts.y) });
            av.setPosition(r.position);
            if (!enabled) av.setColor(sf::Color(110, 110, 110));  // 不可部署时压暗
            win.draw(av);
        }

        // 顶部一条「职业色」（用引擎提供的 operatorColor）
        sf::RectangleShape accent(sf::Vector2f{r.size.x, 5.f});
        accent.setPosition(r.position);
        accent.setFillColor(operatorColor(op));
        win.draw(accent);

        // 选中描边
        sf::RectangleShape frame(r.size);
        frame.setPosition(r.position);
        frame.setFillColor(sf::Color::Transparent);
        frame.setOutlineThickness(sel ? -3.f : -1.f);
        frame.setOutlineColor(sel ? sf::Color::White : sf::Color(90, 100, 120));
        win.draw(frame);

        // 已在场 / 冷却中：盖一层暗幕，再叠状态字（冷却=倒计时秒数，在场=「已部署」）
        if (onField || cd > 0.f)
        {
            sf::RectangleShape ov(r.size);
            ov.setPosition(r.position);
            ov.setFillColor(sf::Color(0, 0, 0, 120));
            win.draw(ov);
        }
        if (hasFont && cd > 0.f)
        {
            std::string s = std::to_string(int(std::ceil(cd)));
            sf::Text t(font, sf::String::fromUtf8(s.begin(), s.end()), 30);
            t.setStyle(sf::Text::Bold);
            t.setFillColor(sf::Color::White);
            sf::FloatRect b = t.getLocalBounds();
            t.setOrigin(b.position + b.size / 2.f);
            t.setPosition(sf::Vector2f{r.position.x + r.size.x / 2.f, r.position.y + r.size.y / 2.f});
            win.draw(t);
        }
        else if (hasFont && onField)
        {
            std::string s = "已部署";
            sf::Text t(font, sf::String::fromUtf8(s.begin(), s.end()), 14);
            t.setFillColor(sf::Color(190, 255, 190));
            sf::FloatRect b = t.getLocalBounds();
            t.setOrigin(b.position + b.size / 2.f);
            t.setPosition(sf::Vector2f{r.position.x + r.size.x / 2.f, r.position.y + r.size.y / 2.f});
            win.draw(t);
        }

        if (hasFont)
        {
            // 左上角费用（黄=买得起 / 灰=不够），带深色小底提高可读性
            sf::RectangleShape costBg(sf::Vector2f{28.f, 20.f});
            costBg.setPosition(sf::Vector2f{r.position.x + 2.f, r.position.y + 6.f});
            costBg.setFillColor(sf::Color(0, 0, 0, 150));
            win.draw(costBg);

            std::string cs = std::to_string(operatorCost(op));
            sf::Text tc(font, sf::String::fromUtf8(cs.begin(), cs.end()), 16);
            tc.setStyle(sf::Text::Bold);
            tc.setFillColor(afford ? sf::Color(255, 215, 80) : sf::Color(160, 160, 160));
            tc.setPosition(sf::Vector2f{r.position.x + 6.f, r.position.y + 5.f});
            win.draw(tc);

            // 底部名字条
            sf::RectangleShape nameBg(sf::Vector2f{r.size.x, 20.f});
            nameBg.setPosition(sf::Vector2f{r.position.x, r.position.y + r.size.y - 20.f});
            nameBg.setFillColor(sf::Color(0, 0, 0, 150));
            win.draw(nameBg);

            std::string nm = operatorName(op);
            sf::Text tn(font, sf::String::fromUtf8(nm.begin(), nm.end()), 14);
            tn.setFillColor(sf::Color::White);
            tn.setPosition(sf::Vector2f{r.position.x + 6.f, r.position.y + r.size.y - 21.f});
            win.draw(tn);
        }
    }

    // 右下角：部署费用数字 + 费用图标（img_cost）
    float iconSize = 44.f;
    float iconX = float(BOARD_W + HUD_W) - 12.f - iconSize;
    float iconY = float(BOARD_H) + (float(DEPLOY_H) - iconSize) / 2.f;
    float midY  = float(BOARD_H) + float(DEPLOY_H) / 2.f;
    if (hasCostTex)
    {
        sf::Sprite ic(costTex);
        sf::Vector2u ts = costTex.getSize();
        ic.setScale(sf::Vector2f{iconSize / float(ts.x), iconSize / float(ts.y)});
        ic.setPosition(sf::Vector2f{iconX, iconY});
        win.draw(ic);
    }
    if (hasFont)
    {
        std::string s = std::to_string(dp);
        sf::Text t(font, sf::String::fromUtf8(s.begin(), s.end()), 30);
        t.setStyle(sf::Text::Bold);
        t.setFillColor(sf::Color(255, 215, 80));
        sf::FloatRect b = t.getLocalBounds();
        t.setOrigin(sf::Vector2f{b.position.x + b.size.x, b.position.y + b.size.y / 2.f});  // 右-中对齐
        t.setPosition(sf::Vector2f{iconX - 8.f, midY});
        win.draw(t);
    }
}
