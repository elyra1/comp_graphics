import tkinter as tk
from tkinter import ttk, messagebox

class Vertex:
    def __init__(self, x, y, index, chain):
        self.x = x
        self.y = y
        self.index = index
        self.chain = chain  # 'upper' или 'lower'

def cross_product(o, a, b):
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x)

def triangulate_monotone_polygon(vertices):
    n = len(vertices)
    if n < 3:
        return []

    #находим левую и правую вершины
    l_idx = min(range(n), key=lambda i: vertices[i][0])
    r_idx = max(range(n), key=lambda i: vertices[i][0])

    #формируем первую цепочку слева направо
    path1 = []
    curr = l_idx
    while curr != r_idx:
        path1.append(curr)
        curr = (curr + 1) % n
    path1.append(r_idx)

    #формируем вторую цепочку справа налево
    path2 = []
    curr = l_idx
    while curr != r_idx:
        path2.append(curr)
        curr = (curr - 1) % n
    path2.append(r_idx)

    # Определяем, какой путь находится выше по координатам
    p1_y_sum = sum(vertices[i][1] for i in path1) / len(path1)
    p2_y_sum = sum(vertices[i][1] for i in path2) / len(path2)

    if p1_y_sum < p2_y_sum:
        upper_indices, lower_indices = path1, path2
    else:
        upper_indices, lower_indices = path2, path1

    chain_map = {}
    for idx in upper_indices:
        chain_map[idx] = 'upper'
    for idx in lower_indices:
        chain_map[idx] = 'lower'

    #сортируем вершины
    sorted_nodes = []
    for i, (x, y) in enumerate(vertices):
        sorted_nodes.append(Vertex(x, y, i, chain_map[i]))

    sorted_nodes.sort(key=lambda v: (v.x, v.y))

    #добавляем первые 2 вершины в стек
    stack = [sorted_nodes[0], sorted_nodes[1]]
    triangles = []

    for i in range(2, len(sorted_nodes)):
        v_i = sorted_nodes[i]
        v_top = stack[-1]

        if v_i.chain != v_top.chain:
            # Вершина на противоположной цепочке -> добавляем треугольники со всеми вершинами в стеке, очищаем стек
            while len(stack) > 1:
                v_s1 = stack.pop(0)
                v_s2 = stack[0]
                triangles.append((v_s1.index, v_s2.index, v_i.index))
            stack = [v_top, v_i]
        else:
            # Вершина на той же цепочке -> проверяем выпуклость и при возможности формируем треугольники
            v_last = stack.pop()
            while stack:
                v_top = stack[-1]
                cp = cross_product(v_top, v_last, v_i)
                
                # Для верхней цепи выпуклый (внутренний) поворот дает CP > 0
                # Для нижней цепи выпуклый (внутренний) поворот дает CP < 0
                is_convex = (v_i.chain == 'upper' and cp > 0) or (v_i.chain == 'lower' and cp < 0)

                if is_convex:
                    triangles.append((v_top.index, v_last.index, v_i.index))
                    v_last = stack.pop()
                else:
                    break
            stack.append(v_last)
            stack.append(v_i)

    return triangles

class TriangulationApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Триангуляция монотонного полигона")
        self.root.geometry("1000x700")
        
        self.vertices = []
        self.triangles = []
        self.colors = ["#FFB6C1", "#ADD8E6", "#90EE90", "#FFD700", "#DDA0DD", "#F0E68C", "#E0BBE4"]
        
        self.setup_ui()
        
    def setup_ui(self):
        control_frame = ttk.Frame(self.root, padding="10")
        control_frame.pack(side=tk.TOP, fill=tk.X)
        
        ttk.Button(control_frame, text="Триангулировать", command=self.triangulate).pack(side=tk.LEFT, padx=5)
        ttk.Button(control_frame, text="Очистить", command=self.clear).pack(side=tk.LEFT, padx=5)
        
        self.canvas = tk.Canvas(self.root, bg="white", highlightthickness=1, highlightbackground="gray")
        self.canvas.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        self.canvas.bind("<Button-1>", self.add_vertex)
        
        self.status_label = ttk.Label(self.root, text="Добавьте вершины кликом мыши (соблюдайте монотонность по X)")
        self.status_label.pack(side=tk.BOTTOM, pady=5)
        
    def add_vertex(self, event):
        self.vertices.append((event.x, event.y))
        self.triangles = []
        self.draw()
        
    def clear(self):
        self.vertices = []
        self.triangles = []
        self.canvas.delete("all")
        
    def triangulate(self):
        if len(self.vertices) < 3:
            messagebox.showwarning("Внимание", "Минимум 3 точки")
            return
        try:
            self.triangles = triangulate_monotone_polygon(self.vertices)
            self.draw()
        except Exception as e:
            messagebox.showerror("Ошибка", f"Ошибка: {str(e)}")
    
    def draw(self):
        self.canvas.delete("all")
        
        if self.triangles:
            for i, tri in enumerate(self.triangles):
                color = self.colors[i % len(self.colors)]
                p = []
                for idx in tri:
                    p.extend([self.vertices[idx][0], self.vertices[idx][1]])
                self.canvas.create_polygon(p, fill=color, outline="black")
        
        if len(self.vertices) > 1:
            for i in range(len(self.vertices)):
                p1 = self.vertices[i]
                p2 = self.vertices[(i + 1) % len(self.vertices)]
                self.canvas.create_line(p1[0], p1[1], p2[0], p2[1], fill="blue", width=2)
        
        for i, (x, y) in enumerate(self.vertices):
            self.canvas.create_oval(x-4, y-4, x+4, y+4, fill="red")
            self.canvas.create_text(x+10, y-10, text=str(i))

if __name__ == "__main__":
    root = tk.Tk()
    app = TriangulationApp(root)
    root.mainloop()