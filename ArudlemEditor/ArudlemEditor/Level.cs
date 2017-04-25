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

        // the variable names for the two arrays of the map
        public string m_LocaMapName = string.Empty;
        public string m_MapIdsName = string.Empty;

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
            if ((x >= 0) && (x < LEVEL_WIDTH) && (y >= 0) && (y < LEVEL_HEIGHT))
                m_Data[x, y] = spriteId;
        }

        public int GetSprite(int x, int y)
        {
            if ((x >= 0) && (x < LEVEL_WIDTH) && (y >= 0) && (y < LEVEL_HEIGHT))
                return m_Data[x, y];
            return -1;
        }

        public void ClearSprite(int x, int y)
        {
            if ((x >= 0) && (x < LEVEL_WIDTH) && (y >= 0) && (y < LEVEL_HEIGHT))
                m_Data[x, y] = -1;
        }

        public bool IsSpriteSet(int x, int y)
        {
            if ((x >= 0) && (x < LEVEL_WIDTH) && (y >= 0) && (y < LEVEL_HEIGHT))
                return m_Data[x, y] != -1;
            return false;
        }

        private string parseVariableName(string variableDeclaration, string variableType)
        {
            int startIndex = variableDeclaration.IndexOf(variableType) + variableType.Length;
            int endIndex = variableDeclaration.IndexOf('[');
            return variableDeclaration.Substring(startIndex, endIndex - startIndex).Trim();
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
            m_LocaMapName = parseVariableName(mapParts[0], "char");
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
            m_MapIdsName = parseVariableName(mapParts[1], "int");
            string[] spriteIds = tokenizeArray(mapParts[1], new string[]{"),"});

            // reset x and y
            x = 0;
            int y = -1;
            foreach (string sprite3Id in spriteIds)
            {
                // remove the begining
                string idsOnly = sprite3Id.Substring(sprite3Id.IndexOf('(') + 1);

                // remove eventual trailling bracket
                int endBracketIndex = idsOnly.IndexOf(')');
                if (endBracketIndex >= 0)
                    idsOnly = idsOnly.Remove(endBracketIndex);

                // split the ids
                string[] triplet = idsOnly.Split(',');
                foreach (string spriteId in triplet)
                {
                    // get the next valid coordinate and set the sprite id at the correct place in the data
                    if (GetNextValidCoordinate(ref x, ref y))
                        m_Data[x, y] = int.Parse(spriteId.Trim());
                }
            }

            return true;
        }

        public void SaveToClipboard()
        {
            // start the string with the declaration of the map
            string text = "const unsigned char " + m_LocaMapName + "[] PROGMEM = { ";

            // parse the data to compute the bitfield
            for (int i = 0; i < LEVEL_WIDTH; ++i)
            {
                // iterate on the whole column
                byte columnBitField = 0;
                for (int j = 0; j < LEVEL_HEIGHT; ++j)
                    if (m_Data[i, j] >= 0)
                        columnBitField |= (byte)(1 << j);

                // write the column once we iterated on the whole height
                text += "0x" + columnBitField.ToString("X") + ", ";
            }

            // now declare the map id variables
            text += "};\nconst unsigned int " + m_MapIdsName + "[] PROGMEM = {\n";

            // parse again the data to save the map ids
            int nbIds = 0;
            string triplet = "\tID(";
            for (int i = 0; i < LEVEL_WIDTH; ++i)
                for (int j = 0; j < LEVEL_HEIGHT; ++j)
                    if (m_Data[i, j] >= 0)
                    {
                        // increase the id count
                        nbIds++;
                        // add the id to the triplet
                        triplet += m_Data[i, j].ToString();
                        // add the comma or close the bracket
                        if ((nbIds % 3) != 0)
                        {
                            triplet += ",";
                        }
                        else
                        {
                            // close the bracket when we have 3 ids
                            triplet += ")";
                            // add the triplet to the text
                            text += triplet;

                            // and restart a new triplet (with go to next line every ten triplet)
                            if ((nbIds % 10) == 0)
                                triplet = ",\n\tID(";
                            else
                                triplet = ", ID(";
                        }                        
                    }

            // finish the last triplet if it is not finished
            if ((nbIds % 1) == 0)
            {
                triplet += "0,0)";
                text += triplet;
            }
            else if ((nbIds % 2) == 0)
            {
                triplet += "0)";
                text += triplet;
            }

            // close the variable definition
            text += "};\n";

            // and copy the text to the clipboard
            Clipboard.SetText(text);
        }
    }
}
