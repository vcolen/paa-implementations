import matplotlib.pyplot as plt
import pandas as pd

# Carregar os dados do CSV
data = pd.read_csv('operations_count.csv')

# Plotar os dados
plt.figure(figsize=(10, 6))
plt.bar(data['Algorithm'], data['Operations'], color='skyblue')
plt.xlabel('Algorithm')
plt.ylabel('Number of Operations')
plt.title('Number of Operations for Each Search Algorithm')
plt.savefig('operations_plot.png')  # Salva o gráfico como imagem
plt.show()  # Exibe o gráfico
