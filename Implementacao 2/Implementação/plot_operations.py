import matplotlib.pyplot as plt
import pandas as pd

# Load the data from the CSV file
data = pd.read_csv('operations_count.csv')

# Create the bar plot
plt.figure(figsize=(12, 8))  # Slightly larger figure for better readability
plt.bar(data['Algorithm'], data['Operations'], color='skyblue')

# Add labels and title
plt.xlabel('Algorithm', fontsize=14)
plt.ylabel('Number of Operations', fontsize=14)
plt.title('Number of Operations for Each Search Algorithm', fontsize=16)

# Rotate x-axis labels if there are many algorithms
plt.xticks(rotation=45, ha='right', fontsize=12)

# Save the plot as a PNG file
plt.savefig('operations_plot.png', bbox_inches='tight')  # bbox_inches to prevent clipping of labels

# Display the plot
plt.show()
