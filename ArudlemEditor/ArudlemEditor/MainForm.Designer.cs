namespace ArudlemEditor
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.loadLevelFromClipboardToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveLevelToClipboardToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.drawLevelGridToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.MapSpritePictureBox = new System.Windows.Forms.PictureBox();
            this.LevelPictureBox = new System.Windows.Forms.PictureBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.LocaMapNameTextBox = new System.Windows.Forms.TextBox();
            this.MapIdsTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.MapSpritePictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.LevelPictureBox)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(733, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.loadLevelFromClipboardToolStripMenuItem,
            this.saveLevelToClipboardToolStripMenuItem,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
            this.newToolStripMenuItem.Text = "New Level";
            this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
            // 
            // loadLevelFromClipboardToolStripMenuItem
            // 
            this.loadLevelFromClipboardToolStripMenuItem.Name = "loadLevelFromClipboardToolStripMenuItem";
            this.loadLevelFromClipboardToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
            this.loadLevelFromClipboardToolStripMenuItem.Text = "Load Level from Clipboard";
            this.loadLevelFromClipboardToolStripMenuItem.Click += new System.EventHandler(this.loadLevelFromClipboardToolStripMenuItem_Click);
            // 
            // saveLevelToClipboardToolStripMenuItem
            // 
            this.saveLevelToClipboardToolStripMenuItem.Name = "saveLevelToClipboardToolStripMenuItem";
            this.saveLevelToClipboardToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
            this.saveLevelToClipboardToolStripMenuItem.Text = "Save Level to Clipboard";
            this.saveLevelToClipboardToolStripMenuItem.Click += new System.EventHandler(this.saveLevelToClipboardToolStripMenuItem_Click);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.drawLevelGridToolStripMenuItem});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.Size = new System.Drawing.Size(39, 20);
            this.editToolStripMenuItem.Text = "Edit";
            // 
            // drawLevelGridToolStripMenuItem
            // 
            this.drawLevelGridToolStripMenuItem.Checked = true;
            this.drawLevelGridToolStripMenuItem.CheckOnClick = true;
            this.drawLevelGridToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
            this.drawLevelGridToolStripMenuItem.Name = "drawLevelGridToolStripMenuItem";
            this.drawLevelGridToolStripMenuItem.Size = new System.Drawing.Size(156, 22);
            this.drawLevelGridToolStripMenuItem.Text = "Draw Level Grid";
            this.drawLevelGridToolStripMenuItem.Click += new System.EventHandler(this.drawLevelGridToolStripMenuItem_Click);
            // 
            // splitContainer1
            // 
            this.splitContainer1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 24);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.groupBox1);
            this.splitContainer1.Size = new System.Drawing.Size(733, 422);
            this.splitContainer1.SplitterDistance = 192;
            this.splitContainer1.TabIndex = 1;
            // 
            // splitContainer2
            // 
            this.splitContainer2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.MapSpritePictureBox);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.LevelPictureBox);
            this.splitContainer2.Size = new System.Drawing.Size(733, 192);
            this.splitContainer2.SplitterDistance = 191;
            this.splitContainer2.TabIndex = 0;
            // 
            // MapSpritePictureBox
            // 
            this.MapSpritePictureBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MapSpritePictureBox.Location = new System.Drawing.Point(0, 0);
            this.MapSpritePictureBox.Name = "MapSpritePictureBox";
            this.MapSpritePictureBox.Size = new System.Drawing.Size(187, 188);
            this.MapSpritePictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.MapSpritePictureBox.TabIndex = 0;
            this.MapSpritePictureBox.TabStop = false;
            this.MapSpritePictureBox.MouseClick += new System.Windows.Forms.MouseEventHandler(this.MapSpritePictureBox_MouseClick);
            // 
            // LevelPictureBox
            // 
            this.LevelPictureBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LevelPictureBox.Location = new System.Drawing.Point(0, 0);
            this.LevelPictureBox.Name = "LevelPictureBox";
            this.LevelPictureBox.Size = new System.Drawing.Size(534, 188);
            this.LevelPictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.LevelPictureBox.TabIndex = 0;
            this.LevelPictureBox.TabStop = false;
            this.LevelPictureBox.MouseClick += new System.Windows.Forms.MouseEventHandler(this.LevelPictureBox_MouseClick);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.MapIdsTextBox);
            this.groupBox1.Controls.Add(this.LocaMapNameTextBox);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(10, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(188, 209);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Variable Names";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 22);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(55, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Loca Map";
            // 
            // LocaMapNameTextBox
            // 
            this.LocaMapNameTextBox.Location = new System.Drawing.Point(79, 19);
            this.LocaMapNameTextBox.Name = "LocaMapNameTextBox";
            this.LocaMapNameTextBox.Size = new System.Drawing.Size(98, 20);
            this.LocaMapNameTextBox.TabIndex = 1;
            // 
            // MapIdsTextBox
            // 
            this.MapIdsTextBox.Location = new System.Drawing.Point(79, 45);
            this.MapIdsTextBox.Name = "MapIdsTextBox";
            this.MapIdsTextBox.Size = new System.Drawing.Size(98, 20);
            this.MapIdsTextBox.TabIndex = 2;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(8, 45);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(45, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Map Ids";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(733, 446);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "MainForm";
            this.Text = "Ardulem Editor";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.MapSpritePictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.LevelPictureBox)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem loadLevelFromClipboardToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveLevelToClipboardToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.PictureBox MapSpritePictureBox;
        private System.Windows.Forms.PictureBox LevelPictureBox;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem drawLevelGridToolStripMenuItem;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox MapIdsTextBox;
        private System.Windows.Forms.TextBox LocaMapNameTextBox;
        private System.Windows.Forms.Label label1;
    }
}

