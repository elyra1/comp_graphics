using System;
using System.Drawing;
using System.Windows.Forms;

namespace Lab5
{
    public partial class MainForm : Form
    {

        LSystem lSystem;
        string filePath = @"..\..\L-systems\F_Koch.txt";

        public MainForm()
        {
            InitializeComponent();
            this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.DoubleBuffer, true);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            LoadLSystemFromFile(filePath);
            cmbFractals.SelectedIndex = 0;
        }

        private void LoadLSystemFromFile(string path)
        {
            try
            {
                lSystem = LSystem.FromFile(path);
                Invalidate();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Ошибка при чтении файла: " + ex.Message);
            }
        }

        private void Form1_Paint(object sender, PaintEventArgs e)
        {
            if (lSystem != null)
            {
                e.Graphics.Clear(Color.White);

                float padding = 50f;

                LSystemDrawer drawer = new LSystemDrawer(e.Graphics, lSystem.Angle);

                drawer.Draw(lSystem.Generate(), PointF.Empty, padding, lSystem.InitialDirection, lSystem.RandomBranching ? lSystem.Angle / 2 : 0);
            }
        }

        private void cmbFractals_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch (cmbFractals.SelectedIndex)
            {
                case 0: filePath = @"..\..\L-systems\F_Koch.txt"; break;
                case 1: filePath = @"..\..\L-systems\F_Island_Koch.txt"; break;
                case 2: filePath = @"..\..\L-systems\F_Serpinski_Carpet.txt"; break;
                case 3: filePath = @"..\..\L-systems\F_Serpinski_Arrowhead.txt"; break;
                case 4: filePath = @"..\..\L-systems\F_Hilbert_Curve.txt"; break;
                case 5: filePath = @"..\..\L-systems\F_Harter-Haythaway.txt"; break;
                case 6: filePath = @"..\..\L-systems\F_Gosper_Hex.txt"; break;
                case 7: filePath = @"..\..\L-systems\T_Bush_1.txt"; break;
                case 8: filePath = @"..\..\L-systems\T_Bush_2.txt"; break;
                case 9: filePath = @"..\..\L-systems\T_Bush_3.txt"; break;
                case 10: filePath = @"..\..\L-systems\Hexagonal_Mosaic.txt"; break;
                case 11: filePath = @"..\..\L-systems\T_Tree_Random.txt"; break;
            }
            LoadLSystemFromFile(filePath);
        }

        private void Form1_Resize(object sender, EventArgs e) => Invalidate();

        private void btnBack_Click(object sender, EventArgs e) => Close();
    }
}