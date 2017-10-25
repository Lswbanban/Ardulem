﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ArudlemEditor
{
	class LevelData
	{
		private int m_StartX = 0;
		private int m_StartY = 0;
		private int m_HomeX = 0;
		private int m_HomeY = 0;
		private int m_TotalTimeIn10thSec = 0;
		private int m_SpawnLemCount = 0;
		private int m_RequiredLemCount = 0;
		private int m_MinDropSpeed = 0;
		private int m_Walker = 0;
		private int m_Blocker = 0;
		private int m_Bomber = 0;
		private int m_DiagDigger = 0;
		private int m_HorizDigger = 0;
		private int m_VertDigger = 0;
		private int m_Stairer = 0;
		private int m_Climber = 0;
		public int m_Parachuter = 0;
		public string m_LocaMapName = string.Empty;
		public string m_MapIdsName = string.Empty;

		public bool m_ExportWithWindowsEOL = true;

		public int StartX
		{
			get { return Clamp(m_StartX, 0, 239); }
			set { m_StartX = value; }
		}

		public int StartY
		{
			get { return Clamp(m_StartY, 0, 55); }
			set { m_StartY = value; }
		}

		public int HomeX
		{
			get { return Clamp(m_HomeX, 0, 240); }
			set { m_HomeX = value; }
		}

		public int HomeY
		{
			get { return Clamp(m_HomeY, 0, 47); }
			set { m_HomeY = value; }
		}

		public int TimeMin
		{
			get { return m_TotalTimeIn10thSec / 6; }
			set { m_TotalTimeIn10thSec = (value * 6) + (TimeSec / 10); }
		}

		public int TimeSec
		{
			get { return (m_TotalTimeIn10thSec % 6) * 10; }
			set { m_TotalTimeIn10thSec = (TimeMin * 6) + (value / 10); }
		}

		public int SpawnLemCount
		{
			get
			{
				if (m_SpawnLemCount == 0)
					return 5;
				return m_SpawnLemCount * 5;
			}
			set
			{
				m_SpawnLemCount = value / 5;
			}
		}

		public int RequiredLemCount
		{
			get
			{
				if (m_RequiredLemCount == 0)
					return 5;
				return m_RequiredLemCount * 5;
			}
			set
			{
				m_RequiredLemCount = value / 5;
			}
		}

		public int MinDropSpeed
		{
			get { return Clamp(m_MinDropSpeed, 0, 15); }
			set { m_MinDropSpeed = value; }
		}

		public int Walker
		{
			get { return Clamp(m_Walker, 0, 9); }
			set { m_Walker = value; }
		}

		public int Blocker
		{
			get { return Clamp(m_Blocker, 0, 9); }
			set { m_Blocker = value; }
		}

		public int Bomber
		{
			get { return Clamp(m_Bomber, 0, 9); }
			set { m_Bomber = value; }
		}

		public int DiagDigger
		{
			get { return Clamp(m_DiagDigger, 0, 9); }
			set { m_DiagDigger = value; }
		}

		public int HorizDigger
		{
			get { return Clamp(m_HorizDigger, 0, 9); }
			set { m_HorizDigger = value; }
		}

		public int VertDigger
		{
			get { return Clamp(m_VertDigger, 0, 9); }
			set { m_VertDigger = value; }
		}

		public int Stairer
		{
			get { return Clamp(m_Stairer, 0, 9); }
			set { m_Stairer = value; }
		}

		public int Climber
		{
			get { return Clamp(m_Climber, 0, 9); }
			set { m_Climber = value; }
		}

		public int Parachuter
		{
			get { return Clamp(m_Parachuter, 0, 9); }
			set { m_Parachuter = value; }
		}

		private static int Clamp(int value, int min, int max)  
		{  
			return (value < min) ? min : (value > max) ? max : value;  
		}

		private string GetEOL()
		{
			return (m_ExportWithWindowsEOL ? "\r\n" : "\n");
		}

		private bool ParseNextInt(string[] mapParts, ref int currentIndex, out int result)
		{
			while (currentIndex < mapParts.Length)
			{
				string token = mapParts[currentIndex].Trim();
				currentIndex++;
				if (int.TryParse(token, out result))
					return true;
			}
			result = 0;
			return false;
		}

		public bool LoadFromClipboard()
		{
			string text = Clipboard.GetText();
			
			// clamp before the open curved bracket
			int openBracketIndex = text.IndexOf('{');
			if (openBracketIndex >= 0)
				text = text.Substring(openBracketIndex + 1);

			// clamb after the closed curver bracket
			int closeBracketIndex = text.IndexOf('}');
			if (closeBracketIndex > 0)
				text = text.Remove(closeBracketIndex - 1);

			// them trim empty spaces
			text = text.Trim().Replace("\r\n","\n");

			// now remove the end line comments
			int startCommentIndex = text.IndexOf("//");
			while (startCommentIndex != -1)
			{
				// get the end of line (if theres no end of line, remove the rest of the text, because we found the beginning of the comment)
				int endCommentIndex = text.IndexOf('\n', startCommentIndex);
				if (endCommentIndex != -1)
					text = text.Substring(0, (startCommentIndex > 0) ? startCommentIndex - 1 : 0) + text.Substring((endCommentIndex < text.Length - 1) ? endCommentIndex + 1 : endCommentIndex);
				else
					text = text.Remove((startCommentIndex > 0) ? startCommentIndex - 1 : 0);
				// get the next comment
				startCommentIndex = text.IndexOf("//");
			}

			// tokenize with the comma
			string[] mapParts = text.Split(',');
			int currentIndex = 0;

			// get all the data one by one
			if (!ParseNextInt(mapParts, ref currentIndex, out m_StartX))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_StartY))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_HomeX))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_HomeY))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_TotalTimeIn10thSec))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_SpawnLemCount))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_RequiredLemCount))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_MinDropSpeed))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_Walker))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_Blocker))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_Bomber))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_DiagDigger))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_HorizDigger))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_VertDigger))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_Climber))
				return false;
			if (!ParseNextInt(mapParts, ref currentIndex, out m_Parachuter))
				return false;

			return true;
		}
	}
}
