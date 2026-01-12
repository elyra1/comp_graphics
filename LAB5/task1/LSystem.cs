using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace Lab5
{
    public class LSystem
    {
        public string Axiom { get; private set; }
        public float Angle { get; private set; }
        public float InitialDirection { get; private set; }
        public int Iterations { get; private set; }
        private Dictionary<char, string> rules = new Dictionary<char, string>();
        public bool RandomBranching { get; private set; } = false;

        public static LSystem FromFile(string path)
        {
            var lines = File.ReadAllLines(path);
            if (lines.Length < 1) throw new Exception("Файл пуст.");

            var initialLine = lines[0].Split();
            if (initialLine.Length < 2) throw new Exception("Некорректный формат первой строки.");

            var axiom = initialLine[0];
            var angle = float.Parse(initialLine[1]);
            var initialDirection = initialLine.Length >= 3 ? float.Parse(initialLine[2]) : 0f;
            var iterations = initialLine.Length == 4 ? int.Parse(initialLine[3]) : 5;

            var lSystem = new LSystem(axiom, angle, initialDirection, iterations);

            lSystem.RandomBranching = lines.Any(line => line.Contains('@'));

            for (int i = 1; i < lines.Length; ++i)
            {
                if (string.IsNullOrWhiteSpace(lines[i])) continue;

                var index = lines[i].IndexOf("->");
                if (index == -1) throw new Exception($"Некорректное правило в строке {i + 1}: {lines[i]}");

                var symbol = lines[i][0];
                var result = lines[i].Substring(index + 2).Trim();

                lSystem.AddRule(symbol, result);
            }

            return lSystem;
        }

        public LSystem(string axiom, float angle, float initialDirection, int iterations = 5)
        {
            Axiom = axiom;
            Angle = angle;
            InitialDirection = initialDirection;
            Iterations = iterations;
        }

        public void AddRule(char symbol, string result) => rules[symbol] = result;

        public string Generate()
        {
            string current = Axiom;
            for (int i = 0; i < Iterations; ++i)
            {
                StringBuilder next = new StringBuilder();
                foreach (char c in current)
                    next.Append(rules.ContainsKey(c) ? rules[c] : c.ToString());
                current = next.ToString();
            }
            return current;
        }
    }
}