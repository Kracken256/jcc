#!/usr/bin/env python3

import random
random.seed(246)   
import numpy
numpy.random.seed(246)
import uuid
import signal
import os
from PIL import Image, ImageTk
import tkinter as tk
import sys
import matplotlib.pyplot as plt
import networkx as nx
import yaml
import string
from hashlib import sha256

import warnings
warnings.filterwarnings("ignore")

known_labels = set()

class Node:
    def __init__(self, value):
        if value in known_labels:
            self.value = ''.join(random.choices(string.ascii_uppercase +
                                          string.digits, k=2)) + "_" + value
        else:
            self.value = value
        known_labels.add(value)

    def __str__(self):
        return f'{self.value}'

data_hash = ''

def visual_ast_as_graph(src_yaml: str, dst_png: str):
    # Load AST data from the YAML file
    with open(src_yaml, 'rb') as file:
        ast_data = yaml.safe_load(file)
        global data_hash

        file.seek(0)

        new_data_hash = sha256(file.read()).hexdigest()
        if new_data_hash == data_hash:
            return
        else:
            data_hash = new_data_hash
    
    print("Refreshing display")
    known_labels.clear()

    # Create a directed graph
    ast_graph = nx.DiGraph()

    def build_graph(data, parent=None):
        node_name = data['value']
        new_node = Node(node_name)
        ast_graph.add_node(new_node)
        if parent:
            ast_graph.add_edge(parent, new_node)
        if 'children' in data:
            for child in data['children']:
                build_graph(child, new_node)

    # Build the graph from AST data
    for key in ast_data:
        new_node = Node(key)
        ast_graph.add_node(new_node)
        build_graph(ast_data[key], new_node)

    # Draw the graph
    plot = plt.figure(figsize=(19.2, 10.3))

    # Force
    pos = nx.nx_pydot.graphviz_layout(ast_graph, prog='dot')

    # rectangle shape

    nx.draw(ast_graph, pos, with_labels=True, node_size=4000,
            node_color='skyblue',  arrows=True,  font_size=10, font_weight='bold')

    plt.title('Abstract Syntax Tree')
    # Save the graph as PNG
    plot.savefig(dst_png)


def rebuild_compiler():
    os.system(f"./build.sh >/dev/null")
    os.system(f"rm -f out.cpp {sys.argv[1]}; ./build/cdlc -o out.cpp {' '.join(sys.argv[2:])} > /dev/null")


def update_image():
    try:
        rebuild_compiler()
        visual_ast_as_graph(sys.argv[1], 'generated.png')
        img = Image.open('generated.png')  # Open the generated image
        # img = img.resize((1000*2, 2*156), Image.ANTIALIAS)  # Resize the image if needed

        # Convert the image for Tkinter
        tk_img = ImageTk.PhotoImage(img)

        # Update the label's image
        label.config(image=tk_img)
        label.image = tk_img  # Keep a reference to avoid garbage collection

        # Call this function again after 1000ms (1 second)
        root.after(400, update_image)
    except FileNotFoundError:
        print("Not ready to refresh yet")
        root.after(400, update_image)
    except:
        print("Error: ", sys.exc_info()[0])
        root.after(400, update_image)


def handler(event):
    print('caught ^C')
    sys.exit(0)


def check():
    root.after(500, check)  # time in ms.


if len(sys.argv) < 3:
    print("Usage: visualize-ast.py <yaml-file> [input files...]")
    sys.exit(1)

# Create the main window
root = tk.Tk()
root.title("CompilerDev LiveUpdate")

# Create a label to display the image
label = tk.Label(root)
label.pack()

# the or is a hack just because I've shoving all this in a lambda. setup before calling main loop
signal.signal(signal.SIGINT, lambda x, y: print(
    'terminal ^C') or handler(None))

# this let's the terminal ^C get sampled every so often
root.after(500, check)  # time in ms.

root.bind_all('<Control-c>', handler)

# Start updating the image
update_image()

# Run the Tkinter main loop
root.mainloop()
