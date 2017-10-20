﻿using System;
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
        private bool[,] m_Mirror = new bool[LEVEL_WIDTH, LEVEL_HEIGHT];

        // the variable names for the two arrays of the map
        public string m_LocaMapName = string.Empty;
        public string m_MapIdsName = string.Empty;
        public bool m_ShouldTrimLevelAtExport = true;

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

		public void InverseMirrorStatus(int x, int y)
		{
			m_Mirror[x, y] = !m_Mirror[x, y];
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
                    if (m_Data[i,j] >= 0)
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
			bool isReadingMirrorData = false;
            foreach (string sprite3Id in spriteIds)
            {
                // remove the begining
                string idsOnly = sprite3Id.Substring(sprite3Id.IndexOf('(') + 1);

                // remove eventual trailling bracket
                int endBracketIndex = idsOnly.IndexOf(')');
                if (endBracketIndex >= 0)
                    idsOnly = idsOnly.Remove(endBracketIndex);

				if (sprite3Id.TrimStart().StartsWith("ID"))
				{
					// split the ids
					string[] triplet = idsOnly.Split(',');
					foreach (string spriteId in triplet)
					{
						// get the next valid coordinate and set the sprite id at the correct place in the data
						if (GetNextValidCoordinate(ref x, ref y))
							m_Data[x, y] = int.Parse(spriteId.Trim());
					}
				}
				else
				{
					// if we enter for the first time in the mirror list, set the flag and reset x and y
					if (!isReadingMirrorData)
					{
						x = 0;
						y = -1;
						isReadingMirrorData = true;
					}

					// this is the mirror data, set the mirror status for the first 16 bits
					int mirrorValue = int.Parse(idsOnly.Trim());
					for (int i = 0; i < 16; i++)						
						if (GetNextValidCoordinate(ref x, ref y))
							m_Mirror[x, y] = (mirrorValue & (1 << i)) != 0;
				}
            }

            return true;
        }

        public void SaveToClipboard()
        {
            // start the string with the declaration of the map
            string text = "const unsigned char " + m_LocaMapName + "[] PROGMEM = { ";

            // determines the start and end column depending if we need to trim the border
            int startI = 0;
            int endI = LEVEL_WIDTH-1;

            // modify the start and end if we need to trim the level
            if (m_ShouldTrimLevelAtExport)
            {
                bool wasStartIFound = false;
                // do a first blanck iteration to find start and end column of the level
                for (int i = 0; i < LEVEL_WIDTH; ++i)
                    for (int j = 0; j < LEVEL_HEIGHT; ++j)
                        if (m_Data[i, j] >= 0)
                        {
                            // set the start i if it was never set yet
                            if (!wasStartIFound)
                            {
                                wasStartIFound = true;
                                startI = i;
                            }
                            // always set the end i, as we want the most one on the right
                            endI = i;
                        }
            }

            // parse the data to compute the bitfield, from start to end found column
            for (int i = startI; i <= endI; ++i)
            {
                // iterate on the whole column
                byte columnBitField = 0;
                for (int j = 0; j < LEVEL_HEIGHT; ++j)
                    if (m_Data[i, j] >= 0)
                        columnBitField |= (byte)(1 << j);

                // write the column once we iterated on the whole height
                text += "0x" + columnBitField.ToString("X") + ", ";
            }

            // we will use a list of bool, to store each mirror status, in the right order
            List<bool> mirrorList = new List<bool>();

            // now declare the map id variables
            text += "};\nconst unsigned int " + m_MapIdsName + "[] PROGMEM = {\n";

            // parse again the data to save the map ids
            int nbIds = 0;
            string triplet = "\tID(";
            for (int i = startI; i <= endI; ++i)
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

                        // also memorise the mirror status in the flat list
                        mirrorList.Add(m_Mirror[i, j]);
                    }

            // finish the last triplet if it is not finished
            int orpheanIdsCount = (nbIds % 3);
            if (orpheanIdsCount == 1)
            {
                triplet += "0,0)";
                text += triplet;
            }
            else if (orpheanIdsCount == 2)
            {
                triplet += "0)";
                text += triplet;
            }

            // Now save the Mirror values in a series of bitfield of 16 bits
            int currentSpriteId = 0;
            int bitfield = 0;
			foreach (bool isMirrored in mirrorList)
			{
				// set the bit in the bitfielf is this sprite is mirrored
				if (isMirrored)
					bitfield |= (1 << currentSpriteId);

				// increment the current sprite id
				currentSpriteId++;

				// check if the bitfield is full
				if (currentSpriteId == 16)
				{
					// write the bitfield
					text += ", MIRROR(" + bitfield.ToString() + ")";
					// reset the bitfield and the currentSprite id
					bitfield = 0;
					currentSpriteId = 0;
				}
			}
			// write the last bitfield if any remaining
			if ((mirrorList.Count > 0) && ((mirrorList.Count % 16) != 0))
				text += ", MIRROR(" + bitfield.ToString() + ")";

            // close the variable definition
            text += "};\n";

            // and copy the text to the clipboard
            Clipboard.SetText(text);
        }
    }
}
