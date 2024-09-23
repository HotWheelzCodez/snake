#include <fstream>
#include <iostream>
#include <vector>

#include "../include/raylib.h"

#define SCREEN_DIM 600

#define SECTOR_DIM 15

void RenderGrid(void)
{
  for (uint8_t y = 0; y < SCREEN_DIM/SECTOR_DIM; y++)
    for (uint8_t x = 0; x < SCREEN_DIM/SECTOR_DIM; x++)
      DrawRectangleLinesEx({ (float)x*SECTOR_DIM, (float)y*SECTOR_DIM, SECTOR_DIM, SECTOR_DIM }, 0.25f, WHITE);
}

class Snake
{
public:
  Snake(void) 
    : m_Body(std::move(std::vector<Vector2>(5))), m_Velocity({ 1, 0 })
  { 
    uint8_t length = SCREEN_DIM/SECTOR_DIM;
    uint8_t offset = 0;

    for (Vector2& bodyPart : m_Body)
    {
      bodyPart.y = length/2;
      bodyPart.x = (length/2)-offset;
      offset++;
    }
  }

  [[nodiscard]] bool UpdateAndRender(Vector2& foodPos)
  {
    Vector2 prevPosition = m_Body[0];

    if (prevPosition.x < 0 || prevPosition.x > SCREEN_DIM/SECTOR_DIM || prevPosition.y < 0 || prevPosition.y > SCREEN_DIM/SECTOR_DIM)
    {
      for (const Vector2& bodyPart : m_Body)
        DrawRectangle(bodyPart.x*SECTOR_DIM, bodyPart.y*SECTOR_DIM, SECTOR_DIM, SECTOR_DIM, GREEN);
      return true;
    }

    for (size_t i = 1; i < m_Body.size(); i++) 
    {
      if (m_Body[i].x == prevPosition.x && m_Body[i].y == prevPosition.y)
      {
        for (const Vector2& bodyPart : m_Body)
          DrawRectangle(bodyPart.x*SECTOR_DIM, bodyPart.y*SECTOR_DIM, SECTOR_DIM, SECTOR_DIM, GREEN);
        return true;
      }
    }

    if (prevPosition.x == foodPos.x && prevPosition.y == foodPos.y)
    {
      const Vector2 lastPosition = m_Body[m_Body.size()-1];
      const Vector2 secondLastPosition = m_Body[m_Body.size()-2];

      if (secondLastPosition.y < lastPosition.y)
        m_Body.push_back({ lastPosition.x, lastPosition.y-1 });
      else if (secondLastPosition.y > lastPosition.y)
        m_Body.push_back({ lastPosition.x, lastPosition.y+1 });
      else if (secondLastPosition.x > lastPosition.x)
        m_Body.push_back({ lastPosition.x-1, lastPosition.y });
      else
        m_Body.push_back({ lastPosition.x+1, lastPosition.y });

      foodPos = { (float)GetRandomValue(0, (SCREEN_DIM/SECTOR_DIM)-1), (float)GetRandomValue(0, (SCREEN_DIM/SECTOR_DIM)-1) };
    }

    int key = GetKeyPressed();

    switch (key)
    {
      case 'W':
        if (!m_Velocity.y)
        {
          m_Velocity.y = -1;
          m_Velocity.x = 0;
        }
        break;
      case 265: // Up arrow
        if (!m_Velocity.y)
        {
          m_Velocity.y = -1;
          m_Velocity.x = 0;
        }
        break;
      case 'S':
        if (!m_Velocity.y)
        {
          m_Velocity.y = 1;
          m_Velocity.x = 0;
        }
        break;
      case 264: // Down arrow
        if (!m_Velocity.y)
        {
          m_Velocity.y = 1;
          m_Velocity.x = 0;
        }
        break;
      case 'A':
        if (!m_Velocity.x)
        {
          m_Velocity.x = -1;
          m_Velocity.y = 0;
        }
        break;
      case 263: // Left arrow
        if (!m_Velocity.x)
        {
          m_Velocity.x = -1;
          m_Velocity.y = 0;
        }
        break;
      case 'D':
        if (!m_Velocity.x)
        {
          m_Velocity.x = 1;
          m_Velocity.y = 0;
        }
        break;
      case 262: // Right arrow
        if (!m_Velocity.x)
        {
          m_Velocity.x = 1;
          m_Velocity.y = 0;
        }
        break;
    }

    m_Body[0].x += m_Velocity.x;
    m_Body[0].y += m_Velocity.y;

    for (size_t i = 1; i < m_Body.size(); i++)
    {
      Vector2 temp = m_Body[i];
      m_Body[i] = prevPosition;
      prevPosition = temp;
    }

    for (const Vector2& bodyPart : m_Body)
      DrawRectangle(bodyPart.x*SECTOR_DIM, bodyPart.y*SECTOR_DIM, SECTOR_DIM, SECTOR_DIM, GREEN);

    return false;
  }

  [[nodiscard]] size_t GetScore(void) const
  {
    return m_Body.size()-5; 
  }

private:
  Vector2 m_Velocity;
  std::vector<Vector2> m_Body;
};

[[nodiscard]] size_t GetHighScore(void)
{
  std::string filePath = "save.txt";
  std::fstream file(filePath);
  size_t highScore = 0;

  if (!file.is_open() || file.bad())
  {
    std::cerr << "ERROR: " << filePath << " Does not exist or could not be opened!\n";
    return highScore;
  }

  std::string line;
  while (std::getline(file, line)) { highScore = std::stoll(line); }
  
  file.close();
  return highScore;
}

void SaveHighScore(size_t score)
{
  std::string filePath = "save.txt";
  std::fstream file(filePath);

  if (!file.is_open() || file.bad())
  {
    std::cerr << "ERROR: " << filePath << " Does not exist or could not be opened!\n";
    return;
  }

  size_t highScore = GetHighScore();

  if (score > highScore)
    file << score;

  file.close();
}

void SetScore(size_t score)
{
  std::string filePath = "save.txt";
  std::fstream file(filePath);

  if (!file.is_open() || file.bad())
  {
    std::cerr << "ERROR: " << filePath << " Does not exist or could not be opened!\n";
    return;
  }

  file << score;
  file.close();
}

bool Button(Rectangle bounds, Color color, const char* text, int fontSize)
{
  if (CheckCollisionPointRec(GetMousePosition(), bounds))
  {
    DrawRectangleRec(bounds, BLACK);
    DrawRectangle(bounds.x+10, bounds.y-10, bounds.width, bounds.height, color);

    int textWidth = MeasureText(text, fontSize);
    DrawText(text, (bounds.x+bounds.width/2)-(textWidth/2)+10, (bounds.y+bounds.height/2)-(fontSize/2)-10, fontSize, WHITE);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      return true;
    else
      return false;
  }

  DrawRectangleRec(bounds, color);
  int textWidth = MeasureText(text, fontSize);
  DrawText(text, (bounds.x+bounds.width/2)-(textWidth/2), (bounds.y+bounds.height/2)-fontSize/2, fontSize, WHITE);

  return false;
}

int main(void)
{
  InitWindow(SCREEN_DIM, SCREEN_DIM, "Snake");
  SetTargetFPS(10);

  bool checkReset = false;
  bool reset = false;

  Snake snake;
  Vector2 foodPos = { (float)GetRandomValue(0, (SCREEN_DIM/SECTOR_DIM)-1), (float)GetRandomValue(0, (SCREEN_DIM/SECTOR_DIM)-1) };

  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(BLACK);

    RenderGrid();

    DrawRectangle(foodPos.x*SECTOR_DIM, foodPos.y*SECTOR_DIM, SECTOR_DIM, SECTOR_DIM, RED);
    if (snake.UpdateAndRender(foodPos))
    {
      if (!reset)
        SaveHighScore(snake.GetScore());

      const char* gameOverDisplay = "GAME OVER";
      int gameOverDisplayWidth = MeasureText(gameOverDisplay, 60);
      DrawText(gameOverDisplay, (SCREEN_DIM/2)-(gameOverDisplayWidth/2), SCREEN_DIM/2, 60, RED);

      const char* infoDisplay = "Press 'SPACE' to continue";
      int infoDisplayWidth = MeasureText(infoDisplay, 40);
      DrawText(infoDisplay, (SCREEN_DIM/2)-(infoDisplayWidth/2), (SCREEN_DIM/2)+60, 40, WHITE);

      const char* resetDisplay = "Press 'r' to reset high score";
      int resetDisplayWidth = MeasureText(resetDisplay, 30);
      DrawText(resetDisplay, (SCREEN_DIM/2)-(resetDisplayWidth/2), (SCREEN_DIM/2)+100, 30, WHITE);

      int key = GetKeyPressed();

      if (key == 32)
      {
        
        snake = Snake(); // Reset the snake game
        foodPos = { (float)GetRandomValue(0, (SCREEN_DIM/SECTOR_DIM)-1), (float)GetRandomValue(0, (SCREEN_DIM/SECTOR_DIM)-1) };
        reset = false;
      }
      else if ((char)key == 'R')
      {
        checkReset = true;
      }

      if (checkReset)
      {
        DrawRectangle(100, 100, SCREEN_DIM-200, SCREEN_DIM-200, BLUE);

        const char* questionDisplay = "Are you sure?";
        int questionDisplayWidth = MeasureText(questionDisplay, 30);
        DrawText(questionDisplay, (SCREEN_DIM/2)-(questionDisplayWidth/2), 100, 30, WHITE);

        if (Button({ 150, 400, 100, 20 }, GREEN, "YES", 20))
        {
          SetScore(0);
          checkReset = false;
          reset = true;
        }

        if (Button({ 350, 400, 100, 20 }, RED, "NO", 20))
        {
          checkReset = false;
        }
      }
    }

    std::string scoreDisplay = "Score: " + std::to_string(snake.GetScore());
    int scoreDisplayWidth = MeasureText(scoreDisplay.c_str(), 40);
    DrawText(scoreDisplay.c_str(), (SCREEN_DIM/2)-(scoreDisplayWidth/2), 20, 40, WHITE);

    std::string highScoreDisplay = "High Score: " + std::to_string(GetHighScore());
    int highScoreDisplayWidth = MeasureText(highScoreDisplay.c_str(), 15);
    DrawText(highScoreDisplay.c_str(), (SCREEN_DIM/2)-(highScoreDisplayWidth/2), 60, 15, WHITE);

    EndDrawing();
  }

  CloseWindow();
}
