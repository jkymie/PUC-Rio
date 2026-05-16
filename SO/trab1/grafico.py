"""
Gera o gráfico Chaves Inseridas x Colisões Acumuladas
a partir do CSV produzido por hash_cpf.c

Uso:
    python grafico.py resultados.csv
ou (se já há um arquivo padrão):
    python grafico.py
"""

import sys
import csv
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

filename = sys.argv[1] if len(sys.argv) > 1 else "resultados.csv"

keys       = []
collisions = []

with open(filename, newline="", encoding="utf-16") as f:
    reader = csv.DictReader(f)
    for row in reader:
        keys.append(int(row["chaves_inseridas"]))
        collisions.append(int(row["colisoes_acumuladas"]))

fig, ax = plt.subplots(figsize=(10, 6))

ax.plot(keys, collisions, color="steelblue", linewidth=2, marker="o",
        markersize=3, label="Colisões acumuladas")

# linha de referência: y = x (colisão em toda inserção)
ax.plot([0, keys[-1]], [0, keys[-1]], color="tomato", linestyle="--",
        linewidth=1, label="Referência: 1 colisão/chave")

ax.set_title("Tabela Hash (Double Hashing) – Chaves vs Colisões\n"
             f"N = 5003, h₁(k) = k mod 5003, h₂(k) = 1 + (k mod 4999)",
             fontsize=13)
ax.set_xlabel("Chaves inseridas", fontsize=12)
ax.set_ylabel("Colisões acumuladas", fontsize=12)
ax.legend(fontsize=11)
ax.xaxis.set_major_locator(ticker.MultipleLocator(400))
ax.yaxis.set_major_locator(ticker.MultipleLocator(500))
ax.grid(True, linestyle="--", alpha=0.5)

plt.tight_layout()
plt.savefig("grafico_colisoes.png", dpi=150)
plt.show()
print("Gráfico salvo em grafico_colisoes.png")
