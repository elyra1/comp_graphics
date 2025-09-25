// Form3.cs
using System;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace LAB2
{
    public partial class MainForm : Form
    {
        private Bitmap _originalBitmap;
        private Bitmap _processedBitmap;

        public MainForm()
        {
            InitializeComponent();
            UpdateTextValues();
        }

        private void button_loadFile_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    _originalBitmap = new Bitmap(openFileDialog1.FileName);
                    pictureBox1.Image = _originalBitmap;

                    // Активация элементов управления
                    button_saveFile.Enabled = true;
                    trackBar_Hue.Enabled = true;
                    trackBar_Saturation.Enabled = true;
                    trackBar_Value.Enabled = true;

                    // При загрузке нового изображения сбрасываем ползунки и обновляем картинку
                    ResetTrackBars();
                    UpdateImage();
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Не удалось открыть файл: " + ex.Message, "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void button_saveFile_Click(object sender, EventArgs e)
        {
            if (pictureBox1.Image != null && saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                pictureBox1.Image.Save(saveFileDialog1.FileName);
            }
        }

        private void button_reset_Click(object sender, EventArgs e)
        {
            ResetTrackBars();
            UpdateImage();
        }

        private void trackBar_Scroll(object sender, EventArgs e)
        {
            UpdateImage();
        }

        private void trackBar_ValueChanged(object sender, EventArgs e)
        {
            UpdateTextValues();
        }

        /// <summary>
        /// Главный метод для обработки изображения
        /// </summary>
        private void UpdateImage()
        {
            if (_originalBitmap == null) return;

            // Клонируем исходное изображение, чтобы не изменять его
            _processedBitmap = (Bitmap)_originalBitmap.Clone();

            // Получаем смещения из ползунков
            float hueShift = trackBar_Hue.Value; // -180 to 180
            float saturationShift = trackBar_Saturation.Value / 100f; // -1.0 to 1.0
            float valueShift = trackBar_Value.Value / 100f; // -1.0 to 1.0

            // Используем LockBits для быстрой обработки
            Rectangle rect = new Rectangle(0, 0, _processedBitmap.Width, _processedBitmap.Height);
            BitmapData bmpData = _processedBitmap.LockBits(rect, ImageLockMode.ReadWrite, _processedBitmap.PixelFormat);

            IntPtr ptr = bmpData.Scan0;
            int bytes = Math.Abs(bmpData.Stride) * _processedBitmap.Height;
            byte[] rgbValues = new byte[bytes];

            Marshal.Copy(ptr, rgbValues, 0, bytes);

            int bytesPerPixel = Image.GetPixelFormatSize(_processedBitmap.PixelFormat) / 8;

            for (int i = 0; i < rgbValues.Length; i += bytesPerPixel)
            {
                byte b = rgbValues[i];
                byte g = rgbValues[i + 1];
                byte r = rgbValues[i + 2];

                var hsv = ColorConverter.RgbToHsv(Color.FromArgb(r, g, b));

                // Применяем изменения
                double newHue = (hsv.H + hueShift);
                double newSaturation = Math.Max(0, Math.Min(1, hsv.S + saturationShift));
                double newValue = Math.Max(0, Math.Min(1, hsv.V + valueShift));

                Color newRgb = ColorConverter.HsvToRgb(newHue, newSaturation, newValue);

                rgbValues[i] = newRgb.B;
                rgbValues[i + 1] = newRgb.G;
                rgbValues[i + 2] = newRgb.R;
            }

            Marshal.Copy(rgbValues, 0, ptr, bytes);
            _processedBitmap.UnlockBits(bmpData);

            pictureBox1.Image = _processedBitmap;
        }

        private void UpdateTextValues()
        {
            textBox_Hue.Text = trackBar_Hue.Value.ToString();
            textBox_Saturation.Text = trackBar_Saturation.Value.ToString();
            textBox_Value.Text = trackBar_Value.Value.ToString();
        }

        private void ResetTrackBars()
        {
            trackBar_Hue.Value = 0;
            trackBar_Saturation.Value = 0;
            trackBar_Value.Value = 0;
        }
    }
}