// ColorConverter.cs
using System;
using System.Drawing;

namespace LAB2
{
    public static class ColorConverter
    {
        public static (double H, double S, double V) RgbToHsv(Color rgb)
        {
            double r = rgb.R / 255.0;
            double g = rgb.G / 255.0;
            double b = rgb.B / 255.0;

            double max = Math.Max(r, Math.Max(g, b));
            double min = Math.Min(r, Math.Min(g, b));
            double delta = max - min;

            double hue = 0;
            if (delta != 0)
            {
                if (max == r)
                    hue = (g - b) / delta;
                else if (max == g)
                    hue = (b - r) / delta + 2;
                else // max == b
                    hue = (r - g) / delta + 4;
                hue *= 60;
                if (hue < 0)
                    hue += 360;
            }

            double saturation = (max == 0) ? 0 : delta / max;
            double value = max;

            return (hue, saturation, value);
        }

        public static Color HsvToRgb(double hue, double saturation, double value)
        {
            hue = hue % 360;
            if (hue < 0) hue += 360;

            int hi = Convert.ToInt32(Math.Floor(hue / 60)) % 6;
            double f = hue / 60 - Math.Floor(hue / 60);

            value = value * 255;
            int v = Convert.ToInt32(value);
            int p = Convert.ToInt32(value * (1 - saturation));
            int q = Convert.ToInt32(value * (1 - f * saturation));
            int t = Convert.ToInt32(value * (1 - (1 - f) * saturation));

            switch (hi)
            {
                case 0: return Color.FromArgb(255, v, t, p);
                case 1: return Color.FromArgb(255, q, v, p);
                case 2: return Color.FromArgb(255, p, v, t);
                case 3: return Color.FromArgb(255, p, q, v);
                case 4: return Color.FromArgb(255, t, p, v);
                default: return Color.FromArgb(255, v, p, q);
            }
        }
    }
}