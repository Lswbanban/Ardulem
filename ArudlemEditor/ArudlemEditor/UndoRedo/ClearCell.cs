﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ArudlemEditor.UndoRedo
{
	class ClearCell : Action
	{
		private int m_X = 0;
		private int m_Y = 0;
		private int m_OriginalId = -1;

		public ClearCell(Level level, int x, int y)
		{
			m_Level = level;
			m_X = x;
			m_Y = y;
			m_OriginalId = level.GetSprite(x, y);
		}

		public override void Undo()
		{
			m_Level.SetSprite(m_X, m_Y, m_OriginalId);
		}

		public override void Redo()
		{
			m_Level.ClearSprite(m_X, m_Y);
		}
	}
}
