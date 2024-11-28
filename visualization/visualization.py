import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import imageio.v2 as imageio
import os
from tqdm import tqdm
import json
import sys

def create_heatmap_gif(data_list, gif_name="heatmap.gif", cmap="viridis", interval=0.5):
    """
    Create a GIF of 2D heatmap visualizations.

    Parameters:
    - data_list: List of 2D numpy arrays (heatmap data).
    - gif_name: Name of the output GIF file.
    - cmap: Colormap for the heatmaps.
    - interval: Time between frames in the GIF (seconds).
    """
    # Create a temporary directory for heatmap images
    temp_dir = "temp_heatmaps"
    os.makedirs(temp_dir, exist_ok=True)

    image_files = []

    # Generate heatmap images
    for i, data in tqdm(enumerate(data_list)):
        plt.figure(figsize=(6, 6))
        sns.heatmap(data, cmap=cmap, cbar=True, square=True)
        plt.title(f"Heatmap Frame {i+1}")
        file_path = os.path.join(temp_dir, f"heatmap_{i:05d}.png")
        plt.savefig(file_path)
        plt.close()
        image_files.append(file_path)

    # Create GIF using imageio
    images = [imageio.imread(img) for img in image_files]
    imageio.mimsave(gif_name, images, duration=interval)

    # Cleanup temporary files
    for img in image_files:
        os.remove(img)
    os.rmdir(temp_dir)

    print(f"GIF saved as {gif_name}")

def load_heatmap_data_from_file(file_path):
    """
    Load heatmap data from a JSON file.

    Parameters:
    - file_path: Path to the input JSON file.

    Returns:
    - A list of 2D numpy arrays.
    """
    try:
        with open(file_path, "r") as f:
            data = json.load(f)
        # Convert the nested lists into numpy arrays
        heatmap_data = [np.array(frame) for frame in data]
        return heatmap_data
    except Exception as e:
        print(f"Error loading JSON file: {e}")
        sys.exit(1)

# Main script
if __name__ == "__main__":
    # Check for input JSON file
    # if len(sys.argv) != 2:
    #     print("Usage: python script.py input.json")
    #     sys.exit(1)

    input_file = "../input.json"

    # Load heatmap data from input.json
    heatmap_data = load_heatmap_data_from_file(input_file)

    # Create GIF
    create_heatmap_gif(heatmap_data, gif_name="../example_heatmap.gif", cmap="plasma", interval=1)
