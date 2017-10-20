using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ArudlemEditor
{
    public partial class MainForm : Form
    {
        #region parameters
        // number of sprites in the sprite map
        private const int MAP_SPRITE_WIDTH = 8;
        private const int MAP_SPRITE_HEIGHT = 4;
        // the zoom that should be applied on the original images used for the arduboy
        private const int IMAGE_ZOOM_SCALE = 16;
        #endregion

        #region variables
        private Bitmap m_MapSpriteImage = null;
		private Bitmap m_MapSpriteImageMirrored = null;
		private Pen m_MapSpriteLinesPen = new Pen(Color.Yellow, 4f);
        private Pen m_MapSpriteSelectionPen = new Pen(Color.Red, 4f);
        private Pen m_LevelLinesPen = new Pen(Color.Yellow, 1f);

        private int m_CurrentSpriteIndex = 0;

        private Level m_CurrentLevel = new Level();
        #endregion

        #region init
        public MainForm()
        {
            InitializeComponent();
            LoadMapSpriteImage();
            DrawLevel();
        }

        private void LoadMapSpriteImage()
        {
            // get the original image
            string mapSpritePath = Application.StartupPath + @"/../../../../Assets/Maps/MapSprite.png";
            Bitmap originalMapSpriteImage = new Bitmap(mapSpritePath);
            m_MapSpriteImage = new Bitmap(originalMapSpriteImage, originalMapSpriteImage.Size.Width * IMAGE_ZOOM_SCALE, originalMapSpriteImage.Size.Height * IMAGE_ZOOM_SCALE);
			// create the mirrored image from the loaded one
			CreateMirroredImage(m_MapSpriteImage);
            // and init the image
            DrawMapSprite();
        }

		private void CreateMirroredImage(Bitmap image)
		{
			// copy the image in the mirrored image
			m_MapSpriteImageMirrored = new Bitmap(image);

			// Create some image attribut to change the color of the mirrored image
			ImageAttributes imageAttributes = new ImageAttributes();
			float[][] colorMatrixElements = { 
			   new float[] {.5f,  0,  0,  0, 0},     // red scaling factor of 0
			   new float[] {0,  .7f,  0,  0, 0},     // green scaling factor of 0
			   new float[] {0,  0,  1,  0, 0},     // blue scaling factor of 1
			   new float[] {0,  0,  0,  1, 0},     // alpha scaling factor of 1
			   new float[] {0,  0,  0,  0, 1}};    // translations
			ColorMatrix colorMatrix = new ColorMatrix(colorMatrixElements);
			imageAttributes.SetColorMatrix(colorMatrix, ColorMatrixFlag.Default, ColorAdjustType.Bitmap);

			// paint the image in the mirrored one with the image attribute
			Graphics gc = Graphics.FromImage(m_MapSpriteImageMirrored);
			gc.DrawImage(image,		// source image
				new Rectangle(0, 0, image.Width, image.Height),  // destination rectangle 
				0, 0,				// upper-left corner of source rectangle 
				image.Width,		// width of source rectangle
				image.Height,		// height of source rectangle
				GraphicsUnit.Pixel,
				imageAttributes);

			// and mirror it
			m_MapSpriteImageMirrored.RotateFlip(RotateFlipType.RotateNoneFlipX);
		}
        #endregion

        #region drawing
        private SizeF GetMapSpriteCellSize(SizeF size)
        {
            return new SizeF(size.Width / MAP_SPRITE_WIDTH, size.Height / MAP_SPRITE_HEIGHT);
        }

        private SizeF GetLevelCellSize(SizeF size)
        {
            return new SizeF(size.Width / Level.LEVEL_WIDTH, size.Height / Level.LEVEL_HEIGHT);
        }

        private void DrawMapSpriteLines(Graphics gc, Size imageSise, int horizontalCellCount, int verticalCellCount, Pen pen)
        {
            float cellWidth = imageSise.Width / horizontalCellCount;
            float cellHeight = imageSise.Height / verticalCellCount;

            // get the x and y of the picture box
            float x = 0;
            float y = 0;

            // draw the vertical lines
            for (int i = 1; i < horizontalCellCount; ++i)
            {
                x = i * cellWidth;
                gc.DrawLine(pen, x, y, x, y + imageSise.Height);
            }

            // Draw the horizontal lines
            x = 0;
            for (int i = 1; i < verticalCellCount; ++i)
            {
                y = i * cellHeight;
                gc.DrawLine(pen, x, y, x + imageSise.Width, y);
            }
        }

        private void DrawMapSprite()
        {
            // resize the image to draw the lines
            this.MapSpritePictureBox.Image = new Bitmap(m_MapSpriteImage);
            
            // get the gc of the image
            Graphics gc = Graphics.FromImage(this.MapSpritePictureBox.Image);
            
            // draw the lines
            DrawMapSpriteLines(gc, this.MapSpritePictureBox.Image.Size, MAP_SPRITE_WIDTH, MAP_SPRITE_HEIGHT, m_MapSpriteLinesPen);

            // and draw the the selected sprite
            SizeF cellSize = GetMapSpriteCellSize(this.MapSpritePictureBox.Image.Size);
            gc.DrawRectangle(m_MapSpriteSelectionPen, (m_CurrentSpriteIndex / MAP_SPRITE_HEIGHT) * cellSize.Width, (m_CurrentSpriteIndex % MAP_SPRITE_HEIGHT) * cellSize.Height, cellSize.Width, cellSize.Height);
        }

        private void DrawLevel()
        {
            this.LevelPictureBox.Image = new Bitmap(Level.LEVEL_WIDTH * IMAGE_ZOOM_SCALE, Level.LEVEL_HEIGHT * IMAGE_ZOOM_SCALE);

            // get the gc of the image
            Graphics gc = Graphics.FromImage(this.LevelPictureBox.Image);

            // get the sprite cell size and level cell size
            SizeF spriteCellSize = GetMapSpriteCellSize(m_MapSpriteImage.Size);
            SizeF levelCellSize = GetLevelCellSize(this.LevelPictureBox.Image.Size);

            // iterate through the level data to paint the correct sprite
            for (int i = 0; i < Level.LEVEL_WIDTH; ++i)
                for (int j = 0; j < Level.LEVEL_HEIGHT; ++j)
                {
                    Rectangle levelRectangle = new Rectangle((int)(i * levelCellSize.Width), (int)(j * levelCellSize.Height), (int)levelCellSize.Width, (int)levelCellSize.Height);
                    if (m_CurrentLevel.IsSpriteSet(i, j))
                    {
                        int spriteIndex = m_CurrentLevel.GetSprite(i, j);
						if (m_CurrentLevel.IsSpriteMirrored(i, j))
						{
							Rectangle spriteRectangle = new Rectangle((int)((MAP_SPRITE_WIDTH-1 - (spriteIndex / MAP_SPRITE_HEIGHT)) * spriteCellSize.Width), (int)((spriteIndex % MAP_SPRITE_HEIGHT) * spriteCellSize.Height), (int)spriteCellSize.Width, (int)spriteCellSize.Height);
							gc.DrawImage(m_MapSpriteImageMirrored, levelRectangle, spriteRectangle, GraphicsUnit.Pixel);
						}
						else
						{
							Rectangle spriteRectangle = new Rectangle((int)((spriteIndex / MAP_SPRITE_HEIGHT) * spriteCellSize.Width), (int)((spriteIndex % MAP_SPRITE_HEIGHT) * spriteCellSize.Height), (int)spriteCellSize.Width, (int)spriteCellSize.Height);
							gc.DrawImage(m_MapSpriteImage, levelRectangle, spriteRectangle, GraphicsUnit.Pixel);
						}
                    }
                    else
                    {
                        gc.FillRectangle(Brushes.Black, levelRectangle);
                    }
                }

            // finally draw the grid on top of it
            if (this.drawLevelGridToolStripMenuItem.Checked)
                DrawMapSpriteLines(gc, this.LevelPictureBox.Image.Size, Level.LEVEL_WIDTH, Level.LEVEL_HEIGHT, m_LevelLinesPen);
        }
        #endregion

        #region form events from menus
        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // instantiate an empty new level
            m_CurrentLevel = new Level();
            DrawLevel();
        }

        private void loadLevelFromClipboardToolStripMenuItem_Click(object sender, EventArgs e)
        {
            bool isOk = m_CurrentLevel.LoadFromClipboard();
            if (!isOk)
            {
                MessageBox.Show("Error while parsing the clipboard, please try again to copy.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            
            // redraw the level
            DrawLevel();

            // set the values in the combo boxes
            this.LocaMapNameTextBox.Text = m_CurrentLevel.m_LocaMapName;
            this.MapIdsTextBox.Text = m_CurrentLevel.m_MapIdsName;
        }

        private void saveLevelToClipboardToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // set the values from the combo boxes to the current level before calling the save
            m_CurrentLevel.m_LocaMapName = this.LocaMapNameTextBox.Text;
            m_CurrentLevel.m_MapIdsName = this.MapIdsTextBox.Text;
            m_CurrentLevel.m_ShouldTrimLevelAtExport = this.TrimLevelCheckBox.Checked;

            // and copy the current level to clipboard
            m_CurrentLevel.SaveToClipboard();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void drawLevelGridToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DrawLevel();
        }
        #endregion

        #region click events

        private void MapSpritePictureBox_MouseClick(object sender, MouseEventArgs e)
        {
            SizeF cellSize = GetMapSpriteCellSize(this.MapSpritePictureBox.Size);

            // update the selected sprite index
            m_CurrentSpriteIndex = (int)(e.Y / cellSize.Height) + ((int)(e.X / cellSize.Width) * MAP_SPRITE_HEIGHT);

            // redraw the map sprite
            DrawMapSprite();
        }

        private void LevelPictureBox_MouseClick(object sender, MouseEventArgs e)
        {
            SizeF cellSize = GetLevelCellSize(this.LevelPictureBox.Size);

            // compute the coordinates
            int x = (int)(e.X / cellSize.Width);
            int y = (int)(e.Y / cellSize.Height);

            // set the current index in the correct column
			if (e.Button == System.Windows.Forms.MouseButtons.Left)
				m_CurrentLevel.SetSprite(x, y, m_CurrentSpriteIndex);
			else if (e.Button == System.Windows.Forms.MouseButtons.Right)
				m_CurrentLevel.ClearSprite(x, y);
			else if (e.Button == System.Windows.Forms.MouseButtons.Middle)
				m_CurrentLevel.InverseMirrorStatus(x, y);

            // redraw the map sprite
            DrawLevel();
        }
        #endregion

        #region event for parameter changes
        private void TrimLevelCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            m_CurrentLevel.m_ShouldTrimLevelAtExport = this.TrimLevelCheckBox.Checked;
        }
        #endregion
    }
}
