import pandas as pd
import matplotlib.pyplot as plt
import os

# Leitura dos arquivos CSV
felzenszwalb_file = "felzenszwalb_results.csv"
boykov_file = "boykov_results.csv"

try:
    # Verificar se os arquivos existem
    if not os.path.exists(felzenszwalb_file) or not os.path.exists(boykov_file):
        raise FileNotFoundError("Os arquivos CSV de resultados não foram encontrados.")

    # Carregar os dados
    felzenszwalb_data = pd.read_csv(felzenszwalb_file)
    boykov_data = pd.read_csv(boykov_file)

    # Preparar os conjuntos de dados por número de nós
    node_sizes = felzenszwalb_data["NumNodes"].unique()
    for node_size in node_sizes:
        # Filtrar dados para o mesmo número de nós
        felzenszwalb_subset = felzenszwalb_data[felzenszwalb_data["NumNodes"] == node_size]
        boykov_subset = boykov_data[boykov_data["NumNodes"] == node_size]

        # Validar correspondência dos dados
        if len(felzenszwalb_subset) != len(boykov_subset):
            raise ValueError(f"Número de entradas não corresponde para {node_size} nós.")

        # Configurar os dados para o gráfico
        densities = felzenszwalb_subset["NumEdges"] // node_size  # Calcula a densidade (arestas por nó)
        felzenszwalb_times = felzenszwalb_subset["Time(ms)"]
        boykov_times = boykov_subset["Time(ms)"]

        # Criar o gráfico de barras
        plt.figure(figsize=(10, 6))
        bar_width = 0.35  # Largura das barras
        index = range(len(densities))

        plt.bar(index, felzenszwalb_times, bar_width, label="Felzenszwalb & Huttenlocher")
        plt.bar([i + bar_width for i in index], boykov_times, bar_width, label="Boykov & Funka-Lea")

        # Configurações do gráfico
        plt.title(f"Comparação de Tempo - {node_size} Nós")
        plt.xlabel("Densidade (arestas por nó)")
        plt.ylabel("Tempo (ms)")
        plt.xticks([i + bar_width / 2 for i in index], densities)
        plt.legend()
        plt.grid(True, axis='y', linestyle='--', alpha=0.7)

        # Salvar gráfico
        output_file = f"comparison_{node_size}_nodes.png"
        plt.savefig(output_file)
        print(f"Gráfico salvo como '{output_file}'")
        plt.close()

except FileNotFoundError as e:
    print(f"Erro: {e}")
except Exception as e:
    print(f"Um erro inesperado ocorreu: {e}")