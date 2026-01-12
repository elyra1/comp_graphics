using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace Lab5
{
    public partial class MainForm : Form
    {
        private List<PointF> _points = new List<PointF>();
        private PointF? _draggedPoint = null;
        private int _draggedIndex = -1;
        private Bitmap _curveBitmap;

        public MainForm()
        {
            InitializeComponent();
            this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.OptimizedDoubleBuffer, true);
            pictureBox1.Paint += PictureBox1_Paint;
            pictureBox1.MouseDown += PictureBox1_MouseDown;
            pictureBox1.MouseMove += PictureBox1_MouseMove;
            pictureBox1.MouseUp += PictureBox1_MouseUp;
            this.Resize += Form_Resize;
        }

        private void PictureBox1_Paint(object sender, PaintEventArgs e)
        {
            if (_curveBitmap != null)
                e.Graphics.DrawImage(_curveBitmap, Point.Empty);

            DrawPointsAndHandles(e.Graphics);
        }

        private void DrawPointsAndHandles(Graphics g)
        {
            if (_points.Count == 0) return;

            g.SmoothingMode = SmoothingMode.AntiAlias;

            using (var anchorBrush = new SolidBrush(Color.Red))
            using (var controlBrush = new SolidBrush(Color.Green))
            using (var linePen = new Pen(Color.Gray, 1))
            {
                for (int i = 0; i < _points.Count - 1; i++)
                {
                    g.DrawLine(linePen, _points[i], _points[i + 1]);
                }

                for (int i = 0; i < _points.Count; i++)
                {
                    var pt = _points[i];
                    bool isAnchor = (i % 3 == 0);
                    var brush = isAnchor ? anchorBrush : controlBrush;
                    int radius = isAnchor ? 4 : 3;
                    g.FillEllipse(brush, pt.X - radius, pt.Y - radius, radius * 2, radius * 2);
                }
            }
        }

        private void RedrawCurve()
        {
            if (pictureBox1.Width <= 0 || pictureBox1.Height <= 0) return;

            _curveBitmap?.Dispose();
            _curveBitmap = new Bitmap(pictureBox1.Width, pictureBox1.Height);

            using (var g = Graphics.FromImage(_curveBitmap))
            {
                g.Clear(Color.White);
                g.SmoothingMode = SmoothingMode.None;

                for (int i = 0; i <= _points.Count - 4; i += 3)
                {
                    var p0 = _points[i];
                    var p1 = _points[i + 1];
                    var p2 = _points[i + 2];
                    var p3 = _points[i + 3];

                    using (var blackBrush = new SolidBrush(Color.Black))
                    {
                        for (double t = 0.0; t <= 1.0; t += 0.001)
                        {
                            double oneMinusT = 1.0 - t;
                            double x = p0.X * oneMinusT * oneMinusT * oneMinusT
                                     + 3 * p1.X * oneMinusT * oneMinusT * t
                                     + 3 * p2.X * oneMinusT * t * t
                                     + p3.X * t * t * t;

                            double y = p0.Y * oneMinusT * oneMinusT * oneMinusT
                                     + 3 * p1.Y * oneMinusT * oneMinusT * t
                                     + 3 * p2.Y * oneMinusT * t * t
                                     + p3.Y * t * t * t;

                            int ix = (int)Math.Max(0, Math.Min(_curveBitmap.Width - 1, x));
                            int iy = (int)Math.Max(0, Math.Min(_curveBitmap.Height - 1, y));
                            g.FillRectangle(blackBrush, ix, iy, 1, 1);
                        }
                    }
                }
            }

            pictureBox1.Invalidate();
        }

        private void PictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                var hitIndex = FindPointIndexAt(e.Location, 10);
                if (hitIndex >= 0)
                {
                    _draggedIndex = hitIndex;
                    _draggedPoint = _points[hitIndex];
                    return;
                }

                PointF newPoint = new PointF(e.X, e.Y);

                if (_points.Count < 4)
                {
                    _points.Add(newPoint);
                }
                else
                {
                    if ((_points.Count - 1) % 3 == 0)
                    {
                        PointF lastAnchorOriginal = _points[_points.Count - 1];
                        PointF prevControl = _points[_points.Count - 2];

                        PointF shiftedAnchor = new PointF(
                            (lastAnchorOriginal.X + prevControl.X) / 2f,
                            (lastAnchorOriginal.Y + prevControl.Y) / 2f
                        );

                        _points[_points.Count - 1] = shiftedAnchor;

                        _points.Add(lastAnchorOriginal);

                        _points.Add(newPoint);
                    }
                    else
                    {
                        _points.Add(newPoint);
                    }
                }

                RedrawCurve();
            }
            else if (e.Button == MouseButtons.Right)
            {
                var hitIndex = FindPointIndexAt(e.Location, 10);
                if (hitIndex >= 0 && hitIndex % 3 == 0)
                {
                    if (hitIndex == 0)
                    {
                        if (_points.Count <= 4)
                        {
                            _points.Clear();

                        }
                        else
                        {
                            _points.RemoveRange(0, 3);
                        }
                        
                    }
                    else if (hitIndex == _points.Count - 1) 
                    {
                        _points.RemoveAt(hitIndex);
                    }
                    else
                    {
                        if (hitIndex > 0)
                        {
                            _points.RemoveAt(hitIndex - 1); 
                            _points.RemoveAt(hitIndex - 1); 
                            _points.RemoveAt(hitIndex - 1); 
                        }
                    }
                }
                RedrawCurve();
            }


        }

        private void PictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (_draggedPoint.HasValue && e.Button == MouseButtons.Left)
            {
                _points[_draggedIndex] = new PointF(e.X, e.Y);
                pictureBox1.Invalidate();
            }
        }

        private void PictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            if (_draggedPoint.HasValue)
            {
                RedrawCurve();
                _draggedPoint = null;
                _draggedIndex = -1;
            }
        }

        private int FindPointIndexAt(PointF loc, float radius)
        {
            for (int i = 0; i < _points.Count; i++)
            {
                if (Distance(_points[i], loc) <= radius)
                    return i;
            }
            return -1;
        }

        private float Distance(PointF a, PointF b)
        {
            float dx = a.X - b.X;
            float dy = a.Y - b.Y;
            return (float)Math.Sqrt(dx * dx + dy * dy);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            _points.Clear();
            _curveBitmap?.Dispose();
            _curveBitmap = null;
            pictureBox1.Invalidate();
        }

        private void Form_Resize(object sender, EventArgs e)
        {
            RedrawCurve();
        }
    }
}