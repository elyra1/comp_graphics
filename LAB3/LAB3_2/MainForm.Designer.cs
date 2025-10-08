namespace LAB3 {
    /// <summary>
    /// Рисование отрезков.
    /// </summary>
    partial class MainForm {
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.RadioButton radioButtonBresenham;
        private System.Windows.Forms.RadioButton radioButtonWu;
        private System.Windows.Forms.Button buttonReset;
        private System.Windows.Forms.Button buttonUndo;

        /// <summary>
        /// Освобождает ресурсы, используемые формой.
        /// </summary>
        /// <param name="disposing">Указывает, нужно ли освобождать управляемые ресурсы.</param>
        protected override void Dispose(bool disposing) {
            if (disposing && (components != null))
                components.Dispose();
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        private void InitializeComponent() {
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.radioButtonBresenham = new System.Windows.Forms.RadioButton();
            this.radioButtonWu = new System.Windows.Forms.RadioButton();
            this.buttonReset = new System.Windows.Forms.Button();
            this.buttonUndo = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.BackColor = System.Drawing.Color.White;
            this.pictureBox1.Location = new System.Drawing.Point(13, 3);
            this.pictureBox1.Margin = new System.Windows.Forms.Padding(4);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(1307, 646);
            this.pictureBox1.TabIndex = 4;
            this.pictureBox1.TabStop = false;
            this.pictureBox1.Paint += new System.Windows.Forms.PaintEventHandler(this.PictureBox1_Paint);
            this.pictureBox1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.PictureBox1_MouseClick);
            this.pictureBox1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseMove);
            // 
            // radioButtonBresenham
            // 
            this.radioButtonBresenham.Checked = true;
            this.radioButtonBresenham.Location = new System.Drawing.Point(86, 35);
            this.radioButtonBresenham.Margin = new System.Windows.Forms.Padding(4);
            this.radioButtonBresenham.Name = "radioButtonBresenham";
            this.radioButtonBresenham.Size = new System.Drawing.Size(133, 30);
            this.radioButtonBresenham.TabIndex = 1;
            this.radioButtonBresenham.TabStop = true;
            this.radioButtonBresenham.Text = "Брезенхем";
            this.radioButtonBresenham.CheckedChanged += new System.EventHandler(this.RadioButtonBresenham_CheckedChanged);
            // 
            // radioButtonWu
            // 
            this.radioButtonWu.Location = new System.Drawing.Point(227, 35);
            this.radioButtonWu.Margin = new System.Windows.Forms.Padding(4);
            this.radioButtonWu.Name = "radioButtonWu";
            this.radioButtonWu.Size = new System.Drawing.Size(67, 30);
            this.radioButtonWu.TabIndex = 2;
            this.radioButtonWu.Text = "Ву";
            this.radioButtonWu.CheckedChanged += new System.EventHandler(this.RadioButtonBresenham_CheckedChanged);
            // 
            // buttonReset
            // 
            this.buttonReset.Location = new System.Drawing.Point(302, 33);
            this.buttonReset.Margin = new System.Windows.Forms.Padding(4);
            this.buttonReset.Name = "buttonReset";
            this.buttonReset.Size = new System.Drawing.Size(133, 34);
            this.buttonReset.TabIndex = 3;
            this.buttonReset.Text = "Сброс";
            this.buttonReset.Click += new System.EventHandler(this.ButtonReset_Click);
            // 
            // buttonUndo
            // 
            this.buttonUndo.Location = new System.Drawing.Point(463, 33);
            this.buttonUndo.Margin = new System.Windows.Forms.Padding(4);
            this.buttonUndo.Name = "buttonUndo";
            this.buttonUndo.Size = new System.Drawing.Size(133, 34);
            this.buttonUndo.TabIndex = 3;
            this.buttonUndo.Text = "Отмена";
            this.buttonUndo.UseVisualStyleBackColor = true;
            this.buttonUndo.Click += new System.EventHandler(this.ButtonUndo_Click);
            // 
            // panel1
            // 
            this.panel1.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.panel1.Controls.Add(this.buttonUndo);
            this.panel1.Controls.Add(this.radioButtonBresenham);
            this.panel1.Controls.Add(this.buttonReset);
            this.panel1.Controls.Add(this.radioButtonWu);
            this.panel1.Location = new System.Drawing.Point(312, 656);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(697, 100);
            this.panel1.TabIndex = 5;
            // 
            // Form2
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1333, 800);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.pictureBox1);
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "Form2";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Рисование отрезка";
            this.Resize += new System.EventHandler(this.Form2_Resize);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private System.Windows.Forms.Panel panel1;
    }
}