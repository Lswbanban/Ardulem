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

        private string[] tokenizeArray(string arrayDefinition, string[] separators)
        {
            int startIndex = arrayDefinition.IndexOf('{') + 1;
            int endIndex = arrayDefinition.IndexOf('}');
            string arrayValues = arrayDefinition.Substring(startIndex, endIndex - startIndex);
            return arrayValues.Split(separators, StringSplitOptions.RemoveEmptyEntries);
        }

        private bool GetNextValidCoordinate(ref int x, ref int y)
        {
            // at least move to the next cell
            int i = x;
            int j = y + 1;
            if (j >= LEVEL_HEIGHT)
            {
                // if we reach the end of the column, move to the next column
                j = 0;
                i = x + 1;
                // if we reach the last colum, there's no more valide coord
                if (i >= LEVEL_WIDTH)
                    return false;
            }

            // iterate on the data until we found a valid sprite id
            for (; i < LEVEL_WIDTH; ++i)
                for (j = j % LEVEL_HEIGHT; j < LEVEL_HEIGHT; ++j)
                    if (m_Data[i,j] == 1)
                    {
                        // if we found a valide coord, set the coordinate and return true
                        x = i;
                        y = j;
                        return true;
                    }

            // if we reach the end without finding a new valid coordinate return false
            return false;
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
            string[] loca = tokenizeArray(mapParts[0], new string[]{","});

            // then iterate on all the loca id
            int x = 0;
            foreach (string locaId in loca)
            {
                // trim the white space
                string trimLocaId = locaId.Trim();
                if (trimLocaId == string.Empty)
                    continue;

                // get the id as int
                int id = int.Parse(trimLocaId.Substring(2), System.Globalization.NumberStyles.HexNumber);

                // fill the data with a 1 for now for every bit set
                for (int i = 0; i < 8; ++i)
                    if ((id & (1 << i)) != 0)
                        m_Data[x, i] = 1;

                // increment the x
                x++;
            }

            // now read the sprite ids
            string[] spriteIds = tokenizeArray(mapParts[1], new string[]{"),"});

            // reset x and y
            x = 0;
            int y = -1;
            foreach (string sprite3Id in spriteIds)
            {
                string[] triplet = sprite3Id.Substring(sprite3Id.IndexOf('(') + 1).Split(',');
                foreach (string spriteId in triplet)
                {
                    // get the next valid coordinate and set the sprite id at the correct place in the data
                    if (GetNextValidCoordinate(ref x, ref y))
                        m_Data[x, y] = int.Parse(spriteId.Trim());
                }
            }

            return true;
        }
    }
}
