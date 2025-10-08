using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;

namespace LAB3 {
    /// <summary>
    /// Рисование отрезков.
    /// </summary>
    public partial class MainForm : Form {
        private const int ScaleFactor = 5;
        /// <summary>
        /// Начальная точка.
        /// </summary>
        private Point? startPoint = null;
        /// <summary>
        /// Конечная точка.
        /// </summary>
        private Point? endPoint = null;
        /// <summary>
        /// Флаг используемого алгоритма.
        /// </summary>
        private bool useBresenham = true;

        /// <summary>
        /// Список, содержащий информацию о нарисованных линиях, который хранит начальные и конечные точки линий, а также цвет.
        /// </summary>
        private List<LineInfo> lines = new List<LineInfo>();

        /// <summary>
        /// Информация о линии.
        /// </summary>
        private class LineInfo {
            /// <summary>
            /// Стартовая точка линии.
            /// </summary>
            public Point Start { get; set; }
            /// <summary>
            /// Конечная точка линии.
            /// </summary>
            public Point End { get; set; }
            /// <summary>
            /// Флаг используемого алгоритма.
            /// </summary>
            public bool UseBresenham { get; set; }
        }

        /// <summary>
        /// Инициализирует новый экземпляр класса <see cref="MainForm"/>.
        /// </summary>
        /// <param name="menu">Главная форма меню, используемая для навигации.</param>
        public MainForm( ) {
            InitializeComponent();
        }

        /// <summary>
        /// Использует заданный алгоритм рисования линии (Брезенхема или Ву).
        /// </summary>
        /// <param name="sender">Объект, вызвавший событие.</param>
        /// <param name="e">Аргументы события.</param>
        private void PictureBox1_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.ScaleTransform(5.0f, 5.0f);

            // Рисуем все сохраненные линии
            foreach (var line in lines)
            {
                if (line.UseBresenham)
                    BresenhamLine(e.Graphics, line.Start, line.End, Color.Black);
                else
                    WuLine(e.Graphics, line.Start, line.End, Color.Black);
            }

            // Рисуем "живую" линию-превью, если заданы обе точки
            if (startPoint.HasValue && endPoint.HasValue)
            {
                // Координаты для превью нужно тоже масштабировать
                Point correctedStart = new Point(startPoint.Value.X / ScaleFactor, startPoint.Value.Y / ScaleFactor);
                Point correctedEnd = new Point(endPoint.Value.X / ScaleFactor, endPoint.Value.Y / ScaleFactor);

                if (useBresenham)
                    BresenhamLine(e.Graphics, correctedStart, correctedEnd, Color.Blue); // Можно другим цветом для наглядности
                else
                    WuLine(e.Graphics, correctedStart, correctedEnd, Color.Blue);
            }
        }

        /// <summary>
        /// При первом щелчке устанавливает начальную точку линии,  при втором щелчке устанавливает конечную точку и инициирует перерисовку PictureBox.
        /// </summary>
        /// <param name="sender">Объект, вызвавший событие.</param>
        /// <param name="e">Аргументы события.</param>
        private void PictureBox1_MouseClick(object sender, MouseEventArgs e)
        {
            if (startPoint == null)
            {
                startPoint = e.Location;
            }
            else
            {
                endPoint = e.Location; // Фиксируем конечную точку

                // Добавляем линию в список для постоянного хранения
                lines.Add(new LineInfo
                {
                    Start = new Point(startPoint.Value.X / ScaleFactor, startPoint.Value.Y / ScaleFactor),
                    End = new Point(endPoint.Value.X / ScaleFactor, endPoint.Value.Y / ScaleFactor),
                    UseBresenham = useBresenham
                });

                // Сбрасываем точки для рисования следующей линии
                startPoint = null;
                endPoint = null;

                pictureBox1.Invalidate();
            }
        }

        /// <summary>
        /// Устанавливает флаг использования алгоритма Брезенхема, если радиокнопка Брезенхема выбрана.
        /// </summary>
        /// <param name="sender">Объект, вызвавший событие.</param>
        /// <param name="e">Аргументы события.</param>
        private void RadioButtonBresenham_CheckedChanged(object sender, EventArgs e)
        {
            if (sender is RadioButton rb && rb.Checked)
            {
                useBresenham = (rb == radioButtonBresenham);
            }
        }
        /// <summary>
        /// Очищает все нарисованные линии.
        /// </summary>
        /// <param name="sender">Объект, вызвавший событие.</param>
        /// <param name="e">Аргументы события.</param>
        private void ButtonReset_Click(object sender, EventArgs e) {
            startPoint = null;
            endPoint = null;
            lines.Clear();
            pictureBox1.Invalidate();
        }

        /// <summary>
        /// Удаляет последнюю нарисованную линию из списка и перерисовывает PictureBox.
        /// Если линий нет, действие игнорируется.
        /// </summary>
        /// <param name="sender">Объект, вызвавший событие.</param>
        /// <param name="e">Аргументы события.</param>
        private void ButtonUndo_Click(object sender, EventArgs e) {
            if (lines.Count > 0) {
                lines.RemoveAt(lines.Count - 1);
                pictureBox1.Invalidate();
            }
        }

        /// <summary>
        /// Рисует линию между двумя точками с использованием целочисленного алгоритма Брезенхема.
        /// </summary>
        /// <param name="g">Объект, на котором производится рисование.</param>
        /// <param name="p0">Начальная точка линии.</param>
        /// <param name="p1">Конечная точка линии.</param>
        /// <param name="color">Цвет линии.</param>
        private void BresenhamLine(Graphics g, Point p0, Point p1, Color color)
        {

            PlotLine(g,p0.X, p0.Y, p1.X, p1.Y, color);
        }

        private static void PlotLineLow(Graphics g, int x0, int y0, int x1, int y1, Color color)
        {
            int dx = x1 - x0;
            int dy = y1 - y0;
            int yi = 1;

            if (dy < 0)
            {
                yi = -1;
                dy = -dy;
            }

            int D = (2 * dy) - dx;
            int y = y0;
            SolidBrush brush = new SolidBrush(color);

            for (int x = x0; x <= x1; x++)
            {
                g.FillRectangle(brush, x, y, 1, 1);

                if (D > 0)
                {
                    y = y + yi;
                    D = D + (2 * (dy - dx));
                }
                else
                {
                    D = D + 2 * dy;
                }
            }
        }

        /// <summary>
        /// Отрисовывает линию для крутых склонов (когда |dy| >= |dx|).
        /// Итерация происходит по оси Y.
        /// </summary>
        private  void PlotLineHigh(Graphics g, int x0, int y0, int x1, int y1, Color color)
        {
            int dx = x1 - x0;
            int dy = y1 - y0;
            int xi = 1;

            if (dx < 0)
            {
                xi = -1;
                dx = -dx;
            }

            int D = (2 * dx) - dy;
            int x = x0;
            SolidBrush brush = new SolidBrush(color);
            for (int y = y0; y <= y1; y++)
            {
                g.FillRectangle(brush, x, y, 1, 1);

                if (D > 0)
                {
                    x = x + xi;
                    D = D + (2 * (dx - dy));
                }
                else
                {
                    D = D + 2 * dx;
                }
            }
        }

        /// <summary>
        /// Основной метод для отрисовки линии между двумя точками (x0, y0) и (x1, y1).
        /// Автоматически определяет, какой вспомогательный метод (PlotLineLow или PlotLineHigh) использовать.
        /// </summary>
        /// <param name="x0">X-координата начальной точки.</param>
        /// <param name="y0">Y-координата начальной точки.</param>
        /// <param name="x1">X-координата конечной точки.</param>
        /// <param name="y1">Y-координата конечной точки.</param>
        /// <param name="plot">Действие, которое нужно выполнить для каждой точки линии (например, отрисовка пикселя).</param>
        public  void PlotLine(Graphics g, int x0, int y0, int x1, int y1, Color color)
        {
            if (Math.Abs(y1 - y0) < Math.Abs(x1 - x0))
            {
                // Пологий склон
                if (x0 > x1)
                {
                    PlotLineLow(g, x1, y1, x0, y0, color);
                }
                else
                {
                    PlotLineLow(g, x0, y0, x1, y1, color);
                }
            }
            else
            {
                // Крутой склон
                if (y0 > y1)
                {
                    PlotLineHigh(g, x1, y1, x0, y0, color);
                }
                else
                {
                    PlotLineHigh(g, x0, y0, x1, y1, color);
                }
            }
        }

        /// <summary>
        /// Рисует линию со сглаживанием между двумя точками с использованием алгоритма Ву.
        /// </summary>
        /// <param name="g">Объект, на котором производится рисование.</param>
        /// <param name="p0">Начальная точка линии.</param>
        /// <param name="p1">Конечная точка линии.</param>
        /// <param name="color">Цвет линии.</param>
        private void WuLine(Graphics g, Point p0, Point p1, Color color)
        {
            int x0 = p0.X, y0 = p0.Y;
            int x1 = p1.X, y1 = p1.Y;

            bool steep = Math.Abs(y1 - y0) > Math.Abs(x1 - x0);
            if (steep) { Swap(ref x0, ref y0); Swap(ref x1, ref y1); }
            if (x0 > x1) { Swap(ref x0, ref x1); Swap(ref y0, ref y1); }

            int dx = x1 - x0;
            if (dx == 0)
            {
                int startY = Math.Min(y0, y1);
                int endY = Math.Max(y0, y1);
                using (var brush = new SolidBrush(color))
                {
                    for (int y_temp = startY; y_temp <= endY; y_temp++)
                    {
                        if (steep) g.FillRectangle(brush, y_temp, x0, 1, 1);
                        else g.FillRectangle(brush, x0, y_temp, 1, 1);
                    }
                }
                return;
            }

            float grad = (float)(y1 - y0) / dx;
            float y = y0;

            using (var brush = new SolidBrush(color))
            {
                for (int x = x0; x <= x1; x++)
                {
                    int alpha1 = (int)(255 * (1 - Frac(y)));
                    int alpha2 = (int)(255 * Frac(y));

                    if (steep)
                    {
                        brush.Color = Color.FromArgb(alpha1, color);
                        g.FillRectangle(brush, (int)y, x, 1, 1);
                        brush.Color = Color.FromArgb(alpha2, color);
                        g.FillRectangle(brush, (int)y + 1, x, 1, 1);
                    }
                    else
                    {
                        brush.Color = Color.FromArgb(alpha1, color);
                        g.FillRectangle(brush, x, (int)y, 1, 1);
                        brush.Color = Color.FromArgb(alpha2, color);
                        g.FillRectangle(brush, x, (int)y + 1, 1, 1);
                    }

                    y += grad; // Переходим к следующей y-координате
                }
            }
        }

        /// <summary>
        /// Меняет местами значения двух целочисленных переменных.
        /// </summary>
        /// <param name="a">Первая переменная.</param>
        /// <param name="b">Вторая переменная.</param>
        private void Swap(ref int a, ref int b) => (b, a) = (a, b);

        /// <summary>
        /// Возвращает дробную часть числа.
        /// </summary>
        /// <param name="x">Число.</param>
        /// <returns>Дробная часть числа.</returns>
        private float Frac(float x) => x - (float)Math.Floor(x);

        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (startPoint.HasValue && !lines.Any(l => l.Start == startPoint.Value))
            {
                endPoint = e.Location; // Обновляем конечную точку
                pictureBox1.Invalidate(); // Запускаем перерисовку
            }
        }

        private void Form2_Resize(object sender, EventArgs e)
        {
            panel1.Left = (this.ClientSize.Width - panel1.Width) / 2;
        }
    }
}
