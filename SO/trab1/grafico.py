import sys
import matplotlib.pyplot as plt

arq = sys.argv[1] if len(sys.argv) > 1 else "resultados.csv"

chaves = []
colisoes = []

with open(arq, encoding="utf-8") as f:
    f.readline()
    for linha in f:
        linha = linha.strip()
        if linha:
            k, c = linha.split(",")
            chaves.append(int(k))
            colisoes.append(int(c))

plt.figure(figsize=(10, 6))
plt.plot(chaves, colisoes, "b-o", markersize=3, label="colisoes acumuladas")
plt.plot([0, chaves[-1]], [0, chaves[-1]], "r--", linewidth=1, label="ref: 1 col/chave")

plt.title("Tabela Hash (double hashing) - Chaves vs Colisoes\nN=5003  h1=k%5003  h2=1+(k%4999)")
plt.xlabel("chaves inseridas")
plt.ylabel("colisoes acumuladas")
plt.legend()
plt.grid(True, alpha=0.4)
plt.tight_layout()
plt.savefig("grafico_colisoes.png", dpi=150)
plt.show()
print("salvo em grafico_colisoes.png")
