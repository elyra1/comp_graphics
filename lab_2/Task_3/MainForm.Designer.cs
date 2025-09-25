namespace LAB2
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
            this.button_loadFile = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.button_saveFile = new System.Windows.Forms.Button();
            this.trackBar_Hue = new System.Windows.Forms.TrackBar();
            this.trackBar_Saturation = new System.Windows.Forms.TrackBar();
            this.trackBar_Value = new System.Windows.Forms.TrackBar();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.label1 = new System.Windows.Forms.Label();
            this.textBox_Hue = new System.Windows.Forms.TextBox();
            this.textBox_Saturation = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_Value = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.button_reset = new System.Windows.Forms.Button();
            this.tableLayoutPanelBottom = new System.Windows.Forms.TableLayoutPanel();
            this.tableLayoutPanelHueControl = new System.Windows.Forms.TableLayoutPanel();
            this.tableLayoutPanelSaturationControl = new System.Windows.Forms.TableLayoutPanel();
            this.tableLayoutPanelValueControl = new System.Windows.Forms.TableLayoutPanel();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_Hue)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_Saturation)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_Value)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.tableLayoutPanelBottom.SuspendLayout();
            this.tableLayoutPanelHueControl.SuspendLayout();
            this.tableLayoutPanelSaturationControl.SuspendLayout();
            this.tableLayoutPanelValueControl.SuspendLayout();
            this.SuspendLayout();
            // 
            // button_loadFile
            // 
            this.button_loadFile.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button_loadFile.Location = new System.Drawing.Point(3, 45);
            this.button_loadFile.Name = "button_loadFile";
            this.button_loadFile.Size = new System.Drawing.Size(358, 45);
            this.button_loadFile.TabIndex = 2;
            this.button_loadFile.Text = "Загрузить файл...";
            this.button_loadFile.UseVisualStyleBackColor = true;
            this.button_loadFile.Click += new System.EventHandler(this.button_loadFile_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.Filter = "Image Files|*.png;*.jpg;*.jpeg;*.bmp";
            // 
            // button_saveFile
            // 
            this.button_saveFile.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button_saveFile.Enabled = false;
            this.button_saveFile.Location = new System.Drawing.Point(367, 45);
            this.button_saveFile.Name = "button_saveFile";
            this.button_saveFile.Size = new System.Drawing.Size(358, 45);
            this.button_saveFile.TabIndex = 3;
            this.button_saveFile.Text = "Сохранить файл...";
            this.button_saveFile.UseVisualStyleBackColor = true;
            this.button_saveFile.Click += new System.EventHandler(this.button_saveFile_Click);
            // 
            // trackBar_Hue
            // 
            this.trackBar_Hue.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.trackBar_Hue.BackColor = System.Drawing.SystemColors.Control;
            this.trackBar_Hue.Enabled = false;
            this.trackBar_Hue.Location = new System.Drawing.Point(82, 3);
            this.trackBar_Hue.Maximum = 180;
            this.trackBar_Hue.Minimum = -180;
            this.trackBar_Hue.Name = "trackBar_Hue";
            this.trackBar_Hue.Size = new System.Drawing.Size(273, 30);
            this.trackBar_Hue.TabIndex = 4;
            this.trackBar_Hue.TickFrequency = 30;
            this.trackBar_Hue.Scroll += new System.EventHandler(this.trackBar_Scroll);
            this.trackBar_Hue.ValueChanged += new System.EventHandler(this.trackBar_ValueChanged);
            // 
            // trackBar_Saturation
            // 
            this.trackBar_Saturation.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.trackBar_Saturation.Enabled = false;
            this.trackBar_Saturation.Location = new System.Drawing.Point(81, 3);
            this.trackBar_Saturation.Maximum = 100;
            this.trackBar_Saturation.Minimum = -100;
            this.trackBar_Saturation.Name = "trackBar_Saturation";
            this.trackBar_Saturation.Size = new System.Drawing.Size(274, 30);
            this.trackBar_Saturation.TabIndex = 5;
            this.trackBar_Saturation.TickFrequency = 10;
            this.trackBar_Saturation.Scroll += new System.EventHandler(this.trackBar_Scroll);
            this.trackBar_Saturation.ValueChanged += new System.EventHandler(this.trackBar_ValueChanged);
            // 
            // trackBar_Value
            // 
            this.trackBar_Value.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.trackBar_Value.Enabled = false;
            this.trackBar_Value.Location = new System.Drawing.Point(81, 3);
            this.trackBar_Value.Maximum = 100;
            this.trackBar_Value.Minimum = -100;
            this.trackBar_Value.Name = "trackBar_Value";
            this.trackBar_Value.Size = new System.Drawing.Size(277, 30);
            this.trackBar_Value.TabIndex = 6;
            this.trackBar_Value.TickFrequency = 10;
            this.trackBar_Value.Scroll += new System.EventHandler(this.trackBar_Scroll);
            this.trackBar_Value.ValueChanged += new System.EventHandler(this.trackBar_ValueChanged);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBox1.Location = new System.Drawing.Point(12, 12);
            this.pictureBox1.Margin = new System.Windows.Forms.Padding(4);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(1071, 531);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox1.TabIndex = 1;
            this.pictureBox1.TabStop = false;
            // 
            // label1
            // 
            this.label1.Anchor = System.Windows.Forms.AnchorStyles.Left;
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 10);
            this.label1.Margin = new System.Windows.Forms.Padding(3);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(17, 16);
            this.label1.TabIndex = 8;
            this.label1.Text = "H";
            // 
            // textBox_Hue
            // 
            this.textBox_Hue.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox_Hue.Location = new System.Drawing.Point(26, 7);
            this.textBox_Hue.Name = "textBox_Hue";
            this.textBox_Hue.ReadOnly = true;
            this.textBox_Hue.Size = new System.Drawing.Size(50, 22);
            this.textBox_Hue.TabIndex = 9;
            this.textBox_Hue.Text = "0";
            this.textBox_Hue.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // textBox_Saturation
            // 
            this.textBox_Saturation.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox_Saturation.Location = new System.Drawing.Point(25, 7);
            this.textBox_Saturation.Name = "textBox_Saturation";
            this.textBox_Saturation.ReadOnly = true;
            this.textBox_Saturation.Size = new System.Drawing.Size(50, 22);
            this.textBox_Saturation.TabIndex = 11;
            this.textBox_Saturation.Text = "0";
            this.textBox_Saturation.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label2
            // 
            this.label2.Anchor = System.Windows.Forms.AnchorStyles.Left;
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(3, 10);
            this.label2.Margin = new System.Windows.Forms.Padding(3);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(16, 16);
            this.label2.TabIndex = 10;
            this.label2.Text = "S";
            // 
            // textBox_Value
            // 
            this.textBox_Value.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox_Value.Location = new System.Drawing.Point(25, 7);
            this.textBox_Value.Name = "textBox_Value";
            this.textBox_Value.ReadOnly = true;
            this.textBox_Value.Size = new System.Drawing.Size(50, 22);
            this.textBox_Value.TabIndex = 13;
            this.textBox_Value.Text = "0";
            this.textBox_Value.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label3
            // 
            this.label3.Anchor = System.Windows.Forms.AnchorStyles.Left;
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(3, 10);
            this.label3.Margin = new System.Windows.Forms.Padding(3);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(16, 16);
            this.label3.TabIndex = 12;
            this.label3.Text = "V";
            // 
            // saveFileDialog1
            // 
            this.saveFileDialog1.Filter = "PNG Image|*.png|JPEG Image|*.jpg";
            // 
            // button_reset
            // 
            this.button_reset.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button_reset.Location = new System.Drawing.Point(731, 45);
            this.button_reset.Name = "button_reset";
            this.button_reset.Size = new System.Drawing.Size(361, 45);
            this.button_reset.TabIndex = 14;
            this.button_reset.Text = "Сброс";
            this.button_reset.UseVisualStyleBackColor = true;
            this.button_reset.Click += new System.EventHandler(this.button_reset_Click);
            // 
            // tableLayoutPanelBottom
            // 
            this.tableLayoutPanelBottom.ColumnCount = 3;
            this.tableLayoutPanelBottom.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33F));
            this.tableLayoutPanelBottom.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33F));
            this.tableLayoutPanelBottom.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.34F));
            this.tableLayoutPanelBottom.Controls.Add(this.tableLayoutPanelHueControl, 0, 0);
            this.tableLayoutPanelBottom.Controls.Add(this.tableLayoutPanelSaturationControl, 1, 0);
            this.tableLayoutPanelBottom.Controls.Add(this.tableLayoutPanelValueControl, 2, 0);
            this.tableLayoutPanelBottom.Controls.Add(this.button_loadFile, 0, 1);
            this.tableLayoutPanelBottom.Controls.Add(this.button_saveFile, 1, 1);
            this.tableLayoutPanelBottom.Controls.Add(this.button_reset, 2, 1);
            this.tableLayoutPanelBottom.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.tableLayoutPanelBottom.Location = new System.Drawing.Point(0, 559);
            this.tableLayoutPanelBottom.Name = "tableLayoutPanelBottom";
            this.tableLayoutPanelBottom.RowCount = 2;
            this.tableLayoutPanelBottom.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 45.97701F));
            this.tableLayoutPanelBottom.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 54.02299F));
            this.tableLayoutPanelBottom.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanelBottom.Size = new System.Drawing.Size(1095, 93);
            this.tableLayoutPanelBottom.TabIndex = 15;
            // 
            // tableLayoutPanelHueControl
            // 
            this.tableLayoutPanelHueControl.ColumnCount = 3;
            this.tableLayoutPanelHueControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.tableLayoutPanelHueControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 56F));
            this.tableLayoutPanelHueControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelHueControl.Controls.Add(this.label1, 0, 0);
            this.tableLayoutPanelHueControl.Controls.Add(this.textBox_Hue, 1, 0);
            this.tableLayoutPanelHueControl.Controls.Add(this.trackBar_Hue, 2, 0);
            this.tableLayoutPanelHueControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanelHueControl.Location = new System.Drawing.Point(3, 3);
            this.tableLayoutPanelHueControl.Name = "tableLayoutPanelHueControl";
            this.tableLayoutPanelHueControl.RowCount = 1;
            this.tableLayoutPanelHueControl.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelHueControl.Size = new System.Drawing.Size(358, 36);
            this.tableLayoutPanelHueControl.TabIndex = 16;
            // 
            // tableLayoutPanelSaturationControl
            // 
            this.tableLayoutPanelSaturationControl.ColumnCount = 3;
            this.tableLayoutPanelSaturationControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.tableLayoutPanelSaturationControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 56F));
            this.tableLayoutPanelSaturationControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelSaturationControl.Controls.Add(this.label2, 0, 0);
            this.tableLayoutPanelSaturationControl.Controls.Add(this.textBox_Saturation, 1, 0);
            this.tableLayoutPanelSaturationControl.Controls.Add(this.trackBar_Saturation, 2, 0);
            this.tableLayoutPanelSaturationControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanelSaturationControl.Location = new System.Drawing.Point(367, 3);
            this.tableLayoutPanelSaturationControl.Name = "tableLayoutPanelSaturationControl";
            this.tableLayoutPanelSaturationControl.RowCount = 1;
            this.tableLayoutPanelSaturationControl.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelSaturationControl.Size = new System.Drawing.Size(358, 36);
            this.tableLayoutPanelSaturationControl.TabIndex = 17;
            // 
            // tableLayoutPanelValueControl
            // 
            this.tableLayoutPanelValueControl.ColumnCount = 3;
            this.tableLayoutPanelValueControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.tableLayoutPanelValueControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 56F));
            this.tableLayoutPanelValueControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelValueControl.Controls.Add(this.label3, 0, 0);
            this.tableLayoutPanelValueControl.Controls.Add(this.textBox_Value, 1, 0);
            this.tableLayoutPanelValueControl.Controls.Add(this.trackBar_Value, 2, 0);
            this.tableLayoutPanelValueControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanelValueControl.Location = new System.Drawing.Point(731, 3);
            this.tableLayoutPanelValueControl.Name = "tableLayoutPanelValueControl";
            this.tableLayoutPanelValueControl.RowCount = 1;
            this.tableLayoutPanelValueControl.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelValueControl.Size = new System.Drawing.Size(361, 36);
            this.tableLayoutPanelValueControl.TabIndex = 18;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1095, 652);
            this.Controls.Add(this.tableLayoutPanelBottom);
            this.Controls.Add(this.pictureBox1);
            this.MinimumSize = new System.Drawing.Size(750, 480);
            this.Name = "MainForm";
            this.Text = "RGB2HSV";
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_Hue)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_Saturation)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_Value)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.tableLayoutPanelBottom.ResumeLayout(false);
            this.tableLayoutPanelHueControl.ResumeLayout(false);
            this.tableLayoutPanelHueControl.PerformLayout();
            this.tableLayoutPanelSaturationControl.ResumeLayout(false);
            this.tableLayoutPanelSaturationControl.PerformLayout();
            this.tableLayoutPanelValueControl.ResumeLayout(false);
            this.tableLayoutPanelValueControl.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Button button_loadFile;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Button button_saveFile;
        private System.Windows.Forms.TrackBar trackBar_Hue;
        private System.Windows.Forms.TrackBar trackBar_Saturation;
        private System.Windows.Forms.TrackBar trackBar_Value;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBox_Hue;
        private System.Windows.Forms.TextBox textBox_Saturation;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBox_Value;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.Button button_reset;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanelBottom;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanelHueControl;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanelSaturationControl;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanelValueControl;
    }
}