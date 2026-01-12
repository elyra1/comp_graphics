using System;
using System.Collections.Generic;
using System.Drawing;

namespace Lab5
{
    public class LSystemDrawer
    {
        private Graphics graphics;
        private float angleStep;
        private Random random;
        private float randomAngleRange;

        public LSystemDrawer(Graphics graphics, float angleStep)
        {
            this.graphics = graphics;
            this.angleStep = angleStep;
            random = new Random();
            randomAngleRange = 0;
        }

        public void Draw(string commands, PointF start, float scale, float initialDirection, float randomAngleRange = 0)
        {
            this.randomAngleRange = randomAngleRange;

            List<DrawCommand> drawCommands = GenerateDrawCommands(commands, initialDirection);

            ScaleAndDrawCommands(drawCommands, start, scale);
        }

        private struct DrawCommand
        {
            public PointF Start;
            public PointF End;
            public float Thickness;
            public Color Color;
            public int Depth;

            public DrawCommand(PointF start, PointF end, float thickness, Color color, int depth)
            {
                Start = start;
                End = end;
                Thickness = thickness;
                Color = color;
                Depth = depth;
            }
        }

        private List<DrawCommand> GenerateDrawCommands(string commands, float initialDirection)
        {
            List<DrawCommand> drawCommands = new List<DrawCommand>();
            Stack<Tuple<PointF, float, int>> positionStack = new Stack<Tuple<PointF, float, int>>();

            var currentPosition = new PointF(0, 0);
            var currentDirection = initialDirection;
            int currentDepth = 0;

            float maxStepLength = 15f;
            float minStepLength = 1f;
            float maxThickness = 20f;
            float minThickness = 1f;
            int maxDepth = 12; 

            foreach (var command in commands)
            {
                int clampedDepth = Math.Min(currentDepth, maxDepth);

                float currentStepLength = Interpolation(clampedDepth, 0, maxDepth, maxStepLength, minStepLength);
                float currentThickness = Interpolation(clampedDepth, 0, maxDepth, maxThickness, minThickness);

                switch (command)
                {
                    case 'F':
                        var newPosition = MoveForward(currentPosition, currentDirection, currentStepLength);
                        Color lineColor = GetColor(clampedDepth, maxDepth);

                        drawCommands.Add(new DrawCommand(currentPosition, newPosition, currentThickness, lineColor, currentDepth));

                        currentPosition = newPosition;
                        break;

                    case '+':
                        currentDirection += angleStep + GetRandomAngle(randomAngleRange);
                        break;

                    case '-':
                        currentDirection -= angleStep + GetRandomAngle(randomAngleRange);
                        break;

                    case '[':
                        positionStack.Push(Tuple.Create(currentPosition, currentDirection, currentDepth));
                        break;

                    case ']':
                        var savedState = positionStack.Pop();
                        currentPosition = savedState.Item1;
                        currentDirection = savedState.Item2;
                        currentDepth = savedState.Item3;
                        break;

                    case '@':
                        currentDepth++;
                        break;
                }
            }

            return drawCommands;
        }

        private void ScaleAndDrawCommands(List<DrawCommand> commands, PointF targetStart, float padding)
        {
            if (commands.Count == 0) return;

            float minX = commands[0].Start.X;
            float maxX = commands[0].Start.X;
            float minY = commands[0].Start.Y;
            float maxY = commands[0].Start.Y;

            foreach (var cmd in commands)
            {
                if (cmd.Start.X < minX) minX = cmd.Start.X;
                if (cmd.Start.X > maxX) maxX = cmd.Start.X;
                if (cmd.Start.Y < minY) minY = cmd.Start.Y;
                if (cmd.Start.Y > maxY) maxY = cmd.Start.Y;
                if (cmd.End.X < minX) minX = cmd.End.X;
                if (cmd.End.X > maxX) maxX = cmd.End.X;
                if (cmd.End.Y < minY) minY = cmd.End.Y;
                if (cmd.End.Y > maxY) maxY = cmd.End.Y;
            }

            float width = maxX - minX;
            float height = maxY - minY;
            float centerX = (maxX + minX) / 2;
            float centerY = (maxY + minY) / 2;

            int screenWidth = (int)graphics.VisibleClipBounds.Width;
            int screenHeight = (int)graphics.VisibleClipBounds.Height;

            float scaleX = (screenWidth - padding * 2) / width;
            float scaleY = (screenHeight - padding * 2) / height;
            float scale = Math.Min(scaleX, scaleY);

            float offsetX = screenWidth / 2 - centerX * scale;
            float offsetY = screenHeight / 2 - centerY * scale;

            foreach (var cmd in commands)
            {
                PointF scaledStart = new PointF(
                    cmd.Start.X * scale + offsetX,
                    cmd.Start.Y * scale + offsetY
                );
                PointF scaledEnd = new PointF(
                    cmd.End.X * scale + offsetX,
                    cmd.End.Y * scale + offsetY
                );

                float scaledThickness = Math.Max(1f, cmd.Thickness * scale / 10f);

                using (Pen pen = new Pen(cmd.Color, scaledThickness))
                {
                    graphics.DrawLine(pen, scaledStart, scaledEnd);
                }
            }
        }

        private PointF MoveForward(PointF point, float direction, float distance)
        {
            var rad = direction * Math.PI / 180;
            var newX = point.X + (float)(Math.Cos(rad) * distance);
            var newY = point.Y + (float)(Math.Sin(rad) * distance);
            return new PointF(newX, newY);
        }

        private float GetRandomAngle(float range) => (float)(random.NextDouble() * range * 2 - range);

        private Color GetColor(int depth, int maxDepth)
        {
            if (randomAngleRange == 0) return Color.Black;

            Color rootColor = Color.SaddleBrown;
            Color leafColor = Color.LimeGreen;

            float ratio = (float)depth / maxDepth;
            ratio = Math.Max(0, Math.Min(1, ratio));

            int r = (int)(rootColor.R * (1 - ratio) + leafColor.R * ratio);
            int g = (int)(rootColor.G * (1 - ratio) + leafColor.G * ratio);
            int b = (int)(rootColor.B * (1 - ratio) + leafColor.B * ratio);

            return Color.FromArgb(r, g, b);
        }

        private float Interpolation(float x, float min1, float max1, float min2, float max2)
        {
            x = Math.Max(min1, Math.Min(max1, x));
            return (x - min1) / (max1 - min1) * (max2 - min2) + min2;
        }
    }
}