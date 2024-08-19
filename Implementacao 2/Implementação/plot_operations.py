import matplotlib.pyplot as plt
import pandas as pd

# Load the data from the CSV file
data = pd.read_csv('operations_count.csv')

# Extract the scenarios (Beginning, Middle, End, NotFound)
scenarios = ['Beginning', 'Middle', 'End', 'NotFound']

# Separate the algorithms into two groups based on their operation scales
low_scale_algorithms = ['BinarySearch']
high_scale_algorithms = ['SimpleLinearSearch', 'SentinelLinearSearch']

# 1. Plot for low-scale operations (BinarySearch)
plt.figure(figsize=(12, 8))
for algorithm in low_scale_algorithms:
    scenario_data = [data[data['Algorithm'] == f"{algorithm}_{scenario}"]['Operations'].values[0] for scenario in scenarios]
    plt.plot(scenarios, scenario_data, marker='o', label=algorithm)

plt.xlabel('Scenario', fontsize=14)
plt.ylabel('Number of Operations', fontsize=14)
plt.title('Comparison of BinarySearch Across Scenarios', fontsize=16)
plt.legend(title='Algorithm')
plt.grid(True)
plt.savefig('binary_search_comparison_plot.png', bbox_inches='tight')
plt.show()

# 2. Plot for high-scale operations (SimpleLinearSearch and SentinelLinearSearch)
plt.figure(figsize=(12, 8))
for algorithm in high_scale_algorithms:
    scenario_data = [data[data['Algorithm'] == f"{algorithm}_{scenario}"]['Operations'].values[0] for scenario in scenarios]
    plt.plot(scenarios, scenario_data, marker='o', label=algorithm)

plt.xlabel('Scenario', fontsize=14)
plt.ylabel('Number of Operations', fontsize=14)
plt.title('Comparison of Linear Search Algorithms Across Scenarios', fontsize=16)
plt.legend(title='Algorithm')
plt.grid(True)
plt.savefig('linear_search_comparison_plot.png', bbox_inches='tight')
plt.show()
