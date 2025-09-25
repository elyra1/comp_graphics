using System;
using System.Drawing;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace Lab2_Task2
{
    public partial class Form1 : Form
    {
        private Bitmap bitmap;


        public Form1()
        {
            InitializeComponent();

            try
            {
                Image image = Image.FromFile("C:\\Users\\mirkh\\OneDrive\\Рабочий стол\\8.png");
                bitmap = new Bitmap(image);

                pictureBoxTrue.Image = bitmap;

                pictureBoxRed.Image = getBitmapByChannel(bitmap, 'R');
                pictureBoxGreen.Image = getBitmapByChannel(bitmap, 'G');
                pictureBoxBlue.Image = getBitmapByChannel(bitmap, 'B');

                DisplayHistogram(chartRed, GetHistogram(bitmap, 'R'));
                DisplayHistogram(chartGreen, GetHistogram(bitmap, 'G'));
                DisplayHistogram(chartBlue, GetHistogram(bitmap, 'B'));
            }
            catch (Exception ex)
            {
                MessageBox.Show("Ошибка при загрузке изображения: " + ex.Message);
            }
        }

        private Bitmap getBitmapByChannel(Bitmap source, char channel)
        {
            Bitmap channelImage = new Bitmap(source.Width, source.Height);

            for (int y = 0; y < source.Height; y++)
            {
                for (int x = 0; x < source.Width; x++)
                {
                    Color pixelColor = source.GetPixel(x, y);
                    Color newColor;

                    switch (channel)
                    {
                        case 'R':
                            newColor = Color.FromArgb(255, pixelColor.R, 0, 0);
                            break;
                        case 'G':
                            newColor = Color.FromArgb(255, 0, pixelColor.G, 0);
                            break;
                        case 'B':
                            newColor = Color.FromArgb(255, 0, 0, pixelColor.B);
                            break;
                        default:
                            newColor = Color.Black;
                            break;
                    }

                    channelImage.SetPixel(x, y, newColor);
                }
            }

            return channelImage;
        }

        private int[] GetHistogram(Bitmap bitmap, char channel)
        {
            int[] histogram = new int[256];

            for (int y = 0; y < bitmap.Height; y++)
            {
                for (int x = 0; x < bitmap.Width; x++)
                {
                    Color pixelColor = bitmap.GetPixel(x, y);
                    int value = 0;

                    switch (channel)
                    {
                        case 'R': value = pixelColor.R; break;
                        case 'G': value = pixelColor.G; break;
                        case 'B': value = pixelColor.B; break;
                    }

                    histogram[value]++;
                }
            }

            return histogram;
        }

        private void DisplayHistogram(Chart chart, int[] histogram)
        {
            chart.Series[0].Points.Clear();
            for (int i = 0; i < histogram.Length; i++)
            {
                chart.Series[0].Points.AddXY(i, histogram[i]);
            }
        }

      
    }
}
