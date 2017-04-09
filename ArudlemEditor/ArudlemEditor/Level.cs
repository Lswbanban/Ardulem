using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

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
            Clear();
        }

        public void Clear()
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

        public bool LoadFromClipboard()
        {
            string text = Clipboard.GetText();
            string[] mapParts = text.Split(';');

            if (mapParts.Length < 2)
                return false;

            // clear the data
            Clear();

            // get the loca and the id
            int startIndex = mapParts[0].IndexOf('{') + 1;
            int endIndex = mapParts[0].IndexOf('}');
            string locaString = mapParts[0].Substring(startIndex, endIndex - startIndex);
            string[] loca = locaString.Split(',');

            // then iterate on all the loca id
            int x = 0;
            foreach (string locaId in loca)
            {
                // trim the white space
                string trimLocaId = locaId.Trim();
                if (trimLocaId == string.Empty)
                    continue;

                // get the id as int
                int id = Int32.Parse(trimLocaId.Substring(2), System.Globalization.NumberStyles.HexNumber);

                // fill the data with a 1 for now for every bit set
                for (int i = 0; i < 8; ++i)
                    if ((id & (1 << i)) != 0)
                        m_Data[x, i] = 1;

                // increment the x
                x++;
            }

            return true;
        }
    }
}
