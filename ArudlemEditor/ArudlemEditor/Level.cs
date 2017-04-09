using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ArudlemEditor
{
    class Level
    {
        // number of cells in the level
        public const int LEVEL_WIDTH = 32;
        public const int LEVEL_HEIGHT = 8;

        // the level data
        private int[,] m_Data = new int[LEVEL_WIDTH, LEVEL_HEIGHT];

        public Level()
        {
            for (int i = 0; i < LEVEL_WIDTH; ++i)
                for (int j = 0; j < LEVEL_HEIGHT; ++j)
                    m_Data[i, j] = -1;
        }

        public void SetSprite(int x, int y, int spriteId)
        {
            m_Data[x, y] = spriteId;
        }

        public int GetSprite(int x, int y)
        {
            return m_Data[x, y];
        }

        public void ClearSprite(int x, int y)
        {
            m_Data[x, y] = -1;
        }

        public bool IsSpriteSet(int x, int y)
        {
            return m_Data[x, y] != -1;
        }
    }
}
