using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
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
            // and init the image
            DrawMapSprite();
        }
        #endregion

        #region drawing
        private Size GetMapSpriteCellSize(Size size)
        {
            return new Size(size.Width / MAP_SPRITE_WIDTH, size.Height / MAP_SPRITE_HEIGHT);
        }

        private Size GetLevelCellSize(Size size)
        {
            return new Size(size.Width / Level.LEVEL_WIDTH, size.Height / Level.LEVEL_HEIGHT);
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
            Size cellSize = GetMapSpriteCellSize(this.MapSpritePictureBox.Image.Size);
            gc.DrawRectangle(m_MapSpriteSelectionPen, (m_CurrentSpriteIndex % MAP_SPRITE_WIDTH) * cellSize.Width, (m_CurrentSpriteIndex / MAP_SPRITE_WIDTH) * cellSize.Height, cellSize.Width, cellSize.Height);
        }

        private void DrawLevel()
        {
            this.LevelPictureBox.Image = new Bitmap(Level.LEVEL_WIDTH * IMAGE_ZOOM_SCALE, Level.LEVEL_HEIGHT * IMAGE_ZOOM_SCALE);

            // get the gc of the image
            Graphics gc = Graphics.FromImage(this.LevelPictureBox.Image);

            // get the sprite cell size and level cell size
            Size spriteCellSize = GetMapSpriteCellSize(m_MapSpriteImage.Size);
            Size levelCellSize = GetLevelCellSize(this.LevelPictureBox.Image.Size);

            // iterate through the level data to paint the correct sprite
            for (int i = 0; i < Level.LEVEL_WIDTH; ++i)
                for (int j = 0; j < Level.LEVEL_HEIGHT; ++j)
                {
                    Rectangle levelRectangle = new Rectangle(i * levelCellSize.Width, j * levelCellSize.Height, levelCellSize.Width, levelCellSize.Height);
                    if (m_CurrentLevel.IsSpriteSet(i, j))
                    {
                        int spriteIndex = m_CurrentLevel.GetSprite(i, j);
                        Rectangle spriteRectangle = new Rectangle((spriteIndex % MAP_SPRITE_WIDTH) * spriteCellSize.Width, (spriteIndex / MAP_SPRITE_WIDTH) * spriteCellSize.Height, spriteCellSize.Width, spriteCellSize.Height);
                        gc.DrawImage(m_MapSpriteImage, levelRectangle, spriteRectangle, GraphicsUnit.Pixel);
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
            if (isOk)
                DrawLevel();
            else
                MessageBox.Show("Error while parsing the clipboard, please try again to copy.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
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
            Size cellSize = GetMapSpriteCellSize(this.MapSpritePictureBox.Size);

            // update the selected sprite index
            m_CurrentSpriteIndex = (e.X / cellSize.Width) + ((e.Y / cellSize.Height) * MAP_SPRITE_WIDTH);

            // redraw the map sprite
            DrawMapSprite();
        }

        private void LevelPictureBox_MouseClick(object sender, MouseEventArgs e)
        {
            Size cellSize = GetLevelCellSize(this.LevelPictureBox.Size);

            // compute the coordinates
            int x = (e.X / cellSize.Width);
            int y = (e.Y / cellSize.Height);

            // set the current index in the correct column
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
                m_CurrentLevel.SetSprite(x, y, m_CurrentSpriteIndex);
            else if (e.Button == System.Windows.Forms.MouseButtons.Right)
                m_CurrentLevel.ClearSprite(x, y);

            // redraw the map sprite
            DrawLevel();
        }
        #endregion
    }
}
