"""
Gera relatorio_trab1.pdf a partir do conteudo do trabalho.
Uso: python gerar_relatorio.py
"""

from fpdf import FPDF
from fpdf.enums import XPos, YPos
import os

BASE = os.path.dirname(os.path.abspath(__file__))
GRAPH = os.path.join(BASE, "grafico_colisoes.png")

NL   = {"new_x": XPos.LMARGIN, "new_y": YPos.NEXT}
SAME = {"new_x": XPos.RIGHT,   "new_y": YPos.TOP}

OUTPUT_DATA = [
    (100,1),(200,3),(300,8),(400,15),(500,28),(600,39),(700,54),(800,68),
    (900,86),(1000,111),(1100,126),(1200,157),(1300,201),(1400,230),
    (1500,272),(1600,316),(1700,358),(1800,399),(1900,457),(2000,510),
    (2100,590),(2200,667),(2300,740),(2400,838),(2500,934),(2600,1039),
    (2700,1157),(2800,1272),(2900,1420),(3000,1571),(3100,1725),(3200,1884),
    (3300,2053),(3400,2264),(3500,2457),(3600,2655),(3700,2934),(3800,3215),
    (3900,3533),(4000,3915),(4096,4219),
]


class PDF(FPDF):
    def header(self):
        self.set_font("Helvetica", "I", 9)
        self.set_text_color(130, 130, 130)
        self.cell(0, 7, "INF1010 - Estruturas de Dados Avancadas | Trabalho 1 - Tabela Hash",
                  align="C", **NL)
        self.set_draw_color(200, 200, 200)
        self.set_line_width(0.3)
        self.line(self.l_margin, self.get_y(), self.w - self.r_margin, self.get_y())
        self.ln(3)

    def footer(self):
        self.set_y(-12)
        self.set_font("Helvetica", "I", 8)
        self.set_text_color(160, 160, 160)
        self.cell(0, 10, str(self.page_no()), align="C")

    def titulo_secao(self, txt):
        self.ln(5)
        self.set_font("Helvetica", "B", 12)
        self.set_text_color(20, 50, 110)
        self.cell(0, 7, txt, **NL)
        self.set_draw_color(20, 50, 110)
        self.set_line_width(0.4)
        self.line(self.l_margin, self.get_y(), self.w - self.r_margin, self.get_y())
        self.set_text_color(0, 0, 0)
        self.ln(3)

    def subtitulo(self, txt):
        self.set_font("Helvetica", "B", 10)
        self.set_text_color(40, 40, 40)
        self.cell(0, 6, txt, **NL)
        self.set_text_color(0, 0, 0)
        self.ln(1)

    def texto(self, txt):
        self.set_font("Helvetica", "", 10)
        self.multi_cell(0, 5.5, txt)
        self.ln(1)

    def mono(self, txt):
        self.set_font("Courier", "", 9)
        self.set_fill_color(248, 248, 248)
        self.multi_cell(0, 5, txt, fill=True)
        self.ln(1)

    def campo(self, label, valor):
        self.set_font("Helvetica", "B", 10)
        self.cell(32, 6, label + ":", **SAME)
        self.set_font("Helvetica", "", 10)
        self.cell(0, 6, valor, **NL)


pdf = PDF()
pdf.set_auto_page_break(auto=True, margin=15)
pdf.set_margins(20, 20, 20)

# --- Capa ---
pdf.add_page()
pdf.ln(18)
pdf.set_font("Helvetica", "B", 18)
pdf.set_text_color(20, 50, 110)
pdf.cell(0, 11, "Trabalho 1 - Tabela Hash com CPFs", align="C", **NL)
pdf.set_font("Helvetica", "", 12)
pdf.set_text_color(70, 70, 70)
pdf.cell(0, 8, "INF1010 - Estruturas de Dados Avancadas - 2026.1", align="C", **NL)
pdf.cell(0, 8, "PUC-Rio", align="C", **NL)
pdf.ln(10)
pdf.set_draw_color(20, 50, 110)
pdf.set_line_width(0.8)
pdf.line(60, pdf.get_y(), pdf.w - 60, pdf.get_y())
pdf.ln(10)
pdf.set_font("Helvetica", "", 11)
pdf.set_text_color(0, 0, 0)
pdf.cell(0, 7, "Julia Kymie Dias Okada  -  Matricula: 2421312", align="C", **NL)
pdf.cell(0, 7, "15 de maio de 2026", align="C", **NL)

# --- 1. Identificacao ---
pdf.add_page()
pdf.titulo_secao("1. Identificacao")
pdf.campo("Aluna",     "Julia Kymie Dias Okada")
pdf.campo("Matricula", "2421312")
pdf.campo("Disciplina","INF1010 - Estruturas de Dados Avancadas")
pdf.campo("Turma",     "3VC / 3WA - 2026.1")
pdf.campo("Entrega",   "15 de maio de 2026")

# --- 2. Objetivo ---
pdf.titulo_secao("2. Objetivo")
pdf.texto(
    "O trabalho pede pra guardar cerca de 4096 CPFs numa tabela hash de tamanho "
    "fixo, usando endereçamento aberto pra resolver colisoes. A tabela tem que ter "
    "uma folga de uns 20% em relacao ao numero de chaves. Alem disso, o programa "
    "precisa gerar dados pra montar um grafico mostrando quantas colisoes foram "
    "acumulando conforme as chaves sao inseridas (de 100 em 100 ate 4096), pra dar "
    "pra ver o comportamento da funcao hash na pratica."
)

# --- 3. Estrutura ---
pdf.titulo_secao("3. Estrutura do Programa")

pdf.subtitulo("hash_cpf.c")
itens = [
    ("inicializa()",  "preenche a tabela inteira com -1 (slot vazio)."),
    ("insere(cpf)",   "insere o CPF usando double hashing; retorna quantas "
                      "colisoes ocorreram antes de achar slot livre, ou -1 se "
                      "a tabela estiver cheia."),
    ("main()",        "le CPFsValidos.txt, insere os CPFs acumulando colisoes "
                      "e imprime uma linha CSV a cada 100 insercoes."),
]
for nome, desc in itens:
    pdf.set_font("Helvetica", "B", 10)
    pdf.cell(0, 6, f"  {nome}", **NL)
    pdf.set_font("Helvetica", "", 10)
    pdf.multi_cell(0, 5.5, f"    {desc}")
    pdf.ln(1)

pdf.subtitulo("grafico.py")
pdf.texto("Le o CSV e plota o grafico de colisoes acumuladas com matplotlib. Salva em grafico_colisoes.png.")

pdf.subtitulo("CPFsValidos.txt")
pdf.texto("Arquivo de entrada fornecido pelo professor com 4096 CPFs validos, um por linha.")

# --- 4. Solucao ---
pdf.titulo_secao("4. Solucao")

pdf.subtitulo("4.1 Tamanho da tabela")
pdf.texto(
    "Com 4096 chaves e folga de 20%, o minimo seria 4915. Foi escolhido N = 5003, "
    "que e o primeiro primo acima disso. Usar numero primo e importante pro double "
    "hashing funcionar direito, porque garante que a sondagem percorra todas as "
    "posicoes da tabela antes de repetir."
)
pdf.texto("Fator de carga com todas as chaves: 4096 / 5003 = 81,9%")

pdf.subtitulo("4.2 Funcao hash")
pdf.mono("h1(cpf) = cpf % 5003")
pdf.texto(
    "O CPF e tratado como long long. O modulo por primo espalha bem as chaves "
    "porque nao tem padrao de multiplo que criaria grupos de colisoes."
)

pdf.subtitulo("4.3 Tratamento de colisoes - Double Hashing")
pdf.texto(
    "Foi usado double hashing porque e o que gera menos agrupamento entre os tres "
    "metodos de endereçamento aberto:"
)
pdf.texto(
    "  - Linear: simples, mas forma blocos grandes conforme enche.\n"
    "  - Quadratico: resolve o agrupamento primario mas ainda tem o secundario.\n"
    "  - Double hashing: cada chave tem seu proprio passo, entao nao agrupa."
)
pdf.texto("A sondagem funciona assim:")
pdf.mono(
    "pos(i) = (h1(cpf) + i * h2(cpf)) % N\n"
    "h2(cpf) = 1 + (cpf % 4999)"
)
pdf.texto(
    "O 4999 tambem e primo e ligeiramente menor que N. Isso garante que o passo "
    "nunca seja zero e que mdc(h2, N) = 1, ou seja, a sondagem visita todas as "
    "N posicoes antes de repetir."
)

pdf.subtitulo("4.4 Como a insercao conta colisoes")
pdf.texto(
    "Na funcao insere(), o loop começa em i=0. Se a posicao inicial ja ta "
    "ocupada, i sobe pra 1, 2, etc. Quando acha um slot livre, retorna i, "
    "que e exatamente o numero de slots cheios visitados antes (as colisoes "
    "dessa insercao). O main acumula esses valores e imprime a cada 100 insercoes."
)

pdf.subtitulo("4.5 Saida do programa")
pdf.texto("Stderr:")
pdf.mono("CPFs lidos: 4096  tamanho da tabela: 5003")
pdf.texto("Stdout (CSV):")

csv_lines = ["chaves_inseridas,colisoes_acumuladas"] + [f"{k},{c}" for k, c in OUTPUT_DATA]
pdf.mono("\n".join(csv_lines))

# Grafico
pdf.subtitulo("4.6 Grafico")
pdf.texto(
    "O grafico mostra que ate umas 2000 chaves as colisoes crescem quase linear. "
    "Depois acelera bastante quando a tabela passa dos 60% de ocupacao. Com todas "
    "as 4096 chaves o total foi 4219 colisoes, razoavel pra um fator de carga de ~82%."
)
if os.path.exists(GRAPH):
    w = pdf.w - pdf.l_margin - pdf.r_margin
    pdf.image(GRAPH, x=pdf.l_margin, w=w)
    pdf.ln(2)
else:
    pdf.texto("[grafico_colisoes.png nao encontrado]")

# --- 5. Observacoes ---
pdf.titulo_secao("5. Observacoes e Conclusoes")

pdf.subtitulo("Como compilar e rodar")
pdf.mono(
    "gcc hash_cpf.c -o hash_cpf -O2\n"
    "\n"
    "# Windows:\n"
    ".\\hash_cpf.exe > resultados.csv\n"
    "\n"
    "# Linux:\n"
    "./hash_cpf > resultados.csv\n"
    "\n"
    "# grafico (pip install matplotlib se nao tiver):\n"
    "python grafico.py resultados.csv"
)

pdf.subtitulo("O que funciona")
pdf.texto(
    "  - Le os 4096 CPFs corretamente.\n"
    "  - Insere todos sem a tabela ficar cheia.\n"
    "  - Conta as colisoes certo.\n"
    "  - Gera CSV e grafico."
)

pdf.subtitulo("Limitacoes")
pdf.texto(
    "  - So faz insercao, nao tem busca nem remocao.\n"
    "  - Tabela com tamanho fixo, nao redimensiona.\n"
    "  - CPF duplicado no arquivo e ignorado silenciosamente."
)

pdf.subtitulo("Dificuldades")
pdf.texto(
    "Cometi um erro bobo no inicio: o fscanf tava com dois campos (\"%d %lld\"), "
    "entao ele consumia um token extra por iteracao e eu so lia 2048 CPFs em vez "
    "de 4096. Corrigi pra \"%lld\" simples."
)
pdf.texto(
    "Outro problema foi que o PowerShell salva o stdout em UTF-16 LE com BOM, e o "
    "Python nao conseguia ler o CSV. Resolvi passando encoding=\"utf-16\" no open() "
    "do grafico.py."
)

pdf.subtitulo("Conclusao")
pdf.texto(
    "O double hashing funcionou bem. O grafico confirma o que a teoria diz: "
    "colisoes crescem devagar no começo e aceleram quando a tabela enche. "
    "Escolher N primo foi essencial pra o passo do double hashing cobrir a "
    "tabela inteira."
)

out = os.path.join(BASE, "relatorio_trab1.pdf")
pdf.output(out)
print(f"PDF gerado: {out}")
